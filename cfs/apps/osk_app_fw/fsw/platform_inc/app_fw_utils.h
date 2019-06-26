/* 
** Purpose: Application Framework Library utilities
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

#ifndef _app_fw_utils_
#define _app_fw_utils_


/*
** Includes
*/

#include "app_fw_cfg.h"

/*
** Event Message IDs
*/

#define AFU_INVLD_FILENAME_LEN_ERR_EID   (APP_FW_UTILS_BASE_EID + 0)
#define AFU_INVLD_FILENAME_STR_ERR_EID   (APP_FW_UTILS_BASE_EID + 1)
#define AFU_INVLD_FILENAME_CHR_ERR_EID   (APP_FW_UTILS_BASE_EID + 2)
#define AFU_FILE_READ_OPEN_ERR_EID       (APP_FW_UTILS_BASE_EID + 3)

/*
** Type Definitions
*/


/*
** Exported Functions
*/


/******************************************************************************
** Function: AppFw_ReadLine
**
** Read a line from a text file.
**
*/
boolean AppFw_ReadLine (int FileHandle, char* DestBuf, int MaxChar);


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



