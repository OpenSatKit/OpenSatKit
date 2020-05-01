/*
** Purpose: Define application configurations for the 42 interface
**          application
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application 
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
**
*/
#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "i42_platform_cfg.h"
#include "osk_c_fw.h"

/******************************************************************************
** I42 Application Macros
*/

#define  I42_MAJOR_VER     1
#define  I42_MINOR_VER     1  /* Refactored for OSK 2.2 */
#define  I42_LOCAL_REV     0

#define  I42_CMD_PIPE_DEPTH     5
#define  I42_CMD_PIPE_NAME      "I42_CMD_PIPE"

#define  I42_ACTUATOR_PIPE_DEPTH  5
#define  I42_ACTUATOR_PIPE_NAME   "I42_ACTUATOR_PIPE"

/******************************************************************************
** Command Macros
*/

#define I42_NETIF_CONNECT_42_CMD_FC     (CMDMGR_APP_START_FC + 0)
#define I42_NETIF_DISCONNECT_42_CMD_FC  (CMDMGR_APP_START_FC + 1)


/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/


#define I42_BASE_EID    (OSK_C_FW_APP_BASE_EID +  0)
#define NETIF_BASE_EID  (OSK_C_FW_APP_BASE_EID + 10)  /* Uses more than 10 IDs */

/*
** One event ID is used for all initialization debug messages. Uncomment one of
** the I42_INIT_EVS_TYPE definitions. Set it to INFORMATION if you want to
** see the events during initialization. This is opposite to what you'd expect 
** because INFORMATION messages are enabled by default when an app is loaded.
*/

#define I42_INIT_DEBUG_EID 999
#define I42_INIT_EVS_TYPE CFE_EVS_DEBUG
//#define I42_INIT_EVS_TYPE CFE_EVS_INFORMATION


/******************************************************************************
** NET_IF Macros
*/

#define NETIF_RECV_BUFF_LEN 512



#endif /* _app_cfg_ */
