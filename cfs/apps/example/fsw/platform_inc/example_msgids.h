/*
** Purpose: Define message IDs for the Example application
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _example_msgids_
#define _example_msgids_

/*
** Command Message IDs
*/

#define  EXAMPLE_CMD_MID        0x1883
#define  EXAMPLE_SEND_HK_MID    0x1885  /* Same as KIT_CI to simplify integration */

/*
** Telemetry Message IDs
*/

#define  EXAMPLE_TLM_HK_MID     0x0883

#endif /* _example_msgids_ */
