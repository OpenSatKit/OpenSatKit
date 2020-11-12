/*
** Purpose: Define the OpenSat Kit Command Ingest application.
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _kit_ci_
#define _kit_ci_

/*
** Includes
*/

#include "app_cfg.h"

#include "uplink.h"


/***********************/
/** Macro Definitions **/
/***********************/

/*
** Events
*/


#define KIT_CI_APP_INIT_EID         (KIT_CI_APP_BASE_EID + 0)
#define KIT_CI_APP_NOOP_EID         (KIT_CI_APP_BASE_EID + 1)
#define KIT_CI_APP_INVALID_MID_EID  (KIT_CI_APP_BASE_EID + 2)
#define KIT_CI_APP_EXIT_EID         (KIT_CI_APP_BASE_EID + 3)


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
   ** UPLINK Data
   */

   boolean  SocketConnected;
   boolean  MsgTunnelEnabled;
   uint16   SocketId;
   uint32   RecvMsgCnt;
   uint32   RecvMsgErrCnt;

   uint16   MappingsPerformed;
   UPLINK_LastMapping  LastMapping;

} OS_PACK KIT_CI_HkPkt;
#define KIT_CI_TLM_HK_LEN sizeof (KIT_CI_HkPkt)


/******************************************************************************
** KIT_CI Class
*/

typedef struct {

   /* 
   ** App Framework
   */   
   CFE_SB_PipeId_t CmdPipe;   
   CMDMGR_Class    CmdMgr;
   
   /*
   ** Telemetry Packets
   */
   KIT_CI_HkPkt  HkPkt;

   /*
   ** App Objects
   */   
   UPLINK_Class  Uplink;

} KIT_CI_Class;


/*******************/
/** Exported Data **/
/*******************/

extern KIT_CI_Class KitCi;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: KIT_CI_AppMain
**
*/
void KIT_CI_AppMain(void);


/******************************************************************************
** Function: KIT_CI_NoOpCmd
**
** Notes:
**   1. Function signature must match the CMDMGR_CmdFuncPtr definition
**
*/
boolean KIT_CI_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: KIT_CI_ResetAppCmd
**
** Notes:
**   1. Function signature must match the CMDMGR_CmdFuncPtr definition
**
*/
boolean KIT_CI_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _kit_ci_ */
