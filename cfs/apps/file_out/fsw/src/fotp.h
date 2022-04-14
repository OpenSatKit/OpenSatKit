/*
** Purpose: Define the "File Output Transfer Protocol" (FOTP).
**
** Notes:
**   1. The FOTP is a custom algorithm that is similar to the CFDP Class 1
**      protocol and the TFTP algorithm without the acks for each data
**      segment. The need for this protocol was driven by half-duplex and 
**      highly imbalanced communication links.
**   2. Only one file tranfer can be active at a time and it is considered
**      an error if a new transfer is attempted when a transfer is already
**      in progress.
**   3. A state machine is used to manage the file transfer. State transitions
**      occur in response to commands or Scheduler execute requests. The frequency
**      of Scheduler requests  
**
**         A. IDLE:
**            - Default state when no file transfer is in progress
**            - START_TRANSFER command received: Transition to START_TRANSFER state
**         B. START_TRANSFER:
**            - Scheduler Execute Request: Senda start transfer telemetry packet
**              and transition to SEND_DATA state
**         C. SEND_DATA:
**            - Scheduler Execute Request: Send 1..N Data Segment telemetry packets.
**              - If end of file transition to FINISH_TRANSFER state
**              - If flow control semaphore indicates the app outputting telemetry
**                can't receive any more data segments remain in SEND_DATA state 
**         D. FINISH_TRANSFER:
**            - Scheduler Execute Request: Send Finish Transfer telemetry packet
**              and transfer to IDLE state
**
**   4. If an error occurs in the START_TRANSFER, SEND_DATA, or FINISH_TRANSFER 
**      states the algorithm will remain in the state and will repeatedly try
**      perform the erroneous operation 
**   5. A pause command received while in the START_TRANSFER, SEND_DATA, or
**      FINISH_TRANSFER will cause the state machine to enter the paused state until
**      either a resume or cancel command is received. The START_TRANSFER and
**      FINISH_TRANSFER states only exist for one execution cycle so the chances of
**      these states being paused is low.  
**   6. There are no timers associated with the protocol
**   7. A cancel file transfer command can be sent at any time 
**      
** References:
**   1. OpenSatKit Object-based Application Developer's Guide and the
**      osk_c_demo app that illustrates best practices with comments.  
**   2. cFS Application Developer's Guide.
**
**   Written by David McComas, licensed under the Apache License, Version 2.0
**   (the "License"); you may not use this file except in compliance with the
**   License. You may obtain a copy of the License at
**
**      http://www.apache.org/licenses/LICENSE-2.0
**
**   Unless required by applicable law or agreed to in writing, software
**   distributed under the License is distributed on an "AS IS" BASIS,
**   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**   See the License for the specific language governing permissions and
**   limitations under the License.
*/

#ifndef _fotp_
#define _fotp_

/*
** Includes
*/

#include <string.h>

#include "app_cfg.h"


/***********************/
/** Macro Definitions **/
/***********************/

/*
** Event Message IDs
*/

#define FOTP_START_TRANSFER_CMD_EID      (FOTP_BASE_EID +  0)
#define FOTP_START_TRANSFER_CMD_ERR_EID  (FOTP_BASE_EID +  1)

#define FOTP_PAUSE_TRANSFER_CMD_EID      (FOTP_BASE_EID +  2)
#define FOTP_PAUSE_TRANSFER_CMD_ERR_EID  (FOTP_BASE_EID +  3)

#define FOTP_RESUME_TRANSFER_CMD_EID     (FOTP_BASE_EID +  4)
#define FOTP_RESUME_TRANSFER_CMD_ERR_EID (FOTP_BASE_EID +  5)

#define FOTP_CANCEL_TRANSFER_CMD_EID     (FOTP_BASE_EID +  6)
#define FOTP_CANCEL_TRANSFER_CMD_ERR_EID (FOTP_BASE_EID +  7)

#define FOTP_SEND_DATA_SEGMENT_EID       (FOTP_BASE_EID +  8)
#define FOTP_SEND_DATA_SEGMENT_ERR_EID   (FOTP_BASE_EID +  9)

#define FOTP_SEND_FILE_TRANSFER_EID      (FOTP_BASE_EID + 10)
#define FOTP_SEND_FILE_TRANSFER_ERR_EID  (FOTP_BASE_EID + 10)

#define FOTP_EXECUTE_EID                 (FOTP_BASE_EID + 11)

/**********************/
/** Type Definitions **/
/**********************/

typedef enum
{

  FOTP_UNDEF = 0,
  FOTP_IDLE,
  FOTP_START,
  FOTP_SEND_DATA,
  FOTP_FINISH,
  FOTP_PAUSED
 
} FOTP_FileTransferState_t;

/******************************************************************************
** Command Packets
*/

typedef struct
{

   uint8   Header[CFE_SB_CMD_HDR_SIZE];
   uint32  DataSegLen;
   uint16  DataSegOffset;
   char    SrcFilename[FOTP_FILENAME_LEN];

}  OS_PACK FOTP_StartTransferCmdMsg_t;
#define FOTP_START_TRANSFER_CMD_DATA_LEN  (sizeof(FOTP_StartTransferCmdMsg_t) - CFE_SB_CMD_HDR_SIZE)

#define FOTP_PAUSE_TRANSFER_CMD_DATA_LEN   PKTUTIL_NO_PARAM_CMD_DATA_LEN
#define FOTP_RESUME_TRANSFER_CMD_DATA_LEN  PKTUTIL_NO_PARAM_CMD_DATA_LEN
#define FOTP_CANCEL_TRANSFER_CMD_DATA_LEN  PKTUTIL_NO_PARAM_CMD_DATA_LEN


/******************************************************************************
** Telemetry Packets
*/

typedef struct
{

   uint8   Header[CFE_SB_TLM_HDR_SIZE];
   uint32  DataLen;                          /* Either file length or file length minus commanded segment offset */
   char    SrcFilename[FOTP_FILENAME_LEN];

} OS_PACK FOTP_StartTransferPkt_t;
#define FOTP_START_TRANSFER_TLM_LEN sizeof (FOTP_StartTransferPkt_t)

typedef struct
{

   uint8   Header[CFE_SB_TLM_HDR_SIZE];
   uint16  Id;
   uint16  Len;
   uint8   Data[FOTP_DATA_SEG_MAX_LEN];   /* Data must be defined last because it is variable length */

} OS_PACK FOTP_DataSegmentPkt_t;
#define FOTP_DATA_SEGMENT_TLM_LEN sizeof (FOTP_DataSegmentPkt_t)
#define FOTP_DATA_SEGMENT_NON_DATA_TLM_LEN (FOTP_DATA_SEGMENT_TLM_LEN-FOTP_DATA_SEG_MAX_LEN)

typedef struct
{

   uint8   Header[CFE_SB_TLM_HDR_SIZE];
   uint32  FileLen;
   uint32  FileCrc;
   uint16  LastDataSegmentId;

} OS_PACK FOTP_FinishTransferPkt_t;
#define FOTP_FINISH_TRANSFER_TLM_LEN sizeof (FOTP_FinishTransferPkt_t)


/******************************************************************************
** FOTP Class
**
** - See execute function implementation for details on the file transfer
**   state management
*/

typedef struct
{

   /*
   ** Framework Objects
   */

   INITBL_Class* IniTbl;

   /*
   ** Telemetry Packets
   */
   
   FOTP_StartTransferPkt_t   StartTransferPkt;
   FOTP_DataSegmentPkt_t     DataSegmentPkt;
   FOTP_FinishTransferPkt_t  FinishTransferPkt;

   /*
   ** FOTP State Data
   */

   char     SrcFilename[FOTP_FILENAME_LEN];
   int32    FileHandle;

   uint32   FileTransferByteCnt;
   uint32   DataTransferLen;
   uint16   DataSegmentLen;
   uint16   DataSegmentOffset;
   uint32   FileLen;
   uint32   FileByteOffset;
   uint32   FileRunningCrc;
   uint16   NextDataSegmentId;
   uint16   FileTransferCnt;
   boolean  PrevSendDataSegmentFailed;
   boolean  LastDataSegment;             /* In error scenarios this needs to be preserved across executions so it can't be local */

   FOTP_FileTransferState_t FileTransferState;
   FOTP_FileTransferState_t PausedFileTransferState;  /* Identifies which state was paused */

} FOTP_Class_t;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: FOTP_Constructor
**
** Construct a FOTP object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void FOTP_Constructor(FOTP_Class_t*  FotpPtr, INITBL_Class* IniTbl);


/******************************************************************************
** Function:  FOTP_ResetStatus
**
*/
void FOTP_ResetStatus(void);


/******************************************************************************
** Function: FOTP_Execute
**
** Manage the transfer of a file after a FOTP_StartTransferCmd() command has_denorm
** been received.
*/
void FOTP_Execute(void);


/******************************************************************************
** Function: FOTP_StartTransferCmd
**
** 
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
boolean FOTP_StartTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FOTP_CancelTransferCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
boolean FOTP_CancelTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

/******************************************************************************
** Function: FOTP_PauseTransferCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
boolean FOTP_PauseTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FOTP_ResumeTransferCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
boolean FOTP_ResumeTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);



#endif /* _fotp_ */
