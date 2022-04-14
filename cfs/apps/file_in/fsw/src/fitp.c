/* 
** Purpose: Implement the "File Input Transfer Protocol" (FITP).
**
** Notes
**   1. See fitp.h file prologue for protocol overview and functions
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

#include "fitp.h"


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static void DestructorCallback(void);


/**********************/
/** Global File Data **/
/**********************/

static FITP_Class_t* Fitp = NULL;


/******************************************************************************
** Function: FITP_Constructor
**
*/
void FITP_Constructor(FITP_Class_t*  FitpPtr)
{

   Fitp = FitpPtr;

   /* Assumes some default/undefined states are 0 */
   
   CFE_PSP_MemSet((void*)Fitp, 0, sizeof(FITP_Class_t));
   
   FITP_ResetStatus();

   OS_TaskInstallDeleteHandler(DestructorCallback); /* Call when application terminates */

} /* End FITP_Constructor() */


/******************************************************************************
** Function:  FITP_ResetStatus
**
*/
void FITP_ResetStatus(void)
{

   if (Fitp->FileTransferActive == FALSE)
   {

      Fitp->FileTransferCnt     = 0;
      Fitp->LastDataSegmentId   = FITP_DATA_SEG_ID_NULL;
      Fitp->DataSegmentErrCnt   = 0;
      Fitp->FileTransferByteCnt = 0;
      Fitp->FileRunningCrc      = 0;
      strcpy(Fitp->DestFilename, "Undefined");
      
   } /* End if not FileTransferActive */
   
} /* End FITP_ResetStatus() */


/******************************************************************************
** Function: FITP_StartTransferCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
boolean FITP_StartTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FITP_StartTransferCmdMsg_t* StartTransferCmd = (FITP_StartTransferCmdMsg_t *) MsgPtr;
   boolean RetStatus = FALSE;
   
   if (Fitp->FileTransferActive)
   {
      
      CFE_EVS_SendEvent(FITP_START_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                        "Start transfer command rejected: %s transfer in progress",
                        Fitp->DestFilename);
   }
   else
   {
         
      if (FileUtil_VerifyFilenameStr(StartTransferCmd->DestFilename))
      {
         
         Fitp->FileHandle = OS_creat(StartTransferCmd->DestFilename, OS_WRITE_ONLY);
         
         if (Fitp->FileHandle >= 0)
         {      
         
            strncpy(Fitp->DestFilename, StartTransferCmd->DestFilename, FITP_FILENAME_LEN);      
            Fitp->LastDataSegmentId   = FITP_DATA_SEG_ID_NULL;
            Fitp->DataSegmentErrCnt   = 0;
            Fitp->FileTransferByteCnt = 0;
            Fitp->FileRunningCrc      = 0;
            Fitp->FileTransferActive  = TRUE;

            RetStatus = TRUE;
            CFE_EVS_SendEvent(FITP_START_TRANSFER_CMD_EID, CFE_EVS_INFORMATION, 
                              "Start file transfer command accepted for %s",
                              Fitp->DestFilename);
            
         }
         else
         {
         
            CFE_EVS_SendEvent(FITP_START_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                              "Start transfer command rejected: Open %s failed, status = 0x%08X",
                              Fitp->DestFilename, Fitp->FileHandle);
                              
         }
      }
      else
      {
         
         CFE_EVS_SendEvent(FITP_START_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                           "Start transfer command rejected: Invalid filename %s",
                           Fitp->DestFilename);

      }
      
   }
   
   return RetStatus;
   
} /* FITP_StartTransferCmd() */


/******************************************************************************
** Function: FITP_DataSegmentCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
**   2. Event messages are issued for error cases. The app should configure
**      filters for these events so they don't flood telemetry. It's helpful
**      to get one message that contains information about the error situation.
**   3. LastDataSegmentId, FileTransferByteCnt, and FileRunningCrc are based on
**      successful file writes. If a file write fails then the stats are not
**      updated.
*/
boolean FITP_DataSegmentCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FITP_DataSegmentCmdMsg_t* DataSegmentCmd = (FITP_DataSegmentCmdMsg_t *) MsgPtr;
   boolean RetStatus = FALSE;
   int32 BytesWritten;

   if (Fitp->FileTransferActive == TRUE)
   {
      
      if (DataSegmentCmd->Len <= FITP_DATA_SEG_MAX_LEN)
      {
         
         if (DataSegmentCmd->Id == (Fitp->LastDataSegmentId+1))
         {
            
            BytesWritten = OS_write(Fitp->FileHandle, (void *)DataSegmentCmd->Data, DataSegmentCmd->Len);   
  
            if (BytesWritten == DataSegmentCmd->Len) 
            {
            
               Fitp->LastDataSegmentId++;
               Fitp->FileTransferByteCnt += DataSegmentCmd->Len;
               Fitp->FileRunningCrc = CFE_ES_CalculateCRC(DataSegmentCmd->Data, DataSegmentCmd->Len, Fitp->FileRunningCrc, CFE_ES_DEFAULT_CRC);
              
               RetStatus = TRUE;
               CFE_EVS_SendEvent(FITP_DATA_SEGMENT_CMD_EID, CFE_EVS_DEBUG,
                                 "Data segment command passed: ID = %d, SegLen = %d, File Bytes = %d, File CRC = 0x%04x",
                                 Fitp->LastDataSegmentId, DataSegmentCmd->Len, Fitp->FileTransferByteCnt, Fitp->FileRunningCrc);
            }
            else
            {
               
               OS_close(Fitp->FileHandle);
               Fitp->FileTransferActive = FALSE;
                   
               CFE_EVS_SendEvent(FITP_DATA_SEGMENT_CMD_ERR_EID, CFE_EVS_ERROR, 
                                 "Data segment command failed: Error writing data to file %s. Attempted %d bytes, wrote %d",
                                 Fitp->DestFilename, DataSegmentCmd->Len, BytesWritten);
            }
            
         
         } /* End if valid segment ID */
         else
         {
            Fitp->DataSegmentErrCnt++;
            CFE_EVS_SendEvent(FITP_DATA_SEGMENT_CMD_ERR_EID, CFE_EVS_ERROR, 
                              "Data segment command rejected: Invalid data segment ID %d received. Expected ID %d",
                              DataSegmentCmd->Id, (Fitp->LastDataSegmentId+1));

         } /* End if invalid segment ID */
         
      } /* End if valid data segment length */
      else 
      {
      
         Fitp->DataSegmentErrCnt++;
         CFE_EVS_SendEvent(FITP_DATA_SEGMENT_CMD_ERR_EID, CFE_EVS_ERROR, 
                           "Data segment command rejected: Invalid data segment length of %d greater than maximum length %d",
                           DataSegmentCmd->Len,  FITP_DATA_SEG_MAX_LEN);
      
      } /* End if invalid data segment length */
      
   } /* End if FileTransferActive */
   else
   {
      
      CFE_EVS_SendEvent(FITP_DATA_SEGMENT_CMD_ERR_EID, CFE_EVS_ERROR, 
                        "Data segment command rejected: No file transfer currently active");

   } /* End if not FileTransferActive */

   return RetStatus;
   
} /* FITP_DataSegmentCmd() */


/******************************************************************************
** Function: FITP_FinishTransferCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
**   2. Always close the file regardless of whether the onboard file data
**      matches the commanded length and CRC values. If a data segment error
**      occurs during a transfer then the finish command shouldn't be sent if
**      the sender is verifying data segments and plans to resend failed 
**      segments. 
**   3. Fitp->FileTransferCnt is only incremented if the commanded (expected)
**      file length, CRC, and sgement IDs are verified. If the expected values
**      are not correct then there's a chance the file was transferred.
*/
boolean FITP_FinishTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
 
   FITP_FinishTransferCmdMsg_t* FinishTransferCmd = (FITP_FinishTransferCmdMsg_t *) MsgPtr;
   boolean RetStatus = FALSE;
   uint16  ValidityFailures = 0;

   if (Fitp->FileTransferActive == TRUE)
   {
      
      OS_close(Fitp->FileHandle);
      Fitp->FileTransferActive = FALSE;
      
      if (FinishTransferCmd->FileCrc != Fitp->FileRunningCrc)
      {
         ValidityFailures++;
         CFE_EVS_SendEvent(FITP_FINISH_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                           "Finish file transfer command error: Commanded CRC 0x%08X not equal to onboard computed CRC 0x%08X",
                           FinishTransferCmd->FileCrc, Fitp->FileRunningCrc);
      }

      if (FinishTransferCmd->FileLen != Fitp->FileTransferByteCnt)
      {
         ValidityFailures++;
         CFE_EVS_SendEvent(FITP_FINISH_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                           "Finish file transfer command error: Commanded file length %d not equal to onboard file length %d",
                           FinishTransferCmd->FileLen, Fitp->FileTransferByteCnt);
      }

      if (FinishTransferCmd->LastDataSegmentId != Fitp->LastDataSegmentId)
      {
         ValidityFailures++;
         CFE_EVS_SendEvent(FITP_FINISH_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                           "Finish file transfer command error: Commanded last segment ID %d not equal to onboard last segment ID %d",
                           FinishTransferCmd->LastDataSegmentId, Fitp->LastDataSegmentId);       
      }

      if (ValidityFailures == 0)
      {
      
         Fitp->FileTransferCnt++; 
         RetStatus = TRUE;
         CFE_EVS_SendEvent(FITP_FINISH_TRANSFER_CMD_EID, CFE_EVS_INFORMATION, 
                           "Finish file transfer command successful for %s",
                           Fitp->DestFilename);
      }
            
   } /* End if FileTransferActive */
   else
   {
      
      CFE_EVS_SendEvent(FITP_FINISH_TRANSFER_CMD_ERR_EID, CFE_EVS_ERROR, 
                        "Finish file transfer command rejected: No file transfer currently active");

   } /* End if not FileTransferActive */

   return RetStatus;
   
} /* FITP_FinishTransferCmd() */


/******************************************************************************
** Function: FITP_CancelTransferCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
**   2. Receiving a cancel command when no transfer is in progress is not
**      considered an error because this command may be sent in the blind 
*/
boolean FITP_CancelTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   boolean RetStatus = TRUE;
   
   if (Fitp->FileTransferActive == TRUE)
   {
   
      Fitp->FileTransferActive = FALSE;
      OS_close(Fitp->FileHandle);
   
      CFE_EVS_SendEvent(FITP_CANCEL_TRANSFER_CMD_EID, CFE_EVS_INFORMATION, 
                        "Cancel file transfer command terminated transfer for %s",
                        Fitp->DestFilename);

   } /* End if FileTransferActive */
   else
   {
      
      CFE_EVS_SendEvent(FITP_CANCEL_TRANSFER_CMD_EID, CFE_EVS_INFORMATION, 
                        "Cancel file transfer command received when no file transfer is currently active");

   } /* End if not FileTransferActive */
 
   FITP_ResetStatus();
    
   return RetStatus;

} /* FITP_CancelTransferCmd() */


/******************************************************************************
** Function: DestructorCallback
**
** This function is called when the app is terminated. This should
** never occur but if it does this will close an open file. 
*/
static void DestructorCallback(void)
{
 
   if (Fitp->FileTransferActive == TRUE)
   {
      OS_close(Fitp->FileHandle);
   }
   
} /* End DestructorCallback() */


