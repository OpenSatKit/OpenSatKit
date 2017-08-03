/*
** Purpose: Define the 42 interface application
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application 
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/
#ifndef _i42_app_
#define _i42_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "netif.h"

/*
** Macro Definitions
*/

#define I42_APP_INIT_APP_INFO_EID   (I42_APP_BASE_EID + 0)
#define I42_APP_NOOP_INFO_EID       (I42_APP_BASE_EID + 1)
#define I42_APP_EXIT_ERR_EID        (I42_APP_BASE_EID + 2)
#define I42_APP_INVALID_MID_ERR_EID (I42_APP_BASE_EID + 3)

#define I42_APP_TOTAL_EID  4


/*
** Type Definitions
*/

typedef struct
{


   CMDMGR_Class     CmdMgr;
   NETIF_Class      NetIf;

   CFE_SB_PipeId_t  CmdPipe;
   CFE_SB_PipeId_t  ActuatorPipe;

} I42_APP_Class;

typedef struct
{

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** NETIF Data
   */
   
   uint32   ActuatorPktCnt;
   uint32   SensorPktCnt;
   
   uint16   ConnectCycles;   /* Connection time in sensor read cycles */

   boolean  Connected;
   
} OS_PACK I42_APP_HkPkt;
#define I42_APP_TLM_HK_LEN sizeof (I42_APP_HkPkt)

/*
** Exported Data
*/

extern I42_APP_Class  I42App;


/*
** Exported Functions
*/

/******************************************************************************
** Function: I42_AppMain
**
*/
void I42_AppMain(void);


/******************************************************************************
** Function: I42_APP_NoOpCmd
**
*/
boolean I42_APP_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: I42_APP_ResetAppCmd
**
*/
boolean I42_APP_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: I42_APP_SendHousekeepingPkt
**
*/
void I42_APP_SendHousekeepingPkt(void);

#endif /* _i42_app_ */
