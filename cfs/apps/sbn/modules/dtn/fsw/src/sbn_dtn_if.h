#ifndef _sbn_dtn_if_h_
#define _sbn_dtn_if_h_

#include "sbn_constants.h"
#include "sbn_interfaces.h"
#include "cfe.h"

/* The version #'s this module works with. If this doesn't match the core
 * SBN application version #'s, this module should report and return an error.
 * Note: these DEFINITELY should be hardcoded here, do not map them to
 * the SBN version numbers.
 */
#define SBN_DTN_MAJOR 1
#define SBN_DTN_MINOR 11
#define SBN_DTN_REVISION 0

int SBN_DTN_Init(int Major, int Minor, int Revision);

int SBN_DTN_LoadNet(SBN_NetInterface_t *Net, const char *Address);

int SBN_DTN_LoadPeer(SBN_PeerInterface_t *Peer, const char *Address);

int SBN_DTN_InitNet(SBN_NetInterface_t *NetInterface);

int SBN_DTN_InitPeer(SBN_PeerInterface_t *PeerInterface);

int SBN_DTN_PollPeer(SBN_PeerInterface_t *PeerInterface);

int SBN_DTN_Send(SBN_PeerInterface_t *Peer, SBN_MsgType_t MsgType,
    SBN_MsgSz_t MsgSz, void *Payload);

int SBN_DTN_Recv(SBN_NetInterface_t *Net, SBN_MsgType_t *MsgTypePtr,
        SBN_MsgSz_t *MsgSzPtr, SBN_CpuID_t *CpuIDPtr,
        void *PayloadBuffer);

int SBN_DTN_ReportModuleStatus(SBN_ModuleStatusPacket_t *Packet);

int SBN_DTN_ResetPeer(SBN_PeerInterface_t *PeerInterface);

int SBN_DTN_UnloadNet(SBN_NetInterface_t *NetInterface);

int SBN_DTN_UnloadPeer(SBN_PeerInterface_t *PeerInterface);

SBN_IfOps_t SBN_DTN_Ops =
{
    SBN_DTN_Init,
    SBN_DTN_LoadNet,
    SBN_DTN_LoadPeer,
    SBN_DTN_InitNet,
    SBN_DTN_InitPeer,
    SBN_DTN_PollPeer,
    SBN_DTN_Send,
    NULL,
    SBN_DTN_Recv,
    SBN_DTN_ReportModuleStatus,
    SBN_DTN_ResetPeer,
    SBN_DTN_UnloadNet,
    SBN_DTN_UnloadPeer
};

#endif /* _sbn_dtn_if_h_ */
