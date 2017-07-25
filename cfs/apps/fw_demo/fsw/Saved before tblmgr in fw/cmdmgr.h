/*
** $Id: $
** 
** Purpose: Manage command dispatching for the application.
**
** Notes:
**   1. This file should not contain any application specific
**      references (i.e. it's reusable).
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
**
** $Date: $
** $Revision: $
** $Log: $
**
*/

#ifndef _cmd_mgr_
#define _cmd_mgr_

/*
** Includes
*/

#include "app_cfg.h"
#include "cfe.h"

/*
** Event Message IDs
*/

#define CMDMGR_REG_INVALID_FUNC_CODE_ERR_EID       (CMDMGR_BASE_EID + 0)
#define CMDMGR_DISPATCH_UNUSED_FUNC_CODE_ERR_EID   (CMDMGR_BASE_EID + 1)
#define CMDMGR_DISPATCH_INVALID_CHECKSUM_ERR_EID   (CMDMGR_BASE_EID + 2)
#define CMDMGR_DISPATCH_INVALID_LEN_ERR_EID        (CMDMGR_BASE_EID + 3)
#define CMDMGR_DISPATCH_INVALID_FUNC_CODE_ERR_EID  (CMDMGR_BASE_EID + 4)

/*
** Type Definitions
*/

typedef boolean (*CMDMGR_CmdFuncPtr) (const CFE_SB_MsgPtr_t MsgPtr);


typedef struct
{

   uint16             UserDataLen;    /* User data length in bytes */
   CMDMGR_CmdFuncPtr  FuncPtr;

} CMDMGR_Cmd;

typedef struct
{

   uint16      ValidCmdCnt;       /* Number of valid messages received since init or reset */
   uint16      InvalidCmdCnt;     /* Number of invalid messages received since init or reset */
   CMDMGR_Cmd  Cmd[CMDMGR_CMD_FUNC_TOTAL];

} CMDMGR_Class;

/*
** Exported Functions
*/

/******************************************************************************
** Function: CMDMGR_Constructor
**
** Notes:
**    1. This function must be called prior to any other functions being called
**       because the "singleton" design pattern is being used. Only one command
**       manager should be needed for an application.
*/
void CMDMGR_Constructor2(CMDMGR_Class* CmdMgrPtr);


/******************************************************************************
** Function: CMDMGR_RegisterFunc
**
*/
void CMDMGR_RegisterFunc2(uint16 FuncCode, CMDMGR_CmdFuncPtr FuncPtr, uint16 UserDataLen);


/******************************************************************************
** Function: CMDMGR_ResetStatus
**
*/
void CMDMGR_ResetStatus2(void);


/******************************************************************************
** Function: CMDMGR_DispatchFunc
**
*/
boolean CMDMGR_DispatchFunc2(const CFE_SB_MsgPtr_t  MsgPtr);


#endif /* _cmd_mgr_ */
