/*
** Purpose: Define MQTT's MQTT-to-SB message table
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
*/

#include "cfe_tbl_filedef.h"
#include "app_cfg.h"
#include "mq2sb.h"
#include "mqtt_msg.h"


/*
** MQTT to Software Bus table (mqtt2sb) 
** 
** Each table entry has the following:
**  Type: MQTT_MSG_PKT_*
**  Message ID:
**  Data Type:
**  Data Length: (may not be needed)
*/
MQ2SB_TblData MQ2SB_Tbl =
{
   {

      /* 0 - Test with HiveMQ */
      {   
        .MsgType           = MQTT_MSG_TYPE_PRIMITIVE,
        .MsgId             = CFG_STR32_TLM_MID,      /* TODO - MQTT_STR32_TLM_MID */
        .MqttTopic         = "osk/1",
        .PktType           = MQTT_MSG_PKT_TLM,
        .DataType          = MQTT_MSG_STR32,
        .DataLength        = 32 
      },

      /* 1 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_INT32,
         .DataLength        = 4
      },

      /* 2 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_INT32,
         .DataLength        = 4
      },

      /* 3 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_INT32,
         .DataLength        = 4
      },

      /* 4 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_INT32,
         .DataLength        = 4
      },

      /* 5 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_INT32,
         .DataLength        = 4
      },

      /* 6 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_INT32,
         .DataLength        = 4
      },

      /* 7 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_INT32,
         .DataLength        = 4
      },

      /* 8 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_INT32,
         .DataLength        = 4
      },

      /* 9 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_INT32,
         .DataLength        = 4
      }
    
   }
}; 

/*
** cFE table file header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "MQ2SB_Tbl", MQTT_APP_CFE_NAME "." MQTT_MQ2SB_TBL_CFE_NAME,
    "MQTT Msg to SB table", "mq2sb_tbl.tbl",sizeof(MQ2SB_TblData)
};
