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

#ifndef _filemgr_msgids_
#define _filemgr_msgids_

/*
** Command Message IDs
*/

#define FILEMGR_CMD_MID              0x188C
#define FILEMGR_SEND_HK_MID          0x188D

/*
** Telemetry Message IDs
*/

#define FILEMGR_HK_TLM_MID           0x088A 
#define FILEMGR_FILE_INFO_TLM_MID    0x088B
#define FILEMGR_DIR_LIST_TLM_MID     0x088C
#define FILEMGR_OPEN_FILES_TLM_MID   0x088D
#define FILEMGR_FILESYS_TLM_MID      0x088E

#endif /* _filemgr_msgids_ */
