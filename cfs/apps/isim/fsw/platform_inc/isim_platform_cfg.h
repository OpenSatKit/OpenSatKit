/*
** Purpose: Define platform configurations for the Isim application
**
** Notes:
**   None
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
** Application Macros
*/

#define  ISIM_CMD_PIPE_DEPTH    10
#define  ISIM_CMD_PIPE_NAME     "ISIM_CMD_PIPE"

/******************************************************************************
** Example Object Macros
*/

#define  ISIMTBL_DEF_LOAD_FILE  "/cf/isim_tbl.json"
#define  ISIMTBL_DEF_DUMP_FILE  "/cf/isim_tbl~.json"

#endif /* _isim_platform_cfg_ */
