/*************************************************************************
** File:
**   $Id: cs_verify.h 1.4.1.1 2015/03/03 11:58:53EST sstrege Exp  $
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
**   Contains CFS Checksum macros that run preprocessor checks
**   on mission configurable parameters
**
** References:
**   Flight Software Branch C Coding Standard Version 1.2
**   CFS Development Standards Document
**   CFS CS Heritage Analysis Document
**   CFS CS CDR Package
**
** Notes:
**
**   $Log: cs_verify.h  $
**   Revision 1.4.1.1 2015/03/03 11:58:53EST sstrege 
**   Added copyright information
**   Revision 1.4 2011/09/19 17:33:45EDT jmdagost 
**   Removed verification of the default CRC algorithm.
**   Revision 1.3 2010/03/09 15:05:13EST jmdagost 
**   Added requested verification checks.
**   Revision 1.2 2008/07/23 10:34:38EDT njyanchik 
**   Check in of CS Unit test
**   Revision 1.1 2008/06/13 09:04:26EDT njyanchik 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/src/project.pj
** 
*************************************************************************/
#ifndef _cs_verify_
#define _cs_verify_

/*************************************************************************
** Includes
*************************************************************************/
#include "cs_platform_cfg.h"
#include "cs_mission_cfg.h"
/*************************************************************************
** Macro Definitions
*************************************************************************/

#if CS_MAX_NUM_EEPROM_TABLE_ENTRIES  > 65535
    #error CS_MAX_NUM_EEPROM_TABLE_ENTRIES cannot be greater than 65535!
#endif 

#if CS_MAX_NUM_MEMORY_TABLE_ENTRIES  > 65535
    #error CS_MAX_NUM_MEMORY_TABLE_ENTRIES cannot be greater than 65535!
#endif

#if CS_MAX_NUM_APP_TABLE_ENTRIES > CFE_ES_MAX_APPLICATIONS
    #error CS_MAX_NUM_APP_TABLE_ENTRIES cannot be greater than CFE_ES_MAX_APPLICATIONS!
#endif

#if CS_MAX_NUM_TABLES_TABLE_ENTRIES > CFE_TBL_MAX_NUM_TABLES
    #error CS_MAX_NUM_TABLES_TABLE_ENTRIES cannot be greater than CFE_TBL_MAX_NUM_TABLES!
#endif

#if (CS_MAX_NUM_EEPROM_TABLE_ENTRIES < 1)
    #error CS_MAX_NUM_EEPROM_TABLE_ENTRIES must be at least 1!
#endif

#if (CS_MAX_NUM_MEMORY_TABLE_ENTRIES < 1)
    #error CS_MAX_NUM_MEMORY_TABLE_ENTRIES must be at least 1!
#endif 

#if (CS_MAX_NUM_TABLES_TABLE_ENTRIES < 1)
    #error CS_MAX_NUM_TABLES_TABLE_ENTRIES must be at least 1!
#endif 

#if (CS_MAX_NUM_APP_TABLE_ENTRIES < 1)
    #error CS_MAX_NUM_APP_TABLE_ENTRIES must be at least 1!
#endif

#if (CS_DEFAULT_BYTES_PER_CYCLE > 0xFFFFFFFF)
    #error CS_DEFAULT_BYTES_PER_CYCLE cannot be greater than 0xFFFFFFFF!
#endif

#if (CS_DEFAULT_BYTES_PER_CYCLE < 0)
    #error CS_DEFAULT_BYTES_PER_CYCLE cannot be less than 0!
#endif

#if (CS_CHILD_TASK_PRIORITY < 1)
    #error CS_CHILD_TASK_PRIORITY must be greater than 0!
#endif

#if (CS_CHILD_TASK_PRIORITY > 255)
    #error CS_CHILD_TASK_PRIORITY cannot be greater than 255!
#endif


#endif
/*_cs_verify_*/


/************************/
/*  End of File Comment */
/************************/
