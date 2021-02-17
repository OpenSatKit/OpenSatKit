/*
** Purpose: Define application configurations for the MQTT App
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**   2. These macros can only be build with the application and can't
**      have a platform scope because the same app_cfg.h file name is used for
**      all applications following the object-based application design.
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/
#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "mqtt_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
** Application Macros
*/

/*
** Versions:
**
** 1.0.0 - Initial refactoring of open source FM
*/

#define  MQTT_MAJOR_VER      1
#define  MQTT_MINOR_VER      0

/******************************************************************************
**  INI File
*/

#define  MQTT_INI_MAX_STRINGS 20

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
#define CFG_CHILD_TASK_PERF_ID  CHILD_TASK_PERF_ID

#define CFG_CMD_MID             CMD_MID
#define CFG_SEND_HK_MID         SEND_HK_MID

#define CFG_HK_TLM_MID          HK_TLM_MID
#define CFG_STR32_TLM_MID       STR32_TLM_MID

#define CFG_CMD_PIPE_DEPTH      CMD_PIPE_DEPTH
#define CFG_CMD_PIPE_NAME       CMD_PIPE_NAME

#define CFG_SERVER_PORT         SERVER_PORT
#define CFG_SERVER_ADDRESS      SERVER_ADDRESS
#define CFG_SERVER_USERNAME     SERVER_USERNAME
#define CFG_SERVER_PASSWORD     SERVER_PASSWORD
      
#define CFG_CLIENT_NAME         CLIENT_NAME

#define CFG_TBL_CFE_NAME        TBL_CFE_NAME
#define CFG_TBL_DEF_FILENAME    TBL_DEF_FILENAME
#define CFG_TBL_ERR_CODE        TBL_ERR_CODE

#define CFG_CHILD_NAME          CHILD_NAME
#define CFG_CHILD_STACK_SIZE    CHILD_STACK_SIZE
#define CFG_CHILD_PRIORITY      CHILD_PRIORITY


#define APP_CONFIG(XX) \
   XX(APP_CFE_NAME,char*) \
   XX(APP_MAIN_PERF_ID,uint32) \
   XX(CHILD_TASK_PERF_ID,uint32) \
   XX(CMD_MID,uint32) \
   XX(SEND_HK_MID,uint32) \
   XX(HK_TLM_MID,uint32) \
   XX(STR32_TLM_MID,uint32) \
   XX(CMD_PIPE_DEPTH,uint32) \
   XX(CMD_PIPE_NAME,char*) \
   XX(SERVER_PORT,uint32) \
   XX(SERVER_ADDRESS,char*) \
   XX(SERVER_USERNAME,char*) \
   XX(SERVER_PASSWORD,char*) \
   XX(CLIENT_NAME,char*) \
   XX(TBL_CFE_NAME,char*) \
   XX(TBL_DEF_FILENAME,char*) \
   XX(TBL_ERR_CODE,uint32) \
   XX(CHILD_NAME,char*) \
   XX(CHILD_STACK_SIZE,uint32) \
   XX(CHILD_PRIORITY,uint32)

DECLARE_ENUM(Config,APP_CONFIG)


/******************************************************************************
** Command Macros
** - Commands implmented by child task are annotated with a comment
** - Load/dump table definitions are placeholders for JSON table
*/

#define MQTT_TBL_LOAD_CMD_FC   (CMDMGR_APP_START_FC +  0)
#define MQTT_TBL_DUMP_CMD_FC   (CMDMGR_APP_START_FC +  1)


/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define MQTT_BASE_EID        (OSK_C_FW_APP_BASE_EID +  0)
#define MQTT_MSG_BASE_EID    (OSK_C_FW_APP_BASE_EID + 20)
#define MQ2SB_BASE_EID       (OSK_C_FW_APP_BASE_EID + 30)
#define MQTT_CLIENT_BASE_EID (OSK_C_FW_APP_BASE_EID + 40)


/******************************************************************************
** MQTT Client
**
*/

#define MQTT_CLIENT_READ_BUF_LEN  1000 
#define MQTT_CLIENT_SEND_BUF_LEN  1000 
#define MQTT_CLIENT_TIMEOUT_MS    2000 


#endif /* _app_cfg_ */
