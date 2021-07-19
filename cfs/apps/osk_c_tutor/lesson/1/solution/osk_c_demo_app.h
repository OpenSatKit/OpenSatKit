/* LESSON 1 - Solution
** Purpose: Define the OSK C Demo application
**
** Notes:
**   1. Demonstrates an application using the OSK C Framework. It also serves
**      as the final app that is developed during the Code-As-You-Go(CAYG)
**      app development tutorial.
**
** References:
**   1. OpenSat Application Developer's Guide
**   2. cFS Application Developer's Guide
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/
#ifndef _osk_c_demo_app_
#define _osk_c_demo_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "initbl.h"


/***********************/
/** Macro Definitions **/
/***********************/

/*
** Events
*/

#define OSK_C_DEMO_INIT_APP_EID    (OSK_C_DEMO_BASE_EID + 0)
#define OSK_C_DEMO_NOOP_EID        (OSK_C_DEMO_BASE_EID + 1)
#define OSK_C_DEMO_EXIT_EID        (OSK_C_DEMO_BASE_EID + 2)
#define OSK_C_DEMO_INVALID_MID_EID (OSK_C_DEMO_BASE_EID + 3)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/


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
   ** CHILDMGR Data
   */
   uint16   ChildValidCmdCnt;
   uint16   ChildInvalidCmdCnt;
 
   /*
   ** Table Data 
   ** - Loaded with status from the last table action 
   */

   uint8    LastTblAction;
   uint8    LastTblActionStatus;

   
   /*
   ** MSGLOG Data
   */

   boolean  MsgLogEna;
   boolean  MsgPlaybkEna;

   uint16   MsgLogCnt;      
   char     MsgLogFilename[OS_MAX_PATH_LEN];


} OS_PACK OSK_C_DEMO_HkPkt;
#define OSK_C_DEMO_TLM_HK_LEN sizeof (OSK_C_DEMO_HkPkt)


/******************************************************************************
** OSK_C_DEMO_Class
*/
typedef struct {

   /* 
   ** App Framework
   */ 
   
   INITBL_Class    IniTbl; 
   CFE_SB_PipeId_t CmdPipe;
   CMDMGR_Class    CmdMgr;
   TBLMGR_Class    TblMgr;
   

   
   /*
   ** Command Packets
   */

   PKTUTIL_NoParamCmdMsg MsgLogRunChildFuncCmd;
 
   /*
   ** Telemetry Packets
   */
   
   OSK_C_DEMO_HkPkt  HkPkt;
   
   /*
   ** OSK_C_DEMO State & Child Objects
   */ 
           
   uint32          PerfId;
   CFE_SB_MsgId_t  CmdMid;
   CFE_SB_MsgId_t  ExecuteMid;
   CFE_SB_MsgId_t  SendHkMid;
   

   
} OSK_C_DEMO_Class;


/*******************/
/** Exported Data **/
/*******************/

extern OSK_C_DEMO_Class  OskCDemo;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: OSK_C_DEMO_AppMain
**
*/
void OSK_C_DEMO_AppMain(void);


/******************************************************************************
** Function: OSK_C_DEMO_NoOpCmd
**
*/
boolean OSK_C_DEMO_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: OSK_C_DEMO_ResetAppCmd
**
*/
boolean OSK_C_DEMO_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _osk_c_demo_ */
