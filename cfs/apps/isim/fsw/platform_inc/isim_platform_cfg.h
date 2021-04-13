/*
** Purpose: Define platform configurations for the Instrument Simulator App
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

#ifndef _isim_platform_cfg_
#define _isim_platform_cfg_

/*
** Includes
*/

#include "isim_mission_cfg.h"
#include "isim_msgids.h"
#include "isim_perfids.h"


/******************************************************************************
** Platform Deployment Configurations
*/

#define ISIM_PLATFORM_REV   0
#define ISIM_INI_FILENAME   "/cf/isim_ini.json"


/******************************************************************************
** ISIM Table Object Macros
*/

#define  ISIMTBL_DEF_LOAD_FILE  "/cf/isim_tbl.json"
#define  ISIMTBL_DEF_DUMP_FILE  "/cf/isim_tbl~.json"

#endif /* _isim_platform_cfg_ */
