/*
** Purpose: Define platform configurations for the @Template@ application
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

#ifndef _@template@_platform_cfg_
#define _@template@_platform_cfg_

/*
** Includes
*/

#include "@template@_mission_cfg.h"
#include "@template@_msgids.h"
#include "@template@_perfids.h"

/******************************************************************************
** Demo Application Macros
*/

#define  @TEMPLATE@_RUNLOOP_DELAY    500  /* Delay in milliseconds */

#define  @TEMPLATE@_CMD_PIPE_DEPTH    10
#define  @TEMPLATE@_CMD_PIPE_NAME     "@TEMPLATE@"

/******************************************************************************
** Example Object Macros
*/

#define  @TEMPLATE@_EXTBL_DEF_LOAD_FILE  "/cf/extbl.txt"
#define  @TEMPLATE@_EXTBL_DEF_DUMP_FILE  "/cf/extbl_d.txt"

#endif /* _@template@_platform_cfg_ */
