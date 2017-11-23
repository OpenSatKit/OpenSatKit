/*
** Purpose: Define message IDs for the @Template@ application
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
#ifndef _@template@_msgids_
#define _@template@_msgids_

/*
** Command Message IDs
*/

#define  @TEMPLATE@_CMD_MID        0x1883
#define  @TEMPLATE@_SEND_HK_MID    0x1885  /* Same as KIT_CI to simplify integration */

/*
** Telemetry Message IDs
*/

#define  @TEMPLATE@_TLM_HK_MID     0x0883

#endif /* _@template@_msgids_ */
