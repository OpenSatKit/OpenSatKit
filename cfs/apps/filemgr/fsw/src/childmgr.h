/*
** Purpose: Manage child task command dispatching
**
** Notes:
**   1. This is part of a FM refactor prototype. Initial version is hardcoded
**      for ChildMgr being dedicated to one app. The prototype will investigate
**      whether an app framework module can be created.
**   2. The initial prototype uses a singleton pattern so it is not reentrant
**      and only one instance can exist per app. It would need to be refactored
**      to be added to the osk_c_fw.
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

#ifndef _childmgr_
#define _childmgr_

/*
** Includes
*/

#include "app_cfg.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define CHILDMGR_SEM_INVALID  0xFFFFFFFF
#define CHILDMGR_MUTEX_NAME   "CHILDMGR_Q"


/*
** Event Message IDs
*/

#define CHILDMGR_INIT_ERR_EID                    (CHILDMGR_BASE_EID + 0)
#define CHILDMGR_INIT_COMPLETE_EID               (CHILDMGR_BASE_EID + 1)
#define CHILDMGR_REG_INVALID_FUNC_CODE_EID       (CHILDMGR_BASE_EID + 2)
#define CHILDMGR_EMPTY_TASK_Q_EID                (CHILDMGR_BASE_EID + 3)
#define CHILDMGR_INVALID_Q_READ_IDX_EID          (CHILDMGR_BASE_EID + 4)
#define CHILDMGR_TAKE_SEM_FAILED_EID             (CHILDMGR_BASE_EID + 5)
#define CHILDMGR_INVOKE_CHILD_ERR_EID            (CHILDMGR_BASE_EID + 6)
#define CHILDMGR_DISPATCH_UNUSED_FUNC_CODE_EID   (CHILDMGR_BASE_EID + 7)


/**********************/
/** Type Definitions **/
/**********************/

typedef struct {

   char    Buffer[CHILDMGR_CMD_BUFFER_LEN];

} CHILDMGR_CmdQ_Entry;

typedef struct {

   uint32  Mutex;
   uint8   WriteIndex;
   uint8   ReadIndex;
   uint8   Count;
   uint8   Spare;
   
   CHILDMGR_CmdQ_Entry  Entry[CHILDMGR_CMD_Q_ENTRIES];

} CHILDMGR_CmdQ;


typedef boolean (*CHILDMGR_CmdFuncPtr) (void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

/*
** Objects register their command functions so each command structure
** contains a pointer to the object's data and to the command function.
** The parent app's CMDMGR takes care of validating commands so command 
** attributes like data length are not needed here.
*/


typedef struct {

   void*                DataPtr;
   CHILDMGR_CmdFuncPtr  FuncPtr; 

} CHILDMGR_Cmd;

typedef struct {
   
   uint32  TaskId;
   uint32  WakeUpSemaphore;

   uint16  ValidCmdCnt;
   uint16  InvalidCmdCnt;

   uint8   CurrCmdCode;
   uint8   PrevCmdCode;

   CHILDMGR_Cmd  Cmd[CHILDMGR_CMD_FUNC_TOTAL];

   CHILDMGR_CmdQ CmdQ;

} CHILDMGR_Class;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: CHILDMGR_Constructor
**
** Notes:
**    1. This function must be called prior to any other functions being
**       called using the same cmdmgr instance.
*/
int32 CHILDMGR_Constructor(CHILDMGR_Class* ChildMgr);


/******************************************************************************
** Function: CHILDMGR_RegisterFunc
**
*/
void CHILDMGR_RegisterFunc(uint16 FuncCode, void* ObjDataPtr, CHILDMGR_CmdFuncPtr ObjFuncPtr);


/******************************************************************************
** Function: CHILDMGR_ResetStatus
**
*/
void CHILDMGR_ResetStatus(void);


/******************************************************************************
** Function: CHILDMGR_InvokeChildCmd
** 
** Notes:
**   1. Perform's orginal FM's FM_VerifyChildTask() & FM_InvokeChildTask()
**   2. This command function is registered with the app's cmdmgr with all of
**      the function codes that use the child task to process the command.
*/
boolean CHILDMGR_InvokeChildCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t  MsgPtr);

/******************************************************************************
** Function: CHILDMGR_Task
**
*/
void CHILDMGR_Task(void);


#endif /* _childmgr_ */
