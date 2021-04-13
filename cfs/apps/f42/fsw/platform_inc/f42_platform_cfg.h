/*
** Purpose: Define platform configurations for the 42 FSW application
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application 
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/
#ifndef _f42_platform_cfg_
#define _f42_platform_cfg_

/*
** Includes
*/

#include "f42_mission_cfg.h"
#include "f42_msgids.h"
#include "f42_perfids.h"


/******************************************************************************
** Platform Deployment Configurations
*/

#define F42_PLATFORM_REV   0
#define F42_INI_FILENAME   "/cf/f42_ini.json"


/******************************************************************************
** F42 Application Macros
*/

#define  F42_SENSOR_TIMEOUT   1500  /* Pend timeout (ms) for sensor data read */
 
#define  F42_CTRL_TBL_DEF_LOAD_FILE  "/cf/f42_ctrl_tbl.json"

#define  F42_DBG_FILE  "/cf/f42_debug.txt"

#endif /* _f42_platform_cfg_ */
