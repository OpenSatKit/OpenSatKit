/*
** Purpose: Define message IDs for the Isim application
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
#ifndef _isim_msgids_
#define _isim_msgids_

/*
** Command Message IDs
*/

#define  ISIM_CMD_MID        0x1802
#define  ISIM_SEND_HK_MID    0x1885  /* Same as KIT_CI to simplify integration */

/*
** Telemetry Message IDs
*/

#define  ISIM_TLM_HK_MID     0x0882

#endif /* _isim_msgids_ */
