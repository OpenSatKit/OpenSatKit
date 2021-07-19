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
#ifndef _sb2mq_
#define _sb2mq_

/*
** Includes
*/

#include "app_cfg.h"

/***********************/
/** Macro Definitions **/
/***********************/


/* TODO #define SB2MQ_TBL_REGISTER_ERR_EID  (SB2MQ_BASE_EID + 0) */


/**********************/
/** Type Definitions **/
/**********************/


/*
** Table
*/

typedef struct {
   
   CFE_SB_MsgId_t      MsgId;      /**< Message ID (must be unique)      */
   CFE_SB_Qos_t        qos;          /**< Quality of Service flag          */
   uint16              MsgLimit;   /**< Max Num. of this Msgs in pipe    */
   uint16              RouteMask;  /**< Bitwize Route Mask               */
   uint32              GroupData;  /**< Group data Mask                  */
   uint16              Flag;       /**< Custom defined flag              */
   uint16              State;      /**< Message ID is enabled = 1        */

} SB2MQ_TblEntry;


typedef struct {
   
   SB2MQ_Entry TblEntry[MQTT_SB2MQ_TBL_ENTRIES];

} SB2MQ_TblData;


/*
** Class 
*/

typedef struct {

   boolean           Registered;
   int32             Status;        /* Status of last cFE Table service call */
   CFE_TBL_Handle_t  Handle;
   SB2MQ_TblData*    DataPtr;

} SB2MQ_CfeTbl;


typedef struct {

   /*
   ** App Framework
   */
   
   const char* CfeTblName;
   
   /*
   ** Tables
   */
   
   SB2MQ_CfeTbl  CfeTbl;


} SB2MQ_Class;


/************************/
/** Exported Functions **/
/************************/



/******************************************************************************
** Function: SB2MQ_Constructor
**
** Initialize the example cFE Table object.
**
** Notes:
**   None
*/
void SB2MQ_Constructor(FILESYS_Class* FileSysPtr);


/******************************************************************************
** Function: SB2MQ_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void SB2MQ_ResetStatus(void);


/******************************************************************************
** Function: SB2MQ_ManageTbl
**
** Manage the cFE table interface for table loads and validation. 
*/
void SB2MQ_ManageTbl(void);


#endif /* _sb2mq_ */
