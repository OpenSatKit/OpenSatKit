/*
** Purpose: File Output application
**
** Notes:
**   1. This app manages the File Output Transfer Protocol (FITP) which is
**      a custom protocol that is similar to the CCSDS File Deliver
**      Protocol (CFDP) Class 1. See fotp.h for protocol details. 
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
#ifndef _file_out_app_
#define _file_out_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "fotp.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define FILE_OUT_APP_INIT_EID        (FILE_OUT_APP_BASE_EID + 0)
#define FILE_OUT_APP_INIT_ERR_EID    (FILE_OUT_APP_BASE_EID + 1)
#define FILE_OUT_APP_NOOP_EID        (FILE_OUT_APP_BASE_EID + 2)
#define FILE_OUT_APP_EXIT_EID        (FILE_OUT_APP_BASE_EID + 3)
#define FILE_OUT_APP_INVALID_MID_EID (FILE_OUT_APP_BASE_EID + 4)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/


/******************************************************************************
** Telemetry Packets
*/

typedef struct
{

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** File transfer data
   */
   
   uint8   FileTransferCnt;     /* Number of complete file transfers       */                
   uint8   FileTransferState;   /* See FOTP_FileTransferState_t definition */ 
   uint8   PausedTransferState; /* Identify which state was paused         */ 
   uint8   PrevSegmentFailed;   /* If true then FOTP attempts to resend    */ 
   
   uint32  FileTranferByteCnt;  /* Number of file data bytes sent          */
   uint32  FileRunningCrc;      /* Running CRC of file data sent           */
   
   uint32  DataTransferLen;   
   uint32  FileLen;
   uint32  FileByteOffset;      /* DataSegmentOffset*DataSegmentLen        */
   uint16  DataSegmentLen;      /* Length in start transfer command        */
   uint16  DataSegmentOffset;   /* Starting data segment                   */   
   uint16  NextDataSegmentId;
   
   char    SrcFilename[FOTP_FILENAME_LEN];
   
} OS_PACK FILE_OUT_HkPkt_t;
#define FILE_OUT_TLM_HK_LEN sizeof (FILE_OUT_HkPkt_t)

   
/******************************************************************************
** FILE_OUT Class
*/

typedef struct
{

   /* 
   ** App Framework
   */
   
   INITBL_Class    IniTbl;
   CFE_SB_PipeId_t CmdPipe;
   CMDMGR_Class    CmdMgr;

   /*
   ** Telemetry Packets
   */
   
   FILE_OUT_HkPkt_t  HkPkt;

   /*
   ** FILE_OUT State & Contained Objects
   */
  
   uint32  CmdMidValue;      
   uint32  SendHkMidValue;  
   uint32  ExecuteMidValue;  
   
   FOTP_Class_t Fotp;

} FILE_OUT_Class_t;


/*******************/
/** Exported Data **/
/*******************/

extern FILE_OUT_Class_t  FileOut;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: FILE_OUT_AppMain
**
*/
void FILE_OUT_AppMain(void);


/******************************************************************************
** Function: FILE_OUT_APP_NoOpCmd
**
*/
boolean FILE_OUT_APP_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILE_OUT_APP_ResetAppCmd
**
*/
boolean FILE_OUT_APP_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _file_out_app_ */
