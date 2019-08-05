/* 
** Purpose: Manage Packet Table that defines which packets will be sent from the
**          software bus to a socket.
**
** Notes:
**   1. This is much simpler than a typical flight TO app. If a packet is defined
**      in the table then it will be subscribed to and every packet received will 
**      be output over the socket. No filter algorithms are available.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Include Files:
*/

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "app_cfg.h"
#include "pktmgr.h"
#include "network_includes.h"

/*
** Global File Data
*/

static PKTMGR_Class*  PktMgr = NULL;
static PKTTBL_Pkt     UnusedTblPkt = {PKTTBL_UNUSED_MSG_ID, {0, 0}, 0};

/*
** Local Function Prototypes
*/

static void DestructorCallback(void);
static void FlushTlmPipe(void);
static int  StreamIdPktIdx(uint16 StreamId);
static int  UnusedTblPktIdx(void);

/******************************************************************************
** Function: PKTMGR_Constructor
**
*/
void PKTMGR_Constructor(PKTMGR_Class*  PktMgrPtr, char* PipeName, uint16 PipeDepth)
{

   int i;

   PktMgr = PktMgrPtr;

   PktMgr->DownlinkOn   = FALSE;
   PktMgr->SuppressSend = TRUE;
   PktMgr->TlmSockId    = 0;
   strncpy(PktMgr->TlmDestIp, "000.000.000.000", PKTMGR_IP_STR_LEN);

   CFE_PSP_MemSet((void*)&(PktMgr->Tbl), 0, sizeof(PKTTBL_Tbl));
   for (i=0; i < PKTTBL_MAX_PKT_CNT; i++) {

      PktMgr->Tbl.Pkt[i].StreamId = PKTTBL_UNUSED_MSG_ID;

   } /* end pkt loop */

   CFE_SB_CreatePipe(&(PktMgr->TlmPipe), PipeDepth, PipeName);

   OS_TaskInstallDeleteHandler((void *)(&DestructorCallback)); /* Call when application terminates */

} /* End PKTMGR_Constructor() */


/******************************************************************************
** Function: PKTMGR_GetTblPtr
**
*/
const PKTTBL_Tbl* PKTMGR_GetTblPtr()
{

   return &(PktMgr->Tbl);

} /* End PKTMGR_GetTblPtr() */


/******************************************************************************
** Function:  PKTMGR_ResetStatus
**
*/
void PKTMGR_ResetStatus(void)
{

   /* Nothing to do */

} /* End PKTMGR_ResetStatus() */


/******************************************************************************
** Function: PKTMGR_OutputTelemetry
**
*/
void PKTMGR_OutputTelemetry(void)
{

   static struct sockaddr_in SocketAddr;
   int                       SocketStatus;
   int32                     SbStatus;
   uint16                    PktLen;
   CFE_SB_Msg_t              *PktPtr;

   CFE_PSP_MemSet((void*)&SocketAddr, 0, sizeof(SocketAddr));
   SocketAddr.sin_family      = AF_INET;
   SocketAddr.sin_addr.s_addr = inet_addr(PktMgr->TlmDestIp);
   SocketAddr.sin_port        = htons(KIT_TO_TLM_PORT);
   SocketStatus = 0;

   /*
   ** CFE_SB_RcvMsg returns CFE_SUCCESS when it gets a packet, otherwise
   ** no packet was received
   */
   do
   {
      SbStatus = CFE_SB_RcvMsg(&PktPtr, PktMgr->TlmPipe, CFE_SB_POLL);

      if ( (SbStatus == CFE_SUCCESS) && (PktMgr->SuppressSend == FALSE) ) {
          
         PktLen = CFE_SB_GetTotalMsgLength(PktPtr);

         if(PktMgr->DownlinkOn) {
               
            SocketStatus = sendto(PktMgr->TlmSockId, (char *)PktPtr, PktLen, 0,
                                    (struct sockaddr *) &SocketAddr, sizeof(SocketAddr) );
          
         } /* End if downlink enabled */
          
         if (SocketStatus < 0) {
             
            CFE_EVS_SendEvent(PKTMGR_SOCKET_SEND_ERR_EID,CFE_EVS_ERROR,
                              "Error sending packet on socket %s, port %d, errno %d. Tlm output suppressed\n",
                              PktMgr->TlmDestIp, KIT_TO_TLM_PORT, errno);
            PktMgr->SuppressSend = TRUE;
         }

       } /* End if SB received msg and output enabled */

    } while(SbStatus == CFE_SUCCESS);

} /* End of PKTMGR_OutputTelemetry() */


/******************************************************************************
** Function: PKTMGR_LoadTbl
**
*/
boolean PKTMGR_LoadTbl(PKTTBL_Tbl* NewTbl)
{

   int      i, PktCnt = 0, FailedSubscription = 0;
   int32    Status;
   boolean  RetStatus = TRUE;

   CFE_SB_MsgPtr_t MsgPtr = NULL;

   PKTMGR_RemoveAllPktsCmd(NULL, MsgPtr);  /* Parameter is unused so OK to be NULL */

   CFE_PSP_MemCpy(&(PktMgr->Tbl), NewTbl, sizeof(PKTTBL_Tbl));

   for (i=0; i < PKTTBL_MAX_PKT_CNT; i++) {

      if(PktMgr->Tbl.Pkt[i].StreamId != PKTTBL_UNUSED_MSG_ID) {
         
         PktCnt++;
         Status = CFE_SB_SubscribeEx(PktMgr->Tbl.Pkt[i].StreamId,
                                     PktMgr->TlmPipe,PktMgr->Tbl.Pkt[i].Qos,
                                     PktMgr->Tbl.Pkt[i].BufLim);

         if(Status != CFE_SUCCESS) {
            
            FailedSubscription++;
            CFE_EVS_SendEvent(PKTMGR_LOAD_TBL_SUBSCRIBE_ERR_EID,CFE_EVS_ERROR,
                              "Error subscribing to stream 0x%4X, BufLim %d, Status %i",
                              PktMgr->Tbl.Pkt[i].StreamId, PktMgr->Tbl.Pkt[i].BufLim, Status);
         }
      }

   } /* End pkt loop */

   if (FailedSubscription == 0) {
      
      CFE_EVS_SendEvent(PKTMGR_LOAD_TBL_INFO_EID, CFE_EVS_INFORMATION,
                        "Loaded new table with %d packets", PktCnt);
   }
   else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(PKTMGR_LOAD_TBL_ERR_EID, CFE_EVS_INFORMATION,
                        "Attempted to load new table with %d packets. Failed %d subscriptions",
                        PktCnt, FailedSubscription);
   }

   return RetStatus;

} /* End PKTMGR_LoadTbl() */


/******************************************************************************
** Function: PKTMGR_LoadTblEntry
**
*/
boolean PKTMGR_LoadTblEntry(uint16 PktIdx, PKTTBL_Pkt* PktArray)
{

   int32    Status;
   boolean  RetStatus = TRUE;
   PKTTBL_Pkt* NewPkt = &(PktMgr->Tbl.Pkt[PktIdx]); 

   CFE_PSP_MemCpy(NewPkt,&PktArray[PktIdx],sizeof(PKTTBL_Pkt));

   Status = CFE_SB_SubscribeEx(NewPkt->StreamId, PktMgr->TlmPipe, NewPkt->Qos, NewPkt->BufLim);

   if(Status == CFE_SUCCESS) {
      
      CFE_EVS_SendEvent(PKTMGR_LOAD_TBL_ENTRY_INFO_EID, CFE_EVS_INFORMATION,
                        "Loaded table entry %d with stream 0x%4X, BufLim %d",
                        PktIdx, NewPkt->StreamId, NewPkt->BufLim);
   }
   else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(PKTMGR_LOAD_TBL_ENTRY_SUBSCRIBE_ERR_EID,CFE_EVS_ERROR,
                        "Error subscribing to stream 0x%4X, BufLim %d, Status %i",
                        NewPkt->StreamId, NewPkt->BufLim, Status);
   }

   return RetStatus;

} /* End PKTMGR_LoadTblEntry() */


/******************************************************************************
** Function: PKTMGR_EnableOutputCmd
**
*/
boolean PKTMGR_EnableOutputCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const PKTMGR_EnableOutputCmdParam *EnableOutputCmd = (const PKTMGR_EnableOutputCmdParam *) MsgPtr;
   boolean  RetStatus = TRUE;

   strncpy(PktMgr->TlmDestIp, EnableOutputCmd->DestIp, PKTMGR_IP_STR_LEN);

   PktMgr->SuppressSend = FALSE;
   CFE_EVS_SendEvent(PKTMGR_TLM_OUTPUT_ENA_INFO_EID, CFE_EVS_INFORMATION,
                     "Telemetry output enabled for IP %s", PktMgr->TlmDestIp);

   /*
   ** If disabled then create the socket and turn it on. If already
   ** enabled then destination address is changed in the existing socket
   */
   if(PktMgr->DownlinkOn == FALSE)
   {

      if ( (PktMgr->TlmSockId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
      {
         RetStatus = FALSE;
         CFE_EVS_SendEvent(PKTMGR_TLM_OUTPUT_ENA_SOCKET_ERR_EID, CFE_EVS_ERROR,
                           "Telemetry output enable socket error. errno %d", errno);
      }
      else
      {
         PktMgr->DownlinkOn = TRUE;
      }

   } /* End if downlink disabled */

   return RetStatus;

} /* End PKTMGR_EnableOutputCmd() */


/******************************************************************************
** Function: PKTMGR_AddPktCmd
**
*/
boolean PKTMGR_AddPktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const  PKTMGR_AddPktCmdParam *CmdParam = (const PKTMGR_AddPktCmdParam *) MsgPtr;
   int         PktIdx;
   PKTTBL_Pkt  NewPkt;
   boolean     RetStatus = TRUE;

   if ( (PktIdx = StreamIdPktIdx(CmdParam->StreamId)) < 0) {

      if ( (PktIdx = UnusedTblPktIdx()) >= 0 ) {

         NewPkt.StreamId = CmdParam->StreamId;
         NewPkt.Qos      = CmdParam->Qos;
         NewPkt.BufLim   = CmdParam->BufLim;
         PKTMGR_LoadTblEntry(PktIdx, &NewPkt); /* FIXME - Assumes source is also an array */
         CFE_EVS_SendEvent(PKTMGR_ADD_PKT_INFO_EID, CFE_EVS_INFORMATION,
                           "Added packet 0x%04X, QoS (%d,%d), BufLim %d",
                           NewPkt.StreamId, NewPkt.Qos.Priority, NewPkt.Qos.Reliability, NewPkt.BufLim);

      } /* end if found unused entry */
      else
      {

         RetStatus = FALSE;
         CFE_EVS_SendEvent(PKTMGR_ADD_PKT_NO_FREE_ENTRY_ERR_EID, CFE_EVS_ERROR,
                           "Error adding packet 0x%04X, packet table is full. Unused StreamId = 0x%04X",
                           CmdParam->StreamId,PKTTBL_UNUSED_MSG_ID);

      }

   } /* End if didn't find existing entry */
   else
   {
      CFE_EVS_SendEvent(PKTMGR_ADD_PKT_DUPLICATE_ENTRY_EID, CFE_EVS_ERROR,
                        "Error adding packet, stream id 0x%4X exists at entry %d",
                        CmdParam->StreamId, PktIdx);

   } /* End if found existing entry */

   return RetStatus;

} /* End of PKTMGR_AddPktCmd() */

/*******************************************************************
** Function: PKTMGR_RemovePktCmd
**
*/
boolean PKTMGR_RemovePktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const PKTMGR_RemovePktCmdParam *RemovePktCmd = (const PKTMGR_RemovePktCmdParam *) MsgPtr;
   int      PktIdx = -1;
   int32    Status;
   boolean  RetStatus = TRUE;

   if ( (PktIdx = StreamIdPktIdx(RemovePktCmd->StreamId)) >= 0)
   {

      CFE_PSP_MemCpy(&(PktMgr->Tbl.Pkt[PktIdx]), &UnusedTblPkt, sizeof(PKTTBL_Pkt));
      Status = CFE_SB_Unsubscribe(RemovePktCmd->StreamId, PktMgr->TlmPipe);

      if(Status == CFE_SUCCESS)
      {
         CFE_EVS_SendEvent(PKTMGR_REMOVE_PKT_INFO_EID, CFE_EVS_INFORMATION,
                           "Removed stream id 0x%4X at table packet index %d",
                           RemovePktCmd->StreamId, PktIdx);
      }
      else
      {
         RetStatus = FALSE;
         CFE_EVS_SendEvent(PKTMGR_REMOVE_PKT_SB_ERR_EID, CFE_EVS_ERROR,
                           "Error removing stream id 0x%4X at table packet index %d. Unsubscribe status 0x%8x",
                           RemovePktCmd->StreamId, PktIdx, Status);
      }

   } /* End if found stream ID in table */
   else
   {

      CFE_EVS_SendEvent(PKTMGR_REMOVE_PKT_NOT_FOUND_ERR_EID, CFE_EVS_ERROR,
                        "Error removing stream id 0x%4X. Packet not found",
                        RemovePktCmd->StreamId);

   } /* End if didn't find stream ID in table */

   return RetStatus;

} /* End of PKTMGR_RemovePktCmd() */


/******************************************************************************
** Function: PKTMGR_RemoveAllPktsCmd
**
** Notes:
**   1. The cFE to_lab code unsubscribes the command and send HK MIDs. I'm not
**      sure why this is done and I'm not sure how the command is used. This 
**      command is intended to help manage TO telemetry packets.
*/
boolean PKTMGR_RemoveAllPktsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   int      i, PktCnt = 0, FailedUnsubscribe = 0;
   int32    Status;
   boolean  RetStatus = TRUE;

   for (i=0; i < PKTTBL_MAX_PKT_CNT; i++)
   {
       if (PktMgr->Tbl.Pkt[i].StreamId != PKTTBL_UNUSED_MSG_ID )
       {

          PktCnt++;
          CFE_PSP_MemCpy(&(PktMgr->Tbl.Pkt[i]), &UnusedTblPkt, sizeof(PKTTBL_Pkt));
          Status = CFE_SB_Unsubscribe(PktMgr->Tbl.Pkt[i].StreamId, PktMgr->TlmPipe);

          if(Status != CFE_SUCCESS)
          {
             FailedUnsubscribe++;
             CFE_EVS_SendEvent(PKTMGR_REMOVE_ALL_PKTS_UNSUBSCRIBE_ERR_EID,CFE_EVS_ERROR,
                               "Error removing stream id 0x%4X at table packet index %d. Unsubscribe status 0x%8X",
                               PktMgr->Tbl.Pkt[i].StreamId, i, Status);
          }

       } /* End if packet in use */

   } /* End pkt loop */

   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "PKTMGR_RemoveAllPktsCmd() - About to flush pipe\n");
   FlushTlmPipe();
   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "PKTMGR_RemoveAllPktsCmd() - Completed pipe flush\n");

   if (FailedUnsubscribe == 0)
   {
      CFE_EVS_SendEvent(PKTMGR_REMOVE_ALL_PKTS_INFO_EID, CFE_EVS_INFORMATION,
                        "Removed %d table packet entries", PktCnt);
   }
   else
   {
      RetStatus = FALSE;
      CFE_EVS_SendEvent(PKTMGR_REMOVE_ALL_PKTS_ERR_EID, CFE_EVS_INFORMATION,
                        "Attempted to remove %d packet entries. Failed %d unsubscribes",
                        PktCnt, FailedUnsubscribe);
   }

   return RetStatus;

} /* End of PKTMGR_RemoveAllPktsCmd() */


/******************************************************************************
** Function: DestructorCallback
**
** This function is called when the app is killed. This should
** never occur but if it does this will close the network socket.
*/
static void DestructorCallback(void)
{

   CFE_EVS_SendEvent(PKTMGR_DESTRUCTOR_INFO_EID, CFE_EVS_INFORMATION, "Destructor callback -- Closing TO Network socket. Downlink on = %d\n", PktMgr->DownlinkOn);
   
   if (PktMgr->DownlinkOn) {
      
      close(PktMgr->TlmSockId);
   
   }

} /* End DestructorCallback() */

/******************************************************************************
** Function: FlushTlmPipe
**
** Remove all of the packets from the input pipe.
**
*/
static void FlushTlmPipe(void)
{

   int32 Status;
   CFE_SB_MsgPtr_t MsgPtr = NULL;

   do
   {
      Status = CFE_SB_RcvMsg(&MsgPtr, PktMgr->TlmPipe, CFE_SB_POLL);

   } while(Status == CFE_SUCCESS);

} /* End FlushTlmPipe() */

/******************************************************************************
** Function: StreamIdPktIdx
**
** Look for a StreamId in the packet table. Return the index of the
** packet if it is found and return -1 if it is not found.
**
*/
static int StreamIdPktIdx(uint16 StreamId)
{

   int i;
   int PktIdx = -1;

   for (i=0; i < PKTTBL_MAX_PKT_CNT; i++)
   {

      if (PktMgr->Tbl.Pkt[i].StreamId == StreamId)
      {
         PktIdx = i;
         break;
      }

   } /* end pkt loop */

   return PktIdx;

} /* End StreamIdPktIdx() */

/******************************************************************************
** Function: UnusedTblPktIdx
**
** Look for an unused packet table entry. Return the index of the
** unused table entry if one is found and return -1 if one is not
** found.
**
*/
static int UnusedTblPktIdx(void)
{

   int i;
   int PktIdx = -1;

   for (i=0; i < PKTTBL_MAX_PKT_CNT; i++)
   {

      if (PktMgr->Tbl.Pkt[i].StreamId == PKTTBL_UNUSED_MSG_ID)
      {
         PktIdx = i;
         break;
      }

   } /* end pkt loop */

   return PktIdx;

} /* End UnusedTblPktIdx() */

/* end of file */
