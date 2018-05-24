/*
** Purpose: Define platform configurations for the OpenSat Kit Telemetry Output application
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _kit_to_msgids_
#define _kit_to_msgids_


/*
** Command Message IDs
*/

#define  KIT_TO_CMD_MID      (0x1880)
#define  KIT_TO_SEND_HK_MID  (0x1881)

/*
** Telemetry Message IDs
*/

#define  KIT_TO_HK_TLM_MID        (0x0880)
#define  KIT_TO_DATA_TYPE_TLM_MID (0x0881)

#endif /*_kit_to_msgids_*/
