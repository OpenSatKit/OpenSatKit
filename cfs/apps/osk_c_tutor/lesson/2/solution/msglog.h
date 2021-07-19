/* LESSON 2 - Solution
** Purpose: Manage logging message headers to a text file
**          and playing them back in telemetry
**
** Notes:
**   1. This demo app serves as the final result of a 
**      Code-As-You-Go(CAYG) series of self-guided exercises. 
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Application Developers Guide
**   2. cFS Application Developer's Guide
*/

#ifndef _msglog_
#define _msglog_

/*
** Includes
*/

#include "app_cfg.h"



/***********************/
/** Macro Definitions **/
/***********************/

/* 
** Number of hex characters in a primary CCSDS header
** - Even number of bytes since hex char array used in tlm pkt definition
*/


/*
** Event Message IDs
*/

#define MSGLOG_PERIODIC_CMD_EID     (MSGLOG_BASE_EID + 0)
#define MSGLOG_START_LOG_CMD_EID    (MSGLOG_BASE_EID + 1)
#define MSGLOG_STOP_LOG_CMD_EID     (MSGLOG_BASE_EID + 2)
#define MSGLOG_START_PLAYBK_CMD_EID (MSGLOG_BASE_EID + 3)
#define MSGLOG_STOP_PLAYBK_CMD_EID  (MSGLOG_BASE_EID + 4)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16  MsgId;

} MSGLOG_StartLogCmdMsg;
#define MSGLOG_START_LOG_CMD_DATA_LEN     ((sizeof(MSGLOG_StartLogCmdMsg) - CFE_SB_CMD_HDR_SIZE))
#define MSGLOG_STOP_LOG_CMD_DATA_LEN      PKTUTIL_NO_PARAM_CMD_DATA_LEN

#define MSGLOG_START_PLAYBK_CMD_DATA_LEN  PKTUTIL_NO_PARAM_CMD_DATA_LEN
#define MSGLOG_STOP_PLAYBK_CMD_DATA_LEN   PKTUTIL_NO_PARAM_CMD_DATA_LEN

#define MSGLOG_PERIODIC_CMD_DATA_LEN      PKTUTIL_NO_PARAM_CMD_DATA_LEN


/******************************************************************************
** Telmetery Packets
*/

/* 
** Playback packet that is sent while a playback is active. One packet header
** entry sent in each packet. 
*/



/******************************************************************************
** MSGLOG_Class
*/

typedef struct {

   /*
   ** Framework References
   */
   
   INITBL_Class*   IniTbl;

   
   /*
   ** Telemetry Packets
   */
   


   /*
   ** Class State Data
   */

   boolean  LogEna;
   
   boolean  PlaybkEna;
    
   /*
   ** Child Objects
   */


   
} MSGLOG_Class;



/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: MSGLOG_Constructor
**
** Initialize the packet log to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void MSGLOG_Constructor(MSGLOG_Class* MsgLogPtr, INITBL_Class* IniTbl);


/******************************************************************************
** Function: MSGLOG_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void MSGLOG_ResetStatus(void);



/******************************************************************************
** Function: MSGLOG_StartLogCmd
**
*/
boolean MSGLOG_StartLogCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: MSGLOG_StopLogCmd
**
*/
boolean MSGLOG_StopLogCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: MSGLOG_StartPlaybkCmd
**
*/
boolean MSGLOG_StartPlaybkCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: MSGLOG_StopPlaybkCmd
**
*/
boolean MSGLOG_StopPlaybkCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _msglog_ */

