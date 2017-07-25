/* 
** Purpose: Application Framework Library utilities
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
**
*/

#ifndef _app_fw_utils_
#define _app_fw_utils_

/*
** Includes
*/


/*
** Type Definitions
*/


/*
** Exported Functions
*/


/******************************************************************************
** Function: AppFw_VerifyFilenameStr
**
**  Verify file name len, termination, and characters are valid.
*/
boolean AppFw_VerifyFilenameStr(const char* Filename);


/******************************************************************************
** Function: AppFw_VerifyFileForRead
**
**   Verify file name is valid and that the file exists for a read operation.
*/
boolean AppFw_VerifyFileForRead(const char* Filename);


/******************************************************************************
** Function: AppFw_VerifyDirForWrite
**
**   Verify file name is valid and that the directory exists.
*/
boolean AppFw_VerifyDirForWrite(const char* Filename);

#endif /* _app_fw_utils_ */



