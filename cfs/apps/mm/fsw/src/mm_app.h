/************************************************************************
** File:
**   $Id: mm_app.h 1.2 2015/12/29 15:22:20EST czogby Exp  $
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
**   Unit specification for the Core Flight System (CFS) 
**   Memory Manger (MM) Application.  
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS MM Heritage Analysis Document
**   CFS MM CDR Package
**
** Notes:
**
**   $Log: mm_app.h  $
**   Revision 1.2 2015/12/29 15:22:20EST czogby 
**   Move function prototypes from .c files into .h files
**   Revision 1.1 2015/07/28 12:21:36EDT rperera 
**   Initial revision
**   Member added to project /CFS-APPs-PROJECT/mm/fsw/src/project.pj
**   Revision 1.8 2015/04/06 15:41:02EDT lwalling 
**   Verify results of calls to PSP memory read/write/copy/set functions
**   Revision 1.7 2015/03/30 17:33:56EDT lwalling 
**   Create common process to maintain and report last action statistics
**   Revision 1.6 2015/03/20 14:16:53EDT lwalling 
**   Add last peek/poke/fill command data value to housekeeping telemetry
**   Revision 1.5 2015/03/02 14:26:41EST sstrege 
**   Added copyright information
**   Revision 1.4 2008/09/05 14:24:51EDT dahardison 
**   Updated declaration of local HK variables
**   Revision 1.3 2008/05/19 15:22:56EDT dahardison 
**   Version after completion of unit testing
** 
*************************************************************************/
#ifndef _mm_app_
#define _mm_app_

/************************************************************************
** Includes
*************************************************************************/
#include "mm_msg.h"
#include "cfe.h"

/************************************************************************
** Macro Definitions
*************************************************************************/
/**
** \name MM Command Pipe Parameters */ 
/** \{ */
#define MM_CMD_PIPE_DEPTH   12
#define MM_HK_LIMIT          2
#define MM_CMD_LIMIT         4    
/** \} */

/************************************************************************
** Type Definitions
*************************************************************************/
/** 
**  \brief MM global data structure
*/
typedef struct          
{ 
   MM_HkPacket_t        HkPacket;        /**< \brief Housekeeping telemetry packet  */
   
   CFE_SB_MsgPtr_t      MsgPtr;          /**< \brief Pointer to command message     */
   CFE_SB_PipeId_t      CmdPipe;         /**< \brief Command pipe ID                */
   
   uint32               RunStatus;       /**< \brief Application run status         */
   
   char                 PipeName[16];    /**< \brief Command pipe name              */
   uint16               PipeDepth;       /**< \brief Command pipe message depth     */
   
   uint8                LimitHK;         /**< \brief Houskeeping messages limit     */
   uint8                LimitCmd;        /**< \brief Command messages limit         */

   uint8                CmdCounter;      /**< \brief MM Application Command Counter       */
   uint8                ErrCounter;      /**< \brief MM Application Command Error Counter */
   uint8                LastAction;      /**< \brief Last command action executed         */
   uint8                MemType;         /**< \brief Memory type for last command         */
   uint32               Address;         /**< \brief Fully resolved address used for last 
                                                     command                              */
   uint32               DataValue;       /**< \brief Last command data value -- may be 
                                                     fill pattern or peek/poke value      */    
   uint32               BytesProcessed;  /**< \brief Bytes processed for last command     */
   
   char                 FileName[OS_MAX_PATH_LEN];   /**< \brief Name of the data file 
                                                                 used for last command, 
                                                                 where applicable         */
   
   uint32         LoadBuffer[MM_MAX_LOAD_DATA_SEG / 4];  /**< \brief Load file i/o buffer */
   uint32         DumpBuffer[MM_MAX_DUMP_DATA_SEG / 4];  /**< \brief Dump file i/o buffer */
   uint32         FillBuffer[MM_MAX_FILL_DATA_SEG / 4];  /**< \brief Fill memory buffer   */
   
} MM_AppData_t;           

/************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief CFS Memory Manager (MM) application entry point
**  
**  \par Description
**       Memory Manager application entry point and main process loop.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
*************************************************************************/
void MM_AppMain(void);

/************************************************************************
** Function prototypes
*************************************************************************/
/************************************************************************/
/** \brief Initialize the memory manager CFS application
**  
**  \par Description
**       Memory manager application initialization routine. This 
**       function performs all the required startup steps to 
**       get the application registered with the cFE services so
**       it can begin to receive command messages. 
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
**  \retstmt Return codes from #CFE_EVS_Register         \endcode
**  \retstmt Return codes from #CFE_SB_CreatePipe        \endcode
**  \retstmt Return codes from #CFE_SB_Subscribe         \endcode
**  \endreturns
**
*************************************************************************/
int32 MM_AppInit(void); 

/************************************************************************/
/** \brief Process a command pipe message
**  
**  \par Description
**       Processes a single software bus command pipe message. Checks
**       the message and command IDs and calls the appropriate routine
**       to handle the command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg   A #CFE_SB_MsgPtr_t pointer that
**                      references the software bus message 
**
**  \sa #CFE_SB_RcvMsg
**
*************************************************************************/
void MM_AppPipe(CFE_SB_MsgPtr_t msg);
 
/************************************************************************/
/** \brief Process housekeeping request
**  
**  \par Description
**       Processes an on-board housekeeping request message.
**
**  \par Assumptions, External Events, and Notes:
**       This command does not affect the command execution counter
**       
**  \param [in]   msg   A #CFE_SB_MsgPtr_t pointer that
**                      references the software bus message 
**
*************************************************************************/
void MM_HousekeepingCmd(CFE_SB_MsgPtr_t msg);
 
/************************************************************************/
/** \brief Process noop command
**  
**  \par Description
**       Processes a noop ground command.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg   A #CFE_SB_MsgPtr_t pointer that
**                      references the software bus message 
**
**  \sa #MM_NOOP_CC
**
*************************************************************************/
void MM_NoopCmd(CFE_SB_MsgPtr_t msg);

/************************************************************************/
/** \brief Process reset counters command
**  
**  \par Description
**       Processes a reset counters ground command which will reset
**       the memory manager commmand error and command execution counters
**       to zero.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   MessagePtr   A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_RESET_CC
**
*************************************************************************/
void MM_ResetCmd(CFE_SB_MsgPtr_t MessagePtr);

/************************************************************************/
/** \brief Process lookup symbol command
**  
**  \par Description
**       Processes a lookup symbol ground command which takes a 
**       symbol name and tries to resolve it to an address using the
**       #OS_SymbolLookup OSAL function.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg          A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_LOOKUP_SYM_CC
**
*************************************************************************/
void MM_LookupSymbolCmd(CFE_SB_MsgPtr_t msg);

/************************************************************************/
/** \brief Dump symbol table to file command
**  
**  \par Description
**       Processes a dump symbol table to file ground command which calls  
**       the #OS_SymbolTableDump OSAL function using the specified dump
**       file name.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg          A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_SYMTBL_TO_FILE_CC
**
*************************************************************************/
void MM_SymTblToFileCmd(CFE_SB_MsgPtr_t msg);

/************************************************************************/
/** \brief Write-enable EEPROM command
**  
**  \par Description
**       Processes a EEPROM write enable ground command which calls  
**       the #CFE_PSP_EepromWriteEnable cFE function using the specified
**       bank number.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg          A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_ENABLE_EEPROM_WRITE_CC
**
*************************************************************************/
void MM_EepromWriteEnaCmd(CFE_SB_MsgPtr_t msg);

/************************************************************************/
/** \brief Write-disable EEPROM command
**  
**  \par Description
**       Processes a EEPROM write disable ground command which calls  
**       the #CFE_PSP_EepromWriteDisable cFE function using the specified
**       bank number.
**
**  \par Assumptions, External Events, and Notes:
**       None
**       
**  \param [in]   msg          A #CFE_SB_MsgPtr_t pointer that
**                             references the software bus message 
**
**  \sa #MM_DISABLE_EEPROM_WRITE_CC
**
*************************************************************************/
void MM_EepromWriteDisCmd(CFE_SB_MsgPtr_t msg);

/************************************************************************
** Exported Data
*************************************************************************/
extern MM_AppData_t     MM_AppData;


#endif /* _mm_app_ */

/************************/
/*  End of File Comment */
/************************/
