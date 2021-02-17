/*
** Purpose: Implement the MQTT_MSG_Class methods
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>

#include "mqtt_msg.h"


/**********************/
/** Global File Data **/
/**********************/

static MQTT_MSG_Class*  MqttMsg = NULL;


/*
** Local Function Prototypes
*/

static CFE_SB_Msg_t* ConvertMqToCcsds(MQ2SB_TblEntry* Mq2SbTbl, char* Payload);


/******************************************************************************
** Function: MQTT_MSG_Constructor
**
*/
void MQTT_MSG_Constructor(MQTT_MSG_Class*  MqttMsgPtr,
                          INITBL_Class*    IniTbl,
                          MQ2SB_TblData**  Mq2SbTbl)
{
 
   MqttMsg = MqttMsgPtr;

   
   CFE_PSP_MemSet((void*)MqttMsgPtr, 0, sizeof(MQTT_MSG_Class));
   
   MqttMsg->IniTbl   = IniTbl;
   MqttMsg->Mq2SbTbl = Mq2SbTbl;
   
} /* End MQTT_MSG_Constructor() */


/******************************************************************************
** Function: MQTT_MSG_ProcessMsg
**
** Notes:
**   1. Signature must match MQTT_CLIENT_MsgCallback
**   2. MQTT has no delimeter between the topic and payload
**
*/
void MQTT_MSG_ProcessMsg(MessageData* MsgData)
{
   
   MQTTMessage* MsgPtr = MsgData->message;
   
   int16    i = 0;
   char     TopicStr[MQTT_TOPIC_LEN];
   boolean  TblEnd   = FALSE;
   boolean  MsgFound = FALSE;
   MQ2SB_TblData*  Mq2SbTbl = *(MqttMsg->Mq2SbTbl);

   CFE_EVS_SendEvent(MQTT_MSG_CALLBACK_EID, CFE_EVS_INFORMATION,
                     "MQTT_MSG_ProcessMsg: %s", MsgData->topicName->lenstring.data);
                    
   if(MsgPtr->payloadlen) {
      
      //~~ OS_printf("MsgPtr->payloadlen=%d\n",MsgPtr->payloadlen);
      while ( MsgFound == FALSE && TblEnd == FALSE && i < MQTT_MQ2SB_TBL_ENTRIES) {
         
         if (Mq2SbTbl->Entry[i].MsgType == MQTT_MSG_TYPE_UNDEF) {
            
            TblEnd = TRUE;

         }
         else {
            
            int TopicLen = strlen(Mq2SbTbl->Entry[i].MqttTopic);
            
            //~~ OS_printf("Table topic length=%d\n",TopicLen);
            if (strncmp(Mq2SbTbl->Entry[i].MqttTopic, MsgData->topicName->lenstring.data, TopicLen) == 0) {
               MsgFound = TRUE;
               memcpy(TopicStr, MsgData->topicName->lenstring.data, TopicLen);
               TopicStr[TopicLen] = '\0';
               CFE_EVS_SendEvent(MQTT_MSG_CALLBACK_EID, CFE_EVS_INFORMATION, "MQTT_MSG_ProcessMsg: Topic=%s, Payload=%s", 
                                 TopicStr, &MsgData->topicName->lenstring.data[TopicLen]);
            } 
            else {
               i++;
            }
         }

      } /* End while loop */

      if (MsgFound) {
            
         char  PayloadStr[80];
         CFE_SB_Msg_t* SbMsgPtr;
         
         strncpy(PayloadStr, MsgPtr->payload, MsgPtr->payloadlen); 
         PayloadStr[MsgPtr->payloadlen] = 0;
         
         SbMsgPtr = ConvertMqToCcsds(&Mq2SbTbl->Entry[i],PayloadStr);
          
         if (SbMsgPtr != NULL) {
            CFE_SB_TimeStampMsg(SbMsgPtr);
            CFE_SB_SendMsg(SbMsgPtr);
         }
         
      } /* End if message found */
      else if (TblEnd == TRUE) {
      
         OS_printf("MQTT --> Message not found before end of table!\n");
      
      }
   
   } /* End null message len */
   else {
      
      OS_printf("Null MQTT wessage data length for %s\n", MsgData->topicName->lenstring.data);
    
   }
   
   fflush(stdout);

} /* End MQTT_MSG_ProcessMsg() */


/******************************************************************************
** Function:  ConvertMqToCcsds
**
** TODO - Think thorugh SB packet init and table loads.
** TODO - I added message ID defs to JSON ini file so the cFE table doesn't
**        contain the actual message ID but a ini table config ID. I've only
**        changed STR32 below since this is still in prototype stage
*/
static CFE_SB_Msg_t* ConvertMqToCcsds(MQ2SB_TblEntry* Mq2SbTbl, char* Payload)
{
  
   CFE_SB_Msg_t*  CcsdsMsgPtr = NULL;

   int32    MqInt32;
   float    MqFloat;
   double   MqDouble;

   CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

   /*~~
   CFE_EVS_SendEvent(MQTT_MSG_CALLBACK_EID, CFE_EVS_INFORMATION,
                     "ConvertMqToCcsds: MessageType=%d",Mq2SbTbl->MsgType);
   */
   switch (Mq2SbTbl->MsgType) {
      
      case MQTT_MSG_TYPE_PRIMITIVE:
         
         switch (Mq2SbTbl->DataType) {
            
            int Status;
            
            case MQTT_MSG_INT32:
               Status = sscanf(Payload, "%d", (int *) &MqInt32); 
               if (Status == 1) {

                  CcsdsMsgPtr = (CFE_SB_Msg_t*)&MqttMsg->Int32TlmPkt;
                  CFE_SB_InitMsg(CcsdsMsgPtr, Mq2SbTbl->MsgId,
                                 MQTT_MSG_INT32_TLM_PKT_LEN, TRUE);

                  MqttMsg->Int32TlmPkt.Data = MqInt32;
                  
               }
               break;

            case MQTT_MSG_FLOAT:
               Status = sscanf(Payload, "%f", &MqFloat); 
               if (Status == 1) {
                  
                  CcsdsMsgPtr = (CFE_SB_Msg_t*)&MqttMsg->FloatTlmPkt;
                  CFE_SB_InitMsg(CcsdsMsgPtr, Mq2SbTbl->MsgId,
                                 MQTT_MSG_FLOAT_TLM_PKT_LEN, TRUE);
                  MqttMsg->FloatTlmPkt.Data = MqFloat;

               }
               break;

            case MQTT_MSG_DOUBLE:
               Status = sscanf(Payload, "%lf", &MqDouble); 
               if (Status == 1) {
                  
                  CcsdsMsgPtr = (CFE_SB_Msg_t*)&MqttMsg->DoubleTlmPkt;
                  CFE_SB_InitMsg(CcsdsMsgPtr, Mq2SbTbl->MsgId,
                                 MQTT_MSG_DOUBLE_TLM_PKT_LEN, TRUE);
                  MqttMsg->DoubleTlmPkt.Data = MqDouble;
               
               }   
               break;

            case MQTT_MSG_STR32:
               
               CcsdsMsgPtr = (CFE_SB_Msg_t*)&MqttMsg->Str32TlmPkt;
               MsgId = (CFE_SB_MsgId_t)INITBL_GetIntConfig(MqttMsg->IniTbl, Mq2SbTbl->MsgId);
               //~~ OS_printf("MQTT_MSG_STR32 MsgId = 0x%04X\n",MsgId);
               CFE_SB_InitMsg(CcsdsMsgPtr, MsgId, MQTT_MSG_STR32_TLM_PKT_LEN, TRUE);
               strncpy(MqttMsg->Str32TlmPkt.Data, Payload, 32); 
               break;

            case MQTT_MSG_STR64:
               
               CcsdsMsgPtr = (CFE_SB_Msg_t*)&MqttMsg->Str64TlmPkt;
               CFE_SB_InitMsg(CcsdsMsgPtr, Mq2SbTbl->MsgId,
                              MQTT_MSG_STR64_TLM_PKT_LEN, TRUE);
               strncpy(MqttMsg->Str64TlmPkt.Data, Payload, 64); 
               break;

            default:
               OS_printf("MQTT --> ERROR: Unknown type!\n");
         
         } /* End DataType switch */
         break;
      
      case MQTT_MSG_TYPE_JSON_PACKET:
         OS_printf("MQTT JSON Packet Message\n");
         break;

      case MQTT_MSG_TYPE_JSON_PAYLOAD:
         OS_printf("MQTT JSON Payload Message\n");
         break;

      default:
         OS_printf("Unknown Message Type\n");

   } /* End MsgType switch */
   
   return (CcsdsMsgPtr);

} /* End ConvertMqToCcsds() */

