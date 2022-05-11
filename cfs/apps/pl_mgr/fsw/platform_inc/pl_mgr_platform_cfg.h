/*
** Purpose: Define platform configurations for the Payload Manager App
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

#ifndef _pl_mgr_platform_cfg_
#define _pl_mgr_platform_cfg_

/*
** Includes
*/

#include "pl_mgr_mission_cfg.h"
#include "pl_mgr_msgids.h"
#include "pl_mgr_perfids.h"


/******************************************************************************
** Platform Deployment Configurations
*/

#define PL_MGR_PLATFORM_REV   0
#define PL_MGR_INI_FILENAME   "/cf/pl_mgr_app_ini.json"


#endif /* _pl_mgr_platform_cfg_ */
