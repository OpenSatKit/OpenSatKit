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

#include "f42_msgids.h"

/*
** Macro Definitions
*/

/* Command Message IDs */ 

#define  I42_CMD_MID       (0x19D2)
#define  I42_SEND_HK_MID   (0x19D3)

/* Telemetry Message IDs */ 

#define  I42_HK_TLM_MID    (0x09D2)

#endif /*_i42_msgids_*/

/* end of file */
