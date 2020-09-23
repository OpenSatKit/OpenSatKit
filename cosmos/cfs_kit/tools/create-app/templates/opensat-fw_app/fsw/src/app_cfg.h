/*
** Purpose: Define configurations for the @Template@ application.
**
** Notes:
**   1. These macros can only be built with the application and can't
**      have a platform scope because the same file name is used for
**      all applications following the object-based application design.
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "cfe.h"
#include "@template@_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
** @Template@ Application Macros
*/


#define  @TEMPLATE@_MAJOR_VERSION      0
#define  @TEMPLATE@_MINOR_VERSION      9
#define  @TEMPLATE@_REVISION           0
#define  @TEMPLATE@_MISSION_REV        0

/******************************************************************************
** Command Macros
**
*/

#define EXTBL_LOAD_CMD_FC  (CMDMGR_APP_START_FC + 0)
#define EXTBL_DUMP_CMD_FC  (CMDMGR_APP_START_FC + 1)
#define EXOBJ_DEMO_CMD_FC  (CMDMGR_APP_START_FC + 2)


/******************************************************************************
** Event Macros
** 
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define @TEMPLATE@_BASE_EID (OSK_C_FW_APP_BASE_EID +  0)
#define EXTBL_BASE_EID      (OSK_C_FW_APP_BASE_EID + 10)
#define EXOBJ_BASE_EID      (OSK_C_FW_APP_BASE_EID + 20)


/******************************************************************************
** Example Table Configurations
**
*/

#define EXTBL_MAX_ENTRY_ID 32

#endif /* _app_cfg_ */
