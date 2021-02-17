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
#ifndef _mqtt_msg_
#define _mqtt_msg_

/*
** Includes
*/

#include "cfe.h"

#include "mq2sb.h"

#include "MQTTLinux.h"   /* Must be included prior to mqtt_li.h */
#include "mqtt_lib.h"

/***********************/
/** Macro Definitions **/
/***********************/


#define MQTT_MSG_QOS0        0
#define MQTT_MSG_QOS1        1
#define MQTT_MSG_QOS2        2

#define MQTT_MSG_PKT_UNDEF   0
#define MQTT_MSG_PKT_TLM     1
#define MQTT_MSG_PKT_CMD     2

#define MQTT_MSG_INT8        8 
#define MQTT_MSG_UINT8       9 
#define MQTT_MSG_UINT16     15
#define MQTT_MSG_INT16      16
#define MQTT_MSG_UINT32     31
#define MQTT_MSG_INT32      32
#define MQTT_MSG_FLOAT      33
#define MQTT_MSG_DOUBLE     64
#define MQTT_MSG_STR16     116
#define MQTT_MSG_STR32     132
#define MQTT_MSG_STR64     164
#define MQTT_MSG_STR128    200

/*
** Incoming MQTT Messages have 3 potential types
** 1. A single data item with a primitive type 
** 2. A CCSDS packet in JSON format
** 3. A data payload in JSON format ( no CCSDS header info )
**   Note: may not need to distinguish between 2 and 3
*/

#define MQTT_MSG_TYPE_UNDEF         000
#define MQTT_MSG_TYPE_PRIMITIVE     300
#define MQTT_MSG_TYPE_JSON_PACKET   400
#define MQTT_MSG_TYPE_JSON_PAYLOAD  500


/*
** Events
*/

#define MQTT_MSG_CALLBACK_EID   (MQTT_MSG_BASE_EID + 0)



/**********************/
/** Type Definitions **/
/**********************/

/*
** Int32 Payload
*/

typedef struct {
   
   uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
   int32    Data;

}  MQTT_MSG_Int32TlmPkt;
#define MQTT_MSG_INT32_TLM_PKT_LEN  sizeof(MQTT_MSG_Int32TlmPkt)


/*
** Float Payload
*/

typedef struct {
   
   uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
   float    Data;

}  MQTT_MSG_FloatTlmPkt;
#define MQTT_MSG_FLOAT_TLM_PKT_LEN  sizeof(MQTT_MSG_FloatTlmPkt)


/*
** Double Payload
*/

typedef struct {
   
   uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
   double   Data;

}  MQTT_MSG_DoubleTlmPkt;
#define MQTT_MSG_DOUBLE_TLM_PKT_LEN  sizeof(MQTT_MSG_DoubleTlmPkt)


/*
** String32 Payload
*/

typedef struct {
   
   uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
   char    Data[32];

}  MQTT_MSG_Str32TlmPkt;
#define MQTT_MSG_STR32_TLM_PKT_LEN  sizeof(MQTT_MSG_Str32TlmPkt)


/*
** String64 Payload
*/

typedef struct {
   
   uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
   char    Data[64];

}  MQTT_MSG_Str64TlmPkt;
#define MQTT_MSG_STR64_TLM_PKT_LEN  sizeof(MQTT_MSG_Str64TlmPkt)


/*
** String100 Payload
*/

typedef struct {
   
   uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
   char    Data[100];

}  MQTT_MSG_Str100TlmPkt;
#define MQTT_MSG_STR100_TLM_PKT_LEN  sizeof(MQTT_MSG_Str100TlmPkt)


/*
** Class Definition
*/

typedef struct {

   /*
   ** Framework References
   */
   
   INITBL_Class*  IniTbl;

   /*
   ** Table referencees
   */
   
   MQ2SB_TblData**  Mq2SbTbl;
   
   /*
   ** Telemetry Packets
   */
   
   MQTT_MSG_Int32TlmPkt    Int32TlmPkt;
   MQTT_MSG_FloatTlmPkt    FloatTlmPkt;
   MQTT_MSG_DoubleTlmPkt   DoubleTlmPkt;
   MQTT_MSG_Str32TlmPkt    Str32TlmPkt;
   MQTT_MSG_Str64TlmPkt    Str64TlmPkt;
   MQTT_MSG_Str100TlmPkt   Str100TlmPkt;

} MQTT_MSG_Class;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: MQTT_MSG_Constructor
**
** Notes:
**    1. This function must be called prior to any other functions being
**       called using the same cmdmgr instance.
*/
void MQTT_MSG_Constructor(MQTT_MSG_Class*  MqttMsgPtr,
                          INITBL_Class*    IniTbl,
                          MQ2SB_TblData**  Mq2SbTbl);


/******************************************************************************
** Function: MQTT_MSG_ProcessMsg
**
** Notes:
**   1. Signature must mach MQTT_CLIENT_MsgCallback
**
*/
void MQTT_MSG_ProcessMsg(MessageData* MsgData);


#endif /* _mqtt_msg_ */
