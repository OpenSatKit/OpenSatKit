/* 
** Purpose: Define the Payload Manager App
**
** Notes:
**   1. Provide an example app that controls a payload through
**      library calls. pl_sim_lib provides a library with functions
**      that emulate a hardware library interfacing to a payload.
**      pl_sim_app provides a ground interface for configring the
**      simulation during runtime.
**   2. An alternative simulation design is to create a payload
**      interface app that send SB messages between the payload
**      and the payload manager app. 
**   3. The 'business logic' regarding when payload data can be collected
**      and when science files can be created is managed at the app level
**      so the SCI_FILE object's scope is limited to science data formats
**      and science file management. 
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
#ifndef _pl_mgr_
#define _pl_mgr_

/*
** Includes
*/

#include "app_cfg.h"
#include "payload.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define PL_MGR_INIT_EID              (PL_MGR_BASE_EID + 0)
#define PL_MGR_EXIT_EID              (PL_MGR_BASE_EID + 1)
#define PL_MGR_CMD_NOOP_EID          (PL_MGR_BASE_EID + 2)
#define PL_MGR_CMD_INVALID_MID_EID   (PL_MGR_BASE_EID + 3)

/**********************/
/** Type Definitions **/
/**********************/

/******************************************************************************
** Command Packets
**
*/

/******************************************************************************
** Telemetry Packets
*/

typedef struct
{

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;
   
   /*
   ** Payload
   */
   
   uint8    PayloadPowerState;
   boolean  PayloadDetectorFault;
   uint16   PayloadDetectorReadoutRow;
   uint16   PayloadDetectorImageCnt;
   
   /*
   ** Science 
   */
   
   boolean  SciFileOpen;
   uint8    SciFileImageCnt;   
   char SciFilename[OS_MAX_PATH_LEN];
   
} OS_PACK PL_MGR_StatusTlm_t;
#define PL_MGR_APP_STATUS_TLM_LEN sizeof (PL_MGR_StatusTlm_t)


typedef struct
{

   /* 
   ** App Framework
   */ 
   
   INITBL_Class    IniTbl;
   CFE_SB_PipeId_t CmdPipe;
   CMDMGR_Class    CmdMgr;
   
   /*
   ** Telemetry Packets
   */

   PL_MGR_StatusTlm_t StatusTlm;

   /*
   ** PL_MGR state and Child Objects
   */   
   
   uint32           PerfId;
   CFE_SB_MsgId_t   CmdMid;
   CFE_SB_MsgId_t   ExecuteMid;
   uint32           TlmSlowRate;
   uint32           TlmSlowRateCnt;
   
   PAYLOAD_Class_t  Payload;
   
}  PL_MGR_Class_t;



/*******************/
/** Exported Data **/
/*******************/

extern  PL_MGR_Class_t  PlMgr;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function:  PL_MGR_AppMain
**
*/
void  PL_MGR_AppMain(void);


#endif /* _pl_mgr_app_ */
