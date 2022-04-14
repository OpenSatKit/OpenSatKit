/*
** Purpose: File Input application
**
** Notes:
**   1. This app manages the File Input Transfer Protocol (FITP) which is
**      a custom protocol that is similar to the CCSDS File Delivery
**      Protocol (CFDP) Class 1. See fitp.h for protocol details. 
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
#ifndef _file_in_app_
#define _file_in_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "fitp.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define FILE_IN_APP_INIT_EID        (FILE_IN_APP_BASE_EID + 0)
#define FILE_IN_APP_INIT_ERR_EID    (FILE_IN_APP_BASE_EID + 1)
#define FILE_IN_APP_NOOP_EID        (FILE_IN_APP_BASE_EID + 2)
#define FILE_IN_APP_EXIT_EID        (FILE_IN_APP_BASE_EID + 3)
#define FILE_IN_APP_INVALID_MID_EID (FILE_IN_APP_BASE_EID + 4)


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
   
   uint8   FileTransferCnt;      /* Number of complete file transfers          */                
   uint8   FileTransferActive;   /* Boolean indicating if file transfer active */ 
   uint16  LastDataSegmentId;    /* ID of the last data segment saved to file  */
   uint16  DataSegmentErrCnt;    /* Count of data segments with errors         */                
   uint32  FileTransferByteCnt;  /* Number of file data bytes received/written */
   uint32  FileRunningCrc;       /* Running CRC of file data received          */
   char    DestFilename[FITP_FILENAME_LEN];
   
} OS_PACK FILE_IN_HkPkt_t;
#define FILE_IN_TLM_HK_LEN sizeof (FILE_IN_HkPkt_t)

   
/******************************************************************************
** FILE_IN Class
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
   
   FILE_IN_HkPkt_t  HkPkt;

   /*
   ** FILE_IN State & Contained Objects
   */
  
   uint32  CmdMidValue;      
   uint32  SendHkMidValue;  
   
   FITP_Class_t Fitp;

} FILE_IN_Class_t;


/*******************/
/** Exported Data **/
/*******************/

extern FILE_IN_Class_t  FileIn;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: FILE_IN_AppMain
**
*/
void FILE_IN_AppMain(void);


/******************************************************************************
** Function: FILE_IN_APP_NoOpCmd
**
*/
boolean FILE_IN_APP_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: FILE_IN_APP_ResetAppCmd
**
*/
boolean FILE_IN_APP_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _file_in_app_ */
