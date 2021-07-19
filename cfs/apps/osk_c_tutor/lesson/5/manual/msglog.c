/* LESSON 5 - Manual Updates
** 
** Make the changes below to osk_c_tutor/fsw/src/msglog.c created
** in a previous lesson.
**
*/


/* >>>>>>>>>>>>>>> STEP 1 <<<<<<<<<<<<<<<
**
** In the "Local Function Prototypes" add the function prototype below
** I personally like to keep the prototypes alphabetical.
** 
*/

static void PlaybkMessages(void);


/* >>>>>>>>>>>>>>> STEP 2 <<<<<<<<<<<<<<<
**
** In MSGLOG_Constructor() add the following at the end of the function to
** initialize the playback telemetry packet. 
*/


   CFE_SB_InitMsg(&MsgLog->PlaybkPkt, (CFE_SB_MsgId_t)INITBL_GetIntConfig(MsgLog->IniTbl,
                  CFG_PLAYBK_TLM_MID), sizeof(MSGLOG_PlaybkPkt), TRUE);



/* >>>>>>>>>>>>>>> STEP 3 <<<<<<<<<<<<<<<
**
** In MSGLOG_RunChildFuncCmd() replace the "Playback logic will go here" 
** comment with the code below. If the comment is missing the code should 
** go after the 'if (MsgLog->LogEna) {...}' code block.
**
** Noteworthy Design/Code
**  - Now that the app has a table the playback delay value is simply defined
**    in the table and accessed via 'MsgLog->Tbl.Data.PlaybkDelay'
*/

   
   if (MsgLog->PlaybkEna) {
      
      MsgLog->PlaybkDelay++;
      if (MsgLog->PlaybkDelay > MsgLog->Tbl.Data.PlaybkDelay) {
         
         MsgLog->PlaybkDelay = 0;
         PlaybkMessages();
      
      }
   
   } /* End if playback enabled */
   

/* >>>>>>>>>>>>>>> STEP 4 <<<<<<<<<<<<<<<
**  
**   Replace the previous lesson's stubs for the following functions.
**   - MSGLOG_StartPlaybkCmd()
**   - MSGLOG_StopPlaybkCmd()
**   - StopPlaybk()
**   - StopPlaybk()
**
**   Noteworthy Design/Code
**   - The OSK framework utility FileUtil_GetFileInfo() is used in conjunction
**     with the FILEUTIL_FILE_EXISTS macro.  This is a common OSK pattern and
**     there are other macros to test for other file/dir conditions.
*/


/******************************************************************************
** Function: MSGLOG_StartPlaybkCmd
**
** Notes:
**   1. See file prologue for logging/playback logic.
*/
boolean MSGLOG_StartPlaybkCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   boolean RetStatus = FALSE;
   FileUtil_FileInfo FileInfo;
   

   if (MsgLog->LogEna){
      StopLog();
   }

   if (!MsgLog->PlaybkEna) {
      
      if (MsgLog->LogCnt > 0) {

         FileInfo = FileUtil_GetFileInfo(MsgLog->Filename, OS_MAX_PATH_LEN, FALSE);
          
         if (FILEUTIL_FILE_EXISTS(FileInfo.State)) {

            RetStatus           = TRUE;
            MsgLog->PlaybkEna   = TRUE;
            MsgLog->PlaybkCnt   = 0;
            MsgLog->PlaybkDelay = 0;
            MsgLog->FileHandle  = OS_open(MsgLog->Filename, OS_READ_ONLY, 0);
            
            CFE_EVS_SendEvent (MSGLOG_START_PLAYBK_CMD_EID, CFE_EVS_INFORMATION,
                              "Playback file %s started with a %d cycle delay between updates",
                              MsgLog->Filename, MsgLog->Tbl.Data.PlaybkDelay);
         
         } /* End if file exists */
         else {
         
            CFE_EVS_SendEvent (MSGLOG_START_PLAYBK_CMD_EID, CFE_EVS_ERROR,
                              "Start playback failed. Message log file does not exist.");

         }
      
      } /* MsgLog->LogCnt > 0 */
      else {
         
         CFE_EVS_SendEvent (MSGLOG_START_PLAYBK_CMD_EID, CFE_EVS_ERROR,
                            "Start playback failed. Message log count is zero");
      }
   } /* End if playback not in progress */ 
   else {

      CFE_EVS_SendEvent (MSGLOG_START_PLAYBK_CMD_EID, CFE_EVS_ERROR,
                         "Start playback ignored. Playback already in progress.");

   }
   
   return RetStatus;
   
} /* End MSGLOG_StartPlaybkCmd() */


/******************************************************************************
** Function: MSGLOG_StopPlaybkCmd
**
*/
boolean MSGLOG_StopPlaybkCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   if (MsgLog->PlaybkEna) {
         
      StopPlaybk();
      
   }
   else {
   
      CFE_EVS_SendEvent (MSGLOG_STOP_LOG_CMD_EID, CFE_EVS_INFORMATION,
                         "Stop playback command received with no playback in progress");
   
   }
   
   return TRUE;
   
} /* End MSGLOG_StopPlaybkCmd() */


/******************************************************************************
** Function: StopPlaybk
**
** Notes:
**   1. Assumes caller checked if playback was in progress. 
**   2. Clears playback state data and sends a final playback telemetry packet
*/
static void StopPlaybk(void)
{
   
   OS_close(MsgLog->FileHandle);
   
   MsgLog->PlaybkEna = FALSE;
   MsgLog->PlaybkPkt.LogFileEntry = 0;
   memset(MsgLog->PlaybkPkt.HdrTxt,'\0',MSGLOG_PRI_HDR_HEX_CHAR);

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &(MsgLog->PlaybkPkt));
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &(MsgLog->PlaybkPkt));
   
   CFE_EVS_SendEvent (MSGLOG_STOP_PLAYBK_CMD_EID, CFE_EVS_INFORMATION,
                      "Playback stopped. Closed log file %s", MsgLog->Filename);

}/* End StopPlaybk() */



/* >>>>>>>>>>>>>>> STEP 5 <<<<<<<<<<<<<<<
**  
**   Add the following new function
**   - PlaybkMessages()
**
**   Noteworthy Design/Code
**   - See function prologue for a design trade.
*/

/******************************************************************************
** Functions: PlaybkMessages
**
** Copy one message into playback telemetry packet and send the telemetry
** packet. 
**
** Notes:
**   1. Uses log counter to determine when to wrap around to start of file.
**      Startlog command ensures at least one message is in the log file.
**   2. FileUtil_ReadLine() could have been used for reading text file lines
**      but decided on fixed sized binary reads 
**
*/
static void PlaybkMessages(void)
{
   
   int32    SysStatus;

   if (MsgLog->PlaybkCnt < MsgLog->LogCnt) {
   
      SysStatus = OS_read(MsgLog->FileHandle, MsgLog->PlaybkPkt.HdrTxt, MSGLOG_PRI_HDR_HEX_CHAR);

      if (SysStatus == MSGLOG_PRI_HDR_HEX_CHAR) {
      
         MsgLog->PlaybkPkt.LogFileEntry = MsgLog->PlaybkCnt;
         
         CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &(MsgLog->PlaybkPkt));
         CFE_SB_SendMsg((CFE_SB_Msg_t *) &(MsgLog->PlaybkPkt));

         MsgLog->PlaybkCnt++;
         if (MsgLog->PlaybkCnt >= MsgLog->LogCnt) {
         
            MsgLog->PlaybkCnt = 0;
            OS_lseek(MsgLog->FileHandle, 0, OS_SEEK_SET);
         
         }
      }
      else {
         StopPlaybk();   
      }
      
   }
   else {
      StopPlaybk();   
   }

} /* End PlaybkMessages() */


