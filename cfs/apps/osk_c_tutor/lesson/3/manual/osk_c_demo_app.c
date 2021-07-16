/* LESSON 3 - Manual Updates
** 
** Make the changes below to osk_c_tutor/fsw/src/msglog.h created
** in lesson 2.
**
** When developing the code it's an terative process of creating code
** and discovering what data is needed and identifying data to support
** the code. In this tutorial you will be udating all of the data first
** and itwil become clear how it is used when msglog.c is updated.
*/

/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** In the SendHousekeepingPkt() replace the code that copies stub data
** into the HK packet with the code below. 
*/

   OskCDemo.HkPkt.MsgLogCnt    = OskCDemo.MsgLog.LogCnt;
   
   strncpy(OskCDemo.HkPkt.MsgLogFilename, OskCDemo.MsgLog.Filename, OS_MAX_PATH_LEN);



/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** In the ProcessCommands() "else if (MsgId == OskCDemo.ExecuteMid)" clause
** replace the "Placeholder" comment with the following code.
**
*/

MSGLOG_RunChildFuncCmd (MSGLOG_OBJ, (CFE_SB_Msg_t*)&OskCDemo.MsgLogRunChildFuncCmd);

