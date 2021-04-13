/*
** Purpose: Define message IDs for the Instrument Simulator App
**
** Notes:
**   1. See JSON init file.
**   2. ISIM_CMD_MID is defined in this header because it is needed by stored commands.
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

#define  ISIM_CMD_MID  0x19C0


/*
** Telemetry Message IDs
*/

#define  ISIM_TLM_HK_MID  0x09C0  /* Needed for LC, must match isim_ini.json definition */

#endif /* _isim_msgids_ */
