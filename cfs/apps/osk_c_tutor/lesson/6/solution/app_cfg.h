/* LESSON 6 - Solution
** Purpose: Define application configurations for the OSK C Demo
**          application
**
** Notes:
**   1. These configurations should have an application scope and define
**      parameters that shouldn't need to change across deployments. If
**      a change is made to this file or any other app source file during
**      a deployment then the definition of the OSK_C_DEMO_PLATFORM_REV
**      macro in osk_c_demo_platform.h should be updated.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/
#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "osk_c_fw.h"
#include "osk_c_demo_platform_cfg.h"


/******************************************************************************
** Application Macros
*/

/*
** Versions:
**
** 1.0 - Initial version
*/

#define  OSK_C_DEMO_MAJOR_VER   1
#define  OSK_C_DEMO_MINOR_VER   6  /* Lesson ID */


/******************************************************************************
** Init JSON file declarations. The following steps show how to define and
** use initialization parameters defined in the JSON ini file. Users don't 
** need to know the structures created by the macros but they are shown for
** completeness. The app's command pipe definitions are used as an example. 
**
** 1. Define configuration parameter names
**
**    #define CFG_CMD_PIPE_NAME   CMD_PIPE_NAME
**    #define CFG_CMD_PIPE_DEPTH  CMD_PIPE_DEPTH
**
** 2. Add the parameter to the APP_CONFIG(XX) macro using the name as defined
**    in step 1
**
**    #define APP_CONFIG(XX) \
**       XX(CMD_PIPE_NAME,char*) \
**       XX(CMD_PIPE_DEPTH,uint32) \
**
** 3. Define the parameterin the JSON ini file's "config" object using the
**    same parameter as defined in step 1
**
**    "config": {
**       "CMD_PIPE_NAME":  "OSK_C_DEMO_CMD",
**       "CMD_PIPE_DEPTH": 5,
** 
** 4. Access the parameteres in your code 
**    
**    INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME)
**    INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH)
**
** The following declarations are created using the APP_CONFIG(XX) and 
** XX(name,type) macros:
** 
**    typedef enum {
**       CMD_PIPE_DEPTH,
**       CMD_PIPE_NAME
**    } INITBL_ConfigEnum;
**    
**    typedef struct {
**       CMD_PIPE_DEPTH,
**       CMD_PIPE_NAME
**    } INITBL_ConfigStruct;
**
**    const char *GetConfigStr(value);
**    ConfigEnum GetConfigVal(const char *str);
**
*/

#define CFG_APP_CFE_NAME        APP_CFE_NAME
#define CFG_APP_PERF_ID         APP_PERF_ID

#define CFG_CMD_MID             CMD_MID
#define CFG_EXECUTE_MID         EXECUTE_MID
#define CFG_SEND_HK_MID         SEND_HK_MID
#define CFG_HK_TLM_MID          HK_TLM_MID

#define CFG_CMD_PIPE_NAME       CMD_PIPE_NAME
#define CFG_CMD_PIPE_DEPTH      CMD_PIPE_DEPTH

#define CFG_MSGLOG_PIPE_DEPTH   MSGLOG_PIPE_DEPTH
#define CFG_MSGLOG_PIPE_NAME    MSGLOG_PIPE_NAME

#define CFG_TBL_LOAD_FILE       TBL_LOAD_FILE
#define CFG_TBL_DUMP_FILE       TBL_DUMP_FILE

#define CFG_PLAYBK_TLM_MID      PLAYBK_TLM_MID


#define CFG_CHILD_NAME          CHILD_NAME
#define CFG_CHILD_PERF_ID       CHILD_PERF_ID
#define CFG_CHILD_STACK_SIZE    CHILD_STACK_SIZE
#define CFG_CHILD_PRIORITY      CHILD_PRIORITY

#define APP_CONFIG(XX) \
   XX(APP_CFE_NAME,char*) \
   XX(APP_PERF_ID,uint32) \
   XX(CMD_MID,uint32) \
   XX(EXECUTE_MID,uint32) \
   XX(SEND_HK_MID,uint32) \
   XX(HK_TLM_MID,uint32) \
   XX(CMD_PIPE_NAME,char*) \
   XX(CMD_PIPE_DEPTH,uint32) \
   XX(MSGLOG_PIPE_DEPTH,uint32) \
   XX(MSGLOG_PIPE_NAME,char*) \
   XX(TBL_LOAD_FILE,char*) \
   XX(TBL_DUMP_FILE,char*) \
   XX(PLAYBK_TLM_MID,uint32) \
   XX(CHILD_NAME,char*) \
   XX(CHILD_PERF_ID,uint32) \
   XX(CHILD_STACK_SIZE,uint32) \
   XX(CHILD_PRIORITY,uint32) \

DECLARE_ENUM(Config,APP_CONFIG)


/******************************************************************************
** Command Macros
** - Commands implemented by child task are annotated with a comment
*/

#define OSK_C_DEMO_TBL_LOAD_CMD_FC  (CMDMGR_APP_START_FC + 0)
#define OSK_C_DEMO_TBL_DUMP_CMD_FC  (CMDMGR_APP_START_FC + 1)

#define MSGLOG_START_LOG_CMD_FC     (CMDMGR_APP_START_FC + 2) /* Child */
#define MSGLOG_STOP_LOG_CMD_FC      (CMDMGR_APP_START_FC + 3) /* Child */
#define MSGLOG_START_PLAYBK_CMD_FC  (CMDMGR_APP_START_FC + 4) /* Child */
#define MSGLOG_STOP_PLAYBK_CMD_FC   (CMDMGR_APP_START_FC + 5) /* Child */

#define MSGLOG_RUN_CHILD_ALT_CMD_FC (CMDMGR_APP_START_FC + 6) /* Child */  


/******************************************************************************
** Message Log Table Macros
*/

#define MSGLOGTBL_JSON_MAX_OBJ          10
#define MSGLOGTBL_JSON_FILE_MAX_CHAR  2000 

#define MSGLOGTBL_FILE_EXT_MAX_LEN       5  /* Includes '.' and null terminator */


/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define OSK_C_DEMO_BASE_EID  (OSK_C_FW_APP_BASE_EID +  0)
#define MSGLOG_BASE_EID      (OSK_C_FW_APP_BASE_EID + 20)
#define MSGLOGTBL_BASE_EID   (OSK_C_FW_APP_BASE_EID + 40)
#endif /* _app_cfg_ */

