/* LESSON 3 - Manual Updates
** 
** Make the changes below to osk_c_tutor/fsw/src/msglog.h created
** in lesson 2 to add message logging functionality.
**
** When developing the code it's an iterative process of creating code
** and discovering what data is needed and identifying data to support
** the code. In this tutorial you will be udating all of the data first
** and it will become clear how it is used when msglog.c is updated.
*/

/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** In the "Macro Definitions" section add under the "Number of hex characters..."
** comment add the following definition.
*/

#define MSGLOG_PRI_HDR_HEX_CHAR  16  


/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** In the "MSGLOG_Class" section under the "Framework References" comment add the
** following definitions
*/

   CFE_SB_PipeId_t MsgPipe;

   
/* >>>>>>>>>>>>>>> STEP 3 <<<<<<<<<<<<<<<
**
** In the "MSGLOG_Class" section under the "Class State Data" comment add the
** following definitions. 
*/


   uint16   LogCnt;   
   
   uint16   MsgId;
   int32    FileHandle;
   char     Filename[OS_MAX_PATH_LEN];
    


/* >>>>>>>>>>>>>>> STEP 4 <<<<<<<<<<<<<<<
**
** Below the "MSGLOG_ResetStatus" function prototye add a new function
** protoype for a function that will execute each time the execute message
** is received by the main app on it's command pipe. In a later lesson
** this function will run within a child task which accounts for its name. 
*/

/******************************************************************************
** Function: MSGLOG_RunChildFuncCmd
**
** Notes:
**   1. This is not intended to be a ground command. This function provides a
**      mechanism for the parent app to periodically call a child task function.
**
*/
boolean MSGLOG_RunChildFuncCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


