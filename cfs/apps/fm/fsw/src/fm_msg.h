/*
** $Id: fm_msg.h 1.7.1.3 2017/01/23 21:52:57EST sstrege Exp  $
**
**  Copyright (c) 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Title: CFS File Manager (FM) Application Command and Telemetry
**        Packet Header File
**
** Purpose: Specification for the CFS FM command and telemetry messages.
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
*/

#ifndef _fm_msg_h_
#define _fm_msg_h_

#include "cfe.h"
#include "fm_platform_cfg.h"
#include "fm_defs.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- command packet structures                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
**  \brief Housekeeping Request command packet structure
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} FM_HousekeepingCmd_t;


/*
**  \brief No-Operation command packet structure
**
**  For command details see #FM_NOOP_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} FM_NoopCmd_t;


/*
**  \brief Reset Counters command packet structure
**
**  For command details see #FM_RESET_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} FM_ResetCmd_t;


/*
**  \brief Copy File command packet structure
**
**  For command details see #FM_COPY_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    uint16  Overwrite;                      /**< \brief Allow overwrite */
    char    Source[OS_MAX_PATH_LEN];        /**< \brief Source filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename */

} FM_CopyFileCmd_t;


/*
**  \brief Move File command packet structure
**
**  For command details see #FM_MOVE_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    uint16  Overwrite;                      /**< \brief Allow overwrite */
    char    Source[OS_MAX_PATH_LEN];        /**< \brief Source filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename */

} FM_MoveFileCmd_t;


/*
**  \brief Rename File command packet structure
**
**  For command details see #FM_RENAME_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Source[OS_MAX_PATH_LEN];        /**< \brief Source filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename */

} FM_RenameFileCmd_t;


/*
**  \brief Delete File command packet structure
**
**  For command details see #FM_DELETE_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Filename[OS_MAX_PATH_LEN];      /**< \brief Delete filename */

} FM_DeleteFileCmd_t;


/*
**  \brief Delete All command packet structure
**
**  For command details see #FM_DELETE_ALL_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Directory[OS_MAX_PATH_LEN];     /**< \brief Directory name */

} FM_DeleteAllCmd_t;


/*
**  \brief Decompress File command packet structure
**
**  For command details see #FM_DECOMPRESS_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Source[OS_MAX_PATH_LEN];        /**< \brief Source filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename */

} FM_DecompressCmd_t;


/*
**  \brief Concatenate Files command packet structure
**
**  For command details see #FM_CONCAT_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Source1[OS_MAX_PATH_LEN];       /**< \brief Source 1 filename */
    char    Source2[OS_MAX_PATH_LEN];       /**< \brief Source 2 filename */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename */

} FM_ConcatCmd_t;


/*
**  \brief Get File Info command packet structure
**
**  For command details see #FM_GET_FILE_INFO_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Filename[OS_MAX_PATH_LEN];      /**< \brief Filename */
    uint32  FileInfoCRC;                    /**< \brief File info CRC method */

} FM_GetFileInfoCmd_t;


/*
**  \brief Get Open Files command packet structure
**
**  For command details see #FM_GET_OPEN_FILES_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} FM_GetOpenFilesCmd_t;


/*
**  \brief Create Directory command packet structure
**
**  For command details see #FM_CREATE_DIR_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Directory[OS_MAX_PATH_LEN];     /**< \brief Directory name */

} FM_CreateDirCmd_t;


/*
**  \brief Delete Directory command packet structure
**
**  For command details see #FM_DELETE_DIR_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Directory[OS_MAX_PATH_LEN];     /**< \brief Directory name */

} FM_DeleteDirCmd_t;


/*
**  \brief Get DIR List to File command packet structure
**
**  For command details see #FM_GET_DIR_FILE_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Directory[OS_MAX_PATH_LEN];     /**< \brief Directory name */
    char    Filename[OS_MAX_PATH_LEN];      /**< \brief Filename */
    uint8   GetSizeTimeMode;                /**< \brief Option to query size, time, and mode of files (CPU intensive) */
    uint8   Spare01[3];                     /**< \brief Padding to 32 bit boundary */
 
} FM_GetDirFileCmd_t;


/*
**  \brief Get DIR List to Packet command packet structure
**
**  For command details see #FM_GET_DIR_PKT_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    Directory[OS_MAX_PATH_LEN];     /**< \brief Directory name */
    uint32  DirListOffset;                  /**< \brief Index of 1st dir entry to put in packet */
    uint8   GetSizeTimeMode;                /**< \brief Option to query size, time, and mode of files (CPU intensive) */
    uint8   Spare01[3];                     /**< \brief Padding to 32 bit boundary */

} FM_GetDirPktCmd_t;


/*
**  \brief Get Free Space command packet structure
**
**  For command details see #FM_GET_FREE_SPACE_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */

} FM_GetFreeSpaceCmd_t;


/*
**  \brief Set Table State command packet structure
**
**  For command details see #FM_SET_TABLE_STATE_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    uint32  TableEntryIndex;                /**< \brief Table entry index */
    uint32  TableEntryState;                /**< \brief New table entry state */

} FM_SetTableStateCmd_t;


/*
**  \brief Set Permissions for a file
**
**  For command details see #FM_SET_PERM_CC
*/
typedef struct
{
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE SB cmd hdr */
    char    FileName[OS_MAX_PATH_LEN];      /**< \brief File name of the permissions to set */
    uint32  Mode;                           /**< \brief Permissions, passed directly to OS_chmod */

} FM_SetPermCmd_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- get directory listing telemetry structures                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \fmtlm Get Directory Listing entry structure
**/
typedef struct
{
    char    EntryName[OS_MAX_PATH_LEN];     /**< \fmtlmmnemonic \FM_DLFileName
                                                 \brief Directory Listing Filename */
    uint32  EntrySize;                      /**< \fmtlmmnemonic \FM_DLFileSize
                                                 \brief Directory Listing File Size */
    uint32  ModifyTime;                     /**< \fmtlmmnemonic \FM_DLModTime
                                                 \brief Directory Listing File Last Modification Times */
    uint32  Mode;                           /**< \fmtlmmnemonic \FM_DLMode
                                                 \brief Mode of the file (Permissions) */
} FM_DirListEntry_t;

/**
**  \fmtlm Get Directory Listing telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    char    DirName[OS_MAX_PATH_LEN];       /**< \fmtlmmnemonic \FM_DirName
                                                 \brief Directory Name */
    uint32  TotalFiles;                     /**< \fmtlmmnemonic \FM_TotalFiles
                                                 \brief Number of files in the directory */
    uint32  PacketFiles;                    /**< \fmtlmmnemonic \FM_PacketFiles
                                                 \brief Number of files in this packet */
    uint32  FirstFile;                      /**< \fmtlmmnemonic \FM_FirstFile
                                                 \brief Index into directory files of first packet file */
    FM_DirListEntry_t  FileList[FM_DIR_LIST_PKT_ENTRIES];  /**< \fmtlmmnemonic \FM_DLFileList
                                                 \brief Directory listing file data */
} FM_DirListPkt_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- get directory listing to file structures                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Get Directory Listing file statistics structure
**/
typedef struct
{
    char    DirName[OS_MAX_PATH_LEN];       /**< \brief Directory name */
    uint32  DirEntries;                     /**< \brief Number of entries in the directory */
    uint32  FileEntries;                    /**< \brief Number of entries written to output file */

} FM_DirListFileStats_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- get file information telemetry structure                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \fmtlm Get File Info telemetry packet
**/
typedef struct
{
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
} FM_FileInfoPkt_t;



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- get open files list telemetry structures                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Get Open Files list entry structure
**/
typedef struct
{
	char    LogicalName[OS_MAX_PATH_LEN];   /**< \brief Logical filename */
    char    AppName[OS_MAX_API_NAME];       /**< \brief Application that opened file */

} FM_OpenFilesEntry_t;


/**
**  \fmtlm Get Open Files telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    uint32  NumOpenFiles;                   /**< \fmtlmmnemonic \FM_TotalOpenFiles
                                                 \brief Number of files opened via cFE */
    FM_OpenFilesEntry_t  OpenFilesList[OS_MAX_NUM_OPEN_FILES];  /**< \fmtlmmnemonic \FM_OpenFilesList
                                                 \brief List of files opened via cFE */
} FM_OpenFilesPkt_t;



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- get file system free space telemetry structures           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Get Free Space list entry structure
**/
typedef struct
{
    uint32  FreeSpace_A;                    /**< \fmtlmmnemonic \FM_FreeSpace_A
                                                 \brief First 32 bit portion of a 64 bit value */
    uint32  FreeSpace_B;                    /**< \fmtlmmnemonic \FM_FreeSpace_B
                                                 \brief Second 32 bit portion of a 64 bit value */
    char    Name[OS_MAX_PATH_LEN];          /**< \fmtlmmnemonic \FM_PktFsName
                                                 \brief File system name */
} FM_FreeSpacePktEntry_t;


/**
**  \fmtlm Get Free Space telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    FM_FreeSpacePktEntry_t  FileSys[FM_TABLE_ENTRY_COUNT]; /**< \fmtlmmnemonic \FM_PktFsList
                                                 \brief Array of file system free space entries */
} FM_FreeSpacePkt_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- housekeeping telemetry structure                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
** \fmtlm Housekeeping telemetry packet
**/
typedef struct
{
    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE]; /**< \brief cFE SB tlm hdr */

    uint8   CommandCounter;		            /**< \fmtlmmnemonic \FM_CMDPC
                                                 \brief Application command counter */
    uint8   CommandErrCounter;			    /**< \fmtlmmnemonic \FM_CMDEC
                                                 \brief Application command error counter */
    uint8   Spare;                          /**< \brief Placeholder for unused command warning counter */

    uint8   NumOpenFiles;                   /**< \fmtlmmnemonic \FM_NumOpen
                                                 \brief Number of open files in the system */

    uint8   ChildCmdCounter;                /**< \fmtlmmnemonic \FM_ChildCMDPC
                                                 \brief Child task command counter */
    uint8   ChildCmdErrCounter;             /**< \fmtlmmnemonic \FM_ChildCMDEC
                                                 \brief Child task command error counter */
    uint8   ChildCmdWarnCounter;            /**< \fmtlmmnemonic \FM_ChildWarnCtr
                                                 \brief Child task command warning counter */

    uint8   ChildQueueCount;                /**< \fmtlmmnemonic \FM_ChildQueueCount
                                                 \brief Number of pending commands in queue */

    uint8   ChildCurrentCC;                 /**< \fmtlmmnemonic \FM_ChildCurrCC
                                                 \brief Command code currently executing */
    uint8   ChildPreviousCC;                /**< \fmtlmmnemonic \FM_ChildPrevCC
                                                 \brief Command code previously executed */

} FM_HousekeepingPkt_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- file system free space table structures                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Get Free Space table entry
**/
typedef struct
{
    uint32  State;                          /**< \brief Table entry enable/disable state */
    char    Name[OS_MAX_PATH_LEN];          /**< \brief File system name = string */

} FM_TableEntry_t;


/**
**  \brief Get Free Space table definition
**/
typedef struct
{
    FM_TableEntry_t FileSys[FM_TABLE_ENTRY_COUNT]; /**< \brief One entry for each file system */

} FM_FreeSpaceTable_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- child task interface command queue entry                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Child Task Interface command queue entry structure
**/
typedef struct
{
    uint32  CommandCode;                    /**< \brief Command code - identifies the command */
    uint32  DirListOffset;                  /**< \brief Starting entry for dir list commands */
    uint32  FileInfoState;                  /**< \brief File info state */
    uint32  FileInfoSize;                   /**< \brief File info size */
    uint32  FileInfoTime;                   /**< \brief File info time */
    uint32  FileInfoCRC;                    /**< \brief File info CRC method */
    char    Source1[OS_MAX_PATH_LEN];       /**< \brief First source file or directory name command argument */
    char    Source2[OS_MAX_PATH_LEN];       /**< \brief Second source filename command argument */
    char    Target[OS_MAX_PATH_LEN];        /**< \brief Target filename command argument */
    uint8   GetSizeTimeMode;                /**< \brief Whether to invoke stat call for size and time (CPU intensive) */
    uint32  Mode;                           /**< \brief File Mode */
} FM_ChildQueueEntry_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- application global data structure                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \fmtlm Application global data structure
**/
typedef struct
{
    FM_FreeSpaceTable_t *FreeSpaceTablePtr; /**< \brief File System Table Pointer */
    CFE_TBL_Handle_t FreeSpaceTableHandle;  /**< \brief File System Table Handle */

    CFE_SB_PipeId_t  CmdPipe;               /**< \brief cFE software bus command pipe */

    uint32  ChildTaskID;                    /**< \brief Child task ID */
    uint32  ChildSemaphore;                 /**< \brief Child task wakeup counting semaphore */
    uint32  ChildQueueCountSem;             /**< \brief Child queue counter mutex semaphore */

    uint8   ChildCmdCounter;                /**< \brief Child task command success counter */
    uint8   ChildCmdErrCounter;             /**< \brief Child task command error counter */
    uint8   ChildCmdWarnCounter;            /**< \brief Child task command warning counter */

    uint8   ChildWriteIndex;                /**< \brief Array index for next write to command args */
    uint8   ChildReadIndex;                 /**< \brief Array index for next read from command args */
    uint8   ChildQueueCount;                /**< \brief Number of pending commands in queue */

    uint8   CommandCounter;                 /**< \brief Application command success counter */
    uint8   CommandErrCounter;	            /**< \brief Application command error counter */
    uint8   Spare8a;                        /**< \brief Placeholder for unused command warning counter */

    uint8   ChildCurrentCC;                 /**< \brief Command code currently executing */
    uint8   ChildPreviousCC;                /**< \brief Command code previously executed */
    uint8   Spare8b;                        /**< \brief Structure alignment spare */

    uint32  FileStatTime;                   /**< \brief Modify time from most recent OS_stat */
    uint32  FileStatSize;                   /**< \brief File size from most recent OS_stat */

    FM_DirListFileStats_t DirListFileStats; /**< \brief Get dir list to file statistics structure */

    FM_DirListPkt_t DirListPkt;             /**< \brief Get dir list to packet telemetry packet */

    FM_FreeSpacePkt_t FreeSpacePkt;         /**< \brief Get free space telemetry packet */

    FM_FileInfoPkt_t FileInfoPkt;           /**< \brief Get file info telemetry packet */

    FM_OpenFilesPkt_t OpenFilesPkt;         /**< \brief Get open files telemetry packet */

    FM_HousekeepingPkt_t HousekeepingPkt;   /**< \brief Application housekeeping telemetry packet */

    char    ChildBuffer[FM_CHILD_FILE_BLOCK_SIZE]; /**< \brief Child task file I/O buffer */

    FM_ChildQueueEntry_t ChildQueue[FM_CHILD_QUEUE_DEPTH];  /**< \brief Child task command queue */

} FM_GlobalData_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM -- access to global data structure (defined in fm_app.c)     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern FM_GlobalData_t    FM_GlobalData;


#endif /* _fm_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
