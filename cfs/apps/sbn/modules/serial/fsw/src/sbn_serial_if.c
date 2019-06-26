#include "sbn_serial_if_struct.h"
#include "sbn_serial_if.h"
#include "sbn_serial_events.h"
#include "cfe.h"
#include <network_includes.h>
#include <string.h>
#include <errno.h>

#include <termios.h>
#include <unistd.h>

/* at some point this will be replaced by the OSAL network interface */
#ifdef _VXWORKS_OS_
#include "selectLib.h"
#else
#include <sys/select.h>
#endif

int SBN_SERIAL_Init(int Major, int Minor, int Revision)
{   
    if(Major != SBN_SERIAL_MAJOR || Minor != SBN_SERIAL_MINOR
        || Revision != SBN_SERIAL_REVISION)
    {   
        CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
                "mismatching version %d.%d.%d (SBN app reports %d.%d.%d)",
                SBN_SERIAL_MAJOR, SBN_SERIAL_MINOR, SBN_SERIAL_REVISION,
                Major, Minor, Revision);
        return SBN_ERROR;
    }/* end if */
    
    return SBN_SUCCESS;
}/* end SBN_SERIAL_Init() */

int SBN_SERIAL_LoadNet(SBN_NetInterface_t *Net, const char *Address)
{
    /* this space intentionally left blank */
    return SBN_SUCCESS;
}/* end SBN_SERIAL_LoadNet */

static uint8 SendBuf[SBN_MAX_PACKED_MSG_SZ],
    RecvBufs[SBN_MAX_NETS * SBN_MAX_PEER_CNT];
static int BufCnt = 0;

int SBN_SERIAL_LoadPeer(SBN_PeerInterface_t *Peer, const char *Address)
{
    SBN_SERIAL_Peer_t *PeerData = (SBN_SERIAL_Peer_t *)Peer->ModulePvt;

    strncpy(PeerData->Filename, Address, sizeof(PeerData->Filename));

    PeerData->BufNum = BufCnt++;

    return SBN_SUCCESS;
}/* end SBN_SERIAL_LoadEntry */

/**
 * Initializes an SERIAL host or peer data struct depending on the
 * CPU name.
 *
 * @param  Interface data structure containing the file entry
 * @return SBN_SUCCESS on success, error code otherwise
 */
int SBN_SERIAL_InitNet(SBN_NetInterface_t *Net)
{
    /* this space intentionally left blank */
    return SBN_SUCCESS;
}/* end SBN_SERIAL_Init */

/** returns 1 on successful connection */
static int TrySerial(SBN_SERIAL_Peer_t *PeerData)
{
    OS_time_t CurrentTime;
    OS_GetLocalTime(&CurrentTime);
    if(CurrentTime.seconds < PeerData->LastConnTry.seconds + SBN_SERIAL_CONNTRY_TIME)
    {
        return FALSE;
    }

    int FD = open(PeerData->Filename, OS_READ_WRITE);
    if(FD < 0)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_DEVICE_EID, CFE_EVS_ERROR,
            "Unable to open device: %s\n", PeerData->Filename);
        return FALSE;
    }

    struct termios tty;

    if(tcgetattr(FD, &tty) != 0)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
                "unable to get attrs on %s", PeerData->Filename);
        close(FD);
        return FALSE;
    }

    cfsetspeed(&tty, B115200);

    cfmakeraw(&tty);

    if(tcsetattr(FD, TCSANOW, &tty) != 0)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_CONFIG_EID, CFE_EVS_ERROR,
                "unable to set attrs on %s", PeerData->Filename);
        close(FD);
        return FALSE;
    }

    /* all good! */

    CFE_EVS_SendEvent(SBN_SERIAL_DEBUG_EID, CFE_EVS_INFORMATION,
        "Serial device %s (task=%d, fd=%d) attached, flushing.", PeerData->Filename, OS_TaskGetId(), FD);

    PeerData->SerialConn = TRUE;
    PeerData->FD = FD;

    return TRUE;
}

/**
 * Initializes an SERIAL peer.
 *
 * @param  Interface data structure containing the file entry
 * @return SBN_SUCCESS on success, error code otherwise
 */
int SBN_SERIAL_InitPeer(SBN_PeerInterface_t *Peer)
{
    return SBN_SUCCESS;
}/* end SBN_SERIAL_Init */

int SBN_SERIAL_PollPeer(SBN_PeerInterface_t *Peer)
{
    SBN_SERIAL_Peer_t *PeerData = (SBN_SERIAL_Peer_t *)Peer->ModulePvt;

    if(!PeerData->SerialConn)
    {
        if(!TrySerial(PeerData))
        {
            /* no luck */
            return 0;
        }/* end if */
    }/* end if */

    OS_time_t CurrentTime;
    OS_GetLocalTime(&CurrentTime);

    if(SBN_SERIAL_PEER_HEARTBEAT > 0 &&
        CurrentTime.seconds - Peer->LastSend.seconds
            > SBN_SERIAL_PEER_HEARTBEAT)
    {
        OS_GetLocalTime(&Peer->LastSend);

        SBN_SERIAL_Send(Peer, SBN_SERIAL_HEARTBEAT_MSG, 0, NULL);
    }/* end if */

    if(Peer->Connected == TRUE && SBN_SERIAL_PEER_TIMEOUT > 0 &&
        CurrentTime.seconds - Peer->LastRecv.seconds
            > SBN_SERIAL_PEER_TIMEOUT)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_DEBUG_EID, CFE_EVS_INFORMATION,
            "CPU %d disconnected", Peer->ProcessorID);

        close(PeerData->FD);

        SBN_Disconnected(Peer);
    }/* end if */

    return 0;
} /* end SBN_SERIAL_PollPeer */

int SBN_SERIAL_Send(SBN_PeerInterface_t *Peer,
    SBN_MsgType_t MsgType, SBN_MsgSz_t MsgSz, void *Msg)
{
    SBN_SERIAL_Peer_t *PeerData = (SBN_SERIAL_Peer_t *)Peer->ModulePvt;

    if(!PeerData->SerialConn)
    {
        return SBN_SUCCESS;
    }/* end if */

    SBN_PackMsg(&SendBuf, MsgSz, MsgType, CFE_PSP_GetProcessorId(), Msg);
    size_t sent_size = write(PeerData->FD, &SendBuf,
        MsgSz + SBN_PACKED_HDR_SZ);
    if(sent_size < MsgSz + SBN_PACKED_HDR_SZ && Peer->Connected == TRUE)
    {
        CFE_EVS_SendEvent(SBN_SERIAL_DEBUG_EID, CFE_EVS_INFORMATION,
            "CPU %d disconnected", Peer->ProcessorID);

        close(PeerData->FD);

        SBN_Disconnected(Peer);
    }/* end if */

    return SBN_SUCCESS;
}/* end SBN_SERIAL_Send */

int SBN_SERIAL_Recv(SBN_NetInterface_t *Net, SBN_PeerInterface_t *Peer,
    SBN_MsgType_t *MsgTypePtr, SBN_MsgSz_t *MsgSzPtr,
    SBN_CpuID_t *CpuIDPtr, void *MsgBuf)
{
    SBN_SERIAL_Peer_t *PeerData = (SBN_SERIAL_Peer_t *)Peer->ModulePvt;

    if(!PeerData->SerialConn)
    {
        if(!TrySerial(PeerData))
        {
            return SBN_IF_EMPTY;
        }/* end if */
    }/* end if */

#ifndef SBN_RECV_TASK
        fd_set ReadFDs;
        struct timeval Timeout;
        FD_ZERO(&ReadFDs);
        FD_SET(PeerData->FD, &ReadFDs);
        memset(&Timeout, 0, sizeof(Timeout));
        Timeout.tv_usec = 100;

        int Result = 0;
        if((Result = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &Timeout)) == 0)
        {
            return SBN_IF_EMPTY;
        }/* end if */

        if(Result < 0)
        {
            CFE_EVS_SendEvent(SBN_SERIAL_DEBUG_EID, CFE_EVS_INFORMATION,
                "Read error TaskID=%d, FD=%d, errno=%d, CPU=%d disconnected",
                OS_TaskGetId(), PeerData->FD, errno, Peer->ProcessorID);

            close(PeerData->FD);
            PeerData->SerialConn = FALSE;

            SBN_Disconnected(Peer);

            return SBN_IF_EMPTY;
        }
#endif /* !SBN_RECV_TASK */

    ssize_t Received = 0;

    int ToRead = 0;

    if(!PeerData->ReceivingBody)
    {
        /* read the header first */
        ToRead = SBN_PACKED_HDR_SZ - PeerData->RecvSz;

        Received = read(PeerData->FD,
            (char *)&RecvBufs[PeerData->BufNum] + PeerData->RecvSz,
            ToRead);

        if(Received < 0)
        {
            CFE_EVS_SendEvent(SBN_SERIAL_DEBUG_EID, CFE_EVS_INFORMATION,
                "Read error FD=%d, errno=%d, CPU=%d disconnected",
                PeerData->FD, errno, Peer->ProcessorID);

            close(PeerData->FD);
            PeerData->SerialConn = FALSE;

            SBN_Disconnected(Peer);

            return SBN_IF_EMPTY;
        }/* end if */

        if(Peer->Connected == FALSE)
        {
            CFE_EVS_SendEvent(SBN_SERIAL_DEBUG_EID, CFE_EVS_INFORMATION,
                "CPU=%d connected", Peer->ProcessorID);

            SBN_Connected(Peer);
        }/* end if */

        PeerData->RecvSz += Received;

        if(Received >= ToRead)
        {
            PeerData->ReceivingBody = TRUE; /* and continue on to read body */
        }
        else
        {
            return SBN_IF_EMPTY; /* wait for the complete header */
        }/* end if */
    }/* end if */

    /* only get here if we're read'd the header and ready for the body */

    ToRead = CFE_MAKE_BIG16(*((SBN_MsgSz_t *)&RecvBufs[PeerData->BufNum]))
        + SBN_PACKED_HDR_SZ - PeerData->RecvSz;
    if(ToRead)
    {
        Received = read(PeerData->FD,
            (char *)&RecvBufs[PeerData->BufNum] + PeerData->RecvSz,
            ToRead);
        if(Received < 0)
        {
            return SBN_ERROR;
        }/* end if */

        PeerData->RecvSz += Received;

        if(Received < ToRead)
        {
            return SBN_IF_EMPTY; /* wait for the complete body */
        }/* end if */
    }/* end if */

    /* we have the complete body, decode! */
    if(SBN_UnpackMsg(&RecvBufs[PeerData->BufNum], MsgSzPtr, MsgTypePtr,
        CpuIDPtr, MsgBuf))
    {
        if(Peer->Connected == FALSE) SBN_Connected(Peer);
    }
    else
    {
        /* TODO: what to do if we can't unpack the message? */
    }/* end if */

    PeerData->ReceivingBody = FALSE;
    PeerData->RecvSz = 0;

    return SBN_SUCCESS;
}/* end SBN_SERIAL_Recv */

int SBN_SERIAL_ReportModuleStatus(SBN_ModuleStatusPacket_t *Packet)
{
    return SBN_NOT_IMPLEMENTED;
}/* end SBN_SERIAL_ReportModuleStatus */

int SBN_SERIAL_UnloadNet(SBN_NetInterface_t *Net)
{
    int PeerIdx = 0;
    for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
    {
        SBN_SERIAL_UnloadPeer(&Net->Peers[PeerIdx]);
    }/* end if */

    return SBN_SUCCESS;
}/* end SBN_SERIAL_ResetPeer */

int SBN_SERIAL_UnloadPeer(SBN_PeerInterface_t *Peer)
{
    SBN_SERIAL_Peer_t *PeerData = (SBN_SERIAL_Peer_t *)Peer->ModulePvt;

    if(PeerData->FD)
    {
        close(PeerData->FD);
    }/* end if */

    if(Peer->Connected == TRUE)
    {
        SBN_Disconnected(Peer);
    }/* end if */

    return SBN_SUCCESS;
}/* end SBN_SERIAL_ResetPeer */
