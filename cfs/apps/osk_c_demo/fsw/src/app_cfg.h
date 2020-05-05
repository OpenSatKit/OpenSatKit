/*
** Purpose: Define configurations for the Demo application. This 
**          app exercises the OpenSatKit application framework and 
**          serves as a good starting point for trying new ideas.
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
#include "osk_c_demo_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
** OSK Demo Application Macros
*/

#define  OSK_C_DEMO_MAJOR_VER     1
#define  OSK_C_DEMO_MINOR_VER     4 /* Moved table buffers from app to DemoObj */
#define  OSK_C_DEMO_LOCAL_REV     0

/******************************************************************************
** Command Macros
*/

#define OSK_C_DEMO_DEMOBJ_TBL_LOAD_CMD_FC      (CMDMGR_APP_START_FC + 0)
#define OSK_C_DEMO_DEMOBJ_TBL_DUMP_CMD_FC      (CMDMGR_APP_START_FC + 1)
#define OSK_C_DEMO_ENA_TBL_DATA_CMD_FC         (CMDMGR_APP_START_FC + 2)
#define OSK_C_DEMO_SET_ACTIVE_TBL_CMD_FC       (CMDMGR_APP_START_FC + 3)
#define OSK_C_DEMO_FAULTREP_CFG_CMD_FC         (CMDMGR_APP_START_FC + 4)
#define OSK_C_DEMO_FAULTREP_CLR_CMD_FC         (CMDMGR_APP_START_FC + 5)
#define OSK_C_DEMO_DEMOFR_SET_TLM_MODE_CMD_FC  (CMDMGR_APP_START_FC + 6)
#define OSK_C_DEMO_DEMOFR_SIM_FAULT_CMD_FC     (CMDMGR_APP_START_FC + 7)


/******************************************************************************
** Event Macros
** 
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define OSK_C_DEMO_BASE_EID (OSK_C_FW_APP_BASE_EID +  0)
#define DEMOBJ_BASE_EID     (OSK_C_FW_APP_BASE_EID + 10)
#define XMLTBL_BASE_EID     (OSK_C_FW_APP_BASE_EID + 20)
#define SCANFTBL_BASE_EID   (OSK_C_FW_APP_BASE_EID + 30)
#define JSONTBL_BASE_EID    (OSK_C_FW_APP_BASE_EID + 40)
#define DEMOFR_BASE_EID     (OSK_C_FW_APP_BASE_EID + 50)

/*
** One event ID is used for all initialization debug messages. Uncomment one of
** the OSK_C_DEMO_INIT_EVS_TYPE definitions. Set it to INFORMATION if you want to
** see the events during initialization. This is opposite to what you'd expect 
** because INFORMATION messages are enabled by default when an app is loaded.
*/

#define OSK_C_DEMO_INIT_DEBUG_EID 999
#define OSK_C_DEMO_INIT_EVS_TYPE CFE_EVS_DEBUG
//#define OSK_C_DEMO_INIT_EVS_TYPE CFE_EVS_INFORMATION


/******************************************************************************
** Example Table Configurations
**
** - They must be identical
*/

#define OSK_C_DEMO_TBL_MAX_ENTRY_ID 32
#define XMLTBL_MAX_ENTRY_ID      OSK_C_DEMO_TBL_MAX_ENTRY_ID
#define SCANFTBL_MAX_ENTRY_ID    OSK_C_DEMO_TBL_MAX_ENTRY_ID
#define JSONTBL_MAX_ENTRY_ID     OSK_C_DEMO_TBL_MAX_ENTRY_ID

/******************************************************************************
** Fault Reporter/Detector 
**
** Show how app can be lower than osk_c_fw default & keep low for simple screen 
*/

#define OSK_C_DEMO_FAULT_ID_MAX    16  
#define OSK_C_DEMO_FAULT_ID_WORDS   1

#endif /* _app_cfg_ */
