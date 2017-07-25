/* 
** Purpose: Define a demo application. This app exercises the OpenSat 
**          application framework and serves as a good starting point for
**          trying new ideas.
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _fw_demo_
#define _fw_demo_

/*
** Includes
*/

#include "app_cfg.h"
#include "app_fw.h"
#include "exobj.h"

/*
** Macro Definitions
*/

#define FW_DEMO_INIT_INFO_EID            (FW_DEMO_BASE_EID + 0)
#define FW_DEMO_EXIT_ERR_EID             (FW_DEMO_BASE_EID + 1)
#define FW_DEMO_CMD_NOOP_INFO_EID        (FW_DEMO_BASE_EID + 2)
#define FW_DEMO_CMD_INVALID_MID_ERR_EID  (FW_DEMO_BASE_EID + 3)

/*
** Type Definitions
*/

typedef struct
{

   CMDMGR_Class   CmdMgr;
   TBLMGR_Class   TblMgr;
   EXOBJ_Class    ExObj;
   XMLTBL_Class   XmlTbl;
   SCANFTBL_Class ScanfTbl;
   
   CFE_SB_PipeId_t CmdPipe;

} FW_DEMO_Class;

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

   boolean    EnableDataLoad;
   uint8      TblIndex;
   uint16     Data1;
   uint16     Data2;
   uint16     Data3;

} OS_PACK FW_DEMO_HkPkt;

#define FW_DEMO_TLM_HK_LEN sizeof (FW_DEMO_HkPkt)

/*
** Exported Data
*/

extern FW_DEMO_Class  FwDemo;

/*
** Exported Functions
*/

/******************************************************************************
** Function: FW_DEMO_Main
**
*/
void FW_DEMO_Main(void);

#endif /* _fw_demo_ */
