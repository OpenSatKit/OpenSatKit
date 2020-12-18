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
#include "initbl.h"


/***********************/
/** Macro Definitions **/
/***********************/

#define DBG_CHILDMGR  1

/*****************/
/** Global Data **/
/*****************/

CHILDMGR_Class* ChildMgr;


/******************************/
/** File Function Prototypes **/
/******************************/

static boolean UnusedFuncCode(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);
static void DispatchCmdFunc(void);


/******************************************************************************
** Function: CHILDMGR_Constructor
**
** Notes:
**   1. This function must be called prior to any other functions being
**      called using the same cmdmgr instance.
**   2. Constructors typically have void return type. This returns status
**      because it's failure could  prevent the parent app from initializing.
*/
int32 CHILDMGR_Constructor(CHILDMGR_Class* ChildMgrPtr)
{

   int i;

   int32 RetStatus;
   char  FailedFuncStr[32] = "\0";
   
   ChildMgr = ChildMgrPtr;
   

   CFE_PSP_MemSet(ChildMgr, 0, sizeof(CHILDMGR_Class));
   for (i=0; i < CHILDMGR_CMD_FUNC_TOTAL; i++) {
      ChildMgr->Cmd[i].FuncPtr = UnusedFuncCode;
   }

   /* Create counting semaphore (given by parent to wake-up child) */
   RetStatus = OS_CountSemCreate(&ChildMgr->WakeUpSemaphore, INITBL_GetStrConfig(CFG_CHILD_SEM_NAME), 0, 0);
   
   if (RetStatus == CFE_SUCCESS) {
      
      RetStatus = OS_MutSemCreate(&ChildMgr->CmdQ.Mutex, CHILDMGR_MUTEX_NAME, 0);
      
      if (RetStatus == CFE_SUCCESS) {      

         RetStatus = CFE_ES_CreateChildTask(&ChildMgr->TaskId,
                                            INITBL_GetStrConfig(CFG_CHILD_NAME),
                                            CHILDMGR_Task, 0,
                                            INITBL_GetIntConfig(CFG_CHILD_STACK_SIZE),
                                            INITBL_GetIntConfig(CFG_CHILD_PRIORITY), 0);
         if (RetStatus != CFE_SUCCESS) { 
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
void CHILDMGR_RegisterFunc(uint16 FuncCode, void* ObjDataPtr, CHILDMGR_CmdFuncPtr ObjFuncPtr)
{

   if (FuncCode < CHILDMGR_CMD_FUNC_TOTAL) {

      ChildMgr->Cmd[FuncCode].DataPtr = ObjDataPtr;
      ChildMgr->Cmd[FuncCode].FuncPtr = ObjFuncPtr;
   
   }
   else {
      
      CFE_EVS_SendEvent (CHILDMGR_REG_INVALID_FUNC_CODE_EID, CFE_EVS_ERROR,
         "Attempt to register function code %d which is greater than max %d",
         FuncCode,(CHILDMGR_CMD_FUNC_TOTAL-1));
   }

} /* End CHILDMGR_RegisterFunc() */


/******************************************************************************
** Function: CHILDMGR_ResetStatus
**
*/
void CHILDMGR_ResetStatus(void)
{

   ChildMgr->ValidCmdCnt = 0;
   ChildMgr->InvalidCmdCnt = 0;

} /* End CHILDMGR_ResetStatus() */



/******************************************************************************
** Function: CHILDMGR_Task
**
*/
void CHILDMGR_Task(void)
{

   int32 Status = CFE_SUCCESS;

   /*
   ** The child task runs until the parent dies (normal end) or
   ** until it encounters a fatal error (semaphore error, etc.)...
   */
   
   Status = CFE_ES_RegisterChildTask();

   if (Status == CFE_SUCCESS) {
   
      CFE_EVS_SendEvent(CHILDMGR_INIT_COMPLETE_EID, CFE_EVS_INFORMATION, "File Manager child task initialization complete");

      while (Status == CFE_SUCCESS) {
         
         CFE_ES_PerfLogExit(FILEMGR_CHILD_TASK_PERF_ID); 
         //~~OS_printf("CHILDMGR_Task() Before OS_CountSemTake(ChildMgr->WakeUpSemaphore=%d)\n",ChildMgr->WakeUpSemaphore);         
         Status = OS_CountSemTake(ChildMgr->WakeUpSemaphore); /* Pend until parent app gives semaphore */
         //~~OS_printf("CHILDMGR_Task() After OS_CountSemTake(ChildMgr->WakeUpSemaphore=%d), Status = 0x%4X\n", ChildMgr->WakeUpSemaphore, Status);         
         CFE_ES_PerfLogEntry(FILEMGR_CHILD_TASK_PERF_ID); 

         if (Status == CFE_SUCCESS) {
            
            /* Check parent/child handshake integrity and terminate main loop if errors */
            if (ChildMgr->CmdQ.Count == 0) {
               
               CFE_EVS_SendEvent(CHILDMGR_EMPTY_TASK_Q_EID, CFE_EVS_ERROR,
                  "CHILDMGR_Task invoked with an empty command queue");

               Status = CFE_OS_ERROR;
            
            }
            else if (ChildMgr->CmdQ.ReadIndex >= CHILDMGR_CMD_Q_ENTRIES) {

               CFE_EVS_SendEvent(CHILDMGR_INVALID_Q_READ_IDX_EID, CFE_EVS_ERROR,
                  "CHILDMGR_Task invoked with a command queue read index of %d that is greater than max %d",
                  (int)ChildMgr->CmdQ.ReadIndex, (CHILDMGR_CMD_Q_ENTRIES-1));

               Status = CFE_OS_ERROR;
            
            }
            else {
               
               DispatchCmdFunc();
            
            }
         
         } /* End if wake up semaphore successful */
         else {
           
            CFE_EVS_SendEvent(CHILDMGR_TAKE_SEM_FAILED_EID, CFE_EVS_ERROR,
               "CHILDMGR_Task take semaphore failed: result = %d", Status);
         }

      } /* End task while loop */
      
   } /* End if CFE_ES_RegisterChildTask() successful */
   else {
       
	   /* Can't use CFE_EVS_SendEvent() since no task ID created due to CFE_ES_RegisterChildTask() failure */
      CFE_ES_WriteToSysLog("File Manager child task initialization call to CFE_ES_RegisterChildTask() failed, Status=%d",
                           (int)Status);
   }

   ChildMgr->WakeUpSemaphore = CHILDMGR_SEM_INVALID;  /* Prevent parent from invoking the child task */

   CFE_ES_ExitChildTask();  /* Clean-up system resources */

} /* End CHILDMGR_Task() */



/******************************************************************************
** Function: CHILDMGR_InvokeChildCmd
** 
** Notes:
**   1. Perform's orginal FM's FM_VerifyChildTask() & FM_InvokeChildTask()
**   2. This command function is registered with the app's cmdmgr with all of
**      the function codes that use the child task to process the command.
*/
boolean CHILDMGR_InvokeChildCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t  MsgPtr)
{

   boolean RetStatus = FALSE;
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
            
           //~~OS_printf("CHILDMGR_InvokeChildCmd() Before OS_CountSemGive(ChildMgr->WakeUpSemaphore=%d)\n",ChildMgr->WakeUpSemaphore);
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
boolean CHILDMGR_PauseTask(uint16* TaskBlockCnt, uint16 TaskBlockLim, uint32 TaskBlockDelayMs) 
{
   
   boolean TaskPaused = FALSE;
   
   (*TaskBlockCnt)++;
   if (*TaskBlockCnt >= TaskBlockLim) {
                    
      CFE_ES_PerfLogExit(FILEMGR_CHILD_TASK_PERF_ID);
      OS_TaskDelay(TaskBlockDelayMs);
      CFE_ES_PerfLogEntry(FILEMGR_CHILD_TASK_PERF_ID);
      
      *TaskBlockCnt = 0;
  
      TaskPaused = TRUE;
  
   }
 
   return TaskPaused; 
   
} /* End CHILDMGR_PauseTask() */


/******************************************************************************
** Function: DispatchCmdFunc
**
** The parent app's cmdmgr has performed all of the checks so this command
** dispatcher doesn't need to do the command integrity checks.
**
*/
static void DispatchCmdFunc(void)
{

   boolean ValidCmd;
   CFE_SB_MsgPtr_t MsgPtr;
   

   MsgPtr = (CFE_SB_MsgPtr_t)&ChildMgr->CmdQ.Entry[ChildMgr->CmdQ.ReadIndex].Buffer;

   ChildMgr->CurrCmdCode = CFE_SB_GetCmdCode(MsgPtr);
   
   ValidCmd = (ChildMgr->Cmd[ChildMgr->CurrCmdCode].FuncPtr)(ChildMgr->Cmd[ChildMgr->CurrCmdCode].DataPtr, MsgPtr);

   if (ValidCmd == TRUE) {
      
      ChildMgr->ValidCmdCnt++;
      
   }
   else {

      ChildMgr->InvalidCmdCnt++;

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
** Function: UnusedFuncCode
**
*/
static boolean UnusedFuncCode(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (CHILDMGR_DISPATCH_UNUSED_FUNC_CODE_EID, CFE_EVS_ERROR,
                      "Unused command function code %d received",CFE_SB_GetCmdCode(MsgPtr));

   return FALSE;

} /* End UnusedFuncCode() */