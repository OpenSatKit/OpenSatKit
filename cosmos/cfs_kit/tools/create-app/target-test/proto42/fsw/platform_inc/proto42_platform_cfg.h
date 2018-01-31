/*
** Purpose: Define platform configurations for the Proto42 application
**
** Notes:
**   1. Template written by David McComas and licensed under the GNU
**      Lesser General Public License (LGPL).
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _proto42_platform_cfg_
#define _proto42_platform_cfg_

/*
** Includes
*/

#include "proto42_mission_cfg.h"
#include "proto42_msgids.h"
#include "proto42_perfids.h"

/******************************************************************************
** Demo Application Macros
*/

#define  PROTO42_RUNLOOP_DELAY    500  /* Delay in milliseconds */

#define  PROTO42_CMD_PIPE_DEPTH    10
#define  PROTO42_CMD_PIPE_NAME     "PROTO42"

/******************************************************************************
** Example Object Macros
*/

#define  PROTO42_EXTBL_DEF_LOAD_FILE  "/cf/extbl.txt"
#define  PROTO42_EXTBL_DEF_DUMP_FILE  "/cf/extbl_d.txt"

#endif /* _proto42_platform_cfg_ */
