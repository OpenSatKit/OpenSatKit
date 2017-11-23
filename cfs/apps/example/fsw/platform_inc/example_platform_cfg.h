/*
** Purpose: Define platform configurations for the Example application
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

#ifndef _example_platform_cfg_
#define _example_platform_cfg_

/*
** Includes
*/

#include "example_mission_cfg.h"
#include "example_msgids.h"
#include "example_perfids.h"

/******************************************************************************
** Demo Application Macros
*/

#define  EXAMPLE_RUNLOOP_DELAY    500  /* Delay in milliseconds */

#define  EXAMPLE_CMD_PIPE_DEPTH    10
#define  EXAMPLE_CMD_PIPE_NAME     "EXAMPLE_CMD_PIPE"

/******************************************************************************
** Example Object Macros
*/

#define  EXAMPLE_EXTBL_DEF_LOAD_FILE  "/cf/example_extbl.json"
#define  EXAMPLE_EXTBL_DEF_DUMP_FILE  "/cf/example_extbl_d.json"

#endif /* _example_platform_cfg_ */
