/* 
** Purpose: Define a demo application. This app exercises the OpenSatKit 
**          application framework and serves as a good starting point for
**          trying new ideas.
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _osk_demo_
#define _osk_demo_

/*
** Includes
*/

#include "app_cfg.h"
#include "demobj.h"

/*
** Macro Definitions
*/

#define OSK_DEMO_INIT_INFO_EID            (OSK_DEMO_BASE_EID + 0)
#define OSK_DEMO_EXIT_ERR_EID             (OSK_DEMO_BASE_EID + 1)
#define OSK_DEMO_CMD_NOOP_INFO_EID        (OSK_DEMO_BASE_EID + 2)
#define OSK_DEMO_CMD_INVALID_MID_ERR_EID  (OSK_DEMO_BASE_EID + 3)

/*
** Type Definitions
*/

typedef struct
{

   CMDMGR_Class   CmdMgr;
   TBLMGR_Class   TblMgr;
   DEMOBJ_Class   DemObj;
   XMLTBL_Class   XmlTbl;
   SCANFTBL_Class ScanfTbl;
   JSONTBL_Class  JsonTbl;
   
   CFE_SB_PipeId_t CmdPipe;

} OSK_DEMO_Class;

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
   ** DEMOBJ Data
   */

   boolean    EnableDataLoad;
   uint8      TblIndex;
   uint16     Data1;
   uint16     Data2;
   uint16     Data3;

} OS_PACK OSK_DEMO_HkPkt;

#define OSK_DEMO_TLM_HK_LEN sizeof (OSK_DEMO_HkPkt)

/*
** Exported Data
*/

extern OSK_DEMO_Class  FwDemo;

/*
** Exported Functions
*/

/******************************************************************************
** Function: OSK_DEMO_Main
**
*/
void OSK_DEMO_Main(void);

#endif /* _osk_demo_ */
