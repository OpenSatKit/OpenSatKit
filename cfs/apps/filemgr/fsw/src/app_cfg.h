/*
** Purpose: Define application configurations for the File Manager (FILEMGR)
**          application
**
** Notes:
**   1. FILEMGR is a refactoring of NASA's FM app using OSK's app framework.
**      It's also a prototype for using a JSON init file for application 
**      parameters that can be specified during runtime. 
**   2. These macros can only be build with the application and can't
**      have a platform scope because the same app_cfg.h file name is used for
**      all applications following the object-based application design.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
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

#include "filemgr_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
** Application Macros
*/

/*
** Versions:
**
** 1.0 - Initial refactoring of open source FM
** 1.1 - Moved childmgr utility into osk_c_fw, Moved perf & msg ids to ini file
*/

#define  FILEMGR_MAJOR_VER      1
#define  FILEMGR_MINOR_VER      1


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

#define CFG_APP_CFE_NAME           APP_CFE_NAME

#define CFG_APP_MAIN_PERF_ID       APP_MAIN_PERF_ID
#define CFG_CHILD_TASK_PERF_ID     CHILD_TASK_PERF_ID

#define CFG_CMD_MID                CMD_MID
#define CFG_SEND_HK_MID            SEND_HK_MID

#define CFG_HK_TLM_MID             HK_TLM_MID
#define CFG_FILE_INFO_TLM_MID      FILE_INFO_TLM_MID
#define CFG_DIR_LIST_TLM_MID       DIR_LIST_TLM_MID
#define CFG_OPEN_FILES_TLM_MID     OPEN_FILES_TLM_MID
#define CFG_FILESYS_TLM_MID        FILESYS_TLM_MID
      
#define CFG_CMD_PIPE_DEPTH         CMD_PIPE_DEPTH
#define CFG_CMD_PIPE_NAME          CMD_PIPE_NAME
      
#define CFG_TBL_CFE_NAME           TBL_CFE_NAME
#define CFG_TBL_DEF_FILENAME       TBL_DEF_FILENAME
#define CFG_TBL_ERR_CODE           TBL_ERR_CODE
      
#define CFG_DIR_LIST_FILE_DEFNAME  DIR_LIST_FILE_DEFNAME
#define CFG_DIR_LIST_FILE_SUBTYPE  DIR_LIST_FILE_SUBTYPE
#define CFG_DIR_LIST_FILE_ENTRIES  DIR_LIST_FILE_ENTRIES
      
#define CFG_CHILD_NAME             CHILD_NAME
#define CFG_CHILD_STACK_SIZE       CHILD_STACK_SIZE
#define CFG_CHILD_PRIORITY         CHILD_PRIORITY
      
#define CFG_TASK_FILE_BLOCK_CNT    TASK_FILE_BLOCK_CNT
#define CFG_TASK_FILE_BLOCK_DELAY  TASK_FILE_BLOCK_DELAY
#define CFG_TASK_FILE_STAT_CNT     TASK_FILE_STAT_CNT
#define CFG_TASK_FILE_STAT_DELAY   TASK_FILE_STAT_DELAY


#define APP_CONFIG(XX) \
   XX(APP_CFE_NAME,char*) \
   XX(APP_MAIN_PERF_ID,uint32) \
   XX(CHILD_TASK_PERF_ID,uint32) \
   XX(CMD_MID,uint32) \
   XX(SEND_HK_MID,uint32) \
   XX(HK_TLM_MID,uint32) \
   XX(FILE_INFO_TLM_MID,uint32) \
   XX(DIR_LIST_TLM_MID,uint32) \
   XX(OPEN_FILES_TLM_MID,uint32) \
   XX(FILESYS_TLM_MID,uint32) \
   XX(CMD_PIPE_DEPTH,uint32) \
   XX(CMD_PIPE_NAME,char*) \
   XX(TBL_CFE_NAME,char*) \
   XX(TBL_DEF_FILENAME,char*) \
   XX(TBL_ERR_CODE,uint32) \
   XX(DIR_LIST_FILE_DEFNAME,char*) \
   XX(DIR_LIST_FILE_SUBTYPE,uint32) \
   XX(DIR_LIST_FILE_ENTRIES,uint32) \
   XX(CHILD_NAME,char*) \
   XX(CHILD_STACK_SIZE,uint32) \
   XX(CHILD_PRIORITY,uint32) \
   XX(TASK_FILE_BLOCK_CNT,uint32) \
   XX(TASK_FILE_BLOCK_DELAY,uint32) \
   XX(TASK_FILE_STAT_CNT,uint32) \
   XX(TASK_FILE_STAT_DELAY,uint32) \

DECLARE_ENUM(Config,APP_CONFIG)


/******************************************************************************
** Command Macros
** - Commands implmented by child task are annotated with a comment
** - Load/dump table definitions are placeholders for JSON table
*/

#define FILEMGR_TBL_LOAD_CMD_FC            (CMDMGR_APP_START_FC +  0)
#define FILEMGR_TBL_DUMP_CMD_FC            (CMDMGR_APP_START_FC +  1)

#define FILE_COPY_CMD_FC                   (CMDMGR_APP_START_FC +  2) /* Child */
#define FILE_MOVE_CMD_FC                   (CMDMGR_APP_START_FC +  3) /* Child */
#define FILE_RENAME_CMD_FC                 (CMDMGR_APP_START_FC +  4) /* Child */
#define FILE_DELETE_CMD_FC                 (CMDMGR_APP_START_FC +  5) /* Child */
#define FILE_DECOMPRESS_CMD_FC             (CMDMGR_APP_START_FC +  6) /* Child */
#define FILE_CONCAT_CMD_FC                 (CMDMGR_APP_START_FC +  7) /* Child */
#define FILE_DELETE_ALT_CMD_FC             (CMDMGR_APP_START_FC +  8) /* Child */
#define FILE_SET_PERMISSIONS_CMD_FC        (CMDMGR_APP_START_FC +  9) /* Child */
#define FILE_SEND_INFO_CMD_FC              (CMDMGR_APP_START_FC + 10) /* Child */

#define DIR_CREATE_CMD_FC                  (CMDMGR_APP_START_FC + 11) /* Child */
#define DIR_DELETE_CMD_FC                  (CMDMGR_APP_START_FC + 12) /* Child */
#define DIR_DELETE_ALL_CMD_FC              (CMDMGR_APP_START_FC + 13) /* Child */
#define DIR_WRITE_LIST_FILE_CMD_FC         (CMDMGR_APP_START_FC + 14) /* Child */
#define DIR_SEND_LIST_PKT_CMD_FC           (CMDMGR_APP_START_FC + 15) /* Child */

#define FILESYS_SEND_OPEN_FILES_PKT_CMD_FC (CMDMGR_APP_START_FC + 16)
#define FILESYS_SEND_TBL_PKT_CMD_FC        (CMDMGR_APP_START_FC + 17)
#define FILESYS_SET_TBL_STATE_CMD_FC       (CMDMGR_APP_START_FC + 18)

/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define FILEMGR_BASE_EID  (OSK_C_FW_APP_BASE_EID +  0)
#define DIR_BASE_EID      (OSK_C_FW_APP_BASE_EID + 20)
#define FILE_BASE_EID     (OSK_C_FW_APP_BASE_EID + 40)
#define FILESYS_BASE_EID  (OSK_C_FW_APP_BASE_EID + 60)


#endif /* _app_cfg_ */
