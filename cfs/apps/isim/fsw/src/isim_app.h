/* 
** Purpose: Define the Isim application.
**
** Notes:
**   None
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _isim_app_
#define _isim_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "isim.h"
#include "isimtbl.h"

/*
** Macro Definitions
*/

#define ISIM_APP_INIT_INFO_EID            (ISIM_APP_BASE_EID + 0)
#define ISIM_APP_EXIT_ERR_EID             (ISIM_APP_BASE_EID + 1)
#define ISIM_APP_CMD_NOOP_INFO_EID        (ISIM_APP_BASE_EID + 2)
#define ISIM_APP_CMD_INVALID_MID_ERR_EID  (ISIM_APP_BASE_EID + 3)

/*
** Type Definitions
*/

typedef struct
{

   CMDMGR_Class   CmdMgr;
   TBLMGR_Class   TblMgr;
   ISIM_Class     Isim;
   ISIMTBL_Class  IsimTbl;
   
   CFE_SB_PipeId_t CmdPipe;

} ISIM_APP_Class;

typedef struct
{

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** Table Data 
   ** - Loaded with status from the last table action 
   */

   uint8    LastAction;
   uint8    LastActionStatus;

   
   /*
   ** ISIM Data
   */

   uint8    IsimInstrState;
   uint8    IsimSciState;
   boolean  Fault;
   uint8    Spare;
   
   uint16   InitCycleCnt;
   uint16   SciFileCycleCnt;
   
   char Filename[OS_MAX_PATH_LEN];
   
} OS_PACK ISIM_HkPkt;

#define ISIM_TLM_HK_LEN sizeof (ISIM_HkPkt)

/*
** Exported Data
*/

extern ISIM_APP_Class  IsimApp;

/*
** Exported Functions
*/

/******************************************************************************
** Function: ISIM_AppMain
**
*/
void ISIM_AppMain(void);

#endif /* _isim_app_ */
