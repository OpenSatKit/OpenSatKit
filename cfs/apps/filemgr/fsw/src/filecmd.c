/*
** Purpose: Process file commands for FileMgr
**
** Notes:
**   1. TBD
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
#include "filecmd.h"

/**********************/
/** Global File Data **/
/**********************/

static FILECMD_Class*  FileCmd = NULL;


/*
** Local Function Prototypes
*/


/******************************************************************************
** Function: FILECMD_Constructor
**
*/
void FILECMD_Constructor(FILECMD_Class*  FileCmdPtr)
{
 
   FileCmd = FileCmdPtr;

   CFE_PSP_MemSet((void*)FileCmd, 0, sizeof(FILECMD_Class));
 
} /* End FILECMD_Constructor */


/******************************************************************************
** Function:  FILECMD_ResetStatus
**
*/
void FILECMD_ResetStatus()
{
 
   FileCmd->WarnCmdCnt = 0;
   
} /* End FILECMD_ResetStatus() */


/******************************************************************************
** Function: FILECMD_Copy
**
*/
boolean FILECMD_Copy(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILECMD_Copy_t*   CopyCmd = (FILECMD_Copy_t *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32   OS_Status;   
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
            
               CFE_EVS_SendEvent(FILECMD_COPY_ERR_EID, CFE_EVS_ERROR,
                  "Copy file from %s to %s failed: Attempt to overwrite an open file",
                  CopyCmd->SourceFilename, CopyCmd->TargetFilename);
            }
         
         } /* End if overwrite true */
         else {
         
            if (FileInfo.State == FILEUTIL_FILE_NONEXISTENT) {
               
               PerformCopy = TRUE;
               
            }
            else {
            
               CFE_EVS_SendEvent(FILECMD_COPY_ERR_EID, CFE_EVS_ERROR,
                  "Copy file from %s to %s failed: Target file exists and overwrite is false",
                  CopyCmd->SourceFilename, CopyCmd->TargetFilename);
            }
         
         } /* End if overwrite false */
            
      } /* End if source file exists */
      else {
          
         CFE_EVS_SendEvent(FILECMD_COPY_ERR_EID, CFE_EVS_ERROR,
            "Copy file from %s to %s failed: Source file doesn't exist",
            CopyCmd->SourceFilename, CopyCmd->TargetFilename);
      
      } /* End if source file doesn't exists */
   } /* End if valid Overwrite arg */
   else {
   
      CFE_EVS_SendEvent(FILECMD_COPY_ERR_EID, CFE_EVS_ERROR,
         "Copy file from %s to %s failed: Invalid overwrite flag %d. Must be True(%d) or False(%d)",
         CopyCmd->SourceFilename, CopyCmd->TargetFilename, CopyCmd->Overwrite, TRUE, FALSE);
   
   } /* End if invalid Overwrite arg */
      
   
   if (PerformCopy) {
      
      OS_Status = OS_cp(CopyCmd->SourceFilename, CopyCmd->TargetFilename);

      if (OS_Status == OS_SUCCESS) {
      
         RetStatus = TRUE;      
         CFE_EVS_SendEvent(FILECMD_COPY_EID, CFE_EVS_DEBUG, "Copied file from %s to %s",
                           CopyCmd->SourceFilename, CopyCmd->TargetFilename);
      }
      else {
         
         CFE_EVS_SendEvent(FILECMD_COPY_ERR_EID, CFE_EVS_ERROR,
            "Copy file from %s to %s failed: Parameters validated but OS_cp() failed with status=%d",
            CopyCmd->SourceFilename, CopyCmd->TargetFilename, (int)OS_Status);
      }
      
   } /* End if Perform copy */
   
   return RetStatus;

} /* End of FILECMD_Copy() */


/******************************************************************************
** Function: FILECMD_Delete
**
*/
boolean FILECMD_Delete(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILECMD_Delete_t* DeleteCmd = (FILECMD_Delete_t *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32   OS_Status;   
   boolean RetStatus = FALSE;
   
   FileInfo = FileUtil_GetFileInfo(DeleteCmd->Filename, OS_MAX_PATH_LEN, FALSE);

   if (FileInfo.State == FILEUTIL_FILE_CLOSED) {
 
         OS_Status = OS_remove(DeleteCmd->Filename);

         if (OS_Status == OS_SUCCESS) {
            
            RetStatus = TRUE;
            CFE_EVS_SendEvent(FILECMD_DELETE_EID, CFE_EVS_DEBUG, "Deleted file %s", DeleteCmd->Filename);
            
         }
         else {
            
            CFE_EVS_SendEvent(FILECMD_DELETE_ERR_EID, CFE_EVS_ERROR,
               "Delete directory %s failed: Parameters validated but OS_remove() failed with status=%d",
               DeleteCmd->Filename, (int)OS_Status);

         }

   } /* End if file closed */
   else {
      
      CFE_EVS_SendEvent(FILECMD_DELETE_ERR_EID, CFE_EVS_ERROR,
         "Delete file command for %s rejected due to %s.",
         DeleteCmd->Filename, FileUtil_FileStateStr(FileInfo.State));
      
   }

    return RetStatus;

} /* End of FILECMD_Delete() */


/******************************************************************************
** Function: FILECMD_Move
**
*/
boolean FILECMD_Move(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILECMD_Move_t*   MoveCmd = (FILECMD_Move_t *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32   OS_Status;   
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
            
               CFE_EVS_SendEvent(FILECMD_MOVE_ERR_EID, CFE_EVS_ERROR,
                  "Move file from %s to %s failed: Attempt to overwrite an open file",
                  MoveCmd->SourceFilename, MoveCmd->TargetFilename);
            }
         
         } /* End if overwrite true */
         else {
         
            if (FileInfo.State == FILEUTIL_FILE_NONEXISTENT) {
               
               PerformMove = TRUE;
               
            }
            else {
            
               CFE_EVS_SendEvent(FILECMD_MOVE_ERR_EID, CFE_EVS_ERROR,
                  "Move file from %s to %s failed: Target file exists and overwrite is false",
                  MoveCmd->SourceFilename, MoveCmd->TargetFilename);
            }
         
         } /* End if overwrite false */
            
      } /* End if source file exists */
      else {
          
         CFE_EVS_SendEvent(FILECMD_MOVE_ERR_EID, CFE_EVS_ERROR,
            "Move file from %s to %s failed: Source file doesn't exist",
            MoveCmd->SourceFilename, MoveCmd->TargetFilename);

      } /* End if source file doesn't exists */
   } /* End if valid Overwrite arg */
   else {
   
      CFE_EVS_SendEvent(FILECMD_MOVE_ERR_EID, CFE_EVS_ERROR,
         "Move file from %s to %s failed: Invalid overwrite flag %d. Must be True(%d) or False(%d)",
         MoveCmd->SourceFilename, MoveCmd->TargetFilename, MoveCmd->Overwrite, TRUE, FALSE);
   
   } /* End if invalid Overwrite arg */
      
   
   if (PerformMove) {
      
      OS_Status = OS_mv(MoveCmd->SourceFilename, MoveCmd->TargetFilename);

      if (OS_Status == OS_SUCCESS) {
      
         RetStatus = TRUE;      
         CFE_EVS_SendEvent(FILECMD_MOVE_EID, CFE_EVS_DEBUG, "Move file from %s to %s",
                           MoveCmd->SourceFilename, MoveCmd->TargetFilename);
      }
      else {
         
         CFE_EVS_SendEvent(FILECMD_COPY_ERR_EID, CFE_EVS_ERROR,
            "Move file %s to %s failed: Parameters validated but OS_mv() failed with status=%d",
            MoveCmd->SourceFilename, MoveCmd->TargetFilename, (int)OS_Status);
      }
      
   } /* End if Perform move */
   
   return RetStatus;

} /* End of FILECMD_Move() */


/******************************************************************************
** Function: FILECMD_Rename
**
*/
boolean FILECMD_Rename(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILECMD_Rename_t* RenameCmd = (FILECMD_Rename_t *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32   OS_Status;   
   boolean RetStatus = FALSE;
   

   FileInfo = FileUtil_GetFileInfo(RenameCmd->SourceFilename, OS_MAX_PATH_LEN, FALSE);
   
   if (FILEUTIL_FILE_EXISTS(FileInfo.State)) {
   
      FileInfo = FileUtil_GetFileInfo(RenameCmd->TargetFilename, OS_MAX_PATH_LEN, FALSE);
   
      if (FileInfo.State == FILEUTIL_FILE_NONEXISTENT) {
      
         OS_Status = OS_rename(RenameCmd->SourceFilename, RenameCmd->TargetFilename);
        
         if (OS_Status == OS_SUCCESS) {
            
            RetStatus = TRUE;
            CFE_EVS_SendEvent(FILECMD_RENAME_EID, CFE_EVS_DEBUG, "Renamed file from %s to %s",
               RenameCmd->SourceFilename, RenameCmd->TargetFilename);
            
         }
         else {
            
            CFE_EVS_SendEvent(FILECMD_RENAME_ERR_EID, CFE_EVS_ERROR,
               "Renamed file %s to %s failed: Parameters validated but OS_rename() failed with status=%d",
               RenameCmd->SourceFilename, RenameCmd->TargetFilename, (int)OS_Status);

         }

      
      } /* End if target file doesn't exist */
      else {
       
         CFE_EVS_SendEvent(FILECMD_RENAME_ERR_EID, CFE_EVS_ERROR,
            "Renamed file %s to %s failed: Target file exists",
            RenameCmd->SourceFilename, RenameCmd->TargetFilename);
      
      }
   } /* End if source file exists */
   else {
    
      CFE_EVS_SendEvent(FILECMD_RENAME_ERR_EID, CFE_EVS_ERROR,
         "Renamed file %s to %s failed: Source file doesn't exist",
         RenameCmd->SourceFilename, RenameCmd->TargetFilename);
   
   }
   
   return RetStatus;

} /* End of FILECMD_Rename() */


/******************************************************************************
** Function: FILECMD_SetPermissions
**
*/
boolean FILECMD_SetPermissions(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   FILECMD_SetPermissions_t* SetPermissionsCmd = (FILECMD_SetPermissions_t *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32   OS_Status;   
   boolean RetStatus = FALSE;
   
   
   if (FileUtil_VerifyFilenameStr(SetPermissionsCmd->Filename)) {
      
      FileInfo = FileUtil_GetFileInfo(SetPermissionsCmd->Filename, OS_MAX_PATH_LEN, FALSE);
      if (FILEUTIL_FILE_EXISTS(FileInfo.State)) {

         OS_Status = OS_chmod(SetPermissionsCmd->Filename, SetPermissionsCmd->Mode);

         if (OS_Status == OS_SUCCESS) {
            
            RetStatus = TRUE;
            CFE_EVS_SendEvent(FILECMD_SET_PERMISSIONS_EID, CFE_EVS_DEBUG,
               "Set permissions for file %s to 0o%4o ",
               SetPermissionsCmd->Filename, SetPermissionsCmd->Mode);
               
         }
         else {
               
            CFE_EVS_SendEvent(FILECMD_SET_PERMISSIONS_EID, CFE_EVS_ERROR,
               "Set file permissions failed: Parameters validated but OS_chmod() failed with status=%d",
               (int)OS_Status);
         }
         
      } /* End if file exists */
      else {
         
         CFE_EVS_SendEvent(FILECMD_SET_PERMISSIONS_ERR_EID, CFE_EVS_ERROR,
                           "Set file permissions failed: Nonexistent file %s",
                           SetPermissionsCmd->Filename);

      }

   } /* End if valid filename */
   else {

      CFE_EVS_SendEvent(FILECMD_SET_PERMISSIONS_ERR_EID, CFE_EVS_ERROR,
         "Set file permissions failed: Invalid filename %s",
         SetPermissionsCmd->Filename);
   
   } /* End if invalid filename */
   
   return RetStatus;

} /* End of FILECMD_SetPermissions() */
