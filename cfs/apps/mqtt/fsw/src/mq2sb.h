/*
** Purpose: TODO
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
**
*/
#ifndef _mq2sb_
#define _mq2sb_

/*
** Includes
*/

#include "app_cfg.h"

/***********************/
/** Macro Definitions **/
/***********************/


#define MQ2SB_TBL_REGISTER_ERR_EID   (MQ2SB_BASE_EID + 0)
#define MQ2SB_TBL_VERIFY_EID         (MQ2SB_BASE_EID + 1)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Table Struture
*/

typedef struct {
   
   uint32              MsgType;       /* MQTT_MSG_TYPE_* */
   CFE_SB_MsgId_t      MsgId;         /* Message ID (must be unique)  */
   char                MqttTopic[MQTT_TOPIC_LEN]; /* MQTT Topic */ 
   uint32              PktType;       /* MQTT_MSG_* */
   uint32              DataType;      /* MQTT Message Data Type  */
   uint32              DataLength;    /* MQTT Message Data Length */

} MQ2SB_TblEntry;



typedef struct {
   
   MQ2SB_TblEntry Entry[MQTT_MQ2SB_TBL_ENTRIES];

} MQ2SB_TblData;


/******************************************************************************
** Class Struture
*/

typedef struct {

   boolean           Registered;
   int32             Status;       /* Status of last cFE Table service call */
   int32             ErrCode;      /* App specific code for table errors    */
   CFE_TBL_Handle_t  Handle;
   MQ2SB_TblData*    DataPtr;
   const char*       Name;
   
} MQ2SB_CfeTbl;


typedef struct {

   /*
   ** App Framework
   */
   
   
   /*
   ** Tables
   */
   
   MQ2SB_CfeTbl  CfeTbl;


} MQ2SB_Class;


/************************/
/** Exported Functions **/
/************************/



/******************************************************************************
** Function: MQ2SB_Constructor
**
** Initialize the example cFE Table object.
**
** Notes:
**   None
*/
void MQ2SB_Constructor(MQ2SB_Class*  Mq2SbPtr,
                       const char* DefTblFilename,
                       const char* CfeTblName,
                       int32 TblErrCode);


/******************************************************************************
** Function: MQ2SB_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void MQ2SB_ResetStatus(void);


/******************************************************************************
** Function: MQ2SB_ManageTbl
**
** Manage the cFE table interface for table loads and validation. Return
** value indicates whether the table has been updated.
*/
boolean MQ2SB_ManageTbl(void);


#endif /* _mq2sb_ */
