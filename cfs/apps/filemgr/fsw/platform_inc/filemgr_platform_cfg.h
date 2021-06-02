/*
** Purpose: Define platform configurations for the OSK File Manager application
**
** Notes:
**   1. This is part of a refactoring prototype. The definitions in this file 
**      should come from a EDS or equivalent toolchain
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _filemgr_platform_cfg_
#define _filemgr_platform_cfg_

/*
** Includes
*/

#include "filemgr_mission_cfg.h"

/******************************************************************************
** Platform Deployment Configurations
*/

#define FILEMGR_PLATFORM_REV   0
#define FILEMGR_INI_FILENAME   "/cf/filemgr_ini.json"


/******************************************************************************
** These are frustrating. They're only needed statically because of the table
** decsriptor build process. 
*/

#define FILEMGR_APP_CFE_NAME   "FILEMGR"
#define FILEMGR_TBL_CFE_NAME   "FileSys"

/******************************************************************************
** These will be in a spec file and the toolchain will create these
** definitions.
*/

#define FILEMGR_DIR_LIST_PKT_ENTRIES     20
#define FILEMGR_FILESYS_TBL_VOL_CNT       8
#define FILEMGR_TASK_FILE_BLOCK_SIZE   2048  /* Chunk of file to work with for one iteration of a task like computing a CRC */

#endif /* _filemgr_platform_cfg_ */
