/*
** Purpose: Define the 42 Attitude Control (AC) application
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application 
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**   3. 42 open source repository at 
**      https://sourceforge.net/projects/fortytwospacecraftsimulation/
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/
#ifndef _f42_app_
#define _f42_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "f42_adp.h"

/*
** Macro Definitions
*/

#define F42_INIT_APP_INFO_EID   (F42_BASE_EID + 0)
#define F42_NOOP_INFO_EID       (F42_BASE_EID + 1)
#define F42_EXIT_ERR_EID        (F42_BASE_EID + 2)
#define F42_INVALID_MID_ERR_EID (F42_BASE_EID + 3)
#define F42_DEBUG_CMD_ERR_EID   (F42_BASE_EID + 4)
#define F42_DEBUG_CMD_INFO_EID  (F42_BASE_EID + 5)
#define F42_DEBUG_EID           (F42_BASE_EID + 6)

#define F42_TOTAL_EID  7


/*
** Type Definitions
*/

typedef struct
{

   CFE_SB_PipeId_t  CmdPipe;
   CFE_SB_PipeId_t  SensorPipe;

   CMDMGR_Class     CmdMgr;
   TBLMGR_Class     TblMgr;
   
   F42_ADP_Class    F42Adp;

   uint32           CtrlExeCnt;
   
   int32            DbgFileHandle;
   boolean          DbgEnabled;

} F42_Class;

typedef struct
{

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** TBLMGR Data 
   ** - Loaded with status from the last table action 
   */

   uint8    LastAction;
   uint8    LastActionStatus;
   
   /*
   ** Application Data
   */
   
   uint32   CtrlExeCnt;

   /*
   ** 42 FSW Adapter Data
   */
   
   uint16   CtrlMode;      /* 16-bit to keep aligned */
   uint16   OvrSunValid;   /* 16-bit to keep aligned */

   float    wc[3];
   float    zc[3];

   float    Kr[3];
   float    Kp[3];

   float    Hwcmd[3];
   
} OS_PACK F42_HkPkt;
#define F42_TLM_HK_LEN sizeof (F42_HkPkt)

typedef struct
{

   uint8  Header[CFE_SB_TLM_HDR_SIZE];
   
   float  wbn[3];
   float  qbr[4];
   float  AttErr[3];
   float  Herr[3];
   float  svb[3];
   float  WhlCmd[3];
   float  MtbCmd[3];
   float  GimCmd;
   
   uint16 SunValid;

} OS_PACK F42_CtrlPkt;
#define F42_TLM_CTRL_PKT_LEN sizeof (F42_CtrlPkt)

/******************************************************************************
** Command Functions
*/

typedef struct
{

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16   NewState;                        /* 0=Disable, 1=Enable */

}  OS_PACK F42_ConfigDbgCmdPkt;
#define F42_CONFIG_DBG_CMD_DATA_LEN  (sizeof(F42_ConfigDbgCmdPkt) - CFE_SB_CMD_HDR_SIZE)


/*
** Exported Data
*/

extern F42_Class  F42;


/*
** Exported Functions
*/

/******************************************************************************
** Function: F42_AppMain
**
*/
void F42_AppMain(void);


/******************************************************************************
** Function: F42_NoOpCmd
**
*/
boolean F42_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: F42_ResetAppCmd
**
*/
boolean F42_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: F42_SendHousekeepingPkt
**
*/
void F42_SendHousekeepingPkt(void);

#endif /* _f42_app_ */
