/*
** Purpose: Define platform configurations for the 42 interface application
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application 
**
** References:
**   1. OpenSat Object-based Application Developer's Guide
**   2. cFS Application Developer's Guide
**   3. 42/Docs/Standalone AcApp text file
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/
#ifndef _i42_platform_cfg_
#define _i42_platform_cfg_

/*
** Includes
*/

#include "i42_mission_cfg.h"
#include "i42_msgids.h"
#include "i42_perfids.h"


/******************************************************************************
** Platform Deployment Configurations
*/

#define I42_PLATFORM_REV   0
#define I42_INI_FILENAME   "/cf/i42_ini.json"


/******************************************************************************
** I42 Application Configurations
*/
#define  I42_APP_NAME "I42"

#define  I42_EXECUTE_MSG_CYCLES_MIN   1
#define  I42_EXECUTE_MSG_CYCLES_MAX  20
#define  I42_EXECUTE_MSG_CYCLES_DEF   1

#define  I42_EXECUTE_CYCLE_DELAY_MIN   10
#define  I42_EXECUTE_CYCLE_DELAY_MAX  250
#define  I42_EXECUTE_CYCLE_DELAY_DEF  200

#define  I42_EXECUTE_UNCLOSED_CYCLE_LIM 3   /* Number of 'unclosed' cycles before force child task to run */

#define  I42_LOCAL_HOST_STR   "localhost"
#define  I42_SOCKET_ADDR_STR  "127.000.000.001"
#define  I42_SOCKET_PORT      10001   /* 42's standalone default */

#define  I42_CHILD_NAME       "IF42_CHILD"
#define  I42_CHILD_SEM_NAME   "IF42_CHILD_SEM"
#define  I42_CHILD_STACK_SIZE  32768
#define  I42_CHILD_PRIORITY    60

#endif /* _i42_platform_cfg_ */
