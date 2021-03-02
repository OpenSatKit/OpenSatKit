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


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommandPipe(void);
static void SendHousekeepingPkt(void);

/*
** Global Data
*/

ISIM_APP_Class  IsimApp;


/*
** Convenience Macros
*/

#define  CMDMGR_OBJ (&(IsimApp.CmdMgr))
#define  TBLMGR_OBJ (&(IsimApp.TblMgr))
#define  ISIM       (&(IsimApp.Isim))
#define  ISIM_TBL   (&(IsimApp.IsimTbl))


/******************************************************************************
** Function: ISIM_AppMain
**
*/
void ISIM_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


   CFE_ES_PerfLogEntry(ISIM_MAIN_PERF_ID);
   Status = CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL,0,0);


   /*
   ** Perform application specific initialization
   */
   if (Status == CFE_SUCCESS) {
      
       Status = InitApp();
   }

   /*
   ** At this point many flight apps use CFE_ES_WaitForStartupSync() to
   ** synchronize their startup timing with other apps. This is not
   ** needed for this simple app.
   */

   if (Status == CFE_SUCCESS) RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus)) {

      /*
      ** ProcessCommands() pends indefinitely. The scheduler sends
      ** a message to execute a simulation step.
      */
	  
	   CFE_ES_PerfLogExit(ISIM_MAIN_PERF_ID);
      ProcessCommandPipe();
      CFE_ES_PerfLogEntry(ISIM_MAIN_PERF_ID);

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("ISIM Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(ISIM_APP_EXIT_EID, CFE_EVS_CRITICAL, "ISIM Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(ISIM_MAIN_PERF_ID);
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
*/
static int32 InitApp(void)
{
   
   int32 Status = CFE_SUCCESS;
 
   /*
   ** Initialize 'entity' objects
   */

   ISIMTBL_Constructor(ISIM_TBL, ISIM_GetTblPtr, ISIM_LoadTbl, ISIM_LoadTblEntry);
   ISIM_Constructor(ISIM);

   /*
   ** Initialize cFE interfaces 
   */

   CFE_SB_CreatePipe(&IsimApp.CmdPipe, ISIM_CMD_PIPE_DEPTH, ISIM_CMD_PIPE_NAME);
   CFE_SB_Subscribe(ISIM_CMD_MID, IsimApp.CmdPipe);
   CFE_SB_Subscribe(ISIM_EXECUTE_MID, IsimApp.CmdPipe);
   CFE_SB_Subscribe(ISIM_SEND_HK_MID, IsimApp.CmdPipe);

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
   TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, ISIMTBL_LoadCmd, ISIMTBL_DumpCmd, ISIMTBL_DEF_LOAD_FILE);
                         
   CFE_SB_InitMsg(&IsimApp.HkPkt, ISIM_TLM_HK_MID, ISIM_APP_HK_PKT_LEN, TRUE);

                        
   /*
   ** Application startup event message
   */
   Status = CFE_EVS_SendEvent(ISIM_APP_INIT_EID, CFE_EVS_INFORMATION,
                              "ISIM App Initialized. Version %d.%d.%d",
                              ISIM_MAJOR_VER, ISIM_MINOR_VER, ISIM_PLATFORM_REV);

   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommandPipe
**
*/
static void ProcessCommandPipe(void)
{

   int32           Status;
   CFE_SB_Msg_t*   CmdMsgPtr;
   CFE_SB_MsgId_t  MsgId;

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, IsimApp.CmdPipe, CFE_SB_PEND_FOREVER);

   if (Status == CFE_SUCCESS) {
      
      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId) {
         
         case ISIM_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case ISIM_EXECUTE_MID:
            ISIM_Execute();
            break;

         case ISIM_SEND_HK_MID:
            SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(ISIM_APP_CMD_INVALID_MID_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommandPipe() */

