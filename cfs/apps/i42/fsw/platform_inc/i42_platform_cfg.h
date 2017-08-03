/*
** Purpose: Define platform configurations for the 42 interface application
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
#ifndef _i42_platform_cfg_
#define _i42_platform_cfg_

/*
** Includes
*/

#include "i42_mission_cfg.h"
#include "i42_msgids.h"
#include "i42_perfids.h"

/******************************************************************************
** I42 Application Configurations
*/

#define  I42_APP_ACTUATOR_TIMEOUT   1500  /* Pend timeout (ms) for actuator data read */
 

/******************************************************************************
** I42 Network Interface Configurations
*/

#define  I42_NETIF_DISCONNECT_CYCLES  10  /* Number of 'no data' sensor data read attempts before disconnect */

#endif /* _i42_platform_cfg_ */
