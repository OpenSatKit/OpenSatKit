/* 
** Purpose: OSK App C Framework Library file utilities
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
** Includes
*/

#include "cfe.h"
#include "cfs_utils.h"
#include "fileutil.h"

#include <string.h>

/*********************/
/** Local Functions **/
/*********************/

static void CheckFileOpenState(uint32 ObjId, void* CallbackArg);
static void LoadOpenFileData(uint32 ObjId, void* CallbackArg);


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: FileUtil_AppendPathSep
**
** Append a path separator to a directory path. 
** 
** Returns FALSE if invalid string length or appending the separator would
** exceed the BufferLen.
**
*/
boolean FileUtil_AppendPathSep(char *DirName, uint16 BufferLen)
{

   uint16 StringLen;
   boolean RetStatus = FALSE;
   
   StringLen = strlen(DirName);

   if (( StringLen > 0) && (StringLen < BufferLen)) {

      if (DirName[StringLen - 1] != FILEUTIL_DIR_SEP_CHAR) {
         
         if (StringLen < BufferLen-1) {
         
            strcat(DirName, FILEUTIL_DIR_SEP_STR);
            RetStatus = TRUE;        
   
         }      
      } /* End if no path separator */
      else {
          RetStatus = TRUE;
      }
      
   } /* End if valid string length */

   return RetStatus;
   
} /* End FileUtil_AppendPathSep() */


/******************************************************************************
** Function: FileUtil_GetFileInfo
**
** Return file state (FileUtil_FileState) and optionally include the file size
** and time for existing files.
*/
FileUtil_FileInfo FileUtil_GetFileInfo(char *Filename, uint16 FilenameBufLen, boolean IncludeSizeTime)
{
   
   os_fstat_t               FileStatus;
   FileUtil_FileInfo        FileInfo;
   FileUtil_CheckFileState  FileState;
    
   FileInfo.IncludeSizeTime = IncludeSizeTime;
   FileInfo.Size  = 0;
   FileInfo.Time  = 0;
   FileInfo.Mode  = 0;
   FileInfo.State = FILEUTIL_FILENAME_INVALID;

   /* TODO - Fix all file utilities to accept a length parameter with a OS_MAX_PATH_LEN check */
   if (FilenameBufLen != OS_MAX_PATH_LEN) {
      CFE_EVS_SendEvent(FILEUTIL_MAX_PATH_LEN_CONFLICT_EID, CFE_EVS_ERROR, 
         "FileUtil_GetFileInfo() checking a filename buffer len=%d using a utility hard coded with OS_MAX_PATH_LEN=%d",
         FilenameBufLen, OS_MAX_PATH_LEN);
   }

   if (FileUtil_VerifyFilenameStr(Filename)) {
      
      /* Check to see if Filename is exists */
      if (OS_stat(Filename, &FileStatus) == OS_SUCCESS) {
         
         FileInfo.Mode = OS_FILESTAT_MODE(FileStatus);
         
         if (OS_FILESTAT_ISDIR(FileStatus)) {
            
            FileInfo.State = FILEUTIL_FILE_IS_DIR;
         
         }
         else {
            
            FileState.IsOpen = FALSE;
            FileState.Name   = Filename;

            OS_ForEachObject(0, CheckFileOpenState, &FileState);

            FileInfo.State = FileState.IsOpen? FILEUTIL_FILE_OPEN : FILEUTIL_FILE_CLOSED;

            if (IncludeSizeTime) {
               
               FileInfo.Size  = OS_FILESTAT_SIZE(FileStatus);
               FileInfo.Time  = OS_FILESTAT_TIME(FileStatus);

            }
         }
         
      } /* End if file exists */
      else {
         
         FileInfo.State = FILEUTIL_FILE_NONEXISTENT;

      } /* End if file doesn't exist */
      
      
   } /* End if valid filename */

   return (FileInfo);

} /* End FileUtil_GetFileInfo() */


/******************************************************************************
** Function: FileUtil_FileStateStr
**
** Type checking should enforce valid parameter but check just to be safe.
*/
const char* FileUtil_FileStateStr(FileUtil_FileState  FileState)
{

   static char* FileStateStr[] = {
      "Undefined", 
      "Invalid Filename",    /* FILEUTIL_FILENAME_INVALID */
      "Nonexistent File",    /* FILEUTIL_FILE_NONEXISTENT */
      "File Open",           /* FILEUTIL_FILE_OPEN        */
      "File Closed",         /* FILEUTIL_FILE_OPEN        */
      "File is a Directory"  /* FILEUTIL_FILE_IS_DIR      */
   };

   uint8 i = 0;
   
   if ( FileState >= FILEUTIL_FILENAME_INVALID &&
        FileState <= FILEUTIL_FILE_IS_DIR) {
   
      i =  FileState;
   
   }
        
   return FileStateStr[i];

} /* End FileUtil_FileStateStr() */


/******************************************************************************
** Function: FileUtil_GetOpenFileList
**
** Type checking should enforce valid parameter but check just to be safe.
*/

uint16 FileUtil_GetOpenFileList(FileUtil_OpenFileList *OpenFileList)
{
    OpenFileList->OpenCount = 0;
    
    OS_ForEachObject(0, LoadOpenFileData, (void*)OpenFileList);

    return (OpenFileList->OpenCount);

} /* End FileUtil_GetOpenFileList() */


/******************************************************************************
** Function: FileUtil_ReadLine
**
** Read a line from a text file.
**
*/
boolean FileUtil_ReadLine (int FileHandle, char* DestBuf, int MaxChar) {

   char    c, *DestPtr;
   int32   ReadStatus;
   boolean RetStatus = FALSE;
   
   /* Decrement MaxChar to leave space for termination character */
   for (DestPtr = DestBuf, MaxChar--; MaxChar > 0; MaxChar--) {
      
	  ReadStatus = OS_read(FileHandle, &c, 1);
	  
	  if (ReadStatus == 0  || ReadStatus == OS_FS_ERROR)
         break;
      
	  *DestPtr++ = c;
      
	  if (c == '\n') {
         RetStatus = TRUE;
         break;
      }
   
   } /* End for loop */
   
   *DestPtr = 0;

   return RetStatus;
   
} /* End FileUtil_ReadLine() */


/******************************************************************************
** Function: FileUtil_VerifyFilenameStr
**
** Notes:
**  1. Verify file name len, termination, and characters are valid.
*/
boolean FileUtil_VerifyFilenameStr(const char* Filename)
{

   int16    Len = 0;
   boolean  RetStatus = FALSE;
   
   /* Search file system name buffer for a string terminator */
   while (Len < OS_MAX_PATH_LEN)
   {
      if (Filename[Len] == '\0') break;
      Len++;
   }

   if (Len == 0)
   {
      /* TODO - Could allow a default filename to be used when no file specified */
      CFE_EVS_SendEvent(FILEUTIL_INVLD_FILENAME_LEN_EID, CFE_EVS_ERROR, "Invalid filename string: Length is 0");
   } 
   else if (Len == OS_MAX_PATH_LEN)
   {
      CFE_EVS_SendEvent(FILEUTIL_INVLD_FILENAME_STR_EID, CFE_EVS_ERROR, "Invalid filename string: No NUL termintaion character");     
   }
   else {
   
      /* Verify characters in string name */
      if (CFS_IsValidFilename((char *)Filename, Len))   /* Cast away the const. CFS really should change */
      {
         RetStatus = TRUE;  
      }
      else
      {
         CFE_EVS_SendEvent(FILEUTIL_INVLD_FILENAME_CHR_EID, CFE_EVS_ERROR, "Invalid characters in filename %s",Filename);     
      }
      
   } /* End if valid length */
   
  return RetStatus;

} /* End FileUtil_VerifyFilenameStr() */


/******************************************************************************
** Function: FileUtil_VerifyFileForRead
**
** Notes:
**  1.  Verify file name is valid and that the file exists for a read operation.
**  2. The file is opened/closed to make sure it's valid for a read operation.
**     The file descriptor is not returned to the caller function because there
**     are scenarios when the user must stil open the file.  For example when
**     they pass the filename to a third party library. 
*/
boolean FileUtil_VerifyFileForRead(const char* Filename)
{

   int32    FileDescriptor;
   boolean  RetStatus = FALSE;
   
   if (FileUtil_VerifyFilenameStr(Filename))
   {
      
      FileDescriptor = OS_open(Filename, OS_READ_ONLY, 0);
         
      /* OS convention is negative return value is an error */ 
      if (FileDescriptor >= 0)
      {
         OS_close (FileDescriptor);
         RetStatus = TRUE;  
      }
      else
      {
         CFE_EVS_SendEvent(FILEUTIL_FILE_READ_OPEN_ERR_EID, CFE_EVS_ERROR, "Read file open failed for %s ",Filename);     
      }
      
   } /* End if valid filename */
   
  return RetStatus;

} /* End FileUtil_VerifyFileForRead() */


/******************************************************************************
** Function: FileUtil_VerifyDirForWrite
**
** Notes:
**  1. Verify file name is valid and that the directory exists.
*/
boolean FileUtil_VerifyDirForWrite(const char* Filename)
{

   boolean  RetStatus = FALSE;
   
   if (FileUtil_VerifyFilenameStr(Filename))
   {
      
      /* TODO - Find last \ and check if directory */
      RetStatus = TRUE;  
      
   } /* End if valid filename */
   
  return RetStatus;

} /* End FileUtil_VerifyDirForWrite() */


/******************************************************************************
** Function: CheckFileOpenState
**
** Callback function for OS_ForEachObject() to check whether a file is open.
** This function is tightly coupledd with FileUtil_GetFileInfo() so only set
** File->IsOpen if a file is verified as open otherwise preserve the state.  
**
*/
static void CheckFileOpenState(uint32 ObjId, void* CallbackArg)
{
   
   FileUtil_CheckFileState* FileState = (FileUtil_CheckFileState *)CallbackArg;
   OS_file_prop_t FileProp;

   if(FileState == (FileUtil_CheckFileState *)NULL) {
      return;
   }
   else if (FileState->Name == (char *)NULL) {
      return;
   }
   
   /*
   ** Get system info for stream objects and compare names
   */
   if(OS_IdentifyObject(ObjId) == OS_OBJECT_TYPE_OS_STREAM) {
      
      if (OS_FDGetInfo(ObjId, &FileProp) == OS_FS_SUCCESS) {
         
         if (strcmp(FileState->Name, FileProp.Path) == 0) {
            
            FileState->IsOpen = TRUE;
         }
      }
   }

} /* End CheckFileOpenState() */


/******************************************************************************
** Function: LoadOpenFileData
**
** Notes:
**  1. Callback function for OS_ForEachObject()
*/
static void LoadOpenFileData(uint32 ObjId, void* CallbackArg)
{
   FileUtil_OpenFileList *OpenFileList = (FileUtil_OpenFileList*)CallbackArg;
   CFE_ES_TaskInfo_t      TaskInfo;
   OS_file_prop_t         FdProp;

   if(OS_IdentifyObject(ObjId) == OS_OBJECT_TYPE_OS_STREAM) {
      
      if(OpenFileList != (FileUtil_OpenFileList*) NULL) {
         
         if(OS_FDGetInfo(ObjId, &FdProp) == OS_SUCCESS) {
            
            strncpy(OpenFileList->Entry[OpenFileList->OpenCount].Filename,
                    FdProp.Path, OS_MAX_PATH_LEN);

            /* Get the name of the application that opened the file */
            memset(&TaskInfo, 0, sizeof(CFE_ES_TaskInfo_t));

            if(CFE_ES_GetTaskInfo(&TaskInfo, FdProp.User) == CFE_SUCCESS) {
               
               strncpy(OpenFileList->Entry[OpenFileList->OpenCount].AppName,
                       (char*)TaskInfo.AppName, OS_MAX_API_NAME);
            }
         }
      } 

      ++OpenFileList->OpenCount;
   }

} /* End LoadOpenFileData() */


