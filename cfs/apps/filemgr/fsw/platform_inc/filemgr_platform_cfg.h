/*
** Purpose: Define platform configurations for the OSk File Manager application
**
** Notes:
**   1. This is part of a refactoring prototype. The definitions in this file 
**      should come from a EDS or equivalent toolchain
**
** License:
**   Template written by David McComas and licensed under the GNU
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
#include "filemgr_msgids.h"
#include "filemgr_perfids.h"

#define FILEMGR_INI_FILE_NAME  "/cf/filemgr_ini.json"

/******************************************************************************
** These will be moved to a JSON INI file 
*/

#define FILEMGR_INI_APP_NAME          "FILEMGR"
#define FILEMGR_INI_TBL_CFE_NAME      "FileSys"
#define FILEMGR_INI_TBL_FILENAME      "filesys_tbl.tbl"
#define FILEMGR_INI_TBL_DEF_NAME      ("/cf/" FILEMGR_INI_TBL_FILENAME)
#define FILEMGR_INI_TBL_DEF_DESC      "File System Free Space"

#define FILEMGR_INI_TBL_ERR           (0xCF000080L)

#define FILEMGR_INI_CMD_PIPE_DEPTH     10
#define FILEMGR_INI_CMD_PIPE_NAME     "FILEMGR_CMD_PIPE"
 
#define FILEMGR_INI_CHILD_SEM_NAME    "FILEMGR_CHILD_SEM"
#define FILEMGR_INI_CHILD_NAME        "FILEMGR_CHILD"
#define FILEMGR_INI_CHILD_STACK_SIZE  20480
#define FILEMGR_INI_CHILD_PRIORITY    205


/******************************************************************************
** These will be in a spec file and the toolchain will create these
** definitions.
*/

#define FILEMGR_DIR_LIST_PKT_ENTRIES   20
#define FILEMGR_FILESYS_TBL_VOL_CNT     8


#endif /* _filemgr_platform_cfg_ */
