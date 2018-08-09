/*
** Purpose: Define configurations for the OPenSat Kit Scheduler. This app
**          exercises the OpenSat application framework.
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
#include "osk_app_fw.h"

/******************************************************************************
** Scheduler Application Macros
*/

#define  KIT_SCH_MAJOR_VERSION      1
#define  KIT_SCH_MINOR_VERSION      0
#define  KIT_SCH_REVISION           0
#define  KIT_SCH_MISSION_REV        0

#define  KIT_SCH_PIPE_DEPTH       10
#define  KIT_SCH_PIPE_NAME        "KIT_SCH_CMD_PIPE"


/******************************************************************************
** Command Macros
*/

#define KIT_SCH_LOAD_TBL_CMD_FC      (CMDMGR_APP_START_FC + 0)
#define KIT_SCH_DUMP_TBL_CMD_FC      (CMDMGR_APP_START_FC + 1)
#define SCHEDULER_CFG_SCH_TBL_CMD_FC (CMDMGR_APP_START_FC + 2)

/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
**
*/

#define KIT_SCH_BASE_EID    (APP_FW_APP_BASE_EID +  0)
#define SCHTBL_BASE_EID     (APP_FW_APP_BASE_EID + 10)
#define MSGTBL_BASE_EID     (APP_FW_APP_BASE_EID + 20)
#define SCHEDULER_BASE_EID  (APP_FW_APP_BASE_EID + 30) /* Uses more than 10 IDs */

/*
** One event ID is used for all initialization debug messages. Uncomment one of
** the KIT_SCH_INIT_EVS_TYPE definitions. Set it to INFORMATION if you want to
** see the events during initialization. This is opposite to what you'd expect 
** because INFORMATION messages are enabled by default when an app is loaded.
*/

#define KIT_SCH_INIT_DEBUG_EID 999
#define KIT_SCH_INIT_EVS_TYPE CFE_EVS_DEBUG
//#define KIT_SCH_INIT_EVS_TYPE CFE_EVS_INFORMATION


/*#############################################################################
** XML Element/Attribute tag definitions common to msg and sch tables
**
*/

#define TBL_XML_LEVEL_INDENT 3
#define TBL_XML_MAX_KW_LEN   128   /* Maximum keyword length in bytes */

#define TBL_XML_EL_SCH_TBL    "sch-tbl"
#define TBL_XML_EL_SCH_SLOT   "slot"
#define TBL_XML_EL_MSG_TBL    "msg-tbl"
#define TBL_XML_EL_MSG_ENTRY  "entry"

#define TBL_XML_AT_ID         "id"
#define TBL_XML_AT_ENTRY      "entry"
#define TBL_XML_AT_ENABLE     "enable"
#define TBL_XML_AT_FREQ       "frequency"
#define TBL_XML_AT_OFFSET     "offset"
#define TBL_XML_AT_MSG_ID     "msg_id"
#define TBL_XML_AT_STREAM_ID  "stream-id"
#define TBL_XML_AT_SEQ_SEG    "seq-seg"
#define TBL_XML_AT_MSG_LEN    "length"

#define TBL_XML_AT_VAL_TRUE   "true"
#define TBL_XML_AT_VAL_FALSE  "false"

#define TBL_BUFFSIZE  8192

#endif /* _app_cfg_ */
