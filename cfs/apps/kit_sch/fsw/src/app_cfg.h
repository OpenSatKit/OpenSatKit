/*
** Purpose: Define configurations for the OpenSat Kit Scheduler. This app
**          uses the OpenSatKit application framework.
**
** Notes:
**   1. These macros can only be built with the application and can't
**      have a platform scope because the same file name is used for
**      all applications following the object-based application design.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General
**   Public License (GPL).
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

#include "kit_sch_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
** Scheduler Application Macros
*/

/*
** 1.0 - Initial release
** 1.1 - Refactored for OSK 2.2
** 2.0 - Added Sch & Msg table commands and diagnostics telemetry
*/

#define  KIT_SCH_MAJOR_VER      2
#define  KIT_SCH_MINOR_VER      0

#define  KIT_SCH_PIPE_DEPTH       10
#define  KIT_SCH_PIPE_NAME        "KIT_SCH_CMD_PIPE"


/******************************************************************************
** Command Macros
*/

#define KIT_SCH_LOAD_TBL_CMD_FC             (CMDMGR_APP_START_FC + 0)
#define KIT_SCH_DUMP_TBL_CMD_FC             (CMDMGR_APP_START_FC + 1)
#define SCHEDULER_CFG_SCH_TBL_ENTRY_CMD_FC  (CMDMGR_APP_START_FC + 2)
#define SCHEDULER_LOAD_SCH_TBL_ENTRY_CMD_FC (CMDMGR_APP_START_FC + 3)
#define SCHEDULER_SEND_SCH_TBL_ENTRY_CMD_FC (CMDMGR_APP_START_FC + 4)
#define SCHEDULER_LOAD_MSG_TBL_ENTRY_CMD_FC (CMDMGR_APP_START_FC + 5)
#define SCHEDULER_SEND_MSG_TBL_ENTRY_CMD_FC (CMDMGR_APP_START_FC + 6)
#define SCHEDULER_SEND_DIAG_TLM_CMD_FC      (CMDMGR_APP_START_FC + 7)

/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
**
*/

#define KIT_SCH_APP_BASE_EID  (OSK_C_FW_APP_BASE_EID +   0)
#define SCHTBL_BASE_EID       (OSK_C_FW_APP_BASE_EID + 100)
#define MSGTBL_BASE_EID       (OSK_C_FW_APP_BASE_EID + 200)
#define SCHEDULER_BASE_EID    (OSK_C_FW_APP_BASE_EID + 300)

/*
** One event ID is used for all initialization debug messages. Uncomment one of
** the KIT_SCH_INIT_EVS_TYPE definitions. Set it to INFORMATION if you want to
** see the events during initialization. This is opposite to what you'd expect 
** because INFORMATION messages are enabled by default when an app is loaded.
*/

#define KIT_SCH_INIT_DEBUG_EID 999
#define KIT_SCH_INIT_EVS_TYPE CFE_EVS_DEBUG
//#define KIT_SCH_INIT_EVS_TYPE CFE_EVS_INFORMATION


#endif /* _app_cfg_ */
