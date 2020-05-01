/* 
** Purpose: C App Framework file utilities
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

#ifndef _file_util_
#define _file_util_


/*
** Includes
*/

#include "osk_c_fw_cfg.h"

/*
** Event Message IDs
*/

#define FILE_UTIL_INVLD_FILENAME_LEN_EID   (OSK_C_FW_UTILS_BASE_EID + 0)
#define FILE_UTIL_INVLD_FILENAME_STR_EID   (OSK_C_FW_UTILS_BASE_EID + 1)
#define FILE_UTIL_INVLD_FILENAME_CHR_EID   (OSK_C_FW_UTILS_BASE_EID + 2)
#define FILE_UTIL_FILE_READ_OPEN_ERR_EID   (OSK_C_FW_UTILS_BASE_EID + 3)

/*
** Type Definitions
*/


/*
** Exported Functions
*/


/******************************************************************************
** Function: FileUtil_ReadLine
**
** Read a line from a text file.
**
*/
boolean FileUtil_ReadLine (int FileHandle, char* DestBuf, int MaxChar);


/******************************************************************************
** Function: FileUtil_VerifyFilenameStr
**
**  Verify file name len, termination, and characters are valid.
*/
boolean FileUtil_VerifyFilenameStr(const char* Filename);


/******************************************************************************
** Function: FileUtil_VerifyFileForRead
**
**   Verify file name is valid and that the file exists for a read operation.
*/
boolean FileUtil_VerifyFileForRead(const char* Filename);


/******************************************************************************
** Function: FileUtil_VerifyDirForWrite
**
**   Verify file name is valid and that the directory exists.
*/
boolean FileUtil_VerifyDirForWrite(const char* Filename);

#endif /* _file_util_ */
