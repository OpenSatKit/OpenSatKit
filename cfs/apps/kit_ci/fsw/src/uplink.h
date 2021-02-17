/*
** Purpose: Manage the uplink socket
**
** Notes:
**   1. The "message tunnel" is a simple feature that allows a user to intercept a message and replace the
**      message ID. This is handy for testing.  The new message can be routed to a test application that
**      can manipulate the packet and if needed put back the original message IF so the modified packet gets
**      routed to the original destination. An example would be testing a file transport protocol and the user
**      needs to corrupt or drop specific packets.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _uplink_
#define _uplink_

/*
** Includes
*/

#include "app_cfg.h"
#include "common_types.h"
#include "network_includes.h"
#include "cfe.h"

/*
** Macros
*/



/*
** Event Message IDs
*/

#define UPLINK_SOCKET_CREATE_ERR_EID            (UPLINK_BASE_EID +  0)
#define UPLINK_SOCKET_BIND_ERR_EID              (UPLINK_BASE_EID +  1)
#define UPLINK_RECV_LEN_ERR_EID                 (UPLINK_BASE_EID +  2)
#define UPLINK_CONFIG_MSG_TUNNEL_ENA_INFO_EID   (UPLINK_BASE_EID +  3)
#define UPLINK_CONFIG_MSG_TUNNEL_DIS_INFO_EID   (UPLINK_BASE_EID +  4)
#define UPLINK_CONFIG_MSG_TUNNEL_INDEX_ERR_EID  (UPLINK_BASE_EID +  5)
#define UPLINK_DESTRUCTOR_INFO_EID              (UPLINK_BASE_EID +  6) 
#define UPLINK_DEBUG_EID                        (UPLINK_BASE_EID +  7)

#define UPLINK_TOTAL_EID  8

/*
** Type Definitions
*/

/******************************************************************************
** Message Tunnel
*/

typedef struct {

   boolean  Enabled;
   uint16   OrgMsgId;
   uint16   NewMsgId;

} UPLINK_MsgTunnelMap;

typedef struct {

   uint16   Index;
   uint16   OrgMsgId;
   uint16   NewMsgId;

} UPLINK_LastMapping;

typedef struct {

   boolean Enabled;
   UPLINK_MsgTunnelMap Map[UPLINK_MSG_TUNNEL_CNT];

   uint16  MappingsPerformed;
   UPLINK_LastMapping  LastMapping;

} UPLINK_MsgTunnel;

/******************************************************************************
** Uplink Class
*/

typedef struct {

   boolean  Connected;
   int      SocketId;
   struct sockaddr_in SocketAddress;

   uint32   RecvMsgCnt;
   uint32   RecvMsgErrCnt;
   uint8    RecvBuff[UPLINK_RECV_BUFF_LEN];
   CFE_SB_MsgPtr_t RecvMsgPtr;

   UPLINK_MsgTunnel MsgTunnel;

} UPLINK_Class;


/******************************************************************************
** Command Packets
*/

typedef struct
{

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   boolean  Enabled;
   uint8    Index;
   uint16   OrgMsgId;
   uint16   NewMsgId;

} UPLINK_ConfigMsgTunnelCmdMsg;
#define UPLINK_CONFIG_MSG_TUNNEL_CMD_DATA_LEN  (sizeof(UPLINK_ConfigMsgTunnelCmdMsg) - CFE_SB_CMD_HDR_SIZE)


/*
** Exported Functions
*/

/******************************************************************************
** Function: UPLINK_ConfigMsgTunnelCmd
**
** Enable/Disable a message tunnel. Minimal parameter checking is done because
** this is a test function. The following sequence of functions are performed
** so depending on what you're determines whether a field is used.
**    1. Verify index. oxFF enables/disables all entries.
**    2. Set enable/disable flag. If disabling then function exited after
**       entry is disabled.
**    3. Blindly (no range checks) load the message IDs
**
**
*/
boolean UPLINK_ConfigMsgTunnelCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: UPLINK_Constructor
**
** Construct an UPLINK object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void UPLINK_Constructor(UPLINK_Class*  UplinkPtr, uint16 Port);


/******************************************************************************
** Function: UPLINK_Read
**
** Read up to MaxMsgRead messages and return the number of messages read.
**
*/
int UPLINK_Read(uint16 MaxMsgRead);


/******************************************************************************
** Function: UPLINK_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void UPLINK_ResetStatus(void);


#endif /* _uplink_ */
