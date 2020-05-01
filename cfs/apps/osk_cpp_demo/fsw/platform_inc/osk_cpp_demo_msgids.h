/*
** Purpose: Define message IDs for the C++ Demo Application
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
#ifndef _osk_cpp_demo_msgids_
#define _osk_cpp_demo_msgids_

/*
** Command Message IDs
*/

#define  OSK_CPP_DEMO_CMD_MID         0x1867
#define  OSK_CPP_DEMO_SEND_HK_MID     0x1885  // Same as KIT_CI to simplify integration

/*
** Telemetry Message IDs
*/

#define  OSK_CPP_DEMO_APP_TLM_HK_MID  0x0F05

#endif // _osk_cpp_demo_msgids_ 
