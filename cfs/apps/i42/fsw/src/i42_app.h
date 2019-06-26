/*
** Purpose: Define the 42 FSW interface application
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
#ifndef _i42_app_
#define _i42_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "netif.h"
#include "f42_adp.h"

/*
** Macro Definitions
*/

#define I42_INIT_APP_INFO_EID            (I42_BASE_EID + 0)
#define I42_NOOP_INFO_EID                (I42_BASE_EID + 1)
#define I42_EXIT_ERR_EID                 (I42_BASE_EID + 2)
#define I42_INVALID_MID_ERR_EID          (I42_BASE_EID + 3)
#define I42_IDLE_SOCKET_CLOSE_INFO_EID   (I42_BASE_EID + 4)
#define I42_RESEND_ACTUATOR_PKT_INFO_EID (I42_BASE_EID + 5)
#define I42_DEBUG_EID                    (I42_BASE_EID + 6)

#define I42_TOTAL_EID  7


/*
** Type Definitions
*/

typedef struct
{

   CMDMGR_Class     CmdMgr;
   NETIF_Class      NetIf;

   CFE_SB_PipeId_t  CmdPipe;
   CFE_SB_PipeId_t  ActuatorPipe;

   uint32  ConnectCycleCnt;
   uint16  NoSensorDisconnectCnt;
   uint16  NoSensorDisconnectLim;
   uint16  NoSensorResendActuatorLim;
   
   boolean SensorPktSent;
   uint32  SensorPktCnt;

   boolean ActuatorPktSent;
   boolean ActuatorResend;
   uint32  ActuatorPktCnt;

   F42_ADP_SensorPkt    SensorPkt;
   F42_ADP_ActuatorPkt  ActuatorPkt;

   char  InBuf[I42_SOCKET_BUF_LEN];
   char  OutBuf[I42_SOCKET_BUF_LEN];

} I42_Class;

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
   
   uint16   ConnectCycleCnt;   /* Connection count in sensor read cycles */

   boolean  Connected;
   
} OS_PACK I42_HkPkt;
#define I42_TLM_HK_LEN sizeof (I42_HkPkt)

/*
** Exported Data
*/

extern I42_Class  I42;


/*
** Exported Functions
*/

/******************************************************************************
** Function: I42_AppMain
**
*/
void I42_AppMain(void);


/******************************************************************************
** Function: I42_NoOpCmd
**
*/
boolean I42_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: I42_ResetAppCmd
**
*/
boolean I42_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: I42_SendHousekeepingPkt
**
*/
void I42_SendHousekeepingPkt(void);

#endif /* _i42_app_ */
