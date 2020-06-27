/*
** Purpose: Define configurations for the OpenSat Kit Telemetry Output app. 
**
** Notes:
**   1. These macros can only be built with the application and can't
**      have a platform scope because the same file name is used for
**      all applications following the object-based application design.
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

#include "cfe_platform_cfg.h"
#include "kit_to_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
** Telemetry Output Application Macros
*/

/*
** 1.1 - Refactored for OSK 2.2
** 1.2 - Add statistics, app delay cmd, debug events for SB training, June 2020
*/

#define  KIT_TO_MAJOR_VER     1
#define  KIT_TO_MINOR_VER     2
#define  KIT_TO_LOCAL_REV     0


#define  KIT_TO_CMD_PIPE_DEPTH    20
#define  KIT_TO_CMD_PIPE_NAME     "KIT_TO_CMD_PIPE"


/******************************************************************************
** Command Macros
*/

#define KIT_TO_PKT_TBL_LOAD_CMD_FC       (CMDMGR_APP_START_FC + 0)
#define KIT_TO_PKT_TBL_DUMP_CMD_FC       (CMDMGR_APP_START_FC + 1)

#define KIT_TO_ADD_PKT_CMD_FC            (CMDMGR_APP_START_FC + 2)
#define KIT_TO_REMOVE_PKT_CMD_FC         (CMDMGR_APP_START_FC + 3)
#define KIT_TO_REMOVE_ALL_PKTS_CMD_FC    (CMDMGR_APP_START_FC + 4)
#define KIT_TO_ENABLE_OUTPUT_CMD_FC      (CMDMGR_APP_START_FC + 5)
#define KIT_TO_SEND_DATA_TYPES_CMD_FC    (CMDMGR_APP_START_FC + 6)
#define KIT_TO_SET_RUN_LOOP_DELAY_CMD_FC (CMDMGR_APP_START_FC + 7)

/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define KIT_TO_APP_BASE_EID  (OSK_C_FW_APP_BASE_EID +  0)
#define PKTTBL_BASE_EID      (OSK_C_FW_APP_BASE_EID + 10)
#define PKTMGR_BASE_EID      (OSK_C_FW_APP_BASE_EID + 20)

/*
** One event ID is used for all initialization debug messages. Uncomment one of
** the KIT_TO_INIT_EVS_TYPE definitions. Set it to INFORMATION if you want to
** see the events during initialization. This is opposite to what you'd expect 
** because INFORMATION messages are enabled by default when an app is loaded.
*/

#define KIT_TO_INIT_DEBUG_EID 999
#define KIT_TO_INIT_EVS_TYPE CFE_EVS_DEBUG
//#define KIT_TO_INIT_EVS_TYPE CFE_EVS_INFORMATION


/******************************************************************************
** pktmgr.h Configurations
*/

/*
** Pipe configurations
*/

#define PKTMGR_PIPE_DEPTH      100
#define PKTMGR_PIPE_NAME       "KIT_TO_PKT_PIPE"

/*
** Statistics
*/

#define PKTMGR_STATS_STARTUP_INIT_MS   20000  /* ms after app initialized to start stats computations   */
#define PKTMGR_STATS_RECONFIG_INIT_MS   5000  /* ms after a reconfiguration to start stats computations */

#define PKTMGR_COMPUTE_STATS_INTERVAL_MS KIT_TO_MAX_RUN_LOOP_DELAY_MS /* ms between stats updates       */

/******************************************************************************
** pkttbl.h Configurations
*/

/*
** Maximum Number of Packet Definitions in Packet Table. Must be greater than zero.
*/

#define PKTTBL_MAX_PKT_CNT     64
#define PKTTBL_UNUSED_MSG_ID   (CFE_SB_HIGHEST_VALID_MSGID+1)


#endif /* _app_cfg_ */
