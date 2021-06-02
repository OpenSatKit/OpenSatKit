/*
** Purpose: Define platform configurations for the OSK C Framework Proto application
**
** Notes:
**   None
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _osk_c_proto_platform_cfg_
#define _osk_c_proto_platform_cfg_

/*
** Includes
*/

#include "osk_c_proto_mission_cfg.h"
#include "osk_c_proto_msgids.h"
#include "osk_c_proto_perfids.h"

/******************************************************************************
** Platform Deployment Configurations
*/

#define OSK_C_PROTO_PLATFORM_REV   0
#define OSK_C_PROTO_INI_FILENAME   "/cf/osk_c_proto_ini.json"


/******************************************************************************
** Framework Proto Application Macros
*/

#define  OSK_C_PROTO_APP_NAME         "OSK_C_PROTO"

#define  OSK_C_PROTO_RUNLOOP_DELAY    500  /* Delay in milliseconds */

#define  OSK_C_PROTO_CMD_PIPE_DEPTH    10
#define  OSK_C_PROTO_CMD_PIPE_NAME    "C_PROTO_CMDS"

/******************************************************************************
** Example Object Macros
*/

#define  OSK_C_PROTO_XML_TBL_DEF_LOAD_FILE   "/cf/osk_c_proto_tbl.xml"
#define  OSK_C_PROTO_XML_TBL_DEF_DUMP_FILE   "/cf/osk_c_proto_tbl_d.txt"

#define  OSK_C_PROTO_SCANF_TBL_DEF_LOAD_FILE "/cf/osk_c_proto_tbl.scanf"
#define  OSK_C_PROTO_SCANF_TBL_DEF_DUMP_FILE "/cf/osk_c_proto_tbl_d.scanf"

#define  OSK_C_PROTO_JSON_TBL_DEF_LOAD_FILE  "/cf/osk_c_proto_tbl.json"
#define  OSK_C_PROTO_JSON_TBL_DEF_DUMP_FILE  "/cf/osk_c_proto_tbl_d.json"

#define  OSK_C_PROTO_CFE_TBL_NAME            "Example"
#define  OSK_C_PROTO_CFE_TBL_FILENAME        "osk_c_proto.tbl"
#define  OSK_C_PROTO_CFE_TBL_DEF_LOAD_FILE   "/cf/osk_c_proto.tbl"


#endif /* _osk_c_proto_platform_cfg_ */
