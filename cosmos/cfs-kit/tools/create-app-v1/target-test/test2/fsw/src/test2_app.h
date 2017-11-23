/* 
** Purpose: Define a Test2 application.
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
#ifndef _test2_
#define _test2_

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

#define TEST2_INIT_INFO_EID            (TEST2_BASE_EID + 0)
#define TEST2_EXIT_ERR_EID             (TEST2_BASE_EID + 1)
#define TEST2_CMD_NOOP_INFO_EID        (TEST2_BASE_EID + 2)
#define TEST2_CMD_INVALID_MID_ERR_EID  (TEST2_BASE_EID + 3)

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

} TEST2_Class;

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

} OS_PACK TEST2_HkPkt;

#define TEST2_TLM_HK_LEN sizeof (TEST2_HkPkt)

/*
** Exported Data
*/

extern TEST2_Class  Test2;

/*
** Exported Functions
*/

/******************************************************************************
** Function: TEST2_Main
**
*/
void TEST2_Main(void);

#endif /* _test2_ */
