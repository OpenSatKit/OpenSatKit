/*
** Purpose: Define the "File Input Transfer Protocol" (FITP).
**
** Notes:
**   1. The FITP is a custom algorithm that is similar to the CFDP Class 1
**      protocol and the TFTP algorithm without the acks for each data
**      segment. The need for this protocol was driven by half-duplex and 
**      highly imbalanced communication links.
**   2. Only one file tranfer can be active at a time and it is considered
**      an error if a new transfer is attempted when a trasnfer is already
**      in progress.
**   3. The file transfer is command driven with the file sender issuing
**      the following sequence:
**
**         A. Start File Transfer command
**         B. 1..N Data Segment commands
**         C. Finish File Transfer command
**
**   4. There are no timers associated with the protocol and a cancel file
**      transfer command can be sent at any time.
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

#ifndef _fitp_
#define _fitp_

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

#define FITP_START_TRANSFER_CMD_EID       (FITP_BASE_EID +  0)
#define FITP_START_TRANSFER_CMD_ERR_EID   (FITP_BASE_EID +  1)

#define FITP_DATA_SEGMENT_CMD_EID         (FITP_BASE_EID +  2)
#define FITP_DATA_SEGMENT_CMD_ERR_EID     (FITP_BASE_EID +  3)

#define FITP_FINISH_TRANSFER_CMD_EID      (FITP_BASE_EID +  4)
#define FITP_FINISH_TRANSFER_CMD_ERR_EID  (FITP_BASE_EID +  5)

#define FITP_CANCEL_TRANSFER_CMD_EID      (FITP_BASE_EID +  6)
#define FITP_CANCEL_TRANSFER_CMD_ERR_EID  (FITP_BASE_EID +  7)


/**********************/
/** Type Definitions **/
/**********************/

/******************************************************************************
** Command Packets
*/

typedef struct
{

   uint8   Header[CFE_SB_CMD_HDR_SIZE];
   char    DestFilename[FITP_FILENAME_LEN];

}  OS_PACK FITP_StartTransferCmdMsg_t;
#define FITP_START_TRANSFER_CMD_DATA_LEN  (sizeof(FITP_StartTransferCmdMsg_t) - CFE_SB_CMD_HDR_SIZE)


typedef struct
{

   uint8   Header[CFE_SB_CMD_HDR_SIZE];
   uint16  Id;
   uint16  Len;
   uint8   Data[FITP_DATA_SEG_MAX_LEN];

}  OS_PACK FITP_DataSegmentCmdMsg_t;
#define FITP_DATA_SEGMENT_CMD_DATA_LEN  (sizeof(FITP_DataSegmentCmdMsg_t) - CFE_SB_CMD_HDR_SIZE)


typedef struct
{

   uint8   Header[CFE_SB_CMD_HDR_SIZE];
   uint32  FileLen;
   uint32  FileCrc;
   uint16  LastDataSegmentId;

}  OS_PACK FITP_FinishTransferCmdMsg_t;
#define FITP_FINISH_TRANSFER_CMD_DATA_LEN  (sizeof(FITP_FinishTransferCmdMsg_t) - CFE_SB_CMD_HDR_SIZE)

#define FITP_CANCEL_TRANSFER_CMD_DATA_LEN  PKTUTIL_NO_PARAM_CMD_DATA_LEN


/******************************************************************************
** FITP Class
**
** - See command function implementations for details on the file transfer
**   state management
*/

typedef struct
{

   char     DestFilename[FITP_FILENAME_LEN];

   int32    FileHandle;
   uint32   FileTransferByteCnt;
   uint32   FileRunningCrc;
   boolean  FileTransferActive;
   uint16   FileTransferCnt;

   uint16   LastDataSegmentId;
   uint16   DataSegmentErrCnt;   

} FITP_Class_t;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: FITP_Constructor
**
** Construct a FITP object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void FITP_Constructor(FITP_Class_t*  FitpPtr);


/******************************************************************************
** Function:  FITP_ResetStatus
**
*/
void FITP_ResetStatus(void);


/******************************************************************************
** Function: FITP_StartTransferCmd
**
** 
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
boolean FITP_StartTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FITP_DataSegmentCmd
**
** 
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
boolean FITP_DataSegmentCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FITP_FinishTransferCmd
**
** 
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
boolean FITP_FinishTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FITP_CancelTransferCmd
**
** 
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr_t function signature
*/
boolean FITP_CancelTransferCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _fitp_ */
