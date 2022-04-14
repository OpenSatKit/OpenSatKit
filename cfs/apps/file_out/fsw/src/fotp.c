/* 
** Purpose: Implement the "File Outut Transfer Protocol" (FOTP).
**
** Notes
**   1. See fotp.h file prologue for protocol overview and functions
**      below for protocol details.
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide and the
**      osk_c_demo app that illustrates best practices with comments.  
**   2. cFS Application Developer's Guide.
**
**   Written by David McComas, licensed under the Apache License, Version 2.0
**   (the "License"); you may not use this file except in compliance with the
**   License. You may obtain a copy of the License at
**
**      http://www.apache.org/licenses/LICENSE-2.0
**
**   Unless required by applicable law or agreed to in writing, software
**   distributed under the License is distributed on an "AS IS" BASIS,
**   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**   See the License for the specific language governing permissions and
**   limitations under the License.
*/

/*
** Include Files:
*/

#include "fotp.h"

typedef enum
{

  SEND_DATA_SEGMENT_ACTIVE = 1,
  SEND_DATA_SEGMENT_FINISHED,
  SEND_DATA_SEGMENT_ABORTED

} SendDataSegmentState_t;

/*******************************/
/** Local Function Prototypes **/
/*******************************/

static void DestructorCallback(void);
static boolean SendFileTransferTlm(FOTP_FileTransferState_t FileTransferState);
static SendDataSegmentState_t SendDataSegments(void);
const char* FileTransferStateStr(FOTP_FileTransferState_t  FileTransferState);


/**********************/
/** Global File Data **/
/**********************/

static FOTP_Class_t* Fotp = NULL;


/******************************************************************************
** Function: FOTP_Constructor
**
*/
void FOTP_Constructor(FOTP_Class_t* FotpPtr, INITBL_Class* IniTbl)
{

   Fotp = FotpPtr;

   /* All booleans and counters set to zero. States have non-zero defaults and must be explicitly set */ 
   CFE_PSP_MemSet((void*)Fotp, 0, sizeof(FOTP_Class_t));
  
   Fotp->IniTbl = IniTbl;
   Fotp->FileTransferState = FOTP_IDLE;
   Fotp->PausedFileTransferState = FOTP_IDLE;
   
   FOTP_ResetStatus();

   CFE_SB_InitMsg(&(Fotp->StartTransferPkt), 
                  CFE_SB_ValueToMsgId(INITBL_GetIntConfig(Fotp->IniTbl, CFG_FOTP_START_TRANSFER_TLM_MID)),
                  FOTP_START_TRANSFER_TLM_LEN, TRUE);                       

   CFE_SB_InitMsg(&(Fotp->DataSegmentPkt), 
                 CFE_SB_ValueToMsgId(INITBL_GetIntConfig(Fotp->IniTbl, CFG_FOTP_DATA_SEGMENT_TLM_MID)),
                 FOTP_DATA_SEGMENT_TLM_LEN, TRUE);

   CFE_SB_InitMsg(&(Fotp->FinishTransferPkt),
                  CFE_SB_ValueToMsgId(INITBL_GetIntConfig(Fotp->IniTbl, CFG_FOTP_FINISH_TRANSFER_TLM_MID)),
                  FOTP_FINISH_TRANSFER_TLM_LEN, TRUE);

   OS_TaskInstallDeleteHandler(DestructorCallback); /* Called when application terminates */

} /* End FOTP_Constructor() */


/******************************************************************************
** Function:  FOTP_ResetStatus
**
*/
void FOTP_ResetStatus(void)
{

   if (Fotp->FileTransferState == FOTP_IDLE)
   {

      Fotp->FileTransferByteCnt = 0;
      Fotp->DataTransferLen     = 0;
      Fotp->DataSegmentLen      = 0;
      Fotp->DataSegmentOffset   = 0;
      Fotp->FileLen             = 0;
      Fotp->FileByteOffset      = 0;
      Fotp->FileRunningCrc      = 0;
      Fotp->NextDataSegmentId   = FOTP_DATA_SEGMENT_ID_START;
      Fotp->FileTransferCnt     = 0;
      Fotp->LastDataSegment     = FALSE;
      Fotp->PrevSendDataSegmentFailed = 0;
      Fotp->PausedFileTransferState   = FOTP_IDLE;

   } /* End if not idle */
      
} /* End FOTP_ResetStatus() */


/******************************************************************************
** Function: FOTP_StartTransferCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
**   2. All command parameters are validated prior to updating and FOTP state
**      data.
*/
boolean FOTP_StartTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FOTP_StartTransferCmdMsg_t* StartTransferCmd = (FOTP_StartTransferCmdMsg_t *) MsgPtr;
   FileUtil_FileInfo  FileInfo;
   uint16  i;
   uint32  FileByteOffset=0;
   uint16  DataSegmentReadLen;
   uint8   DataSegment[FOTP_DATA_SEG_MAX_LEN];
   boolean ValidCmdParams = FALSE;
   boolean RetStatus = FALSE;
   

   if (Fotp->FileTransferState == FOTP_IDLE)
   {
            
      /* FileUtil_GetFileInfo() validates the filename */
      FileInfo = FileUtil_GetFileInfo(StartTransferCmd->SrcFilename,FOTP_FILENAME_LEN, TRUE);
      if (FILEUTIL_FILE_EXISTS(FileInfo.State) && FileInfo.State == FILEUTIL_FILE_CLOSED)
      {
         
         if ((StartTransferCmd->DataSegLen >= FOTP_DATA_SEG_MIN_LEN) &&
             (StartTransferCmd->DataSegLen <= FOTP_DATA_SEG_MAX_LEN) )
         {
            FileByteOffset = StartTransferCmd->DataSegLen * StartTransferCmd->DataSegOffset;
            if (FileByteOffset < FileInfo.Size)
            {
               ValidCmdParams = TRUE;   
            }
            else
            {
               CFE_EVS_SendEvent(FOTP_START_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                                 "Start transfer command rejected: File byte offset %d (seg len*offset %d*%d) not less than file length %d.",
                                 StartTransferCmd->DataSegLen, StartTransferCmd->DataSegOffset,
                                 FileByteOffset, FileInfo.Size);
            }
         } /* End if valid DataSegLen */
         else
         {
            CFE_EVS_SendEvent(FOTP_START_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                              "Start transfer command rejected: Invalid data segement length %d. It must be between %d and %d",
                              StartTransferCmd->DataSegLen, FOTP_DATA_SEG_MIN_LEN, FOTP_DATA_SEG_MAX_LEN);
         }
         
         if (ValidCmdParams)
         {

            Fotp->FileHandle = OS_open(StartTransferCmd->SrcFilename, OS_READ_ONLY, 0);
            
            if (Fotp->FileHandle >= 0)
            {      
            
               strncpy(Fotp->SrcFilename, StartTransferCmd->SrcFilename, FOTP_FILENAME_LEN);            
   
               Fotp->DataTransferLen     = FileInfo.Size - FileByteOffset;
               Fotp->DataSegmentLen      = StartTransferCmd->DataSegLen;
               Fotp->DataSegmentOffset   = StartTransferCmd->DataSegOffset;
               Fotp->FileByteOffset      = FileByteOffset;
               Fotp->FileLen             = FileInfo.Size;
               Fotp->NextDataSegmentId   = FOTP_DATA_SEGMENT_ID_START;
               Fotp->FileTransferByteCnt = 0;
               Fotp->FileRunningCrc      = 0;
               Fotp->FileTransferState   = FOTP_IDLE;
               Fotp->LastDataSegment     = FALSE;
               Fotp->PrevSendDataSegmentFailed = FALSE;
               
               for (i=0; i < StartTransferCmd->DataSegOffset; i++)
               {
                  DataSegmentReadLen = OS_read(Fotp->FileHandle, DataSegment, Fotp->DataSegmentLen);
                  if (DataSegmentReadLen == Fotp->DataSegmentLen)
                  {
                     Fotp->FileTransferByteCnt += DataSegmentReadLen;
                     Fotp->FileRunningCrc = CFE_ES_CalculateCRC(DataSegment, DataSegmentReadLen, Fotp->FileRunningCrc, CFE_ES_DEFAULT_CRC);
                  }
                  else
                  {
                     CFE_EVS_SendEvent(FOTP_START_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                                       "Start transfer command rejected: Error advancing to file offset at segment %d. Read %d bytes, expected %d",
                                       i, DataSegmentReadLen, Fotp->DataSegmentLen);
                     break;
                  }                  
               }
OS_printf("i=%d, StartTransferCmd->DataSegOffset=%d\n",i,StartTransferCmd->DataSegOffset);               
               if (i == StartTransferCmd->DataSegOffset)
               {
                  Fotp->FileTransferState = FOTP_START;
                  RetStatus = TRUE;
                  CFE_EVS_SendEvent(FOTP_START_TRANSFER_CMD_EID, CFE_EVS_INFORMATION, 
                                    "Start file transfer command accepted for %s, Segment length %d and offset %d",
                                    Fotp->SrcFilename, StartTransferCmd->DataSegLen, StartTransferCmd->DataSegOffset);
               }
               else
               {
                  OS_close(Fotp->FileHandle);                
               }
         
            } /* End if file opened */
            else
            {
            
               CFE_EVS_SendEvent(FOTP_START_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                                 "Start transfer command rejected: Open %s failed, status = 0x%08X",
                                 Fotp->SrcFilename, Fotp->FileHandle);
                                 
            }
            
         } /* End if valid command parameters */
         
      } /* End if file exists and closed */
      else
      {
         
         CFE_EVS_SendEvent(FOTP_START_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                           "Start transfer command rejected: %s is either an invalid filename or the file is open",
                           Fotp->SrcFilename);

      }
      
   }/* End if idle */
   else
   {
      CFE_EVS_SendEvent(FOTP_START_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                        "Start transfer command rejected: %s transfer in progress",
                        Fotp->SrcFilename);
   }

   return RetStatus;
   
} /* FOTP_StartTransferCmd() */


/******************************************************************************
** Function: FOTP_Execute
**
** Notes:
**   1. After initialization, Fotp->FileTransferState is only set in the this
**      function and the FOTP_xxxTransferCmd() functions.
*/
void FOTP_Execute(void)
{
   SendDataSegmentState_t SendDataSegmentState;
   
   CFE_EVS_SendEvent(FOTP_EXECUTE_EID, CFE_EVS_DEBUG,
                     "Executing state %s",
                     FileTransferStateStr(Fotp->FileTransferState));
                     
   switch (Fotp->FileTransferState)
   {   
        
      case FOTP_START:
         if (SendFileTransferTlm(FOTP_START))
         {
            Fotp->FileTransferState = FOTP_SEND_DATA;
         }
         break;
         
      case FOTP_SEND_DATA:
         SendDataSegmentState = SendDataSegments();
         if (SendDataSegmentState == SEND_DATA_SEGMENT_FINISHED)
         {
            Fotp->FileTransferState = FOTP_FINISH;
         }
         else if (SendDataSegmentState == SEND_DATA_SEGMENT_ABORTED)
         {
            Fotp->FileTransferState = FOTP_IDLE;
         }
         break;
         
      case FOTP_FINISH:
         if (SendFileTransferTlm(FOTP_FINISH))
         {
            Fotp->FileTransferCnt++;
            Fotp->FileTransferState = FOTP_IDLE;
            CFE_EVS_SendEvent(FOTP_EXECUTE_EID, CFE_EVS_INFORMATION,
                              "Completed %d byte file transfer of %s",
                              Fotp->FileTransferByteCnt,
                              Fotp->SrcFilename);
         }
         break;
         
      default:
         break;   

   } /* End state switch */
   
} /* End FOTP_Execute() */
 
 
/******************************************************************************
** Function: FOTP_CancelTransferCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
**   2. Receiving a cancel command without a transfer in progress is not
**      considered an error. A cancel command may be sent in the blind.
*/
boolean FOTP_CancelTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = TRUE;
   
   if (Fotp->FileTransferState != FOTP_IDLE)
   {
      OS_close(Fotp->FileHandle);
      Fotp->FileTransferState = FOTP_IDLE;
      
      CFE_EVS_SendEvent(FOTP_CANCEL_TRANSFER_CMD_EID, CFE_EVS_INFORMATION,
                       "Cancelled file transfer for %s before sending segment %d",
                       Fotp->SrcFilename, Fotp->NextDataSegmentId);

   }
   else 
   {
      CFE_EVS_SendEvent(FOTP_CANCEL_TRANSFER_CMD_EID, CFE_EVS_INFORMATION,
                        "Cancel transfer received with no transfer in progress");
   }
   
   return RetStatus;

} /* End FOTP_CancelTransferCmd() */


/******************************************************************************
** Function: FOTP_PauseTransferCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
boolean FOTP_PauseTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   boolean RetStatus = FALSE;

   if (Fotp->FileTransferState == FOTP_IDLE)
   {
      CFE_EVS_SendEvent(FOTP_PAUSE_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR,
                        "Pause file transfer received while no transfer is in progress");
   }
   else
   {
      RetStatus = TRUE;
      Fotp->PausedFileTransferState = Fotp->FileTransferState;
      Fotp->FileTransferState       = FOTP_PAUSED;
      CFE_EVS_SendEvent(FOTP_PAUSE_TRANSFER_CMD_EID, CFE_EVS_INFORMATION,
                        "Paused file transfer in %s state with next data segment ID %d",
                        FileTransferStateStr(Fotp->PausedFileTransferState),
                        Fotp->NextDataSegmentId);
   }

   return RetStatus;

} /* End FOTP_PauseTransferCmd() */


/******************************************************************************
** Function: FOTP_ResumeTransferCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
boolean FOTP_ResumeTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   boolean RetStatus = FALSE;

   if (Fotp->FileTransferState == FOTP_IDLE)
   {
      CFE_EVS_SendEvent(FOTP_PAUSE_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR,
                        "Resume file transfer received while no transfer is in progress");
   }
   else
   {
      RetStatus = TRUE;
      Fotp->FileTransferState = Fotp->PausedFileTransferState;
      CFE_EVS_SendEvent(FOTP_PAUSE_TRANSFER_CMD_EID, CFE_EVS_INFORMATION,
                        "Resumed file transfer for %s with next data segment ID %d",
                         Fotp->SrcFilename, Fotp->NextDataSegmentId);
   }
   
   return RetStatus;

} /* End FOTP_ResumeTransferCmd() */


/******************************************************************************
** Function: DestructorCallback
**
** This function is called when the app is terminated. This should
** never occur but if it does this will close an open file. 
*/
static void DestructorCallback(void)
{
 
   if (Fotp->FileTransferState != FOTP_IDLE)
   {
      OS_close(Fotp->FileHandle);
   }
   
} /* End DestructorCallback() */


/******************************************************************************
** Function: SendDataSegments
**
** Notes:
**   1. Event messages are issued for error cases. The app should configure
**      filters for these events so they don't flood telemetry. It's helpful
**      to get one message that contains information about the error situation.
**   2. NextDataSegmentId, FileTransferByteCnt, and FileRunningCrc are based on
**      successful telemetry packet send. If the send fails then the stats are
**      not updated.
**   TODO - Replace DataSegmentsSent logic with a TO semaphore handshake
*/
static SendDataSegmentState_t SendDataSegments(void)
{
   
   uint16  DataSegmentsSent = 0;
   uint16  FileBytesRead;
   uint32  RemainingBytes;
   boolean ContinueSend = TRUE;
   boolean CloseFile = FALSE;
   SendDataSegmentState_t SendDataSegmentState = SEND_DATA_SEGMENT_ACTIVE;


   if (!Fotp->PrevSendDataSegmentFailed)
   {
      while (ContinueSend)
      {
         
         Fotp->DataSegmentPkt.Id = Fotp->NextDataSegmentId;
         
         RemainingBytes = Fotp->FileLen - Fotp->FileTransferByteCnt;
         if (RemainingBytes <= Fotp->DataSegmentLen)
         {
            Fotp->DataSegmentPkt.Len = RemainingBytes;
            Fotp->LastDataSegment = TRUE;
         }
         else
         {
            Fotp->DataSegmentPkt.Len = Fotp->DataSegmentLen;
         }
         
         memset(Fotp->DataSegmentPkt.Data, 0, FOTP_DATA_SEG_MAX_LEN);
         FileBytesRead = OS_read(Fotp->FileHandle, Fotp->DataSegmentPkt.Data, Fotp->DataSegmentPkt.Len);
         
         if (FileBytesRead == Fotp->DataSegmentPkt.Len)
         {
            Fotp->FileRunningCrc = CFE_ES_CalculateCRC(Fotp->DataSegmentPkt.Data, FileBytesRead, Fotp->FileRunningCrc, CFE_ES_DEFAULT_CRC);
            CFE_SB_SetTotalMsgLength ((CFE_SB_Msg_t *) &Fotp->DataSegmentPkt, 
                                      (FOTP_DATA_SEGMENT_NON_DATA_TLM_LEN + Fotp->DataSegmentPkt.Len));
          
            if (SendFileTransferTlm(FOTP_SEND_DATA))
            {
               Fotp->PrevSendDataSegmentFailed = FALSE;
               Fotp->NextDataSegmentId++;
               Fotp->FileTransferByteCnt += Fotp->DataSegmentPkt.Len;
               if (Fotp->LastDataSegment)
               {
                  CloseFile    = TRUE;
                  ContinueSend = FALSE;
                  SendDataSegmentState = SEND_DATA_SEGMENT_FINISHED;
               }
               else
               {
                  DataSegmentsSent++;
                  if (DataSegmentsSent >= 1) ContinueSend = FALSE; //TODO - Add flow control logic
               }
            }/* End if sent data segment telemetry */
            else
            {
               Fotp->PrevSendDataSegmentFailed = TRUE;
               ContinueSend = FALSE;
            }
            
         } /* End if successful file read */
         else
         {
           CloseFile = TRUE;
           SendDataSegmentState = SEND_DATA_SEGMENT_ABORTED;
           CFE_EVS_SendEvent(FOTP_SEND_DATA_SEGMENT_ERR_EID, CFE_EVS_ERROR, 
                             "File transfer aborted: Error reading data from file %s. Attempted %d bytes, read %d",
                             Fotp->SrcFilename, Fotp->DataSegmentPkt.Len, FileBytesRead);
         }
         
      } /* End while send DataSegment */
   } /* End if PrevSendDataSegmentFailed */
   else
   {
      /* 
      ** When in an error state, only attempt a single send. If SB sends are
      ** not working then there are probably much bigger issues. 
      ** This repeats some of the logic in the send loop above which is not
      ** desirable but the logic is short a function seems cumbersome at best.     
      */
      
      if (SendFileTransferTlm(FOTP_SEND_DATA))
      {
         
         Fotp->PrevSendDataSegmentFailed = FALSE;
         Fotp->NextDataSegmentId++;
         Fotp->FileTransferByteCnt += Fotp->DataSegmentPkt.Len;
         if (Fotp->LastDataSegment)
         {
            CloseFile = TRUE;
            SendDataSegmentState = SEND_DATA_SEGMENT_FINISHED;
         }

      }/* End if sent data egment telemetry */
      else
      {
         Fotp->PrevSendDataSegmentFailed = TRUE;
      }
      
   } /* End if previous data segment failed */
   
   if (CloseFile)
   {
      OS_close(Fotp->FileHandle);   
   }
   
   return SendDataSegmentState;
   
} /* SendDataSegments() */


/******************************************************************************
** Function: SendFileTransferTlm
**
** Notes:
**   1. Sends a telemetry packet unique to each file transfer state. The START
**      and FINISH telemetry data is loaded in this function. The SEND_DATA
**      telmeetry data is loaded by the calling function. 
**   2. This should only be called in a state that should send a message. If
**      no message is sent (wrong state or SB send failure) then it returns
**      FALSE.
**   3. Event FOTP_SEND_FILE_TRANSFER_ERR_EID should be filtered even though
**      it should never occur. 
*/
static boolean SendFileTransferTlm(FOTP_FileTransferState_t FileTransferState)
{
   
   CFE_SB_Msg_t*  SbMsg = NULL;
   int32   SbStatus;
   boolean RetStatus = FALSE;
   
   switch (Fotp->FileTransferState)
   {   
      case FOTP_START:
         Fotp->StartTransferPkt.DataLen = Fotp->DataTransferLen;
         strncpy(Fotp->StartTransferPkt.SrcFilename, Fotp->SrcFilename, FOTP_FILENAME_LEN);
         SbMsg = (CFE_SB_Msg_t *)&Fotp->StartTransferPkt;
         break;
         
      case FOTP_SEND_DATA:
         /* Segment ID is loaded prior to this call */
         SbMsg = (CFE_SB_Msg_t *)&Fotp->DataSegmentPkt;
         break;
         
      case FOTP_FINISH:
         Fotp->FinishTransferPkt.FileLen           = Fotp->FileLen;
         Fotp->FinishTransferPkt.FileCrc           = Fotp->FileRunningCrc;
         Fotp->FinishTransferPkt.LastDataSegmentId = Fotp->NextDataSegmentId-1;
         SbMsg = (CFE_SB_Msg_t *)&Fotp->FinishTransferPkt;
         break;
         
      default:
         break;   

   } /* End state switch */
   
   if (SbMsg != NULL)
   {
      CFE_SB_TimeStampMsg(SbMsg);
      SbStatus = CFE_SB_SendMsg(SbMsg);
      if (SbStatus == CFE_SUCCESS)
      {
         RetStatus = TRUE;
      }
      else
      {
         CFE_EVS_SendEvent(FOTP_SEND_FILE_TRANSFER_ERR_EID, CFE_EVS_ERROR,
                           "Error sending telemetry packet in the %s state",
                           FileTransferStateStr(Fotp->FileTransferState));
      }
   } /* End if have a message to send */
   
   return RetStatus;
   
} /* End SendFileTransferTlm() */


/******************************************************************************
** Function: FileTransferStateStr
**
** Type checking should enforce valid parameter but check just to be safe.
*/
const char* FileTransferStateStr(FOTP_FileTransferState_t  FileTransferState)
{

   static const char* TransferStateStr[] = {
      "Undefined", 
      "Idle",       /* FOTP_IDLE      */
      "Start",      /* FOTP_START     */
      "Send Data",  /* FOTP_SEND_DATA */
      "Finished",   /* FOTP_FINISH    */
      "Paused"      /*  FOTP_PAUSED   */
   };

   uint8 i = 0;
   
   if ( FileTransferState >= FOTP_IDLE &&
        FileTransferState <= FOTP_PAUSED)
   {
      i = FileTransferState;
   }
        
   return TransferStateStr[i];

} /* End FileTransferStateStr() */
