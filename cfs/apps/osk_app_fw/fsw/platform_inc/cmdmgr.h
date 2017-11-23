/*
** Purpose: Manage command dispatching for an application
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

#ifndef _cmdmgr_
#define _cmdmgr_

/*
** Includes
*/

#include "app_fw_cfg.h"

/*
** Event Message IDs
*/

#define CMDMGR_REG_INVALID_FUNC_CODE_ERR_EID       (CMDMGR_BASE_EID + 0)
#define CMDMGR_DISPATCH_UNUSED_FUNC_CODE_ERR_EID   (CMDMGR_BASE_EID + 1)
#define CMDMGR_DISPATCH_INVALID_CHECKSUM_ERR_EID   (CMDMGR_BASE_EID + 2)
#define CMDMGR_DISPATCH_INVALID_LEN_ERR_EID        (CMDMGR_BASE_EID + 3)
#define CMDMGR_DISPATCH_INVALID_FUNC_CODE_ERR_EID  (CMDMGR_BASE_EID + 4)
#define CMDMGR_TOTAL_EID  5

/*
** Type Definitions
*/

typedef boolean (*CMDMGR_CmdFuncPtr) (void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

/*
** Objects register their command functions so each command structure
** contains a pointer to the object's data and to the command function.
*/

typedef struct
{

   uint16             UserDataLen;    /* User data length in bytes */
   void*              DataPtr;
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
**    1. This function must be called prior to any other functions being
**       called using the same cmdmgr instance.
*/
void CMDMGR_Constructor(CMDMGR_Class* CmdMgr);


/******************************************************************************
** Function: CMDMGR_RegisterFunc
**
*/
void CMDMGR_RegisterFunc(CMDMGR_Class* CmdMgr, uint16 FuncCode, void* ObjDataPtr, 
                         CMDMGR_CmdFuncPtr ObjFuncPtr, uint16 UserDataLen);


/******************************************************************************
** Function: CMDMGR_ResetStatus
**
*/
void CMDMGR_ResetStatus(CMDMGR_Class* CmdMgr);


/******************************************************************************
** Function: CMDMGR_DispatchFunc
**
*/
boolean CMDMGR_DispatchFunc(CMDMGR_Class* CmdMgr, const CFE_SB_MsgPtr_t  MsgPtr);


#endif /* _cmdmgr_ */
