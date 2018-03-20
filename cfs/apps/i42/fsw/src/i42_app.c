/*
** Purpose: Implement the 42 FSW Interface application
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
#include "i42_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);
static void AppTermCallback(void);

static boolean ProcessSensorData(void);
static boolean ProcessActuatorData(void);
static boolean SendActuatorPkt(const char* Buf, uint16 Len);

/*
** Global Data
*/

I42_APP_Class   I42App;
I42_APP_HkPkt   I42AppHkPkt;

/* Convenience macros */
#define  CMDMGR_OBJ (&(I42App.CmdMgr))    
#define  NETIF_OBJ  (&(I42App.NetIf))

/******************************************************************************
** Function: I42_AppMain
**
*/
void I42_AppMain(void)
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
   ** - Sensor data and command pipe are polled
   ** - See ProcessActuatorData() for control loop time management 
   */
   while (CFE_ES_RunLoop(&RunStatus)) {

      ProcessSensorData();
      ProcessActuatorData();
      ProcessCommands();
      
   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("I42 App terminating, err = 0x%08X\n", Status);

   CFE_EVS_SendEvent(I42_APP_EXIT_ERR_EID, CFE_EVS_CRITICAL, "I42 App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of I42_AppMain() */


/******************************************************************************
** Function: I42_APP_NoOpCmd
**
*/

boolean I42_APP_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (I42_APP_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for I42 App version %d.%d",
                      I42_APP_MAJOR_VERSION,I42_APP_MINOR_VERSION);

   return TRUE;


} /* End I42_APP_NoOpCmd() */


/******************************************************************************
** Function: I42_APP_ResetAppCmd
**
*/

boolean I42_APP_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   NETIF42_ResetStatus();
   

   return TRUE;

} /* End I42_APP_ResetAppCmd() */


/******************************************************************************
** Function: I42_APP_SendHousekeepingPkt
**
*/
void I42_APP_SendHousekeepingPkt(void)
{

   /*
   ** I42 Application Data
   */

   I42AppHkPkt.ValidCmdCnt   = I42App.CmdMgr.ValidCmdCnt;
   I42AppHkPkt.InvalidCmdCnt = I42App.CmdMgr.InvalidCmdCnt;

   /*
   ** NETIF Data
   */

   I42AppHkPkt.SensorPktCnt   = I42App.SensorPktCnt;
   I42AppHkPkt.ActuatorPktCnt = I42App.ActuatorPktCnt;

   I42AppHkPkt.ConnectCycleCnt = I42App.ConnectCycleCnt;
   I42AppHkPkt.Connected       = I42App.NetIf.Connected;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &I42AppHkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &I42AppHkPkt);

} /* End I42_APP_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 Status = CFE_SUCCESS;

   CFE_PSP_MemSet((void*)&I42App, 0, sizeof(I42_APP_Class));

   I42App.NoSensorDisconnectLim     = I42_APP_NO_SENSOR_DISCONNECT_LIM;
   I42App.NoSensorResendActuatorLim = I42_APP_NO_SENSOR_RESEND_ACTUATOR_LIM;

   /*
   ** Initialize objects 
   */

   NETIF42_Constructor(NETIF_OBJ, I42_LOCAL_HOST_STR, I42_SOCKET_PORT);  

   /*
   ** Initialize app level interfaces
   */

   CFE_SB_CreatePipe(&I42App.CmdPipe, I42_CMD_PIPE_DEPTH, I42_CMD_PIPE_NAME);
   CFE_SB_Subscribe(I42_CMD_MID, I42App.CmdPipe);
   CFE_SB_Subscribe(I42_SEND_HK_MID, I42App.CmdPipe);

   CMDMGR_Constructor(CMDMGR_OBJ);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, I42_APP_NoOpCmd,     0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, I42_APP_ResetAppCmd, 0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, I42_NETIF_CONNECT_42_CMD_FC,    NETIF_OBJ, NETIF42_Connect42Cmd,    NETIF_CONNECT_42_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, I42_NETIF_DISCONNECT_42_CMD_FC, NETIF_OBJ, NETIF42_Disconnect42Cmd, NETIF_DISCONNECT_42_CMD_DATA_LEN);

   CFE_SB_CreatePipe(&I42App.ActuatorPipe,I42_ACTUATOR_PIPE_DEPTH, I42_ACTUATOR_PIPE_NAME);
   CFE_SB_Subscribe(F42_ACTUATOR_MID, I42App.ActuatorPipe);

   CFE_SB_InitMsg(&I42App.SensorPkt, F42_SENSOR_MID, F42_ADP_SENSOR_PKT_LEN, TRUE);
   CFE_SB_InitMsg(&I42AppHkPkt, I42_HK_TLM_MID, I42_APP_TLM_HK_LEN, TRUE);

       
   OS_TaskInstallDeleteHandler((void *)(&AppTermCallback)); /* Call when application terminates */

   /*
   ** Application startup event message
   */
   Status = CFE_EVS_SendEvent(I42_APP_INIT_APP_INFO_EID,
                              CFE_EVS_INFORMATION,
                              "I42 App Initialized. Version %d.%d.%d.%d",
                              I42_APP_MAJOR_VERSION,
                              I42_APP_MINOR_VERSION,
                              I42_APP_REVISION,
                              I42_APP_MISSION_REV);

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

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, I42App.CmdPipe, CFE_SB_POLL);
   //Status = CFE_SB_RcvMsg(&CmdMsgPtr, I42App.CmdPipe, 2000);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case I42_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case I42_SEND_HK_MID:
            I42_APP_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(I42_APP_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */



/******************************************************************************
** Function:  ProcessSensorData
**
** Automatically disconnect if nothing read from socket. 
*/
static boolean ProcessSensorData(void) 
{

   int32 Status;
   boolean ProcessedPkt = FALSE;
  
   //OS_printf("I42::ProcessSensorData(): Connected=%d, ConnectCycleCnt=%d, SensorPktSent=%d, NoSensorCnt=%d\n",
   //           I42App.NetIf.Connected,I42App.ConnectCycleCnt,I42App.SensorPktSent,I42App.NoSensorDisconnectCnt);

   if (I42App.NetIf.Connected == TRUE) {

      I42App.ConnectCycleCnt++;

	  Status = NETIF42_Recv(I42App.InBuf, sizeof(I42App.InBuf));
     //OS_printf("I42::ProcessSensorData():NETIF42_Recv() status = %d\n",Status);	  
	  if( Status > 0) {
         	  
         //OS_printf("Sensor0: %s\n",I42App.InBuf);
         //OS_printf("Sensor0: %s\n",&I42App.InBuf[100]);
		 
         //if ((Status = sscanf(I42App.InBuf,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d",
         if ((Status = sscanf(I42App.InBuf,"%le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %d",
            &(I42App.SensorPkt.DeltaTime),
            &(I42App.SensorPkt.PosN[0]),&(I42App.SensorPkt.PosN[1]),&(I42App.SensorPkt.PosN[2]),
            &(I42App.SensorPkt.VelN[0]),&(I42App.SensorPkt.VelN[1]),&(I42App.SensorPkt.VelN[2]),
            &(I42App.SensorPkt.wbn[0]), &(I42App.SensorPkt.wbn[1]), &(I42App.SensorPkt.wbn[2]),
            &(I42App.SensorPkt.qbn[0]), &(I42App.SensorPkt.qbn[1]), &(I42App.SensorPkt.qbn[2]),&(I42App.SensorPkt.qbn[3]),
            &(I42App.SensorPkt.svn[0]), &(I42App.SensorPkt.svn[1]), &(I42App.SensorPkt.svn[2]),
            &(I42App.SensorPkt.svb[0]), &(I42App.SensorPkt.svb[1]), &(I42App.SensorPkt.svb[2]),
            &(I42App.SensorPkt.bvb[0]), &(I42App.SensorPkt.bvb[1]), &(I42App.SensorPkt.bvb[2]),
            &(I42App.SensorPkt.Hw[0]),  &(I42App.SensorPkt.Hw[1]),  &(I42App.SensorPkt.Hw[2]),
            &(I42App.SensorPkt.SunValid))) == 27) {

            //OS_printf("I42::ProcessSensorData(): Hw = [%.8e, %.8e, %.8e]\n",
		      //          I42App.SensorPkt.Hw[0],I42App.SensorPkt.Hw[1],I42App.SensorPkt.Hw[2]);	
            //OS_printf("Real Data passed\n");
	 
            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &(I42App.SensorPkt));
            Status = CFE_SB_SendMsg((CFE_SB_Msg_t *) &(I42App.SensorPkt));
            //OS_printf("I42::ProcessSensorData(): SB send status = 0x%08X\n",Status);	
   
            I42App.SensorPktSent = TRUE;
            I42App.SensorPktCnt++;
            ProcessedPkt = TRUE;
         
         } /* End sscanf() */

         I42App.NoSensorDisconnectCnt = 0;

         //OS_printf("Status = %d\n",Status);
      } /* End if fgets() */
      else {
      
	     //OS_printf("I42::ProcessSensorData():sscanf() failed status = %d\n",Status);
         
		 I42App.NoSensorDisconnectCnt++;
         
		 
         if ( (I42App.NoSensorDisconnectCnt >= I42App.NoSensorResendActuatorLim) && !I42App.ActuatorResend ) {
            
			CFE_EVS_SendEvent(I42_APP_RESEND_ACTUATOR_PKT_INFO_EID, CFE_EVS_INFORMATION,
                              "Resending actuator packet after %d cycles of no sensor data packets", I42App.NoSensorDisconnectCnt);
		    
			SendActuatorPkt(I42App.OutBuf, strlen(I42App.OutBuf));
          
		    I42App.ActuatorResend = TRUE;
			
		 } /* End if resend actuator packet */
		 		 	  
	     if (I42App.NoSensorDisconnectCnt >= I42App.NoSensorDisconnectLim) {
			 
            CFE_EVS_SendEvent(I42_APP_IDLE_SOCKET_CLOSE_INFO_EID, CFE_EVS_INFORMATION,
                              "Closing 42 socket. No data received for %d attempts.", I42App.NoSensorDisconnectCnt);
            
			AppTermCallback();
         
		 } /* End if disconnect */

      }
   } /* End if NetIf->Connected */
   else {
      // OS_printf("I42 NETIF: NETIF_ProcessSensorPkt() called with no connection\n");
      I42App.ConnectCycleCnt = 0;;
   }

   return ProcessedPkt;
   
} /* NETIF_ProcessSensorData() */
            
            
/******************************************************************************
** Function: ProcessActuatorData
**
** 
*/
static boolean ProcessActuatorData(void)
{

   int32                 Status;
   boolean               PktSent = FALSE;
   CFE_SB_Msg_t*         MsgPtr;
   CFE_SB_MsgId_t        MsgId;
   F42_ADP_ActuatorPkt*  ActuatorPkt;

   /* 
   ** After a sensor packet is sent use 'long' timeout which should just pend until the
   ** the actuator packet arrives. 
   **
   ** After the actuator packet is sent to 42 use a short timeout so this function will
   ** exit and the sensor data from 42 can be polled. Using a blocked call for sensor 
   ** data has issues which is why actuator packets are used for control flow. 
   **
   ** The short timeout makes the system very responsive during startup, shutdown, and in error situations.
   **
   ** A third timeout value is used when a 42 sim is not in progress.
   */
   if (I42App.SensorPktSent) {
      //Status = CFE_SB_RcvMsg(&MsgPtr, I42App.ActuatorPipe, CFE_SB_POLL);
      Status = CFE_SB_RcvMsg(&MsgPtr, I42App.ActuatorPipe, I42_APP_ACTUATOR_TIMEOUT);
   }
   else {
      if (I42App.NetIf.Connected) {
         //Status = CFE_SB_RcvMsg(&MsgPtr, I42App.ActuatorPipe, I42_APP_ACTUATOR_TIMEOUT);
         Status = CFE_SB_RcvMsg(&MsgPtr, I42App.ActuatorPipe, 10);
      }
      else {
         Status = CFE_SB_RcvMsg(&MsgPtr, I42App.ActuatorPipe, 1000);
      }
   }
   
   //OS_printf("I42::ProcessActuatorData(): SensorPktSent = %d, SB Recv status = 0x%08X\n",I42App.SensorPktSent, Status);
   if (Status == CFE_SUCCESS) {

      I42App.SensorPktSent = FALSE;

      MsgId = CFE_SB_GetMsgId(MsgPtr);

      if (MsgId == F42_ACTUATOR_MID) {

	     ActuatorPkt = (F42_ADP_ActuatorPkt*)MsgPtr;
 		 
       //OS_printf(">WhlTorqCmd[0] = %18.8e, WhlTorqCmd[1] = %18.8e, WhlTorqCmd[2] = %18.8e,\n",
		 //        ActuatorPkt->WhlTorqCmd[0],ActuatorPkt->WhlTorqCmd[1],ActuatorPkt->WhlTorqCmd[2]);
       //OS_printf(">MtbCmd[0] = %18.8e, MtbCmd[1] = %18.8e, MtbCmd[2] = %18.8e,\n",
		 //        ActuatorPkt->MtbCmd[0],ActuatorPkt->MtbCmd[1],ActuatorPkt->MtbCmd[2]);
       //OS_printf(">SaGimbalCmd = %18.8e\n", ActuatorPkt->SaGimbalCmd);
       
		 //snprintf(I42App.OutBuf, sizeof(I42App.OutBuf), "%lf %lf %lf %lf %lf %lf %lf\n",
		 snprintf(I42App.OutBuf, sizeof(I42App.OutBuf), "%18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e\n",
            ActuatorPkt->WhlTorqCmd[0], ActuatorPkt->WhlTorqCmd[1], ActuatorPkt->WhlTorqCmd[2],
            ActuatorPkt->MtbCmd[0], ActuatorPkt->MtbCmd[1], ActuatorPkt->MtbCmd[2],
            ActuatorPkt->SaGimbalCmd);
	     
		 PktSent = SendActuatorPkt(I42App.OutBuf, strlen(I42App.OutBuf));
		 		 
      } /* end if F42_ACTUATOR_MID */
      else {
      
         CFE_EVS_SendEvent(I42_APP_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                           "Received invalid actuator data packet,MID = 0x%4X",MsgId);
      
      } /* End if !I42_ACTUATOR_MID */
      
   } /* End if SB received a packet */

   return PktSent;
   
} /* End ProcessActuatorData() */


/******************************************************************************
** Function: SendActuatorPkt
**
*/
static boolean SendActuatorPkt(const char* Buf, uint16 Len) {
 
   int32                 Status;
   boolean               PktSent = FALSE;
   
   //OS_printf("I42::ProcessActutaorData(): Sending %s\n",I42App.OutBuf);
   Status = NETIF42_Send (Buf, Len);
         
   if (Status > 0) {
      PktSent = TRUE;
      I42App.ActuatorPktCnt++;
      I42App.ActuatorResend = FALSE;
   }
   
   return PktSent;
   
} /* End SendActuatorPkt() */

          		  
/******************************************************************************
** Function: AppTermCallback
**
** This function is called when the app is terminated. This should
** never occur but if it does this will close the network interfaces.
*/
static void AppTermCallback(void) {
 
   I42App.ConnectCycleCnt = 0;
   I42App.SensorPktSent = 0;
   I42App.ActuatorPktSent = 0;
   I42App.ActuatorResend = FALSE;
			
   NETIF42_Close();
   
} /* End AppTermCallback() */

/* end of file */
