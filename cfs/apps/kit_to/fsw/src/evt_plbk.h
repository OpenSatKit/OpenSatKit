/* 
** Purpose: Telemeter (playback) the contents of the current cFE event message log.
**
** Notes:
**   1. Initially motivated by a scenario where ground tools for dumping
**      and displaying the event log were not available. 
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

#ifndef _evt_plbk_
#define _evt_plbk_

/*
** Includes
*/

#include "app_cfg.h"

/*
** Event Message IDs
*/
#define EVT_PLBK_LOG_READ_ERR_EID        (EVT_PLBK_BASE_EID +  0)
#define EVT_PLBK_SENT_WRITE_LOG_CMD_EID  (EVT_PLBK_BASE_EID +  1)
#define EVT_PLBK_STOP_CMD_EID            (EVT_PLBK_BASE_EID +  2)
#define EVT_PLBK_LOG_HDR_TYPE_ERR_EID    (EVT_PLBK_BASE_EID +  3)
#define EVT_PLBK_LOG_HDR_READ_ERR_EID    (EVT_PLBK_BASE_EID +  4)
#define EVT_PLBK_LOG_OPEN_ERR_EID        (EVT_PLBK_BASE_EID +  5)
#define EVT_PLBK_LOG_NONEXISTENT_EID     (EVT_PLBK_BASE_EID +  6)
#define EVT_PLBK_READ_LOG_SUCCESS_EID    (EVT_PLBK_BASE_EID +  7)
#define EVT_PLBK_CFG_CMD_ERR_EID         (EVT_PLBK_BASE_EID +  8)
#define EVT_PLBK_CFG_CMD_EID             (EVT_PLBK_BASE_EID +  9)

/*
** Type Definitions
*/


/******************************************************************************
** Command Packets
**
** - Use separate configuration command so start/stop commands do not require
**   any parameters
** - No limit check performed on HkCyclesPerPkt becuase no harmful affects it
**   unreasonable value sent. 
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    EvsLogFilename[CFE_MISSION_MAX_PATH_LEN];   /* Filename to use when command write EVS log file */
   uint16  HkCyclesPerPkt;                             /* Number of HK request cycles between event log telemetry packets */
   
} EVT_PLBK_ConfigCmdMsg;
#define EVT_PLBK_CONFIG_CMD_DATA_LEN  ((sizeof(EVT_PLBK_ConfigCmdMsg) - CFE_SB_CMD_HDR_SIZE))

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

} EVT_PLBK_NoParamCmdMsg;
#define EVT_PLBK_NO_PARAM_CMD_DATA_LEN  ((sizeof(EVT_PLBK_NoParamCmdMsg) - CFE_SB_CMD_HDR_SIZE))
#define EVT_PLBK_START_CMD_DATA_LEN (EVT_PLBK_NO_PARAM_CMD_DATA_LEN)
#define EVT_PLBK_STOP_CMD_DATA_LEN  (EVT_PLBK_NO_PARAM_CMD_DATA_LEN)


/******************************************************************************
** Telemetry Packets
*/


typedef struct {

   CFE_TIME_SysTime_t Time;
   char    AppName[CFE_MISSION_MAX_API_LEN];
   uint16  EventId;
   uint16  EventType;
   char    Message[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

   
} EVT_PLBK_TlmEvent;

typedef struct {

   uint8   Header[CFE_SB_TLM_HDR_SIZE];

   char    EvsLogFilename[CFE_MISSION_MAX_PATH_LEN];
   uint16  EventCnt;
   uint16  PlbkIdx;
   
   EVT_PLBK_TlmEvent Event[EVT_PLBK_EVENTS_PER_TLM_MSG];

} OS_PACK EVT_PLBK_TlmMsg;

#define EVT_PLBK_TLM_MSG_LEN sizeof (EVT_PLBK_TlmMsg)


/******************************************************************************
** Event Playback Class
**
** Since KIT_TO is non-flight without memory constraints and the default event 
** log message count is 20, I took the simple approach to just read the entire
** log into memory. Using default values it's less than 3K bytes.
**
*/

typedef struct {

   boolean Loaded;
   EVT_PLBK_TlmEvent Tlm;

} EVT_PLBK_EventLogMsg;

typedef struct {

   uint16 EventCnt;    /* Number of entries loaded from log file. */
   uint16 PlbkIdx;     /* Currrent index used during playback */
   EVT_PLBK_EventLogMsg Msg[CFE_PLATFORM_EVS_LOG_MAX];

} EVT_PLBK_EventLog;

typedef struct {

   boolean  Enabled;
   boolean  LogFileCopied;
   uint16   EvsLogFileOpenAttempts;  /* Number of execution cycle attempts to open log file after write log commanded */  
   
   uint16   HkCyclePeriod;       /* Number of HK request cycles between event log telemetry packets */
   uint16   HkCycleCount;        /* Current count of HK cycles between telemetry packets sent */

   CFE_TIME_SysTime_t  StartTime;
   
   char EvsLogFilename[CFE_MISSION_MAX_PATH_LEN];
      
   EVT_PLBK_EventLog  EventLog;
   
   EVT_PLBK_TlmMsg    TlmMsg;

} EVT_PLBK_Class;


/*
** Exported Functions
*/

/******************************************************************************
** Function: EVT_PLBK_Constructor
**
** Construct an EVT_PLBK object. 
**
** Notes:
**   1. This must be called prior to any other function.
**   2. Disabled by default.
**
*/
void EVT_PLBK_Constructor(EVT_PLBK_Class* EvtPbPtr, CFE_SB_MsgId_t TlmMsgId, 
                          uint16 HkCyclePeriod, char* EvsLogFilename,
                          CFE_SB_MsgId_t CfeEvsCmdMid);


/******************************************************************************
** Function: EVT_PLBK_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void EVT_PLBK_ResetStatus(void);


/******************************************************************************
** Function: EVT_PLBK_Execute
**
** If enabled create and send a new telmetry packet with the next set of 
** event messages.
**
** Notes:
**   1. It's assumed this function is called during the main apps HK request
**      execution cycle and the HkCyclePeriod determines how many HK cycles
**      should be between telemetry packet generations.
**   2. The current event log is captured (written to a file) when the start
**      playback command is received and this function continually loops
**      through the log file. 
**
*/
void EVT_PLBK_Execute(void);


/******************************************************************************
** Function: EVT_PLBK_ConfigCmd
**
** Configure the behavior of playbacks. See command parameters definitions
** for details.
**
*/
boolean EVT_PLBK_ConfigCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: EVT_PLBK_StartCmd
**
** Command EVS to write the current event log to a file and start playing back
** the messages.
**
*/
boolean EVT_PLBK_StartCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: EVT_PLBK_StopCmd
**
** Stop a playback if one is in progress.
**
*/
boolean EVT_PLBK_StopCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _evt_plbk_ */
