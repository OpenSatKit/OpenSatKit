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

F42_Class   F42;
F42_HkPkt   F42HkPkt;
F42_CtrlPkt F42CtrlPkt;

/* Convenience macro */
#define  CMDMGR_OBJ  (&(F42.CmdMgr))  
#define  TBLMGR_OBJ  (&(F42.TblMgr))
#define  F42_ADP_OBJ (&(F42.F42Adp))
#define  CTRLTBL_OBJ (&(F42.F42Adp.CtrlTbl))

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

   CFE_EVS_SendEvent(F42_EXIT_ERR_EID, CFE_EVS_CRITICAL, "F42 App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of F42_AppMain() */


/******************************************************************************
** Function: F42_NoOpCmd
**
*/

boolean F42_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (F42_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for F42 App version %d.%d",
                      F42_MAJOR_VERSION,F42_MINOR_VERSION);

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

   F42.CtrlExeCnt = 0;
   F42_ADP_ResetStatus();
	  
   return TRUE;

} /* End F42_ResetAppCmd() */


/******************************************************************************
** Function: F42_ConfigDbgCmd
**
** TODO - Add file command parameter
*/

boolean F42_ConfigDbgCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const F42_ConfigDbgCmdPkt *ConfigDbgCmd = (const F42_ConfigDbgCmdPkt *) MsgPtr;
   boolean  RetStatus = FALSE;

   if ( ConfigDbgCmd->NewState == TRUE) {
     
      if (F42.DbgEnabled == TRUE) {
         
         CFE_EVS_SendEvent(F42_DEBUG_CMD_ERR_EID, CFE_EVS_ERROR, "Enable debug command rejected; debug file currently opened.");
      
      }
      else {
         
         F42.DbgFileHandle = OS_creat(F42_DBG_FILE, OS_WRITE_ONLY);
         
         if (F42.DbgFileHandle >= OS_FS_SUCCESS) {
            
            F42.DbgEnabled = TRUE;
            CFE_EVS_SendEvent(F42_DEBUG_CMD_INFO_EID, CFE_EVS_INFORMATION, "Created debug file %s",F42_DBG_FILE);
            RetStatus = TRUE;
         
         }
         else {

            CFE_EVS_SendEvent(F42_DEBUG_CMD_ERR_EID, CFE_EVS_ERROR, "Error creating debug file %s",F42_DBG_FILE);
         
         } /* End if error creating file */
      } /* End if debug not enabled */
   } /* End if enable debug command */
   else {
      
      if (F42.DbgEnabled == TRUE) {
         
         F42.DbgEnabled = FALSE;
         OS_close(F42.DbgFileHandle);
         CFE_EVS_SendEvent(F42_DEBUG_CMD_INFO_EID, CFE_EVS_INFORMATION, "Debug file %s closed", F42_DBG_FILE);
      
      }
      else {

         CFE_EVS_SendEvent(F42_DEBUG_CMD_INFO_EID, CFE_EVS_INFORMATION, "Disable debug command rejected; debug not enabled.");
      }
      
   } /* End if disable debug command */
        
   return RetStatus;

} /* End F42_ConfigDbgCmd() */


/******************************************************************************
** Function: F42_SendHousekeepingPkt
**
*/
void F42_SendHousekeepingPkt(void)
{
   int i;
   const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);
	  
   /*
   ** F42 Application Data
   */

   F42HkPkt.ValidCmdCnt   = F42.CmdMgr.ValidCmdCnt;
   F42HkPkt.InvalidCmdCnt = F42.CmdMgr.InvalidCmdCnt;

   F42HkPkt.LastAction       = LastTbl->LastAction;
   F42HkPkt.LastActionStatus = LastTbl->LastActionStatus;

   F42HkPkt.CtrlExeCnt = F42.CtrlExeCnt;

   /*
   ** 42 FSW Adapter Data
   */

   F42HkPkt.CtrlMode     = F42.F42Adp.CtrlMode;
   F42HkPkt.OvrSunValid  = F42.F42Adp.Override[F42_ADP_OVR_SUN_VALID];

   for (i=0; i < 3; i++) {
      F42HkPkt.wc[i] = F42.F42Adp.Fsw.wc[i];
      F42HkPkt.zc[i] = F42.F42Adp.Fsw.zc[i];
      F42HkPkt.Kr[i] = F42.F42Adp.Fsw.Kr[i];
      F42HkPkt.Kp[i] = F42.F42Adp.Fsw.Kp[i];
      F42HkPkt.Hwcmd[i] = F42.F42Adp.Fsw.Hwcmd[i];
   }

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &F42HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &F42HkPkt);

} /* End F42_SendHousekeepingPkt() */


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

   CFE_SB_CreatePipe(&F42.CmdPipe, F42_CMD_PIPE_DEPTH, F42_CMD_PIPE_NAME);
   CFE_SB_Subscribe(F42_CMD_MID, F42.CmdPipe);
   CFE_SB_Subscribe(F42_SEND_HK_MID, F42.CmdPipe);

   CMDMGR_Constructor(CMDMGR_OBJ);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, F42_NoOpCmd,     0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, F42_ResetAppCmd, 0);

   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_TBL_LOAD_CMD_FC, TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_TBL_DUMP_CMD_FC, TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);

   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_CTRL_MODE_CMD_FC,      F42_ADP_OBJ, F42_ADP_SetCtrlModeCmd,     F42_ADP_SET_CTRL_MODE_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_OVR_CMD_FC,            F42_ADP_OBJ, F42_ADP_SetOvrCmd,          F42_ADP_SET_OVR_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_ADP_SET_TARGET_WHL_MOM_CMD_FC, F42_ADP_OBJ, F42_ADP_SetTargetWhlMomCmd, F42_ADP_SET_TARGET_WHL_MOM_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, F42_CONFIG_DBG_CMD_FC,             F42_ADP_OBJ, F42_ConfigDbgCmd,       F42_CONFIG_DBG_CMD_DATA_LEN);

	TBLMGR_Constructor(TBLMGR_OBJ);
   TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, CTRLTBL_LoadCmd,  CTRLTBL_DumpCmd,  F42_CTRL_TBL_DEF_LOAD_FILE);

   CFE_SB_CreatePipe(&F42.SensorPipe,F42_SENSOR_PIPE_DEPTH, F42_SENSOR_PIPE_NAME);
   CFE_SB_Subscribe(F42_SENSOR_MID, F42.SensorPipe);

   CFE_SB_InitMsg(&F42HkPkt, F42_HK_TLM_MID, F42_TLM_HK_LEN, TRUE);
   CFE_SB_InitMsg(&F42CtrlPkt, F42_CONTROL_MID, F42_TLM_CTRL_PKT_LEN, TRUE);

      
   /*
   ** Application startup event message
   */
   Status = CFE_EVS_SendEvent(F42_INIT_APP_INFO_EID,
                              CFE_EVS_INFORMATION,
                              "F42 App Initialized. Version %d.%d.%d.%d",
                              F42_MAJOR_VERSION,
                              F42_MINOR_VERSION,
                              F42_REVISION,
                              F42_MISSION_REV);

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

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, F42.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case F42_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case F42_SEND_HK_MID:
            F42_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(F42_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
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
   struct FSWType*     Fsw = &(F42.F42Adp.Fsw);
   char                DbgRecord[512];
   
   Status = CFE_SB_RcvMsg(&MsgPtr, F42.SensorPipe, F42_SENSOR_TIMEOUT);
   CFE_EVS_SendEvent(F42_DEBUG_EID, CFE_EVS_DEBUG, "F42::ProcessSensorData() SB recv status = 0x%08X\n",Status);	
   
   if (Status == CFE_SUCCESS) {

      MsgId = CFE_SB_GetMsgId(MsgPtr);
      CFE_EVS_SendEvent(F42_DEBUG_EID, CFE_EVS_DEBUG, "F42::ProcessSensorData() received msgid = 0x%04X\n",MsgId);	

      if (MsgId == F42_SENSOR_MID) {

         F42.CtrlExeCnt++;
         
         SensorPkt = (F42_ADP_SensorPkt*)MsgPtr;
         F42_ADP_Run42Fsw(SensorPkt);
         
         if (F42.DbgEnabled) {
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
            OS_write(F42.DbgFileHandle,DbgRecord,strlen(DbgRecord));

            /*
            F42.F42Adp.Fsw.wbn[i];
            F42.F42Adp.Fsw.qbr[i];
            F42.F42Adp.Fsw.qbr[i]*2.0);
            F42.F42Adp.Fsw.ierr[i];  // TODO - Hack to get Her to TLM
            F42.F42Adp.Fsw.svb[i];
            F42.F42Adp.Fsw.Twhlcmd[i];
            F42.F42Adp.Fsw.Mmtbcmd[i];
            F42.F42Adp.Fsw.GimCmd[0].Ang[0];
            F42.F42Adp.Fsw.SunValid;
            */

         } /* End if debug enabled */
         
      }
      else {
      
         CFE_EVS_SendEvent(F42_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
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
      F42CtrlPkt.wbn[i]    = (float)F42.F42Adp.Fsw.wbn[i];
      F42CtrlPkt.qbr[i]    = (float)F42.F42Adp.Fsw.qbr[i];
      F42CtrlPkt.AttErr[i] = (float)(F42.F42Adp.Fsw.qbr[i]*2.0);
      F42CtrlPkt.Herr[i]   = (float)F42.F42Adp.Fsw.ierr[i];  // TODO - Hack to get Her to TLM
      F42CtrlPkt.svb[i]    = (float)F42.F42Adp.Fsw.svb[i];
      F42CtrlPkt.WhlCmd[i] = (float)F42.F42Adp.Fsw.Twhlcmd[i];
      F42CtrlPkt.MtbCmd[i] = (float)F42.F42Adp.Fsw.Mmtbcmd[i];
   }
   F42CtrlPkt.qbr[3]   = (float)F42.F42Adp.Fsw.qbr[3];
   F42CtrlPkt.GimCmd   = (float)F42.F42Adp.Fsw.GimCmd[0].Ang[0];
   F42CtrlPkt.SunValid = F42.F42Adp.Fsw.SunValid;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &F42CtrlPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &F42CtrlPkt);

} /* End SendCtrlTlm() */
                             
/* end of file */
