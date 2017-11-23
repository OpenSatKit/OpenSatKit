/* 
** Purpose: Define a Example application.
**
** Notes:
**   None
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _example_
#define _example_

/*
** Includes
*/

#include "app_cfg.h"
#include "exobj.h"
#include "extbl.h"

/*
** Macro Definitions
*/

#define EXAMPLE_INIT_INFO_EID            (EXAMPLE_BASE_EID + 0)
#define EXAMPLE_EXIT_ERR_EID             (EXAMPLE_BASE_EID + 1)
#define EXAMPLE_CMD_NOOP_INFO_EID        (EXAMPLE_BASE_EID + 2)
#define EXAMPLE_CMD_INVALID_MID_ERR_EID  (EXAMPLE_BASE_EID + 3)

/*
** Type Definitions
*/

typedef struct
{

   CMDMGR_Class  CmdMgr;
   TBLMGR_Class  TblMgr;
   EXOBJ_Class   ExObj;
   EXTBL_Class   ExTbl;
   
   CFE_SB_PipeId_t CmdPipe;

} EXAMPLE_Class;

typedef struct
{

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** Example Table Data 
   ** - Loaded with status from the last table action 
   */

   uint8    LastAction;
   uint8    LastActionStatus;

   
   /*
   ** EXOBJ Data
   */

   uint16   ExObjExecCnt;

} OS_PACK EXAMPLE_HkPkt;

#define EXAMPLE_TLM_HK_LEN sizeof (EXAMPLE_HkPkt)

/*
** Exported Data
*/

extern EXAMPLE_Class  Example;

/*
** Exported Functions
*/

/******************************************************************************
** Function: EXAMPLE_AppMain
**
*/
void EXAMPLE_AppMain(void);

#endif /* _example_ */
