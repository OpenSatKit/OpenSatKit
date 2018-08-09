/*
** Purpose: Define configurations for the OpenSat Kit Command Ingest application
**
** Notes:
**   1. These macros can only be built with the application and can't
**      have a platform scope because the same file name is used for
**      all applications following the object-based application design pattern.
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

#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "kit_ci_platform_cfg.h"
#include "osk_app_fw.h"


/******************************************************************************
** Command Ingest Application Macros
*/

#define  KIT_CI_MAJOR_VERSION      1
#define  KIT_CI_MINOR_VERSION      0
#define  KIT_CI_REVISION           0
#define  KIT_CI_MISSION_REV        0

#define CMDMGR_PIPE_DEPTH         10
#define CMDMGR_PIPE_NAME          "KIT_CI_CMD_PIPE"

/******************************************************************************
** Command Macros
*/

#define KIT_CI_CONFIG_MSG_TUNNEL_CMD_FC   (CMDMGR_APP_START_FC + 0)


/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define KIT_CI_BASE_EID  (APP_FW_APP_BASE_EID +  0)
#define UPLINK_BASE_EID  (APP_FW_APP_BASE_EID + 10)

/*
** One event ID is used for all initialization debug messages. Uncomment one of
** the KIT_CI_INIT_EVS_TYPE definitions. Set it to INFORMATION if you want to
** see the events during initialization. This is opposite to what you'd expect 
** because INFORMATION messages are enabled by default when an app is loaded.
*/

#define KIT_CI_INIT_DEBUG_EID 999
#define KIT_CI_INIT_EVS_TYPE CFE_EVS_DEBUG
//#define KIT_CI_INIT_EVS_TYPE CFE_EVS_INFORMATION


/******************************************************************************
** uplink.h Configurations
*/


#endif /* _app_cfg_ */
