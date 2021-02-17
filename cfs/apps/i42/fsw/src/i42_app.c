/*
** Purpose: Implement the 42 FSW Interface application
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application 
**   2. A single input pipe is used for all data and control because the 42
**      socket interface provides all sensor data in a single read and it keeps
**      the design simple. A flight data ingets app would typically be more
**      complex.
**
** References:
**   1. OpenSat Object-based Application Developer's Guide
**   2. cFS Application Developer's Guide
**   3. 42/Docs/Standalone AcApp text file
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
static void ProcessSbPipe(void);
static void AppTermCallback(void);

static void SendHousekeepingPkt(void);

/*
** Global Data
*/

I42_Class   I42;

/* Convenience macros */
#define  CMDMGR_OBJ (&(I42.CmdMgr))    
#define  IF42_OBJ   (&(I42.If42))

/******************************************************************************
** Function: I42_AppMain
**
*/
void I42_AppMain(void)
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

   /*
   ** At this point many flight apps use CFE_ES_WaitForStartupSync() to
   ** synchronize their startup timing with other apps. This is not
   ** needed.
   */

   if (Status == CFE_SUCCESS) RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus)) {

      ProcessSbPipe();
      
   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("I42 App terminating, err = 0x%08X\n", Status);

   CFE_EVS_SendEvent(I42_EXIT_EID, CFE_EVS_CRITICAL, "I42 App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of I42_AppMain() */


/******************************************************************************
** Function: I42_NoOpCmd
**
*/

boolean I42_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (I42_NOOP_EID, CFE_EVS_INFORMATION,
                      "No operation command received for I42 App version %d.%d.%d",
                      I42_MAJOR_VER, I42_MINOR_VER, I42_PLATFORM_REV);

   return TRUE;


} /* End I42_NoOpCmd() */


/******************************************************************************
** Function: I42_ResetAppCmd
**
*/

boolean I42_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   
   IF42_ResetStatus();
   
   return TRUE;

} /* End I42_ResetAppCmd() */


/******************************************************************************
** Function: I42_ConfigExecuteCmd
**
*/

boolean I42_ConfigExecuteCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const I42_ConfigExecute* ExecuteCmd = (const I42_ConfigExecute *) MsgPtr;
   boolean RetStatus = FALSE;
   uint16  SavedExecuteMsgCycles  = I42.ExecuteMsgCycles;
   uint16  SavedExecuteCycleDelay = I42.ExecuteCycleDelay;
  
   if ((ExecuteCmd->MsgCycles >= I42_EXECUTE_MSG_CYCLES_MIN) &&
       (ExecuteCmd->MsgCycles <= I42_EXECUTE_MSG_CYCLES_MAX)) {
      
      if ((ExecuteCmd->CycleDelay >= I42_EXECUTE_CYCLE_DELAY_MIN) &&
          (ExecuteCmd->CycleDelay <= I42_EXECUTE_CYCLE_DELAY_MAX)) {
      
         I42.ExecuteMsgCycles  = ExecuteCmd->MsgCycles;
         I42.ExecuteCycleDelay = ExecuteCmd->CycleDelay;

         RetStatus = TRUE;
         CFE_EVS_SendEvent(I42_EXECUTE_CMD_EID, CFE_EVS_INFORMATION,
                           "Execution cycle changed from %d to %d and cycle delay changed from %d to %d",
                           SavedExecuteMsgCycles, I42.ExecuteMsgCycles, SavedExecuteCycleDelay, I42.ExecuteCycleDelay);
      
      } /* End if valid delay */
      else {
      
         CFE_EVS_SendEvent(I42_EXECUTE_CMD_ERR_EID, CFE_EVS_ERROR,
                           "Configure execute command rejected. Invalid cycle delay %d, must be in range %d..%d",
                           ExecuteCmd->CycleDelay, I42_EXECUTE_CYCLE_DELAY_MIN, I42_EXECUTE_CYCLE_DELAY_MAX);
      }

   }  /* End if valid cycles */
   else {
   
      CFE_EVS_SendEvent(I42_EXECUTE_CMD_ERR_EID, CFE_EVS_ERROR,
                        "Configure execute command rejected. Invalid cycles %d, must be in range %d..%d",
                        ExecuteCmd->MsgCycles, I42_EXECUTE_MSG_CYCLES_MIN, I42_EXECUTE_MSG_CYCLES_MAX);
   }
   
   return RetStatus;

} /* End I42_ConfigExecuteCmd() */


/******************************************************************************
** Function: SendHousekeepingPkt
**
*/
static void SendHousekeepingPkt(void)
{

   /*
   ** I42 Application Data
   */

   I42.HkPkt.ValidCmdCnt   = I42.CmdMgr.ValidCmdCnt;
   I42.HkPkt.InvalidCmdCnt = I42.CmdMgr.InvalidCmdCnt;

   /*
   ** IF42 Data
   */
   
   I42.HkPkt.ExecuteCycleCnt = I42.If42.ExecuteCycleCnt;
   I42.HkPkt.ActuatorPktCnt  = I42.If42.ActuatorPktCnt;
   I42.HkPkt.SensorPktCnt    = I42.If42.SensorPktCnt;
   I42.HkPkt.Connected       = I42.If42.Connected;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &I42.HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &I42.HkPkt);

} /* End SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{


   int32 Status = CFE_SUCCESS;

   CFE_PSP_MemSet((void*)&I42, 0, sizeof(I42_Class));

   I42.ExecuteMsgCycles  = I42_EXECUTE_MSG_CYCLES_DEF;
   I42.ExecuteCycleDelay = I42_EXECUTE_CYCLE_DELAY_DEF;

   /*
   ** Initialize objects 
   */

   IF42_Constructor(IF42_OBJ, I42_LOCAL_HOST_STR, I42_SOCKET_PORT);  

   /*
   ** Initialize app level interfaces
   */

   CFE_SB_CreatePipe(&I42.SbPipe, I42_SB_PIPE_DEPTH, I42_SB_PIPE_NAME);
   CFE_SB_Subscribe(I42_CMD_MID, I42.SbPipe);
   CFE_SB_Subscribe(I42_SEND_HK_MID, I42.SbPipe);
   CFE_SB_SubscribeEx(I42_EXECUTE_MID, I42.SbPipe, CFE_SB_Default_Qos, I42_SB_EXECUTE_MSG_LIM);
   CFE_SB_SubscribeEx(I42_ACTUATOR_CMD_DATA_MID, I42.SbPipe, CFE_SB_Default_Qos, I42_SB_ACTUATOR_MSG_LIM);

   CMDMGR_Constructor(CMDMGR_OBJ);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,        NULL, I42_NoOpCmd,          0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,       NULL, I42_ResetAppCmd,      0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, I42_CONFIG_EXECUTE_CMD_FC, NULL, I42_ConfigExecuteCmd, I42_CONFIG_EXECUTE_CMD_DATA_LEN);
   
   CMDMGR_RegisterFunc(CMDMGR_OBJ, IF42_CONNECT_CMD_FC,    IF42_OBJ, IF42_ConnectCmd,    IF42_CONNECT_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, IF42_DISCONNECT_CMD_FC, IF42_OBJ, IF42_DisconnectCmd, IF42_DISCONNECT_CMD_DATA_LEN);

   CFE_SB_InitMsg(&I42.HkPkt, I42_HK_TLM_MID, I42_TLM_HK_LEN, TRUE);

       
   OS_TaskInstallDeleteHandler((void *)(&AppTermCallback)); /* Call when application terminates */

   /*
   ** Application startup event message
   */
   Status = CFE_EVS_SendEvent(I42_INIT_APP_EID, CFE_EVS_INFORMATION,
                              "I42 App Initialized. Version %d.%d.%d",
                              I42_MAJOR_VER, I42_MINOR_VER, I42_PLATFORM_REV);

   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessSbPipe
**
** This ExecuteLoop allows a user some control over how many simulator control
** cycles are executed for each scheduler wakeup. IF42 manages the interface 
** details such as whether the interface is connected, previous control cycle
** has completed, etc.
*/
static void ProcessSbPipe(void)
{

   int32           CfeStatus;
   CFE_SB_Msg_t*   SbMsgPtr;
   CFE_SB_MsgId_t  MsgId;
   uint16          ExecuteCycle = 0;
   boolean         ExecuteLoop  = FALSE;
   
   CfeStatus = CFE_SB_RcvMsg(&SbMsgPtr, I42.SbPipe, CFE_SB_PEND_FOREVER);
   
   do {

      if (CfeStatus == CFE_SUCCESS) {

         MsgId = CFE_SB_GetMsgId(SbMsgPtr);
   
         CFE_EVS_SendEvent(I42_DEBUG_EID, CFE_EVS_DEBUG,
                           "***ProcessSbPipe(): ExecuteLoop=%d,  ExecuteCycle=%d, MsgId=0x%04X, SensorPkt.InitCycle=%d",
                           ExecuteLoop,ExecuteCycle,MsgId,I42.If42.SensorPkt.InitCycle);
         
         switch (MsgId) {
            
            case I42_CMD_MID:
               CMDMGR_DispatchFunc(CMDMGR_OBJ, SbMsgPtr);
               break;

            case I42_EXECUTE_MID:
               IF42_ManageExecution();
               ++ExecuteCycle;
               ExecuteLoop = TRUE;
               break;
           
            case I42_ACTUATOR_CMD_DATA_MID:
               IF42_SendActuatorCmds((IF42_ActuatorCmdDataPkt*)SbMsgPtr);
               break;

            case I42_SEND_HK_MID:
               SendHousekeepingPkt();
               break;

            default:
               CFE_EVS_SendEvent(I42_INVALID_MID_EID, CFE_EVS_ERROR,
                                 "Received invalid command packet,MID = 0x%4X",MsgId);

               break;

         } /* End Msgid switch */

      } /* End if SB received a packet */
      
      if (ExecuteLoop) {
         if (ExecuteCycle < I42.ExecuteMsgCycles) {
            OS_TaskDelay(I42.ExecuteCycleDelay);
            IF42_ManageExecution();
            ++ExecuteCycle;
         }
         else {
            ExecuteLoop = FALSE;
         }
      }
      
      CfeStatus = CFE_SB_RcvMsg(&SbMsgPtr, I42.SbPipe, CFE_SB_POLL);
   
   } while ( ExecuteLoop || (CfeStatus == CFE_SUCCESS) );
   
} /* End ProcessSbPipe() */

        		  
/******************************************************************************
** Function: AppTermCallback
**
** This function is called when the app is terminated. This should
** never occur but if it does a graceful termination is attempted.
*/
static void AppTermCallback(void) {
 
   IF42_Close();
   
} /* End AppTermCallback() */

