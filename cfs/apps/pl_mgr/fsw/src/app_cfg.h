/*
** Purpose: Define configurations for the Payload Manager App
**
** Notes:
**   1. These macros can only be built with the application and can't
**      have a platform scope because the same file name is used for
**      all applications following the object-based application design.
**
** License:
**   Written by David McComas and licensed under the GNU
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
#include "pl_mgr_platform_cfg.h"
#include "osk_c_fw.h"


/******************************************************************************
** PL_MGR Application Macros
*/

/*
** Versions:
**
** 1.0 - Initial release based on OSK Instrument Simulator(ISIM)
*/

#define  PL_MGR_MAJOR_VER   1
#define  PL_MGR_MINOR_VER   0


/******************************************************************************
** JSON init file definitions/declarations.
**   
** When the payload is powered off PL_MGR's telemetry is not sent every 
** execution cycle. TLM_SLOW_RATE defines the interval of execution cycles 
** between status telemetry packets being sent.  
*/

#define CFG_APP_CFE_NAME        APP_CFE_NAME
#define CFG_APP_PERF_ID         APP_PERF_ID

#define CFG_CMD_MID             CMD_MID
#define CFG_EXECUTE_MID         EXECUTE_MID
#define CFG_TLM_MID             TLM_MID
#define CFG_TLM_SLOW_RATE       TLM_SLOW_RATE
      
#define CFG_CMD_PIPE_DEPTH      CMD_PIPE_DEPTH
#define CFG_CMD_PIPE_NAME       CMD_PIPE_NAME

#define CFG_SCI_FILE_PATH_BASE  SCI_FILE_PATH_BASE
#define CFG_SCI_FILE_EXTENSION  SCI_FILE_EXTENSION
#define CFG_SCI_FILE_IMAGE_CNT  SCI_FILE_IMAGE_CNT

#define APP_CONFIG(XX) \
   XX(APP_CFE_NAME,char*) \
   XX(APP_PERF_ID,uint32) \
   XX(CMD_MID,uint32) \
   XX(EXECUTE_MID,uint32) \
   XX(TLM_MID,uint32) \
   XX(TLM_SLOW_RATE,uint32) \
   XX(CMD_PIPE_DEPTH,uint32) \
   XX(CMD_PIPE_NAME,char*) \
   XX(SCI_FILE_PATH_BASE,char*) \
   XX(SCI_FILE_EXTENSION,char*) \
   XX(SCI_FILE_IMAGE_CNT,uint32) \

DECLARE_ENUM(Config,APP_CONFIG)


/******************************************************************************
** Command Macros
**
** Start/stop science are owned by payload but they are prefixed with 'PL_MGR'
** because the macos are used in stored commands and the app name makes them
** more readable 
*/

#define PAYLOAD_START_SCI_CMD_FC  (CMDMGR_APP_START_FC + 0)
#define PAYLOAD_STOP_SCI_CMD_FC   (CMDMGR_APP_START_FC + 1)
#define SCI_FILE_CONFIG_CMD_FC    (CMDMGR_APP_START_FC + 2)


/******************************************************************************
** Event Macros
** 
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define PL_MGR_BASE_EID    (OSK_C_FW_APP_BASE_EID +  0)
#define PAYLOAD_BASE_EID   (OSK_C_FW_APP_BASE_EID + 20)
#define SCI_FILE_BASE_EID  (OSK_C_FW_APP_BASE_EID + 40)


/*
** One event ID is used for all initialization debug messages. Uncomment one of
** the PL_MGR_INIT_EVS_TYPE definitions. Set it to INFORMATION if you want to
** see the events during initialization. This is opposite to what you'd expect 
** because INFORMATION messages are enabled by default when an app is loaded.
*/

#define PL_MGR_INIT_DEBUG_EID 999
#define PL_MGR_INIT_EVS_TYPE CFE_EVS_DEBUG
//#define PL_MGR_INIT_EVS_TYPE CFE_EVS_INFORMATION

/******************************************************************************
** SCI_FILE Configurations
**
*/

#define SCI_FILE_EXT_MAX_CHAR   8
#define SCI_FILE_UNDEF_FILE     "null"


#endif /* _app_cfg_ */
