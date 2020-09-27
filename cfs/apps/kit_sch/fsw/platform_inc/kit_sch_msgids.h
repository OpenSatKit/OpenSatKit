/*
** Purpose: Define platform configurations for the OpenSat Kit Scheduler application
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
#ifndef _kit_sch_msgids_
#define _kit_sch_msgids_


/*
** Command Message IDs
*/

#define  KIT_SCH_CMD_MID       (0x1F10)
#define  KIT_SCH_SEND_HK_MID   (0x1F11)

/*
** Telemetry Message IDs
*/

#define  KIT_SCH_HK_TLM_MID        (0x0F10)
#define  KIT_SCH_DIAG_TLM_MID      (0x0F11)
#define  KIT_SCH_TBL_ENTRY_TLM_MID (0x0F12)

#endif /*_kit_sch_msgids_*/
