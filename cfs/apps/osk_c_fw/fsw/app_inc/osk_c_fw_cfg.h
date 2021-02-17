/*
** Purpose: Define configurations for the application framework
**
** Notes:
**   1. These definitions are intrinsic to the framework and should not
**      change across platform deployments. They should be fixed between
**      framework releases but may need to can during framework development
**      and for a release. See osk_c_fw_platform.h and osk_c_fw_mission.h
**      for parameters that can be configured for a deployment.
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

#ifndef _osk_c_fw_cfg_
#define _osk_c_fw_cfg_

/*
** Includes
*/

#include "cfe.h"
#include "osk_c_fw_platform_cfg.h"


/******************************************************************************
** Command Manager (CMDMGR)
*/

#define CMDMGR_CMD_FUNC_TOTAL  32

/* Standard function definitions */

#define CMDMGR_NOOP_CMD_FC      0  
#define CMDMGR_RESET_CMD_FC     1
#define CMDMGR_APP_START_FC     2  /* First FC available for app */


/******************************************************************************
** Event Macros
** 
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define OSK_C_FW_INIT_INFO_EID     0
#define INILIB_BASE_EID            1 
#define CMDMGR_BASE_EID           10 
#define TBLMGR_BASE_EID           20
#define JSON_BASE_EID             30
#define CHILDMGR_BASE_EID         50
#define STATEREP_BASE_EID         70
#define OSK_C_FW_UTILS_BASE_EID   80
#define OSK_C_FW_APP_BASE_EID    100 /* Starting ID for the App using the framework */


/******************************************************************************
** Debug macros
**
** Set debug macros to 1 to enable debug message outputs to the console
*/


#define DBG_INITBL    0
#define DBG_CMDMGR    0
#define DBG_TBLMGR    0
#define DBG_JSON      0
#define DBG_FAULTREP  0
#define DBG_CHILDMGR  0


#endif /* _osk_c_fw_cfg_ */
