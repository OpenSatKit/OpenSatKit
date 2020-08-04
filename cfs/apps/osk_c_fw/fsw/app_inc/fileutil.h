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

/***********************/
/** Macro Definitions **/
/***********************/

#define FILE_UTIL_THIS_DIRECTORY  "."
#define FILE_UTIL_DIRECTORY       ".."

/*
** Event Message IDs
*/

#define FILE_UTIL_INVLD_FILENAME_LEN_EID     (OSK_C_FW_UTILS_BASE_EID + 0)
#define FILE_UTIL_INVLD_FILENAME_STR_EID     (OSK_C_FW_UTILS_BASE_EID + 1)
#define FILE_UTIL_INVLD_FILENAME_CHR_EID     (OSK_C_FW_UTILS_BASE_EID + 2)
#define FILE_UTIL_FILE_READ_OPEN_ERR_EID     (OSK_C_FW_UTILS_BASE_EID + 3)
#define FILE_UTIL_MAX_PATH_LEN_CONFLICT_EID  (OSK_C_FW_UTILS_BASE_EID + 4)

/**********************/
/** Type Definitions **/
/**********************/

typedef enum {
   
   FILEUTIL_FILE_NAME_INVALID = 1,
   FILEUTIL_FILE_NONEXISTENT  = 2,
   FILEUTIL_FILE_OPEN         = 3,
   FILEUTIL_FILE_CLOSED       = 4,
   FILEUTIL_FILE_IS_DIR       = 5

} FileUtil_FileState;


typedef struct {

   boolean IncludeSizeTime;
   uint32  Size;
   uint32  Time;
   FileUtil_FileState State;

} FileUtil_FileInfo;


typedef struct {

   boolean  IsOpen;
   char*    Name;

} FileUtil_CheckFileState;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: FileUtil_GetFileInfo
**
** Return file state (FileUtil_FileState) and optionally include the file size
** and time for existing files.
*/
FileUtil_FileInfo FileUtil_GetFileInfo(char *Filename, uint32 FilenameBufLen, boolean IncludeSizeTime);


/******************************************************************************
** Function: FileUtil_FileStateStr
**
** Return a string describing the FileUtil_FileState enumeration.
*/
const char* FileUtil_FileStateStr(FileUtil_FileState  FileState);


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
