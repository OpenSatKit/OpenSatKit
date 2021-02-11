/* 
** Purpose: Manage the Packet Table that defines which packets will be sent from the
**          software bus to a socket.
**
** Notes:
**   1. This has some of the features of a flight app such as packet filtering but it
**      would need design/code reviews to transition it to a flight mission. For starters
**      it uses UDP sockets and it doesn't regulate output bit rates. 
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
#define PKTMGR_SOCKET_SEND_ERR_EID               (PKTMGR_BASE_EID +  0)
#define PKTMGR_LOAD_TBL_SUBSCRIBE_ERR_EID        (PKTMGR_BASE_EID +  1)
#define PKTMGR_LOAD_TBL_INFO_EID                 (PKTMGR_BASE_EID +  2)
#define PKTMGR_LOAD_TBL_ERR_EID                  (PKTMGR_BASE_EID +  3)
#define PKTMGR_LOAD_TBL_ENTRY_SUBSCRIBE_ERR_EID  (PKTMGR_BASE_EID +  4)
#define PKTMGR_LOAD_TBL_ENTRY_INFO_EID           (PKTMGR_BASE_EID +  5)
#define PKTMGR_TLM_OUTPUT_ENA_INFO_EID           (PKTMGR_BASE_EID +  6)
#define PKTMGR_TLM_OUTPUT_ENA_SOCKET_ERR_EID     (PKTMGR_BASE_EID +  7)
#define PKTMGR_ADD_PKT_SUCCESS_EID               (PKTMGR_BASE_EID +  8)
#define PKTMGR_ADD_PKT_ERROR_EID                 (PKTMGR_BASE_EID +  9)
#define PKTMGR_REMOVE_PKT_SUCCESS_EID            (PKTMGR_BASE_EID + 10)
#define PKTMGR_REMOVE_PKT_ERROR_EID              (PKTMGR_BASE_EID + 11)
#define PKTMGR_REMOVE_ALL_PKTS_SUCCESS_EID       (PKTMGR_BASE_EID + 12)
#define PKTMGR_REMOVE_ALL_PKTS_ERROR_EID         (PKTMGR_BASE_EID + 13)
#define PKTMGR_DESTRUCTOR_INFO_EID               (PKTMGR_BASE_EID + 14)
#define PKTMGR_UPDATE_FILTER_CMD_SUCCESS_EID     (PKTMGR_BASE_EID + 15)
#define PKTMGR_UPDATE_FILTER_CMD_ERR_EID         (PKTMGR_BASE_EID + 16)
#define PKTMGR_DEBUG_EID                         (PKTMGR_BASE_EID + 17)


/*
** Type Definitions
*/


/******************************************************************************
** Command Packets
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    DestIp[PKTMGR_IP_STR_LEN];

} PKTMGR_EnableOutputCmdMsg;
#define PKKTMGR_ENABLE_OUTPUT_CMD_DATA_LEN  (sizeof(PKTMGR_EnableOutputCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8                CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_SB_MsgId_t       StreamId;
   CFE_SB_Qos_t         Qos;
   uint8                BufLim;
   uint16               FilterType;
   PktUtil_FilterParam  FilterParam;

}  OS_PACK PKTMGR_AddPktCmdMsg;
#define PKKTMGR_ADD_PKT_CMD_DATA_LEN  (sizeof(PKTMGR_AddPktCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_SB_MsgId_t     StreamId;

}  PKTMGR_RemovePktCmdMsg;
#define PKKTMGR_REMOVE_PKT_CMD_DATA_LEN  (sizeof(PKTMGR_RemovePktCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_SB_MsgId_t     StreamId;

}  PKTMGR_SendPktTblTlmCmdMsg;
#define PKKTMGR_SEND_PKT_TBL_TLM_CMD_DATA_LEN  (sizeof(PKTMGR_SendPktTblTlmCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8                CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_SB_MsgId_t       StreamId;
   uint16               FilterType;
   PktUtil_FilterParam  FilterParam;

}  OS_PACK PKTMGR_UpdateFilterCmdMsg;
#define PKKTMGR_UPDATE_FILTER_CMD_DATA_LEN  (sizeof(PKTMGR_UpdateFilterCmdMsg) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   CFE_SB_MsgId_t  StreamId;
   CFE_SB_Qos_t    Qos;
   uint16          BufLim;

   uint16               FilterType;
   PktUtil_FilterParam  FilterParam;

} OS_PACK PKTMGR_PktTlm;

#define PKTMGR_PKT_TLM_LEN sizeof (PKTMGR_PktTlm)


/******************************************************************************
** Packet Manager Class
*/

/*
** Packet Manager Statistics
** - Stats are computed over an interval of PKTMGR_COMPUTE_STATS_INTERVAL_MS
*/
typedef enum {

   PKTMGR_STATS_INIT_CYCLE    = 1,
   PKTMGR_STATS_INIT_INTERVAL,
   PKTMGR_STATS_VALID
   
} PKTMGR_StatsState;

typedef struct {

   uint16  InitCycles;         /* 0: Init done, >0: Number of remaining init cycles  */  
   
   double  OutputTlmInterval;  /* ms between calls to PKTMGR_OutputTelemetry()    */  
   double  IntervalMilliSecs;  /* Number of ms in the current computational cycle */
   uint32  IntervalPkts;
   uint32  IntervalBytes;
   
   CFE_TIME_SysTime_t PrevTime; 
   double  PrevIntervalAvgPkts;
   double  PrevIntervalAvgBytes;
   
   double  AvgPktsPerSec;
   double  AvgBytesPerSec;
   
   PKTMGR_StatsState State;
   
} PKTMGR_Stats;


typedef struct {

   CFE_SB_PipeId_t   TlmPipe;
   int               TlmSockId;
   char              TlmDestIp[PKTMGR_IP_STR_LEN];

   boolean           DownlinkOn;
   boolean           SuppressSend;
   PKTMGR_Stats      Stats;

   PKTTBL_Tbl        Tbl;

   PKTMGR_PktTlm     PktTlm;

} PKTMGR_Class;

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
** Function:  PKTMGR_InitStats
**
** OutputTlmInterval - Number of ms between calls to PKTMGR_OutputTelemetry()
**                     If zero retain the last interval value
** InitDelay         - Number of ms to delay starting stats computation
*/
void PKTMGR_InitStats(uint16 OutputTlmInterval, uint16 InitDelay);


/******************************************************************************
** Function: PKTMGR_GetTblPtr
**
** Return a pointer to the packet table.
**
** Notes:
**   1. Function signature must match PKTTBL_GetTblPtr
**
*/
const PKTTBL_Tbl* PKTMGR_GetTblPtr(void);


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
** If downlink is enabled and output hasn't been suppressed it sends all of the
** SB packets on the telemetry input pipe out the socket.
**
*/
uint16 PKTMGR_OutputTelemetry(void);


/******************************************************************************
** Function: PKTMGR_LoadTbl
**
** Unsubscribe from all current SB telemetry, flushes the telemetry input pipe,
** loads the entire new table and subscribes to each message.
**
** Notes:
**   1. No validity checks are performed on the table data.
**   2. Function signature must match PKTTBL_LoadTbl
**
*/
boolean PKTMGR_LoadTbl(PKTTBL_Tbl* NewTbl);


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
boolean PKTMGR_LoadTblEntry(uint16 PktIdx, PKTTBL_Pkt* PktArray);


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
**
** Notes:
**   1. Command rejected if table has existing entry for thecommanded Stream ID
**   2. Only update the table if the software bus subscription successful
** 
*/
boolean PKTMGR_AddPktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: PKTMGR_RemoveAllPktsCmd
**
** Notes:
**   1. The cFE to_lab code unsubscribes the command and send HK MIDs. I'm not
**      sure why this is done and I'm not sure how the command is used. This 
**      command is intended to help manage TO telemetry packets.
*/
boolean PKTMGR_RemoveAllPktsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


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
** Function: PKTMGR_SendPktTblTlmCmd
**
** Send a telemetry packet containg the packet table entry for the commanded
** Stream ID.
**
*/
boolean PKTMGR_SendPktTblTlmCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: PKTMGR_UpdateFilterCmd
**
** Notes:
**   1. Command rejected if AppId packet entry has not been loaded 
**   2. The filter type is verified but the filter parameter values are not 
** 
*/
boolean PKTMGR_UpdateFilterCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _pktmgr_ */
