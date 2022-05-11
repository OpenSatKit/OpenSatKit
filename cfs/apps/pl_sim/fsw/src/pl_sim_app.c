/*
** Purpose: Implement the Payload Simulator application
**
** Notes:
**   1. See header notes. 
**
** References:
**   1. OpenSat Application Developer's Guide
**   2. cFS Application Developer's Guide
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/

/*
** Includes
*/

#include <string.h>
#include "pl_sim_app.h"


/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ    (&(PlSim.IniTbl))
#define  CMDMGR_OBJ    (&(PlSim.CmdMgr))


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(void);
static int32 ProcessCommandPipe(void);
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

PL_SIM_Class_t  PlSim;


/******************************************************************************
** Function: PL_SIM_AppMain
**
*/
void PL_SIM_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;

   Status = CFE_ES_RegisterApp();
   
   if (Status == CFE_SUCCESS)
   {
      
      CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);

      Status = InitApp();
   
      if (Status == CFE_SUCCESS)
      {
         
         RunStatus = CFE_ES_APP_RUN;
         
      }
   
   } /* End if RegisterApp() success */
   
   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {

      RunStatus = ProcessCommandPipe();
      
   } /* End CFE_ES_RunLoop */

   CFE_ES_WriteToSysLog("PL_SIM App terminating, err = 0x%08X\n", Status);   /* Use SysLog, events may not be working */

   CFE_EVS_SendEvent(PL_SIM_EXIT_EID, CFE_EVS_CRITICAL, "PL_SIM App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of PL_SIM_AppMain() */


/******************************************************************************
** Function: PL_SIM_NoOpCmd
**
*/
boolean PL_SIM_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (PL_SIM_NOOP_EID, CFE_EVS_INFORMATION,
                      "No operation command received for PL_SIM App version %d.%d.%d",
                      PL_SIM_MAJOR_VER, PL_SIM_MINOR_VER, PL_SIM_PLATFORM_REV);

   return TRUE;


} /* End PL_SIM_NoOpCmd() */


/******************************************************************************
** Function: PL_SIM_ResetAppCmd
**
** Notes:
**   1. Framework objects require an object reference since they are
**      reentrant. Applications use the singleton pattern and store a
**      reference pointer to the object data during construction.
*/
boolean PL_SIM_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   
   /* Leave the PL_SIM library state intact */
	  
   return TRUE;

} /* End PL_SIM_ResetAppCmd() */


/******************************************************************************
** Functions: PL_SIM_PowerOnCmd
**
** Power on the payload
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. The PL_SIM_LIB outputs an event message power state transitions
*/
boolean PL_SIM_PowerOnCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = FALSE;

   if (PlSim.Lib.State.Power == PL_SIM_LIB_POWER_OFF)
   {
      PL_SIM_LIB_PowerOn();      
      RetStatus = TRUE;
   
   }  
   else
   { 
      CFE_EVS_SendEvent (PL_SIM_PWR_ON_CMD_ERR_EID, CFE_EVS_ERROR, 
                         "Power on payload cmd rejected. Payload must be in OFF state and it's in the %s state.",
                         PL_SIM_LIB_GetPowerStateStr(PlSim.Lib.State.Power));
   }
   
   return RetStatus;

} /* End PL_SIM_PowerOnCmd() */


/******************************************************************************
** Functions: PL_SIM_PowerOffCmd
**
** Power off science instrument regardless of current state. The science state
** is unmodified and the PL_SIM_Execute() function takes care of any science
** data cleanup activities.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. The PL_SIM library outputs an event message power state transitions
*/
boolean PL_SIM_PowerOffCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   PL_SIM_LIB_PowerOff();
      
   return TRUE;

} /* End PL_SIM_PowerOffCmd() */


/******************************************************************************
** Functions: PL_SIM_PowerResetCmd
**
** Initiate a power reset. The PL_SIM_ExecuteStep() method defines how the
** simple responds to a reset.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. The PL_SIM library outputs an event message power state transitions
*/
boolean PL_SIM_PowerResetCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = FALSE;

   if (PlSim.Lib.State.Power == PL_SIM_LIB_POWER_READY)
   {
      PL_SIM_LIB_PowerReset();
      RetStatus = TRUE;
   
   }  
   else
   { 
      CFE_EVS_SendEvent (PL_SIM_PWR_RESET_CMD_ERR_EID, CFE_EVS_ERROR, 
                         "Reset payload power cmd rejected. Payload must be in READY state and it's in the %s state.",
                         PL_SIM_LIB_GetPowerStateStr(PlSim.Lib.State.Power));
   }
   
   return RetStatus;

} /* End PL_SIM_PowerResetCmd() */


/******************************************************************************
** Functions: PL_SIM_SetFaultCmd
**
** Set instrument fault state to TRUE.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean PL_SIM_SetFaultCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   PL_SIM_LIB_SetFault();

   CFE_EVS_SendEvent (PL_SIM_SET_FAULT_CMD_EID, CFE_EVS_INFORMATION, 
                      "Payload fault set to TRUE.");
               
   return TRUE;

} /* End PL_SIM_SetFaultCmd() */


/******************************************************************************
** Functions: PL_SIM_ClearFaultCmd
**
** Set instrument fault state to FALSE.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean PL_SIM_ClearFaultCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   PL_SIM_LIB_ClearFault();

   CFE_EVS_SendEvent (PL_SIM_CLEAR_FAULT_CMD_EID, CFE_EVS_INFORMATION, 
                      "Payload fault set to FALSE.");
               
   return TRUE;

} /* End PL_SIM_ClearFaultCmd() */


/******************************************************************************
** Function: SendStatusTlm
**
*/
static void SendStatusTlm(void)
{

   /*
   ** Framework Data
   */
   
   PlSim.StatusTlm.ValidCmdCnt   = PlSim.CmdMgr.ValidCmdCnt;
   PlSim.StatusTlm.InvalidCmdCnt = PlSim.CmdMgr.InvalidCmdCnt;
   
   
   /*
   ** PL_SIM Library Data
   */

   PlSim.StatusTlm.LibPwrState           = PlSim.Lib.State.Power;
   PlSim.StatusTlm.LibPwrInitCycleCnt    = PlSim.Lib.State.PowerInitCycleCnt;
   PlSim.StatusTlm.LibPwrResetCycleCnt   = PlSim.Lib.State.PowerResetCycleCnt;
   PlSim.StatusTlm.LibDetectorFault      = PlSim.Lib.State.DetectorFaultPresent;
   PlSim.StatusTlm.LibDetectorReadoutRow = PlSim.Lib.Detector.ReadoutRow;
   PlSim.StatusTlm.LibDetectorImageCnt   = PlSim.Lib.Detector.ImageCnt;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &PlSim.StatusTlm);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &PlSim.StatusTlm);

} /* End SendStatusTlm() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 Status = OSK_C_FW_CFS_ERROR;
   
   /*
   ** Initialize objects 
   */

   if (INITBL_Constructor(&PlSim.IniTbl, PL_SIM_INI_FILENAME, &IniCfgEnum))
   {
   
      PlSim.CmdMid      = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_MID);
      PlSim.ExecuteMid  = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_EXECUTE_MID);
      PlSim.TlmSlowRate = INITBL_GetIntConfig(INITBL_OBJ, CFG_TLM_SLOW_RATE);

      Status = CFE_SUCCESS; 
  
   } /* End if INITBL Constructed */
  
   if (Status == CFE_SUCCESS)
   {

      /*
      ** Constuct app's child objects
      */
            
        
      /*
      ** Initialize app level interfaces
      */
      
      CFE_SB_CreatePipe(&PlSim.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH), INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME));  
      CFE_SB_Subscribe(PlSim.CmdMid,     PlSim.CmdPipe);
      CFE_SB_Subscribe(PlSim.ExecuteMid, PlSim.CmdPipe);

      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, PL_SIM_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, PL_SIM_ResetAppCmd, 0);

      CMDMGR_RegisterFunc(CMDMGR_OBJ, PL_SIM_POWER_ON_CMD_FC,    &PlSim,  PL_SIM_PowerOnCmd,    PL_SIM_POWER_ON_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, PL_SIM_POWER_OFF_CMD_FC,   &PlSim,  PL_SIM_PowerOffCmd,   PL_SIM_POWER_OFF_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, PL_SIM_POWER_RESET_CMD_FC, &PlSim,  PL_SIM_PowerResetCmd, PL_SIM_POWER_RESET_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, PL_SIM_SET_FAULT_CMD_FC,   &PlSim,  PL_SIM_SetFaultCmd,   PL_SIM_SET_FAULT_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, PL_SIM_CLEAR_FAULT_CMD_FC, &PlSim,  PL_SIM_ClearFaultCmd, PL_SIM_CLEAR_FAULT_CMD_DATA_LEN);

      /*
      ** Initialize app messages 
      */
 
      CFE_SB_InitMsg(&PlSim.StatusTlm, (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_TLM_MID), PL_SIM_APP_STATUS_TLM_LEN, TRUE);

      /*
      ** Application startup event message
      */
      CFE_EVS_SendEvent(PL_SIM_INIT_APP_EID, CFE_EVS_INFORMATION,
                        "PL_SIM App Initialized. Version %d.%d.%d",
                        PL_SIM_MAJOR_VER, PL_SIM_MINOR_VER, PL_SIM_PLATFORM_REV);
                        
   } /* End if CHILDMGR constructed */
   
   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommandPipe
**
** 
*/
static int32 ProcessCommandPipe(void)
{
   
   int32           SbStatus;
   int32           RetStatus = CFE_ES_APP_RUN;
   CFE_SB_Msg_t*   CmdMsgPtr;
   CFE_SB_MsgId_t  MsgId;

   SbStatus = CFE_SB_RcvMsg(&CmdMsgPtr, PlSim.CmdPipe, CFE_SB_PEND_FOREVER);

   if (SbStatus == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      if (MsgId == PlSim.CmdMid)
      {

         CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            
      } 
      else if (MsgId == PlSim.ExecuteMid)
      {

         PL_SIM_LIB_ExecuteStep();
         PL_SIM_LIB_ReadState(&PlSim.Lib);
         if (PlSim.Lib.State.Power != PL_SIM_LIB_POWER_OFF)
         {
            SendStatusTlm();
         }
         else
         {
            if (PlSim.TlmSlowRateCnt >= PlSim.TlmSlowRate)
            {
               SendStatusTlm();
               PlSim.TlmSlowRateCnt = 0;
            }
            else
            {
               PlSim.TlmSlowRateCnt++;
            }
         }

      }
      else
      {
            
         CFE_EVS_SendEvent(PL_SIM_INVALID_MID_EID, CFE_EVS_ERROR,
                           "Received invalid command packet,MID = 0x%4X",MsgId);
      }

   } /* End if SB received a packet */
   else
   {
      
      RetStatus = CFE_ES_APP_ERROR;
      
   } /* End if SB failure */

   return RetStatus;
   
} /* End ProcessCommandPipe() */
