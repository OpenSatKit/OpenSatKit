/*
** Purpose: Define configurations for the Demo application. This 
**          app exercises the OpenSat application framework and 
**          serves as a good starting point for trying new ideas.
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**   2. These macros can only be built with the application and can't
**      have a platform scope because the same file name is used for
**      all applications following the object-based application design.
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "cfe.h"
#include "fw_demo_platform_cfg.h"
#include "app_fw.h"

/******************************************************************************
** Framework Demo Application Macros
*/

#define  FW_DEMO_MAJOR_VERSION      1
#define  FW_DEMO_MINOR_VERSION      0
#define  FW_DEMO_REVISION           0
#define  FW_DEMO_MISSION_REV        0

/******************************************************************************
** Command Macros
*/

#define FW_DEMO_EXOBJ_TBL_LOAD_CMD_FC   (CMDMGR_APP_START_FC + 0)
#define FW_DEMO_EXOBJ_TBL_DUMP_CMD_FC   (CMDMGR_APP_START_FC + 1)
#define FW_DEMO_ENA_DATA_LOAD_CMD_FC    (CMDMGR_APP_START_FC + 2)
#define FW_DEMO_SET_TBL_INDEX_CMD_FC    (CMDMGR_APP_START_FC + 3)


/******************************************************************************
** Event Macros
** 
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define FW_DEMO_BASE_EID    (APP_FW_APP_BASE_EID +  0)
#define EXOBJ_BASE_EID      (APP_FW_APP_BASE_EID + 10)
#define XMLTBL_BASE_EID     (APP_FW_APP_BASE_EID + 20)
#define SCANFTBL_BASE_EID   (APP_FW_APP_BASE_EID + 30)

/******************************************************************************
** Example Table Configurations
**
** - They must be identical
*/

#define FW_DEMO_TBL_MAX_ENTRY_ID 32
#define XMLTBL_MAX_ENTRY_ID      FW_DEMO_TBL_MAX_ENTRY_ID
#define SCANFTBL_MAX_ENTRY_ID    FW_DEMO_TBL_MAX_ENTRY_ID

#endif /* _app_cfg_ */
