/************************************************************************
 ** File:
 **   $Id: cs_msg.h 1.6.1.1 2015/03/03 11:58:17EST sstrege Exp  $
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
 **   Specification for the CFS Checksum command and telemetry 
 **   messages.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **   CFS CS Heritage Analysis Document
 **   CFS CS CDR Package
 **
 ** Notes:
 **
 **   $Log: cs_msg.h  $
 **   Revision 1.6.1.1 2015/03/03 11:58:17EST sstrege 
 **   Added copyright information
 **   Revision 1.6 2011/09/06 14:19:55EDT jmdagost 
 **   Added ChildTaskInUse and OneShotInUse flags to housekeeping tlm.
 **   Revision 1.5 2010/03/09 17:01:10EST jmdagost 
 **   Moved command code definitions to cs_msgdefs.h
 **   Revision 1.4 2009/04/18 11:50:55EDT dkobe 
 **   Corrected typos in doxygen comments
 **   Revision 1.3 2008/07/31 14:05:04EDT njyanchik 
 **   The Startup sync call has been added to the main loop and a platform config parameter has been added to the cs platform config file to regulate the 
 **   timeout for the synchronization.
 **   Revision 1.2 2008/07/23 15:34:43BST njyanchik 
 **   Check in of CS Unit test
 **   Revision 1.1 2008/06/13 09:04:17EDT njyanchik 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/src/project.pj
 ** 
 *************************************************************************/

#ifndef _cs_msg_
#define _cs_msg_

/*************************************************************************
 **
 ** Include section
 **
 **************************************************************************/

#include "cfe.h"

/*************************************************************************
 **
 ** Macro definitions
 **
 **************************************************************************/

/*************************************************************************
 **
 ** Type definitions
 **
 **************************************************************************/

/** 
 **  \cstlm Housekeeping Packet Structure
 */
typedef struct
{
    uint8               TlmHeader[CFE_SB_TLM_HDR_SIZE];     /**< \brief cFE SB Tlm Msg Hdr */

    uint8               CmdCounter;                         /**< \cstlmmnemonic \CS_CMDPC
                                                                 \brief CS Application Command Counter */
    uint8               CmdErrCounter;                      /**< \cstlmmnemonic \CS_CMDEC
                                                                 \brief CS Application Command Error Counter */
    uint8               ChecksumState;                      /**< \cstlmmnemonic \CS_STATE
                                                                 \brief CS Application global checksum state */
    uint8               EepromCSState;                      /**< \cstlmmnemonic \CS_EEPROMSTATE
                                                                 \brief CS Eeprom table checksum stat e*/
    uint8               MemoryCSState;                      /**< \cstlmmnemonic \CS_MEMORYSTATE
                                                                 \brief CS Memory table checksum state */
    uint8               AppCSState;                         /**< \cstlmmnemonic \CS_APPSTATE
                                                                 \brief CS App table checksum state */
    uint8               TablesCSState;                      /**< \cstlmmnemonic \CS_TABLESSTATE
                                                                 \brief CS Tables table checksum stat e*/
    uint8               OSCSState;                          /**< \cstlmmnemonic \CS_OSSTATE
                                                                 \brief OS code segment checksum state */
    uint8               CfeCoreCSState;                     /**< \cstlmmnemonic \CS_CFECORESTATE
                                                                 \brief cFE Core code segment checksum stat e*/
    uint8               ChildTaskInUse;                     /**< \cstlmmnemonic \CS_CHILDTASKINUSE
                                                                 \brief CS "Child Task In Use" flag */
    uint8               OneShotTaskInUse;                   /**< \cstlmmnemonic \CS_ONESHOTTASKINUSE
                                                                 \brief CS "OneShot Task In Use" flag */
    uint8               Filler8;                            /**< \cstlmmnemonic \CS_FILLER8
                                                                 \brief 8 bit padding */
    
    uint16              EepromCSErrCounter;                 /**< \cstlmmnemonic \CS_EEPROMEC
                                                                 \brief Eeprom miscompare counte r*/
    uint16              MemoryCSErrCounter;                 /**< \cstlmmnemonic \CS_MEMORYEC
                                                                 \brief Memory miscompare counter */
    uint16              AppCSErrCounter;                    /**< \cstlmmnemonic \CS_APPEC
                                                                 \brief  App miscompare counter */
    uint16              TablesCSErrCounter;                 /**< \cstlmmnemonic \CS_TABLESEC
                                                                 \brief Tables miscompare counter */
    uint16              CfeCoreCSErrCounter;                /**< \cstlmmnemonic \CS_CFECOREEC
                                                                 \brief cFE core miscompare counter */
    uint16              OSCSErrCounter;                     /**< \cstlmmnemonic \CS_OSEC
                                                                 \brief OS code segment miscopmare counter */
    uint16              CurrentCSTable;                     /**< \cstlmmnemonic \CS_CURRTABLE
                                                                 \brief Current table being checksummed */
    uint16              CurrentEntryInTable;                /**< \cstlmmnemonic \CS_CURRENTRYINTABLE
                                                                 \brief Current entry ID in the table being checksummed */

    uint32              EepromBaseline;                     /**< \cstlmmnemonic \CS_EEPROMBASELINE
                                                                 \brief Baseline checksum for all of Eeprom */
    uint32              OSBaseline;                         /**< \cstlmmnemonic \CS_OSBASELINE
                                                                 \brief Baseline checksum for the OS code segment */
    uint32              CfeCoreBaseline;                    /**< \cstlmmnemonic \CS_CFECOREBASELINE
                                                                 \brief Basline checksum for the cFE core */
    
    uint32              LastOneShotAddress;                 /**< \cstlmmnemonic \CS_LASTONESHOTADDR
                                                                 \brief Address used in last one shot checksum command */
    uint32              LastOneShotSize;                    /**< \cstlmmnemonic \CS_LASTONESHOTSIZE
                                                                 \brief Size used in the last one shot checksum command */
    uint32              LastOneShotChecksum;                /**< \cstlmmnemonic \CS_LASTONESHOTCHECKSUM
                                                                 \brief Checksum of the last one shot checksum command */
    
    uint32              PassCounter;                        /**< \cstlmmnemonic \CS_PASSCOUNTER
                                                                 \brief Number of times CS has passed through all of its tables */
} CS_HkPacket_t;


/**
 ** \brief No arguments command data type
 **  For command details see #CS_NOOP_CC #CS_RESET_CC, #CS_ENABLE_ALL_CS_CC, #CS_DISABLE_ALL_CS_CC,
 **  #CS_ENABLE_CFECORE_CC, #CS_DISABLE_CFECORE_CC, #CS_ENABLE_OS_CC, #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC,
 **  #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC, #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 **  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONESHOT_CC
 */
typedef struct
{
    uint8       CmdHeader[CFE_SB_CMD_HDR_SIZE];
    
}CS_NoArgsCmd_t;
    
    
/**
 ** \brief Get entry ID command 
 **  For command details see CS_GET_ENTRY_ID_EEPROM_CC, #CS_GET_ENTRY_ID_MEMORY_CC 
 */
typedef struct
{
    uint8       CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32      Address;                            /**< \brief Address to get the ID for */
    
}CS_GetEntryIDCmd_t;

/**
 ** \brief Command type for commands using Memory or Eeprom tables
 **  For command details see #CS_ENABLE_ENTRY_EEPROM_CC, #CS_DISABLE_ENTRY_EEPROM_CC,
 ** #CS_ENABLE_ENTRY_MEMORY_CC, #CS_DISABLE_ENTRY_MEMORY_CC
 */
typedef struct
{
    uint8       CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16      EntryID;                            /**< \brief EntryID to perform a command on */
} CS_EntryCmd_t;

/**
 ** \brief Command type for commanding by table name
 **  For command details see #CS_ENABLE_NAME_TABLE_CC, #CS_DISABLE_NAME_TABLE_CC, 
 **  #CS_RECOMPUTE_BASELINE_TABLE_CC, #CS_REPORT_BASELINE_TABLE_CC
 */
typedef struct
{
    uint8       CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char        Name[CFE_TBL_MAX_FULL_NAME_LEN];    /**< \brief Table name to perform a command on */
} CS_TableNameCmd_t;


/**
 ** \brief Command type for commanding by app name
 **  For command details see e #CS_ENABLE_NAME_APP_CC, #CS_DISABLE_NAME_APP_CC, 
 **  #CS_RECOMPUTE_BASELINE_APP_CC, #CS_REPORT_BASELINE_APP_CC
 */
typedef struct
{
    uint8       CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char        Name[OS_MAX_API_NAME];              /**< \brief App name to perform a command on */
} CS_AppNameCmd_t;
/**
 ** \brief Command type for sending one shot calculation
 **  For command details see #CS_ONESHOT_CC
 */
typedef struct
{
    uint8       CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32      Address;                            /**< \brief Address to start checksum */
    uint32      Size;                               /**< \brief Number of bytes to checksum */
}CS_OneShotCmd_t;
    
#endif /* _cs_msg_ */

/************************/
/*  End of File Comment */
/************************/
