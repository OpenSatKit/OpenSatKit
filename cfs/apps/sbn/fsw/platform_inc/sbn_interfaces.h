#ifndef _sbn_interfaces_h_
#define _sbn_interfaces_h_

#include "cfe.h"
#include "sbn_constants.h"
#include "sbn_msg.h"

/*
** Other Structures
*/

/* used in local and peer subscription tables */
typedef struct {
  uint32            InUseCtr;
  CFE_SB_MsgId_t    MsgID;
  CFE_SB_Qos_t      QoS;
} SBN_Subs_t;

typedef uint16 SBN_MsgSz_t;
typedef uint8 SBN_MsgType_t;
typedef uint32 SBN_CpuID_t;

/**
 * @note The packed size is likely smaller than an in-memory's struct
 * size, as the compiler will align objects.
 * SBN headers are MsgSz + MsgType + CpuID
 * SBN subscription messages are MsgID + QoS
 */

/**
 * uint16 MsgSz, uint8 MsgType, uint32 ProcessorID
 */
#define SBN_PACKED_HDR_SZ (2 + 1 + 4)

/*
 * SBN_PACKED_HDR_SZ + uint16 SubCnt + CFE_SB_MsgId_t Subs[SubCnt]
 */
#define SBN_PACKED_SUB_SZ (SBN_PACKED_HDR_SZ + 2 + sizeof(CFE_SB_MsgId_t) * SBN_MAX_SUBS_PER_PEER)

#define SBN_MAX_PACKED_MSG_SZ (SBN_PACKED_HDR_SZ + CFE_SB_MAX_SB_MSG_SIZE)

/**
 * @brief Used by modules to pack messages to send.
 *
 * @param SBNMsgBuf[out] The buffer pointer to receive the packed message.
 *                       Should be MsgSz + SBN_PACKED_HDR_SZ bytes or larger.
 * @param MsgSz[in] The size of the Msg parameter.
 * @param MsgType[in] The type of the Msg (app, sub/unsub, heartbeat, announce).
 * @param CpuID[in] The CPU ID of the sender (should be CFE_CPU_ID)
 * @param Msg[in] The SBN message payload (CCSDS message, sub/unsub)
 *
 * @sa SBN_UnpackMsg
 */
void SBN_PackMsg(void *SBNMsgBuf, SBN_MsgSz_t MsgSz,
    SBN_MsgType_t MsgType, SBN_CpuID_t CpuID, void *Msg);

/**
 * @brief Used by modules to unpack messages received.
 *
 * @param SBNMsgBuf[in] The buffer pointer containing the SBN message.
 * @param MsgSzPtr[out] The size of the Msg parameter.
 * @param MsgTypePtr[out] The type of the Msg (app, sub/unsub, heartbeat, announce).
 * @param CpuID[out] The CPU ID of the sender (should be CFE_CPU_ID)
 * @param Msg[out] The SBN message payload (CCSDS message, sub/unsub, ensure it is at least CFE_SB_MAX_SB_MSG_SIZE)
 * @return TRUE if we were unable to unpack/verify the message.
 *
 * @sa SBN_PackMsg
 */
boolean SBN_UnpackMsg(void *SBNBuf, SBN_MsgSz_t *MsgSzPtr,
    SBN_MsgType_t *MsgTypePtr, SBN_CpuID_t *CpuIDPtr, void *Msg);

typedef struct SBN_IfOps_s SBN_IfOps_t;
typedef struct SBN_NetInterface_s SBN_NetInterface_t;

typedef struct {
    /** @brief The processor ID of this peer (MUST match the ProcessorID.) */
    uint32 ProcessorID;

    /** @brief The Spacecraft ID of this peer (MUST match the SpacecraftID.) */
    uint32 SpacecraftID;

    /** @brief A convenience pointer to the net that this peer belongs to. */
    SBN_NetInterface_t *Net;

    #ifdef SBN_SEND_TASK

    /**
     * @brief The ID of the task created to pend on the pipe and send messages
     * to the net as soon as they are read.
     */
    uint32 SendTaskID;

    #endif /* SBN_SEND_TASK */

    #ifdef SBN_RECV_TASK

    /**
     * @brief The ID of the task created to pend on the net and send messages
     * to the software bus as soon as they are read.
     */
    uint32 RecvTaskID; /* for mesh nets */

    #endif /* SBN_RECV_TASK */

    /** @brief The pipe ID used to read messages destined for the peer. */
    CFE_SB_PipeId_t Pipe;

    /** @brief The name of the pipe. */
    /* TODO: do I need this? */
    char PipeName[OS_MAX_API_NAME];

    /**
     * @brief A local table of subscriptions the peer has requested.
     * Includes one extra byte for a null termination.
     */
    /* TODO: do I need this? */
    SBN_Subs_t Subs[SBN_MAX_SUBS_PER_PEER + 1];

    OS_time_t LastSend, LastRecv;
    uint16 SendCnt, RecvCnt, SendErrCnt, RecvErrCnt, SubCnt;

    uint8 Connected; /**< @brief 0 = peer not conn, !0 == peer is connected */

    /** @brief generic blob of bytes for the module-specific data. */
    uint8 ModulePvt[128];
} SBN_PeerInterface_t;

/**
 * @brief When a module detects a (re)connection, a full subscription update
 * should be sent.
 *
 * @param Peer[in] The peer to which to send the subs.
 */
void SBN_SendLocalSubsToPeer(SBN_PeerInterface_t *Peer);

/**
 * @brief For a given network and processor ID, get the peer interface.
 *
 * @param Net[in] The network to check.
 * @param ProcessorID[in] The processor of the peer.
 *
 * @return A pointer to the peer interface structure.
 */
SBN_PeerInterface_t *SBN_GetPeer(SBN_NetInterface_t *Net, uint32 ProcessorID);

struct SBN_NetInterface_s {
    boolean Configured;

    uint8 ProtocolID;

    #ifdef SBN_SEND_TASK

    uint32 SendTaskID, SendMutex; /* for star nets */

    #endif /* SBN_SEND_TASK */

    #ifdef SBN_RECV_TASK

    uint32 RecvTaskID; /* for star nets */

    #endif /* SBN_RECV_TASK */

    SBN_IfOps_t *IfOps; /* convenience */

    uint16 PeerCnt;

    SBN_PeerInterface_t Peers[SBN_MAX_PEER_CNT];

    /** @brief generic blob of bytes, module-specific */
    uint8  ModulePvt[128];
};

/**
 * This structure contains function pointers to interface-specific versions
 * of the key SBN functions.  Every interface module must have an equivalent
 * structure that points to the approprate functions for that interface.
 */
struct SBN_IfOps_s {
    /** TODO: Document */
    int (*InitModule)(int MajorVersion, int MinorVersion, int Revision);

    /** TODO: Document */
    int (*LoadNet)(SBN_NetInterface_t *Net, const char *Address);
    /** TODO: Document */
    int (*LoadPeer)(SBN_PeerInterface_t *Peer, const char *Address);

    /**
     * Initializes the host interface.
     *
     * @param Net[in,out] Struct pointer describing a single interface
     * @return SBN_SUCCESS on successful initialization
     *         SBN_ERROR otherwise
     */
    int (*InitNet)(SBN_NetInterface_t *Host);

    /**
     * Initializes the peer interface.
     *
     * @param Peer[in,out] The peer interface to initialize
     * @return SBN_SUCCESS on successful initialization
     *         SBN_ERROR otherwise
     */
    int (*InitPeer)(SBN_PeerInterface_t *Peer);

    /**
     * SBN will poll any peer that does not have any messages to be sent
     * after a timeout period. This is for (re)establishing connections
     * and handshaking subscriptions.
     *
     * @param Peer[in] The peer to poll.
     *
     * @return SBN_SUCCESS on successful polling, SBN_ERROR otherwise.
     */
    int (*PollPeer)(SBN_PeerInterface_t *Peer);

    /**
     * Sends a message to a peer over the specified interface.
     * Both protocol and data message buffers are included in the parameters,
     * but only one is used at a time.  The data message buffer is used for
     * un/subscriptions and app messages.  The protocol message buffer is used
     * for announce and heartbeat messages/acks.
     *
     * @param Net[in] Interface data for the network where this peer lives.
     * @param Peer[in] Interface data describing the intended peer recipient.
     * @param MsgType[in] The SBN message type.
     * @param MsgSz[in] The size of the SBN message payload.
     * @param Payload[in] The SBN message payload.
     *
     * @return Number of bytes sent on success, -1 on error.
     */
    int (*Send)(SBN_PeerInterface_t *Peer, SBN_MsgType_t MsgType,
        SBN_MsgSz_t MsgSz, void *Payload);

    /**
     * Receives an individual message from the specified peer. Note, only
     * define this or the RecvFromNet method, not both!
     *
     * @param Net[in] Interface data for the network where this peer lives.
     * @param Peer[in] Interface data describing the intended peer recipient.
     * @param MsgTypePtr[out] SBN message type received.
     * @param MsgSzPtr[out] Payload size received.
     * @param CpuIDPtr[out] CpuID of the sender.
     * @param PayloadBuffer[out] Payload buffer
     *                      (pass in a buffer of CFE_SB_MAX_SB_MSG_SIZE)
     *
     * @return SBN_SUCCESS on success, SBN_ERROR on failure
     */
    int (*RecvFromPeer)(SBN_NetInterface_t *Net, SBN_PeerInterface_t *Peer,
        SBN_MsgType_t *MsgTypePtr, SBN_MsgSz_t *MsgSzPtr,
        SBN_CpuID_t *CpuIDPtr, void *PayloadBuffer);

    /**
     * Receives an individual message from the network.
     *
     * @param Net[in] Interface data for the network where this peer lives.
     * @param MsgTypePtr[out] SBN message type received.
     * @param MsgSzPtr[out] Payload size received.
     * @param CpuIDPtr[out] CpuID of the sender.
     * @param PayloadBuffer[out] Payload buffer
     *                      (pass in a buffer of CFE_SB_MAX_SB_MSG_SIZE)
     *
     * @return SBN_SUCCESS on success, SBN_ERROR on failure
     */
    int (*RecvFromNet)(SBN_NetInterface_t *Net, SBN_MsgType_t *MsgTypePtr,
        SBN_MsgSz_t *MsgSzPtr, SBN_CpuID_t *CpuIDPtr,
        void *PayloadBuffer);

    /**
     * Reports the status of the module.  The status can be in a module-specific
     * format but must be no larger than SBN_MOD_STATUS_MSG_SZ bytes (as
     * defined in sbn_platform_cfg.h).  The status packet is passed in
     * initialized (with message ID and size), the module fills it, and upon
     * return the SBN application sends the message over the software bus.
     *
     * @param Buffer[out] Status packet to fill
     * @param Peer[in] Peer to report status
     *
     * @return SBN_NOT_IMPLEMENTED if the module does not implement this
     *         function
     *         SBN_SUCCESS otherwise
     */
    int (*ReportModuleStatus)(SBN_ModuleStatusPacket_t *Buffer);

    /**
     * Unload a network. This will unload all associated peers as well.
     *
     * @param Net[in] Network to unload.
     *
     * @return  SBN_SUCCESS when the net is unloaded.
     *          SBN_ERROR if the net cannot be unloaded.
     *          SBN_NOT_IMPLEMENTED if the module does not implement this
     *          function.
     *
     * @sa LoadNet, LoadPeer, UnloadPeer
     */
    int (*UnloadNet)(SBN_NetInterface_t *Net);

    /**
     * Unload a peer.
     *
     * @param Peer[in] Peer to unload.
     *
     * @return  SBN_SUCCESS when the peer is unloaded.
     *          SBN_ERROR if the peer cannot be unloaded.
     *          SBN_NOT_IMPLEMENTED if the module does not implement this
     *          function.
     *
     * @sa LoadNet, LoadPeer, UnloadNet
     */
    int (*UnloadPeer)(SBN_PeerInterface_t *Peer);
};

/**
 * @brief Called by backend modules to signal that the connection has been
 * established and that the initial handshake should ensue.
 */
uint32 SBN_Connected(SBN_PeerInterface_t *Peer);

/**
 * @brief Called by backend modules to signal that the connection has been
 * lost.
 */
uint32 SBN_Disconnected(SBN_PeerInterface_t *Peer);

/**
 * @brief Used by modules to send protocol-specific messages
 * (particularly UDP which needs to send announcement/heartbeat msgs.)
 */
int SBN_SendNetMsg(SBN_MsgType_t MsgType, SBN_MsgSz_t MsgSz,
    void *Msg, SBN_PeerInterface_t *Peer);

#endif /* _sbn_interfaces_h_ */
