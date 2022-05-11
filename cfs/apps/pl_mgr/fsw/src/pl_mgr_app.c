/* 
** Purpose: Implement the Payload Manager App
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
#include "pl_mgr_app.h"


/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ   (&(PlMgr.IniTbl))
#define  CMDMGR_OBJ   (&(PlMgr.CmdMgr))
#define  PAYLOAD_OBJ  (&(PlMgr.Payload))
#define  SCI_FILE_OBJ (&(PlMgr.Payload.SciFile))


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(void);
static void ProcessCommandPipe(void);
static void SendStatusTlm(void);

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

PL_MGR_Class_t  PlMgr;


/******************************************************************************
** Function: PL_MGR_AppMain
**
*/
void PL_MGR_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;
   
   Status = CFE_ES_RegisterApp();

   /*
   ** Perform application specific initialization
   */
   if (Status == CFE_SUCCESS)
   {

      CFE_EVS_Register(NULL,0,0);
      Status = InitApp(); /* Performs initial CFE_ES_PerfLogEntry() call */
   
      if (Status == CFE_SUCCESS)
      {
         RunStatus = CFE_ES_APP_RUN;
      }
   }
   
   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {

      /*
      ** ProcessCommands() pends indefinitely. The scheduler sends
      ** a message to manage science files.
      */
	  
      ProcessCommandPipe();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("PL_MGR Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(PL_MGR_EXIT_EID, CFE_EVS_CRITICAL, 
                     "PL_MGR Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(PlMgr.PerfId);
   CFE_ES_ExitApp(RunStatus);

} /* End of PL_MGR_Main() */


/******************************************************************************
** Function: PL_MGR_NoOpCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean PL_MGR_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (PL_MGR_CMD_NOOP_EID, CFE_EVS_INFORMATION,
                      "No operation command received for PL_MGR version %d.%d.%d",
                      PL_MGR_MAJOR_VER, PL_MGR_MINOR_VER, PL_MGR_PLATFORM_REV);

   return TRUE;


} /* End PL_MGR_NoOpCmd() */


/******************************************************************************
** Function: PL_MGR_ResetAppCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean PL_MGR_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   PAYLOAD_ResetStatus();
   SCI_FILE_ResetStatus();

   return TRUE;

} /* End PL_MGR_ResetAppCmd() */


/******************************************************************************
** Function: SendStatusTlm
**
*/
static void SendStatusTlm(void)
{

   /*
   ** CMDMGR Data
   */

   PlMgr.StatusTlm.ValidCmdCnt   = PlMgr.CmdMgr.ValidCmdCnt;
   PlMgr.StatusTlm.InvalidCmdCnt = PlMgr.CmdMgr.InvalidCmdCnt;

   
   /*
   ** Payload Data
   */
   
   PlMgr.StatusTlm.PayloadPowerState         = PlMgr.Payload.CurrPower;
   PlMgr.StatusTlm.PayloadDetectorFault      = PlMgr.Payload.DetectorFault;
   PlMgr.StatusTlm.PayloadDetectorReadoutRow = PlMgr.Payload.Detector.ReadoutRow;
   PlMgr.StatusTlm.PayloadDetectorImageCnt   = PlMgr.Payload.Detector.ImageCnt;

   /*
   ** Science File Data
   */   

   PlMgr.StatusTlm.SciFileOpen     = PlMgr.Payload.SciFile.IsOpen;
   PlMgr.StatusTlm.SciFileImageCnt = PlMgr.Payload.SciFile.ImageCnt;   
   strncpy(PlMgr.StatusTlm.SciFilename, PlMgr.Payload.SciFile.Name, OS_MAX_PATH_LEN);
   
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &PlMgr.StatusTlm);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &PlMgr.StatusTlm);

} /* End SendStatusTlm() */

   

/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{
   
   int32 Status = OSK_C_FW_CFS_ERROR;
 
   /*
   ** Initialize 'entity' objects
   */

   if (INITBL_Constructor(INITBL_OBJ, PL_MGR_INI_FILENAME, &IniCfgEnum)) {
   
      PlMgr.PerfId  = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_PERF_ID);
      CFE_ES_PerfLogEntry(PlMgr.PerfId);

      PlMgr.CmdMid      = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_MID);
      PlMgr.ExecuteMid  = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_EXECUTE_MID);
      PlMgr.TlmSlowRate = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_TLM_SLOW_RATE);

      PAYLOAD_Constructor(PAYLOAD_OBJ, INITBL_OBJ);

      /*
      ** Initialize cFE interfaces 
      */

      CFE_SB_CreatePipe(&PlMgr.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH), 
                        INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME));
      
      CFE_SB_Subscribe(PlMgr.CmdMid,     PlMgr.CmdPipe);
      CFE_SB_Subscribe(PlMgr.ExecuteMid, PlMgr.CmdPipe);

      /*
      ** Initialize App Framework Components 
      */

      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, PL_MGR_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, PL_MGR_ResetAppCmd, 0);
              
  
      CMDMGR_RegisterFunc(CMDMGR_OBJ, PAYLOAD_START_SCI_CMD_FC, PAYLOAD_OBJ,  PAYLOAD_StartSciCmd, PAYLOAD_START_SCI_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, PAYLOAD_STOP_SCI_CMD_FC,  PAYLOAD_OBJ,  PAYLOAD_StopSciCmd,  PAYLOAD_STOP_SCI_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, SCI_FILE_CONFIG_CMD_FC,   SCI_FILE_OBJ, SCI_FILE_ConfigCmd,  SCI_FILE_CONFIG_CMD_DATA_LEN);
     
      CFE_SB_InitMsg(&PlMgr.StatusTlm, (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_TLM_MID), PL_MGR_APP_STATUS_TLM_LEN, TRUE);

      /*
      ** Application startup event message
      */
      CFE_EVS_SendEvent(PL_MGR_INIT_EID, CFE_EVS_INFORMATION, "PL_MGR App Initialized. Version %d.%d.%d",
                        PL_MGR_MAJOR_VER, PL_MGR_MINOR_VER, PL_MGR_PLATFORM_REV);

      Status = CFE_SUCCESS;

   } /* End if INITBL constructed */

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

   CFE_ES_PerfLogExit(PlMgr.PerfId);
   Status = CFE_SB_RcvMsg(&CmdMsgPtr, PlMgr.CmdPipe, CFE_SB_PEND_FOREVER);
   CFE_ES_PerfLogEntry(PlMgr.PerfId);

   if (Status == CFE_SUCCESS)
   {
      
      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      if (MsgId == PlMgr.CmdMid)
      {
      
         CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
   
      } 
      else if (MsgId == PlMgr.ExecuteMid)
      {
   
         PAYLOAD_ManageData();
         if (PlMgr.Payload.CurrPower != PL_SIM_LIB_POWER_OFF)
         {
            SendStatusTlm();
         }
         else
         {
            if (PlMgr.TlmSlowRateCnt >= PlMgr.TlmSlowRate)
            {
               SendStatusTlm();
               PlMgr.TlmSlowRateCnt = 0;
            }
            else
            {
               PlMgr.TlmSlowRateCnt++;
            }
         }
         
      }
      else 
      {
   
         CFE_EVS_SendEvent(PL_MGR_CMD_INVALID_MID_EID, CFE_EVS_ERROR,
                           "Received invalid command packet,MID = 0x%4X",MsgId);
      }

   } /* End if SB received a packet */

} /* End ProcessCommandPipe() */


