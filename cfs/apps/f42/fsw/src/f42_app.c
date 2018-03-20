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
static void SendCtrlTlm(void);

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
      SendCtrlTlm();
      
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

   F42App.CtrlExeCnt = 0;
   F42_ADP_ResetStatus();
	  
   return TRUE;

} /* End F42_APP_ResetAppCmd() */


/******************************************************************************
** Function: F42_APP_ConfigDbgCmd
**
** TODO - Add file command parameter
*/

boolean F42_APP_ConfigDbgCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const F42_APP_ConfigDbgCmdPkt *ConfigDbgCmd = (const F42_APP_ConfigDbgCmdPkt *) MsgPtr;
   boolean  RetStatus = FALSE;

   if ( ConfigDbgCmd->NewState == TRUE) {
     
      if (F42App.DbgEnabled == TRUE) {
         
         CFE_EVS_SendEvent(999, CFE_EVS_ERROR, "Enable debug command rejected; debug already enabled.");
      
      }
      else {
         
         F42App.DbgFileHandle = OS_creat(F42_APP_DBG_FILE, OS_WRITE_ONLY);
         
         if (F42App.DbgFileHandle >= OS_FS_SUCCESS) {
            
            F42App.DbgEnabled = TRUE;
            CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "Created debug file %s",F42_APP_DBG_FILE);
            RetStatus = TRUE;
         
         }
         else {

            CFE_EVS_SendEvent(999, CFE_EVS_ERROR, "Error creating debug file %s",F42_APP_DBG_FILE);
         
         } /* End if error creating file */
      } /* End if debug not enabled */
   } /* End if enable debug command */
   else {
      
      if (F42App.DbgEnabled == TRUE) {
         
         F42App.DbgEnabled = FALSE;
         OS_close(F42App.DbgFileHandle);
         CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "Debug file %s closed", F42_APP_DBG_FILE);
      
      }
      else {

         CFE_EVS_SendEvent(999, CFE_EVS_ERROR, "Disable debug command rejected; debug not enabled.");
      }
      
   } /* End if disable debug command */
        
   return RetStatus;

} /* End F42_APP_ConfigDbgCmd() */


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

   F42AppHkPkt.CtrlExeCnt = F42App.CtrlExeCnt;

   /*
   ** 42 FSW Adapter Data
   */

   F42AppHkPkt.CtrlMode     = F42App.F42Adp.CtrlMode;
   F42AppHkPkt.OvrSunValid  = F42App.F42Adp.Override[F42_ADP_OVR_SUN_VALID];

   for (i=0; i < 3; i++) {
      F42AppHkPkt.wc[i] = F42App.F42Adp.Fsw.wc[i];
      F42AppHkPkt.zc[i] = F42App.F42Adp.Fsw.zc[i];
      F42AppHkPkt.Kr[i] = F42App.F42Adp.Fsw.Kr[i];
      F42AppHkPkt.Kp[i] = F42App.F42Adp.Fsw.Kp[i];
      F42AppHkPkt.Hwcmd[i] = F42App.F42Adp.Fsw.Hwcmd[i];
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

   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_TBL_LOAD_CMD_FC, TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_TBL_DUMP_CMD_FC, TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);

   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_CTRL_MODE_CMD_FC,      F42_ADP_OBJ, F42_ADP_SetCtrlModeCmd,     F42_ADP_SET_CTRL_MODE_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_OVR_CMD_FC,            F42_ADP_OBJ, F42_ADP_SetOvrCmd,          F42_ADP_SET_OVR_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_TARGET_WHL_MOM_CMD_FC, F42_ADP_OBJ, F42_ADP_SetTargetWhlMomCmd, F42_ADP_SET_TARGET_WHL_MOM_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_APP_CONFIG_DBG_CMD_FC,         F42_ADP_OBJ, F42_APP_ConfigDbgCmd,       F42_APP_CONFIG_DBG_CMD_DATA_LEN);

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
   struct FSWType*     Fsw = &(F42App.F42Adp.Fsw);
   char                DbgRecord[512];
   
   Status = CFE_SB_RcvMsg(&MsgPtr, F42App.SensorPipe, F42_APP_SENSOR_TIMEOUT);
   //OS_printf("F42::ProcessSensorData() SB recv status = 0x%08X\n",Status);	
   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(MsgPtr);
      //OS_printf("F42::ProcessSensorData() received msgid = 0x%04X\n",MsgId);	

      if (MsgId == F42_SENSOR_MID) {

         F42App.CtrlExeCnt++;
         
         SensorPkt = (F42_ADP_SensorPkt*)MsgPtr;
         F42_ADP_Run42Fsw(SensorPkt);
         
         if (F42App.DbgEnabled) {
            sprintf(DbgRecord,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
              Fsw->MOI[0],Fsw->MOI[1],Fsw->MOI[2], 
              Fsw->wc[0], Fsw->wc[1], Fsw->wc[2],
              Fsw->zc[0], Fsw->zc[1], Fsw->zc[2],
              Fsw->Kr[0], Fsw->Kr[1], Fsw->Kr[2],
              Fsw->Kp[0], Fsw->Kp[1], Fsw->Kp[2]);
            
            /*
            sprintf(DbgRecord,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
              Fsw->qbn[0],Fsw->qbn[1],Fsw->qbn[2],Fsw->qbn[3],Fsw->qbr[0]*2.0,Fsw->qbr[1]*2.0,Fsw->qbr[2]*2.0,Fsw->qbr[3],
              Fsw->Twhlcmd[0],Fsw->Twhlcmd[1],Fsw->Twhlcmd[2]);
            */
            OS_write(F42App.DbgFileHandle,DbgRecord,strlen(DbgRecord));

            /*
            F42App.F42Adp.Fsw.wbn[i];
            F42App.F42Adp.Fsw.qbr[i];
            F42App.F42Adp.Fsw.qbr[i]*2.0);
            F42App.F42Adp.Fsw.ierr[i];  // TODO - Hack to get Her to TLM
            F42App.F42Adp.Fsw.svb[i];
            F42App.F42Adp.Fsw.Twhlcmd[i];
            F42App.F42Adp.Fsw.Mmtbcmd[i];
            F42App.F42Adp.Fsw.GimCmd[0].Ang[0];
            F42App.F42Adp.Fsw.SunValid;
            */

         } /* End if debug enabled */
         
      }
      else {
      
         CFE_EVS_SendEvent(F42_APP_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                           "Received invalid sensor data packet,MID = 0x%4X",MsgId);
      
      } /* End if F42_SENSOR_MID */
      
   } /* End if SB received a packet */

} /* End ProcessSensorData() */
                          

                          
/******************************************************************************
** Function: SendCtrlTlm
**
*/
static void SendCtrlTlm(void)
{
   uint8 i;
   
   for (i=0; i<3; i++) {
      F42AppCtrlPkt.wbn[i]    = (float)F42App.F42Adp.Fsw.wbn[i];
      F42AppCtrlPkt.qbr[i]    = (float)F42App.F42Adp.Fsw.qbr[i];
      F42AppCtrlPkt.AttErr[i] = (float)(F42App.F42Adp.Fsw.qbr[i]*2.0);
      F42AppCtrlPkt.Herr[i]   = (float)F42App.F42Adp.Fsw.ierr[i];  // TODO - Hack to get Her to TLM
      F42AppCtrlPkt.svb[i]    = (float)F42App.F42Adp.Fsw.svb[i];
      F42AppCtrlPkt.WhlCmd[i] = (float)F42App.F42Adp.Fsw.Twhlcmd[i];
      F42AppCtrlPkt.MtbCmd[i] = (float)F42App.F42Adp.Fsw.Mmtbcmd[i];
   }
   F42AppCtrlPkt.qbr[3]   = (float)F42App.F42Adp.Fsw.qbr[3];
   F42AppCtrlPkt.GimCmd   = (float)F42App.F42Adp.Fsw.GimCmd[0].Ang[0];
   F42AppCtrlPkt.SunValid = F42App.F42Adp.Fsw.SunValid;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &F42AppCtrlPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &F42AppCtrlPkt);

} /* End SendCtrlTlm() */
                             
/* end of file */
