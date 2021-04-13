/*
** Purpose: Define configurations for the Instrument Simulator App
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
#include "isim_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
** Isim Application Macros
*/

/*
** Versions:
**
** 1.0 - Initial release
** 1.1 - Main loop control changed from task delay to pend for scheduler wakeup
** 2.0 - Major enhancements to support Space Systems course student assignments.
**       Restructured ISIM to have a Detector and SciFile subclasses. When fault
**       present it manifest itself in the science data.
** 2.1 - Create a JSON init file. Moved platform, perfid, and msg defs.
*/

#define  ISIM_MAJOR_VER      2
#define  ISIM_MINOR_VER      1


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
   XX(CMD_PIPE_DEPTH,uint32) \
   XX(CMD_PIPE_NAME,char*) \
   XX(TBL_LOAD_FILE,char*) \
   XX(TBL_DUMP_FILE,char*) \

DECLARE_ENUM(Config,APP_CONFIG)


/******************************************************************************
** Command Macros
**
*/

#define ISIMTBL_LOAD_CMD_FC         (CMDMGR_APP_START_FC + 0)
#define ISIMTBL_DUMP_CMD_FC         (CMDMGR_APP_START_FC + 1)
#define ISIM_PWR_ON_INSTR_CMD_FC    (CMDMGR_APP_START_FC + 2)
#define ISIM_PWR_OFF_INSTR_CMD_FC   (CMDMGR_APP_START_FC + 3)
#define ISIM_PWR_RESET_INSTR_CMD_FC (CMDMGR_APP_START_FC + 4)
#define ISIM_START_SCI_CMD_FC       (CMDMGR_APP_START_FC + 5)
#define ISIM_STOP_SCI_CMD_FC        (CMDMGR_APP_START_FC + 6)
#define ISIM_SET_FAULT_CMD_FC       (CMDMGR_APP_START_FC + 7)
#define ISIM_CLEAR_FAULT_CMD_FC     (CMDMGR_APP_START_FC + 8)


/******************************************************************************
** Event Macros
** 
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define ISIM_APP_BASE_EID  (OSK_C_FW_APP_BASE_EID +  0)
#define ISIM_BASE_EID      (OSK_C_FW_APP_BASE_EID + 10)
#define ISIMTBL_BASE_EID   (OSK_C_FW_APP_BASE_EID + 50)

/*
** One event ID is used for all initialization debug messages. Uncomment one of
** the ISIM_INIT_EVS_TYPE definitions. Set it to INFORMATION if you want to
** see the events during initialization. This is opposite to what you'd expect 
** because INFORMATION messages are enabled by default when an app is loaded.
*/

#define ISIM_INIT_DEBUG_EID 999
#define ISIM_INIT_EVS_TYPE CFE_EVS_DEBUG
//#define ISIM_INIT_EVS_TYPE CFE_EVS_INFORMATION

/******************************************************************************
** ISIM Table Configurations
**
*/

#define ISIM_FILE_EXT_MAX_CHAR 8

#endif /* _app_cfg_ */
