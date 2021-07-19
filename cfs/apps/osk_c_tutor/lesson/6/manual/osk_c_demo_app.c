/* LESSON 6 - Manual Updates
** 
** Make the changes below to the current osk_c_tutor/fsw/src/osk_c_demo_app.c
** created from previous lessons.
** 
*/


/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** In the convenience macro section add the following:
*/

#define  CHILDMGR_OBJ  (&(OskCDemo.ChildMgr))

/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** In SendHousekeepingPkt() replace the child command counter code
** with the following code.
*/

   OskCDemo.HkPkt.ChildValidCmdCnt   = OskCDemo.ChildMgr.ValidCmdCnt;
   OskCDemo.HkPkt.ChildInvalidCmdCnt = OskCDemo.ChildMgr.InvalidCmdCnt;


/* >>>>>>>>>>>>>>> STEP 3 <<<<<<<<<<<<<<<
**
** In InitApp() in the local variable declarations below 'int32 Status'
** add the following line
*/

   CHILDMGR_TaskInit ChildTaskInit;
   
/* >>>>>>>>>>>>>>> STEP 4 <<<<<<<<<<<<<<<
**
** In InitApp() below the 'Child Manager constructor sends error events' comment add the 
** following code.
**
** Note all of the child task runtime parameters come from the app's init table
** JSON file.  Passing the address of ChildMgr_TaskMainCmdDispatch() indicates
** means the child task execution will be command driven. 
*/

      ChildTaskInit.TaskName  = INITBL_GetStrConfig(INITBL_OBJ, CFG_CHILD_NAME);
      ChildTaskInit.StackSize = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_STACK_SIZE);
      ChildTaskInit.Priority  = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_PRIORITY);
      ChildTaskInit.PerfId    = INITBL_GetIntConfig(INITBL_OBJ, CHILD_PERF_ID);

      Status = CHILDMGR_Constructor(CHILDMGR_OBJ, 
                                    ChildMgr_TaskMainCmdDispatch,
                                    NULL, 
                                    &ChildTaskInit); 
                                    

/* >>>>>>>>>>>>>>> STEP 5 <<<<<<<<<<<<<<<
**
** In InitApp() paste the following code over the following existing four lines
**
**      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_START_LOG_CMD_FC,    MSGLOG_OBJ, MSGLOG_StartLogCmd,    MSGLOG_START_LOG_CMD_DATA_LEN);
**      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_STOP_LOG_CMD_FC,     MSGLOG_OBJ, MSGLOG_StopLogCmd,     MSGLOG_STOP_LOG_CMD_DATA_LEN);
**      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_START_PLAYBK_CMD_FC, MSGLOG_OBJ, MSGLOG_StartPlaybkCmd, MSGLOG_START_PLAYBK_CMD_DATA_LEN);
**      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_STOP_PLAYBK_CMD_FC,  MSGLOG_OBJ, MSGLOG_StopPlaybkCmd,  MSGLOG_STOP_PLAYBK_CMD_DATA_LEN);
**
** The new code changes the start/stop log/playback commands to be dispatched as child task commands. It also registers MSGLOG_RunChildFuncCmd()
** that was directly called by ProcessCommands() in previous lessons to be an alternate command (no command counter increment) that
** runs within the child task. 
*/

      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_START_LOG_CMD_FC,    CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, MSGLOG_START_LOG_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_STOP_LOG_CMD_FC,     CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, MSGLOG_STOP_LOG_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_START_PLAYBK_CMD_FC, CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, MSGLOG_START_PLAYBK_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_STOP_PLAYBK_CMD_FC,  CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, MSGLOG_STOP_PLAYBK_CMD_DATA_LEN);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, MSGLOG_START_LOG_CMD_FC,    MSGLOG_OBJ, MSGLOG_StartLogCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, MSGLOG_STOP_LOG_CMD_FC,     MSGLOG_OBJ, MSGLOG_StopLogCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, MSGLOG_START_PLAYBK_CMD_FC, MSGLOG_OBJ, MSGLOG_StartPlaybkCmd);
      CHILDMGR_RegisterFunc(CHILDMGR_OBJ, MSGLOG_STOP_PLAYBK_CMD_FC,  MSGLOG_OBJ, MSGLOG_StopPlaybkCmd);

      /* 
      ** Alternative commands don't increment the main command counters, but they do increment the child command counters.
      ** This "command" is used by the app's main loop to perform periodic processing 
      */
      CMDMGR_RegisterFuncAltCnt(CMDMGR_OBJ, MSGLOG_RUN_CHILD_ALT_CMD_FC, CHILDMGR_OBJ, CHILDMGR_InvokeChildCmd, PKTUTIL_NO_PARAM_CMD_DATA_LEN);
      CHILDMGR_RegisterFuncAltCnt(CHILDMGR_OBJ, MSGLOG_RUN_CHILD_ALT_CMD_FC, MSGLOG_OBJ, MSGLOG_RunChildFuncCmd);


/* >>>>>>>>>>>>>>> STEP 6 <<<<<<<<<<<<<<<
**
** In InitApp() next to the 'CFE_SB_InitMsg(&OskCDemo.HkPkt...' line paste the code below. 
**
** This initializes the MsgLogRunChildFuncCmd command packet
*/

      CFE_SB_InitMsg((CFE_SB_MsgPtr_t)&OskCDemo.MsgLogRunChildFuncCmd, (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_EXECUTE_MID), sizeof(PKTUTIL_NoParamCmdMsg), TRUE);
      CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&OskCDemo.MsgLogRunChildFuncCmd, MSGLOG_RUN_CHILD_ALT_CMD_FC);
      CFE_SB_GenerateChecksum((CFE_SB_MsgPtr_t)&OskCDemo.MsgLogRunChildFuncCmd);


/* >>>>>>>>>>>>>>> STEP 7 <<<<<<<<<<<<<<<
**
** In ProcessCommands() replace with the code below.
**   MSGLOG_RunChildFuncCmd (MSGLOG_OBJ, (CFE_SB_Msg_t*)&OskCDemo.MsgLogRunChildFuncCmd);
**
*/

         CMDMGR_DispatchFunc(CMDMGR_OBJ, (CFE_SB_Msg_t*)&OskCDemo.MsgLogRunChildFuncCmd);
