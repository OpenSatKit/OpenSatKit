/*
** Purpose: Define platform configurations for the C++ demo application
**
** Notes:
**   None
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _osk_cpp_demo_platform_cfg_
#define _osk_cpp_demo_platform_cfg_

/*
** Includes
*/

#include "osk_cpp_demo_mission_cfg.h"
#include "osk_cpp_demo_msgids.h"
#include "osk_cpp_demo_perfids.h"

/******************************************************************************
** Application Macros
*/

#define  OSK_CPP_DEMO_APP_NAME         "OSK_CPP_DEMO"

#define  OSK_CPP_DEMO_APP_LOCAL_REV    0

#define  OSK_CPP_DEMO_RUNLOOP_DELAY    500  // Delay in milliseconds

#define  OSK_CPP_DEMO_CMD_PIPE_DEPTH   10
#define  OSK_CPP_DEMO_CMD_PIPE_NAME    "CPP_CMD_PIPE"

#define  OSK_CPP_DEMO_JSON_TBL_DEF_LOAD_FILE "/cf/osk_cpp_tbl.json"

#define  OSK_CPP_DEMO_CFE_TBL_NAME           "Example"
#define  OSK_CPP_DEMO_CFE_TBL_FILENAME       "osk_cpp_demo.tbl"
#define  OSK_CPP_DEMO_CFE_TBL_DEF_LOAD_FILE  "/cf/osk_cpp_demo.tbl"
   
#endif // _osk_cpp_demo_platform_cfg_ 
