/*
** Purpose: Define a 42 simulator interface
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
**   Written by David McComas, licensed under the copyleft GNU General
**   Public License (GPL). 
*/

#ifndef _netif_
#define _netif_

/*
** Includes
*/

#include "cfe.h"
#include "app_cfg.h"
#include "f42_adp.h"

#define NETIF_IP_STR_LEN        16
#define NETIF_SOCKET_BUF_LEN   512
#define NETIF_LOCAL_HOST_STR   "localhost"
#define NETIF_DEF_IP_ADDR_STR  "127.000.000.001"
#define NETIF_DEF_PORT         42420

/*
** Event Message IDs
*/

#define NETIF_CONNECT_TO_42_INFO_EID     (NETIF_BASE_EID +  0)
#define NETIF_SOCKET_OPEN_ERR_EID        (NETIF_BASE_EID +  1)
#define NETIF_HOST_ERR_EID               (NETIF_BASE_EID +  2)
#define NETIF_CONNECT_ERR_EID            (NETIF_BASE_EID +  3)
#define NETIF_STREAM_OPEN_ERR_EID        (NETIF_BASE_EID +  4)
#define NETIF_IDLE_SOCKET_CLOSE_INFO_EID (NETIF_BASE_EID +  5)
#define NETIF_SOCKET_CLOSE_INFO_EID      (NETIF_BASE_EID +  6)

/*
** Type Definitions
*/

/******************************************************************************
** NETIF Class
*/

typedef struct {

   int     SocketFd;
   FILE*   StreamId;
   char    IpAddrStr[NETIF_IP_STR_LEN];
   uint16  Port;

   boolean Connected;
   uint16  ConnectCycles;        /* Connection cycles based on sensor read attempts */
   uint16  DisconnectCycles;

   char  InBuf[NETIF_SOCKET_BUF_LEN];
   char  OutBuf[NETIF_SOCKET_BUF_LEN];
   
   F42_ADP_SensorPkt  SensorPkt;
   
   uint32  SensorPktCnt;
   uint32  ActuatorPktCnt;
   
} NETIF_Class;


/******************************************************************************
** Command Functions
*/

typedef struct
{

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    IpAddrStr[NETIF_IP_STR_LEN];
   uint16  Port;

} NETIF_ConnectTo42CmdParam;
#define NETIF_CONNECT_TO_42_CMD_DATA_LEN  (sizeof(NETIF_ConnectTo42CmdParam) - CFE_SB_CMD_HDR_SIZE)


/*
** Exported Functions
*/

/******************************************************************************
** Function: NETIF_Constructor
**
** Initialize a network interface object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void NET42IF_Constructor(NETIF_Class*  NetIfPtr);


/******************************************************************************
** Function:  NETIF_ResetStatus
**
*/
void NETIF_ResetStatus(void);


/******************************************************************************
** Function: NETIF_ConnectTo42Cmd
**
*/
boolean NETIF_ConnectTo42Cmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function:  NETIF_ProcessSensorPkt
**
*/
boolean NETIF_ProcessSensorPkt(void);


/******************************************************************************
** Function: NETIF_SendActuatorPkt
**
*/
ssize_t NETIF_SendActuatorPkt(F42_ADP_ActuatorPkt*  ActuatorPkt);


/******************************************************************************
** Function: NETIF_Close
**
*/
void NETIF_Close(void);


#endif /* _netif_ */
