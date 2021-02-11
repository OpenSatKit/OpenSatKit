/*
** Purpose: 42 Interface  
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application.
**   2. This object hides all of the 42 interface details and I42 should only
**      interface with it through functions. See osk_42_lib.h for more
**      42 AcApp porting information.
**   3. In a more realistic design there would be different interfaces for 
**      sensors and actuators so I42's functionality may be distributed. Even
**      with multiple interface apps, having a single app to aggregate and
**      synchronize sensor data is often helpful. 
**
** References:
**   1. OpenSat Object-based Application Developer's Guide
**   2. cFS Application Developer's Guide
**   3. 42/Docs/Standalone AcApp text file
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General
**   Public License (GPL). 
*/

#ifndef _if42_
#define _if42_

/*
** Includes
*/

#include "app_cfg.h"
#include "osk_42_lib.h"


/***********************/
/** Macro Definitions **/
/***********************/

#define IF42_IP_ADDR_STR_LEN  16
#define IF42_SEM_INVALID      0xFFFFFFFF

/*
** Event Message IDs
*/

#define IF42_CREATE_SEM_ERR_EID      (IF42_BASE_EID +  0)
#define IF42_CONNECT_TO_42_EID       (IF42_BASE_EID +  1)
#define IF42_CONNECT_ERR_EID         (IF42_BASE_EID +  2)
#define IF42_CREATE_CHILD_ERR_EID    (IF42_BASE_EID +  3)
#define IF42_CHILD_TASK_INIT_EID     (IF42_BASE_EID +  4)
#define IF42_CHILD_TASK_ERR_EID      (IF42_BASE_EID +  5)
#define IF42_SOCKET_OPEN_ERR_EID     (IF42_BASE_EID +  6)
#define IF42_HOST_ERR_EID            (IF42_BASE_EID +  7)
#define IF42_SOCKET_CLOSE_EID        (IF42_BASE_EID +  8)
#define IF42_NO_ACTUATOR_CMD_EID     (IF42_BASE_EID +  9)
#define IF42_SKIP_INIT_CYCLE_EID     (IF42_BASE_EID + 10)
#define IF42_DEBUG_EID               (IF42_BASE_EID + 11)

/**********************/
/** Type Definitions **/
/**********************/

/******************************************************************************
** Commands
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    IpAddrStr[IF42_IP_ADDR_STR_LEN];
   uint16  Port;
   
}  OS_PACK IF42_ConnectCmdMsg;
#define IF42_CONNECT_CMD_DATA_LEN  (sizeof(IF42_ConnectCmdMsg) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   
}  OS_PACK IF42_DisconnectCmdMsg;
#define IF42_DISCONNECT_CMD_DATA_LEN  (sizeof(IF42_DisconnectCmdMsg) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry
*/


/*
** Sensor Data
**
** This sensor data is really engineering units that would normally be derived
** from sensor inputs.
*/

typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   double  Time;     /* GPS */
   double  PosN[3];
   double  VelN[3];
   
   double  qbn[4];   /* ST */

   double  wbn[3];   /* Gyro */
   
   double  svb[3];   /* CSS/FSS */

   double  bvb[3];   /* TAM */

   double  WhlH[AC42_NWHL];  /* Wheels */

   boolean GpsValid;
   boolean StValid;  
   boolean SunValid; /* CSS/FSS */
   boolean InitCycle;

} OS_PACK IF42_SensorDataPkt;
#define IF42_SENSOR_DATA_PKT_LEN sizeof (IF42_SensorDataPkt)


/******************************************************************************
** Actuator Commands
*/

typedef struct {

   uint8   Header[CFE_SB_TLM_HDR_SIZE];

   double  Tcmd[3];    /* Wheel */
   double  Mcmd[3];    /* MTB   */
   double  SaGcmd;     /* SA    */

} OS_PACK IF42_ActuatorCmdDataPkt;
#define IF42_ACTUATOR_CMD_DATA_PKT_LEN sizeof (IF42_ActuatorCmdDataPkt)


/******************************************************************************
** IF42 Class
*/
typedef struct {


   /*
   ** Object Data
   */

   uint32  ChildTaskId;
   uint32  WakeUpSemaphore;
   
   boolean Connected;
   boolean InitCycle;
   boolean ActuatorPktSent;     /* Used for each control cycle */
   uint32  SensorPktCnt;
   uint32  ActuatorPktCnt;
   uint32  ExecuteCycleCnt;
   uint16  UnclosedCycleCnt;    /* 'Unclosed' is when ManageExecution() called but sesnor-ctrl-actuator cycle didn't finish */ 
   
   SOCKET  SocketFd;
   char    IpAddrStr[IF42_IP_ADDR_STR_LEN];
   uint16  Port;
   
   /*
   ** Contained Objects
   */
   
   AC42_Class *Ac42;
   
   /*
   ** Telemetry
   */
   
   IF42_SensorDataPkt SensorPkt;

   
} IF42_Class;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: IF42_Constructor
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void IF42_Constructor(IF42_Class* If42Obj, const char* IpAddrStr, uint16 Port);


/******************************************************************************
** Function: IF42_ResetStatus
**
*/
void IF42_ResetStatus(void);


/******************************************************************************
** Function: IF42_Close
**
*/
void IF42_Close(void);


/******************************************************************************
** Function:  IF42_ManageExecution
**
*/
void IF42_ManageExecution(void);


/******************************************************************************
** Function: IF42_ConnectCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr function signature
*/
boolean IF42_ConnectCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: IF42_DisconnectCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr function signature
*/
boolean IF42_DisconnectCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: IF42_RecvSensorData
**
** Read sensor data from 42 socket and load process sensor data packet.
*/
boolean IF42_RecvSensorData(IF42_SensorDataPkt* SensorDataPkt);


/******************************************************************************
** Function: IF42_SendActuatorCmds
**
** Send actuator commandd data to 42.
*/
boolean IF42_SendActuatorCmds(const IF42_ActuatorCmdDataPkt* ActuatorCmdDataPkt); 


#endif /* _if42_ */
