/*
** Purpose: Process file commands for FileMgr
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

#ifndef _filecmd_
#define _filecmd_

/*
** Includes
*/

#include "app_cfg.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define FILECMD_IGNORE_CRC  0


/*
** Event Message IDs
*/

#define FILECMD_COPY_EID                (FILECMD_BASE_EID + 0)
#define FILECMD_COPY_ERR_EID            (FILECMD_BASE_EID + 0)
#define FILECMD_DELETE_EID              (FILECMD_BASE_EID + 0)
#define FILECMD_DELETE_ERR_EID          (FILECMD_BASE_EID + 0)
#define FILECMD_MOVE_EID                (FILECMD_BASE_EID + 0)
#define FILECMD_MOVE_ERR_EID            (FILECMD_BASE_EID + 0)
#define FILECMD_RENAME_EID              (FILECMD_BASE_EID + 0)
#define FILECMD_RENAME_ERR_EID          (FILECMD_BASE_EID + 0)
#define FILECMD_SET_PERMISSIONS_EID     (FILECMD_BASE_EID + 0)
#define FILECMD_SET_PERMISSIONS_ERR_EID (FILECMD_BASE_EID + 0)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** FILECMD_Class
*/

typedef struct {

   uint16  WarnCmdCnt;
   FileUtil_OpenFileList OpenFileList;
   
} FILECMD_Class;


/******************************************************************************
** Command Packets
*/


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16  Overwrite;
    char    SourceFilename[OS_MAX_PATH_LEN];
    char    TargetFilename[OS_MAX_PATH_LEN];

} FILECMD_Copy_t;
#define FILECMD_COPY_CMD_DATA_LEN  (sizeof(FILECMD_Copy_t) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    Filename[OS_MAX_PATH_LEN];
} FILECMD_Delete_t;
#define FILECMD_DELETE_CMD_DATA_LEN  (sizeof(FILECMD_Delete_t) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16  Overwrite;
    char    SourceFilename[OS_MAX_PATH_LEN];
    char    TargetFilename[OS_MAX_PATH_LEN];

} FILECMD_Move_t;
#define FILECMD_MOVE_CMD_DATA_LEN  (sizeof(FILECMD_Move_t) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    SourceFilename[OS_MAX_PATH_LEN];
    char    TargetFilename[OS_MAX_PATH_LEN];

} FILECMD_Rename_t;
#define FILECMD_RENAME_CMD_DATA_LEN  (sizeof(FILECMD_Rename_t) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr  */
    char    Source[OS_MAX_PATH_LEN];        /**< \brief Source filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename */

} FILECMD_DecompressCmd;
#define FILECMD_DECOMPRESS_CMD_DATA_LEN  (sizeof(FILECMD_DecompressCmd) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr    */
    char    Source1[OS_MAX_PATH_LEN];       /**< \brief Source 1 filename */
    char    Source2[OS_MAX_PATH_LEN];       /**< \brief Source 2 filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename   */

} FILECMD_ConcatCmd;
#define FILECMD_CONCAT_CMD_DATA_LEN  (sizeof(FILECMD_ConcatCmd) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Filename[OS_MAX_PATH_LEN];      /**< \brief Filename */
    uint32  FileInfoCRC;                    /**< \brief File info CRC method */

} FILECMD_GetInfoCmd;
#define FILECMD_GET_FILE_INFO_CMD_DATA_LEN  (sizeof(FILECMD_GetFileInfoCmd) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} FILECMD_SendOpenFilesPktCmd;
#define FILECMD_SEND_OPEN_FILES_PKT_CMD_DATA_LEN  (sizeof(FILECMD_SendOpenFilesPktCmd) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char    Filename[OS_MAX_PATH_LEN];
    uint32  Mode;                           /* Permissions, passed directly to OS_chmod */

} FILECMD_SetPermissions_t;
#define FILECMD_SET_PERMISSIONS_CMD_DATA_LEN  (sizeof(FILECMD_SetPermissions_t) - CFE_SB_CMD_HDR_SIZE)

/******************************************************************************
** Telmetery Packets
*/

/* Packet sent in response to FILECMD_GetInfoCmd */

typedef struct {

    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    uint8   FileStatus;                     /**< \fmtlmmnemonic \FM_FileStatus
                                                 \brief Status indicating whether the file is open or closed */
    uint8   CRC_Computed;                   /**< \fmtlmmnemonic \FM_ComputeCRC
                                                 \brief Flag indicating whether a CRC was computed or not */
    uint8   Spare[2];                       /**< \fmtlmmnemonic \FM_InfoPad
                                                 \brief Structure padding */
    uint32  CRC;                            /**< \fmtlmmnemonic \FM_CRC
                                                 \brief CRC value if computed */
    uint32  FileSize;                       /**< \fmtlmmnemonic \FM_InfoFileSize
                                                 \brief File Size */
    uint32  LastModifiedTime;               /**< \fmtlmmnemonic \FM_ModTime
                                                 \brief Last Modification Time of File */
    uint32  Mode;                           /**< \fmtlmmnemonic \FM_Mode
                                                 \brief Mode of the file (Permissions) */
    
    char    Filename[OS_MAX_PATH_LEN];      /**< \fmtlmmnemonic \FM_InfoFileName
                                                 \brief Name of File */
} FILECMD_GetInfoPkt;


/* Packet sent in response to FILECMD_SendOpenFilesPktCmd */

typedef struct {

    char    LogicalName[OS_MAX_PATH_LEN];   /**< \brief Logical filename */
    char    AppName[OS_MAX_API_NAME];       /**< \brief Application that opened file */

} FILECMD_SendOpenFilesPktEntry;


typedef struct {

    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    uint32  NumOpenFiles;                   /**< \fmtlmmnemonic \FM_TotalOpenFiles
                                                 \brief Number of files opened via cFE */
    FILECMD_SendOpenFilesPktEntry  OpenFilesList[OS_MAX_NUM_OPEN_FILES];  /**< \fmtlmmnemonic \FM_OpenFilesList
                                                 \brief List of files opened via cFE */
} FILECMD_SendOpenFilesPkt;


/******************************************************************************
** File Structure
*/


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: FILECMD_Constructor
**
** Initialize the example object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void FILECMD_Constructor(FILECMD_Class *FileCmdPtr);


/******************************************************************************
** Function: FILECMD_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void FILECMD_ResetStatus(void);


/******************************************************************************
** Function: FILECMD_Copy
**
*/
boolean FILECMD_Copy(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILECMD_Delete
**
*/
boolean FILECMD_Delete(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILECMD_Move
**
*/
boolean FILECMD_Move(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILECMD_Rename
**
*/
boolean FILECMD_Rename(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILECMD_SetPermissions
**
*/
boolean FILECMD_SetPermissions(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _filecmd_ */
