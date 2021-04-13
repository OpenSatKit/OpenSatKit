/*
** Purpose: Define platform configurations for the Spacecraft Simulator App
**
** Notes:
**   1. See JSON init file for runtime configurations.
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

#ifndef _scsim_platform_cfg_
#define _scsim_platform_cfg_

/*
** Includes
*/

#include "scsim_mission_cfg.h"
#include "scsim_msgids.h"
#include "scsim_perfids.h"


/******************************************************************************
** Platform Deployment Configurations
*/

#define SCSIM_PLATFORM_REV   0
#define SCSIM_INI_FILENAME   "/cf/scsim_ini.json"


/******************************************************************************
** SCSIM Table Object Macros
*/

#define  SCSIMTBL_DEF_LOAD_FILE  "/cf/scsim_tbl.json"
#define  SCSIMTBL_DEF_DUMP_FILE  "/cf/scsim_tbl~.json"

#endif /* _scsim_platform_cfg_ */
