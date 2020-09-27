/*
** Purpose: Process directory commands for FileMgr
**
** Notes:
**   1. TBD
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _dircmd_
#define _dircmd_

/*
** Includes
*/

#include "app_cfg.h"

/*
** Event Message IDs
*/

#define DIRCMD_CREATE_EID      (DIRCMD_BASE_EID + 1)
#define DIRCMD_CREATE_ERR_EID  (DIRCMD_BASE_EID + 0)
#define DIRCMD_DELETE_EID      (DIRCMD_BASE_EID + 3)
#define DIRCMD_DELETE_ERR_EID  (DIRCMD_BASE_EID + 2)
#define DIRCMD_DELETE_ALL_EID         (DIRCMD_BASE_EID + 3)
#define DIRCMD_DELETE_ALL_ERR_EID     (DIRCMD_BASE_EID + 2)
#define DIRCMD_DELETE_ALL_WARNING_EID (DIRCMD_BASE_EID + 2)

/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** DIRCMD_Class
*/

typedef struct {

   uint16  WarnCmdCnt;
   
} DIRCMD_Class;


/******************************************************************************
** Command Packets
*/

typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    DirName[OS_MAX_PATH_LEN];

} DIRCMD_Create_t;
#define DIRCMD_CREATE_CMD_DATA_LEN  (sizeof(DIRCMD_Create_t) - CFE_SB_CMD_HDR_SIZE)



typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    DirName[OS_MAX_PATH_LEN];
    
} DIRCMD_Delete_t;
#define DIRCMD_DELETE_CMD_DATA_LEN  (sizeof(DIRCMD_Delete_t) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    DirName[OS_MAX_PATH_LEN];

} DIRCMD_DeleteAll_t;
#define DIRCMD_DELETE_ALL_CMD_DATA_LEN  (sizeof(DIRCMD_DeleteAll_t) - CFE_SB_CMD_HDR_SIZE)


typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    DirName[OS_MAX_PATH_LEN];
    char    Filename[OS_MAX_PATH_LEN];
    uint8   IncludeSizeTime;
    uint8   Spare01[3];
 
} DIRCMD_ListInFileCmd;
#define DIRCMD_LIST_IN_FILE_CMD_DATA_LEN  (sizeof(DIRCMD_ListInFileCmd) - CFE_SB_CMD_HDR_SIZE)


typedef struct {
 
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    DirName[OS_MAX_PATH_LEN];
    uint32  DirListOffset;          /* Starting offset into directory listing */
    uint8   IncludeSizeTime;
    uint8   Spare01[3];

} DIRCMD_ListInPktCmd;
#define DIRCMD_LIST_IN_PKT_CMD_DATA_LEN  (sizeof(DIRCMD_ListInPktCmd) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

    char    EntryName[OS_MAX_PATH_LEN];
    uint32  EntrySize;
    uint32  ModifyTime;    /* File's Last Modification Times */
    uint32  Mode;          /* Mode of the file (Permissions) */
} DIRCMD_PktListEntry;

typedef struct {

    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    char    DirName[OS_MAX_PATH_LEN]; 
    uint32  TotalFiles;                 /* Number of files in the directory */
    uint32  PacketFiles;                /* Number of files in this packet   */
    uint32  FirstFile;                  /* Index into directory files of first packet file */
    DIRCMD_PktListEntry  FileList[FILEMGR_DIR_LIST_PKT_ENTRIES];

} FM_DirListPkt_t;


/******************************************************************************
** File Structures
*/

typedef struct {

    char    DirName[OS_MAX_PATH_LEN];       /**< \brief Directory name */
    uint32  DirEntries;                     /**< \brief Number of entries in the directory */
    uint32  FileEntries;                    /**< \brief Number of entries written to output file */

} DIRCMD_FileListStats;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: DIRCMD_Constructor
**
** Initialize the DIRCMD to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void DIRCMD_Constructor(DIRCMD_Class *DirCmdPtr);


/******************************************************************************
** Function: DIRCMD_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void DIRCMD_ResetStatus(void);


/******************************************************************************
** Function: DIRCMD_Create
**
** Create a new directory. 
*/
boolean DIRCMD_Create(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: DIRCMD_Delete
**
** Delete an existing empty directory.
*/
boolean DIRCMD_Delete(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: DIRCMD_DeleteAll
**
*/
boolean DIRCMD_DeleteAll(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _dircmd_ */
