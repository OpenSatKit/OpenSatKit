/*
** Purpose: Define configurations for the File Output
**          application
**
** Notes:
**   1. These configurations should have an application scope and define
**      parameters that shouldn't need to change across deployments. If
**      a change is made to this file or any other app source file during
**      a deployment then the definition of the FILE_UP_PLATFORM_REV
**      macro in file_up_platform_cfg.h should be updated.
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide and the
**      osk_c_demo app that illustrates best practices with comments.  
**   2. cFS Application Developer's Guide.
**
**   Written by David McComas, licensed under the Apache License, Version 2.0
**   (the "License"); you may not use this file except in compliance with the
**   License. You may obtain a copy of the License at
**
**      http://www.apache.org/licenses/LICENSE-2.0
**
**   Unless required by applicable law or agreed to in writing, software
**   distributed under the License is distributed on an "AS IS" BASIS,
**   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**   See the License for the specific language governing permissions and
**   limitations under the License.
*/

#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "file_out_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
**
** Versions:
**
** 1.0 - Initial release
*/

#define  FILE_OUT_MAJOR_VER   1
#define  FILE_OUT_MINOR_VER   0


/******************************************************************************
** JSON init file definitions/declarations.
**    
*/

#define CFG_APP_CFE_NAME           APP_CFE_NAME
#define CFG_APP_PERF_ID            APP_PERF_ID

#define CFG_APP_CMD_PIPE_NAME      APP_CMD_PIPE_NAME
#define CFG_APP_CMD_PIPE_DEPTH     APP_CMD_PIPE_DEPTH

#define CFG_APP_CMD_MID            APP_CMD_MID
#define CFG_APP_SEND_HK_MID        APP_SEND_HK_MID
#define CFG_APP_EXECUTE_MID        APP_EXECUTE_MID

#define CFG_APP_HK_TLM_MID               APP_HK_TLM_MID
#define CFG_FOTP_START_TRANSFER_TLM_MID  FOTP_START_TRANSFER_TLM_MID
#define CFG_FOTP_DATA_SEGMENT_TLM_MID    FOTP_DATA_SEGMENT_TLM_MID
#define CFG_FOTP_FINISH_TRANSFER_TLM_MID FOTP_FINISH_TRANSFER_TLM_MID
 
#define APP_CONFIG(XX) \
   XX(APP_CFE_NAME,char*) \
   XX(APP_PERF_ID,uint32) \
   XX(APP_CMD_PIPE_NAME,char*) \
   XX(APP_CMD_PIPE_DEPTH,uint32) \
   XX(APP_CMD_MID,uint32) \
   XX(APP_SEND_HK_MID,uint32) \
   XX(APP_EXECUTE_MID,uint32) \
   XX(APP_HK_TLM_MID,uint32) \
   XX(FOTP_START_TRANSFER_TLM_MID,uint32) \
   XX(FOTP_DATA_SEGMENT_TLM_MID,uint32) \
   XX(FOTP_FINISH_TRANSFER_TLM_MID,uint32) \
   
DECLARE_ENUM(Config,APP_CONFIG)


/******************************************************************************
** Command Macros
*/

/* File Output Transfer Protocol */

#define FOTP_START_TRANSFER_CMD_FC    (CMDMGR_APP_START_FC + 0)
#define FOTP_PAUSE_TRANSFER_CMD_FC    (CMDMGR_APP_START_FC + 1)
#define FOTP_RESUME_TRANSFER_CMD_FC   (CMDMGR_APP_START_FC + 2)
#define FOTP_CANCEL_TRANSFER_CMD_FC   (CMDMGR_APP_START_FC + 3)


/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define FILE_OUT_APP_BASE_EID  (OSK_C_FW_APP_BASE_EID +  0)
#define FOTP_BASE_EID          (OSK_C_FW_APP_BASE_EID + 20)


/******************************************************************************
** File Outut Transfer Protocol (FOTP)
*/

#define FOTP_FILENAME_LEN          (OS_MAX_PATH_LEN)
#define FOTP_DATA_SEG_MIN_LEN         8   /* Must be an even number since it is used in word-aligned telemetry*/
#define FOTP_DATA_SEG_MAX_LEN      1024   /* Must be an even number since it is used in word-aligned telemetry */
#define FOTP_DATA_SEGMENT_ID_START    1 


#endif /* _app_cfg_ */
