/*
** Purpose: Define message IDs for the application. This uses the same "standard" 
**          message ID's used by CFDP.
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
#ifndef _tftp_msgids_
#define _tftp_msgids_

/*
** Macro Definitions
*/

/* Command Message IDs */ 

#define  TFTP_CMD_MID       (0x1900)
#define  TFTP_SEND_HK_MID   (0x1901)   

/* Telemetry Message IDs */ 

#define  TFTP_HK_TLM_MID       (0x0900)
#define  TFTP_TRANSFER_REQ_MID (0x0901)

#endif /*_tftp_msgids_*/
