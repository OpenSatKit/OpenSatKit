/******************************************************************************
 ** \file sbn_subs.c
 **
 **      Copyright (c) 2004-2006, United States government as represented by the
 **      administrator of the National Aeronautics Space Administration.
 **      All rights reserved. This software(cFE) was created at NASA's Goddard
 **      Space Flight Center pursuant to government contracts.
 **
 **      This software may be used only pursuant to a United States government
 **      sponsored project and the United States government may not be charged
 **      for use thereof.
 **
 ** Purpose:
 **      This file contains source code for the Software Bus Network Application.
 **
 ** Authors:   J. Wilmot/GSFC Code582
 **            R. McGraw/SSI
 **            E. Timmons/GSFC Code587
 **            C. Knight/ARC Code TI
 */

#include "sbn_app.h"
#include <string.h>
#include <arpa/inet.h>
#include "cfe_msgids.h"
#include "sbn_pack.h"

// TODO: instead of using void * for the buffer for SBN messages, use
// a struct that has the SBN header in packed bytes.

/**
 * Informs the software bus to send this application all subscription requests.
 */
void SBN_SendSubsRequests(void)
{
    CFE_SB_CmdHdr_t     SBCmdMsg;

    /* Turn on SB subscription reporting */
    CFE_SB_InitMsg(&SBCmdMsg, CFE_SB_CMD_MID, sizeof(CFE_SB_CmdHdr_t),
        TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &SBCmdMsg,
        CFE_SB_ENABLE_SUB_REPORTING_CC);
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &SBCmdMsg);

    /* Request a list of previous subscriptions from SB */
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &SBCmdMsg, CFE_SB_SEND_PREV_SUBS_CC);
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &SBCmdMsg);
}/* end SBN_SendSubsRequests */

/**
 * \brief Sends a local subscription over the wire to a peer.
 *
 * @param[in] SubType Whether this is a subscription or unsubscription.
 * @param[in] MsgID The CCSDS message ID being (un)subscribed.
 * @param[in] QoS The CCSDS quality of service being (un)subscribed.
 * @param[in] Peer The Peer interface
 */
static void SendLocalSubToPeer(int SubType, CFE_SB_MsgId_t MsgID,
    CFE_SB_Qos_t QoS, SBN_PeerInterface_t *Peer)
{
    uint8 Buf[SBN_PACKED_SUB_SZ];
    Pack_t Pack;
    Pack_Init(&Pack, &Buf, SBN_PACKED_SUB_SZ, 0);
    Pack_Data(&Pack, SBN_IDENT, SBN_IDENT_LEN);
    Pack_UInt16(&Pack, 1);

    Pack_MsgID(&Pack, MsgID);
    Pack_Data(&Pack, &QoS, sizeof(QoS)); /* 2 uint8's */

    SBN_SendNetMsg(SubType, Pack.BufUsed, Buf, Peer);
}/* end SendLocalSubToPeer */

/**
 * \brief Sends all local subscriptions over the wire to a peer.
 *
 * @param[in] Peer The peer interface.
 */
void SBN_SendLocalSubsToPeer(SBN_PeerInterface_t *Peer)
{
    uint8 Buf[SBN_PACKED_SUB_SZ];
    Pack_t Pack;
    Pack_Init(&Pack, &Buf, SBN_PACKED_SUB_SZ, 0);
    Pack_Data(&Pack, SBN_IDENT, SBN_IDENT_LEN);
    Pack_UInt16(&Pack, SBN.SubCnt);

    int i = 0;
    for(i = 0; i < SBN.SubCnt; i++)
    {
        Pack_MsgID(&Pack, SBN.Subs[i].MsgID);
        /* 2 uint8's */
        Pack_Data(&Pack, &SBN.Subs[i].QoS, sizeof(SBN.Subs[i].QoS));
    }/* end for */

    SBN_SendNetMsg(SBN_SUB_MSG, Pack.BufUsed, Buf, Peer);
}/* end SBN_SendLocalSubsToPeer */

/**
 * Utility to find the subscription index (SBN.Subs)
 * that is subscribed to the CCSDS message ID.
 *
 * @param[out] IdxPtr The subscription index found.
 * @param[in] MsgID The CCSDS message ID of the subscription being sought.
 * @return TRUE if found.
 */
static int IsMsgIDSub(int *IdxPtr, CFE_SB_MsgId_t MsgID)
{
    int     i = 0;

    for(i = 0; i < SBN.SubCnt; i++)
    {
        if(SBN.Subs[i].MsgID == MsgID)
        {
            if (IdxPtr)
            {
                *IdxPtr = i;
            }/* end if */

            return TRUE;
        }/* end if */
    }/* end for */

    return FALSE;
}/* end IsMsgIDSub */

/**
 * \brief Is this peer subscribed to this message ID? If so, what is the index
 *        of the subscription?
 *
 * @param[out] SubIdxPtr The pointer to the subscription index value.
 * @param[in] MsgID The CCSDS message ID of the subscription being sought.
 * @param[in] Peer The peer interface.
 *
 * @return TRUE if found.
 */
static int IsPeerSubMsgID(int *SubIdxPtr, CFE_SB_MsgId_t MsgID,
    SBN_PeerInterface_t *Peer)
{
    int     i = 0;

    for(i = 0; i < Peer->SubCnt; i++)
    {
        if(Peer->Subs[i].MsgID == MsgID)
        {
            *SubIdxPtr = i;
            return TRUE;
        }/* end if */
    }/* end for */

    return FALSE;

}/* end IsPeerSubMsgID */

/**
 * \brief I have seen a local subscription, send it on to peers if this is the
 * first instance of a subscription for this message ID.
 *
 * @param[in] MsgID The CCSDS Message ID of the local subscription.
 * @param[in] QoS The CCSDS quality of service of the local subscription.
 */
static void ProcessLocalSub(CFE_SB_MsgId_t MsgID, CFE_SB_Qos_t QoS)
{
    /* don't subscribe to event messages */
    if(MsgID == CFE_EVS_EVENT_MSG_MID) return;

    if(SBN.SubCnt >= SBN_MAX_SUBS_PER_PEER)
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
                "local subscription ignored for MsgID 0x%04X, max (%d) met",
                ntohs(MsgID), SBN_MAX_SUBS_PER_PEER);
        return;
    }/* end if */

    int SubIdx = 0;

    /* if there is already an entry for this msg id,just incr InUseCtr */
    if(IsMsgIDSub(&SubIdx, MsgID))
    {
        SBN.Subs[SubIdx].InUseCtr++;
        /* does not send to peers, as they already know */
        return;
    }/* end if */

    /* log new entry into Subs array */
    SBN.Subs[SBN.SubCnt].InUseCtr = 1;
    SBN.Subs[SBN.SubCnt].MsgID = MsgID;
    SBN.Subs[SBN.SubCnt].QoS = QoS;
    SBN.SubCnt++;

    int NetIdx = 0, PeerIdx = 0;
    for(NetIdx = 0; NetIdx < SBN.NetCnt; NetIdx++)
    {
        SBN_NetInterface_t *Net = &SBN.Nets[NetIdx];
        for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
        {
            SBN_PeerInterface_t *Peer = &Net->Peers[PeerIdx];

            SendLocalSubToPeer(SBN_SUB_MSG, MsgID, QoS, Peer);
        }/* end for */
    }/* end for */
}/* end ProcessLocalSub */

/**
 * \brief I have seen a local unsubscription, send it on to peers if this is the
 * last instance of a subscription for this message ID.
 *
 * @param[in] MsgID The CCSDS Message ID of the local unsubscription.
 * @param[in] QoS The CCSDS quality of service of the local unsubscription.
 */
static void ProcessLocalUnsub(CFE_SB_MsgId_t MsgID)
{
    int SubIdx;
    /* find idx of matching subscription */
    if(!IsMsgIDSub(&SubIdx, MsgID))
    {
        return;
    }/* end if */

    SBN.Subs[SubIdx].InUseCtr--;

    /* do not modify the array and tell peers
    ** until the # of local subscriptions = 0
    */
    if(SBN.Subs[SubIdx].InUseCtr > 0)
    {
        return;
    }/* end if */

    /* remove sub from array for and
    ** shift all subscriptions in higher elements to fill the gap
    ** note that the Subs[] array has one extra element to allow for an
    ** unsub from a full table.
    */
    for(; SubIdx < SBN.SubCnt; SubIdx++)
    {
        memcpy(&SBN.Subs[SubIdx], &SBN.Subs[SubIdx + 1],
            sizeof(SBN_Subs_t));
    }/* end for */

    SBN.SubCnt--;

    /* send unsubscription to all peers if peer state is heartbeating and */
    /* only if no more local subs (InUseCtr = 0)  */
    int NetIdx = 0, PeerIdx = 0;
    for(NetIdx = 0; NetIdx < SBN.NetCnt; NetIdx++)
    {
        SBN_NetInterface_t *Net = &SBN.Nets[NetIdx];
        for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
        {
            SBN_PeerInterface_t *Peer = &Net->Peers[PeerIdx];

            SendLocalSubToPeer(SBN_UNSUB_MSG, SBN.Subs[PeerIdx].MsgID,
                SBN.Subs[PeerIdx].QoS, Peer);
        }/* end for */
    }/* end for */
}/* end ProcessLocalUnsub */

/**
 * \brief Check the local pipe for subscription messages. Send them on to peers
 *        if there are any (new)subscriptions.
 * @return TRUE if subscriptions received.
 */
int32 SBN_CheckSubscriptionPipe(void)
{
    int32 RecvSub = FALSE;
    CFE_SB_MsgPtr_t SBMsgPtr;
    CFE_SB_SubRprtMsg_t *SubRprtMsgPtr;

    while(CFE_SB_RcvMsg(&SBMsgPtr, SBN.SubPipe, CFE_SB_POLL) == CFE_SUCCESS)
    {
        SubRprtMsgPtr = (CFE_SB_SubRprtMsg_t *)SBMsgPtr;

        switch(CFE_SB_GetMsgId(SBMsgPtr))
        {
            case CFE_SB_ONESUB_TLM_MID:
                switch(SubRprtMsgPtr->Payload.SubType)
                {
                    case CFE_SB_SUBSCRIPTION:
                        ProcessLocalSub(SubRprtMsgPtr->Payload.MsgId,
                            SubRprtMsgPtr->Payload.Qos);
                        RecvSub = TRUE;
                        break;
                    case CFE_SB_UNSUBSCRIPTION:
                        ProcessLocalUnsub(SubRprtMsgPtr->Payload.MsgId);
                        RecvSub = TRUE;
                        break;
                    default:
                        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
                            "unexpected subscription type (%d) in "
                            "SBN_CheckSubscriptionPipe",
                            SubRprtMsgPtr->Payload.SubType);
                }/* end switch */
                break;

            case CFE_SB_ALLSUBS_TLM_MID:
                SBN_ProcessAllSubscriptions((CFE_SB_PrevSubMsg_t *) SBMsgPtr);
                RecvSub = TRUE;
                break;

            default:
                CFE_EVS_SendEvent(SBN_MSG_EID, CFE_EVS_ERROR,
                        "unexpected message id (0x%04X) on SBN.SubPipe",
                        ntohs(CFE_SB_GetMsgId(SBMsgPtr)));
        }/* end switch */
    }/* end while */

    return RecvSub;
}/* end SBN_CheckSubscriptionPipe */

static void AddSub(SBN_PeerInterface_t *Peer, CFE_SB_MsgId_t MsgID,
    CFE_SB_Qos_t QoS)
{   
    int idx = 0, FirstOpenSlot = Peer->SubCnt;
    uint32 Status = CFE_SUCCESS;
    
    /* if msg id already in the list, ignore */
    if(IsPeerSubMsgID(&idx, MsgID, Peer))
    {   
        return;
    }/* end if */

    if(Peer->SubCnt >= SBN_MAX_SUBS_PER_PEER)
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
            "cannot process subscription from ProcessorID %d, max (%d) met",
            Peer->ProcessorID, SBN_MAX_SUBS_PER_PEER);
        return;
    }/* end if */
    
    /* SubscribeLocal suppresses the subscription report */
    Status = CFE_SB_SubscribeLocal(MsgID, Peer->Pipe, SBN_DEFAULT_MSG_LIM);
    if(Status != CFE_SUCCESS)
    {   
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
            "Unable to subscribe to MID 0x%04X", htons(MsgID));
        return;
    }/* end if */
    
    /* log the subscription in the peer table */ 
    Peer->Subs[FirstOpenSlot].MsgID = MsgID;
    Peer->Subs[FirstOpenSlot].QoS = QoS;
    
    Peer->SubCnt++;
}/* end AddSub */

static void ProcessSubFromPeer(SBN_PeerInterface_t *Peer, CFE_SB_MsgId_t MsgID,
    CFE_SB_Qos_t QoS)
{
    if(OS_MutSemTake(SBN.RemapMutex) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_REMAP_EID, CFE_EVS_ERROR, "unable to take mutex");
        return;
    }/* end if */

    boolean RemappedFlag = FALSE;

    /** If there's a filter, ignore the sub request.  */
    int idx = 0;
    for(idx = 0; !RemappedFlag && idx < SBN.RemapTbl->EntryCnt; idx++)
    {
        if(SBN.RemapTbl->Entries[idx].ProcessorID == Peer->ProcessorID
            && SBN.RemapTbl->Entries[idx].FromMID == MsgID
            && SBN.RemapTbl->Entries[idx].ToMID == 0x0000)
        {
            RemappedFlag = TRUE;
        }/* end if */
    }/* end for */

    /*****
     * If there's a remap that would generate that
     * MID, I need to subscribe locally to the "from". Usually this will
     * only be one "from" but it's possible that multiple "from"s map to
     * the same "to".
     */
    for(idx = 0; idx < SBN.RemapTbl->EntryCnt; idx++)
    {
        if(SBN.RemapTbl->Entries[idx].ProcessorID
                == Peer->ProcessorID
            && SBN.RemapTbl->Entries[idx].ToMID == MsgID)
        {
            AddSub(Peer, SBN.RemapTbl->Entries[idx].FromMID, QoS);
            RemappedFlag = TRUE;
        }/* end if */
    }/* end for */

    /* if there's no remap ID's, subscribe to the original MID. */
    if(!RemappedFlag)
    {
        AddSub(Peer, MsgID, QoS);
    }/* end if */

    if(OS_MutSemGive(SBN.RemapMutex) != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_REMAP_EID, CFE_EVS_ERROR,
            "unable to give mutex");
    }/* end if */
}/* ProcessSubFromPeer */

/**
 * \brief Process a subscription message from a peer.
 *
 * @param[in] PeerIdx The peer index (in SBN.Peer)
 * @param[in] Msg The subscription SBN message.
 */
void SBN_ProcessSubsFromPeer(SBN_PeerInterface_t *Peer, void *Msg)
{
    Unpack_t Unpack;
    char VersionHash[SBN_IDENT_LEN];

    Unpack_Init(&Unpack, Msg, CFE_SB_MAX_SB_MSG_SIZE);

    Unpack_Data(&Unpack, VersionHash, SBN_IDENT_LEN);

    if(strncmp(VersionHash, SBN_IDENT, SBN_IDENT_LEN))
    {
        CFE_EVS_SendEvent(SBN_PROTO_EID, CFE_EVS_ERROR,
            "version number mismatch with peer CpuID %d",
            Peer->ProcessorID);
    }

    uint16 SubCnt;
    Unpack_UInt16(&Unpack, &SubCnt);

    int SubIdx = 0;
    for(SubIdx = 0; SubIdx < SubCnt; SubIdx++)
    {
        CFE_SB_MsgId_t MsgID;
        Unpack_MsgID(&Unpack, &MsgID);
        CFE_SB_Qos_t QoS;
        Unpack_Data(&Unpack, &QoS, sizeof(QoS));

        ProcessSubFromPeer(Peer, MsgID, QoS);
    }/* end for */
}/* SBN_ProcessSubsFromPeer */

static void ProcessUnsubFromPeer(SBN_PeerInterface_t *Peer,
    CFE_SB_MsgId_t MsgID)
{
    int i = 0, idx = 0;
    uint32 Status = CFE_SUCCESS;

    if(IsPeerSubMsgID(&idx, MsgID, Peer))
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_INFORMATION,
            "%s:Cannot process unsubscription from ProcessorID %d,"
            "msg 0x%04X not found",
            CFE_CPU_NAME, Peer->ProcessorID, htons(MsgID));
        return;
    }/* end if */

    /* remove sub from array for that peer and
    ** shift all subscriptions in higher elements to fill the gap
    ** note that the Subs[] array has one extra element to allow for an
    ** unsub from a full table.
    */
    for(i = idx; i < Peer->SubCnt; i++)
    {
        memcpy(&Peer->Subs[i],
            &Peer->Subs[i + 1],
            sizeof(SBN_Subs_t));
    }/* end for */

    /* decrement sub cnt */
    Peer->SubCnt--;

    /* unsubscribe to the msg id on the peer pipe */
    Status = CFE_SB_UnsubscribeLocal(MsgID, Peer->Pipe);
    if(Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
            "Unable to unsubscribe from MID 0x%04X", htons(MsgID));
        return;
    }/* end if */
}/* end ProcessUnsubFromPeer */

/**
 * \brief Process an unsubscription message from a peer.
 *
 * @param[in] PeerIdx The peer index (in SBN.Peer)
 * @param[in] Msg The unsubscription SBN message.
 */
void SBN_ProcessUnsubsFromPeer(SBN_PeerInterface_t *Peer, void *Msg)
{
    Unpack_t Unpack;

    Unpack_Init(&Unpack, Msg, CFE_SB_MAX_SB_MSG_SIZE);

    char VersionHash[SBN_IDENT_LEN];

    Unpack_Data(&Unpack, VersionHash, SBN_IDENT_LEN);

    if(strncmp(VersionHash, SBN_IDENT, SBN_IDENT_LEN))
    {
        CFE_EVS_SendEvent(SBN_PROTO_EID, CFE_EVS_ERROR,
            "version number mismatch with peer CpuID %d",
            Peer->ProcessorID);
    }

    uint16 SubCnt;
    Unpack_UInt16(&Unpack, &SubCnt);

    int SubIdx = 0;
    for(SubIdx = 0; SubIdx < SubCnt; SubIdx++)
    {
        CFE_SB_MsgId_t MsgID;
        Unpack_MsgID(&Unpack, &MsgID);
        CFE_SB_Qos_t QoS;
        Unpack_Data(&Unpack, &QoS, sizeof(QoS));

        int idx, RemappedFlag = 0;

        if(OS_MutSemTake(SBN.RemapMutex) != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_REMAP_EID, CFE_EVS_ERROR,
                "unable to take mutex");
            continue;
        }/* end if */

        /** If there's a filter, ignore this unsub. */
        for(idx = 0; RemappedFlag == 0 && idx < SBN.RemapTbl->EntryCnt; idx++)
        {   
            if(SBN.RemapTbl->Entries[idx].ProcessorID
                    == Peer->ProcessorID
                && SBN.RemapTbl->Entries[idx].FromMID == MsgID
                && SBN.RemapTbl->Entries[idx].ToMID == 0x0000)
            {   
                RemappedFlag = 1;
            }/* end if */
        }/* end for */

        /*****
         * If there's a remap that would generate that
         * MID, I need to unsubscribe locally to the "from". Usually this will
         * only be one "from" but it's possible that multiple "from"s map to
         * the same "to".
         */
        for(idx = 0; idx < SBN.RemapTbl->EntryCnt; idx++)
        {   
            if(SBN.RemapTbl->Entries[idx].ProcessorID
                    == Peer->ProcessorID
                && SBN.RemapTbl->Entries[idx].ToMID == MsgID)
            {   
                /* Unsubscribe from each "ToMID" match. */
                ProcessUnsubFromPeer(Peer,
                    SBN.RemapTbl->Entries[idx].FromMID);

                RemappedFlag = 1;
            }/* end if */
        }/* end for */

        /* if there's no remap ID's, unsubscribe from the original MID. */
        if(!RemappedFlag)
        {
            ProcessUnsubFromPeer(Peer, MsgID);
        }/* end if */

        if(OS_MutSemGive(SBN.RemapMutex) != OS_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_REMAP_EID, CFE_EVS_ERROR,
                "unable to give mutex");
        }/* end if */
    }/* end for */
}/* end SBN_ProcessUnsubFromPeer */

/**
 * When SBN starts, it queries for all existing subscriptions. This method
 * processes those subscriptions.
 *
 * @param[in] Ptr SB message pointer.
 */
void SBN_ProcessAllSubscriptions(CFE_SB_PrevSubMsg_t *Ptr)
{
    int i = 0;

    if(Ptr->Payload.Entries > CFE_SB_SUB_ENTRIES_PER_PKT)
    {
        CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
            "entries value %d in SB PrevSubMsg exceeds max %d, aborting",
            (int)Ptr->Payload.Entries,
            CFE_SB_SUB_ENTRIES_PER_PKT);
        return;
    }/* end if */

    for(i = 0; i < Ptr->Payload.Entries; i++)
    {
        ProcessLocalSub(Ptr->Payload.Entry[i].MsgId, Ptr->Payload.Entry[i].Qos);
    }/* end for */
}/* end SBN_ProcessAllSubscriptions */

/**
 * Removes all subscriptions (unsubscribe from the local SB )
 * for the specified peer, particularly when the peer connection has been lost.
 *
 * @param[in] PeerIdx The peer index (into SBN.Peer) to clear.
 */
void SBN_RemoveAllSubsFromPeer(SBN_PeerInterface_t *Peer)
{
    int i = 0;
    uint32 Status = CFE_SUCCESS;

    for(i = 0; i < Peer->SubCnt; i++)
    {
        Status = CFE_SB_UnsubscribeLocal(Peer->Subs[i].MsgID,
            Peer->Pipe);
        if(Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_ERROR,
                "unable to unsubscribe from message id 0x%04X",
                htons(Peer->Subs[i].MsgID));
        }/* end if */
    }/* end for */

    CFE_EVS_SendEvent(SBN_SUB_EID, CFE_EVS_INFORMATION,
        "unsubscribed %d message id's from ProcessorID %d",
        (int)Peer->SubCnt,
        Peer->ProcessorID);

    Peer->SubCnt = 0;
}/* end SBN_RemoveAllSubsFromPeer */
