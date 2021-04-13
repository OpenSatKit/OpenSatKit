/* 
** Purpose: Implement the Instrument Simulator App
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
#include "isim_app.h"

/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ  (&(IsimApp.IniTbl))
#define  CMDMGR_OBJ  (&(IsimApp.CmdMgr))
#define  TBLMGR_OBJ  (&(IsimApp.TblMgr))
#define  ISIM        (&(IsimApp.Isim))
#define  ISIM_TBL    (&(IsimApp.IsimTbl))


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

ISIM_APP_Class  IsimApp;



/******************************************************************************
** Function: ISIM_AppMain
**
*/
void ISIM_AppMain(void)
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

   CFE_ES_WriteToSysLog("ISIM Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(ISIM_APP_EXIT_EID, CFE_EVS_CRITICAL, "ISIM Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(AppMainPerfId);
   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of ISIM_Main() */


/******************************************************************************
** Function: ISIM_NoOpCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean ISIM_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (ISIM_APP_CMD_NOOP_EID, CFE_EVS_INFORMATION,
                      "No operation command received for ISIM version %d.%d.%d",
                      ISIM_MAJOR_VER, ISIM_MINOR_VER, ISIM_PLATFORM_REV);

   return TRUE;


} /* End ISIM_NoOpCmd() */


/******************************************************************************
** Function: ISIM_ResetAppCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean ISIM_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   ISIMTBL_ResetStatus();
   ISIM_ResetStatus();

   return TRUE;

} /* End ISIM_ResetAppCmd() */


/******************************************************************************
** Function: SendHousekeepingPkt
**
*/
static void SendHousekeepingPkt(void)
{

   /* Good design practice in case app expands to more than on etable */
   const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);

   
   /*
   ** CMDMGR Data
   */

   IsimApp.HkPkt.ValidCmdCnt   = IsimApp.CmdMgr.ValidCmdCnt;
   IsimApp.HkPkt.InvalidCmdCnt = IsimApp.CmdMgr.InvalidCmdCnt;

   
   /*
   ** ISIM/ISIMTBL Data
   ** - At a minimum all OBJECT variables effected by a reset must be included
   */

   IsimApp.HkPkt.LastAction       = LastTbl->LastAction;
   IsimApp.HkPkt.LastActionStatus = LastTbl->LastActionStatus;

   IsimApp.HkPkt.IsimPwrState           = (uint8)ISIM->Instr.PwrState;
   IsimApp.HkPkt.IsimPwrInitCycleCnt    = (uint8)ISIM->Instr.PwrInitCycleCnt;
   IsimApp.HkPkt.IsimPwrResetCycleCnt   = (uint8)ISIM->Instr.PwrResetCycleCnt;
   IsimApp.HkPkt.IsimSciState           = (uint8)ISIM->Instr.SciState;
   IsimApp.HkPkt.IsimDetectorImageCnt   = ISIM->Instr.Detector.ImageCnt;
   IsimApp.HkPkt.IsimDetectorReadoutRow = (uint8)ISIM->Instr.Detector.ReadoutRow;
   IsimApp.HkPkt.IsimDetectorFault      = (uint8)ISIM->Instr.Detector.FaultPresent;
   IsimApp.HkPkt.IsimSciFileOpen        = (uint8)ISIM->SciFile.IsOpen;
   IsimApp.HkPkt.CurrFileImageCnt       = (uint8)ISIM->Instr.CurrFileImageCnt;
   
   strncpy(IsimApp.HkPkt.SciFilename, ISIM->SciFile.Name, OS_MAX_PATH_LEN);
   
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &IsimApp.HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &IsimApp.HkPkt);

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

   if (INITBL_Constructor(INITBL_OBJ, ISIM_INI_FILENAME, &IniCfgEnum)) {
   
      *AppMainPerfId = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_MAIN_PERF_ID);
      CFE_ES_PerfLogEntry(*AppMainPerfId);

      ISIMTBL_Constructor(ISIM_TBL, ISIM_GetTblPtr, ISIM_LoadTbl, ISIM_LoadTblEntry);
      ISIM_Constructor(ISIM);

      /*
      ** Initialize cFE interfaces 
      */

      CFE_SB_CreatePipe(&IsimApp.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH), INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME));
      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_MID),     IsimApp.CmdPipe);
      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_EXECUTE_MID), IsimApp.CmdPipe);
      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_SEND_HK_MID), IsimApp.CmdPipe);

      /*
      ** Initialize App Framework Components 
      */

      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, ISIM_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, ISIM_ResetAppCmd, 0);
       
      CMDMGR_RegisterFunc(CMDMGR_OBJ, ISIMTBL_LOAD_CMD_FC,  TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, ISIMTBL_DUMP_CMD_FC,  TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);
       
      CMDMGR_RegisterFunc(CMDMGR_OBJ, ISIM_PWR_ON_INSTR_CMD_FC,    ISIM,  ISIM_PwrOnInstrCmd,    ISIM_PWR_ON_INSTR_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, ISIM_PWR_OFF_INSTR_CMD_FC,   ISIM,  ISIM_PwrOffInstrCmd,   ISIM_PWR_OFF_INSTR_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, ISIM_PWR_RESET_INSTR_CMD_FC, ISIM,  ISIM_PwrResetInstrCmd, ISIM_PWR_RESET_INSTR_CMD_DATA_LEN);
      
      CMDMGR_RegisterFunc(CMDMGR_OBJ, ISIM_START_SCI_CMD_FC,       ISIM,  ISIM_StartSciCmd,      ISIM_START_SCI_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, ISIM_STOP_SCI_CMD_FC,        ISIM,  ISIM_StopSciCmd,       ISIM_STOP_SCI_CMD_DATA_LEN);
     
      CMDMGR_RegisterFunc(CMDMGR_OBJ, ISIM_SET_FAULT_CMD_FC,       ISIM,  ISIM_SetFaultCmd,      ISIM_SET_FAULT_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, ISIM_CLEAR_FAULT_CMD_FC,     ISIM,  ISIM_ClearFaultCmd,    ISIM_CLEAR_FAULT_CMD_DATA_LEN);

      TBLMGR_Constructor(TBLMGR_OBJ);
      TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, ISIMTBL_LoadCmd, ISIMTBL_DumpCmd, INITBL_GetStrConfig(INITBL_OBJ, CFG_TBL_LOAD_FILE));
                            
      CFE_SB_InitMsg(&IsimApp.HkPkt, (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_HK_TLM_MID), ISIM_APP_HK_PKT_LEN, TRUE);

                           
      /*
      ** Application startup event message
      */
      CFE_EVS_SendEvent(ISIM_APP_INIT_EID, CFE_EVS_INFORMATION, "ISIM App Initialized. Version %d.%d.%d",
                        ISIM_MAJOR_VER, ISIM_MINOR_VER, ISIM_PLATFORM_REV);

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

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, IsimApp.CmdPipe, CFE_SB_PEND_FOREVER);

   if (Status == CFE_SUCCESS) {
      
      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      if (MsgId == CmdMid) {
      
         CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
   
      } 
      else if (MsgId == ExecuteMid) {
   
         ISIM_Execute();
      
      }
      else if (MsgId == SendHkMid) {
      
         SendHousekeepingPkt();
   
      }
      else {
   
         CFE_EVS_SendEvent(ISIM_APP_CMD_INVALID_MID_EID, CFE_EVS_ERROR,
                           "Received invalid command packet,MID = 0x%4X",MsgId);
      }

   } /* End if SB received a packet */

} /* End ProcessCommandPipe() */

