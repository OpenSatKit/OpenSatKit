/* 
** Purpose: App Framework Library utilities
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

/*s
** Includes
*/

extern "C" {
   #include "cfe.h"
   #include "cfs_utils.h"
}
#include "FileUtil.h"


/*
** Event Message IDs
*/
enum EID {
   
   EID_INVLD_FILENAME_LEN = oskfw::Cfg::UTIL_BASE_EID,
   EID_INVLD_FILENAME_STR,
   EID_INVLD_FILENAME_CHR,
   EID_FILE_READ_OPEN_ERR

};

namespace oskfw {
   
/******************************************************************************
** Function: ReadLine
**
** Read a line from a text file.
**
*/
boolean FileUtil::ReadLine (int FileHandle, char* DestBuf, int MaxChar) {

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
   
} // End ReadLine()


/******************************************************************************
** Function: VerifyFilenameStr
**
** Notes:
**  1. Verify file name len, termination, and characters are valid.
*/
boolean FileUtil::VerifyFilenameStr(const char* FileName) {
   
   int16    Len = 0;
   boolean  RetStatus = FALSE;
   
   /* Search file system name buffer for a string terminator */
   while (Len < OS_MAX_PATH_LEN) {
      
      if (FileName[Len] == '\0') break;
      Len++;
   }

   if (Len == 0) {
      
      /* TODO - Could allow a default filename to be used when no file specified */
      CFE_EVS_SendEvent(EID_INVLD_FILENAME_LEN, CFE_EVS_ERROR, "Invalid filename string: Length is 0");
   } 
   else if (Len == OS_MAX_PATH_LEN) {
      
      CFE_EVS_SendEvent(EID_INVLD_FILENAME_STR, CFE_EVS_ERROR, "Invalid filename string: No NULL termintaion character");     
   }
   else {
   
      // Verify characters in string name
      // Cast away the const. CFS really should change
      if (CFS_IsValidFilename((char *)FileName, Len))    {
         
         RetStatus = TRUE;  
      
      }
      else {
         
         CFE_EVS_SendEvent(EID_INVLD_FILENAME_CHR, CFE_EVS_ERROR, "Invalid characters in filename %s",FileName);     
      
      }      
   } /* End if valid length */
   
  return RetStatus;

} // End VerifyFilenameStr()


/******************************************************************************
** Function: VerifyFileForRead
**
** Notes:
**  1.  Verify file name is valid and that the file exists for a read operation.
**  2. The file is opened/closed to make sure it's valid for a read operation.
**     The file descriptor is not returned to the caller function because there
**     are scenarios when the user must stil open the file.  For example when
**     they pass the filename to a third party library. 
*/
boolean FileUtil::VerifyFileForRead(const char* Filename) {

   int32    FileDescriptor;
   boolean  RetStatus = FALSE;
   
   if (FileUtil::VerifyFilenameStr(Filename)) {
      
      FileDescriptor = OS_open(Filename, OS_READ_ONLY, 0);
         
      /* OS convention is negative return value is an error */ 
      if (FileDescriptor >= 0) {
         
         OS_close (FileDescriptor);
         RetStatus = TRUE;  
      }
      else {
         
         CFE_EVS_SendEvent(EID_FILE_READ_OPEN_ERR, CFE_EVS_ERROR, "Read file open failed for %s ",Filename);     
      
      }
   } /* End if valid filename */
   
  return RetStatus;

} // End VerifyFileForRead()


/******************************************************************************
** Function: VerifyDirForWrite
**
** Notes:
**  1. Verify file name is valid and that the directory exists.
*/
boolean FileUtil::VerifyDirForWrite(const char* FileName) {

   boolean  RetStatus = FALSE;
   
   if (FileUtil::VerifyFilenameStr(FileName)) {
      
      /* TODO - Find last \ and check if directory */
      RetStatus = TRUE;  
      
   } /* End if valid filename */
   
  return RetStatus;

} // End VerifyDirForWrite()

} // End namespace oskfw