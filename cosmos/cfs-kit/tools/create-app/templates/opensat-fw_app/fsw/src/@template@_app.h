/* 
** Purpose: Define a @Template@ application.
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
#ifndef _@template@_
#define _@template@_

/*
** Includes
*/

#include "app_cfg.h"
#include "app_fw.h"
#include "exobj.h"

/*
** Macro Definitions
*/

#define @TEMPLATE@_INIT_INFO_EID            (@TEMPLATE@_BASE_EID + 0)
#define @TEMPLATE@_EXIT_ERR_EID             (@TEMPLATE@_BASE_EID + 1)
#define @TEMPLATE@_CMD_NOOP_INFO_EID        (@TEMPLATE@_BASE_EID + 2)
#define @TEMPLATE@_CMD_INVALID_MID_ERR_EID  (@TEMPLATE@_BASE_EID + 3)

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

} @TEMPLATE@_Class;

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

   uint8    ExTblLastLoadStatus;
   uint8    ExTblAttrErrCnt;       /* Attribute errors during the last load */

   /*
   ** EXOBJ Data
   */

   uint16   ExObjExecCnt;

} OS_PACK @TEMPLATE@_HkPkt;

#define @TEMPLATE@_TLM_HK_LEN sizeof (@TEMPLATE@_HkPkt)

/*
** Exported Data
*/

extern @TEMPLATE@_Class  @Template@;

/*
** Exported Functions
*/

/******************************************************************************
** Function: @TEMPLATE@_Main
**
*/
void @TEMPLATE@_Main(void);

#endif /* _@template@_ */
