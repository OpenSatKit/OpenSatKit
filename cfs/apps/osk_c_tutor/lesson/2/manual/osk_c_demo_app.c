/* LESSON 2 - Manual Updates
** 
** Make the changes below to osk_c_tutor/fsw/src/osk_c_demo_app.c created
** in lesson 1.
**
** Note that the Message Log object functions are stubs except for log enabled
** amd playback enabled flags. These are included in this lesson so you can 
** send commands and see responses in telemetry. This also means the HK telemetry
** needs to be updated with MessageLog's current values. 
*/

/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** In the convenience macro section add the following:
*/

#define  MSGLOG_OBJ    (&(OskCDemo.MsgLog))

/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** In OSK_C_DEMO_ResetAppCmd() add the following:
*/

   MSGLOG_ResetStatus();


/* >>>>>>>>>>>>>>> STEP 3 <<<<<<<<<<<<<<<
**
** In SendHousekeepingPkt() replace with code below
**   OskCDemo.HkPkt.MsgLogEna    = 0;
**   OskCDemo.HkPkt.MsgPlaybkEna = 0;
*/

   OskCDemo.HkPkt.MsgLogEna    = OskCDemo.MsgLog.LogEna;
   OskCDemo.HkPkt.MsgPlaybkEna = OskCDemo.MsgLog.PlaybkEna;
   
   
/* >>>>>>>>>>>>>>> STEP 4 <<<<<<<<<<<<<<<
**

** In InitApp()'s "Constuct app objects" section add:
*/

      MSGLOG_Constructor(MSGLOG_OBJ, &OskCDemo.IniTbl);



/* >>>>>>>>>>>>>>> STEP 5 <<<<<<<<<<<<<<<
**
** In InitApp()'s "Initialize app level interfaces" section
** under CMDMGR_RegisterFunc(CMDMGR_OBJ, OSK_C_DEMO_TBL_DUMP_CMD_FC...)
** register Message Log's 4 commands by adding:
*/

      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_START_LOG_CMD_FC,    MSGLOG_OBJ, MSGLOG_StartLogCmd,    MSGLOG_START_LOG_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_STOP_LOG_CMD_FC,     MSGLOG_OBJ, MSGLOG_StopLogCmd,     MSGLOG_STOP_LOG_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_START_PLAYBK_CMD_FC, MSGLOG_OBJ, MSGLOG_StartPlaybkCmd, MSGLOG_START_PLAYBK_CMD_DATA_LEN);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, MSGLOG_STOP_PLAYBK_CMD_FC,  MSGLOG_OBJ, MSGLOG_StopPlaybkCmd,  MSGLOG_STOP_PLAYBK_CMD_DATA_LEN);
