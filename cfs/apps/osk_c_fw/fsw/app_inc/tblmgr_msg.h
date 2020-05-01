/*
** Purpose: Define Table Manager's message definitions
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

#ifndef _tblmgr_msg_
#define _tblmgr_msg_

/*
** Includes
*/

#include "cfe.h"

/*
** Type Definitions
*/

/*
** Load/Dump Table Commands
**
** Identifer is assigned at the time of table registration
*/

typedef struct
{
   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /* cFE Software Bus Command Message Header     */
   uint8   Id;                             /* Table identifier                            */
   uint8   LoadType;                       /* Replace or update table records             */
   char    Filename[OS_MAX_PATH_LEN];      /* ASCII text string of full path and filename */

} TBLMGR_LoadTblCmdMsg;
#define TBLMGR_LOAD_TBL_CMD_DATA_LEN  (sizeof(TBLMGR_LoadTblCmdMsg) - CFE_SB_CMD_HDR_SIZE)

typedef struct
{
   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /* cFE Software Bus Command Message Header     */
   uint8   Id;                             /* Table identifier                            */
   uint8   DumpType;                       /* Placeholder for user defined qualifier      */
   char    Filename[OS_MAX_PATH_LEN];      /* ASCII text string of full path and filename */

} TBLMGR_DumpTblCmdMsg;
#define TBLMGR_DUMP_TBL_CMD_DATA_LEN  (sizeof(TBLMGR_DumpTblCmdMsg) - CFE_SB_CMD_HDR_SIZE)


#endif /* _tblmgr_msg_ */
