/*
** Purpose: Define message IDs for the OSK C Framework Demo application
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

#ifndef _osk_c_demo_msgids_
#define _osk_c_demo_msgids_

/*
** Command Message IDs
*/

#define  OSK_C_DEMO_CMD_MID        0x1866
#define  OSK_C_DEMO_SEND_HK_MID    0x1885  /* Same as KIT_CI to simplify integration */


/*
** Telemetry Message IDs
*/

#define  OSK_C_DEMO_TLM_HK_MID     0x0F03  /* Housekeeping */
#define  OSK_C_DEMO_TLM_FR_MID     0x0F04  /* FaultRep     */

#endif /* _osk_c_demo_msgids_ */
