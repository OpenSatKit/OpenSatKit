/*
** Purpose: Define message IDs for the @Template@ application
**
** Notes:
**   1. Default to OSK test IDs. These IDs should be changed if the app
**      is integrated with the user's system.
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

#define  @TEMPLATE@_CMD_MID        0x1FF0
#define  @TEMPLATE@_SEND_HK_MID    0x1A06   /* Generic 4s scheduled packet */

/*
** Telemetry Message IDs
*/

#define  @TEMPLATE@_HK_TLM_MID     0x0FF0

#endif /* _@template@_msgids_ */
