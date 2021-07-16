/* LESSON 5 - Solution
** Purpose: Implement the MSGLOG_Class methods
**
** Notes:
**   1. The log/playback functionality is for demonstration purposes and 
**      the logic is kept simple so users can focus on learning developing 
**      apps using the OSK C Framework.
**   2. Logging and playback can't be enabled at the same time. If a command
**      to start a playback is received when logging is in progress, the
**      logging will be stopped and a playback will be started. The same
**      occurs in reverse when a playback is in progress and a command to 
**      start a message log is received. Neither case is considered an error.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Application Developers Guide
**   2. cFS Application Developer's Guide
*/

/*
** Include Files:
*/

#include <string.h>

#include "app_cfg.h"
#include "initbl.h"
#include "msglog.h"


/***********************/
/** Macro Definitions **/
/***********************/


/* Convenience macro */
#define TBL_OBJ (&(MsgLog->Tbl))


/**********************/
/** Global File Data **/
/**********************/

static MSGLOG_Class*  MsgLog = NULL;


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static void CreateFilename(void);
static void LogMessages(void);
static void PlaybkMessages(void);
static void StopLog(void);
static void StopPlaybk(void);


/******************************************************************************
** Function: MSGLOG_Constructor
**
*/
void MSGLOG_Constructor(MSGLOG_Class*  MsgLogPtr, INITBL_Class* IniTbl)
{
 
   MsgLog = MsgLogPtr;

   CFE_PSP_MemSet((void*)MsgLog, 0, sizeof(MSGLOG_Class));
 
   MsgLog->IniTbl = IniTbl;
   
   CFE_SB_CreatePipe(&MsgLog->MsgPipe, INITBL_GetIntConfig(MsgLog->IniTbl, CFG_MSGLOG_PIPE_DEPTH), 
                     INITBL_GetStrConfig(MsgLog->IniTbl, CFG_MSGLOG_PIPE_NAME));  

  CFE_SB_InitMsg(&MsgLog->PlaybkPkt, (CFE_SB_MsgId_t)INITBL_GetIntConfig(MsgLog->IniTbl,
                  CFG_PLAYBK_TLM_MID), sizeof(MSGLOG_PlaybkPkt), TRUE);

   MSGLOGTBL_Constructor(TBL_OBJ, IniTbl);

} /* End MSGLOG_Constructor */


/******************************************************************************
** Function:  MSGLOG_ResetStatus
**
*/
void MSGLOG_ResetStatus()
{
 
   /* Nothing to do */
   
} /* End MSGLOG_ResetStatus() */


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
   

   if (MsgLog->PlaybkEna) {
      
      MsgLog->PlaybkDelay++;
      if (MsgLog->PlaybkDelay > MsgLog->Tbl.Data.PlaybkDelay) {
         
         MsgLog->PlaybkDelay = 0;
         PlaybkMessages();
      
      }
   
   } /* End if playback enabled */

   return TRUE;

} /* End MSGLOG_RunChildFuncCmd() */


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
                            MsgLog->Filename, MsgLog->Tbl.Data.File.EntryCnt);

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
                      MsgLog->Tbl.Data.File.PathBaseName, 
                      MsgLog->Tbl.Data.File.Extension,
                      MsgLog->Tbl.Data.File.EntryCnt); 

   sprintf(MsgIdStr,"%04X",MsgLog->MsgId);

   strcpy (MsgLog->Filename, MsgLog->Tbl.Data.File.PathBaseName);

   i = strlen(MsgLog->Filename);  /* Starting position for message ID */
   strcat (&(MsgLog->Filename[i]), MsgIdStr);
   
   i = strlen(MsgLog->Filename);  /* Starting position for extension */
   strcat (&(MsgLog->Filename[i]), MsgLog->Tbl.Data.File.Extension);
   

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
         if (MsgLog->LogCnt >= MsgLog->Tbl.Data.File.EntryCnt) {
            
            StopLog();
            
         }
      
      } /* End SB read */
      
                 
   } while((Status == CFE_SUCCESS) && MsgLog->LogEna);
      

} /* End LogMessages() */


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


