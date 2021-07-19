/*
** Purpose: Define platform configurations for the OSK C Demo application
**
** Notes:
**   1. These definitions should minimal and only contain parameters that
**      need to be configurable and that must be defined at compile time.  
**      Use app_cfg.h for compile-time parameters that don't need to be
**      configured when an app is deployed and the JSON initialization
**      file for parameters that can be defined at runtime.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Application Developer's Guide
**   2. cFS Application Developer's Guide
**
*/

#ifndef _osk_c_demo_platform_cfg_
#define _osk_c_demo_platform_cfg_

/*
** Includes
*/

#include "osk_c_demo_mission_cfg.h"

/******************************************************************************
** Platform Deployment Configurations
*/

#define OSK_C_DEMO_PLATFORM_REV   0
#define OSK_C_DEMO_INI_FILENAME   "/cf/osk_c_demo_ini.json"


#endif /* _osk_c_demo_platform_cfg_ */
