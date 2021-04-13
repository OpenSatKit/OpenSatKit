/* 
** Purpose: Define the Spacecraft Simulator App
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
#ifndef _scsim_app_
#define _scsim_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "scsim.h"
#include "scsimtbl.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define SCSIM_APP_INIT_EID            (SCSIM_APP_BASE_EID + 0)
#define SCSIM_APP_EXIT_EID            (SCSIM_APP_BASE_EID + 1)
#define SCSIM_APP_CMD_NOOP_EID        (SCSIM_APP_BASE_EID + 2)
#define SCSIM_APP_CMD_INVALID_MID_EID (SCSIM_APP_BASE_EID + 3)

/**********************/
/** Type Definitions **/
/**********************/

/******************************************************************************
** Command Packets
*/



/******************************************************************************
** Telemetry Packets
**
** - The simulation object telemetry must be separate from the HK packet that
**   provides simulation state information. The simulation status is sent and
**   displayed even when a simulated ground contact is not active. The 
**   subsystem sim model data is only sent when a ground contact is active.
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
   ** SCSIM Management Data
   */

   boolean  SimActive;
   uint8    SimPhase;
   uint32   SimTime;
      
} OS_PACK SCSIM_APP_HkPkt;
#define SCSIM_APP_HK_PKT_LEN sizeof (SCSIM_APP_HkPkt)


/******************************************************************************
** SCSIM Class
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
  
   SCSIM_APP_HkPkt HkPkt;


   /*
   ** App Objects
   */   

   SCSIM_Class     ScSim;
   SCSIMTBL_Class  ScSimTbl;
   

} SCSIM_APP_Class;



/*******************/
/** Exported Data **/
/*******************/

extern SCSIM_APP_Class  ScSimApp;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: SCSIM_AppMain
**
*/
void SCSIM_AppMain(void);


#endif /* _scsim_app_ */
