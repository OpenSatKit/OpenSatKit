/*
** Purpose: Define the Payload Simulator application
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
#ifndef _pl_sim_app_
#define _pl_sim_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "pl_sim_lib.h"


/***********************/
/** Macro Definitions **/
/***********************/

/*
** Events
*/

#define PL_SIM_INIT_APP_EID          (PL_SIM_BASE_EID +  0)
#define PL_SIM_NOOP_EID              (PL_SIM_BASE_EID +  1)
#define PL_SIM_EXIT_EID              (PL_SIM_BASE_EID +  2)
#define PL_SIM_INVALID_MID_EID       (PL_SIM_BASE_EID +  3)
#define PL_SIM_PWR_ON_CMD_EID        (PL_SIM_BASE_EID +  4)
#define PL_SIM_PWR_ON_CMD_ERR_EID    (PL_SIM_BASE_EID +  5)
#define PL_SIM_PWR_OFF_CMD_EID       (PL_SIM_BASE_EID +  6)
#define PL_SIM_PWR_RESET_CMD_ERR_EID (PL_SIM_BASE_EID +  7)
#define PL_SIM_PWR_RESET_CMD_EID     (PL_SIM_BASE_EID +  8)
#define PL_SIM_SET_FAULT_CMD_EID     (PL_SIM_BASE_EID +  9)
#define PL_SIM_CLEAR_FAULT_CMD_EID   (PL_SIM_BASE_EID + 10)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
**
** For each command that is a binary switch use separate function codes 
** so they all have command length with no parameters.
**
*/

#define PL_SIM_POWER_ON_CMD_DATA_LEN     PKTUTIL_NO_PARAM_CMD_DATA_LEN
#define PL_SIM_POWER_OFF_CMD_DATA_LEN    PKTUTIL_NO_PARAM_CMD_DATA_LEN
#define PL_SIM_POWER_RESET_CMD_DATA_LEN  PKTUTIL_NO_PARAM_CMD_DATA_LEN
#define PL_SIM_SET_FAULT_CMD_DATA_LEN    PKTUTIL_NO_PARAM_CMD_DATA_LEN
#define PL_SIM_CLEAR_FAULT_CMD_DATA_LEN  PKTUTIL_NO_PARAM_CMD_DATA_LEN


/******************************************************************************
** Telemetry Packets
*/

typedef struct
{

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;
   
   /*
   ** PL_SIM Library Data
   */

   uint8    LibPwrState;
   uint8    LibPwrInitCycleCnt;
   uint8    LibPwrResetCycleCnt;
   boolean  LibDetectorFault;
   uint16   LibDetectorReadoutRow;
   uint16   LibDetectorImageCnt;
   
} OS_PACK PL_SIM_StatusTlm_t;
#define PL_SIM_APP_STATUS_TLM_LEN sizeof(PL_SIM_StatusTlm_t)


/******************************************************************************
** PL_SIM_Class
*/
typedef struct
{

   /* 
   ** App Framework
   */ 
   
   INITBL_Class    IniTbl; 
   CFE_SB_PipeId_t CmdPipe;
   CMDMGR_Class    CmdMgr;
   
   /*
   ** Telemetry Packets
   */
   
   PL_SIM_StatusTlm_t  StatusTlm;
   
   /*
   ** PL_SIM State
   */ 
           
   uint32         PerfId;
   CFE_SB_MsgId_t CmdMid;
   CFE_SB_MsgId_t ExecuteMid;
   uint32         TlmSlowRate;
   uint32         TlmSlowRateCnt;
   
   PL_SIM_LIB_Class_t  Lib;
   
} PL_SIM_Class_t;


/*******************/
/** Exported Data **/
/*******************/

extern PL_SIM_Class_t  PlSim;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: PL_SIM_AppMain
**
*/
void PL_SIM_AppMain(void);


/******************************************************************************
** Function: PL_SIM_NoOpCmd
**
*/
boolean PL_SIM_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: PL_SIM_ResetAppCmd
**
*/
boolean PL_SIM_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

/******************************************************************************
** Functions: PL_SIM_PwrOnCmd, PL_SIM_PwrOffCmd, PL_SIM_PwrResetCmd
**
** Power on/off/reset payload.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. Use separate command function codes & functions as opposed to one 
**     command with a parameter that would need validation
**  3. Reset allows an intermediate level of initialization to be simulated
**     that allows some system state to persist across the reset. For
**     science data may be allowed to resume immediately after a reset.
**
*/
boolean PL_SIM_PowerOnCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
boolean PL_SIM_PowerOffCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
boolean PL_SIM_PowerResetCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Functions: PL_SIM_SetFaultCmd, PL_SIM_ClearFaultCmd
**
** Set/clear fault state.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. Use separate command function codes & functions as opposed to one 
**     command with a parameter that would need validation
**
*/
boolean PL_SIM_SetFaultCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
boolean PL_SIM_ClearFaultCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _pl_sim_app_ */
