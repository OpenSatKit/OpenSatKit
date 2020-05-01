/*
** Purpose: Define configurations for the application framework
**
** Notes:
**   1. Resources are statically allocated for each application's
**      data structures so these configurations must be sized to 
**      accommodate the 'largest' application.
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

#ifndef _osk_c_fw_platform_cfg_h_
#define _osk_c_fw_platform_cfg_h_

#include "osk_c_fw_mission_cfg.h"

/* 
** Mission specific version number
**
**  An application version number consists of four parts:
**  major version number, minor version number, revision
**  number and mission specific revision number. The mission
**  specific revision number is defined here and the other
**  parts are defined in "osk_c_fw_ver.h".
**
*/
#define OSK_C_FW_MISSION_REV  (0)

#endif /* _osk_c_fw_platform_cfg_h_ */
