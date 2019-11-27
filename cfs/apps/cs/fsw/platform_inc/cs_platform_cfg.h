/*************************************************************************
 ** File:
 **   $Id: cs_platform_cfg.h 1.5.1.1 2015/03/03 11:58:16EST sstrege Exp  $
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
 **   Specification for the CFS Checksum constants that can
 **   be configured from one platform to another
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **   CFS CS Heritage Analysis Document
 **   CFS CS CDR Package
 **
 ** Notes:
 **
 **   $Log: cs_platform_cfg.h  $
 **   Revision 1.5.1.1 2015/03/03 11:58:16EST sstrege 
 **   Added copyright information
 **   Revision 1.5 2011/06/15 16:35:28EDT jmdagost 
 **   Moved mission revision number from cs_version.h
 **   Revision 1.4 2010/03/09 15:07:46EST jmdagost 
 **   Removed unused Limit_HK and LIMIT_CMD definitions, added description text for definitions that were missing it.
 **   Revision 1.3 2008/07/31 14:03:47EDT njyanchik 
 **   The Startup sync call has been added to the main loop and a platform config parameter has been added to the cs platform config file to regulate the 
 **   timeout for the synchronization.
 **   Revision 1.2 2008/07/23 15:34:36BST njyanchik 
 **   Check in of CS Unit test
 **   Revision 1.1 2008/06/13 09:03:43EDT njyanchik 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/platform_inc/project.pj
 ** 
 *************************************************************************/
#ifndef _cs_platform_cfg_
#define _cs_platform_cfg_

#include "cfe_platform_cfg.h" //dcm

/*************************************************************************
 ** Macro Definitions
 *************************************************************************/

/** \brief default file names for the definition tables */
/** \{ */
#define CS_DEF_EEPROM_TABLE_FILENAME                "/cf/apps/cs_eepromtbl.tbl"
#define CS_DEF_MEMORY_TABLE_FILENAME                "/cf/apps/cs_memorytbl.tbl"
#define CS_DEF_TABLES_TABLE_FILENAME                "/cf/apps/cs_tablestbl.tbl"
#define CS_DEF_APP_TABLE_FILENAME                   "/cf/apps/cs_apptbl.tbl"
/** \} */

/**
 ** \name CS Command Pipe Config Parameters */
/**\{ */
#define CS_PIPE_DEPTH                   12
/**\} */

/** \cscfg  Maximum number of entries in the Eeprom table to checksum
 **  
 **  \par  Description:
 **        Maximum number of entries that can be in the table of
 **        Eeprom areas to checksum.
 **
 **  \par Limits:
 **     This parameter is limited by the  uint16 datatype that defines it.
 **     This parameter is limited to 65535. 
 **
 */
#define CS_MAX_NUM_EEPROM_TABLE_ENTRIES     16

/** \cscfg  Maximum number of entries in the Memory table to checksum
 **  
 **  \par  Description:
 **        Maximum number of entries that can be in the table of
 **        Memory areas to checksum.
 **
 **  \par Limits:
 **     This parameter is limited by the  uint16 datatype that defines it.
 **     This parameter is limited to 65535. 
 **
 */
#define CS_MAX_NUM_MEMORY_TABLE_ENTRIES     16

/** \cscfg Maximum number of tables to checksum
 **  
 **  \par  Description:
 **        Maximum number of entries in the table of tables to checksum 
 **
 **  \par Limits:
 **       This parameter is limited by the maximum number of tables allowed
 **       in the system. This parameter is limited to #CFE_TBL_MAX_NUM_TABLES
 **
 */
#define CS_MAX_NUM_TABLES_TABLE_ENTRIES     24

/** \cscfg Maximum number of applications to checksum
 **  
 **  \par  Description:
 **        Maximum number of entries in the table of applications to checksum 
 **
 **  \par Limits:
 **       This parameter is limited by the maximum number of applications allowed
 **       in the system. This parameter is limited to #CFE_ES_MAX_APPLICATIONS
 **
 */
#define CS_MAX_NUM_APP_TABLE_ENTRIES        CFE_ES_MAX_APPLICATIONS //dcm

/** \cscfg Default number of bytes to checksum per cycle
 **  
 **  \par  Description:
 **        The default number of bytes that are checksummed in a single CS cycle
 **
 **  \par Limits:
 **       This parameter is limited by the maximum value allowed by the data type.
 **       In this case, the data type is an unsigned 32-bit integer, so the valid 
 **       range is 0 to 0xFFFFFFFF.  Note that "0" is a valid value, and will 
 **       result in a checksum of 0.
 **
 */
#define CS_DEFAULT_BYTES_PER_CYCLE          (1024 *16) 

/** \cscfg CS Child Task Priority
 **  
 **  \par  Description:
 **        Priority of child tasks created by CS.  Lower numbers are higher priority,
 **        with 1 being the highest priority in the case of a child task.
 **
 **  \par Limits:
 **       Valid range for a child task is 1 to 255, but the priority cannot be 
 **       higher (lower number) than the CS App priority. 
 **
 */
#define CS_CHILD_TASK_PRIORITY              200


/** \cscfg Delay between checksumming cycles for child task
 **  
 **  \par  Description:
 **        CS child tasks perform checksum cycles like the main App.
 **        Since the child tasks aren't scheduled, there needs to be 
 **        some other mechanism to prevent it from hogging the CPU.
 **        This parameter specifies the number of milliseconds to delay
 **        in between cycles.
 **
 **  \par Limits:
 **       CS does not place limits on this parameter. It is intended to
 **       be configurable to prevent the child task from hogging the CPU
 **
 */
#define CS_CHILD_TASK_DELAY                 1000


/** \cscfg Timeout for waiting for other apps to start
 **  
 **  \par  Description:
 **        CS waits for all of the other applications that are listed in
 **        the startup script to start before entering its main loop. If
 **        not all of those apps start, CS can pend indefinitely without
 **        a timeout. Once CS waits this amount of time ( in milliseconds)
 **        it will start regardless of the status of the rest of the apps
 **        in the startup script.  
 **
 **  \par Limits:
 **       CS does not place limits on this parameter. It is intended to
 **       be configurable to allow enough time for all apps to start.
 **
 */
#define CS_STARTUP_TIMEOUT                  60000

/** \cscfg Mission specific version number for CS application
**  
**  \par Description:
**       An application version number consists of four parts:
**       major version number, minor version number, revision
**       number and mission specific revision number. The mission
**       specific revision number is defined here and the other
**       parts are defined in "cs_version.h".
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define CS_MISSION_REV            0


#endif /*_cs_platform_cfg_*/

/************************/
/*  End of File Comment */
/************************/

