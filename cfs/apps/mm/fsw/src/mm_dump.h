/*************************************************************************
** File:
**   $Id: mm_dump.h 1.3 2016/10/28 17:49:18EDT mdeschu Exp  $
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** Purpose: 
**   Specification for the CFS Memory Manager memory dump ground commands.
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**
**   $Log: mm_dump.h  $
**   Revision 1.3 2016/10/28 17:49:18EDT mdeschu 
**   Trac #32: Fix MM SendEvent buffer overrun
**   Revision 1.2 2015/12/29 15:22:20EST czogby 
**   Move function prototypes from .c files into .h files
**   Revision 1.1 2015/07/28 12:21:40EDT rperera 
**   Initial revision
**   Member added to project /CFS-APPs-PROJECT/mm/fsw/src/project.pj
**   Revision 1.4 2015/03/02 14:26:51EST sstrege 
**   Added copyright information
**   Revision 1.3 2008/05/19 15:23:03EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/
#ifndef _mm_dump_
#define _mm_dump_

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "mm_msg.h"
#include "mm_filedefs.h"

/************************************************************************
** Macro Definitions
*************************************************************************/
/*
** This macro defines the maximum number of bytes that can be dumped
** in an event message string based upon the setting of the 
** CFE_EVS_MAX_MESSAGE_LENGTH configuration parameter.
**
** The event message format is:
**    Message head "Memory Dump: "             13 characters
**    Message body "0xFF "                      5 characters per dump byte
**    Message tail "from address: 0xFFFFFFFF"  33 characters including NUL on 64-bit system
*/
/**
** \name Maximum dump bytes in an event string */ 
/** \{ */
#define MM_MAX_DUMP_INEVENT_BYTES  ((CFE_EVS_MAX_MESSAGE_LENGTH - (13 + 33)) / 5)
/** \} */

/*
** This macro defines the size of the scratch buffer used to build
** the dump in event message string. Set it to the size of the
** largest piece shown above including room for a NUL terminator. 
*/
/**
** \name Dump in an event scratch string size */ 
/** \{ */
#define MM_DUMPINEVENT_TEMP_CHARS    36
/** \} */

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Memory peek
**  
**  \par Description
**       Support function for #MM_PeekCmd. This routine will read 
**       8, 16, or 32 bits of data and send it in an event message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   CmdPtr       A #MM_PeekCmd_t pointer to the peek 
**                             command message
**
**  \param [in]   SrcAddress   The source address for the peek operation 
** 
*************************************************************************/
void MM_PeekMem (MM_PeekCmd_t *CmdPtr, 
                 uint32       SrcAddress);

/************************************************************************/
/** \brief Memory dump to file
**  
**  \par Description
**       Support function for #MM_DumpMemToFileCmd. This routine will 
**       read an address range and store the data in a file.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   FileHandle   The open file handle of the dump file  
**
**  \param [in]   FileName     A pointer to a character string holding  
**                             the dump file name
**
**  \param [in]   FileHeader   A #MM_LoadDumpFileHeader_t pointer to  
**                             the dump file header structure initialized
**                             with data based upon the command message 
**                             parameters
**
**  \returns
**  \retstmt Returns TRUE if the dump completed successfully  \endcode
**  \retstmt Returns FALSE if the dump failed due to an error \endcode
**  \endreturns
** 
*************************************************************************/
boolean MM_DumpMemToFile(uint32                   FileHandle, 
                         char                     *FileName, 
                         MM_LoadDumpFileHeader_t  *FileHeader);

/************************************************************************/
/** \brief Verify memory dump to file parameters
**  
**  \par Description
**       This routine will run various checks on the source address, 
**       memory type, and data size (in bytes) for a dump memory to
**       file command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   Address       The source address for the requested 
**                              dump operation 
**
**  \param [in]   MemType       The source memory type for the requested 
**                              dump operation  
**
**  \param [in]   SizeInBytes   The number of bytes for the requested 
**                              dump operation 
**
**  \returns
**  \retstmt Returns TRUE if all the parameter checks passed  \endcode
**  \retstmt Returns FALSE any parameter check failed         \endcode
**  \endreturns
**
*************************************************************************/
boolean MM_VerifyFileDumpParams(uint32 Address, 
                                uint8  MemType, 
                                uint32 SizeInBytes);

/************************************************************************/
/** \brief Write the cFE primary and and MM secondary file headers
**  
**  \par Description
**       Support function for #MM_DumpMemToFileCmd. This routine will 
**       write the cFE primary and MM secondary headers to the
**       file specified by the FileHandle.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   FileName     A pointer to a character string holding  
**                             the file name (used only for error event
**                             messages).
**
**  \param [in]   FileHandle   File Descriptor obtained from a previous
**                             call to #OS_open that is associated with
**                             the file whose headers are to be written.
**
**  \param [in]   CFEHeader    A #CFE_FS_Header_t pointer to the
**                             cFE primary file header structure to be
**                             written.
**
**  \param [in]   MMHeader     A #MM_LoadDumpFileHeader_t pointer to 
**                             the MM secondary file header structure
**                             to be written.
**
**  \returns
**  \retstmt Returns TRUE if the headers were written successfully \endcode
**  \retstmt Returns FALSE if a write error occurred \endcode
**  \endreturns
** 
*************************************************************************/
boolean MM_WriteFileHeaders(char                    *FileName,
                            int32                    FileHandle,
                            CFE_FS_Header_t         *CFEHeader,
                            MM_LoadDumpFileHeader_t *MMHeader);

/************************************************************************/
/** \brief Verify memory dump in event message parameters
**  
**  \par Description
**       This routine will run various checks on the source address, 
**       memory type, and data size (in bytes) for a dump memory in
**       event message command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   Address       The source address for the requested 
**                              dump operation 
**
**  \param [in]   MemType       The source memory type for the requested 
**                              dump operation  
**
**  \param [in]   SizeInBytes   The number of bytes for the requested 
**                              dump operation 
**
**  \returns
**  \retstmt Returns TRUE if all the parameter checks passed  \endcode
**  \retstmt Returns FALSE any parameter check failed         \endcode
**  \endreturns
**
*************************************************************************/
boolean MM_VerifyDumpInEventParams(uint32 Address, 
                                   uint8  MemType, 
                                   uint32 SizeInBytes);

/************************************************************************/
/** \brief Fill dump memory in event message buffer
**  
**  \par Description
**       Support function for #MM_DumpInEventCmd. This routine will 
**       read an address range and store the data in a byte array.
**       It will properly adjust for optional memory types that may
**       require 16 or 32 bit reads.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   SrcAddress   The source address to read from 
**
**  \param [in]   CmdPtr       A #MM_DumpInEventCmd_t pointer to the  
**                             dump in event command message
**
**  \param [in]   DumpBuffer   A pointer to the byte array to store
**                             the dump data in
**
**  \param [out]  *DumpBuffer  A pointer to the byte array holding the
**                             dump data
**
**  \returns
**  \retstmt Returns TRUE if all PSP memory access functions succeed  \endcode
**  \retstmt Returns FALSE if any PSP memory access function fails    \endcode
**  \endreturns
**
*************************************************************************/
boolean MM_FillDumpInEventBuffer(uint32              SrcAddress, 
                                 MM_DumpInEventCmd_t *CmdPtr, 
                                 uint8               *DumpBuffer);

/************************************************************************/
/** \brief Process memory peek command
**  
**  \par Description
**       Processes the memory peek command that will read a memory
**       location and report the data in an event message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_PEEK_CC
**
*************************************************************************/
void MM_PeekCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process memory dump to file command
**  
**  \par Description
**       Processes the memory dump to file command that will read a 
**       address range of memory and store the data in a command
**       specified file.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_DUMP_MEM_TO_FILE_CC
**
*************************************************************************/
void MM_DumpMemToFileCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process memory dump in event command
**  
**  \par Description
**       Processes the memory dump in event command that will read  
**       up to #MM_MAX_DUMP_INEVENT_BYTES from memory and report
**       the data in an event message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_DUMP_IN_EVENT_CC, #MM_MAX_DUMP_INEVENT_BYTES
**
*************************************************************************/
void MM_DumpInEventCmd(CFE_SB_MsgPtr_t MessagePtr);

#endif /* _mm_dump_ */

/************************/
/*  End of File Comment */
/************************/
