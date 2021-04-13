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

#include "osk_c_fw_cfg.h"

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
** Alternate command counters allow an individual command to have its own 
** counters. The class counters are not incremented for the command. This
** is useful when commands are issued from onboard apps and incrementing 
** the class command counters may be confusing to ground operations. 
*/
typedef struct
{

   boolean  Enabled;  /* Use alternate cmd counters */            
   uint16   Valid;    /* Number of valid messages received since init or reset */
   uint16   Invalid;  /* Number of invalid messages received since init or reset */

} CMDMGR_AltCnt;

/*
** Objects register their command functions so each command structure
** contains a pointer to the object's data and to the command function.
*/

typedef struct
{

   uint16             UserDataLen;    /* User data length in bytes  */
   void*              DataPtr;
   CMDMGR_CmdFuncPtr  FuncPtr; 

   CMDMGR_AltCnt      AltCnt;

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
boolean CMDMGR_RegisterFunc(CMDMGR_Class* CmdMgr, uint16 FuncCode, void* ObjDataPtr, 
                            CMDMGR_CmdFuncPtr ObjFuncPtr, uint16 UserDataLen);


/******************************************************************************
** Function: CMDMGR_RegisterFuncAltCnt
**
** Register a command function that will increment its own private alternate
** command counters.
*/
boolean CMDMGR_RegisterFuncAltCnt(CMDMGR_Class* CmdMgr, uint16 FuncCode, void* ObjDataPtr, 
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


/******************************************************************************
** Function: CMDMGR_ValidBoolArg
**
** Use uint16 because commands use both uint8 and uint16 for booleans and test   
** is valid for casting to larger storage but not truncating to shorter storage
*/
boolean CMDMGR_ValidBoolArg(uint16 BoolArg);


/******************************************************************************
** Function: CMDMGR_BoolStr
**
** Purpose: Return a pointer to a string describing the enumerated type
**
** Notes:
**   None
*/
const char* CMDMGR_BoolStr(boolean  BoolArg);

#endif /* _cmdmgr_ */
