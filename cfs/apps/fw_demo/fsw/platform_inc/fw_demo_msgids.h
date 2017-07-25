/*
** Purpose: Define message IDs for the OpenSat Kit Framework Demo application
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _fw_demo_msgids_
#define _fw_demo_msgids_

/*
** Command Message IDs
*/

#define  FW_DEMO_CMD_MID        0x1866
#define  FW_DEMO_SEND_HK_MID    0x1885  /* Same as KIT_CI to simplify integration */


/*
** Telemetry Message IDs
*/

#define  FW_DEMO_TLM_HK_MID     0x0F03

#endif /* _fw_demo_msgids_ */
