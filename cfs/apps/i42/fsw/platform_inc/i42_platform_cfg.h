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

#define  I42_ACTUATOR_TIMEOUT              1500  /* Pend timeout (ms) for actuator data read */
#define  I42_NO_SENSOR_DISCONNECT_LIM      2000  /* Number of 'no data' sensor data read attempts before disconnect */
#define  I42_NO_SENSOR_RESEND_ACTUATOR_LIM   50  /* Number of 'no data' sensor data read attempts before resend actutaor packet */

#define  I42_LOCAL_HOST_STR   "localhost"
#define  I42_SOCKET_ADDR_STR  "127.000.000.001"
#define  I42_SOCKET_PORT      4242
#define  I42_SOCKET_BUF_LEN   1024

#endif /* _i42_platform_cfg_ */
