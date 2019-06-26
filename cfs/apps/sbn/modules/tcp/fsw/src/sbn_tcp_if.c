#include "sbn_tcp_if_struct.h"
#include "sbn_tcp_if.h"
#include "sbn_tcp_events.h"
#include "cfe.h"
#include <network_includes.h>
#include <string.h>
#include <errno.h>

/* at some point this will be replaced by the OSAL network interface */
#ifdef _VXWORKS_OS_
#include "selectLib.h"
#else
#include <sys/select.h>
#endif

int SBN_TCP_Init(int Major, int Minor, int Revision)
{
    if(Major != SBN_TCP_MAJOR || Minor != SBN_TCP_MINOR
        || Revision != SBN_TCP_REVISION)
    {
        CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_ERROR,
                "mismatching version %d.%d.%d (SBN app reports %d.%d.%d)",
                SBN_TCP_MAJOR, SBN_TCP_MINOR, SBN_TCP_REVISION,
                Major, Minor, Revision);
        return SBN_ERROR;
    }/* end if */

    return SBN_SUCCESS;
}/* end SBN_TCP_Init() */

static uint8 SendBufs[SBN_MAX_NETS][SBN_MAX_PACKED_MSG_SZ];
static int SendBufCnt = 0;
int SBN_TCP_LoadNet(SBN_NetInterface_t *Net, const char *Address)
{
    SBN_TCP_Net_t *NetData = (SBN_TCP_Net_t *)Net->ModulePvt;

    char *Colon = strchr(Address, ':');

    if(!Colon)
    {
        CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_ERROR,
                "invalid net address (%s)", Address);
        return SBN_ERROR;
    }/* end if */

    int AddrLen = Colon - Address;

    strncpy(NetData->Host, Address, AddrLen);
    char *ValidatePtr = NULL;
    NetData->Port = strtol(Colon + 1, &ValidatePtr, 0);
    if(!ValidatePtr || ValidatePtr == Colon + 1)
    {
        CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_ERROR,
                "invalid port");
    }/* end if */

    NetData->BufNum = SendBufCnt++;

    CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_INFORMATION,
        "net configured (%s:%d)", NetData->Host, NetData->Port);

    return SBN_SUCCESS;
}/* end SBN_TCP_LoadNet */

static uint8 RecvBufs[SBN_MAX_PEER_CNT][SBN_MAX_PACKED_MSG_SZ];
static int RecvBufCnt = 0;
int SBN_TCP_LoadPeer(SBN_PeerInterface_t *Peer, const char *Address)
{
    SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;

    char *Colon = strchr(Address, ':');

    if(!Colon)
    {
        CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_ERROR,
                "invalid net address (%s)", Address);
        return SBN_ERROR;
    }/* end if */

    int AddrLen = Colon - Address;

    strncpy(PeerData->Host, Address, AddrLen);
    char *ValidatePtr = NULL;
    PeerData->Port = strtol(Colon + 1, &ValidatePtr, 0);
    if(!ValidatePtr || ValidatePtr == Colon + 1)
    {
        CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_ERROR,
                "invalid port");
    }/* end if */

    PeerData->BufNum = SendBufCnt++;

    CFE_EVS_SendEvent(SBN_TCP_CONFIG_EID, CFE_EVS_INFORMATION,
        "peer configured (%s:%d)", PeerData->Host, PeerData->Port);

    return SBN_SUCCESS;
}/* end SBN_TCP_LoadEntry */

/**
 * Initializes an TCP host or peer data struct depending on the
 * CPU name.
 *
 * @param  Interface data structure containing the file entry
 * @return SBN_SUCCESS on success, error code otherwise
 */
int SBN_TCP_InitNet(SBN_NetInterface_t *Net)
{
    SBN_TCP_Net_t *NetData = (SBN_TCP_Net_t *)Net->ModulePvt;

    struct sockaddr_in my_addr;
    int OptVal = 0;

    NetData->Socket = 0;
    int Socket = 0;

    CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_DEBUG,
        "creating socket for %s:%d", NetData->Host, NetData->Port);

    if((Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_ERROR,
            "unable to create socket (errno=%d)", errno);
        return SBN_ERROR;
    }/* end if */

    OptVal = 1;
    setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR,
        (const void *)&OptVal, sizeof(int));

    my_addr.sin_addr.s_addr = inet_addr(NetData->Host);
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(NetData->Port);

    if(bind(Socket, (struct sockaddr *) &my_addr,
        sizeof(my_addr)) < 0)
    {
        close(Socket);
        CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_ERROR,
            "bind call failed (%s:%d Socket=%d errno=%d)",
            NetData->Host, NetData->Port, Socket, errno);
        return SBN_ERROR;
    }/* end if */

    if(listen(Socket, SBN_MAX_PEER_CNT) < 0)
    {
        close(Socket);
        CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_ERROR,
            "listen call failed (%s:%d Socket=%d errno=%d)",
            NetData->Host, NetData->Port, Socket, errno);
        return SBN_ERROR;
    }/* end if */

    NetData->Socket = Socket;

    return SBN_SUCCESS;
}/* end SBN_TCP_Init */

/**
 * Initializes an TCP peer.
 *
 * @param  Interface data structure containing the file entry
 * @return SBN_SUCCESS on success, error code otherwise
 */
int SBN_TCP_InitPeer(SBN_PeerInterface_t *Peer)
{
    SBN_TCP_Peer_t *PeerData
        = (SBN_TCP_Peer_t *)Peer->ModulePvt;

    PeerData->ConnectOut =
        (Peer->ProcessorID > CFE_PSP_GetProcessorId());
    PeerData->Connected = FALSE;

    return SBN_SUCCESS;
}/* end SBN_TCP_Init */

static void CheckNet(SBN_NetInterface_t *Net)
{
    SBN_TCP_Net_t *NetData = (SBN_TCP_Net_t *)Net->ModulePvt;
    fd_set ReadFDs;
    struct timeval timeout;
    struct sockaddr_in ClientAddr;
    socklen_t ClientLen = sizeof(ClientAddr);
    int PeerIdx = 0;

    /**
     * Check for new connections.
     */
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_usec = 100;

    FD_ZERO(&ReadFDs);
    FD_SET(NetData->Socket, &ReadFDs);

    /* TODO: thread implementation? */
    if(select(NetData->Socket + 1, &ReadFDs, 0, 0, &timeout) < 0)
    {
        return;
    }/* end if */

    if(FD_ISSET(NetData->Socket, &ReadFDs))
    {
        int ClientFd = 0;
        if ((ClientFd
            = accept(NetData->Socket,
                (struct sockaddr *)&ClientAddr, &ClientLen)) < 0)
        {
            return;
        }/* end if */
        
        for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
        {
            SBN_PeerInterface_t *Peer = &Net->Peers[PeerIdx];
            SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;
            if(ClientAddr.sin_addr.s_addr
                == inet_addr(PeerData->Host))
            {
                CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_INFORMATION,
                    "CPU %d connected", Peer->ProcessorID);

                PeerData->Socket = ClientFd;
                PeerData->Connected = TRUE;
 
                SBN_Connected(Peer);

                return;
            }/* end if */
        }/* end for */
        
        /* invalid peer */
        close(ClientFd);
    }/* end if */

    /**
     * For peers I connect out to, and which are not currently connected,
     * try connecting now.
     */
    for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
    {
        SBN_PeerInterface_t *Peer = &Net->Peers[PeerIdx];
        SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;
        if(PeerData->ConnectOut && !PeerData->Connected)
        {
            OS_time_t LocalTime;
            OS_GetLocalTime(&LocalTime);
            /* TODO: make a #define */
            if(LocalTime.seconds > PeerData->LastConnectTry.seconds + 5)
            {
                int Socket = 0;

                if((Socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                {
                    CFE_EVS_SendEvent(SBN_TCP_SOCK_EID, CFE_EVS_ERROR,
                        "unable to create socket (errno=%d)", errno);
                    return;
                }/* end if */

                struct sockaddr_in ServerAddr;
                memset(&ServerAddr, 0, sizeof(ServerAddr));
                ServerAddr.sin_family = AF_INET;
                ServerAddr.sin_addr.s_addr = inet_addr(PeerData->Host);
                ServerAddr.sin_port = htons(PeerData->Port);

                if((connect(Socket, (struct sockaddr *)&ServerAddr,
                        sizeof(ServerAddr)))
                    >= 0)
                {
                    CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_INFORMATION,
                        "CPU %d connected", Peer->ProcessorID);

                    PeerData->Socket = Socket;
                    PeerData->Connected = TRUE;

                    SBN_Connected(Peer);
                }
                else
                {
                    close(Socket);
                    PeerData->LastConnectTry.seconds = LocalTime.seconds;
                }/* end if */
            }/* end if */
        }/* end if */
    }/* end for */
}/* end CheckNet */

int SBN_TCP_PollPeer(SBN_PeerInterface_t *Peer)
{
    CheckNet(Peer->Net);

    SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;

    if(!PeerData->Connected)
    {
        return 0;
    }/* end if */

    OS_time_t CurrentTime;
    OS_GetLocalTime(&CurrentTime);

    if(SBN_TCP_PEER_HEARTBEAT > 0 &&
        CurrentTime.seconds - Peer->LastSend.seconds
            > SBN_TCP_PEER_HEARTBEAT)
    {
        SBN_TCP_Send(Peer, SBN_TCP_HEARTBEAT_MSG, 0, NULL);
    }/* end if */

    if(SBN_TCP_PEER_TIMEOUT > 0 &&
        CurrentTime.seconds - Peer->LastRecv.seconds
            > SBN_TCP_PEER_TIMEOUT)
    {
        CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_INFORMATION,
            "CPU %d disconnected", Peer->ProcessorID);

        close(PeerData->Socket);
        PeerData->Connected = FALSE;

        SBN_Disconnected(Peer);
    }/* end if */

    return 0;
}

int SBN_TCP_Send(SBN_PeerInterface_t *Peer,
    SBN_MsgType_t MsgType, SBN_MsgSz_t MsgSz, void *Msg)
{
    SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;
    SBN_NetInterface_t *Net = Peer->Net;
    SBN_TCP_Net_t *NetData = (SBN_TCP_Net_t *)Net->ModulePvt;

    CheckNet(Net);

    if (!PeerData->Connected)
    {
        /* fail silently as the peer is not connected (yet) */
        return SBN_SUCCESS;
    }/* end if */

    SBN_PackMsg(&SendBufs[NetData->BufNum], MsgSz, MsgType, CFE_PSP_GetProcessorId(), Msg);
    size_t sent_size = send(PeerData->Socket, &SendBufs[NetData->BufNum],
        MsgSz + SBN_PACKED_HDR_SZ, MSG_NOSIGNAL);
    if(sent_size < MsgSz + SBN_PACKED_HDR_SZ)
    {
        CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_INFORMATION,
            "CPU %d disconnected", Peer->ProcessorID);

        close(PeerData->Socket);
        PeerData->Connected = FALSE;

        SBN_Disconnected(Peer);
    }/* end if */

    return SBN_SUCCESS;
}/* end SBN_TCP_Send */

int SBN_TCP_Recv(SBN_NetInterface_t *Net, SBN_PeerInterface_t *Peer,
    SBN_MsgType_t *MsgTypePtr, SBN_MsgSz_t *MsgSzPtr,
    SBN_CpuID_t *CpuIDPtr, void *MsgBuf)
{
    SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;

    CheckNet(Net);

    if (!PeerData->Connected)
    {
        /* fail silently as the peer is not connected (yet) */
        return SBN_IF_EMPTY;
    }/* end if */

#ifndef SBN_RECV_TASK

    /* task-based peer connections block on reads, otherwise use select */

    fd_set ReadFDs;
    struct timeval Timeout;

    FD_ZERO(&ReadFDs);
    FD_SET(PeerData->Socket, &ReadFDs);

    memset(&Timeout, 0, sizeof(Timeout));

    if(select(FD_SETSIZE, &ReadFDs, NULL, NULL, &Timeout) == 0)
    {
        /* nothing to receive */
        return SBN_IF_EMPTY;
    }/* end if */

#endif /* !SBN_RECV_TASK */

    ssize_t Received = 0;

    int ToRead = 0;

    if(!PeerData->ReceivingBody)
    {
        /* recv the header first */
        ToRead = SBN_PACKED_HDR_SZ - PeerData->RecvSz;

        Received = recv(PeerData->Socket,
            (char *)&RecvBufs[PeerData->BufNum] + PeerData->RecvSz,
            ToRead, 0);

        if(Received < 0)
        {
            CFE_EVS_SendEvent(SBN_TCP_DEBUG_EID, CFE_EVS_INFORMATION,
                "CPU %d disconnected", Peer->ProcessorID);

            close(PeerData->Socket);
            PeerData->Connected = FALSE;

            SBN_Disconnected(Peer);

            return SBN_IF_EMPTY;
        }/* end if */

        PeerData->RecvSz += Received;

        if(Received >= ToRead)
        {
            PeerData->ReceivingBody = TRUE; /* and continue on to recv body */
        }
        else
        {
            return SBN_IF_EMPTY; /* wait for the complete header */
        }/* end if */
    }/* end if */

    /* only get here if we're recv'd the header and ready for the body */

    ToRead = CFE_MAKE_BIG16(
            *((SBN_MsgSz_t *)&RecvBufs[PeerData->BufNum])
        ) + SBN_PACKED_HDR_SZ - PeerData->RecvSz;
    if(ToRead)
    {
        Received = recv(PeerData->Socket,
            (char *)&RecvBufs[PeerData->BufNum] + PeerData->RecvSz,
            ToRead, 0);
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
        CpuIDPtr, MsgBuf) == FALSE)
    {
        return SBN_ERROR;
    }/* end if */

    PeerData->ReceivingBody = FALSE;
    PeerData->RecvSz = 0;

    return SBN_SUCCESS;
}/* end SBN_TCP_Recv */

int SBN_TCP_ReportModuleStatus(SBN_ModuleStatusPacket_t *Packet)
{
    return SBN_NOT_IMPLEMENTED;
}/* end SBN_TCP_ReportModuleStatus */

int SBN_TCP_ResetPeer(SBN_PeerInterface_t *Peer)
{
    return SBN_NOT_IMPLEMENTED;
}/* end SBN_TCP_ResetPeer */

int SBN_TCP_UnloadNet(SBN_NetInterface_t *Net)
{
    SBN_TCP_Net_t *NetData = (SBN_TCP_Net_t *)Net->ModulePvt;

    if(NetData->Socket)
    {
        close(NetData->Socket);
    }/* end if */

    int PeerIdx = 0;
    for(PeerIdx = 0; PeerIdx < Net->PeerCnt; PeerIdx++)
    {
        SBN_TCP_UnloadPeer(&Net->Peers[PeerIdx]);
    }/* end if */

    return SBN_SUCCESS;
}/* end SBN_TCP_ResetPeer */

int SBN_TCP_UnloadPeer(SBN_PeerInterface_t *Peer)
{
    SBN_TCP_Peer_t *PeerData = (SBN_TCP_Peer_t *)Peer->ModulePvt;

    if(PeerData->Socket)
    {
        close(PeerData->Socket);
    }/* end if */


    if(PeerData->Connected == TRUE)
    {
        SBN_Disconnected(Peer);
    }/* end if */

    return SBN_SUCCESS;
}/* end SBN_TCP_ResetPeer */
