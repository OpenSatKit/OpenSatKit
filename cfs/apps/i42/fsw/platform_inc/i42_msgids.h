/*
** Purpose: Define message IDs for the 42 interface application
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
#ifndef _i42_msgids_
#define _i42_msgids_


/*
** Macro Definitions
*/

/* Command Message IDs */ 

#define  I42_CMD_MID       (0x19E0)
#define  I42_EXECUTE_MID   (0x19E1)
#define  I42_SEND_HK_MID   (0x19E2)

/* Telemetry Message IDs */ 

#define  I42_HK_TLM_MID            (0x09E0)
#define  I42_SENSOR_DATA_MID       (0x09E1)
#define  I42_ACTUATOR_CMD_DATA_MID (0x09E2)

#endif /*_i42_msgids_*/

