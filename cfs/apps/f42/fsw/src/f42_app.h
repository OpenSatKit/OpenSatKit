/*
** Purpose: Define the 42 Attitude Control (AC) application
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application. See osk_42_lib.h for more
**      information.
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/
#ifndef _f42_app_
#define _f42_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "f42_adp.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Events
*/

#define F42_INIT_APP_EID        (F42_BASE_EID + 0)
#define F42_NOOP_EID            (F42_BASE_EID + 1)
#define F42_EXIT_EID            (F42_BASE_EID + 2)
#define F42_INVALID_MID_EID     (F42_BASE_EID + 3)


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
   ** TBLMGR Data 
   ** - Loaded with status from the last table action 
   */

   uint8    LastAction;
   uint8    LastActionStatus;
   
   /*
   ** 42 FSW Adapter Data
   */
   
   uint32   CtrlExeCnt;
   uint16   CtrlMode;
   uint16   OvrSunValid;   /* Use 16-bit to keep word aligned */
   
} OS_PACK F42_HkPkt;
#define F42_TLM_HK_LEN sizeof (F42_HkPkt)



/******************************************************************************
** F42 Class
*/

typedef struct {

   /*
   ** App Framework
   */

   CFE_SB_PipeId_t  SbPipe;
   CMDMGR_Class     CmdMgr;
   TBLMGR_Class     TblMgr;

   /*
   ** Telemetry Packets
   */

   F42_HkPkt   HkPkt;
   
   /*
   ** App Objects
   */

   F42_ADP_Class    F42Adp;


} F42_Class;


/*******************/
/** Exported Data **/
/*******************/

extern F42_Class F42;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: F42_AppMain
**
*/
void F42_AppMain(void);


/******************************************************************************
** Function: F42_NoOpCmd
**
*/
boolean F42_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: F42_ResetAppCmd
**
*/
boolean F42_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _f42_app_ */
