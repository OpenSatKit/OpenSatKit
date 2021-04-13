/*
** Purpose: Define platform configurations for the OpenSatKit Telemetry Output
**          application
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

#define  KIT_TO_CMD_MID      (0x1F20)
#define  KIT_TO_SEND_HK_MID  (0x1F21)

/*
** Telemetry Message IDs
*/

#define  KIT_TO_HK_TLM_MID        (0x0F20)
#define  KIT_TO_PKT_TBL_TLM_MID   (0x0F21)
#define  KIT_TO_DATA_TYPE_TLM_MID (0x0F22)
#define  KIT_TO_EVS_LOG_TLM_MID   (0x0F23)

#endif /*_kit_to_msgids_*/
