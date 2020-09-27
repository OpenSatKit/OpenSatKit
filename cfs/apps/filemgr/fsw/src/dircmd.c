/*
** Purpose: Process directory commands for FileMgr
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
#include "dircmd.h"

/**********************/
/** Global File Data **/
/**********************/

static DIRCMD_Class*  DirCmd = NULL;


/*
** Local Function Prototypes
*/


/******************************************************************************
** Function: DIRCMD_Constructor
**
*/
void DIRCMD_Constructor(DIRCMD_Class*  DirCmdPtr)
{
 
   DirCmd = DirCmdPtr;

   CFE_PSP_MemSet((void*)DirCmd, 0, sizeof(DIRCMD_Class));
 
} /* End DIRCMD_Constructor() */


/******************************************************************************
** Function:  DIRCMD_ResetStatus
**
*/
void DIRCMD_ResetStatus()
{
 
   DirCmd->WarnCmdCnt = 0;
   
} /* End DIRCMD_ResetStatus() */


/******************************************************************************
** Function: DIRCMD_Create
**
*/
boolean DIRCMD_Create(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   DIRCMD_Create_t* CreateCmd = (DIRCMD_Create_t *) MsgPtr;
   boolean RetStatus = FALSE;
   FileUtil_FileInfo FileInfo;
   int32 OS_Status;
   
   FileInfo = FileUtil_GetFileInfo(CreateCmd->DirName, OS_MAX_PATH_LEN, FALSE);

   if (FileInfo.State == FILEUTIL_FILE_NONEXISTENT) {
      
      OS_Status = OS_mkdir(CreateCmd->DirName, 0);

      if (OS_Status == OS_SUCCESS) {

         RetStatus = TRUE;         
         CFE_EVS_SendEvent(DIRCMD_CREATE_EID, CFE_EVS_DEBUG, "Created directory %s", CreateCmd->DirName);
         
      }
      else {
         
         CFE_EVS_SendEvent(DIRCMD_CREATE_ERR_EID, CFE_EVS_ERROR,
            "Create directory %s failed: Parameters validated but OS_mkdir() failed with status=%d",
            CreateCmd->DirName, (int)OS_Status);
      
      }
   }
   else {
      
      CFE_EVS_SendEvent(DIRCMD_CREATE_ERR_EID, CFE_EVS_ERROR,
         "Create directory command for %s failed due to %s",
         CreateCmd->DirName, FileUtil_FileStateStr(FileInfo.State));
      
   }
   
   return RetStatus;
   
   
} /* End DIRCMD_Create() */



/******************************************************************************
** Function: DIRCMD_Delete
**
*/
boolean DIRCMD_Delete(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   DIRCMD_Delete_t* DeleteCmd = (DIRCMD_Delete_t *) MsgPtr;
   FileUtil_FileInfo FileInfo;
   int32 OS_Status;
   os_dirp_t    DirPtr;
   os_dirent_t* DirEntry;
   
   boolean RetStatus = FALSE;
   boolean RemoveDir = TRUE;
   
   FileInfo = FileUtil_GetFileInfo(DeleteCmd->DirName, OS_MAX_PATH_LEN, FALSE);

   if (FileInfo.State == FILEUTIL_FILE_IS_DIR) {
      
      DirPtr = OS_opendir(DeleteCmd->DirName);  /* Open the dir so we can see if it is empty */ 

      if (DirPtr == (os_dirp_t)NULL) {
         
         CFE_EVS_SendEvent(DIRCMD_DELETE_ERR_EID, CFE_EVS_ERROR,
            "Create directory %s failed: OS_opendir return a null pointer", DeleteCmd->DirName);

         RemoveDir = FALSE;

      }
      else {
         
        /* Look for a directory entry that is not "." or ".." */
        while (((DirEntry = OS_readdir(DirPtr)) != NULL) && (RemoveDir == TRUE)) {
           
            if ((strcmp(OS_DIRENTRY_NAME(*DirEntry), FILEUTIL_THIS_DIR)   != 0) &&
                (strcmp(OS_DIRENTRY_NAME(*DirEntry), FILEUTIL_PARENT_DIR) != 0)) {
                   
               CFE_EVS_SendEvent(DIRCMD_DELETE_ERR_EID, CFE_EVS_ERROR,
                  "Create directory %s failed: Directory is not empty", DeleteCmd->DirName);

                RemoveDir = FALSE;
            }
         }

         OS_closedir(DirPtr);
      }

      if (RemoveDir) {
         
         OS_Status = OS_rmdir(DeleteCmd->DirName);

         if (OS_Status == OS_SUCCESS) {
            
            RetStatus = TRUE;
            CFE_EVS_SendEvent(DIRCMD_DELETE_EID, CFE_EVS_DEBUG, "Deleted directory %s", DeleteCmd->DirName);
            
         }
         else {
            
            CFE_EVS_SendEvent(DIRCMD_DELETE_ERR_EID, CFE_EVS_ERROR,
               "Delete directory %s failed: Parameters validated but OS_rmdir() failed with status=%d",
               DeleteCmd->DirName, (int)OS_Status);

         }
      }


   } /* End if file is a directory */
   else {
      
      CFE_EVS_SendEvent(DIRCMD_DELETE_ERR_EID, CFE_EVS_ERROR,
         "Delete directory command for %s failed due to %s",
         DeleteCmd->DirName, FileUtil_FileStateStr(FileInfo.State));
      
   } /* End if file is not a directory */
   
   return RetStatus;
   
   
} /* End DIRCMD_Delete() */


/******************************************************************************
** Function: DIRCMD_DeleteAll
**
*/
boolean DIRCMD_DeleteAll(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   
   DIRCMD_DeleteAll_t* DeleteAllCmd = (DIRCMD_DeleteAll_t *) MsgPtr;
   
   FileUtil_FileInfo FileInfo;
   os_dirp_t         DirPtr = NULL;
   os_dirent_t      *DirEntry = NULL;
   
   uint32  NameLength = 0;
   uint32  DeleteCount = 0;
   uint32  FileNotDeletedCount = 0;
   uint32  DirSkippedCount = 0;
   char    DirWithSep[OS_MAX_PATH_LEN];
   char    Filename[OS_MAX_PATH_LEN];
   int32   OS_Status;   
   boolean RetStatus = FALSE;


   FileInfo = FileUtil_GetFileInfo(DeleteAllCmd->DirName, OS_MAX_PATH_LEN, FALSE);

   if (FileInfo.State == FILEUTIL_FILE_IS_DIR) {
      
      strcpy(DirWithSep, DeleteAllCmd->DirName);
      if (FileUtil_AppendPathSep(DirWithSep, OS_MAX_PATH_LEN)) {
      
         DirPtr = OS_opendir(DeleteAllCmd->DirName);

         if (DirPtr != NULL) {
            
            while ((DirEntry = OS_readdir(DirPtr)) != NULL ) {
               
               /* Ignore the "." and ".." directory entries */
               
               if ((strcmp(OS_DIRENTRY_NAME(*DirEntry), FILEUTIL_THIS_DIR)   != 0) &&
                   (strcmp(OS_DIRENTRY_NAME(*DirEntry), FILEUTIL_PARENT_DIR) != 0)) {
                      
                  /* Construct full path filename */
                  NameLength = strlen(DirWithSep) + strlen(OS_DIRENTRY_NAME(*DirEntry));

                  if (NameLength < OS_MAX_PATH_LEN) {
                     
                     strncpy(Filename, DirWithSep, OS_MAX_PATH_LEN - 1);
                     Filename[OS_MAX_PATH_LEN - 1] = '\0';
                          
                     strncat(Filename, OS_DIRENTRY_NAME(*DirEntry), (NameLength - strlen(DirWithSep)));

                     FileInfo = FileUtil_GetFileInfo(Filename, OS_MAX_PATH_LEN, FALSE);

                     switch (FileInfo.State) {
                              
                        case FILEUTIL_FILE_CLOSED:                   
                        
                           if ((OS_Status = OS_remove(Filename)) == OS_SUCCESS) {
                              
                              OS_rewinddir(DirPtr);  /* Rewind prevents file system from getting confused */

                              ++DeleteCount;
                           }
                           else {
                           
                              ++FileNotDeletedCount;
                           
                           }
                           break;
                        
                        case FILEUTIL_FILE_IS_DIR:
                        
                           ++DirSkippedCount;
                           break;
         
                        case FILEUTIL_FILE_NAME_INVALID:
                        case FILEUTIL_FILE_NONEXISTENT:
                        case FILEUTIL_FILE_OPEN:
                        default:
                           ++FileNotDeletedCount;
                           break;
                  
                     } /* End FileInfo.State switch */
                  
                  } /* End if valid Filename length */
                  else {

                     ++FileNotDeletedCount;
                  
                  } /* End if invalid Filename length */
               } /* End if "." or ".." directory entries */
            } /* End while OS_readdir */

            OS_closedir(DirPtr);
             
            CFE_EVS_SendEvent(DIRCMD_DELETE_ALL_EID, CFE_EVS_DEBUG,
                  "Deleted %d files in %s", (int)DeleteCount, DeleteAllCmd->DirName);

            if (FileNotDeletedCount > 0) {
               CFE_EVS_SendEvent(DIRCMD_DELETE_ALL_WARNING_EID, CFE_EVS_INFORMATION,
                  "Delete all files in %s warning: %d could not be deleted",
                  DeleteAllCmd->DirName, FileNotDeletedCount);
            }

            if (DirSkippedCount > 0) {
               CFE_EVS_SendEvent(DIRCMD_DELETE_ALL_WARNING_EID, CFE_EVS_INFORMATION,
                  "Delete all files in %s warning: %d directories skipped",
                  DeleteAllCmd->DirName, DirSkippedCount);
            }
         
            if ((FileNotDeletedCount > 0) || (DirSkippedCount > 0)) ++DirCmd->WarnCmdCnt;
          
         } /* End if DirPtr != NULL */
         else {

            CFE_EVS_SendEvent(DIRCMD_DELETE_ALL_ERR_EID, CFE_EVS_ERROR,
               "Delete all files in %s failed: Call to OS_opendir() failed",
               DeleteAllCmd->DirName);
             
         } /* End if DirPtr == NULL */
      
      
      } /* End if append path separator */
      else {
      
         CFE_EVS_SendEvent(DIRCMD_DELETE_ALL_ERR_EID, CFE_EVS_ERROR,
            "Delete all files in %s failed: Max path length %d exceeded",
            DeleteAllCmd->DirName, OS_MAX_PATH_LEN);
      
      } /* End if couldn't append path separator */ 
      
   } /* End if file is a directory */ 
   else {
   
      CFE_EVS_SendEvent(DIRCMD_DELETE_ALL_ERR_EID, CFE_EVS_ERROR,
         "Delete all files in %s failed due to %s",
         DeleteAllCmd->DirName, FileUtil_FileStateStr(FileInfo.State));
   
   } /* End if file is not a directory */
   
   return RetStatus;

} /* End of DIRCMD_DeleteAll() */


