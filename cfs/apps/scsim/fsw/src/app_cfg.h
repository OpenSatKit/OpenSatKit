/*
** Purpose: Define configurations for the Spacecraft Simulator App
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
#include "scsim_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
** SCSIM Application Macros
*/

/*
** Versions:
**
** 1.0 - Initial release
*/

#define  SCSIM_MAJOR_VER      1
#define  SCSIM_MINOR_VER      0


/******************************************************************************
** Init File declarations create:
**
**  typedef enum {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigEnum;
**    
**  typedef struct {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigStruct;
**
**   const char *GetConfigStr(value);
**   ConfigEnum GetConfigVal(const char *str);
**
** XX(name,type)
*/

#define CFG_APP_CFE_NAME        APP_CFE_NAME
#define CFG_APP_MAIN_PERF_ID    APP_MAIN_PERF_ID

#define CFG_CMD_MID             CMD_MID
#define CFG_EXECUTE_MID         EXECUTE_MID
#define CFG_SEND_HK_MID         SEND_HK_MID
#define CFG_HK_TLM_MID          HK_TLM_MID
#define CFG_MGMT_TLM_MID        MGMT_TLM_MID
#define CFG_MODEL_TLM_MID       MODEL_TLM_MID

#define CFG_CMD_PIPE_DEPTH      CMD_PIPE_DEPTH
#define CFG_CMD_PIPE_NAME       CMD_PIPE_NAME

#define CFG_TBL_LOAD_FILE       TBL_LOAD_FILE
#define CFG_TBL_DUMP_FILE       TBL_DUMP_FILE


#define APP_CONFIG(XX) \
   XX(APP_CFE_NAME,char*) \
   XX(APP_MAIN_PERF_ID,uint32) \
   XX(CMD_MID,uint32) \
   XX(EXECUTE_MID,uint32) \
   XX(SEND_HK_MID,uint32) \
   XX(HK_TLM_MID,uint32) \
   XX(MGMT_TLM_MID,uint32) \
   XX(MODEL_TLM_MID,uint32) \
   XX(CMD_PIPE_DEPTH,uint32) \
   XX(CMD_PIPE_NAME,char*) \
   XX(TBL_LOAD_FILE,char*) \
   XX(TBL_DUMP_FILE,char*) \

DECLARE_ENUM(Config,APP_CONFIG)


/******************************************************************************
** Command Macros
**
*/

#define SCSIMTBL_LOAD_CMD_FC     (CMDMGR_APP_START_FC + 0)
#define SCSIMTBL_DUMP_CMD_FC     (CMDMGR_APP_START_FC + 1)
#define SCSIM_START_SIM_CMD_FC   (CMDMGR_APP_START_FC + 2)
#define SCSIM_STOP_SIM_CMD_FC    (CMDMGR_APP_START_FC + 3)
#define SCSIM_START_PLBK_CMD_FC  (CMDMGR_APP_START_FC + 4)
#define SCSIM_STOP_PLBK_CMD_FC   (CMDMGR_APP_START_FC + 5)

/******************************************************************************
** Event Macros
** 
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define SCSIM_APP_BASE_EID  (OSK_C_FW_APP_BASE_EID +  0)
#define SCSIM_BASE_EID      (OSK_C_FW_APP_BASE_EID + 10)
#define SCSIMTBL_BASE_EID   (OSK_C_FW_APP_BASE_EID + 50)

/*
** One event ID is used for all initialization debug messages. Uncomment one of
** the SCSIM_INIT_EVS_TYPE definitions. Set it to INFORMATION if you want to
** see the events during initialization. This is opposite to what you'd expect 
** because INFORMATION messages are enabled by default when an app is loaded.
*/

#define SCSIM_INIT_DEBUG_EID 999
#define SCSIM_INIT_EVS_TYPE CFE_EVS_DEBUG
//#define SCSIM_INIT_EVS_TYPE CFE_EVS_INFORMATION

/******************************************************************************
** SCSIM
**
*/

#define SCSIM_DEBUG (0)

#endif /* _app_cfg_ */
