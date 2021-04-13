/* 

** Purpose: Provide a service to playback cFE_EVS event log file in telemetry.
**
** Notes:
**   None
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

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "cfe_evs_extern_typedefs.h"
#include "cfe_evs_msg.h"
#include "app_cfg.h"
#include "evt_plbk.h"

/*
** Global File Data
*/

static EVT_PLBK_Class*  EvtPlbk = NULL;

static CFE_EVS_WriteLogDataFile_t WriteEvsLogFileCmd;

/*
** Local Function Prototypes
*/

static void SendEventTlmMsg(void);
static boolean LoadLogFile(void);


/******************************************************************************
** Function: EVT_PLBK_Constructor
**
*/
void EVT_PLBK_Constructor(EVT_PLBK_Class* EvtPlbkPtr, CFE_SB_MsgId_t TlmMsgId, 
                          uint16 HkCyclePeriod, char* EvsLogFilename,
                          CFE_SB_MsgId_t CfeEvsCmdMid)
{

   EvtPlbk = EvtPlbkPtr;

   memset ((void*)EvtPlbk, 0, sizeof(EVT_PLBK_Class));   /* Enabled set to FALSE */
   
   EvtPlbk->HkCyclePeriod = HkCyclePeriod;
   strncpy(EvtPlbk->EvsLogFilename, EvsLogFilename, CFE_MISSION_MAX_PATH_LEN);
   
   CFE_SB_InitMsg(&(EvtPlbk->TlmMsg), TlmMsgId, EVT_PLBK_TLM_MSG_LEN, TRUE);
   
   /* The file name & checksum are set prior to sending the command */
   CFE_SB_InitMsg(&WriteEvsLogFileCmd, CfeEvsCmdMid, sizeof(CFE_EVS_WriteLogDataFile_t), TRUE);
   CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&WriteEvsLogFileCmd, CFE_EVS_WRITE_LOG_DATA_FILE_CC);
   
} /* End EVT_PLBK_Constructor() */


/******************************************************************************
** Function: EVT_PLBK_ResetStatus
**
*/
void EVT_PLBK_ResetStatus(void)
{

   /* Nothing to do */

} /* End EVT_PLBK_ResetStatus() */


/******************************************************************************
** Function: EVT_PLBK_Execute
**
*/
void EVT_PLBK_Execute(void)
{

   CFE_TIME_SysTime_t  AttemptTime;
   
   if (EvtPlbk->Enabled) {
   
      if (EvtPlbk->LogFileCopied) {

         EvtPlbk->HkCycleCount++;
         
         if (EvtPlbk->HkCycleCount >= EvtPlbk->HkCyclePeriod) {
            
            SendEventTlmMsg();
            EvtPlbk->HkCycleCount = 0;
         
         }         
      } /* End if LogFileCopied */
      else {

         if (LoadLogFile()) {
         
            EvtPlbk->LogFileCopied = TRUE;
         
         }
         else {
         
            EvtPlbk->EvsLogFileOpenAttempts++;
            
            if (EvtPlbk->EvsLogFileOpenAttempts > 2) {
            
               AttemptTime = CFE_TIME_Subtract(CFE_TIME_GetTime(), EvtPlbk->StartTime);
               
               CFE_EVS_SendEvent(EVT_PLBK_LOG_READ_ERR_EID, CFE_EVS_ERROR, 
                                 "Failed to read event log file %s after %d attempts over %d seconds",
                                 WriteEvsLogFileCmd.Payload.LogFilename, (EvtPlbk->EvsLogFileOpenAttempts-1), AttemptTime.Seconds);
                                 
               EvtPlbk->Enabled = FALSE;
       
            }
            
         } /* End if !LoadLogFile() */ 
         
      } /* End if !LogFileCopied */
   
   } /* End if enabled */
     
} /* EVT_PLBK_Execute() */


/******************************************************************************
** Function: EVT_PLBK_ConfigCmd
**
** - Configure the behavior of playbacks. See command parameters definitions
**   for details. 
** - Only verify filename is valid. CFE_EVS will perform checks regarding 
**   whether the log file can be created.
**
*/
boolean EVT_PLBK_ConfigCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const EVT_PLBK_ConfigCmdMsg *ConfigCmd = (const EVT_PLBK_ConfigCmdMsg *) MsgPtr;

   boolean  RetStatus = FALSE;

   EvtPlbk->HkCyclePeriod = ConfigCmd->HkCyclesPerPkt;

   if (FileUtil_VerifyFilenameStr(ConfigCmd->EvsLogFilename)) {
      
      strncpy(EvtPlbk->EvsLogFilename, ConfigCmd->EvsLogFilename, CFE_MISSION_MAX_PATH_LEN);
   
      CFE_EVS_SendEvent(EVT_PLBK_CFG_CMD_EID, CFE_EVS_INFORMATION, 
                        "Config playback command accepted with log file %s and HK period %d",
                        WriteEvsLogFileCmd.Payload.LogFilename, ConfigCmd->HkCyclesPerPkt);

      RetStatus = TRUE;
      
   }
   else {
      
      CFE_EVS_SendEvent(EVT_PLBK_CFG_CMD_ERR_EID, CFE_EVS_ERROR, 
                        "Config playback command rejected, invalid filename %s",
                        WriteEvsLogFileCmd.Payload.LogFilename);

   
   }
   
   return RetStatus;

} /* End of EVT_PLBK_ConfigCmd() */


/******************************************************************************
** Function: EVT_PLBK_StartCmd
**
** Remove log file if it exists because the playback logic checks to see if the
** log exists and don't want an old playback file confusing the logic. 
*/
boolean EVT_PLBK_StartCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   int32   SysStatus;
   FileUtil_FileInfo FileInfo;

   FileInfo = FileUtil_GetFileInfo(EvtPlbk->EvsLogFilename, OS_MAX_PATH_LEN, FALSE);

   if (FileInfo.State == FILEUTIL_FILE_CLOSED) {
      SysStatus = OS_remove(EvtPlbk->EvsLogFilename);
   }
   
   strncpy(WriteEvsLogFileCmd.Payload.LogFilename, EvtPlbk->EvsLogFilename, CFE_MISSION_MAX_PATH_LEN);
   
   CFE_SB_GenerateChecksum((CFE_SB_Msg_t*)&WriteEvsLogFileCmd);
   CFE_SB_SendMsg((CFE_SB_Msg_t*)&WriteEvsLogFileCmd);

   EvtPlbk->StartTime = CFE_TIME_GetTime();   

   EvtPlbk->Enabled = TRUE;
   EvtPlbk->HkCycleCount = 0;

   EvtPlbk->LogFileCopied = FALSE;
   EvtPlbk->EvsLogFileOpenAttempts = 0;
   
   CFE_EVS_SendEvent(EVT_PLBK_SENT_WRITE_LOG_CMD_EID, CFE_EVS_INFORMATION, 
                     "Commanded CFE_EVS to write event log to %s. Event tlm HK period = %d",
                     WriteEvsLogFileCmd.Payload.LogFilename, EvtPlbk->HkCyclePeriod);
   
   return TRUE;
   
} /* End EVT_PLBK_StartCmd() */


/******************************************************************************
** Function: EVT_PLBK_StopCmd
**
*/
boolean EVT_PLBK_StopCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   EvtPlbk->Enabled = FALSE;
   EvtPlbk->LogFileCopied = FALSE;
   EvtPlbk->HkCycleCount = 0;
   
   CFE_EVS_SendEvent(EVT_PLBK_STOP_CMD_EID, CFE_EVS_INFORMATION, "Event playback stopped");
   
   return TRUE;
   
} /* End EVT_PLBK_StopCmd() */


/******************************************************************************
** Function: LoadLogFile()
**
*/
static boolean LoadLogFile(void)
{

   boolean RetStatus = FALSE;
   boolean ReadingFile;
   uint16  i;
   int32   FileHandle;
   int32   FileStatus;
   int32   ReadLength;
   EVT_PLBK_TlmEvent*     TlmEvent;
   FileUtil_FileInfo      FileInfo;
   CFE_FS_Header_t        CfeHeader;
   CFE_EVS_LongEventTlm_t EvsLogEventTlm;
   CFE_EVS_LongEventTlm_Payload_t* LogEvent;



   FileInfo = FileUtil_GetFileInfo(EvtPlbk->EvsLogFilename, OS_MAX_PATH_LEN, FALSE);
    
   if (FILEUTIL_FILE_EXISTS(FileInfo.State)) {
   
      if ((FileHandle = OS_open(EvtPlbk->EvsLogFilename, OS_READ_ONLY, 0)) >= OS_SUCCESS) {
      
         FileStatus = CFE_FS_ReadHeader(&CfeHeader, FileHandle);
         
         if (FileStatus == sizeof(CFE_FS_Header_t)) {
            
            if (CfeHeader.SubType == CFE_FS_SubType_EVS_EVENTLOG) {
                        
               /* 
               ** Event log file:
               ** - Contains full event message with CCSDS header
               ** - Only contains actual events, i.e. no null entries to pad to max entries
               */
               ReadingFile = TRUE;
               for (i=0; ((i < CFE_PLATFORM_EVS_LOG_MAX) && ReadingFile); i++) { 
               
                  ReadLength = OS_read(FileHandle, &EvsLogEventTlm, sizeof(CFE_EVS_LongEventTlm_t));
                  if (ReadLength == sizeof(CFE_EVS_LongEventTlm_t)) {
                     
                     TlmEvent = &EvtPlbk->EventLog.Msg[i].Tlm;
                     LogEvent = &EvsLogEventTlm.Payload;

                     TlmEvent->Time = CFE_SB_GetMsgTime((CFE_SB_MsgPtr_t) &(EvsLogEventTlm.TlmHeader));
                     TlmEvent->EventId   = LogEvent->PacketID.EventID;
                     TlmEvent->EventType = LogEvent->PacketID.EventType;
                     strncpy(TlmEvent->AppName, LogEvent->PacketID.AppName, CFE_MISSION_MAX_API_LEN);
                     strncpy(TlmEvent->Message, LogEvent->Message, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
                     
                     EvtPlbk->EventLog.Msg[i].Loaded = TRUE;
                  
                  }
                  else {
                     ReadingFile = FALSE;               
                  }

               } /* End file read loop */

            } /* End if valid file header subtype */
            else {
                  
               CFE_EVS_SendEvent(EVT_PLBK_LOG_HDR_TYPE_ERR_EID, CFE_EVS_ERROR,
                                 "Invalid file header subtype %d for event log file %s", CfeHeader.SubType, EvtPlbk->EvsLogFilename);
                                    
            } /* End if invalid file header subtype */
       
         } /* End if read file header */
         else {
            
            CFE_EVS_SendEvent(EVT_PLBK_LOG_HDR_READ_ERR_EID, CFE_EVS_ERROR,
                              "Error reading event log %s file header. Return status = 0x%08X", EvtPlbk->EvsLogFilename, FileStatus);
         
         } /* End if file header read error */

         FileStatus = OS_close(FileHandle);
         
         if (!ReadingFile) {
            
            i--;
            EvtPlbk->EventLog.EventCnt = i;
            
            while (i < CFE_PLATFORM_EVS_LOG_MAX) {
               
               EvtPlbk->EventLog.Msg[i].Loaded = FALSE;
               TlmEvent = &EvtPlbk->EventLog.Msg[i].Tlm;
               TlmEvent->Time.Seconds    = 0;
               TlmEvent->Time.Subseconds = 0;
               TlmEvent->EventId   = 0;
               TlmEvent->EventType = 0;
               strncpy(TlmEvent->AppName, "UNDEF", CFE_MISSION_MAX_API_LEN);
               strncpy(TlmEvent->Message, "UNDEF", CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

               i++;
               
            } /* End filler loop */ 
         } /* End if !ReadingFile */ 
         else {
            EvtPlbk->EventLog.EventCnt = i;
         }
        
         EvtPlbk->EventLog.PlbkIdx = 0;  
         RetStatus = TRUE;

         /* Load telemetry that is fixed for each playback session */         
         strncpy(EvtPlbk->TlmMsg.EvsLogFilename, EvtPlbk->EvsLogFilename, CFE_MISSION_MAX_PATH_LEN);
         EvtPlbk->TlmMsg.EventCnt = EvtPlbk->EventLog.EventCnt;

         CFE_EVS_SendEvent(EVT_PLBK_READ_LOG_SUCCESS_EID, CFE_EVS_INFORMATION, "Successfully loaded %d event messages from %s",
                           EvtPlbk->EventLog.EventCnt, EvtPlbk->EvsLogFilename);
         
      } /* End if open file */
      else {
         
         CFE_EVS_SendEvent(EVT_PLBK_LOG_OPEN_ERR_EID, CFE_EVS_ERROR,
                           "Open event log file %s failed. Return status = 0x%08X", EvtPlbk->EvsLogFilename, FileHandle);
         
      } /* End if open failed */
   } /* End if file exists */
   else {
   
      CFE_EVS_SendEvent(EVT_PLBK_LOG_NONEXISTENT_EID, CFE_EVS_ERROR, "Event log file %s doesn't exist", EvtPlbk->EvsLogFilename);
   
   } /* End if log file non-existent */
   
   return RetStatus;
   
} /* End LoadLogFile() */


/******************************************************************************
** Function: SendEventTlmMsg()
**
** Notes:
**   1. The log filename and event count are loaded once when the playback is
**      started.
*/
static void SendEventTlmMsg(void)
{

   uint16 i;
   EVT_PLBK_TlmEvent*  LogEvent;
   EVT_PLBK_TlmEvent*  TlmEvent;
   
   for (i=0; i < EVT_PLBK_EVENTS_PER_TLM_MSG; i++) {
   
      if (EvtPlbk->EventLog.PlbkIdx >= CFE_PLATFORM_EVS_LOG_MAX) EvtPlbk->EventLog.PlbkIdx = 0;
      if (i==0) EvtPlbk->TlmMsg.PlbkIdx = EvtPlbk->EventLog.PlbkIdx;
      
      LogEvent = &EvtPlbk->EventLog.Msg[EvtPlbk->EventLog.PlbkIdx].Tlm;
      TlmEvent = &EvtPlbk->TlmMsg.Event[i];
      
      TlmEvent->Time      = LogEvent->Time;
      TlmEvent->EventId   = LogEvent->EventId;
      TlmEvent->EventType = LogEvent->EventType;
      strncpy(TlmEvent->AppName, LogEvent->AppName, CFE_MISSION_MAX_API_LEN);
      strncpy(TlmEvent->Message, LogEvent->Message, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
                  
      EvtPlbk->EventLog.PlbkIdx++;
   
   }
   
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &(EvtPlbk->TlmMsg));
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &(EvtPlbk->TlmMsg));

} /* End SendEventTlmMsg() */
