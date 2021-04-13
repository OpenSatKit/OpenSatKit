/*
** Purpose: Implement the FILE_Class methods
**
** Notes:
**   1. The sprintf() event message strategy uses buffers that are longer than 
**      CFE_MISSION_EVS_MAX_MESSAGE_LENGTH and relies on CFE_EVS_SendEvent() to
**      truncate long messages. TODO - Improve this.
**   2. When get FileUtil_GetFileInfo() is used to verify whether a target file
**      exists it does not verify the directroy path is valid so an operation
**      could still get an error when it tries to use the target path/file.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>

#include "app_cfg.h"
#include "initbl.h"
#include "childmgr.h"
#include "file.h"


/**********************/
/** Global File Data **/
/**********************/

static FILE_Class*  File = NULL;


/*
** Local Function Prototypes
*/

static boolean ConcatenateFiles(const char* SrcFile1, const char* SrcFile2, const char* TargetFile);
static boolean ComputeFileCrc(const char* CmdName, const char* Filename, uint32* Crc, uint8 CrcType);


/******************************************************************************
** Function: FILE_Constructor
**
*/
void FILE_Constructor(FILE_Class*  FilePtr, INITBL_Class* IniTbl)
{
 
   File = FilePtr;

   CFE_PSP_MemSet((void*)File, 0, sizeof(FILE_Class));
 
   File->IniTbl = IniTbl;
   
   CFE_SB_InitMsg(&File->InfoPkt, (CFE_SB_MsgId_t)INITBL_GetIntConfig(File->IniTbl, CFG_FILE_INFO_TLM_MID), 
                  sizeof(FILE_InfoPkt), TRUE);

} /* End FILE_Constructor */


/******************************************************************************
** Function:  FILE_ResetStatus
**
*/
void FILE_ResetStatus()
{
 
   File->CmdWarningCnt = 0;
   
} /* End FILE_ResetStatus() */


/******************************************************************************
** Function: FILE_ConcatenateCmd
**
** Notes:
**   1. FileUtil_GetFileInfo() verifies filename prior to checking state.
**
*/
boolean FILE_ConcatenateCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILE_ConcatenateCmdMsg* ConcatenateCmd = (FILE_ConcatenateCmdMsg *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   char    EventErrStr[256] = "\0";
   
   boolean RetStatus    = FALSE;
   boolean Source1Valid = FALSE;
   boolean Source2Valid = FALSE;
   boolean TargetValid  = FALSE;
   
   sprintf(EventErrStr,"Unhandled concatenate file error");
   
   /*
   ** Verify source files exist and are closed
   ** Verify target file does not exist
   */
   
   FileInfo = FileUtil_GetFileInfo(ConcatenateCmd->Source1Filename, OS_MAX_PATH_LEN, FALSE);
   if (FileInfo.State == FILEUTIL_FILE_CLOSED) {
      
      Source1Valid = TRUE;
   }
   else {
      sprintf(EventErrStr,"Concatenate file cmd error: Source file must be closed. Source file 1 %s state is %s",
              ConcatenateCmd->Source1Filename, FileUtil_FileStateStr(FileInfo.State));
   }
   
   FileInfo = FileUtil_GetFileInfo(ConcatenateCmd->Source2Filename, OS_MAX_PATH_LEN, FALSE);
   if (FileInfo.State == FILEUTIL_FILE_CLOSED) {
      Source2Valid = TRUE;
   }
   else {
      sprintf(EventErrStr,"Concatenate file cmd error: Source file must be closed. Source file 2 %s state is %s",
              ConcatenateCmd->Source2Filename, FileUtil_FileStateStr(FileInfo.State));
   }
  
   FileInfo = FileUtil_GetFileInfo(ConcatenateCmd->TargetFilename, OS_MAX_PATH_LEN, FALSE);
   if (FileInfo.State == FILEUTIL_FILE_NONEXISTENT) {
      TargetValid = TRUE;
   }
   else {
      sprintf(EventErrStr,"Concatenate file cmd error: Target file must not exist. Target file %s state is %s",
              ConcatenateCmd->TargetFilename, FileUtil_FileStateStr(FileInfo.State));
   }


   if (Source1Valid && Source2Valid && TargetValid) {
   
      RetStatus = ConcatenateFiles(ConcatenateCmd->Source1Filename,
                                   ConcatenateCmd->Source2Filename,
                                   ConcatenateCmd->TargetFilename);
      
      CFE_EVS_SendEvent(FILE_CONCATENATE_EID, CFE_EVS_DEBUG,
                        "Concatenated %s and %s to create %s. Success=%d",
                        ConcatenateCmd->Source1Filename,
                        ConcatenateCmd->Source2Filename,
                        ConcatenateCmd->TargetFilename, RetStatus);


   }
   else {
   
      CFE_EVS_SendEvent(FILE_CONCATENATE_ERR_EID, CFE_EVS_ERROR,"%s",EventErrStr);
      
   }
   

   return RetStatus;

} /* End of FILE_ConcatenateCmd() */


/******************************************************************************
** Function: FILE_CopyCmd
**
** Notes:
**   1. FileUtil_GetFileInfo() verifies filename prior to checking state.
**
*/
boolean FILE_CopyCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILE_CopyCmdMsg*   CopyCmd = (FILE_CopyCmdMsg *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32   SysStatus;   
   boolean PerformCopy = FALSE;
   boolean RetStatus   = FALSE;

   if (CMDMGR_ValidBoolArg(CopyCmd->Overwrite)) {
      
      FileInfo = FileUtil_GetFileInfo(CopyCmd->SourceFilename, OS_MAX_PATH_LEN, FALSE);
    
      if (FILEUTIL_FILE_EXISTS(FileInfo.State)) {
            
         FileInfo = FileUtil_GetFileInfo(CopyCmd->TargetFilename, OS_MAX_PATH_LEN, FALSE);
            
         if (CopyCmd->Overwrite == TRUE) {
            
            if (FileInfo.State == FILEUTIL_FILE_CLOSED) {
               
               PerformCopy = TRUE;
               
            }
            else {
            
               CFE_EVS_SendEvent(FILE_COPY_ERR_EID, CFE_EVS_ERROR,
                  "Copy file from %s to %s failed: Attempt to overwrite an open file",
                  CopyCmd->SourceFilename, CopyCmd->TargetFilename);
            }
         
         } /* End if overwrite true */
         else {
         
            if (FileInfo.State == FILEUTIL_FILE_NONEXISTENT) {
               
               PerformCopy = TRUE;
               
            }
            else {
            
               CFE_EVS_SendEvent(FILE_COPY_ERR_EID, CFE_EVS_ERROR,
                  "Copy file from %s to %s failed: Target file exists and overwrite is false",
                  CopyCmd->SourceFilename, CopyCmd->TargetFilename);
            }
         
         } /* End if overwrite false */
            
      } /* End if source file exists */
      else {
          
         CFE_EVS_SendEvent(FILE_COPY_ERR_EID, CFE_EVS_ERROR,
            "Copy file from %s to %s failed: Source file doesn't exist",
            CopyCmd->SourceFilename, CopyCmd->TargetFilename);
      
      } /* End if source file doesn't exists */
   } /* End if valid Overwrite arg */
   else {
   
      CFE_EVS_SendEvent(FILE_COPY_ERR_EID, CFE_EVS_ERROR,
         "Copy file from %s to %s failed: Invalid overwrite flag %d. Must be True(%d) or False(%d)",
         CopyCmd->SourceFilename, CopyCmd->TargetFilename, CopyCmd->Overwrite, TRUE, FALSE);
   
   } /* End if invalid Overwrite arg */
      
   
   if (PerformCopy) {
      
      SysStatus = OS_cp(CopyCmd->SourceFilename, CopyCmd->TargetFilename);

      if (SysStatus == OS_SUCCESS) {
      
         RetStatus = TRUE;      
         CFE_EVS_SendEvent(FILE_COPY_EID, CFE_EVS_DEBUG, "Copied file from %s to %s",
                           CopyCmd->SourceFilename, CopyCmd->TargetFilename);
      }
      else {
         
         CFE_EVS_SendEvent(FILE_COPY_ERR_EID, CFE_EVS_ERROR,
            "Copy file from %s to %s failed: Parameters validated but OS_cp() failed with status=%d",
            CopyCmd->SourceFilename, CopyCmd->TargetFilename, (int)SysStatus);
      }
      
   } /* End if Perform copy */
   
   return RetStatus;

} /* End of FILE_CopyCmd() */


/******************************************************************************
** Function: FILE_DecompressCmd
**
** Notes:
**    1. FileUtil_GetFileInfo() verifies filename prior to checking state
*/
boolean FILE_DecompressCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILE_DecompressCmdMsg* DecompressCmd = (FILE_DecompressCmdMsg *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32   CfeStatus;
   boolean RetStatus = FALSE;
   

   FileInfo = FileUtil_GetFileInfo(DecompressCmd->SourceFilename, OS_MAX_PATH_LEN, FALSE);
   
   if (FileInfo.State == FILEUTIL_FILE_CLOSED) {
   
      FileInfo = FileUtil_GetFileInfo(DecompressCmd->TargetFilename, OS_MAX_PATH_LEN, FALSE);
   
      if (FileInfo.State == FILEUTIL_FILE_NONEXISTENT) {
          
         CfeStatus = CFE_FS_Decompress(DecompressCmd->SourceFilename, DecompressCmd->TargetFilename);

         if (CfeStatus == CFE_SUCCESS) {
 
            RetStatus = TRUE;
            CFE_EVS_SendEvent(FILE_DECOMPRESS_EID, CFE_EVS_DEBUG,
                              "%s decompressed to %s",
                              DecompressCmd->SourceFilename, DecompressCmd->TargetFilename);
         }
         else {

            CFE_EVS_SendEvent(FILE_DECOMPRESS_ERR_EID, CFE_EVS_ERROR,
                              "Error decompressing %s to %s. CFE status = 0x%04X",
                              DecompressCmd->SourceFilename, DecompressCmd->TargetFilename, CfeStatus);
         }
         
      } /* End if target doesn't exist */ 
      else {
   
         CFE_EVS_SendEvent(FILE_DECOMPRESS_ERR_EID, CFE_EVS_ERROR,
                           "Decompress file cmd error: Target file %s already exists",
                           DecompressCmd->TargetFilename);
                           
      } /* End if target does exist */
   
   } /* End if source exists and is closed */
   else {
   
      CFE_EVS_SendEvent(FILE_DECOMPRESS_ERR_EID, CFE_EVS_ERROR,
                        "Decompress file cmd error: Source file must exist and be closed. Soucre file %s state is %s",
                        DecompressCmd->SourceFilename, FileUtil_FileStateStr(FileInfo.State) );
   
   } /* End if source file not an existing file that is closed */ 
    
   
   return RetStatus;

} /* End of FILE_DecompressCmd() */


/******************************************************************************
** Function: FILE_DeleteCmd
**
** Notes:
**   1. FileUtil_GetFileInfo() verifies filename prior to checking state.
**
*/
boolean FILE_DeleteCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILE_DeleteCmdMsg* DeleteCmd = (FILE_DeleteCmdMsg *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32   SysStatus;
   boolean RetStatus = FALSE;
   
   FileInfo = FileUtil_GetFileInfo(DeleteCmd->Filename, OS_MAX_PATH_LEN, FALSE);

   if (FileInfo.State == FILEUTIL_FILE_CLOSED) {
 
         SysStatus = OS_remove(DeleteCmd->Filename);

         if (SysStatus == OS_SUCCESS) {
            
            RetStatus = TRUE;
            CFE_EVS_SendEvent(FILE_DELETE_EID, CFE_EVS_DEBUG, "Deleted file %s", DeleteCmd->Filename);
            
         }
         else {
            
            CFE_EVS_SendEvent(FILE_DELETE_ERR_EID, CFE_EVS_ERROR,
               "Delete directory %s failed: Parameters validated but OS_remove() failed with status=%d",
               DeleteCmd->Filename, (int)SysStatus);

         }

   } /* End if file closed */
   else {
      
      CFE_EVS_SendEvent(FILE_DELETE_ERR_EID, CFE_EVS_ERROR,
         "Delete file command for %s rejected due to %s.",
         DeleteCmd->Filename, FileUtil_FileStateStr(FileInfo.State));
      
   }

   return RetStatus;

} /* End of FILE_DeleteCmd() */


/******************************************************************************
** Function: FILE_MoveCmd
**
** Notes:
**   1. FileUtil_GetFileInfo() verifies filename prior to checking state.
**
*/
boolean FILE_MoveCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILE_MoveCmdMsg*   MoveCmd = (FILE_MoveCmdMsg *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32   SysStatus;   
   boolean PerformMove = FALSE;
   boolean RetStatus   = FALSE;

   if (CMDMGR_ValidBoolArg(MoveCmd->Overwrite)) {
      
      FileInfo = FileUtil_GetFileInfo(MoveCmd->SourceFilename, OS_MAX_PATH_LEN, FALSE);
    
      if (FILEUTIL_FILE_EXISTS(FileInfo.State)) {
            
         FileInfo = FileUtil_GetFileInfo(MoveCmd->TargetFilename, OS_MAX_PATH_LEN, FALSE);
            
         if (MoveCmd->Overwrite == TRUE) {
            
            if (FileInfo.State == FILEUTIL_FILE_CLOSED) {
               
               PerformMove = TRUE;
               
            }
            else {
            
               CFE_EVS_SendEvent(FILE_MOVE_ERR_EID, CFE_EVS_ERROR,
                  "Move file from %s to %s failed: Attempt to overwrite an open file",
                  MoveCmd->SourceFilename, MoveCmd->TargetFilename);
            }
         
         } /* End if overwrite true */
         else {
         
            if (FileInfo.State == FILEUTIL_FILE_NONEXISTENT) {
               
               PerformMove = TRUE;
               
            }
            else {
            
               CFE_EVS_SendEvent(FILE_MOVE_ERR_EID, CFE_EVS_ERROR,
                  "Move file from %s to %s failed: Target file exists and overwrite is false",
                  MoveCmd->SourceFilename, MoveCmd->TargetFilename);
            }
         
         } /* End if overwrite false */
            
      } /* End if source file exists */
      else {
          
         CFE_EVS_SendEvent(FILE_MOVE_ERR_EID, CFE_EVS_ERROR,
            "Move file from %s to %s failed: Source file doesn't exist",
            MoveCmd->SourceFilename, MoveCmd->TargetFilename);

      } /* End if source file doesn't exists */
   } /* End if valid Overwrite arg */
   else {
   
      CFE_EVS_SendEvent(FILE_MOVE_ERR_EID, CFE_EVS_ERROR,
         "Move file from %s to %s failed: Invalid overwrite flag %d. Must be True(%d) or False(%d)",
         MoveCmd->SourceFilename, MoveCmd->TargetFilename, MoveCmd->Overwrite, TRUE, FALSE);
   
   } /* End if invalid Overwrite arg */
      
   
   if (PerformMove) {
      
      SysStatus = OS_mv(MoveCmd->SourceFilename, MoveCmd->TargetFilename);

      if (SysStatus == OS_SUCCESS) {
      
         RetStatus = TRUE;      
         CFE_EVS_SendEvent(FILE_MOVE_EID, CFE_EVS_DEBUG, "Move file from %s to %s",
                           MoveCmd->SourceFilename, MoveCmd->TargetFilename);
      }
      else {
         
         CFE_EVS_SendEvent(FILE_COPY_ERR_EID, CFE_EVS_ERROR,
            "Move file %s to %s failed: Parameters validated but OS_mv() failed with status=%d",
            MoveCmd->SourceFilename, MoveCmd->TargetFilename, (int)SysStatus);
      }
      
   } /* End if Perform move */
   
   return RetStatus;

} /* End of FILE_MoveCmd() */


/******************************************************************************
** Function: FILE_RenameCmd
**
** Notes:
**   1. FileUtil_GetFileInfo() verifies filename prior to checking state.
**
*/
boolean FILE_RenameCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILE_RenameCmdMsg* RenameCmd = (FILE_RenameCmdMsg *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32   SysStatus;   
   boolean RetStatus = FALSE;
   

   FileInfo = FileUtil_GetFileInfo(RenameCmd->SourceFilename, OS_MAX_PATH_LEN, FALSE);
   
   if (FILEUTIL_FILE_EXISTS(FileInfo.State)) {
   
      FileInfo = FileUtil_GetFileInfo(RenameCmd->TargetFilename, OS_MAX_PATH_LEN, FALSE);
   
      if (FileInfo.State == FILEUTIL_FILE_NONEXISTENT) {
      
         SysStatus = OS_rename(RenameCmd->SourceFilename, RenameCmd->TargetFilename);
        
         if (SysStatus == OS_SUCCESS) {
            
            RetStatus = TRUE;
            CFE_EVS_SendEvent(FILE_RENAME_EID, CFE_EVS_DEBUG, "Renamed file from %s to %s",
               RenameCmd->SourceFilename, RenameCmd->TargetFilename);
            
         }
         else {
            
            CFE_EVS_SendEvent(FILE_RENAME_ERR_EID, CFE_EVS_ERROR,
               "Renamed file %s to %s failed: Parameters validated but OS_rename() failed with status=%d",
               RenameCmd->SourceFilename, RenameCmd->TargetFilename, (int)SysStatus);

         }

      
      } /* End if target file doesn't exist */
      else {
       
         CFE_EVS_SendEvent(FILE_RENAME_ERR_EID, CFE_EVS_ERROR,
            "Renamed file %s to %s failed: Target file exists",
            RenameCmd->SourceFilename, RenameCmd->TargetFilename);
      
      }
   } /* End if source file exists */
   else {
    
      CFE_EVS_SendEvent(FILE_RENAME_ERR_EID, CFE_EVS_ERROR,
         "Renamed file %s to %s failed: Source file doesn't exist",
         RenameCmd->SourceFilename, RenameCmd->TargetFilename);
   
   }
   
   return RetStatus;

} /* End of FILE_RenameCmd() */


/******************************************************************************
** Function: FILE_SendInfoPktCmd
**
** Notes:
**   1. FileUtil_GetFileInfo() verifies filename prior to checking state.
**   2. Specifying an invalid CRC type or not being able to compute a valid
**      CRC are both considered errors as opposed to warnings so the command
**      fails and no information telemetry packet is sent. 
**
*/
boolean FILE_SendInfoPktCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILE_SendInfoPktCmdMsg* SendInfoPktCmd = (FILE_SendInfoPktCmdMsg *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   uint32  Crc;
   boolean RetStatus = FALSE;
   
   FileInfo = FileUtil_GetFileInfo(SendInfoPktCmd->Filename, OS_MAX_PATH_LEN, TRUE);
  
   if (FILEUTIL_FILE_EXISTS(FileInfo.State)) {

      strncpy(File->InfoPkt.Filename, SendInfoPktCmd->Filename, OS_MAX_PATH_LEN - 1);
      File->InfoPkt.Filename[OS_MAX_PATH_LEN - 1] = '\0';

      File->InfoPkt.State = FileInfo.State;
      File->InfoPkt.Size  = FileInfo.Size;
      File->InfoPkt.Time  = FileInfo.Time;
      File->InfoPkt.Mode  = FileInfo.Mode;
      File->InfoPkt.Crc   = 0;
      File->InfoPkt.CrcComputed = FALSE;
      
      if (SendInfoPktCmd->ComputeCrc == TRUE) {

         if (SendInfoPktCmd->CrcType == CFE_MISSION_ES_CRC_16) {
  
            if (ComputeFileCrc("Send File Info", SendInfoPktCmd->Filename,
                               &Crc, SendInfoPktCmd->CrcType)) {

               File->InfoPkt.Crc = Crc;
               File->InfoPkt.CrcComputed = TRUE;
               RetStatus = TRUE;
            
            }
         }
         else {
            
            CFE_EVS_SendEvent(FILE_SEND_INFO_ERR_EID, CFE_EVS_ERROR,
                              "Send file info failed: Invalid CRC type %d. See cFE ES for valid types.",
                              SendInfoPktCmd->CrcType);
            
         } /* End if invalid CrcType */
         
      } /* End if compute CRC */
      else {
         
         RetStatus = TRUE;
      
      } /* End if don't compute CRC */
      
      if (RetStatus == TRUE) {

         CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &File->InfoPkt);
         CFE_SB_SendMsg((CFE_SB_Msg_t *) &File->InfoPkt);
            
         CFE_EVS_SendEvent(FILE_SEND_INFO_EID, CFE_EVS_DEBUG,
                           "Sent info pkt for file %s with state %s",
                           SendInfoPktCmd->Filename, FileUtil_FileStateStr(FileInfo.State));

      }
   
   } /* End if file exists */
   else {
      
      CFE_EVS_SendEvent(FILE_SEND_INFO_ERR_EID, CFE_EVS_ERROR,
                        "Send file info failed: %s doesn't exist",
                        SendInfoPktCmd->Filename);
   
   } /* End if file doesn't exist */
  
  
   return RetStatus;

} /* FILE_SendInfoPktCmd() */


/******************************************************************************
** Function: FILE_SetPermissionsCmd
**
** Notes:
**   1. FileUtil_GetFileInfo() verifies filename prior to checking state.
**
*/
boolean FILE_SetPermissionsCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILE_SetPermissionsCmdMsg* SetPermissionsCmd = (FILE_SetPermissionsCmdMsg *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32   SysStatus;   
   boolean RetStatus = FALSE;
   
   
   if (FileUtil_VerifyFilenameStr(SetPermissionsCmd->Filename)) {
      
      FileInfo = FileUtil_GetFileInfo(SetPermissionsCmd->Filename, OS_MAX_PATH_LEN, FALSE);
      if (FILEUTIL_FILE_EXISTS(FileInfo.State)) {

         SysStatus = OS_chmod(SetPermissionsCmd->Filename, SetPermissionsCmd->Mode);

         if (SysStatus == OS_SUCCESS) {
            
            RetStatus = TRUE;
            CFE_EVS_SendEvent(FILE_SET_PERMISSIONS_EID, CFE_EVS_DEBUG,
               "Set permissions for file %s to octal %03o",
               SetPermissionsCmd->Filename, SetPermissionsCmd->Mode);
               
         }
         else {
               
            CFE_EVS_SendEvent(FILE_SET_PERMISSIONS_ERR_EID, CFE_EVS_ERROR,
               "Set file permissions failed: Parameters validated but OS_chmod() failed with status=%d",
               (int)SysStatus);
         }
         
      } /* End if file exists */
      else {
         
         CFE_EVS_SendEvent(FILE_SET_PERMISSIONS_ERR_EID, CFE_EVS_ERROR,
                           "Set file permissions failed: Nonexistent file %s",
                           SetPermissionsCmd->Filename);

      }

   } /* End if valid filename */
   else {

      CFE_EVS_SendEvent(FILE_SET_PERMISSIONS_ERR_EID, CFE_EVS_ERROR,
                        "Set file permissions failed: Invalid filename %s",
                        SetPermissionsCmd->Filename);
   
   } /* End if invalid filename */
   
   return RetStatus;

} /* End of FILE_SetPermissionsCmd() */


/******************************************************************************
** Function: ConcatenateFiles
**
*/
static boolean ConcatenateFiles(const char* SrcFile1, const char* SrcFile2, const char* TargetFile)
{
    
   int32  SysStatus;
   int32  SourceFileHandle;
   int32  TargetFileHandle;
   int32  BytesRead;
   int32  BytesWritten;
      
   char    EventErrStr[256] = "\0";
   uint16  TaskBlockCnt = 0;
   boolean PerformingCatenation = TRUE;
   boolean ConcatenatedFiles    = FALSE;
  
   sprintf(EventErrStr,"Unhandled concatenate file error");
  
   SysStatus = OS_cp(SrcFile1, TargetFile);
   if (SysStatus == OS_SUCCESS) {
   
      SourceFileHandle = OS_open(SrcFile2, OS_READ_ONLY, 0);
      if (SourceFileHandle >= 0) {
   
         TargetFileHandle = OS_open(TargetFile, OS_READ_WRITE, 0);
         if (TargetFileHandle >= 0) {
         
            OS_lseek(TargetFileHandle, 0, OS_SEEK_END);
        
            while (PerformingCatenation) {
               
               BytesRead = OS_read(SourceFileHandle, File->FileTaskBuf, FILEMGR_TASK_FILE_BLOCK_SIZE);
               if (BytesRead == 0) {
               
                  PerformingCatenation = FALSE;
                  ConcatenatedFiles    = TRUE;
               
               }
               else if (BytesRead < 0) {
                  
                  PerformingCatenation = FALSE;

                  sprintf(EventErrStr,"Concatenate file cmd error: File read error. OS_read status = %d", BytesRead);

               }
               else {
                  
                  BytesWritten = OS_write(TargetFileHandle, File->FileTaskBuf, BytesRead);

                  if (BytesWritten != BytesRead) {

                     PerformingCatenation = FALSE;
                     sprintf(EventErrStr,"Concatenate file cmd error: File read/write error. BytesRead %d  not equal to BytesWritten %d",
                             BytesRead, BytesWritten);
                  
                  }
           
                  CHILDMGR_PauseTask(&TaskBlockCnt, INITBL_GetIntConfig(File->IniTbl, CFG_TASK_FILE_BLOCK_CNT), 
                                     INITBL_GetIntConfig(File->IniTbl, CFG_TASK_FILE_BLOCK_DELAY),
                                     INITBL_GetIntConfig(File->IniTbl, CFG_CHILD_TASK_PERF_ID));
               
               }
               
            } /* End while performing concatenation */

            OS_close(TargetFileHandle);
            
         } /* End if opened target file */
         else {
                        
            sprintf(EventErrStr,"Concatenate file cmd error: Error opening target file %s. OS_open status %d", SrcFile2, TargetFileHandle);
            
         } /* End if  failed to open target file */
         
         OS_close(SourceFileHandle);
         
      } /* End if opened source file */
      else {
         
         sprintf(EventErrStr,"Concatenate file cmd error: Error opening source file %s. OS_open status %d", SrcFile2, SourceFileHandle);
         
      } /* End if  failed to open source file */
  
      if (ConcatenatedFiles == FALSE) OS_remove(TargetFile);  /* remove partial target file */
 
   } /* End if copied first source file to target */
   else {
      
      sprintf(EventErrStr,"Concatenate file cmd error: Error copying first source file to target file. OS_cp status %d", SysStatus);
      
   } /* End if failed to copy first source file to target */

   if (ConcatenatedFiles == FALSE) CFE_EVS_SendEvent(FILE_CONCATENATE_ERR_EID, CFE_EVS_ERROR,"%s",EventErrStr);
   
   return ConcatenatedFiles;

} /* End of ConcatenateFiles() */


/******************************************************************************
** Function: ComputeFileCrc
**
** Notes:
**   1. TaskBlockCnt is the count of "task blocks" performed. A task block is 
**      is group of instructions that is CPU intensive and may need to be 
**      periodically suspended to prevent CPU hogging.
**
*/

static boolean ComputeFileCrc(const char* CmdName, const char* Filename, uint32* Crc, uint8 CrcType)
{
   
   int32   FileHandle;
   int32   FileBytesRead;
   
   uint16  TaskBlockCnt = 0;   /* See prologue */
   uint32  CurrentCrc   = 0;
   boolean CrcComputed  = FALSE;
   boolean ComputingCrc = TRUE;
   

   *Crc = 0;
   FileHandle = OS_open(Filename, OS_READ_ONLY, 0);
   
   if (FileHandle >= 0) {
   
      while (ComputingCrc) {
         
         FileBytesRead = OS_read(FileHandle, File->FileTaskBuf, FILEMGR_TASK_FILE_BLOCK_SIZE);

         if (FileBytesRead == 0) {  /* Successfully finished reading file */
            
            ComputingCrc = FALSE;
            OS_close(FileHandle);

            *Crc = CurrentCrc;
            CrcComputed = TRUE;
           
         }
         else if (FileBytesRead < 0) {  /* Error reading file */
            
            ComputingCrc = FALSE;
            OS_close(FileHandle);
            
            CFE_EVS_SendEvent(FILE_COMPUTE_FILE_CRC_ERR_EID, CFE_EVS_ERROR,
                              "Concatenate file cmd error: File read error, OS_read status = %d", FileBytesRead);

         }
         else {
                
            CurrentCrc = CFE_ES_CalculateCRC(File->FileTaskBuf, FileBytesRead,
                                             CurrentCrc, CrcType);
         
            CHILDMGR_PauseTask(&TaskBlockCnt, INITBL_GetIntConfig(File->IniTbl, CFG_TASK_FILE_BLOCK_CNT),
                               INITBL_GetIntConfig(File->IniTbl, CFG_TASK_FILE_BLOCK_DELAY), 
                               INITBL_GetIntConfig(File->IniTbl, CFG_CHILD_TASK_PERF_ID));
         
         } /* End if still reading file */

      } /* End while computing CRC */
   
   }
   else {
      
      CFE_EVS_SendEvent(FILE_COMPUTE_FILE_CRC_ERR_EID, CFE_EVS_ERROR,
                        "%s failed: Error opening file %s, OS_open status %d", CmdName, Filename, FileHandle);
   
   } /* End if file open */
   
   return CrcComputed;
   
} /* End ComputeCrc() */


