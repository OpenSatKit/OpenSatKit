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
#ifndef _mqtt_client_
#define _mqtt_client


/*
** Includes
*/

#include "app_cfg.h"
#include "mqtt_msg.h"


/***********************/
/** Macro Definitions **/
/***********************/

#define MQTT_CLIENT_CONSTRUCT_EID      (MQTT_CLIENT_BASE_EID + 0)
#define MQTT_CLIENT_CONSTRUCT_ERR_EID  (MQTT_CLIENT_BASE_EID + 1)
#define MQTT_CLIENT_YIELD_ERR_EID      (MQTT_CLIENT_BASE_EID + 2)

/**********************/
/** Type Definitions **/
/**********************/



/*
** Process message function callback signature 
*/

typedef void (*MQTT_CLIENT_MsgCallback) (MessageData* MsgData);

/*
** Class Definition
*/

typedef struct {

   boolean Connected;
   
   /*
   ** MQTT Library
   */
   Network                 Network;
   MQTTClient              Client;
   MQTTPacket_connectData  ConnectData;    
   unsigned char           SendBuf[MQTT_CLIENT_SEND_BUF_LEN];
   unsigned char           ReadBuf[MQTT_CLIENT_READ_BUF_LEN];

} MQTT_CLIENT_Class;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: MQTT_CLIENT_Constructor
**
** Notes:
**   1. This function must be called prior to any other functions being
**      called using the same cmdmgr instance.
*/
boolean MQTT_CLIENT_Constructor(MQTT_CLIENT_Class* MqttClientPtr,
                                const char *Hostname, int Port, 
                                const char *Clientname, 
                                const char *Username, const char *Password);


/******************************************************************************
** Function: MQTT_CLIENT_Subscribe
**
** Notes:
**    1. QOS options are defined in mqtt_msg.h
*/
boolean MQTT_CLIENT_Subscribe(const char *Topic, int Qos, 
                              MQTT_CLIENT_MsgCallback MsgCallbackFunc);


/******************************************************************************
** Function: MQTT_CLIENT_Yield
**
** Notes:
**    1. A task delay will always occur regardless of MQTT interface behavior
**       to avoid CPU hogging
**
*/
boolean MQTT_CLIENT_Yield(unsigned int Timeout);


#endif /* _mqtt_client_ */

