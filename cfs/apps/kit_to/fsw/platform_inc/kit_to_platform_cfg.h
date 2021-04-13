/*
** Purpose: Define platform configurations for the OpenSat Kit Telemetry Output
**          application
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _kit_to_platform_cfg_
#define _kit_to_platform_cfg_

/*
** Includes
*/

#include "kit_to_mission_cfg.h"
#include "kit_to_msgids.h"
#include "kit_to_perfids.h"

/******************************************************************************
** Platform Deployment Configurations
*/

#define KIT_TO_PLATFORM_REV   0
#define KIT_TO_INI_FILENAME   "/cf/kit_to_ini.json"


/******************************************************************************
** Telemetry Output KIT_TO Application Macros
*/

#define  KIT_TO_STARTUP_SYNCH_TIMEOUT  10000  /* Timeout in milliseconds */

#define  KIT_TO_RUN_LOOP_DELAY_MS        500  /* Default delay in milliseconds */
#define  KIT_TO_MIN_RUN_LOOP_DELAY_MS    200  /* Minimum command value */
#define  KIT_TO_MAX_RUN_LOOP_DELAY_MS  10000  /* Maximum command value */

#define  KIT_TO_TLM_PORT  1235

#define  KIT_TO_DEF_PKTTBL_FILENAME  "/cf/osk_to_pkt_tbl.json"

#endif /* _kit_to_platform_cfg_ */
