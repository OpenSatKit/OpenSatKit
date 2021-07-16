/* 
** Purpose: Manage child task command dispatching
**
** Notes:
**   1. 'Command' does not necessarily mean a ground command. 
**   2. See header file for prototype notes.  
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

/*
** Include Files:
*/

#include <string.h>

#include "childmgr.h"


/***********************/
/** Macro Definitions **/
/***********************/


/**********************/
/** Type Definitions **/
/**********************/

/*
** Child Task Management
*/

typedef struct {
   
   uint16  Count;
   CHILDMGR_Class* Instance[CHILDMGR_MAX_TASKS];
   
} ChildTask_t;


/*****************/
/** Global Data **/
/*****************/

static uint16 NameStrId = 0;
static ChildTask_t ChildTask = { 0, {NULL, NULL, NULL, NULL, NULL} };

/******************************/
/** File Function Prototypes **/
/******************************/

static void AppendIdToStr(char* NewStr, char* BaseStr);
static boolean UnusedFuncCode(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);
static void DispatchCmdFunc(CHILDMGR_Class* ChildMgr);
static boolean RegChildMgrInstance(CHILDMGR_Class* ChildMgr);
static CHILDMGR_Class* GetChildMgrInstance(void);


/******************************************************************************
** Function: CHILDMGR_Constructor
**
** Notes:
**   1. This function must be called prior to any other functions being
**      called using the same cmdmgr instance.
**   2. Constructors typically have void return type. This returns status
**      because it's failure could  prevent the parent app from initializing.
*/
int32 CHILDMGR_Constructor(CHILDMGR_Class* ChildMgr,
                           CFE_ES_ChildTaskMainFuncPtr_t ChildTaskMainFunc,
                           CHILDMGR_TaskCallback AppMainCallback,
                           CHILDMGR_TaskInit* TaskInit)
{

   int i;

   int32 RetStatus;
   char  FailedFuncStr[32] = "\0";
   char  ServiceName[OS_MAX_API_NAME];

   CFE_PSP_MemSet(ChildMgr, 0, sizeof(CHILDMGR_Class));
   for (i=0; i < CHILDMGR_CMD_FUNC_TOTAL; i++) {
      ChildMgr->Cmd[i].FuncPtr = UnusedFuncCode;
   }

   ChildMgr->PerfId       = TaskInit->PerfId;
   ChildMgr->TaskCallback = AppMainCallback;

   /* Create counting semaphore (given by parent to wake-up child) */
   AppendIdToStr(ServiceName, CHILDMGR_CNTSEM_NAME);
   if (DBG_CHILDMGR) OS_printf("CHILDMGR_Constructor() - OS_CountSemCreate(%s)\n", ServiceName);
   RetStatus = OS_CountSemCreate(&ChildMgr->WakeUpSemaphore, ServiceName, 0, 0);
   
   if (RetStatus == CFE_SUCCESS) {
      
      AppendIdToStr(ServiceName, CHILDMGR_MUTEX_NAME);
      if (DBG_CHILDMGR) OS_printf("CHILDMGR_Constructor() - OS_MutSemCreate(%s)\n", ServiceName);
      RetStatus = OS_MutSemCreate(&ChildMgr->CmdQ.Mutex, ServiceName, 0);
      
      if (RetStatus == CFE_SUCCESS) {      
         
         if (DBG_CHILDMGR) OS_printf("CHILDMGR_Constructor() - Before CFE_ES_CreateChildTask\n");
         RetStatus = CFE_ES_CreateChildTask(&ChildMgr->TaskId,
                                            TaskInit->TaskName,
                                            ChildTaskMainFunc, 0,
                                            TaskInit->StackSize,
                                            TaskInit->Priority, 0);
         if (DBG_CHILDMGR) OS_printf("CHILDMGR_Constructor() - After CFE_ES_CreateChildTask. Status=0x%08X\n", RetStatus);
         
         if (RetStatus == CFE_SUCCESS) { 
                     
            RegChildMgrInstance(ChildMgr);
             
         }
         else {
            strcpy(FailedFuncStr, "CFE_ES_CreateChildTask()");
         }
         
      } /* End if Mutex created */
      else {
         strcpy(FailedFuncStr, "OS_MutSemCreate()");
      }
     
   } /* End if CountSem created */
   else {
      strcpy(FailedFuncStr, "OS_CountSemCreate()");
   }
   
   if (RetStatus != CFE_SUCCESS) {
       
      CFE_EVS_SendEvent(CHILDMGR_INIT_ERR_EID, CFE_EVS_ERROR,
         "Child Task Manager initialization error: %s failed, Status=0x%8X",
         FailedFuncStr, (int)RetStatus);
   }

   return RetStatus;
   
} /* End CHILDMGR_Constructor() */


/******************************************************************************
** Function: CHILDMGR_RegisterFunc
**
*/
boolean CHILDMGR_RegisterFunc(CHILDMGR_Class* ChildMgr, uint16 FuncCode, 
                              void* ObjDataPtr, CHILDMGR_CmdFuncPtr ObjFuncPtr)
{

   boolean RetStatus = FALSE;
   
   if (FuncCode < CHILDMGR_CMD_FUNC_TOTAL) {

      ChildMgr->Cmd[FuncCode].DataPtr = ObjDataPtr;
      ChildMgr->Cmd[FuncCode].FuncPtr = ObjFuncPtr;
  
      RetStatus = TRUE;
   
   }
   else {
      
      CFE_EVS_SendEvent (CHILDMGR_REG_INVALID_FUNC_CODE_EID, CFE_EVS_ERROR,
         "Attempt to register function code %d which is greater than max %d",
         FuncCode,(CHILDMGR_CMD_FUNC_TOTAL-1));
   }

   return RetStatus;
   
} /* End CHILDMGR_RegisterFunc() */


/******************************************************************************
** Function: CHILDMGR_RegisterFuncAltCnt
**
*/
boolean CHILDMGR_RegisterFuncAltCnt(CHILDMGR_Class* ChildMgr, uint16 FuncCode, 
                                  void* ObjDataPtr, CHILDMGR_CmdFuncPtr ObjFuncPtr)
{

   boolean RetStatus = FALSE;

   if (CHILDMGR_RegisterFunc(ChildMgr, FuncCode, ObjDataPtr, ObjFuncPtr)) {
      
      ChildMgr->Cmd[FuncCode].AltCnt.Enabled = TRUE;      

      RetStatus = TRUE;

   }

   return RetStatus;
   
} /* End CHILDMGR_RegisterFuncAltCnt() */


/******************************************************************************
** Function: CHILDMGR_ResetStatus
**
*/
void CHILDMGR_ResetStatus(CHILDMGR_Class* ChildMgr)
{

   ChildMgr->ValidCmdCnt = 0;
   ChildMgr->InvalidCmdCnt = 0;

} /* End CHILDMGR_ResetStatus() */


/******************************************************************************
** Function: CHILDMGR_InvokeChildCmd
** 
** Notes:
**   1. This command function is registered with the app's cmdmgr and each
**      command processed by the child task must be registered using
**      CHILDMGR_RegisterFunc() and the object data pointer must reference
**      the ChildMgr instance.
*/
boolean CHILDMGR_InvokeChildCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t  MsgPtr)
{

   boolean RetStatus = FALSE;
   CHILDMGR_Class* ChildMgr = (CHILDMGR_Class*)ObjDataPtr;
   
   char EventErrStr[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH] = "\0";
   uint16 MsgLen;
   
   uint8 LocalQueueCount = ChildMgr->CmdQ.Count; /* Use local instance during checks */
   uint16 CmdCode = CFE_SB_GetCmdCode(MsgPtr);
   
   sprintf(EventErrStr, "Error dispatching commmand function %d. Undefined error", CmdCode);
   
   CFE_EVS_SendEvent(CHILDMGR_DEBUG_EID, CFE_EVS_DEBUG,
      "CHILDMGR_InvokeChildCmd() Entry: fc=%d, ChildMgr->WakeUpSemaphore=%d,WriteIdx=%d,ReadIdx=%d,Count=%d\n",
      CmdCode,ChildMgr->WakeUpSemaphore,ChildMgr->CmdQ.WriteIndex,ChildMgr->CmdQ.ReadIndex,ChildMgr->CmdQ.Count);

   /*
   ** Verify child task is active and queue interface is healthy
   */
   if (ChildMgr->WakeUpSemaphore == CHILDMGR_SEM_INVALID) {
      
      sprintf(EventErrStr, "Error dispatching commmand function %d. Child task is disabled",CmdCode);

   }
   else if (LocalQueueCount == CHILDMGR_CMD_Q_ENTRIES) {
      
      sprintf(EventErrStr, "Error dispatching commmand function %d. Child task queue is full",CmdCode);
    
   }
   else if ( (LocalQueueCount > CHILDMGR_CMD_Q_ENTRIES) ||
             (ChildMgr->CmdQ.WriteIndex >= CHILDMGR_CMD_Q_ENTRIES) ) {

      sprintf(EventErrStr, "Error dispatching commmand function %d. Child task interface is corrupted: Count=%d, Index=%d",
              CmdCode, LocalQueueCount, ChildMgr->CmdQ.WriteIndex);

   }
   else {
       
      MsgLen = CFE_SB_GetTotalMsgLength(MsgPtr);
      
      if (MsgLen <= CHILDMGR_CMD_BUFFER_LEN) {
         
         memcpy(&ChildMgr->CmdQ.Entry[ChildMgr->CmdQ.WriteIndex].Buffer, MsgPtr, MsgLen);

         ++ChildMgr->CmdQ.WriteIndex;

         if (ChildMgr->CmdQ.WriteIndex >= CHILDMGR_CMD_Q_ENTRIES) {
            
            ChildMgr->CmdQ.WriteIndex = 0;
         
         }

         /* Prevent parent/child updating queue counter at same time */
         OS_MutSemTake(ChildMgr->CmdQ.Mutex);
         ++ChildMgr->CmdQ.Count;
         OS_MutSemGive(ChildMgr->CmdQ.Mutex);

         /* Does the child task still have a semaphore? */
         if (ChildMgr->WakeUpSemaphore != CHILDMGR_SEM_INVALID) {
            
           if (DBG_CHILDMGR) OS_printf("CHILDMGR_InvokeChildCmd() Before OS_CountSemGive(ChildMgr->WakeUpSemaphore=%d)\n",ChildMgr->WakeUpSemaphore);
           OS_CountSemGive(ChildMgr->WakeUpSemaphore); /* Signal child task to call command handler */
         
         }

         RetStatus = TRUE;
         
      }/* End if valid message length */
      else {
         
         sprintf(EventErrStr, "Error dispatching commmand function %d. Command message length %d exceed max %d",
            CmdCode, MsgLen, CHILDMGR_CMD_BUFFER_LEN);
      
      }
   } /* End if command queue intact */

   if (!RetStatus) {
      
      CFE_EVS_SendEvent(CHILDMGR_INVOKE_CHILD_ERR_EID, CFE_EVS_ERROR, "%s", EventErrStr);

   }

   return RetStatus;


} /* End CHILDMGR_InvokeChildCmd() */


/******************************************************************************
** Function: CHILDMGR_PauseTask
** 
*/
boolean CHILDMGR_PauseTask(uint16* TaskBlockCnt, uint16 TaskBlockLim, 
                           uint32 TaskBlockDelayMs, uint32 PerfId) 
{
   
   boolean TaskPaused = FALSE;
   
   (*TaskBlockCnt)++;
   if (*TaskBlockCnt >= TaskBlockLim) {
                    
      CFE_ES_PerfLogExit(PerfId);
      OS_TaskDelay(TaskBlockDelayMs);
      CFE_ES_PerfLogEntry(PerfId);
      
      *TaskBlockCnt = 0;
  
      TaskPaused = TRUE;
  
   }
 
   return TaskPaused; 
   
} /* End CHILDMGR_PauseTask() */


/******************************************************************************
** Function: UnusedFuncCode
**
*/
static boolean UnusedFuncCode(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (CHILDMGR_DISPATCH_UNUSED_FUNC_CODE_EID, CFE_EVS_ERROR,
                      "Unused command function code %d received",CFE_SB_GetCmdCode(MsgPtr));

   return FALSE;

} /* End UnusedFuncCode() */


/******************************************************************************
** Function: ChildMgr_TaskMainCallback
**
** Notes:
**    1. The ChildMgr instance variable must be on the stack
**
*/
void ChildMgr_TaskMainCallback(void)
{

   int32 RegStatus = CFE_SUCCESS;

   CHILDMGR_Class*  ChildMgr = NULL; 

   /*
   ** The child task runs until the parent dies (normal end) or
   ** until it encounters a fatal error (semaphore error, etc.)...
   */

   if (DBG_CHILDMGR) OS_printf("ChildMgr_TaskMainCallback() - Before CFE_ES_RegisterChildTask()\n");
   RegStatus = CFE_ES_RegisterChildTask();
   if (DBG_CHILDMGR) OS_printf("ChildMgr_TaskMainCallback() - After CFE_ES_RegisterChildTask()\n");
   
   if (RegStatus == CFE_SUCCESS) {
   
      ChildMgr = GetChildMgrInstance();

      if (ChildMgr != NULL) {
         
         if (DBG_CHILDMGR) OS_printf("ChildMgr_TaskMainCallback() - Successful GetChildMgrInstance. PerId=%d\n",ChildMgr->PerfId);
         
         if (ChildMgr->TaskCallback == NULL) {
         
            ChildMgr->RunStatus = CHILDMGR_RUNTIME_ERR;
            CFE_EVS_SendEvent(CHILDMGR_RUNTIME_ERR_EID, CFE_EVS_ERROR, "Child task exiting due to null callback function ointer");
         
         }
         
         ChildMgr->RunStatus = CFE_SUCCESS;
         
         CFE_EVS_SendEvent(CHILDMGR_INIT_COMPLETE_EID, CFE_EVS_INFORMATION, "Child task initialization complete");

         while (ChildMgr->RunStatus == CFE_SUCCESS) {
         
            if ((ChildMgr->TaskCallback)(ChildMgr)) {
               
               ChildMgr->RunStatus = CFE_SUCCESS;
            
            }
            else {
            
               ChildMgr->RunStatus = CHILDMGR_RUNTIME_ERR;
               
               CFE_EVS_SendEvent(CHILDMGR_RUNTIME_ERR_EID, CFE_EVS_ERROR, "Child task exiting due to runtime error");

            }
         
         } /* End task while loop */
      
      
         ChildMgr->WakeUpSemaphore = CHILDMGR_SEM_INVALID;  /* Prevent parent from invoking the child task */
      
      } /* End if ChildMgr != NULL */
      
      CFE_ES_ExitChildTask();  /* Clean-up system resources */

      
   } /* End if CFE_ES_RegisterChildTask() successful */
   else {
       
	   /* Can't use CFE_EVS_SendEvent() since no task ID created due to CFE_ES_RegisterChildTask() failure */
      CFE_ES_WriteToSysLog("Child task initialization call to CFE_ES_RegisterChildTask() failed, ChildMgr->RunStatus=%d",
                           (int)ChildMgr->RunStatus);
   }


} /* End ChildMgr_TaskMainCallback() */


/******************************************************************************
** Function: ChildMgr_TaskMainCmdDispatch
**
** Notes:
**    1. The ChildMgr instance variable must be on the stack
**
*/
void ChildMgr_TaskMainCmdDispatch(void)
{

   int32 RegStatus;
   
   CHILDMGR_Class*  ChildMgr = NULL; 

   /*
   ** The child task runs until the parent dies (normal end) or
   ** until it encounters a fatal error (semaphore error, etc.)...
   */

   if (DBG_CHILDMGR) OS_printf("ChildMgr_TaskMainCmdDispatch() - Before CFE_ES_RegisterChildTask()\n");
   RegStatus = CFE_ES_RegisterChildTask();
   if (DBG_CHILDMGR) OS_printf("ChildMgr_TaskMainCmdDispatch() - After CFE_ES_RegisterChildTask()\n");

   
   if (RegStatus == CFE_SUCCESS) {
   
      ChildMgr = GetChildMgrInstance();
   
      if (ChildMgr != NULL) {
         
         if (DBG_CHILDMGR) OS_printf("ChildMgr_TaskMainCallback() - Successful GetChildMgrInstance. PerId=%d\n",ChildMgr->PerfId);

         ChildMgr->RunStatus = CFE_SUCCESS;
         
         CFE_EVS_SendEvent(CHILDMGR_INIT_COMPLETE_EID, CFE_EVS_INFORMATION, "Child task initialization complete");

         while (ChildMgr->RunStatus == CFE_SUCCESS) {
         
            CFE_ES_PerfLogExit(ChildMgr->PerfId);
            if (DBG_CHILDMGR) OS_printf("CHILDMGR_Task() Before OS_CountSemTake(ChildMgr->WakeUpSemaphore=%d)\n",ChildMgr->WakeUpSemaphore);         
            ChildMgr->RunStatus = OS_CountSemTake(ChildMgr->WakeUpSemaphore); /* Pend until parent app gives semaphore */
            if (DBG_CHILDMGR) OS_printf("CHILDMGR_Task() After OS_CountSemTake(ChildMgr->WakeUpSemaphore=%d), ChildMgr->RunStatus = 0x%4X\n", ChildMgr->WakeUpSemaphore, ChildMgr->RunStatus);         
            CFE_ES_PerfLogEntry(ChildMgr->PerfId); 

            if (ChildMgr->RunStatus == CFE_SUCCESS) {
            
               /* Check parent/child handshake integrity and terminate main loop if errors */
               if (ChildMgr->CmdQ.Count == 0) {
               
                  CFE_EVS_SendEvent(CHILDMGR_EMPTY_TASK_Q_EID, CFE_EVS_ERROR,
                     "CHILDMGR_Task invoked with an empty command queue");

                  ChildMgr->RunStatus = CFE_OS_ERROR;
            
               }
               else if (ChildMgr->CmdQ.ReadIndex >= CHILDMGR_CMD_Q_ENTRIES) {

                  CFE_EVS_SendEvent(CHILDMGR_INVALID_Q_READ_IDX_EID, CFE_EVS_ERROR,
                     "CHILDMGR_Task invoked with a command queue read index of %d that is greater than max %d",
                     (int)ChildMgr->CmdQ.ReadIndex, (CHILDMGR_CMD_Q_ENTRIES-1));

                  ChildMgr->RunStatus = CFE_OS_ERROR;
            
               }
               else {
               
                  DispatchCmdFunc(ChildMgr);
            
               }
         
            } /* End if wake up semaphore successful */
            else {
           
               CFE_EVS_SendEvent(CHILDMGR_TAKE_SEM_FAILED_EID, CFE_EVS_ERROR,
                  "CHILDMGR_Task take semaphore failed: result = %d", ChildMgr->RunStatus);
            }

         } /* End task while loop */
      
      
         ChildMgr->WakeUpSemaphore = CHILDMGR_SEM_INVALID;  /* Prevent parent from invoking the child task */
      
      } /* End if ChildMgr != NULL */
      
      CFE_ES_ExitChildTask();  /* Clean-up system resources */

      
   } /* End if CFE_ES_RegisterChildTask() successful */
   else {
       
	   /* Can't use CFE_EVS_SendEvent() since no task ID created due to CFE_ES_RegisterChildTask() failure */
      CFE_ES_WriteToSysLog("Child task initialization call to CFE_ES_RegisterChildTask() failed, ChildMgr->RunStatus=%d",
                           (int)ChildMgr->RunStatus);
   }


} /* End ChildMgr_TaskMainCmdDispatch() */


/******************************************************************************
** Function: DispatchCmdFunc
**
** Assumes the parent app's cmdmgr has performed all of the checks so this
** command dispatcher doesn't need to do the command integrity checks.
**
*/
static void DispatchCmdFunc(CHILDMGR_Class* ChildMgr)
{

   boolean ValidCmd;
   CFE_SB_MsgPtr_t MsgPtr;
   

   MsgPtr = (CFE_SB_MsgPtr_t)&ChildMgr->CmdQ.Entry[ChildMgr->CmdQ.ReadIndex].Buffer;

   ChildMgr->CurrCmdCode = CFE_SB_GetCmdCode(MsgPtr);
   
   ValidCmd = (ChildMgr->Cmd[ChildMgr->CurrCmdCode].FuncPtr)(ChildMgr->Cmd[ChildMgr->CurrCmdCode].DataPtr, MsgPtr);


   if (ChildMgr->Cmd[ChildMgr->CurrCmdCode].AltCnt.Enabled) {
   
      ValidCmd ? ChildMgr->Cmd[ChildMgr->CurrCmdCode].AltCnt.Valid++ : ChildMgr->Cmd[ChildMgr->CurrCmdCode].AltCnt.Invalid++;
   
   } 
   else {
   
      ValidCmd ? ChildMgr->ValidCmdCnt++ : ChildMgr->InvalidCmdCnt++;
   
   }

   
   ChildMgr->PrevCmdCode = ChildMgr->CurrCmdCode;
   ChildMgr->CurrCmdCode = 0;

   ++ChildMgr->CmdQ.ReadIndex;

   if (ChildMgr->CmdQ.ReadIndex >= CHILDMGR_CMD_Q_ENTRIES) ChildMgr->CmdQ.ReadIndex = 0;
   
   OS_MutSemTake(ChildMgr->CmdQ.Mutex);
   --ChildMgr->CmdQ.Count;
   OS_MutSemGive(ChildMgr->CmdQ.Mutex);

   CFE_EVS_SendEvent (CHILDMGR_DEBUG_EID, CFE_EVS_DEBUG,
      "DispatchCmdFunc() Exit: ChildMgr->WakeUpSemaphore=%d,WriteIdx=%d,ReadIdx=%d,Count=%d\n",
      ChildMgr->WakeUpSemaphore,ChildMgr->CmdQ.WriteIndex,ChildMgr->CmdQ.ReadIndex,ChildMgr->CmdQ.Count);

} /* End DispatchCmdFunc() */


/******************************************************************************
** Function: RegChildMgrInstance
**
** Notes: 
**   1. TODO - Add memory protection
*/
static boolean RegChildMgrInstance(CHILDMGR_Class* ChildMgr)
{
   
   boolean RetStatus = FALSE;
   
   if (DBG_CHILDMGR) OS_printf("CHILDMGR::RegChildMgrInstance() - Task %d, ChildTask.Count %d\n", ChildMgr->TaskId, ChildTask.Count);

   if (ChildTask.Count <  CHILDMGR_MAX_TASKS) {
      
      ChildTask.Instance[ChildTask.Count++] = ChildMgr;
      RetStatus = TRUE;
   
   }
   
   return RetStatus;  
   
} /* RegChildMgrInstance() */


/******************************************************************************
** Function: GetChildMgrInstance
*/
static CHILDMGR_Class* GetChildMgrInstance(void)
{

   CHILDMGR_Class*  Instance = NULL;
   uint16 i = 0;
   uint32 TaskId = OS_TaskGetId();
   
   if (DBG_CHILDMGR) OS_printf("CHILDMGR::GetChildMgrInstance() - Task %d\n", TaskId);
   
   while ( i < ChildTask.Count && i < CHILDMGR_MAX_TASKS) {
   
      if (ChildTask.Instance[i] != NULL) {
         if (ChildTask.Instance[i]->TaskId == TaskId) {
            Instance = ChildTask.Instance[i];
            break;
         }
      }
      
      i++;
      
   }
   
   if (DBG_CHILDMGR) OS_printf("CHILDMGR::GetChildMgrInstance() - Exit: i=%d, found=%d\n",i,(Instance != NULL));
   
   return Instance;
   
} /* End GetChildMgrInstance() */


/******************************************************************************
** Function: AppendIdToStr
**
** Notes:
**   1. No need for checks since a local function with known calling 
**      environments
**   2. Currently ID's are not synched with 
**   3. TODO - Add memory protection if needed
*/
static void AppendIdToStr(char* NewStr, char* BaseStr)
{
   
   char IdStr[5];
   
   strncpy(NewStr,BaseStr,OS_MAX_API_NAME-3);
   sprintf(IdStr,"%d",NameStrId++);
   strcat(NewStr,IdStr);

} /* AppendIdToStr() */

