/*
** Purpose: Define configurations for the application framework
**
** Notes:
**   1. Resources are statically allocated for each application's
**      data structures so these configurations muct be sized to 
**      accommodate the application requiring th emost resources.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
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

#define APP_FW_INIT_INFO_EID     0
#define CMDMGR_BASE_EID          1 
#define TBLMGR_BASE_EID         10
#define JSON_BASE_EID           20
#define FAULTREP_BASE_EID       40
#define APP_FW_UTILS_BASE_EID   50
#define APP_FW_APP_BASE_EID    100 /* Starting ID for the App using the framework */

/******************************************************************************
** Fault Reporter (FAULTREP)
*/

#define FAULTREP_ID_MAX  80

/******************************************************************************
** Debug macros
**
** Set debug macros to 1 to enable debug message outputs to the console
*/


#define DBG_CMDMGR   0
#define DBG_TBLMGR   0
#define DBG_JSON     0
#define DBG_FAULTREP 0


#endif /* _app_fw_cfg_ */
