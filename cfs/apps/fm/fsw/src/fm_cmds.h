/*
** $Id: fm_cmds.h 1.4.1.2 2017/01/23 21:52:47EST sstrege Exp  $
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
** Title: CFS File Manager (FM) Application Ground Commands Header File
**
** Purpose: Specification for the CFS FM ground commands.
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
*/

#ifndef _fm_cmds_h_
#define _fm_cmds_h_

#include "cfe.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler function prototypes                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Move File Command Handler Function
**
**  \par Description
**       This function generates an event that displays the application version
**       numbers.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_NOOP_CC, #FM_Noop, #FM_NoopCmd_t
**/
boolean FM_NoopCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Reset Counters Command Handler Function
**
**  \par Description
**       This function resets the FM housekeeping telemetry counters.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_RESET_CC, #FM_Reset, #FM_ResetCmd_t
**/
boolean FM_ResetCountersCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Copy File Command Handler Function
**
**  \par Description
**       This function copies the command specified source file to the command
**       specified target file.
**
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       copying the file will be performed by a lower priority child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_COPY_CC, #FM_Copy, #FM_CopyFileCmd_t
**/
boolean FM_CopyFileCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Move File Command Handler Function
**
**  \par Description
**       This function moves the command specified source file to the command
**       specified target filename.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_MOVE_CC, #FM_Move, #FM_MoveFileCmd_t
**/
boolean FM_MoveFileCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Rename File Command Handler Function
**
**  \par Description
**       This function renames the command specified source file to the command
**       specified target filename.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_RENAME_CC, #FM_Rename, #FM_RenameFileCmd_t
**/
boolean FM_RenameFileCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Delete File Command Handler Function
**
**  \par Description
**       This function deletes the command specified file.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_DELETE_CC, #FM_Delete, #FM_DeleteFileCmd_t
**/
boolean FM_DeleteFileCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Delete All Files Command Handler Function
**
**  \par Description
**       This function deletes all files from the command specified directory.
**
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       reading the directory and deleting each file will be performed by a
**       lower priority child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_DELETE_ALL_CC, #FM_DeleteAll, #FM_DeleteAllCmd_t
**/
boolean FM_DeleteAllFilesCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Decompress Files Command Handler Function
**
**  \par Description
**       This function decompresses the command specified source file into the
**       command specified target file.
**
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       decompressing the source file into the target file will be performed by
**       a lower priority child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_DECOMPRESS_CC, #FM_Decompress, #FM_DecompressCmd_t
**/
boolean FM_DecompressFileCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Concatenate Files Command Handler Function
**
**  \par Description
**       This function concatenates two command specified source files into the
**       command specified target file.
**
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       copying the first source file to the target file and then appending the
**       second source file to the target file will be performed by a lower priority
**       child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_CONCAT_CC, #FM_Concat, #FM_ConcatCmd_t
**/
boolean FM_ConcatFilesCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Get File Information Command Handler Function
**
**  \par Description
**       This function creates a telemetry packet and populates the packet with
**       the current information regarding the command specified file.
**
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       collecting the status data and calculating the CRC will be performed by
**       a lower priority child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_GET_FILE_INFO_CC, #FM_GetFileInfo, #FM_GetFileInfoCmd_t, #FM_FileInfoPkt_t
**/
boolean FM_GetFileInfoCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Get Open Files List Command Handler Function
**
**  \par Description
**       This function creates a telemetry packet and populates it with a list of
**       the current open files.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_GET_OPEN_FILES_CC, #FM_GetOpenFiles, #FM_GetOpenFilesCmd_t, #FM_OpenFilesPkt_t
**/
boolean FM_GetOpenFilesCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Create Directory Command Handler Function
**
**  \par Description
**       This function creates the command specified directory.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_CREATE_DIR_CC, #FM_CreateDir, #FM_CreateDirCmd_t
**/
boolean FM_CreateDirectoryCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Delete Directory Command Handler Function
**
**  \par Description
**       This function deletes the command specified directory.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_DELETE_DIR_CC, #FM_DeleteDir, #FM_DeleteDirCmd_t
**/
boolean FM_DeleteDirectoryCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Get Directory List to Packet Command Handler Function
**
**  \par Description
**       This function creates an output file and writes a listing of the command
**       specified directory to the file.
**
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       reading the directory will be performed by a lower priority child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_GET_DIR_FILE_CC, #FM_GetDirFile, #FM_GetDirFileCmd_t,
        #FM_DirListFileStats_t, FM_DirListEntry_t
**/
boolean FM_GetDirListFileCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Get Directory List to Packet Command Handler Function
**
**  \par Description
**       This function creates a telemetry packet and populates the packet with
**       the directory listing data for the command specified directory, starting
**       at the command specified directory entry.
**
**       Because of the possibility that this command might take a very long time
**       to complete, command argument validation will be done immediately but
**       reading the directory will be performed by a lower priority child task.
**       As such, the return value for this function only refers to the result
**       of command argument verification and being able to place the command on
**       the child task interface queue.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_GET_DIR_PKT_CC, #FM_GetDirPkt, #FM_GetDirPktCmd_t, #FM_DirListPkt_t
**/
boolean FM_GetDirListPktCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Get Free Space Command Handler Function
**
**  \par Description
**       This function creates a telemetry packet and populates the packet with
**       free space data for each file system listed in the FM File System Free
**       Space Table.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_GET_FREE_SPACE_CC, #FM_GetFreeSpace, #FM_GetFreeSpaceCmd_t, #FM_FreeSpacePkt_t
**/
boolean FM_GetFreeSpaceCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Set Table Entry State Command Handler Function
**
**  \par Description
**       This function modifies the enable/disable state for a single entry in
**       the File System Free Space Table.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_SET_TABLE_STATE_CC, #FM_SetTableState, #FM_SetTableStateCmd_t, #FM_TableEntry_t
**/
boolean FM_SetTableStateCmd(CFE_SB_MsgPtr_t MessagePtr);


/**
**  \brief Set File Permissions of a file
**
**  \par Description
**       This function is a direct call to OS_chmod to set the file access
**
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  MessagePtr - Pointer to Software Bus command packet.
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates command success  \endcode
**  \endreturns
**
**  \sa #FM_SET_PERM_CC, #FM_SetPermCmd_t, #FM_SET_PERM_CMD_EID, #FM_SET_PERM_ERR_EID
**/
boolean FM_SetPermissionsCmd(CFE_SB_MsgPtr_t MessagePtr);

#endif /* _fm_cmds_h_ */

/************************/
/*  End of File Comment */
/************************/

