/*
** Purpose: Define configurations for the application framework
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**   2. Resources are statically allocated for each application's
**      data structures so these configurations muct be sized to 
**      accommodate the 'largest' application.
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _app_fw_platform_cfg_h_
#define _app_fw_platform_cfg_h_

#include "app_fw_mission_cfg.h"

/* 
** Mission specific version number
**
**  An application version number consists of four parts:
**  major version number, minor version number, revision
**  number and mission specific revision number. The mission
**  specific revision number is defined here and the other
**  parts are defined in "app_fw_version.h".
**
*/
#define APP_FW_LIB_MISSION_REV  (0)

#endif /* _app_fw_platform_cfg_h_ */

/************************/
/*  End of File Comment */
/************************/
