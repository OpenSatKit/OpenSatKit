/* LESSON 2 - Solution
** Purpose: Implement the MSGLOG_Class methods
**
** Notes:
**   1. The log/playback functionality is for demonstration purposes and 
**      the logic is kept simple so users can focus on learning developing 
**      apps using the OSK C Framework.
**   2. Logging and playback can't be enabled at the same time. If a command
**      to start a playback is received when logging is in progress, the
**      logging will be stopped and a playback will be started. The same
**      occurs in reverse when a playback is in progress and a command to 
**      start a message log is received. Neither case is considered an error.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Application Developers Guide
**   2. cFS Application Developer's Guide
*/

/*
** Include Files:
*/

#include <string.h>

#include "app_cfg.h"
#include "initbl.h"
#include "msglog.h"


/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macro */



/**********************/
/** Global File Data **/
/**********************/

static MSGLOG_Class*  MsgLog = NULL;


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static void StopLog(void);
static void StopPlaybk(void);


/******************************************************************************
** Function: MSGLOG_Constructor
**
*/
void MSGLOG_Constructor(MSGLOG_Class*  MsgLogPtr, INITBL_Class* IniTbl)
{
 
   MsgLog = MsgLogPtr;

   CFE_PSP_MemSet((void*)MsgLog, 0, sizeof(MSGLOG_Class));
 
   MsgLog->IniTbl = IniTbl;
   

} /* End MSGLOG_Constructor */


/******************************************************************************
** Function:  MSGLOG_ResetStatus
**
*/
void MSGLOG_ResetStatus()
{
 
   /* Nothing to do */
   
} /* End MSGLOG_ResetStatus() */


/******************************************************************************
** Function: MSGLOG_StartLogCmd
**
** Notes:
**   1. See file prologue for logging/playback logic.
*/
boolean MSGLOG_StartLogCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
      
   if (MsgLog->LogEna){
      StopLog();
   }
   
   if (MsgLog->PlaybkEna){
      StopPlaybk();
   }

   MsgLog->LogEna = TRUE;

   CFE_EVS_SendEvent (MSGLOG_START_LOG_CMD_EID, CFE_EVS_INFORMATION,
                      "Received Start Log command");
   
   return TRUE;

} /* End MSGLOG_StartLogCmd() */


/******************************************************************************
** Function: MSGLOG_StopLogCmd
**
*/
boolean MSGLOG_StopLogCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   if (MsgLog->LogEna) {
         
      StopLog();
      
   }
   
   CFE_EVS_SendEvent (MSGLOG_STOP_LOG_CMD_EID, CFE_EVS_INFORMATION,
                      "Received Stop Log command");
   
   return TRUE;
   
} /* End MSGLOG_StopLogCmd() */


/******************************************************************************
** Function: MSGLOG_StartPlaybkCmd
**
** Notes:
**   1. See file prologue for logging/playback logic.
*/
boolean MSGLOG_StartPlaybkCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
  

   if (MsgLog->LogEna){
      StopLog();
   }

   MsgLog->PlaybkEna = TRUE;
   
   CFE_EVS_SendEvent (MSGLOG_START_PLAYBK_CMD_EID, CFE_EVS_INFORMATION,
                      "Received Start Playback command");
      
   return TRUE;
   
} /* End MSGLOG_StartPlaybkCmd() */


/******************************************************************************
** Function: MSGLOG_StopPlaybkCmd
**
*/
boolean MSGLOG_StopPlaybkCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   if (MsgLog->PlaybkEna) {
         
      StopPlaybk();
      
   }
 
   CFE_EVS_SendEvent (MSGLOG_STOP_LOG_CMD_EID, CFE_EVS_INFORMATION,
                      "Received Stop Log command");
   
   return TRUE;
   
} /* End MSGLOG_StopPlaybkCmd() */



/******************************************************************************
** Function: StopLog
**
** Notes:
**   1. Assumes caller checked if log was in progress
*/
static void StopLog(void)
{
   
   MsgLog->LogEna = FALSE;
   
   CFE_EVS_SendEvent (MSGLOG_STOP_LOG_CMD_EID, CFE_EVS_INFORMATION,
                      "Logging stopped");

}/* End StopLog() */


/******************************************************************************
** Function: StopPlaybk
**
** Notes:
**   1. Assumes caller checked if playback was in progress. 
**   2. Clears playback state data and sends a final playback telemetry packet
*/
static void StopPlaybk(void)
{
   
   MsgLog->PlaybkEna = FALSE;
   
   CFE_EVS_SendEvent (MSGLOG_STOP_PLAYBK_CMD_EID, CFE_EVS_INFORMATION,
                      "Playback stopped");

}/* End StopPlaybk() */
