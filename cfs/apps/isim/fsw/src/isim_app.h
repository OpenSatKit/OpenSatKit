/* 
** Purpose: Define the Instrument Simulator App
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

/***********************/
/** Macro Definitions **/
/***********************/

#define ISIM_APP_INIT_EID            (ISIM_APP_BASE_EID + 0)
#define ISIM_APP_EXIT_EID            (ISIM_APP_BASE_EID + 1)
#define ISIM_APP_CMD_NOOP_EID        (ISIM_APP_BASE_EID + 2)
#define ISIM_APP_CMD_INVALID_MID_EID (ISIM_APP_BASE_EID + 3)

/**********************/
/** Type Definitions **/
/**********************/

/******************************************************************************
** Command Packets
*/


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

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

   uint8    IsimPwrState;
   uint8    IsimPwrInitCycleCnt;
   uint8    IsimPwrResetCycleCnt;
   uint8    IsimSciState;
   uint16   IsimDetectorImageCnt;
   uint8    IsimDetectorReadoutRow;
   boolean  IsimDetectorFault;
   boolean  IsimSciFileOpen;
   uint8    CurrFileImageCnt;
   
   char SciFilename[OS_MAX_PATH_LEN];
   
} OS_PACK ISIM_APP_HkPkt;
#define ISIM_APP_HK_PKT_LEN sizeof (ISIM_APP_HkPkt)


/******************************************************************************
** ISIM Class
*/
typedef struct {


   /* 
   ** App Framework
   */ 
   
   INITBL_Class    IniTbl;
   CFE_SB_PipeId_t CmdPipe;
   CMDMGR_Class    CmdMgr;
   TBLMGR_Class    TblMgr;
   
   /*
   ** Telemetry Packets
   */
   ISIM_APP_HkPkt HkPkt;

   /*
   ** App Objects
   */   
   ISIM_Class     Isim;
   ISIMTBL_Class  IsimTbl;
   

} ISIM_APP_Class;



/*******************/
/** Exported Data **/
/*******************/

extern ISIM_APP_Class  IsimApp;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: ISIM_AppMain
**
*/
void ISIM_AppMain(void);


#endif /* _isim_app_ */
