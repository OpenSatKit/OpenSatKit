/*
** Purpose: Implement the File Manager application
**
** Notes:
**   1. See header notes. 
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/

/*
** Includes
*/

#include <string.h>
#include "filemgr_app.h"

/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ   (&(FileMgr.IniTbl))
#define  CMDMGR_OBJ   (&(FileMgr.CmdMgr))
#define  TBLMGR_OBJ   (&(FileMgr.TblMgr))
#define  CHILDMGR_OBJ (&(FileMgr.ChildMgr))
#define  DIR_OBJ      (&(FileMgr.Dir))
#define  FILE_OBJ     (&(FileMgr.File))
#define  FILESYS_OBJ  (&(FileMgr.FileSys))


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(uint32* AppMainPerfId);

/**********************/
/** File Global Data **/
/**********************/

/* 
** Must match DECLARE ENUM() declaration in app_cfg.h
** Defines "static INILIB_CfgEnum IniCfgEnum"
*/
DEFINE_ENUM(Config,APP_CONFIG)  


/*****************/
/** Global Data **/
/*****************/

FILEMGR_Class  FileMgr;


/******************************************************************************
** Function: FILEMGR_AppMain
**
*/
void FILEMGR_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;
   uint32 AppMainPerfId;
   CFE_SB_MsgId_t CmdMid    = CFE_SB_INVALID_MSG_ID;
   CFE_SB_MsgId_t SendHkMid = CFE_SB_INVALID_MSG_ID;

   Status = CFE_ES_RegisterApp();
   
   if (Status == CFE_SUCCESS) {
      
      CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);

      Status = InitApp(&AppMainPerfId); /* Performs initial CFE_ES_PerfLogEntry() call */
   
      if (Status == CFE_SUCCESS) {
         
         RunStatus = CFE_ES_APP_RUN;
         
         CmdMid    = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_MID);
         SendHkMid = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_SEND_HK_MID);

      }
   
   } /* End if RegisterApp() success */
   
   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus)) {

      CFE_SB_Msg_t*   CmdMsgPtr;
      CFE_SB_MsgId_t  MsgId;

      CFE_ES_PerfLogExit(AppMainPerfId);
      Status = CFE_SB_RcvMsg(&CmdMsgPtr, FileMgr.CmdPipe, CFE_SB_PEND_FOREVER);
      CFE_ES_PerfLogEntry(AppMainPerfId);

      if (Status == CFE_SUCCESS) {

         MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

         if (MsgId == CmdMid) {
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
         } 
         else if (MsgId == SendHkMid) {
            FILESYS_ManageTbl();
            FILEMGR_SendHousekeepingPkt();
         }
         else {
            CFE_EVS_SendEvent(FILEMGR_INVALID_MID_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);
         }

      } /* End if SB received a packet */
      else {
      
         RunStatus = CFE_ES_APP_ERROR;
      
      } /* End if SB failure */
      
   } /* End CFE_ES_RunLoop */

   CFE_ES_WriteToSysLog("FILEMGR App terminating, err = 0x%08X\n", Status);   /* Use SysLog, events may not be working */

   CFE_EVS_SendEvent(FILEMGR_EXIT_EID, CFE_EVS_CRITICAL, "FILEMGR App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of FILEMGR_AppMain() */


/******************************************************************************
** Function: FILEMGR_NoOpCmd
**
*/

boolean FILEMGR_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (FILEMGR_NOOP_EID, CFE_EVS_INFORMATION,
                      "No operation command received for FILEMGR App version %d.%d.%d",
                      FILEMGR_MAJOR_VER, FILEMGR_MINOR_VER, FILEMGR_PLATFORM_REV);

   return TRUE;


} /* End FILEMGR_NoOpCmd() */


/******************************************************************************
** Function: FILEMGR_ResetAppCmd
**
*/

boolean FILEMGR_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   CHILDMGR_ResetStatus(CHILDMGR_OBJ);
   
   DIR_ResetStatus();
   FILE_ResetStatus();
   FILESYS_ResetStatus();
	  
   return TRUE;

} /* End FILEMGR_ResetAppCmd() */


/******************************************************************************
** Function: FILEMGR_SendHousekeepingPkt
**
*/
void FILEMGR_SendHousekeepingPkt(void)
{
   
   FileMgr.HkPkt.CommandCounter    = FileMgr.CmdMgr.ValidCmdCnt;
   FileMgr.HkPkt.CommandErrCounter = FileMgr.CmdMgr.InvalidCmdCnt;

   FileMgr.HkPkt.NumOpenFiles = FileUtil_GetOpenFileList(&FileMgr.OpenFileList);

   FileMgr.HkPkt.ChildCmdCounter     = FileMgr.ChildMgr.ValidCmdCnt;
   FileMgr.HkPkt.ChildCmdErrCounter  = FileMgr.ChildMgr.InvalidCmdCnt;
   FileMgr.HkPkt.ChildCmdWarnCounter = FileMgr.File.CmdWarningCnt + FileMgr.Dir.CmdWarningCnt;
 
   FileMgr.HkPkt.ChildQueueCount = FileMgr.ChildMgr.CmdQ.Count;

   FileMgr.HkPkt.ChildCurrentCC  = FileMgr.ChildMgr.CurrCmdCode;
   FileMgr.HkPkt.ChildPreviousCC = FileMgr.ChildMgr.PrevCmdCode;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FileMgr.HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &FileMgr.HkPkt);

} /* End FILEMGR_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(uint32* AppMainPerfId)
{

   int32 Status = OSK_C_FW_CFS_ERROR;
   
   CHILDMGR_TaskInit ChildTaskInit;
   
   /*
   ** Initialize objects 
   */

   if (INITBL_Constructor(&FileMgr.IniTbl, FILEMGR_INI_FILENAME, &IniCfgEnum)) {
   
      *AppMainPerfId = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_MAIN_PERF_ID);
      CFE_ES_PerfLogEntry(*AppMainPerfId);

      /* Constructor sends error events */    
      ChildTaskInit.TaskName  = INITBL_GetStrConfig(INITBL_OBJ, CFG_CHILD_NAME);
      ChildTaskInit.StackSize = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_STACK_SIZE);
      ChildTaskInit.Priority  = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_PRIORITY);
      ChildTaskInit.PerfId    = INITBL_GetIntConfig(INITBL_OBJ, CHILD_TASK_PERF_ID);
      Status = CHILDMGR_Constructor(CHILDMGR_OBJ, 
                                    ChildMgr_TaskMainCmdDispatch,
                                    NULL, 
                                    &ChildTaskInit); 
  
   } /* End if INITBL Constructed */
  
   if (Status == CFE_SUCCESS) {

      DIR_Constructor(DIR_OBJ, &FileMgr.IniTbl);
      FILE_Constructor(FILE_OBJ, &FileMgr.IniTbl);
      FILESYS_Constructor(FILESYS_OBJ, &FileMgr.IniTbl);


      /*
      ** Initialize app level interfaces
      */
      
      CFE_SB_CreatePipe(&FileMgr.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH), INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME));  
      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_MID), FileMgr.CmdPipe);
      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_SEND_HK_MID), FileMgr.CmdPipe);

      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, FILEMGR_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, FILEMGR_ResetAppCmd, 0);

      CMDMGR_RegisterFunc(CMDMGR_OBJ, DIR_CREATE_CMD_FC,          CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, DIR_CREATE_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, DIR_DELETE_CMD_FC,          CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, DIR_DELETE_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, DIR_DELETE_ALL_CMD_FC,      CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, DIR_DELETE_ALL_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, DIR_SEND_LIST_PKT_CMD_FC,   CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, DIR_SEND_LIST_PKT_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, DIR_WRITE_LIST_FILE_CMD_FC, CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, DIR_WRITE_LIST_FILE_CMD_DATA_LEN);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, DIR_CREATE_CMD_FC,          DIR_OBJ, DIR_CreateCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, DIR_DELETE_CMD_FC,          DIR_OBJ, DIR_DeleteCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, DIR_DELETE_ALL_CMD_FC,      DIR_OBJ, DIR_DeleteAllCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, DIR_SEND_LIST_PKT_CMD_FC,   DIR_OBJ, DIR_SendListPktCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, DIR_WRITE_LIST_FILE_CMD_FC, DIR_OBJ, DIR_WriteListFileCmd);


      CMDMGR_RegisterFunc(CMDMGR_OBJ, FILE_CONCAT_CMD_FC,    CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, FILE_CONCATENATE_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, FILE_COPY_CMD_FC,      CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, FILE_COPY_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, FILE_DECOMPRESS_CMD_FC,CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, FILE_DECOMPRESS_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, FILE_DELETE_CMD_FC,    CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, FILE_DELETE_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, FILE_MOVE_CMD_FC,      CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, FILE_MOVE_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, FILE_RENAME_CMD_FC,    CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, FILE_RENAME_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, FILE_SEND_INFO_CMD_FC, CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, FILE_SEND_INFO_PKT_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, FILE_SET_PERMISSIONS_CMD_FC, CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, FILE_SET_PERMISSIONS_CMD_DATA_LEN);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, FILE_CONCAT_CMD_FC,    FILE_OBJ, FILE_ConcatenateCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, FILE_COPY_CMD_FC,      FILE_OBJ, FILE_CopyCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, FILE_DECOMPRESS_CMD_FC,FILE_OBJ, FILE_DecompressCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, FILE_DELETE_CMD_FC,    FILE_OBJ, FILE_DeleteCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, FILE_MOVE_CMD_FC,      FILE_OBJ, FILE_MoveCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, FILE_RENAME_CMD_FC,    FILE_OBJ, FILE_RenameCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, FILE_SEND_INFO_CMD_FC, FILE_OBJ, FILE_SendInfoPktCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, FILE_SET_PERMISSIONS_CMD_FC, FILE_OBJ, FILE_SetPermissionsCmd);

      /* 
      ** Alternative commands don't increment the main command counters. They do increment the child command counters which mimics
      ** the original FM app behavior, but I'm not sure that's desirable since the child counters are also used by ground ops.
      */
      CMDMGR_RegisterFuncAltCnt(CMDMGR_OBJ, FILE_DELETE_ALT_CMD_FC, CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, FILE_DELETE_CMD_DATA_LEN);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, FILE_DELETE_ALT_CMD_FC, FILE_OBJ, FILE_DeleteCmd);
 
      CMDMGR_RegisterFunc(CMDMGR_OBJ, FILESYS_SEND_OPEN_FILES_PKT_CMD_FC,  FILESYS_OBJ, FILESYS_SendOpenFilesPktCmd,  FILESYS_SEND_OPEN_FILES_PKT_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, FILESYS_SEND_TBL_PKT_CMD_FC,         FILESYS_OBJ, FILESYS_SendTblPktCmd,        FILESYS_SEND_TBL_PKT_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, FILESYS_SET_TBL_STATE_CMD_FC,        FILESYS_OBJ, FILESYS_SetTblStateCmd,       FILESYS_SET_TBL_STATE_CMD_DATA_LEN);

      CFE_SB_InitMsg(&FileMgr.HkPkt, (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_HK_TLM_MID), FILEMGR_TLM_HK_LEN, TRUE);

	   TBLMGR_Constructor(TBLMGR_OBJ);
   
      /*
      ** Application startup event message
      */
      CFE_EVS_SendEvent(FILEMGR_INIT_APP_EID, CFE_EVS_INFORMATION,
                        "FILEMGR App Initialized. Version %d.%d.%d",
                        FILEMGR_MAJOR_VER, FILEMGR_MINOR_VER, FILEMGR_PLATFORM_REV);
                        
   } /* End if CHILDMGR constructed */
   
   return(Status);

} /* End of InitApp() */
