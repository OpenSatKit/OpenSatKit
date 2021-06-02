/*
** Purpose: Define platform configurations for the C++ proto application
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

#ifndef _osk_cpp_proto_platform_cfg_
#define _osk_cpp_proto_platform_cfg_

/*
** Includes
*/

#include "osk_cpp_proto_mission_cfg.h"
#include "osk_cpp_proto_msgids.h"
#include "osk_cpp_proto_perfids.h"

/******************************************************************************
** Platform Deployment Configurations
*/

#define OSK_CPP_PROTO_PLATFORM_REV   0
#define OSK_CPP_PROTO_INI_FILENAME   "/cf/osk_cpp_proto_ini.json"


/******************************************************************************
** Application Macros
*/

#define  OSK_CPP_PROTO_APP_NAME         "OSK_CPP_PROTO"

#define  OSK_CPP_PROTO_RUNLOOP_DELAY    500  // Delay in milliseconds

#define  OSK_CPP_PROTO_CMD_PIPE_DEPTH   10
#define  OSK_CPP_PROTO_CMD_PIPE_NAME    "CPP_PROTO_CMDS"

#define  OSK_CPP_PROTO_JSON_TBL_DEF_LOAD_FILE "/cf/osk_cpp_tbl.json"

#define  OSK_CPP_PROTO_CFE_TBL_NAME           "Example"
#define  OSK_CPP_PROTO_CFE_TBL_FILENAME       "osk_cpp_proto.tbl"
#define  OSK_CPP_PROTO_CFE_TBL_DEF_LOAD_FILE  "/cf/osk_cpp_proto.tbl"
   
#endif // _osk_cpp_proto_platform_cfg_ 
