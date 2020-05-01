/*
** Purpose: Define platform configurations for the OSK C Framework Demo application
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

#ifndef _osk_c_demo_platform_cfg_
#define _osk_c_demo_platform_cfg_

/*
** Includes
*/

#include "osk_c_demo_mission_cfg.h"
#include "osk_c_demo_msgids.h"
#include "osk_c_demo_perfids.h"

/******************************************************************************
** Framework Demo Application Macros
*/

#define  OSK_C_DEMO_RUNLOOP_DELAY    500  /* Delay in milliseconds */

#define  OSK_C_DEMO_CMD_PIPE_DEPTH    10
#define  OSK_C_DEMO_CMD_PIPE_NAME    "OSK_C_DEMO_CMD_PIPE"

/******************************************************************************
** Example Object Macros
*/

#define  OSK_C_DEMO_XML_TBL_DEF_LOAD_FILE   "/cf/osk_c_demo_tbl.xml"
#define  OSK_C_DEMO_XML_TBL_DEF_DUMP_FILE   "/cf/osk_c_demo_tbl_d.txt"

#define  OSK_C_DEMO_SCANF_TBL_DEF_LOAD_FILE "/cf/osk_c_demo_tbl.scanf"
#define  OSK_C_DEMO_SCANF_TBL_DEF_DUMP_FILE "/cf/osk_c_demo_tbl_d.scanf"

#define  OSK_C_DEMO_JSON_TBL_DEF_LOAD_FILE  "/cf/osk_c_demo_tbl.json"
#define  OSK_C_DEMO_JSON_TBL_DEF_DUMP_FILE  "/cf/osk_c_demo_tbl_d.json"

#endif /* _osk_c_demo_platform_cfg_ */
