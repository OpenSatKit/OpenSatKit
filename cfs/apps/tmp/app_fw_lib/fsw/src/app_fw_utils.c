/* 
** Purpose: App Framework Library utilities
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
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
      /* TODO - Coudl allow a default filename to be used when no file specified */
      CFE_EVS_SendEvent(999, CFE_EVS_ERROR, "Invalid filename string: Length is 0");
   } 
   else if (Len == OS_MAX_PATH_LEN)
   {
      CFE_EVS_SendEvent(999, CFE_EVS_ERROR, "Invalid filename string: No NUL termintaion character");     
   }
   else {
   
      /* Verify characters in string name */
      if (CFS_IsValidFilename((char *)FileName, Len))   /* Cast away the const. CFS really should change */
      {
         RetStatus = TRUE;  
      }
      else
      {
         CFE_EVS_SendEvent(999, CFE_EVS_ERROR, "Invalid characters in filename %s",FileName);     
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
         CFE_EVS_SendEvent(999, CFE_EVS_ERROR, "Read file open failed for %s ",Filename);     
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

