/* 
** Purpose: Define a Proto42 application.
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
#ifndef _proto42_
#define _proto42_

/*
** Includes
*/

#include "app_cfg.h"
#include "app_fw.h"
#include "exobj.h"
#include "extbl.h"

/*
** Macro Definitions
*/

#define PROTO42_INIT_INFO_EID            (PROTO42_BASE_EID + 0)
#define PROTO42_EXIT_ERR_EID             (PROTO42_BASE_EID + 1)
#define PROTO42_CMD_NOOP_INFO_EID        (PROTO42_BASE_EID + 2)
#define PROTO42_CMD_INVALID_MID_ERR_EID  (PROTO42_BASE_EID + 3)

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

} PROTO42_Class;

typedef struct
{

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** EXTBL Data
   */

   boolean  ExTblLastLoadValid;
   uint8    ExTblErrCnt;

   /*
   ** EXOBJ Data
   */

   uint16   ExObjExecCnt;

} OS_PACK PROTO42_HkPkt;

#define PROTO42_TLM_HK_LEN sizeof (PROTO42_HkPkt)

/*
** Exported Data
*/

extern PROTO42_Class  Proto42;

/*
** Exported Functions
*/

/******************************************************************************
** Function: PROTO42_Main
**
*/
void PROTO42_Main(void);

#endif /* _proto42_ */
