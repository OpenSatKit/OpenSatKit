/* 
** Purpose: Implement the Spacecraft Simulator App
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

/*
** Includes
*/

#include <string.h>
#include "scsim_app.h"

/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ  (&(ScSimApp.IniTbl))
#define  CMDMGR_OBJ  (&(ScSimApp.CmdMgr))
#define  TBLMGR_OBJ  (&(ScSimApp.TblMgr))
#define  SCSIM       (&(ScSimApp.ScSim))
#define  SCSIM_TBL   (&(ScSimApp.ScSimTbl))


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(uint32* AppMainPerfId);
static void ProcessCommandPipe(CFE_SB_MsgId_t CmdMid, CFE_SB_MsgId_t ExecuteMid, CFE_SB_MsgId_t SendHkMid);
static void SendHousekeepingPkt(void);

/**********************/
/** File Global Data **/
/**********************/

/* 
** Must match DECLARE ENUM() declaration in app_cfg.h
** Defines "static INILIB_CfgEnum IniCfgEnum"
*/

DEFINE_ENUM(Config,APP_CONFIG)  


/*****************/
/** Global Data **/
/*****************/

SCSIM_APP_Class  ScSimApp;


/******************************************************************************
** Function: SCSIM_AppMain
**
*/
void SCSIM_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;
   uint32 AppMainPerfId;
   CFE_SB_MsgId_t CmdMid     = CFE_SB_INVALID_MSG_ID;
   CFE_SB_MsgId_t ExecuteMid = CFE_SB_INVALID_MSG_ID;
   CFE_SB_MsgId_t SendHkMid  = CFE_SB_INVALID_MSG_ID;
   
   Status = CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL,0,0);

   /*
   ** Perform application specific initialization
   */
   if (Status == CFE_SUCCESS) {
      
      Status = InitApp(&AppMainPerfId); /* Performs initial CFE_ES_PerfLogEntry() call */
   
   }

   /*
   ** At this point many flight apps use CFE_ES_WaitForStartupSync() to
   ** synchronize their startup timing with other apps. This is not
   ** needed for this simple app.
   */

   if (Status == CFE_SUCCESS) {
   
      CmdMid     = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_MID);
      ExecuteMid = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_EXECUTE_MID);
      SendHkMid  = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_SEND_HK_MID);
   
      RunStatus = CFE_ES_APP_RUN;

   }
   
   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus)) {

      /*
      ** ProcessCommands() pends indefinitely. The scheduler sends
      ** a message to execute a simulation step.
      */
	  
      CFE_ES_PerfLogExit(AppMainPerfId);
      ProcessCommandPipe(CmdMid, ExecuteMid, SendHkMid);
      CFE_ES_PerfLogEntry(AppMainPerfId);

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("SCSIM Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(SCSIM_APP_EXIT_EID, CFE_EVS_CRITICAL, "SCSIM Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(AppMainPerfId);
   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of SCSIM_Main() */


/******************************************************************************
** Function: SCSIM_NoOpCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean SCSIM_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (SCSIM_APP_CMD_NOOP_EID, CFE_EVS_INFORMATION,
                      "No operation command received for SCSIM version %d.%d.%d",
                      SCSIM_MAJOR_VER, SCSIM_MINOR_VER, SCSIM_PLATFORM_REV);

   return TRUE;


} /* End SCSIM_NoOpCmd() */


/******************************************************************************
** Function: SCSIM_ResetAppCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean SCSIM_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   SCSIMTBL_ResetStatus();
   SCSIM_ResetStatus();

   return TRUE;

} /* End SCSIM_ResetAppCmd() */


/******************************************************************************
** Function: SendHousekeepingPkt
**
*/
static void SendHousekeepingPkt(void)
{

   /* Good design practice in case app expands to more than one table */
   const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);

   
   /*
   ** CMDMGR Data
   */

   ScSimApp.HkPkt.ValidCmdCnt   = ScSimApp.CmdMgr.ValidCmdCnt;
   ScSimApp.HkPkt.InvalidCmdCnt = ScSimApp.CmdMgr.InvalidCmdCnt;

   
   /*
   ** SCSIM/SCSIMTBL Data
   ** - At a minimum all OBJECT variables effected by a reset must be included
   */

   ScSimApp.HkPkt.LastAction       = LastTbl->LastAction;
   ScSimApp.HkPkt.LastActionStatus = LastTbl->LastActionStatus;
   
   /*
   ** Spacecraft Simulator 
   */
   

   ScSimApp.HkPkt.SimActive = ScSimApp.ScSim.Active;
   ScSimApp.HkPkt.SimPhase  = ScSimApp.ScSim.Phase;
   ScSimApp.HkPkt.SimTime   = ScSimApp.ScSim.Time.Seconds;
   
      
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &ScSimApp.HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &ScSimApp.HkPkt);

} /* End SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
** TODO - Add additional status checks
*/
static int32 InitApp(uint32* AppMainPerfId)
{
   
   int32 Status = OSK_C_FW_CFS_ERROR;
 
   /*
   ** Initialize 'entity' objects
   */

   if (INITBL_Constructor(INITBL_OBJ, SCSIM_INI_FILENAME, &IniCfgEnum)) {
   
      *AppMainPerfId = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_MAIN_PERF_ID);
      CFE_ES_PerfLogEntry(*AppMainPerfId);

      SCSIMTBL_Constructor(SCSIM_TBL, SCSIM_GetTblPtr, SCSIM_LoadTbl, SCSIM_LoadTblEntry);
      SCSIM_Constructor(SCSIM, (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_MGMT_TLM_MID),
                               (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_MODEL_TLM_MID));

      /*
      ** Initialize cFE interfaces 
      */

      CFE_SB_CreatePipe(&ScSimApp.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH), INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME));
      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_MID),     ScSimApp.CmdPipe);
      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_EXECUTE_MID), ScSimApp.CmdPipe);
      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_SEND_HK_MID), ScSimApp.CmdPipe);

      /*
      ** Initialize App Framework Components 
      */

      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, SCSIM_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, SCSIM_ResetAppCmd, 0);
       
      CMDMGR_RegisterFunc(CMDMGR_OBJ, SCSIMTBL_LOAD_CMD_FC,  TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, SCSIMTBL_DUMP_CMD_FC,  TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);
       
      CMDMGR_RegisterFunc(CMDMGR_OBJ, SCSIM_START_SIM_CMD_FC,  SCSIM, SCSIM_StartSimCmd,  SCSIM_START_SIM_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, SCSIM_STOP_SIM_CMD_FC,   SCSIM, SCSIM_StopSimCmd,   SCSIM_NO_ARGS_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, SCSIM_START_PLBK_CMD_FC, SCSIM, SCSIM_StartPlbkCmd, SCSIM_NO_ARGS_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, SCSIM_STOP_PLBK_CMD_FC,  SCSIM, SCSIM_StopPlbkCmd,  SCSIM_NO_ARGS_CMD_DATA_LEN);

      TBLMGR_Constructor(TBLMGR_OBJ);
      TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, SCSIMTBL_LoadCmd, SCSIMTBL_DumpCmd, INITBL_GetStrConfig(INITBL_OBJ, CFG_TBL_LOAD_FILE));
                            
      CFE_SB_InitMsg(&ScSimApp.HkPkt, (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_HK_TLM_MID), SCSIM_APP_HK_PKT_LEN, TRUE);

                           
      /*
      ** Application startup event message
      */
      CFE_EVS_SendEvent(SCSIM_APP_INIT_EID, CFE_EVS_INFORMATION, "SCSIM App Initialized. Version %d.%d.%d",
                        SCSIM_MAJOR_VER, SCSIM_MINOR_VER, SCSIM_PLATFORM_REV);

      Status = CFE_SUCCESS;

   } /* End if INITBL constructed */

   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommandPipe
**
*/
static void ProcessCommandPipe(CFE_SB_MsgId_t CmdMid, CFE_SB_MsgId_t ExecuteMid, CFE_SB_MsgId_t SendHkMid)
{

   int32           Status;
   CFE_SB_Msg_t*   CmdMsgPtr;
   CFE_SB_MsgId_t  MsgId;

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, ScSimApp.CmdPipe, CFE_SB_PEND_FOREVER);

   if (Status == CFE_SUCCESS) {
      
      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      if (MsgId == CmdMid) {
      
         CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
   
      } 
      else if (MsgId == ExecuteMid) {
   
         SCSIM_Execute();
      
      }
      else if (MsgId == SendHkMid) {
      
         SendHousekeepingPkt();
   
      }
      else {
   
         CFE_EVS_SendEvent(SCSIM_APP_CMD_INVALID_MID_EID, CFE_EVS_ERROR,
                           "Received invalid command packet,MID = 0x%4X",MsgId);
      }

   } /* End if SB received a packet */

} /* End ProcessCommandPipe() */

