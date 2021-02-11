/*
** Purpose: Implement the 42 Attitude Control (AC) application
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application 
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/

/*
** Includes
*/

#include <string.h>
#include "f42_app.h"
#include "i42_msgids.h"

/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessSbPipe(void);
static void SendHousekeepingPkt(void);

/*
** Global Data
*/

F42_Class   F42;


/* Convenience macro */
#define  CMDMGR_OBJ  (&(F42.CmdMgr))  
#define  TBLMGR_OBJ  (&(F42.TblMgr))
#define  F42_ADP_OBJ (&(F42.F42Adp))


/******************************************************************************
** Function: F42_AppMain
**
*/
void F42_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;

   Status = CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);

   /*
   ** Perform application specific initialization
   */
   if (Status == CFE_SUCCESS) {

      Status = InitApp();
   }


   if (Status == CFE_SUCCESS) 
      RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus)) {

      ProcessSbPipe();
      
   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("F42 App terminating, err = 0x%08X\n", Status);

   CFE_EVS_SendEvent(F42_EXIT_EID, CFE_EVS_CRITICAL, "F42 App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of F42_AppMain() */


/******************************************************************************
** Function: F42_NoOpCmd
**
*/

boolean F42_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (F42_NOOP_EID, CFE_EVS_INFORMATION,
                      "No operation command received for F42 App version %d.%d.%d",
                      F42_MAJOR_VER, F42_MINOR_VER, F42_PLATFORM_REV);

   return TRUE;


} /* End F42_NoOpCmd() */


/******************************************************************************
** Function: F42_ResetAppCmd
**
*/

boolean F42_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);

   F42_ADP_ResetStatus();
	  
   return TRUE;

} /* End F42_ResetAppCmd() */


/******************************************************************************
** Function: SendHousekeepingPkt
**
*/
static void SendHousekeepingPkt(void)
{
  
   const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);
	  
   /*
   ** F42 Application Data
   */

   F42.HkPkt.ValidCmdCnt   = F42.CmdMgr.ValidCmdCnt;
   F42.HkPkt.InvalidCmdCnt = F42.CmdMgr.InvalidCmdCnt;

   F42.HkPkt.LastAction       = LastTbl->LastAction;
   F42.HkPkt.LastActionStatus = LastTbl->LastActionStatus;


   /*.
   ** 42 FSW Adapter Data
   */

   F42.HkPkt.CtrlExeCnt  = F42.F42Adp.CtrlExeCnt;
   F42.HkPkt.CtrlMode    = F42.F42Adp.CtrlMode;
   F42.HkPkt.OvrSunValid = F42.F42Adp.Override[F42_ADP_OVR_SUN_VALID];

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &F42.HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &F42.HkPkt);

} /* End SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 Status = CFE_SUCCESS;

   /*
   ** Initialize objects 
   */

   F42_ADP_Constructor(&(F42.F42Adp));


   /*
   ** Initialize app level interfaces
   */

   CFE_SB_CreatePipe(&F42.SbPipe, F42_SB_PIPE_DEPTH, F42_SB_PIPE_NAME);
   CFE_SB_Subscribe(F42_CMD_MID, F42.SbPipe);
   CFE_SB_Subscribe(F42_SEND_HK_MID, F42.SbPipe);
   CFE_SB_Subscribe(I42_SENSOR_DATA_MID, F42.SbPipe);

   CMDMGR_Constructor(CMDMGR_OBJ);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, F42_NoOpCmd,     0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, F42_ResetAppCmd, 0);

   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_TBL_LOAD_CMD_FC, TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_TBL_DUMP_CMD_FC, TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);

   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SEND_CTRL_GAINS_CMD_FC,    F42_ADP_OBJ, F42_ADP_SendCtrlGainsCmd,   F42_ADP_SEND_CTRL_GAINS_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_CTRL_MODE_CMD_FC,      F42_ADP_OBJ, F42_ADP_SetCtrlModeCmd,     F42_ADP_SET_CTRL_MODE_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_OVR_CMD_FC,            F42_ADP_OBJ, F42_ADP_SetOvrCmd,          F42_ADP_SET_OVR_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_TARGET_WHL_MOM_CMD_FC, F42_ADP_OBJ, F42_ADP_SetTargetWhlMomCmd, F42_ADP_SET_TARGET_WHL_MOM_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_CONFIG_DBG_CMD_FC,         F42_ADP_OBJ, F42_ADP_ConfigDbgCmd,       F42_ADP_CONFIG_DBG_CMD_DATA_LEN);

   TBLMGR_Constructor(TBLMGR_OBJ);
   TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, CTRLTBL_LoadCmd,  CTRLTBL_DumpCmd,  F42_CTRL_TBL_DEF_LOAD_FILE);


   CFE_SB_InitMsg(&F42.HkPkt, F42_HK_TLM_MID, F42_TLM_HK_LEN, TRUE);
      
   /*
   ** Application startup event message
   */
   Status = CFE_EVS_SendEvent(F42_INIT_APP_EID, CFE_EVS_INFORMATION,
                              "F42 App Initialized. Version %d.%d.%d",
                              F42_MAJOR_VER, F42_MINOR_VER, F42_PLATFORM_REV);

   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessSbPipe
**
*/
static void ProcessSbPipe(void)
{

   int32           Status;
   CFE_SB_Msg_t*   SbMsgPtr;
   CFE_SB_MsgId_t  MsgId;

   Status = CFE_SB_RcvMsg(&SbMsgPtr, F42.SbPipe, CFE_SB_PEND_FOREVER);

   if (Status == CFE_SUCCESS) {

      MsgId = CFE_SB_GetMsgId(SbMsgPtr);

      switch (MsgId) {

         case F42_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, SbMsgPtr);
            break;

         case F42_SEND_HK_MID:
            SendHousekeepingPkt();
            break;

         case I42_SENSOR_DATA_MID:
            F42_ADP_Run42Fsw((IF42_SensorDataPkt*) SbMsgPtr);
            break;

         default:
            CFE_EVS_SendEvent(F42_INVALID_MID_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessSbPipe() */


