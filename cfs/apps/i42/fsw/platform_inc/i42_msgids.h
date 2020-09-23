/*
** Purpose: Define message IDs for the 42 interface application
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application 
**   2. F42 definitions are included for the I42 app to use. This is backwards
**      from typical message producer/consumer but in this situation F42
**      drove the interface definition. 
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

#include "f42_msgids.h"

/*
** Macro Definitions
*/

/* Command Message IDs */ 

#define  I42_CMD_MID       (0x19E0)
#define  I42_EXECUTE_MID   (0x19E1)
#define  I42_SEND_HK_MID   (0x19E2)

/* Telemetry Message IDs */ 

#define  I42_HK_TLM_MID    (0x09E0)

#endif /*_i42_msgids_*/

