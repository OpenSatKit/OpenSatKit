/*
** Purpose: Define platform configurations for the Test2 application
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

#ifndef _test2_platform_cfg_
#define _test2_platform_cfg_

/*
** Includes
*/

#include "test2_mission_cfg.h"
#include "test2_msgids.h"
#include "test2_perfids.h"

/******************************************************************************
** Demo Application Macros
*/

#define  TEST2_RUNLOOP_DELAY    500  /* Delay in milliseconds */

#define  TEST2_CMD_PIPE_DEPTH    10
#define  TEST2_CMD_PIPE_NAME     "TEST2"

/******************************************************************************
** Example Object Macros
*/

#define  TEST2_EXTBL_DEF_LOAD_FILE  "/cf/extbl.txt"
#define  TEST2_EXTBL_DEF_DUMP_FILE  "/cf/extbl_d.txt"

#endif /* _test2_platform_cfg_ */
