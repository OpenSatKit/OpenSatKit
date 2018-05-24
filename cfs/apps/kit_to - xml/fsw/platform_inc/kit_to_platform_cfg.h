/*
** Purpose: Define platform configurations for the OpenSat Kit Telemetry Output application
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
** Telemetry Output KIT_TO Application Macros
*/

#define  KIT_TO_RUNLOOP_DELAY    500  /* Delay in milliseconds */

#define  KIT_TO_TLM_PORT        1235

#define  KIT_TO_DEF_PKTTBL_FILE_NAME "/cf/kit_to_pkttbl.xml"


/******************************************************************************
** Uplink object Macros
*/

#define  UPLINK_RECV_BUFF_LEN  512

#endif /* _kit_to_platform_cfg_ */
