/*
** Purpose: Define methods for managing directories
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _dir_
#define _dir_

/*
** Includes
*/

#include "app_cfg.h"

/*
** Event Message IDs
*/

#define DIR_CREATE_EID               (DIR_BASE_EID +  0)
#define DIR_CREATE_ERR_EID           (DIR_BASE_EID +  1)
#define DIR_DELETE_ALL_EID           (DIR_BASE_EID +  2)
#define DIR_DELETE_ALL_ERR_EID       (DIR_BASE_EID +  3)
#define DIR_DELETE_ALL_WARN_EID      (DIR_BASE_EID +  4)
#define DIR_DELETE_EID               (DIR_BASE_EID +  5)
#define DIR_DELETE_ERR_EID           (DIR_BASE_EID +  6)
#define DIR_SEND_LIST_PKT_EID        (DIR_BASE_EID +  7)
#define DIR_SEND_LIST_PKT_ERR_EID    (DIR_BASE_EID +  8)
#define DIR_SEND_LIST_PKT_WARN_EID   (DIR_BASE_EID +  9)
#define DIR_WRITE_LIST_FILE_EID      (DIR_BASE_EID + 10)
#define DIR_WRITE_LIST_FILE_ERR_EID  (DIR_BASE_EID + 11)
#define DIR_WRITE_LIST_FILE_WARN_EID (DIR_BASE_EID + 12)

/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    DirName[OS_MAX_PATH_LEN];

} DIR_CreateCmdMsg;
#define DIR_CREATE_CMD_DATA_LEN  (sizeof(DIR_CreateCmdMsg) - CFE_SB_CMD_HDR_SIZE)



typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    DirName[OS_MAX_PATH_LEN];
    
} DIR_DeleteCmdMsg;
#define DIR_DELETE_CMD_DATA_LEN  (sizeof(DIR_DeleteCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    DirName[OS_MAX_PATH_LEN];

} DIR_DeleteAllCmdMsg;
#define DIR_DELETE_ALL_CMD_DATA_LEN  (sizeof(DIR_DeleteAllCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {
 
   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    DirName[OS_MAX_PATH_LEN];
   uint16  DirListOffset;          /* Starting offset into directory listing */
   uint16  IncludeSizeTime;

} DIR_SendListPktCmdMsg;
#define DIR_SEND_LIST_PKT_CMD_DATA_LEN  (sizeof(DIR_SendListPktCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct
{
   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    DirName[OS_MAX_PATH_LEN];
   char    Filename[OS_MAX_PATH_LEN];
   uint16  IncludeSizeTime;
 
} DIR_WriteListFileCmdMsg;
#define DIR_WRITE_LIST_FILE_CMD_DATA_LEN  (sizeof(DIR_WriteListFileCmdMsg) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

    char    Name[OS_MAX_PATH_LEN];
    uint32  Size;
    uint32  Time;     /* File's Last Modification Times */
    uint32  Mode;     /* Mode of the file (Permissions) */

} DIR_FileEntry;


typedef struct {

    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    char    DirName[OS_MAX_PATH_LEN]; 
    uint32  DirFileCnt;                 /* Number of files in the directory    */
    uint32  PktFileCnt;                 /* Number of files in this packet      */
    uint32  DirListOffset;              /* Starting offset into directory list */
    DIR_FileEntry  File[FILEMGR_DIR_LIST_PKT_ENTRIES];

} DIR_ListPkt;


/******************************************************************************
** File Structures
*/

typedef struct {

    char    DirName[OS_MAX_PATH_LEN];
    uint32  DirFileCnt;                 /* Number of files in the directory */
    uint32  FilesWrittenCnt;            /* Number of entries written to file  */

} DIR_ListFilesStats;


/******************************************************************************
** DIR_Class
*/

typedef struct {

   /*
   ** Framework References
   */
   
   INITBL_Class*  IniTbl;

   /*
   ** Class State Data
   */

   uint16  CmdWarningCnt;
   
   /*
   ** Telemetry
   */
   
   DIR_ListPkt  ListPkt;
   
   /*
   ** Files
   */

   DIR_ListFilesStats  ListFileStats;
   
} DIR_Class;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: DIR_Constructor
**
** Initialize the DIR to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void DIR_Constructor(DIR_Class *DirPtr, INITBL_Class* IniTbl);


/******************************************************************************
** Function: DIR_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void DIR_ResetStatus(void);


/******************************************************************************
** Function: DIR_CreateCmd
**
** Create a new directory. 
*/
boolean DIR_CreateCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: DIR_DeleteCmd
**
** Delete an existing empty directory.
*/
boolean DIR_DeleteCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: DIR_DeleteAllCmd
**
*/
boolean DIR_DeleteAllCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: DIR_SendListPktCmd
**
*/
boolean DIR_SendListPktCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: DIR_WriteListFileCmd
**
** Notes:
**   1. Target file will be overwritten if it exists an is closed.
*/
boolean DIR_WriteListFileCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);

#endif /* _dir_ */
