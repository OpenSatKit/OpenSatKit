#ifndef _sbn_msg_h_
#define _sbn_msg_h_

#include "sbn_msgdefs.h"
#include "sbn_platform_cfg.h"
#include "sbn_constants.h"
#include "cfe.h"

/** @brief uint8 NetIdx */
#define SBN_CMD_NET_LEN CFE_SB_CMD_HDR_SIZE + 1

/** @brief uint8 NetIdx, uint8 PeerIdx */
#define SBN_CMD_PEER_LEN CFE_SB_CMD_HDR_SIZE + 2

/** @brief uint8 Enabled, uint8 DefaultFlag */
#define SBN_CMD_REMAPCFG_LEN CFE_SB_CMD_HDR_SIZE + 2

/** @brief uint32 ProcessorID, CFE_SB_MsgId_t FromMID, CFE_SB_MsgId_t ToMID */
#define SBN_CMD_REMAPADD_LEN CFE_SB_CMD_HDR_SIZE + 8

/** @brief uint32 ProcessorID, CFE_SB_MsgId_t FromMID */
#define SBN_CMD_REMAPDEL_LEN CFE_SB_CMD_HDR_SIZE + 6

/** @brief uint8 CC, uint16 CmdCnt, uint16 CmdErrCnt, uint16 SubCnt,
 * uint16 NetCnt
 */
#define SBN_HK_LEN (CFE_SB_TLM_HDR_SIZE + sizeof(uint8) + (sizeof(uint16) * 4))

/** @brief uint8 CC, uint16 SubCnt,
 * CFE_SB_MsgId_t Subs[SBN_MAX_SUBS_PER_PEER]
 */
#define SBN_HKMYSUBS_LEN (CFE_SB_TLM_HDR_SIZE + sizeof(uint8) + sizeof(uint16) + SBN_MAX_SUBS_PER_PEER * sizeof(CFE_SB_MsgId_t))

/** @brief uint8 CC, uint16 NetIdx, uint16 PeerIdx, uint16 SubCnt,
 * CFE_SB_MsgId_t Subs[SBN_MAX_SUBS_PER_PEER]
 */
#define SBN_HKPEERSUBS_LEN (CFE_SB_TLM_HDR_SIZE + sizeof(uint8) + sizeof(uint16) * 3 + SBN_MAX_SUBS_PER_PEER * sizeof(CFE_SB_MsgId_t))

/** @brief uint8 CC, uint8 SubCnt,
 * uint32 ProcessorID, OS_time_t LastSend, OS_time_t LastRecv,
 * uint16 SendCnt, uint16 RecvCnt, uint16 SendErrCnt, uint16 RecvErrCnt,
 */
#define SBN_HKPEER_LEN (CFE_SB_TLM_HDR_SIZE + sizeof(uint8) * 2 + sizeof(OS_time_t) * 2 + sizeof(uint16) * 4)

/** @brief uint8 CC, uint8 ProtocolID, uint16 PeerCnt
 */
#define SBN_HKNET_LEN (CFE_SB_TLM_HDR_SIZE + sizeof(uint8) * 2 + sizeof(uint16))

/**
 * @brief Module status response packet structure
 */
typedef struct {
    /**
     * This is a struct that will be sent (as-is) in a response to a HK command.
     */
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    /** @brief The Protocol ID being queried. */
    uint32  ProtocolID;
    /** @brief The module status as returned by the module. */
    uint8   ModuleStatus[SBN_MOD_STATUS_MSG_SZ];
} SBN_ModuleStatusPacket_t;

typedef struct {
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32  TblId;
} SBN_ReloadTblCmd_t;

#endif /* _sbn_msg_h_ */
