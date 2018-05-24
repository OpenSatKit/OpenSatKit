/* 
** Purpose: Manage the Packet Table that defines which packets will be sent from the
**          software bus to a socket.
**
** Notes:
**   None
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

#ifndef _pktmgr_
#define _pktmgr_

/*
** Includes
*/

#include "app_cfg.h"
#include "pkttbl.h"


#define PKTMGR_IP_STR_LEN  16

/*
** Event Message IDs
*/
#define PKTMGR_SOCKET_SEND_ERR_EID                  (PKTMGR_BASE_EID +  0)
#define PKTMGR_LOAD_TBL_SUBSCRIBE_ERR_EID           (PKTMGR_BASE_EID +  1)
#define PKTMGR_LOAD_TBL_INFO_EID                    (PKTMGR_BASE_EID +  2)
#define PKTMGR_LOAD_TBL_ERR_EID                     (PKTMGR_BASE_EID +  3)
#define PKTMGR_LOAD_TBL_ENTRY_SUBSCRIBE_ERR_EID     (PKTMGR_BASE_EID +  4)
#define PKTMGR_LOAD_TBL_ENTRY_INFO_EID              (PKTMGR_BASE_EID +  5)
#define PKTMGR_TLM_OUTPUT_ENA_INFO_EID              (PKTMGR_BASE_EID +  6)
#define PKTMGR_TLM_OUTPUT_ENA_SOCKET_ERR_EID        (PKTMGR_BASE_EID +  7)
#define PKTMGR_ADD_PKT_INFO_EID                     (PKTMGR_BASE_EID +  8)
#define PKTMGR_ADD_PKT_NO_FREE_ENTRY_ERR_EID        (PKTMGR_BASE_EID +  9)
#define PKTMGR_ADD_PKT_DUPLICATE_ENTRY_EID          (PKTMGR_BASE_EID + 10)
#define PKTMGR_REMOVE_PKT_INFO_EID                  (PKTMGR_BASE_EID + 11)
#define PKTMGR_REMOVE_PKT_SB_ERR_EID                (PKTMGR_BASE_EID + 12)
#define PKTMGR_REMOVE_PKT_NOT_FOUND_ERR_EID         (PKTMGR_BASE_EID + 13)
#define PKTMGR_REMOVE_ALL_PKTS_UNSUBSCRIBE_ERR_EID  (PKTMGR_BASE_EID + 14)
#define PKTMGR_REMOVE_ALL_PKTS_INFO_EID             (PKTMGR_BASE_EID + 15)
#define PKTMGR_REMOVE_ALL_PKTS_ERR_EID              (PKTMGR_BASE_EID + 16)

#define PKTMGR_TOTAL_EID  17

/*
** Type Definitions
*/



/******************************************************************************
** Packet Manager Class
*/

typedef struct {

   CFE_SB_PipeId_t   TlmPipe;
   int               TlmSockId;
   char              TlmDestIp[PKTMGR_IP_STR_LEN];
   boolean           DownlinkOn;
   boolean           SuppressSend;

   PKTTBL_Struct  Tbl;

} PKTMGR_Class;


/******************************************************************************
** Command Functions
*/

typedef struct
{

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    DestIp[PKTMGR_IP_STR_LEN];

} PKTMGR_EnableOutputCmdParam;
#define PKKTMGR_ENABLE_OUTPUT_CMD_DATA_LEN  (sizeof(PKTMGR_EnableOutputCmdParam) - CFE_SB_CMD_HDR_SIZE)


typedef struct
{

   uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_SB_MsgId_t     StreamId;
   CFE_SB_Qos_t       Qos;
   uint8              BuffLim;

}  OS_PACK PKTMGR_AddPktCmdParam;
#define PKKTMGR_ADD_PKT_CMD_DATA_LEN  (sizeof(PKTMGR_AddPktCmdParam) - CFE_SB_CMD_HDR_SIZE)


typedef struct
{

   uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_SB_MsgId_t     StreamId;

}  PKTMGR_RemovePktCmdParam;
#define PKKTMGR_REMOVE_PKT_CMD_DATA_LEN  (sizeof(PKTMGR_RemovePktCmdParam) - CFE_SB_CMD_HDR_SIZE)


/*
** Exported Functions
*/

/******************************************************************************
** Function: PKTMGR_Constructor
**
** Construct a PKTMGR object. All table entries are cleared and the LoadTbl()
** function should be used to load an initial table.
**
** Notes:
**   1. This must be called prior to any other function.
**   2. Decoupling the initial table load gives an app flexibility in file
**      management during startup.
**
*/
void PKTMGR_Constructor(PKTMGR_Class *PktMgrPtr, char* PipeName, uint16 PipeDepth);


/******************************************************************************
** Function: PKTMGR_GetTblPtr
**
** Return a pointer to the packet table.
**
** Notes:
**   1. Function signature must match PKTTBL_GetTblPtr
**
*/
const PKTTBL_Struct* PKTMGR_GetTblPtr(void);


/******************************************************************************
** Function: PKTMGR_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void PKTMGR_ResetStatus(void);


/******************************************************************************
** Function: PKTMGR_OutputTelemetry
**
** If downlink is enabled and output hasn't been supressed it sends all of the
** SB packets on the telemetry input pipe out the socket.
**
*/
void PKTMGR_OutputTelemetry(void);


/******************************************************************************
** Function: PKTMGR_LoadTbl
**
** Unsubscribes to all current SB telemetry, flushes the telemetry input pipe,
** loads the entire new table and subscribes to each message.
**
** Notes:
**   1. No validity checks are performed on the table data.
**   2. Function signature must match PKTTBL_LoadTbl
**
*/
boolean PKTMGR_LoadTbl(PKTTBL_Struct* NewTbl);


/******************************************************************************
** Function: PKTMGR_LoadTblEntry
**
** Load a single message table entry
**
** Notes:
**   1. Range checking is not performed on the parameters.
**   2. Function signature must match PKTTBL_LoadTblEntry
**
*/
boolean PKTMGR_LoadTblEntry(uint16 EntryId, PKTTBL_Entry* NewEntry);


/******************************************************************************
** Function: PKTMGR_EnableOutputCmd
**
** The commanded IP is always saved and downlink suppression is turned off. If
** downlink is disabled then a new socket is created with the new IP and
** downlink is turned on.
**
*/
boolean PKTMGR_EnableOutputCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: PKTMGR_AddPktCmd
**
** Add a packet to the table and subscribe for it on the SB.
*/
boolean PKTMGR_AddPktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: PKTMGR_RemovePktCmd
**
** Remove a packet from the table and unsubscribe from receiving it on the SB.
**
** Notes:
**   1. Don't consider trying to remove an non-existent entry an error
*/
boolean PKTMGR_RemovePktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: PKTMGR_RemoveAllPktsCmd
**
** Remove all packets from the table and unsubscribe from receiving them on the
** SB.
*/
boolean PKTMGR_RemoveAllPktsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _pktmgr_ */
