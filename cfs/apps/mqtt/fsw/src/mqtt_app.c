/*
** Purpose: Implement the MQTT application
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Includes
*/

#include <string.h>
#include "mqtt_app.h"

/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ      (&(Mqtt.IniTbl))
#define  CMDMGR_OBJ      (&(Mqtt.CmdMgr))
#define  TBLMGR_OBJ      (&(Mqtt.TblMgr))    
#define  CHILDMGR_OBJ    (&(Mqtt.ChildMgr))
#define  MQ2SB_OBJ       (&(Mqtt.Mq2Sb))
#define  MQTT_MSG_OBJ    (&(Mqtt.MqttMsg))
#define  MQTT_CLIENT_OBJ (&(Mqtt.MqttClient))

/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(uint32* AppMainPerfId);
static boolean ChildTaskCallback(CHILDMGR_Class* ChildMgr);
static void SubscribeToMqttClient(void);


/**********************/
/** File Global Data **/
/**********************/

/* 
** Must match DECLARE ENUM() declaration in app_cfg.h
** Defines "static INILIB_CfgEnum IniCfgEnum"
*/
DEFINE_ENUM(Config,APP_CONFIG)  

static CFE_EVS_BinFilter_t  EventFilters[] =
{  
   /* Event ID                 Mask */
   {MQTT_CLIENT_YIELD_ERR_EID, CFE_EVS_FIRST_4_STOP}

};

/*****************/
/** Global Data **/
/*****************/

MQTT_Class   Mqtt;

/******************************************************************************
** Function: MQTT_AppMain
**
*/
void MQTT_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;
   uint32 AppMainPerfId;
   CFE_SB_MsgId_t CmdMid    = CFE_SB_INVALID_MSG_ID;
   CFE_SB_MsgId_t SendHkMid = CFE_SB_INVALID_MSG_ID;

   Status = CFE_ES_RegisterApp();
   
   if (Status == CFE_SUCCESS) {
   
      CFE_EVS_Register(EventFilters, sizeof(EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                       CFE_EVS_BINARY_FILTER);

      Status = InitApp(&AppMainPerfId); /* Performs initial CFE_ES_PerfLogEntry() call */
   
      if (Status == CFE_SUCCESS) {

         RunStatus = CFE_ES_APP_RUN;

         AppMainPerfId = 
         CmdMid    = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_MID);
         SendHkMid = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_SEND_HK_MID);

      }
      
   } /* End if RegisterApp() success */
   
   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus)) {

      CFE_SB_Msg_t*   CmdMsgPtr;
      CFE_SB_MsgId_t  MsgId;

      CFE_ES_PerfLogExit(AppMainPerfId);
      Status = CFE_SB_RcvMsg(&CmdMsgPtr, Mqtt.CmdPipe, CFE_SB_PEND_FOREVER);
      CFE_ES_PerfLogEntry(AppMainPerfId);

      if (Status == CFE_SUCCESS) {

         MsgId = CFE_SB_GetMsgId(CmdMsgPtr);
         
         if (MsgId == CmdMid) {
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
         } 
         else if (MsgId == SendHkMid) {
            if (MQ2SB_ManageTbl()) {
               SubscribeToMqttClient();
            }
            MQTT_SendHousekeepingPkt();
         }
         else {
               CFE_EVS_SendEvent(MQTT_INVALID_MID_EID, CFE_EVS_ERROR,
                                 "Received invalid command packet,MID = 0x%4X",MsgId);
         }

      } /* End if SB received a packet */
      else {
      
         RunStatus = CFE_ES_APP_ERROR;
      
      } /* End if SB failure */
      
   } /* End CFE_ES_RunLoop */

   CFE_ES_WriteToSysLog("MQTT App terminating, err = 0x%08X\n", Status);   /* Use SysLog, events may not be working */

   CFE_EVS_SendEvent(MQTT_EXIT_EID, CFE_EVS_CRITICAL, "MQTT App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of MQTT_AppMain() */


/******************************************************************************
** Function: MQTT_NoOpCmd
**
*/

boolean MQTT_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (MQTT_NOOP_EID, CFE_EVS_INFORMATION,
                      "No operation command received for MQTT App version %d.%d.%d",
                      MQTT_MAJOR_VER, MQTT_MINOR_VER, MQTT_PLATFORM_REV);

   return TRUE;


} /* End MQTT_NoOpCmd() */


/******************************************************************************
** Function: MQTT_ResetAppCmd
**
*/

boolean MQTT_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   CHILDMGR_ResetStatus(CHILDMGR_OBJ);
   
   MQ2SB_ResetStatus();
	  
   return TRUE;

} /* End MQTT_ResetAppCmd() */


/******************************************************************************
** Function: MQTT_SendHousekeepingPkt
**
*/
void MQTT_SendHousekeepingPkt(void)
{
   
   Mqtt.HkPkt.ValidCmdCnt    = Mqtt.CmdMgr.ValidCmdCnt;
   Mqtt.HkPkt.InvalidCmdCnt  = Mqtt.CmdMgr.InvalidCmdCnt;

   Mqtt.HkPkt.ChildValidCmdCnt    = Mqtt.ChildMgr.ValidCmdCnt;
   Mqtt.HkPkt.ChildInvalidCmdCnt  = Mqtt.ChildMgr.InvalidCmdCnt;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &Mqtt.HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &Mqtt.HkPkt);

} /* End MQTT_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(uint32* AppMainPerfId)
{

   int32 Status = OSK_C_FW_CFS_ERROR;
   
   CHILDMGR_TaskInit ChildTaskInit;

   /*
   ** Initialize objects 
   */

   if (INITBL_Constructor(&Mqtt.IniTbl, MQTT_INI_FILENAME, &IniCfgEnum)) {
   
      *AppMainPerfId = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_MAIN_PERF_ID);
      CFE_ES_PerfLogEntry(*AppMainPerfId);
      
      MQTT_MSG_Constructor(MQTT_MSG_OBJ, &Mqtt.IniTbl, &MQ2SB_OBJ->CfeTbl.DataPtr);
      
      MQTT_CLIENT_Constructor(MQTT_CLIENT_OBJ,
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_SERVER_ADDRESS), 
                              INITBL_GetIntConfig(INITBL_OBJ, CFG_SERVER_PORT), 
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_CLIENT_NAME), 
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_SERVER_USERNAME),
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_SERVER_PASSWORD));
       
      /* Constructor sends error events */    
      ChildTaskInit.TaskName  = INITBL_GetStrConfig(INITBL_OBJ, CFG_CHILD_NAME);
      ChildTaskInit.StackSize = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_STACK_SIZE);
      ChildTaskInit.Priority  = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_PRIORITY);
      ChildTaskInit.PerfId    = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_TASK_PERF_ID);
      Status = CHILDMGR_Constructor(CHILDMGR_OBJ, ChildMgr_TaskMainCallback,
                                    ChildTaskCallback, 
                                    &ChildTaskInit); 
   
   } /* End if INITBL Constructed */
   
   if (Status == CFE_SUCCESS) {

      MQ2SB_Constructor(MQ2SB_OBJ, INITBL_GetStrConfig(INITBL_OBJ, CFG_TBL_DEF_FILENAME), 
                        INITBL_GetStrConfig(INITBL_OBJ, CFG_TBL_CFE_NAME),
                        INITBL_GetIntConfig(INITBL_OBJ, CFG_TBL_ERR_CODE));

      if (MQ2SB_OBJ->CfeTbl.Status == CFE_SUCCESS) {
      
         SubscribeToMqttClient();
         
      }

      /*
      ** Initialize app level interfaces
      */
      
      CFE_SB_CreatePipe(&Mqtt.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH), INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME));  
      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_MID), Mqtt.CmdPipe);
      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_SEND_HK_MID), Mqtt.CmdPipe);

      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, MQTT_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, MQTT_ResetAppCmd, 0);

      //Child command codin idiom
      //CMDMGR_RegisterFunc(CMDMGR_OBJ, DIR_CREATE_CMD_FC,          DIR_OBJ, CHILDMGR_InvokeChildCmd, DIR_CREATE_CMD_DATA_LEN);
      //CHILDMGR_RegisterFunc(DIR_CREATE_CMD_FC,          DIR_OBJ, DIR_CreateCmd);

      CFE_SB_InitMsg(&Mqtt.HkPkt, (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_HK_TLM_MID), 
                     MQTT_TLM_HK_PKT_LEN, TRUE);

	   TBLMGR_Constructor(TBLMGR_OBJ);
   
      /*
      ** Application startup event message
      */
      CFE_EVS_SendEvent(MQTT_INIT_APP_EID, CFE_EVS_INFORMATION,
                        "MQTT App Initialized. Version %d.%d.%d",
                        MQTT_MAJOR_VER, MQTT_MINOR_VER, MQTT_PLATFORM_REV);
                        
   } /* End if CHILDMGR constructed */
   
   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: SubscribeToMqttClient
**
** TODO - Need return status in case no successful subscriptions
**
*/
static void SubscribeToMqttClient(void)
{
   
   int    i;
   int    SubscribeErr = 0;
   MQ2SB_TblData* Mq2SbTbl = MQ2SB_OBJ->CfeTbl.DataPtr;
   
   for (i = 0; i < MQTT_MQ2SB_TBL_ENTRIES; i++) {

      if (Mq2SbTbl->Entry[i].MsgType == MQTT_MSG_TYPE_PRIMITIVE) {
      
         if (!MQTT_CLIENT_Subscribe(Mq2SbTbl->Entry[i].MqttTopic, 
                                    MQTT_MSG_QOS2, 
                                    MQTT_MSG_ProcessMsg)) {
            
            ++SubscribeErr;
            CFE_EVS_SendEvent(MQTT_CLIENT_SUBSCRIBE_ERR_EID, CFE_EVS_ERROR,
                              "Error subscribing to MQ2SB table entry %d, topic %s, QOS %d",
                              i, Mq2SbTbl->Entry[i].MqttTopic, MQTT_MSG_QOS2);
            
         }
      
      }
      else {
         
         /* Assume the first unused entry is the end of the table */ 
         break;
      
      }
      
   } /* End for loop */
   
   CFE_EVS_SendEvent(MQTT_CLIENT_SUBSCRIBE_EID, CFE_EVS_INFORMATION,
                     "Subscribed to %d MQ2SB table topics with %d errors",
                     (i-SubscribeErr), SubscribeErr);
   
} /* End SubscribeToMqttClient() */


/******************************************************************************
** Function: ChildTaskCallback
**
*/
static boolean ChildTaskCallback(CHILDMGR_Class* ChildMgr)
{
   
   if (MQ2SB_OBJ->CfeTbl.DataPtr != NULL) {     
      
      MQTT_CLIENT_Yield(1000);
   
   }
   else {
   
      OS_TaskDelay(1000);
   
   }

   return TRUE;
   
} /* End ChildTaskCallback() */



