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

#define FILEUTIL_DIR_SEP_CHAR '/'
#define FILEUTIL_DIR_SEP_STR  "/"

#define FILEUTIL_CURRENT_DIR  "."
#define FILEUTIL_PARENT_DIR   ".."

/*
** Event Message IDs
*/

#define FILEUTIL_INVLD_FILENAME_LEN_EID     (OSK_C_FW_UTILS_BASE_EID + 0)
#define FILEUTIL_INVLD_FILENAME_STR_EID     (OSK_C_FW_UTILS_BASE_EID + 1)
#define FILEUTIL_INVLD_FILENAME_CHR_EID     (OSK_C_FW_UTILS_BASE_EID + 2)
#define FILEUTIL_FILE_READ_OPEN_ERR_EID     (OSK_C_FW_UTILS_BASE_EID + 3)
#define FILEUTIL_MAX_PATH_LEN_CONFLICT_EID  (OSK_C_FW_UTILS_BASE_EID + 4)

/**********************/
/** Type Definitions **/
/**********************/

/*
** If the state is (FILEUTIL_FILE_OPEN or FILEUTIL_FILE_CLOSED) then the
** file exists and is not a directory
*/
typedef enum {
   
   FILEUTIL_FILENAME_INVALID  = 1,
   FILEUTIL_FILE_NONEXISTENT  = 2,
   FILEUTIL_FILE_OPEN         = 3,
   FILEUTIL_FILE_CLOSED       = 4,
   FILEUTIL_FILE_IS_DIR       = 5

} FileUtil_FileState;

#define FILEUTIL_FILE_EXISTS(state) ((state==FILEUTIL_FILE_OPEN) || (state==FILEUTIL_FILE_CLOSED))

/*
** File Information
*/

typedef struct {

   boolean IncludeSizeTime;
   uint32  Size;
   uint32  Time;
   uint32  Mode;
   FileUtil_FileState State;

} FileUtil_FileInfo;

typedef struct {

   boolean  IsOpen;
   char*    Name;

} FileUtil_CheckFileState;


/* 
** Contains file information from the cFE's perspective which means 
** the information is based on app's that have used the cFE's file
** API.
*/
typedef struct {

   char    Filename[OS_MAX_PATH_LEN];
   char    AppName[OS_MAX_API_NAME];


} FileUtil_OpenFileEntry;


typedef struct {

   uint16   OpenCount;
   FileUtil_OpenFileEntry Entry[OS_MAX_NUM_OPEN_FILES];

} FileUtil_OpenFileList;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: FileUtil_AppendPathSep
**
** Append a path separator to a directory path. If DirName already ends in a 
** path separator it doesn't modify DirName.
** 
** Returns FALSE if invalid string length or appending the separator would
** exceed the BufferLen.
**
*/
boolean FileUtil_AppendPathSep(char *DirName, uint16 BufferLen);


/******************************************************************************
** Function: FileUtil_GetFileInfo
**
** First verifies the filename string itself and determines the file state and
** returns file state (FileUtil_FileState) and optionally includes the file size
** and time for existing files.
*/
FileUtil_FileInfo FileUtil_GetFileInfo(char *Filename, uint16 FilenameBufLen, boolean IncludeSizeTime);


/******************************************************************************
** Function: FileUtil_GetOpenFileList
**
** Loads the caller supplied OpenFileList data structure with the number of
** open files and details about each. 
**
** Returns the number of open files as a convenience even though it's contained
** in the OpenFileList data struture.
*/
uint16 FileUtil_GetOpenFileList(FileUtil_OpenFileList *OpenFileList);


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
