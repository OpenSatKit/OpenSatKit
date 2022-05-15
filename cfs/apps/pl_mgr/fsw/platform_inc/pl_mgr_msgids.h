/*
** Purpose: Define message IDs for the Playload Manager App
**
** Notes:
**   1. See JSON init file.
**   2. PL_MGR_CMD_MID is defined in this header because it is needed by stored commands.
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
#ifndef _pl_mgr_msgids_
#define _pl_mgr_msgids_

/*
** Command Message IDs
*/

#define  PL_MGR_CMD_MID  0x19C0


/*
** Telemetry Message IDs
*/

#define  PL_MGR_TLM_HK_MID  0x09C0  /* Needed for LC, must match pl_mgr_ini.json definition */

#endif /* _pl_mgr_msgids_ */
