/*
** $Id: fm_cmd_utils.h 1.2.1.2 2017/01/23 21:53:15EST sstrege Exp  $
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
** Title: File Manager (FM) Application Command Utility Function
**        Header File
**
** Purpose: Specification for the CFS File Manager command utility
**          functions.
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
*/

#ifndef _fm_cmd_utils_h_
#define _fm_cmd_utils_h_

#include "cfe.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM command handler utility function prototypes                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Verify Command Packet Length Function
**
**  \par Description
**       This function is invoked from each of the command handlers to verify the
**       length of the command packet.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  CmdPacket - Pointer to a CFE Software Bus command packet
**  \param [in]  ExpectedLength - Expected packet length (command specific)
**  \param [in]  EventID - Error event ID (command specific)
**  \param [in]  CmdText - Error event text (command specific)
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates a valid packet length  \endcode
**  \endreturns
**
**  \sa #CFE_SB_GetTotalMsgLength
**/
boolean FM_IsValidCmdPktLength(CFE_SB_MsgPtr_t CmdPacket, uint16 ExpectedLength, uint32 EventID, char *CmdText);


/**
**  \brief Verify Target File Overwrite Function
**
**  \par Description
**       This function is invoked from the copy and move file command handlers
**       to verify the target file overwrite argument.  Acceptable values are
**       TRUE (one) and FALSE (zero).
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Overwrite - Value being tested
**  \param [in]  EventID - Error event ID (command specific)
**  \param [in]  CmdText - Error event text (command specific)
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates a valid overwrite argument  \endcode
**  \endreturns
**
**  \sa #FM_COPY_CC, #FM_MOVE_CC
**/
boolean FM_VerifyOverwrite(uint16 Overwrite, uint32 EventID, char *CmdText);


/**
**  \brief Get Open Files Data Function
**
**  \par Description
**       This function creates a list of open files and returns
**       the number of open files.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  OpenFilesData - pointer to open files data
**
**  \returns
**  \retstmt The number of open files. \endcode
**  \endreturns
**
**  \sa #OS_FDGetInfo
**/
uint32 FM_GetOpenFilesData(FM_OpenFilesEntry_t *OpenFilesData);


/**
**  \brief Get Filename State Function
**
**  \par Description
**       This function performs a series of tests on the input filename to
**       determine first whether the name contains valid characters for use
**       as a filename, and then whether the filename is currently in use by
**       the file system as a file or directory.
**       If the caller is the Get File Info command handler, the function
**       also stores the file size and last modified timestamp.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Filename - Pointer to buffer containing filename
**  \param [in]  BufferSize - Size of filename character buffer
**  \param [in]  FileInfoCmd - Boolean: Is this for the Get File Info command?
**
**  \returns
**  \retcode #FM_NAME_IS_INVALID      \retdesc \copydoc FM_NAME_IS_INVALID       \endcode
**  \retcode #FM_NAME_IS_NOT_IN_USE   \retdesc \copydoc FM_NAME_IS_NOT_IN_USE    \endcode
**  \retcode #FM_NAME_IS_FILE_OPEN    \retdesc \copydoc FM_NAME_IS_FILE_OPEN     \endcode
**  \retcode #FM_NAME_IS_FILE_CLOSED  \retdesc \copydoc FM_NAME_IS_FILE_CLOSED   \endcode
**  \retcode #FM_NAME_IS_DIRECTORY    \retdesc \copydoc FM_NAME_IS_DIRECTORY     \endcode
**  \endreturns
**
**  \sa #CFS_IsValidFilename, #OS_stat, #OS_FDGetInfo
**/
uint32 FM_GetFilenameState(char *Filename, uint32 BufferSize, boolean FileInfoCmd);


/**
**  \brief Verify Name Function
**
**  \par Description
**       This function calls the Get Filename State function and generates
**       an error event if the state is invalid.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Name - Pointer to buffer containing name
**  \param [in]  BufferSize - Size of name character buffer
**  \param [in]  EventID - Error event ID (command specific)
**  \param [in]  CmdText - Error event text (command specific)
**
**  \returns
**  \retcode #FM_NAME_IS_INVALID      \retdesc \copydoc FM_NAME_IS_INVALID       \endcode
**  \retcode #FM_NAME_IS_NOT_IN_USE   \retdesc \copydoc FM_NAME_IS_NOT_IN_USE    \endcode
**  \retcode #FM_NAME_IS_FILE_OPEN    \retdesc \copydoc FM_NAME_IS_FILE_OPEN     \endcode
**  \retcode #FM_NAME_IS_FILE_CLOSED  \retdesc \copydoc FM_NAME_IS_FILE_CLOSED   \endcode
**  \retcode #FM_NAME_IS_DIRECTORY    \retdesc \copydoc FM_NAME_IS_DIRECTORY     \endcode
**  \endreturns
**
**  \sa #FM_GetFilenameState
**/
uint32 FM_VerifyNameValid(char *Name, uint32 BufferSize, uint32 EventID, char *CmdText);


/**
**  \brief Verify File is Closed Function
**
**  \par Description
**       This function calls the Get Filename State function and generates
**       an error event if the state is anything other than a closed file.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Filename - Pointer to buffer containing filename
**  \param [in]  BufferSize - Size of filename character buffer
**  \param [in]  EventID - Error event ID (command specific)
**  \param [in]  CmdText - Error event text (command specific)
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates that filename is an open file  \endcode
**  \endreturns
**
**  \sa #FM_GetFilenameState
**/
boolean FM_VerifyFileClosed(char *Filename, uint32 BufferSize, uint32 EventID, char *CmdText);


/**
**  \brief Verify File Exists Function
**
**  \par Description
**       This function calls the Get Filename State function and generates
**       an error event if the state is anything other than an open file or
**       a closed file.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Filename - Pointer to buffer containing filename
**  \param [in]  BufferSize - Size of filename character buffer
**  \param [in]  EventID - Error event ID (command specific)
**  \param [in]  CmdText - Error event text (command specific)
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates filename is an existing file  \endcode
**  \endreturns
**
**  \sa #FM_GetFilenameState
**/
boolean FM_VerifyFileExists(char *Filename, uint32 BufferSize, uint32 EventID, char *CmdText);


/**
**  \brief Verify File Does Not Exist Function
**
**  \par Description
**       This function calls the Get Filename State function and generates
**       an error event if the state is anything other than the name is
**       unused - the name is not a file and is not a directory.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Name - Pointer to buffer containing name
**  \param [in]  BufferSize - Size of name character buffer
**  \param [in]  EventID - Error event ID (command specific)
**  \param [in]  CmdText - Error event text (command specific)
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates the name is unused  \endcode
**  \endreturns
**
**  \sa #FM_GetFilenameState
**/
boolean FM_VerifyFileNoExist(char *Name, uint32 BufferSize, uint32 EventID, char *CmdText);


/**
**  \brief Verify File Is Not Open Function
**
**  \par Description
**       This function calls the Get Filename State function and generates
**       an error event if the state is a directory or an open file.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Name - Pointer to buffer containing name
**  \param [in]  BufferSize - Size of name character buffer
**  \param [in]  EventID - Error event ID (command specific)
**  \param [in]  CmdText - Error event text (command specific)
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates the name is unused or a closed file  \endcode
**  \endreturns
**
**  \sa #FM_GetFilenameState
**/
boolean FM_VerifyFileNotOpen(char *Name, uint32 BufferSize, uint32 EventID, char *CmdText);


/**
**  \brief Verify Directory Exists Function
**
**  \par Description
**       This function calls the Get Filename State function and generates
**       an error event if the state is not an existing directory.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Directory - Pointer to buffer containing directory name
**  \param [in]  BufferSize - Size of directory name character buffer
**  \param [in]  EventID - Error event ID (command specific)
**  \param [in]  CmdText - Error event text (command specific)
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates the directory exists  \endcode
**  \endreturns
**
**  \sa #FM_GetFilenameState
**/
boolean FM_VerifyDirExists(char *Directory, uint32 BufferSize, uint32 EventID, char *CmdText);


/**
**  \brief Verify Directory Does Not Exist Function
**
**  \par Description
**       This function calls the Get Filename State function and generates
**       an error event if the state is an existing file or directory.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Directory - Pointer to buffer containing directory name
**  \param [in]  BufferSize - Size of directory name character buffer
**  \param [in]  EventID - Error event ID (command specific)
**  \param [in]  CmdText - Error event text (command specific)
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates the name is unused  \endcode
**  \endreturns
**
**  \sa #FM_GetFilenameState
**/
boolean FM_VerifyDirNoExist(char *Name, uint32 BufferSize, uint32 EventID, char *CmdText);


/**
**  \brief Verify Child Task Interface Function
**
**  \par Description
**       This function verifies that the child task interface queue is
**       not full and that the queue index values are within bounds.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  EventID - Error event ID (command specific)
**  \param [in]  CmdText - Error event text (command specific)
**
**  \returns
**  \retcode #TRUE   \retdesc \copydoc TRUE    \endcode
**  \retcode #FALSE  \retdesc \copydoc FALSE   \endcode
**  \retstmt Boolean TRUE indicates the child task queue is available  \endcode
**  \endreturns
**/
boolean FM_VerifyChildTask(uint32 EventID, char *CmdText);


/**
**  \brief Invoke Child Task Function
**
**  \par Description
**       This function is called after the caller has loaded the next
**       available entry in the child task queue with the arguments for
**       the current command.  The function updates the queue access
**       index and then verifies that the Child Task is operational.
**       If the Child Task is operational then it is signaled via
**       handshake semaphore to process the next command from the queue.
**       If instead, the Child Task is not operational, the Child Task
**       queue processor function is called directly to process the
**       current command.  The difference between the two methods is in
**       which execution thread is active when the command is processed.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  (none)
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #OS_CountSemGive, #FM_ChildProcess
**/
void FM_InvokeChildTask(void);


/**
**  \brief Append Path Separator Function
**
**  \par Description
**       This function appends a path separator character (slash) to
**       a directory name in advance of combining the directory name
**       with directory entry names to create qualified filenames.  The
**       function will only append the separator character if there is
**       room in the buffer for another character.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  Directory - Pointer to buffer containing directory name
**  \param [in]  BufferSize - Size of directory name character buffer
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #FM_GetFilenameState
**/
void FM_AppendPathSep(char *Directory, uint32 BufferSize);


#endif /* _fm_cmd_utils_h_ */

/************************/
/*  End of File Comment */
/************************/
