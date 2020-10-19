/*
** Purpose: Define the 42 FSW interface application
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application. See osk_42_lib.h for more
**      information.
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
#ifndef _i42_app_
#define _i42_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "if42.h"

/***********************/
/** Macro Definitions **/
/***********************/


/*
** Events Message IDs
*/

#define I42_INIT_APP_EID         (I42_BASE_EID + 0)
#define I42_NOOP_EID             (I42_BASE_EID + 1)
#define I42_EXIT_EID             (I42_BASE_EID + 2)
#define I42_INVALID_MID_EID      (I42_BASE_EID + 3)
#define I42_EXECUTE_CMD_EID      (I42_BASE_EID + 4)
#define I42_EXECUTE_CMD_ERR_EID  (I42_BASE_EID + 5)
#define I42_DEBUG_EID            (I42_BASE_EID + 6)

/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

   uint16  MsgCycles;      /* Number of cycles for each execute msg wakeup */
   uint16  CycleDelay;     /* Delay(ms) between execution cycles           */ 
   
}  OS_PACK I42_ConfigExecute;
#define I42_CONFIG_EXECUTE_CMD_DATA_LEN  (sizeof(I42_ConfigExecute) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** IF42 Data
   */
   
   uint32   ActuatorPktCnt;
   uint32   SensorPktCnt;  
   uint16   ExecuteCycleCnt;   /* Execute cycles while connected */
   boolean  Connected;
   
} OS_PACK I42_HkPkt;
#define I42_TLM_HK_LEN sizeof (I42_HkPkt)


/******************************************************************************
** I42_Class
*/

typedef struct {

   /*
   ** App Framework
   */

   CFE_SB_PipeId_t  SbPipe;
   CMDMGR_Class     CmdMgr;

   /* 
   ** I42 App 
   */
   
   uint16  ExecuteCycleCnt;
   uint32  ExecuteMsgCycles;       /* Number of cycles to perform for each execute message */
   uint16  ExecuteCycleDelay;      /* Delays between execution cycles */
   

   /*
   ** Telemetry Packets
   */
   I42_HkPkt  HkPkt;
   
   /*
   ** App Objects
   */ 
   IF42_Class  If42;

} I42_Class;


/*******************/
/** Exported Data **/
/*******************/


extern I42_Class  I42;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: I42_AppMain
**
*/
void I42_AppMain(void);


/******************************************************************************
** Function: I42_NoOpCmd
**
*/
boolean I42_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: I42_ResetAppCmd
**
*/
boolean I42_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: I42_ConfigExecuteStepCmd
**
*/
boolean I42_ConfigExecuteStepCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _i42_app_ */
