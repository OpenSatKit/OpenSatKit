/*
** Purpose: Network interface  
**
** Notes:
**   1. Named NET42IF to avoid dynamic link with TF's NETIF. Ticket entered
**      to create one version and make it part of ask_app_fw.
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

#define NETIF_IP_ADDR_STR_LEN  16

/*
** Event Message IDs
*/

#define NETIF_CONNECT_TO_42_EID    (NETIF_BASE_EID +  0)
#define NETIF_CONNECT_EID          (NETIF_BASE_EID +  1)
#define NETIF_BIND_SOCKET_ERR_EID  (NETIF_BASE_EID +  2)
#define NETIF_SOCKET_OPEN_ERR_EID  (NETIF_BASE_EID +  3)
#define NETIF_HOST_ERR_EID         (NETIF_BASE_EID +  4)
#define NETIF_CONNECT_ERR_EID      (NETIF_BASE_EID +  5)
#define NETIF_STREAM_OPEN_ERR_EID  (NETIF_BASE_EID +  6)
#define NETIF_RECV_ERR_EID         (NETIF_BASE_EID +  7)
#define NETIF_SEND_ERR_EID         (NETIF_BASE_EID +  8)
#define NETIF_SOCKET_CLOSE_EID     (NETIF_BASE_EID +  9)
#define NETIF_DEBUG_EID            (NETIF_BASE_EID + 10)

/*
** Type Definitions
*/

/******************************************************************************
** Command Functions
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    IpAddrStr[NETIF_IP_ADDR_STR_LEN];
   uint16  Port;
   
}  OS_PACK NETIF_Connect42CmdParam;
#define NETIF_CONNECT_42_CMD_DATA_LEN  (sizeof(NETIF_Connect42CmdParam) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   
}  OS_PACK NETIF_Disconnect42CmdParam;
#define NETIF_DISCONNECT_42_CMD_DATA_LEN  (sizeof(NETIF_Disconnect42CmdParam) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** NETIF Class
*/
typedef struct {

   int     SocketFd;
   FILE*   StreamId;
   char    IpAddrStr[NETIF_IP_ADDR_STR_LEN];
   uint16  Port;

   boolean Connected;
    
} NETIF_Class;


/*
** Exported Functions
*/

/******************************************************************************
** Function: NETIF42_Constructor
**
** Construct a local NETIF object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void NETIF42_Constructor(NETIF_Class*  NetIfPtr, const char* IpAddrStr, uint16 Port);


/******************************************************************************
** Function: NETIF42_Close
**
*/
void NETIF42_Close(void);


/******************************************************************************
** Function: NETIF42_Connect42Cmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr function signature
*/
boolean NETIF42_Connect42Cmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: NETIF42_Disconnect42Cmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr function signature
*/
boolean NETIF42_Disconnect42Cmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: NETIF42_Recv
**
*/
int32 NETIF42_Recv(char* BufPtr, const uint16 BufSize);


/******************************************************************************
** Function:  NETIF42_ResetStatus
**
*/
void NETIF42_ResetStatus(void);


/******************************************************************************
** Function: NETIF42_Send
**
*/
int32 NETIF42_Send (const char *BufPtr, uint16 len); 


#endif /* _netif_ */
