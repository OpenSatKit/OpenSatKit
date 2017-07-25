/*
** Purpose: Define configurations for the application framework
**
** Notes:
**   1. Written by David McComas, licensed under the GNU Lesser
**      General Public License (LGPL) Version 3.
**   2. Resources are statically allocated for each application's
**      data structures so these configurations muct be sized to 
**      accommodate the application requiring th emost resources.
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _app_fw_cfg_
#define _app_fw_cfg_

/*
** Includes
*/

#include "cfe.h"
#include "app_fw_platform_cfg.h"


/******************************************************************************
** Command Manager (CMDMGR)
*/

#define CMDMGR_CMD_FUNC_TOTAL  10

/* Standard function definitions */

#define CMDMGR_NOOP_CMD_FC      0  
#define CMDMGR_RESET_CMD_FC     1
#define CMDMGR_APP_START_FC     2  /* First FC available for app */

/******************************************************************************
** Table Manager (TBLMGR)
*/

#define TBLMGR_MAX_TBL_PER_APP  5

/******************************************************************************
** Event Macros
** 
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define CMDMGR_BASE_EID       0 
#define TBLMGR_BASE_EID      10
#define APP_FW_APP_BASE_EID  20 /* Starting ID for the App using the framework */


#endif /* _app_fw_cfg_ */
