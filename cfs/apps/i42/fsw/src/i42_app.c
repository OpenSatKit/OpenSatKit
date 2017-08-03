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
#include "i42_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);
static void ProcessActuatorData(void);
static void AppTermCallback(void);

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
   ** - ProcessSensorPkt() pends indefinitely for sensor data
   ** - Command processing polls the command queue
   */
   while (CFE_ES_RunLoop(&RunStatus)) {

      NETIF_ProcessSensorPkt();
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

   NETIF_ResetStatus();
   
   CMDMGR_ResetStatus(CMDMGR_OBJ);

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

   I42AppHkPkt.SensorPktCnt   = I42App.NetIf.SensorPktCnt;
   I42AppHkPkt.ActuatorPktCnt = I42App.NetIf.ActuatorPktCnt;

   I42AppHkPkt.ConnectCycles  = I42App.NetIf.ConnectCycles;
   I42AppHkPkt.Connected      = I42App.NetIf.Connected;

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

    /*
    ** Initialize objects 
    */

    NET42IF_Constructor(&I42App.NetIf);


    /*
    ** Initialize app level interfaces
    */

    CFE_SB_CreatePipe(&I42App.CmdPipe, I42_CMD_PIPE_DEPTH, I42_CMD_PIPE_NAME);
    CFE_SB_Subscribe(I42_CMD_MID, I42App.CmdPipe);
    CFE_SB_Subscribe(I42_SEND_HK_MID, I42App.CmdPipe);

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, I42_APP_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, I42_APP_ResetAppCmd, 0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, I42_NETIF_CONNECT_TO_42_CMD_FC, NETIF_OBJ, NETIF_ConnectTo42Cmd, NETIF_CONNECT_TO_42_CMD_DATA_LEN);

    CFE_SB_CreatePipe(&I42App.ActuatorPipe,I42_ACTUATOR_PIPE_DEPTH, I42_ACTUATOR_PIPE_NAME);
    CFE_SB_Subscribe(F42_ACTUATOR_MID, I42App.ActuatorPipe);

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
** Function: ProcessActuatorData
**
*/
static void ProcessActuatorData(void)
{

   int32                 Status;
   CFE_SB_Msg_t*         MsgPtr;
   CFE_SB_MsgId_t        MsgId;


   Status = CFE_SB_RcvMsg(&MsgPtr, I42App.ActuatorPipe, I42_APP_ACTUATOR_TIMEOUT);
   
   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(MsgPtr);

      if (MsgId == F42_ACTUATOR_MID) {

         NETIF_SendActuatorPkt((F42_ADP_ActuatorPkt*)MsgPtr);
         
      } /* end if F42_ACTUATOR_MID */
      else {
      
         CFE_EVS_SendEvent(I42_APP_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                           "Received invalid actuator data packet,MID = 0x%4X",MsgId);
      
      } /* End if !I42_ACTUATOR_MID */
      
   } /* End if SB received a packet */

} /* End ProcessActuatorData() */
                          
/******************************************************************************
** Function: AppTermCallback
**
** This function is called when the app is terminated. This should
** never occur but if it does this will close the network interfaces.
*/
static void AppTermCallback(void) {
 
   NETIF_Close();
   
} /* End AppTermCallback() */

/* end of file */
