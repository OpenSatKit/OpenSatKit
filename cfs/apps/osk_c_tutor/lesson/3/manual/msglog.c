/* LESSON 3 - Manual Updates
** 
** Make the changes below to osk_c_tutor/fsw/src/msglog.c created
** in lesson 2.
**
** Most of the changes are replacing all of the code within the stubbed
** functions overwriting any temporary code that was written in the 
** previous lesson. Since this is a self-guided tutorial it is up to you
** to read through the code in each function. The instructions below will
** highlight some important features, but they do not describe every line
** of code.
**
*/


/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** In the "Macro Definitions" section add the PARAM macros defined
** below.
*/

#define PARAM_FILE_PATH_BASE_NAME  "/cf/msg_"
#define PARAM_FILE_EXTENSION       ".txt"
#define PARAM_FILE_ENTRY_CNT       5



/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** In the "Local Function Prototypes" add the function prototypes below.
** These functions do not need to be public and they do the bulk of the
** functionality.  They were 'discovered' as the top-level logic was matured
** and they each perform a specific function. Note the use of verbs in the
** function names. Similar to an object-oriented class, object state variables
** are not passed as parameters and the functions (or methods) update the
** object's state.
*/

static void CreateFilename(void);
static void LogMessages(void);



/* >>>>>>>>>>>>>>> STEP 3 <<<<<<<<<<<<<<<
**
** In MSGLOG_Constructor() add the following at the end of the function
*/

   CFE_SB_CreatePipe(&MsgLog->MsgPipe, INITBL_GetIntConfig(MsgLog->IniTbl, CFG_MSGLOG_PIPE_DEPTH), 
                     INITBL_GetStrConfig(MsgLog->IniTbl, CFG_MSGLOG_PIPE_NAME));  


/* >>>>>>>>>>>>>>> STEP 4 <<<<<<<<<<<<<<<
**
** After the "MSGLOG_ResetStatus" function add the MSGLOG_RunChildFuncCmd()
** implementation as shown below.
*/

/******************************************************************************
** Function: MSGLOG_RunChildFuncCmd
**
** Notes:
**   1. This is not intended to be a ground command. This function provides a
**      mechanism for the parent app to periodically call a child task function.
**
*/
boolean MSGLOG_RunChildFuncCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   
   CFE_EVS_SendEvent (MSGLOG_PERIODIC_CMD_EID, CFE_EVS_DEBUG, 
                      "Run child function command called");

   if (MsgLog->LogEna) {
   
      LogMessages();
   
   } /* End if log in progress */
   

   /* Playback logic will go here */
   
   return TRUE;

} /* End MSGLOG_RunChildFuncCmd() */


/* >>>>>>>>>>>>>>> STEP 5 <<<<<<<<<<<<<<<
**  
**   Replace the previous lesson's stubs for the following functions. Note
**   playback stubs need to be retained so don't paste the content below
**   what exists from the previous lesson.
**   - MSGLOG_StartLogCmd()
**   - MSGLOG_StopLogCmd()
**   - StopLog()
**   - StopPlaybk()
**
**   The following new functions are included in the 
**   - CreateFilename()
**   - LogMessages()
**
**   Noteworthy Design/Code
**   - A message identifier is provided in the Start Log command. When the 
**     command is received the message is subscribed to on MsgLog's SB pipe
**     and when the logging is stopped the message is unsubscribed.
**   - The OSK framework utility FileUtil_GetFileInfo() is used in conjunction
**     with the FILEUTIL_FILE_EXISTS macro.  This is a common OSK pattern and
**     there are other macros to test for other file/dir conditions.
**   - Spoiler Alert: The hardcoded "PARAM_*" macros will be replaced by table
**     definitions that can be changed during runtime.
*/

/******************************************************************************
** Function: MSGLOG_StartLogCmd
**
** Notes:
**   1. See file prologue for logging/playback logic.
*/
boolean MSGLOG_StartLogCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   int32   SbStatus;
   boolean RetStatus = FALSE;
   MSGLOG_StartLogCmdMsg* StartLog = (MSGLOG_StartLogCmdMsg*)MsgPtr;
      
   if (MsgLog->LogEna){
      StopLog();
   }
   
   if (MsgLog->PlaybkEna){
      StopPlaybk();
   }

   SbStatus = CFE_SB_Subscribe(StartLog->MsgId, MsgLog->MsgPipe);

   if (SbStatus == CFE_SUCCESS) {
      
      MsgLog->LogCnt = 0;
      MsgLog->MsgId  = StartLog->MsgId;
      CreateFilename();

      MsgLog->FileHandle = OS_creat(MsgLog->Filename, OS_WRITE_ONLY);

      if (MsgLog->FileHandle >= OS_FS_SUCCESS) {
      
         RetStatus      = TRUE;
         MsgLog->LogEna = TRUE;

         CFE_EVS_SendEvent (MSGLOG_START_LOG_CMD_EID,
                            CFE_EVS_INFORMATION, "Created new log file %s with a maximum of %d entries",
                            MsgLog->Filename, PARAM_FILE_ENTRY_CNT);

      }
      else {
         
         CFE_SB_Unsubscribe(MsgLog->MsgId, MsgLog->MsgPipe);
         
         CFE_EVS_SendEvent (MSGLOG_START_LOG_CMD_EID, CFE_EVS_ERROR, 
                            "Start message log rejected. Error creating new log file %s. Return status = 0x%4X",
                            MsgLog->Filename, MsgLog->FileHandle);         
      
      }

   
   } /* End if SB subscribe */
   else {
   
      CFE_EVS_SendEvent (MSGLOG_START_LOG_CMD_EID, CFE_EVS_ERROR,
                         "Start message log rejected. SB message 0x%04X subscription failed, Status = 0x%04X",
                         StartLog->MsgId, SbStatus);
   
   }
   
   return RetStatus;

} /* End MSGLOG_StartLogCmd() */


/******************************************************************************
** Function: MSGLOG_StopLogCmd
**
*/
boolean MSGLOG_StopLogCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   if (MsgLog->LogEna) {
         
      StopLog();
      
   }
   else {
   
      CFE_EVS_SendEvent (MSGLOG_STOP_LOG_CMD_EID, CFE_EVS_INFORMATION,
                         "Stop log command received with no log in progress");
   
   }
   
   return TRUE;
   
} /* End MSGLOG_StopLogCmd() */


/******************************************************************************
** Function: CreateFilename
**
** Create a filename using the table-defined base path/filename, current
** message ID, and the table-defined extension. 
**
** Notes:
**   1. No string buffer error checking performed
*/
static void CreateFilename(void)
{

   int  i;
   char MsgIdStr[16];

   CFE_EVS_SendEvent (MSGLOG_START_LOG_CMD_EID, CFE_EVS_DEBUG,
                      "CreateFilename using table values: %s,%s,%d",
                      PARAM_FILE_PATH_BASE_NAME, 
                      PARAM_FILE_EXTENSION,
                      PARAM_FILE_ENTRY_CNT); 

   sprintf(MsgIdStr,"%04X",MsgLog->MsgId);

   strcpy (MsgLog->Filename, PARAM_FILE_PATH_BASE_NAME);

   i = strlen(MsgLog->Filename);  /* Starting position for message ID */
   strcat (&(MsgLog->Filename[i]), MsgIdStr);
   
   i = strlen(MsgLog->Filename);  /* Starting position for extension */
   strcat (&(MsgLog->Filename[i]), PARAM_FILE_EXTENSION);
   

} /* End CreateFilename() */


/******************************************************************************
** Functions: LogMessages
**
** Read messages from SB, convert header to hex text, and write it to
** log file.
*/
static void LogMessages(void)
{
   
   int32         Status;
   CFE_SB_Msg_t  *MsgPtr;
   uint8         MsgHdrInt[6];
   char          MsgHdrStr[MSGLOG_PRI_HDR_HEX_CHAR+1];  /* '+1' for string terminator */
   
   
   do
   {
   
      Status = CFE_SB_RcvMsg(&MsgPtr, MsgLog->MsgPipe, CFE_SB_POLL);

      if (Status == CFE_SUCCESS) {
         
         memcpy(MsgHdrInt,MsgPtr,6);
         sprintf(MsgHdrStr," %02X%02X %02X%02X %02X%02X\n",  /* Must be MSGLOG_PRI_HDR_HEX_CHAR in length */
                 MsgHdrInt[0],MsgHdrInt[1],MsgHdrInt[2],
                 MsgHdrInt[3],MsgHdrInt[4],MsgHdrInt[5]);

         Status = OS_write(MsgLog->FileHandle, MsgHdrStr, MSGLOG_PRI_HDR_HEX_CHAR);

         MsgLog->LogCnt++;
         if (MsgLog->LogCnt >= PARAM_FILE_ENTRY_CNT) {
            
            StopLog();
            
         }
      
      } /* End SB read */
      
                 
   } while((Status == CFE_SUCCESS) && MsgLog->LogEna);
      

} /* End LogMessages() */


/******************************************************************************
** Function: StopLog
**
** Notes:
**   1. Assumes caller checked if log was in progress
*/
static void StopLog(void)
{
   
   OS_close(MsgLog->FileHandle);
   CFE_SB_Unsubscribe(MsgLog->MsgId, MsgLog->MsgPipe);
   MsgLog->LogEna = FALSE;
   
   CFE_EVS_SendEvent (MSGLOG_STOP_LOG_CMD_EID, CFE_EVS_INFORMATION,
                      "Logging stopped. Closed log file %s with %d entries", 
                      MsgLog->Filename, MsgLog->LogCnt);

}/* End StopLog() */

