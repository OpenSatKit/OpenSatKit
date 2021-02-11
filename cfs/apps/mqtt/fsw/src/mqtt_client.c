/*
** Purpose: Define MQTT messages
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>

#include "mqtt_client.h"

/*****************/
/** Global Data **/
/*****************/

static MQTT_CLIENT_Class* MqttClient;

/******************************************************************************
** Function: MQTT_CLIENT_Constructor
**
** Notes:
**    1. This function must be called prior to any other functions being
**       called using the same MQTT_CLIENT instance.
*/
boolean MQTT_CLIENT_Constructor(MQTT_CLIENT_Class* MqttClientPtr,
                                const char *Hostname, int Port, 
                                const char *Clientname, 
                                const char *Username, const char *Password) 
{
   
   int RetCode;
   boolean RetStatus = FALSE;  /* TODO - What is a useful return value? Connecting during app init is not an error but may be good to know */ 
   MQTTPacket_connectData DefConnectOptions = MQTTPacket_connectData_initializer;

   MqttClient = MqttClientPtr;
   
   CFE_PSP_MemSet((void*)MqttClient, 0, sizeof(MQTT_CLIENT_Class));
   
   memcpy(&MqttClient->ConnectData, &DefConnectOptions, sizeof(MQTTPacket_connectData));
   
   /*
   ** Init and connect to network
   */
   
   NetworkInit(&MqttClient->Network);
   RetCode = NetworkConnect(&MqttClient->Network, (char *)Hostname, Port);

   if (RetCode == 0) {

      /*
      ** Init MQTT client
      */
      MQTTClientInit(&MqttClient->Client, 
                     &MqttClient->Network, MQTT_CLIENT_TIMEOUT_MS,
                     MqttClient->SendBuf,  MQTT_CLIENT_SEND_BUF_LEN,
                     MqttClient->ReadBuf,  MQTT_CLIENT_READ_BUF_LEN); 

      MqttClient->ConnectData.willFlag = 0;
      MqttClient->ConnectData.MQTTVersion = 3;
      MqttClient->ConnectData.clientID.cstring = (char *)Clientname;
      MqttClient->ConnectData.username.cstring = NULL; /* TODO - Username, JSON Ini doesn't support null */
      MqttClient->ConnectData.password.cstring = NULL; /* TODO - Passwrod, JSON Ini doesn't support null */

      MqttClient->ConnectData.keepAliveInterval = 10;
      MqttClient->ConnectData.cleansession = 1;

      /*
      ** Connect to MQTT server
      */
      
      CFE_EVS_SendEvent(MQTT_CLIENT_CONSTRUCT_EID, CFE_EVS_INFORMATION, 
                        "Connecting to MQTT broker %s:%d as client %s", Hostname, Port, Clientname);
      
      RetCode = MQTTConnect(&MqttClient->Client, &MqttClient->ConnectData);

      if (RetCode == SUCCESS) {
         
         MqttClient->Connected = TRUE;
         RetStatus = TRUE;
         
      }
      else {
      
         CFE_EVS_SendEvent(MQTT_CLIENT_CONSTRUCT_ERR_EID, CFE_EVS_ERROR, 
                           "Error connecting to MQTT broker at %s:%d as client %s. Status=%d", Hostname, Port, Clientname, RetCode);
      
      }
      
   } /* End if successful NetworkConnect */
   else {
   
      CFE_EVS_SendEvent(MQTT_CLIENT_CONSTRUCT_ERR_EID, CFE_EVS_ERROR, 
                        "Error creating network connection at %s:%d. Status=%d", Hostname, Port, RetCode);
   
   }
   
   return RetStatus;

} /* End MQTT_CLIENT_Constructor() */


/******************************************************************************
** Function: MQTT_CLIENT_Subscribe
**
** Notes:
**    1. QOS needs to be converted to MQTT library constants
*/

boolean MQTT_CLIENT_Subscribe(const char *Topic, int Qos, 
                              MQTT_CLIENT_MsgCallback MsgCallbackFunc)
{
   
   boolean RetStatus = FALSE;
   
   int LibQos;

   if (Qos == MQTT_MSG_QOS0) {
      LibQos = QOS0;
   } else if (Qos == MQTT_MSG_QOS1) {
      LibQos = QOS1;
   } else {
      LibQos = QOS2;
   }

   RetStatus = (MQTTSubscribe(&MqttClient->Client, Topic, LibQos, MsgCallbackFunc) == SUCCESS);

   return RetStatus;
   
} /* End MQTT_CLIENT_Subscribe() */



/******************************************************************************
** Function: MQTT_CLIENT_Yield
**
** Notes:
**    1. If yield fails, enforce a timeout to avoid CPU hogging
**
*/
boolean MQTT_CLIENT_Yield(unsigned int Timeout)
{
   
   boolean RetStatus = FALSE;

   if (MqttClient->Connected) {
      
      if (MQTTYield(&MqttClient->Client, Timeout) == SUCCESS) {
         
         RetStatus = TRUE;
      
      }
      else {
         
         CFE_EVS_SendEvent(MQTT_CLIENT_YIELD_ERR_EID, CFE_EVS_ERROR, 
                          "MQTT client yield error");
         OS_TaskDelay(Timeout);

      }
   }
   else {
      OS_TaskDelay(Timeout);
   }
    
   return RetStatus;
    
} /* End MQTT_CLIENT_Yield() */


/******************************************************************************
** Function: MQTT_CLIENT_Disconnect
**
** Notes:
**    None
**
*/
void MQTT_CLIENT_Disconnect(void)
{
   
   MQTTDisconnect(&MqttClient->Client);
   NetworkDisconnect(&MqttClient->Network);

} /* End MQTT_CLIENT_Disconnect() */

