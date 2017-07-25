/*
** Purpose: Define platform configurations for the OpenSat Kit Framework Demo application
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _fw_demo_platform_cfg_
#define _fw_demo_platform_cfg_

/*
** Includes
*/

#include "fw_demo_mission_cfg.h"
#include "fw_demo_msgids.h"
#include "fw_demo_perfids.h"

/******************************************************************************
** Framework Demo Application Macros
*/

#define  FW_DEMO_RUNLOOP_DELAY    500  /* Delay in milliseconds */

#define  FW_DEMO_CMD_PIPE_DEPTH    10
#define  FW_DEMO_CMD_PIPE_NAME     "FW_DEMO_CMD_PIPE"

/******************************************************************************
** Example Object Macros
*/

#define  FW_DEMO_XML_TBL_DEF_LOAD_FILE  "/cf/fw_demo_xmltbl.xml"
#define  FW_DEMO_XML_TBL_DEF_DUMP_FILE  "/cf/fw_demo_xmltbl.txt"

#define  FW_DEMO_SCANF_TBL_DEF_LOAD_FILE  "/cf/fw_demo_scanftbl.txt"
#define  FW_DEMO_SCANF_TBL_DEF_DUMP_FILE  "/cf/fw_demo_scanftbl_d.txt"

#endif /* _fw_demo_platform_cfg_ */
