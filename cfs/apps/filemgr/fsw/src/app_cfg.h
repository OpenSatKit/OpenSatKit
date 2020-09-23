/*
** Purpose: Define application configurations for the File Manager (FILEMGR)
**          application
**
** Notes:
**   1. FILEMGR is a refactoring of NASA's FM app using OSK's app framework.
**      It's also a prototype for using a JSON init file for application 
**      parameters that can be specified during runtime. 
**   2. These macros can only be build with the application and can't
**      have a platform scope because the same app_cfg.h file name is used for
**      all applications following the object-based application design.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/
#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "filemgr_platform_cfg.h"
#include "osk_c_fw.h"


/******************************************************************************
** Application Macros
*/

/*
** Versions:
**
** 1.0.0 - Initial refactoring of open source FM
*/

#define  FILEMGR_MAJOR_VER      1
#define  FILEMGR_MINOR_VER      0
#define  FILEMGR_LOCAL_REV      0

/*
** INI File
*/

#define  FILEMGR_INI_MAX_STRINGS 20

/******************************************************************************
** Command Macros
*/

#define FILEMGR_TBL_LOAD_CMD_FC             (CMDMGR_APP_START_FC +  0)
#define FILEMGR_TBL_DUMP_CMD_FC             (CMDMGR_APP_START_FC +  1)

#define FILECMD_COPY_CMD_FC                 (CMDMGR_APP_START_FC +  2)
#define FILECMD_MOVE_CMD_FC                 (CMDMGR_APP_START_FC +  3) 
#define FILECMD_RENAME_CMD_FC               (CMDMGR_APP_START_FC +  4)
#define FILECMD_DELETE_CMD_FC               (CMDMGR_APP_START_FC +  5)
#define FILECMD_DECOMPRESS_CMD_FC           (CMDMGR_APP_START_FC +  6)
#define FILECMD_CONCAT_CMD_FC               (CMDMGR_APP_START_FC +  7)
#define FILECMD_DELETE_INT_CMD_FC           (CMDMGR_APP_START_FC +  8)
#define FILECMD_SET_PERMISSIONS_CMD_FC      (CMDMGR_APP_START_FC +  9)
#define FILECMD_SEND_INFO_CMD_FC            (CMDMGR_APP_START_FC + 10)
#define FILECMD_SEND_OPEN_FILES_PKT_CMD_FC  (CMDMGR_APP_START_FC + 11)

#define DIRCMD_CREATE_CMD_FC                (CMDMGR_APP_START_FC + 12)
#define DIRCMD_DELETE_CMD_FC                (CMDMGR_APP_START_FC + 13)
#define DIRCMD_DELETE_ALL_CMD_FC            (CMDMGR_APP_START_FC + 14)
#define DIRCMD_WRITE_DIR_FILE_CMD_FC        (CMDMGR_APP_START_FC + 15)
#define DIRCMD_SEND_DIR_PKT_CMD_FC          (CMDMGR_APP_START_FC + 16)

#define FILESYS_SEND_TBL_PKT_CMD_FC         (CMDMGR_APP_START_FC + 17)
#define FILESYS_SET_TBL_STATE_CMD_FC        (CMDMGR_APP_START_FC + 18)


/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define FILEMGR_BASE_EID  (OSK_C_FW_APP_BASE_EID +  0)
#define CHILDMGR_BASE_EID (OSK_C_FW_APP_BASE_EID + 10)
#define DIRCMD_BASE_EID   (OSK_C_FW_APP_BASE_EID + 20)
#define FILECMD_BASE_EID  (OSK_C_FW_APP_BASE_EID + 30)
#define FILESYS_BASE_EID  (OSK_C_FW_APP_BASE_EID + 40)


/******************************************************************************
** Child Manager
**
** Define child manager object parameters. The command buffer length must big
** enough to hold the largest command packet.
*/

#define CHILDMGR_CMD_BUFFER_LEN  256   /* Must be greater than largest cmd msg */ 
#define CHILDMGR_CMD_Q_ENTRIES     3   
#define CHILDMGR_CMD_FUNC_TOTAL   32


#endif /* _app_cfg_ */
