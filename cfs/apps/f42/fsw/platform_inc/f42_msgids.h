/*
** Purpose: Define message IDs for the 42 FSW application
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application 
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/
#ifndef _f42_msgids_
#define _f42_msgids_

/*
** Macro Definitions
*/

/* Command Message IDs */ 

#define  F42_CMD_MID      (0x19D0)
#define  F42_SEND_HK_MID  (0x19D1)   

/* Telemetry Message IDs */ 

#define  F42_HK_TLM_MID         (0x09D0)
#define  F42_CTRL_TLM_MID       (0x09D1)
#define  F42_CTRL_GAINS_TLM_MID (0x09D2)

#endif /*_f42_msgids_*/

