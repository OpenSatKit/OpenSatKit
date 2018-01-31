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


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);
static void ProcessSensorData(void);
static void SendControlTlm(void);

/*
** Global Data
*/

F42_APP_Class   F42App;
F42_APP_HkPkt   F42AppHkPkt;
F42_APP_CtrlPkt F42AppCtrlPkt;
   
/* Convenience macro */
#define  CMDMGR_OBJ  (&(F42App.CmdMgr))  
#define  TBLMGR_OBJ  (&(F42App.TblMgr))
#define  F42_ADP_OBJ (&(F42App.F42Adp))
#define  CTRLTBL_OBJ (&(F42App.F42Adp.CtrlTbl))

/******************************************************************************
** Function: F42_AppMain
**
*/
void F42_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;

   Status = CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL,0,0);

   /*
   ** Perform application specific initialization
   */
   if (Status == CFE_SUCCESS)
   {
       Status = InitApp();
   }

   /*
   ** At this point many flight apps use CFE_ES_WaitForStartupSync() to
   ** synchronize their startup timing with other apps. This is not
   ** needed.
   */

   if (Status == CFE_SUCCESS) 
      RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   ** - Sensor Data processing pends for sensor data with a timeout.
   ** - Command processing polls the command queue
   */
   while (CFE_ES_RunLoop(&RunStatus)) {

      ProcessSensorData();
      ProcessCommands();
      SendControlTlm();
      
   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("F42 App terminating, err = 0x%08X\n", Status);

   CFE_EVS_SendEvent(F42_APP_EXIT_ERR_EID, CFE_EVS_CRITICAL, "F42 App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of F42_AppMain() */


/******************************************************************************
** Function: F42_APP_NoOpCmd
**
*/

boolean F42_APP_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (F42_APP_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for F42 App version %d.%d",
                      F42_APP_MAJOR_VERSION,F42_APP_MINOR_VERSION);

   return TRUE;


} /* End F42_APP_NoOpCmd() */


/******************************************************************************
** Function: F42_APP_ResetAppCmd
**
*/

boolean F42_APP_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);

   F42App.ControllerExeCnt = 0;
   F42_ADP_ResetStatus();
	  
   return TRUE;

} /* End F42_APP_ResetAppCmd() */


/******************************************************************************
** Function: F42_APP_SendHousekeepingPkt
**
*/
void F42_APP_SendHousekeepingPkt(void)
{
   int i;
   const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);
	  
   /*
   ** F42 Application Data
   */

   F42AppHkPkt.ValidCmdCnt   = F42App.CmdMgr.ValidCmdCnt;
   F42AppHkPkt.InvalidCmdCnt = F42App.CmdMgr.InvalidCmdCnt;

   F42AppHkPkt.LastAction       = LastTbl->LastAction;
   F42AppHkPkt.LastActionStatus = LastTbl->LastActionStatus;

   F42AppHkPkt.ControllerExeCnt = F42App.ControllerExeCnt;

   /*
   ** 42 FSW Adapter Data
   */

   F42AppHkPkt.ControlMode   = F42App.F42Adp.ControlMode;

   F42AppHkPkt.SunTargetAxis = F42App.F42Adp.SunTargetAxis;
   F42AppHkPkt.CssFault      = F42App.F42Adp.Fault[F42_ADP_FAULT_CSS];

   for (i=0; i < 3; i++) {
      F42AppHkPkt.Kr[i] = F42App.F42Adp.Fsw.Kr[i];
      F42AppHkPkt.Kp[i] = F42App.F42Adp.Fsw.Kp[i];
   }

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &F42AppHkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &F42AppHkPkt);

} /* End F42_APP_SendHousekeepingPkt() */


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

    F42_ADP_Constructor(&(F42App.F42Adp));


    /*
    ** Initialize app level interfaces
    */

    CFE_SB_CreatePipe(&F42App.CmdPipe, F42_CMD_PIPE_DEPTH, F42_CMD_PIPE_NAME);
    CFE_SB_Subscribe(F42_CMD_MID, F42App.CmdPipe);
    CFE_SB_Subscribe(F42_SEND_HK_MID, F42App.CmdPipe);

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, F42_APP_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, F42_APP_ResetAppCmd, 0);

    CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_TBL_LOAD_CMD_FC,       TBLMGR_OBJ, TBLMGR_LoadTblCmd,        TBLMGR_LOAD_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_TBL_DUMP_CMD_FC,       TBLMGR_OBJ, TBLMGR_DumpTblCmd,        TBLMGR_DUMP_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_MODE_CMD_FC,       F42_ADP_OBJ, F42_ADP_SetModeCmd,      F42_ADP_SET_MODE_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_FAULT_CMD_FC,      F42_ADP_OBJ, F42_ADP_SetFaultCmd,     F42_ADP_SET_FAULT_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_SUN_TARGET_CMD_FC, F42_ADP_OBJ, F42_ADP_SetSunTargetCmd, F42_ADP_SET_SUN_TARGET_CMD_DATA_LEN);

	TBLMGR_Constructor(TBLMGR_OBJ);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, CTRLTBL_LoadCmd,  CTRLTBL_DumpCmd,  F42_APP_CTRL_TBL_DEF_LOAD_FILE);

    CFE_SB_CreatePipe(&F42App.SensorPipe,F42_SENSOR_PIPE_DEPTH, F42_SENSOR_PIPE_NAME);
    CFE_SB_Subscribe(F42_SENSOR_MID, F42App.SensorPipe);

    CFE_SB_InitMsg(&F42AppHkPkt, F42_HK_TLM_MID, F42_APP_TLM_HK_LEN, TRUE);
    CFE_SB_InitMsg(&F42AppCtrlPkt, F42_CONTROL_MID, F42_APP_TLM_CTRL_PKT_LEN, TRUE);

    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(F42_APP_INIT_APP_INFO_EID,
                               CFE_EVS_INFORMATION,
                               "F42 App Initialized. Version %d.%d.%d.%d",
                               F42_APP_MAJOR_VERSION,
                               F42_APP_MINOR_VERSION,
                               F42_APP_REVISION,
                               F42_APP_MISSION_REV);

    return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommands
**
*/
static void ProcessCommands(void)
{

   int32           Status;
   CFE_SB_Msg_t*   CmdMsgPtr;
   CFE_SB_MsgId_t  MsgId;

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, F42App.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case F42_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case F42_SEND_HK_MID:
            F42_APP_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(F42_APP_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */

/******************************************************************************
** Function: ProcessSensorData
**
*/
static void ProcessSensorData(void)
{

   int32               Status;
   CFE_SB_Msg_t*       MsgPtr;
   CFE_SB_MsgId_t      MsgId;
   F42_ADP_SensorPkt*  SensorPkt;


   Status = CFE_SB_RcvMsg(&MsgPtr, F42App.SensorPipe, F42_APP_SENSOR_TIMEOUT);
   
   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(MsgPtr);

      if (MsgId == F42_SENSOR_MID) {

         F42App.ControllerExeCnt++;
         
         SensorPkt = (F42_ADP_SensorPkt*)MsgPtr;
         F42_ADP_RunController(&(SensorPkt->Sensor));
         
      }
      else {
      
         CFE_EVS_SendEvent(F42_APP_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                           "Received invalid sensor data packet,MID = 0x%4X",MsgId);
      
      } /* End if F42_SENSOR_MID */
      
   } /* End if SB received a packet */

} /* End ProcessSensorData() */
                          

                          
/******************************************************************************
** Function: SendControlTlm
**
*/
static void SendControlTlm(void)
{
   uint8 i;
   
   for (i=0; i<3; i++) {
      F42AppCtrlPkt.wbn[i]       = (float)F42App.F42Adp.Fsw.wbn[i];
      F42AppCtrlPkt.svb[i]       = (float)F42App.F42Adp.Fsw.svb[i];
      F42AppCtrlPkt.therr[i]     = (float)F42App.F42Adp.Fsw.therr[i];
      F42AppCtrlPkt.Twhlcmd[i]   = (float)F42App.F42Adp.Fsw.Twhlcmd[i];
      F42AppCtrlPkt.CssCounts[i] = (uint16)F42App.F42Adp.Fsw.Css[i].Counts;
   }
   F42AppCtrlPkt.CssCounts[3] = (uint16)F42App.F42Adp.Fsw.Css[3].Counts;
   
   F42AppCtrlPkt.SunValid = F42App.F42Adp.Fsw.SunValid;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &F42AppCtrlPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &F42AppCtrlPkt);

} /* End SendControlTlm() */
                             
/* end of file */
