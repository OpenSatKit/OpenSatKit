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

/*
** Includes
*/

#include "cfe.h"
#include "cfs_utils.h"
#include "app_fw_utils.h"

/*
** Exported Functions
*/

/******************************************************************************
** Function: AppFw_ReadLine
**
** Read a line from a text file.
**
*/
boolean AppFw_ReadLine (int FileHandle, char* DestBuf, int MaxChar) {

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
   
} /* End AppFw_ReadLine() */


/******************************************************************************
** Function: AppFw_VerifyFileNameStr
**
** Notes:
**  1. Verify file name len, termination, and characters are valid.
*/
boolean AppFw_VerifyFileNameStr(const char* FileName)
{

   int16    Len = 0;
   boolean  RetStatus = FALSE;
   
   /* Search file system name buffer for a string terminator */
   while (Len < OS_MAX_PATH_LEN)
   {
      if (FileName[Len] == '\0') break;
      Len++;
   }

   if (Len == 0)
   {
      /* TODO - Could allow a default filename to be used when no file specified */
      CFE_EVS_SendEvent(AFU_INVLD_FILENAME_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid filename string: Length is 0");
   } 
   else if (Len == OS_MAX_PATH_LEN)
   {
      CFE_EVS_SendEvent(AFU_INVLD_FILENAME_STR_ERR_EID, CFE_EVS_ERROR, "Invalid filename string: No NUL termintaion character");     
   }
   else {
   
      /* Verify characters in string name */
      if (CFS_IsValidFilename((char *)FileName, Len))   /* Cast away the const. CFS really should change */
      {
         RetStatus = TRUE;  
      }
      else
      {
         CFE_EVS_SendEvent(AFU_INVLD_FILENAME_CHR_ERR_EID, CFE_EVS_ERROR, "Invalid characters in filename %s",FileName);     
      }
      
   } /* End if valid length */
   
  return RetStatus;

} /* End AppFw_VerifyFileNameStr() */


/******************************************************************************
** Function: AppFw_VerifyFileForRead
**
** Notes:
**  1.  Verify file name is valid and that the file exists for a read operation.
**  2. The file is opened/closed to make sure it's valid for a read operation.
**     The file descriptor is not returned to the caller function because there
**     are scenarios when the user must stil open the file.  For example when
**     they pass the filename to a third party library. 
*/
boolean AppFw_VerifyFileForRead(const char* Filename)
{

   int32    FileDescriptor;
   boolean  RetStatus = FALSE;
   
   if (AppFw_VerifyFileNameStr(Filename))
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
         CFE_EVS_SendEvent(AFU_FILE_READ_OPEN_ERR_EID, CFE_EVS_ERROR, "Read file open failed for %s ",Filename);     
      }
      
   } /* End if valid filename */
   
  return RetStatus;

} /* End AppFw_VerifyFileForRead() */


/******************************************************************************
** Function: AppFw_VerifyDirForWrite
**
** Notes:
**  1. Verify file name is valid and that the directory exists.
*/
boolean AppFw_VerifyDirForWrite(const char* FileName)
{

   boolean  RetStatus = FALSE;
   
   if (AppFw_VerifyFileNameStr(FileName))
   {
      
      /* TODO - Find last \ and check if directory */
      RetStatus = TRUE;  
      
   } /* End if valid filename */
   
  return RetStatus;

} /* End AppFw_VerifyDirForWrite() */

