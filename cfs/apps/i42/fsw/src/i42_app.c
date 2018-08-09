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

I42_Class   I42;
I42_HkPkt   I42HkPkt;

/* Convenience macros */
#define  CMDMGR_OBJ (&(I42.CmdMgr))    
#define  NETIF_OBJ  (&(I42.NetIf))

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

   CFE_EVS_SendEvent(I42_EXIT_ERR_EID, CFE_EVS_CRITICAL, "I42 App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of I42_AppMain() */


/******************************************************************************
** Function: I42_NoOpCmd
**
*/

boolean I42_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (I42_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for I42 App version %d.%d",
                      I42_MAJOR_VERSION,I42_MINOR_VERSION);

   return TRUE;


} /* End I42_NoOpCmd() */


/******************************************************************************
** Function: I42_ResetAppCmd
**
*/

boolean I42_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   NETIF42_ResetStatus();
   

   return TRUE;

} /* End I42_ResetAppCmd() */


/******************************************************************************
** Function: I42_SendHousekeepingPkt
**
*/
void I42_SendHousekeepingPkt(void)
{

   /*
   ** I42 Application Data
   */

   I42HkPkt.ValidCmdCnt   = I42.CmdMgr.ValidCmdCnt;
   I42HkPkt.InvalidCmdCnt = I42.CmdMgr.InvalidCmdCnt;

   /*
   ** NETIF Data
   */

   I42HkPkt.SensorPktCnt   = I42.SensorPktCnt;
   I42HkPkt.ActuatorPktCnt = I42.ActuatorPktCnt;

   I42HkPkt.ConnectCycleCnt = I42.ConnectCycleCnt;
   I42HkPkt.Connected       = I42.NetIf.Connected;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &I42HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &I42HkPkt);

} /* End I42_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 Status = CFE_SUCCESS;

   CFE_PSP_MemSet((void*)&I42, 0, sizeof(I42_Class));

   I42.NoSensorDisconnectLim     = I42_NO_SENSOR_DISCONNECT_LIM;
   I42.NoSensorResendActuatorLim = I42_NO_SENSOR_RESEND_ACTUATOR_LIM;

   /*
   ** Initialize objects 
   */

   NETIF42_Constructor(NETIF_OBJ, I42_LOCAL_HOST_STR, I42_SOCKET_PORT);  

   /*
   ** Initialize app level interfaces
   */

   CFE_SB_CreatePipe(&I42.CmdPipe, I42_CMD_PIPE_DEPTH, I42_CMD_PIPE_NAME);
   CFE_SB_Subscribe(I42_CMD_MID, I42.CmdPipe);
   CFE_SB_Subscribe(I42_SEND_HK_MID, I42.CmdPipe);

   CMDMGR_Constructor(CMDMGR_OBJ);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, I42_NoOpCmd,     0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, I42_ResetAppCmd, 0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, I42_NETIF_CONNECT_42_CMD_FC,    NETIF_OBJ, NETIF42_Connect42Cmd,    NETIF_CONNECT_42_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, I42_NETIF_DISCONNECT_42_CMD_FC, NETIF_OBJ, NETIF42_Disconnect42Cmd, NETIF_DISCONNECT_42_CMD_DATA_LEN);

   CFE_SB_CreatePipe(&I42.ActuatorPipe,I42_ACTUATOR_PIPE_DEPTH, I42_ACTUATOR_PIPE_NAME);
   CFE_SB_Subscribe(F42_ACTUATOR_MID, I42.ActuatorPipe);

   CFE_SB_InitMsg(&I42.SensorPkt, F42_SENSOR_MID, F42_ADP_SENSOR_PKT_LEN, TRUE);
   CFE_SB_InitMsg(&I42HkPkt, I42_HK_TLM_MID, I42_TLM_HK_LEN, TRUE);

       
   OS_TaskInstallDeleteHandler((void *)(&AppTermCallback)); /* Call when application terminates */

   /*
   ** Application startup event message
   */
   Status = CFE_EVS_SendEvent(I42_INIT_APP_INFO_EID,
                              CFE_EVS_INFORMATION,
                              "I42 App Initialized. Version %d.%d.%d.%d",
                              I42_MAJOR_VERSION,
                              I42_MINOR_VERSION,
                              I42_REVISION,
                              I42_MISSION_REV);

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

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, I42.CmdPipe, CFE_SB_POLL);
   //Status = CFE_SB_RcvMsg(&CmdMsgPtr, I42.CmdPipe, 2000);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case I42_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case I42_SEND_HK_MID:
            I42_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(I42_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
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
  
   CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG, "I42::ProcessSensorData(): Connected=%d, ConnectCycleCnt=%d, SensorPktSent=%d, NoSensorCnt=%d\n",
                     I42.NetIf.Connected,I42.ConnectCycleCnt,I42.SensorPktSent,I42.NoSensorDisconnectCnt);

   if (I42.NetIf.Connected == TRUE) {

      I42.ConnectCycleCnt++;

      Status = NETIF42_Recv(I42.InBuf, sizeof(I42.InBuf));
      CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG, "I42::ProcessSensorData():NETIF42_Recv() status = %d\n",Status);	  
	   if( Status > 0) {
         	  
         CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG,"Sensor0: %s\n",&I42.InBuf[100]);
		 
         //if ((Status = sscanf(I42.InBuf,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d",
         if ((Status = sscanf(I42.InBuf,"%le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %le %d",
            &(I42.SensorPkt.DeltaTime),
            &(I42.SensorPkt.PosN[0]),&(I42.SensorPkt.PosN[1]),&(I42.SensorPkt.PosN[2]),
            &(I42.SensorPkt.VelN[0]),&(I42.SensorPkt.VelN[1]),&(I42.SensorPkt.VelN[2]),
            &(I42.SensorPkt.wbn[0]), &(I42.SensorPkt.wbn[1]), &(I42.SensorPkt.wbn[2]),
            &(I42.SensorPkt.qbn[0]), &(I42.SensorPkt.qbn[1]), &(I42.SensorPkt.qbn[2]),&(I42.SensorPkt.qbn[3]),
            &(I42.SensorPkt.svn[0]), &(I42.SensorPkt.svn[1]), &(I42.SensorPkt.svn[2]),
            &(I42.SensorPkt.svb[0]), &(I42.SensorPkt.svb[1]), &(I42.SensorPkt.svb[2]),
            &(I42.SensorPkt.bvb[0]), &(I42.SensorPkt.bvb[1]), &(I42.SensorPkt.bvb[2]),
            &(I42.SensorPkt.Hw[0]),  &(I42.SensorPkt.Hw[1]),  &(I42.SensorPkt.Hw[2]),
            &(I42.SensorPkt.SunValid))) == 27) {

            CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG,"I42::ProcessSensorData(): Hw = [%.8e, %.8e, %.8e]\n",
		                        I42.SensorPkt.Hw[0],I42.SensorPkt.Hw[1],I42.SensorPkt.Hw[2]);	
            
            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &(I42.SensorPkt));
            Status = CFE_SB_SendMsg((CFE_SB_Msg_t *) &(I42.SensorPkt));
   
            I42.SensorPktSent = TRUE;
            I42.SensorPktCnt++;
            ProcessedPkt = TRUE;
         
         } /* End sscanf() */

         I42.NoSensorDisconnectCnt = 0;

         CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG, "Status = %d\n",Status);
      
      } /* End if fgets() */
      else {
      
	      CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG, "I42::ProcessSensorData():sscanf() failed status = %d\n",Status);
         
		   I42.NoSensorDisconnectCnt++;
         
		 
         if ( (I42.NoSensorDisconnectCnt >= I42.NoSensorResendActuatorLim) && !I42.ActuatorResend ) {
            
            CFE_EVS_SendEvent(I42_RESEND_ACTUATOR_PKT_INFO_EID, CFE_EVS_INFORMATION,
                              "Resending actuator packet after %d cycles of no sensor data packets", I42.NoSensorDisconnectCnt);
		    
            SendActuatorPkt(I42.OutBuf, strlen(I42.OutBuf));
          
            I42.ActuatorResend = TRUE;
			
         } /* End if resend actuator packet */
		 		 	  
         if (I42.NoSensorDisconnectCnt >= I42.NoSensorDisconnectLim) {
			 
            CFE_EVS_SendEvent(I42_IDLE_SOCKET_CLOSE_INFO_EID, CFE_EVS_INFORMATION,
                              "Closing 42 socket. No data received for %d attempts.", I42.NoSensorDisconnectCnt);
            
            AppTermCallback();
         
         } /* End if disconnect */

      }
   } /* End if NetIf->Connected */
   else {
      
      CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG, "I42 NETIF: NETIF_ProcessSensorPkt() called with no connection\n");
      I42.ConnectCycleCnt = 0;;
   
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
   if (I42.SensorPktSent) {
      //Status = CFE_SB_RcvMsg(&MsgPtr, I42.ActuatorPipe, CFE_SB_POLL);
      Status = CFE_SB_RcvMsg(&MsgPtr, I42.ActuatorPipe, I42_ACTUATOR_TIMEOUT);
   }
   else {
      if (I42.NetIf.Connected) {
         //Status = CFE_SB_RcvMsg(&MsgPtr, I42.ActuatorPipe, I42_ACTUATOR_TIMEOUT);
         Status = CFE_SB_RcvMsg(&MsgPtr, I42.ActuatorPipe, 10);
      }
      else {
         Status = CFE_SB_RcvMsg(&MsgPtr, I42.ActuatorPipe, 1000);
      }
   }
   
   CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG, "I42::ProcessActuatorData(): SensorPktSent = %d, SB Recv status = 0x%08X\n",I42.SensorPktSent, Status);
   if (Status == CFE_SUCCESS) {

      I42.SensorPktSent = FALSE;

      MsgId = CFE_SB_GetMsgId(MsgPtr);

      if (MsgId == F42_ACTUATOR_MID) {

         ActuatorPkt = (F42_ADP_ActuatorPkt*)MsgPtr;
 		 
         CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG,"WhlTorqCmd[0] = %18.8e, WhlTorqCmd[1] = %18.8e, WhlTorqCmd[2] = %18.8e,\n",
                           ActuatorPkt->WhlTorqCmd[0],ActuatorPkt->WhlTorqCmd[1],ActuatorPkt->WhlTorqCmd[2]);
         CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG,"MtbCmd[0] = %18.8e, MtbCmd[1] = %18.8e, MtbCmd[2] = %18.8e,\n",
                           ActuatorPkt->MtbCmd[0],ActuatorPkt->MtbCmd[1],ActuatorPkt->MtbCmd[2]);
         CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG,"SaGimbalCmd = %18.8e\n", ActuatorPkt->SaGimbalCmd);
       
         //snprintf(I42.OutBuf, sizeof(I42.OutBuf), "%lf %lf %lf %lf %lf %lf %lf\n",
         snprintf(I42.OutBuf, sizeof(I42.OutBuf), "%18.8e %18.8e %18.8e %18.8e %18.8e %18.8e %18.8e\n",
            ActuatorPkt->WhlTorqCmd[0], ActuatorPkt->WhlTorqCmd[1], ActuatorPkt->WhlTorqCmd[2],
            ActuatorPkt->MtbCmd[0], ActuatorPkt->MtbCmd[1], ActuatorPkt->MtbCmd[2],
            ActuatorPkt->SaGimbalCmd);
	     
         PktSent = SendActuatorPkt(I42.OutBuf, strlen(I42.OutBuf));
		 		 
      } /* end if F42_ACTUATOR_MID */
      else {
      
         CFE_EVS_SendEvent(I42_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
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
   
   CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG, "I42::ProcessActutaorData(): Sending %s\n",I42.OutBuf);
   Status = NETIF42_Send (Buf, Len);
         
   if (Status > 0) {
      PktSent = TRUE;
      I42.ActuatorPktCnt++;
      I42.ActuatorResend = FALSE;
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
 
   I42.ConnectCycleCnt = 0;
   I42.SensorPktSent = 0;
   I42.ActuatorPktSent = 0;
   I42.ActuatorResend = FALSE;
			
   NETIF42_Close();
   
} /* End AppTermCallback() */

/* end of file */
