/*
** Purpose: Define message IDs for the application. This uses the same "standard" 
**          message ID's used by CFDP.
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
#ifndef _tftp_msgids_
#define _tftp_msgids_

/*
** Macro Definitions
*/

/* Command Message IDs */ 

#define  TFTP_CMD_MID       (0x18B5)
#define  TFTP_SEND_HK_MID   (0x18B6)   

/* Telemetry Message IDs */ 

#define  TFTP_HK_TLM_MID    (0x08B0)

#endif /*_tftp_msgids_*/

/* end of file */
