/*
** Purpose: Implement the TFTP protocol.
**
** Notes:
**   1. This code is based on the following file: http://cnds.eecs.jacobs-university.de/courses/np-2012/src/tftp/tftp.c
**      In it's header it states "A TFTP (RFC 1350) over IPv4/IPv6 client. This code is intended to
**      demonstrate (i) how to write encoding/decoding functions, (ii) how to implement a simple state
**      machine, and (iii) how to use a select() main loop to implement timeouts and retransmissions.
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

#ifndef _tftp_
#define _tftp_

/*
** Includes
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "common_types.h"
#include "cfe.h"
#include "netif.h"
#include "app_cfg.h"


/***********************/
/** Macro Definitions **/
/***********************/

/*
** See RFC 1350 section 5 and the appendix.
*/

#define TFTP_OPCODE_RRQ		1
#define TFTP_OPCODE_WRQ		2
#define TFTP_OPCODE_DATA	3
#define TFTP_OPCODE_ACK		4
#define TFTP_OPCODE_ERROR	5

#define TFTP_STATE_TIMEOUT    40  /* Length pepends on TFTP_APP_RUNLOOP_DELAY in tftp_platform_cfg.h */
#define TFTP_DEF_TIMEOUT_SEC  0
#define TFTP_DEF_TIMEOUT_USEC 50000
#define TFTP_MAX_DATA_LEN     512
#define TFTP_MAX_MSGSIZE      (4 + TFTP_MAX_DATA_LEN)

#define TFTP_MODE_OCTET    "octet"
#define TFTP_MODE_NETASCII "netascii"
#define TFTP_MODE_MAIL     "mail"

#define TFTP_ERR_NOT_DEFINED   0
#define TFTP_ERR_NOT_FOUND	    1
#define TFTP_ERR_ACCESS_DENIED 2
#define TFTP_ERR_DISK_FULL     3
#define TFTP_ERR_UNKNOWN_TID   4
#define TFTP_ERR_ILLEGAL_OP    5
#define TFTP_ERR_FILE_EXISTS   6
#define TFTP_ERR_NO_SUCH_USER  7


#define TFTP_STATE_IDLE      10

#define TFTP_STATE_PUT       11
#define TFTP_STATE_PUT_FINI  12
#define TFTP_STATE_GET       13
#define TFTP_STATE_GET_FINI  14

#define TFTP_MAX_MODE_LEN    10
#define TFTP_IP_ADDR_STR_LEN 32
#define TFTP_FILENAME_LEN    64

#define TFTP_GET_CMD_CODE  1
#define TFTP_PUT_CMD_CODE  2

/*
** Event Message IDs
*/

#define TFTP_RECV_LEN_ERR_EID           (TFTP_BASE_EID +  0)
#define TFTP_PUT_FILE_UNDEF_EID         (TFTP_BASE_EID +  1)
#define TFTP_GET_FILE_UNDEF_EID         (TFTP_BASE_EID +  2)
#define TFTP_SEND_PKT_DBG_EID           (TFTP_BASE_EID +  3)
#define TFTP_SEND_PKT_SOCKET_ERR_EID    (TFTP_BASE_EID +  4)
#define TFTP_SEND_PKT_ENCODE_ERR_EID    (TFTP_BASE_EID +  5)
#define TFTP_ENCODE_RRQ_ERR_EID         (TFTP_BASE_EID +  6)
#define TFTP_ENCODE_WRQ_ERR_EID         (TFTP_BASE_EID +  7)
#define TFTP_ENCODE_DATA_ERR_EID        (TFTP_BASE_EID +  8)
#define TFTP_ENCODE_ERR_MSG_ERR_EID     (TFTP_BASE_EID +  9)
#define TFTP_DECODE_OPCODE_ERR_EID      (TFTP_BASE_EID + 10)
#define TFTP_DECODE_BLOCK_NUM_ERR_EID   (TFTP_BASE_EID + 11)
#define TFTP_DECODE_DATA_ERR_EID        (TFTP_BASE_EID + 12)
#define TFTP_DECODE_REQ_STR_ERR_EID     (TFTP_BASE_EID + 13)
#define TFTP_DECODE_DBG_EID             (TFTP_BASE_EID + 14)
#define TFTP_DECODE_ERR_MSG_ERR_EID     (TFTP_BASE_EID + 15)
#define TFTP_DUMP_DBG_EID               (TFTP_BASE_EID + 16)
#define TFTP_DECODE_ERR_MSG_EID         (TFTP_BASE_EID + 17)
#define TFTP_RRQ_FILE_OPEN_ERR_EID      (TFTP_BASE_EID + 18)
#define TFTP_WRQ_FILE_OPEN_ERR_EID      (TFTP_BASE_EID + 19)
#define TFTP_WRQ_DBG_EID                (TFTP_BASE_EID + 20)
#define TFTP_UNEXP_OPCODE_ERR_EID       (TFTP_BASE_EID + 21)
#define TFTP_UNEXP_BLOCKNUM_ERR_EID     (TFTP_BASE_EID + 22)
#define TFTP_UNDEF_STATE_ERR_EID        (TFTP_BASE_EID + 23)
#define TFTP_GND_GET_COMPLETE_EID       (TFTP_BASE_EID + 24)
#define TFTP_GND_PUT_COMPLETE_EID       (TFTP_BASE_EID + 25)
#define TFTP_STATE_TIMEOUT_EID          (TFTP_BASE_EID + 26)

#define TFTP_TOTAL_EID  27

/**********************/
/** Type Definitions **/
/**********************/

/******************************************************************************
** Command Packets
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   int16   ServerPort;
   char    SrcFilename[TFTP_FILENAME_LEN];
   char    DestFilename[TFTP_FILENAME_LEN];

}  OS_PACK TFTP_PutFileCmdMsg;
#define TFTP_PUT_FILE_CMD_DATA_LEN  (sizeof(TFTP_PutFileCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   int16   ServerPort;
   char    SrcFilename[TFTP_FILENAME_LEN];
   char    DestFilename[TFTP_FILENAME_LEN];

}  OS_PACK TFTP_GetFileCmdMsg;
#define TFTP_GET_FILE_CMD_DATA_LEN  (sizeof(TFTP_GetFileCmdMsg) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

   uint8   Header[CFE_SB_TLM_HDR_SIZE];

   uint16  CmdCode;  /* TFTP_xxx_CMD_CODE  */

   char    SrcFilename[TFTP_FILENAME_LEN];
   char    DestFilename[TFTP_FILENAME_LEN];


} OS_PACK TFTP_TransferReqPkt;
#define TFTP_TRANSFER_REQ_PKT_LEN sizeof (TFTP_TransferReqPkt)


/******************************************************************************
** TFTP Class
*/

typedef struct {

   /*
   ** Network Interface
   */
   
   NETIF_Class  NetIf;

   /*
   ** Socket Management
   ** - Added separate send buffer to help debug
   ** - InSocket created by constructor. OutSocket created by first get or put file command
   */

   uint32  MsgCnt;
   uint32  MsgErrCnt;
   uint8   MsgBuf[TFTP_MAX_MSGSIZE];     /* Recv buffer     */
   uint16  MsgLen;                       /* Recv buffer len */
   uint8   SendMsgBuf[TFTP_MAX_MSGSIZE]; /* Send buffer     */
   uint16  SendMsgLen;                   /* Send buffer len */

   uint8   NetIFid;
   uint8   Pad0;
   
   /*
   ** File information
   */

   int32    FileHandle;                        /* File OSAL file descriptor       */
   uint16   GetFileCnt;
   uint16   PutFileCnt;
   char     SrcFilename[TFTP_FILENAME_LEN];
   char     DestFilename[TFTP_FILENAME_LEN];

   /*
   ** TFTP state information
   */

   char     Mode[TFTP_MAX_MODE_LEN];  /* TFTP transfer mode              */
   int	    State;                    /* State of the TFTP state machine */
   uint16   Timer;                    /* Iimer for timeout logic         */
   uint16   BlockNum;                 /* Current block number            */

   /*
   ** Telemetry Packets
   */
   
   TFTP_TransferReqPkt TransferReqPkt;

} TFTP_Class;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: TFTP_Constructor
**
** Construct a TFTP object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void TFTP_Constructor(TFTP_Class*  TftpPtr);


/******************************************************************************
** Function: TFTP_Read
**
** Read up to MaxMsgRead messages and return the number of messages read. Each
** message is processed as it it read.
**
*/
uint16 TFTP_Read(uint16 MaxMsgRead);


/******************************************************************************
** Function:  TFTP_ResetStatus
**
*/
void TFTP_ResetStatus(void);


/******************************************************************************
** Function: TFTP_PutFileCmd
**
** Put a file from the caller to the system hosting the TFTP app.
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr function signature
*/
boolean TFTP_PutFileCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: TFTP_GetFileCmd
**
** Get a file from the system hosting the TFTP app.
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr function signature
*/
boolean TFTP_GetFileCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _tftp_ */
