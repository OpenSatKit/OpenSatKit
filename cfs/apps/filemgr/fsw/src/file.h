/*
** Purpose: Define methods for managing files
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

#ifndef _file_
#define _file_

/*
** Includes
*/

#include "app_cfg.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define FILE_IGNORE_CRC  0


/*
** Event Message IDs
*/

#define FILE_CONCATENATE_EID          (FILE_BASE_EID +  0)
#define FILE_CONCATENATE_ERR_EID      (FILE_BASE_EID +  1)
#define FILE_COPY_EID                 (FILE_BASE_EID +  2)
#define FILE_COPY_ERR_EID             (FILE_BASE_EID +  3)
#define FILE_DECOMPRESS_EID           (FILE_BASE_EID +  4)
#define FILE_DECOMPRESS_ERR_EID       (FILE_BASE_EID +  5)
#define FILE_DELETE_EID               (FILE_BASE_EID +  6)
#define FILE_DELETE_ERR_EID           (FILE_BASE_EID +  7)
#define FILE_MOVE_EID                 (FILE_BASE_EID +  8)
#define FILE_MOVE_ERR_EID             (FILE_BASE_EID +  9)
#define FILE_RENAME_EID               (FILE_BASE_EID + 10)
#define FILE_RENAME_ERR_EID           (FILE_BASE_EID + 11)
#define FILE_SEND_INFO_EID            (FILE_BASE_EID + 12)
#define FILE_SEND_INFO_ERR_EID        (FILE_BASE_EID + 13)
#define FILE_SET_PERMISSIONS_EID      (FILE_BASE_EID + 14)
#define FILE_SET_PERMISSIONS_ERR_EID  (FILE_BASE_EID + 15)
#define FILE_COMPUTE_FILE_CRC_ERR_EID (FILE_BASE_EID + 16)

/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16  Overwrite;
    char    SourceFilename[OS_MAX_PATH_LEN];
    char    TargetFilename[OS_MAX_PATH_LEN];

} FILE_CopyCmdMsg;
#define FILE_COPY_CMD_DATA_LEN  (sizeof(FILE_CopyCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    Filename[OS_MAX_PATH_LEN];
} FILE_DeleteCmdMsg;
#define FILE_DELETE_CMD_DATA_LEN  (sizeof(FILE_DeleteCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16  Overwrite;
    char    SourceFilename[OS_MAX_PATH_LEN];
    char    TargetFilename[OS_MAX_PATH_LEN];

} FILE_MoveCmdMsg;
#define FILE_MOVE_CMD_DATA_LEN  (sizeof(FILE_MoveCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    SourceFilename[OS_MAX_PATH_LEN];
    char    TargetFilename[OS_MAX_PATH_LEN];

} FILE_RenameCmdMsg;
#define FILE_RENAME_CMD_DATA_LEN  (sizeof(FILE_RenameCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    SourceFilename[OS_MAX_PATH_LEN];
    char    TargetFilename[OS_MAX_PATH_LEN];

} FILE_DecompressCmdMsg;
#define FILE_DECOMPRESS_CMD_DATA_LEN  (sizeof(FILE_DecompressCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    Source1Filename[OS_MAX_PATH_LEN];
    char    Source2Filename[OS_MAX_PATH_LEN];
    char    TargetFilename[OS_MAX_PATH_LEN];

} FILE_ConcatenateCmdMsg;
#define FILE_CONCATENATE_CMD_DATA_LEN  (sizeof(FILE_ConcatenateCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    Filename[OS_MAX_PATH_LEN];
    uint8   ComputeCrc;                     /* Boolean flag: TRUE = Compute CRC */ 
    uint8   CrcType;                        /* See CFE_ES for CRC definitions   */ 

} FILE_SendInfoPktCmdMsg;
#define FILE_SEND_INFO_PKT_CMD_DATA_LEN  (sizeof(FILE_SendInfoPktCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    Filename[OS_MAX_PATH_LEN];
    uint32  Mode;                           /* Permissions, passed directly to OS_chmod */

} FILE_SetPermissionsCmdMsg;
#define FILE_SET_PERMISSIONS_CMD_DATA_LEN  (sizeof(FILE_SetPermissionsCmdMsg) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telmetery Packets
*/

/* 
** Packet sent in response to FILE_SendInfoPktCmd that contains information
** for a single file
*/

typedef struct {

    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8   State;                 /* See FileUtil_FileState definitions */
    uint8   CrcComputed;           /* Flag indicating whether a CRC was computed */
    uint8   Spare[2];              /* Double word alignment padding */
    uint32  Crc;                   /* CRC value if computed */
    uint32  Size;                  /* File Size in bytes */
    uint32  Time;                  /* Time of last file modification */
    uint32  Mode;                  /* Permissions as received by call to TODO */    
    char    Filename[OS_MAX_PATH_LEN];

} FILE_InfoPkt;


/******************************************************************************
** FILE_Class
*/

typedef struct {

   /*
   ** Framework References
   */
   
   INITBL_Class*  IniTbl;

   /*
   ** Telemetry Packets
   */
   
   FILE_InfoPkt  InfoPkt;

   /*
   ** Class State Data
   */

   uint16  CmdWarningCnt;

   char FileTaskBuf[FILEMGR_TASK_FILE_BLOCK_SIZE];
   
} FILE_Class;


/******************************************************************************
** File Structure
*/


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: FILE_Constructor
**
** Initialize the example object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void FILE_Constructor(FILE_Class *FilePtr, INITBL_Class* IniTbl);


/******************************************************************************
** Function: FILE_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void FILE_ResetStatus(void);


/******************************************************************************
** Function: FILE_ConcatenateCmd
**
*/
boolean FILE_ConcatenateCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILE_DecompressCmd
**
*/
boolean FILE_DecompressCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILE_CopyCmd
**
*/
boolean FILE_CopyCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILE_DeleteCmd
**
*/
boolean FILE_DeleteCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILE_MoveCmd
**
*/
boolean FILE_MoveCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILE_RenameCmd
**
*/
boolean FILE_RenameCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILE_SendInfoPktCmd
**
** Notes:
**   1. If the file exists then a telemetry packet will be sent regardless of
**      whether a CRC was request but could not be computed due to the file
**      being open
**
*/
boolean FILE_SendInfoPktCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILE_SetPermissionsCmd
**
*/
boolean FILE_SetPermissionsCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _file_ */
