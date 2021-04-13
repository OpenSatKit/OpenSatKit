/*
** Purpose: Trivial File Transport Protocol application. 
**
** Notes:
**   None
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
#ifndef _tftp_app_
#define _tftp_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "tftp.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define TFTP_APP_INIT_EID        (TFTP_APP_BASE_EID + 0)
#define TFTP_APP_NOOP_EID        (TFTP_APP_BASE_EID + 1)
#define TFTP_APP_EXIT_EID        (TFTP_APP_BASE_EID + 2)
#define TFTP_APP_INVALID_MID_EID (TFTP_APP_BASE_EID + 3)


/**********************/
/** Type Definitions **/
/**********************/

/******************************************************************************
** Command Packets
*/


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** File transfer data
   */

   uint16  GetFileCnt;
   uint16  PutFileCnt;
   uint16  BlockNum;

   char    SrcFilename[TFTP_FILENAME_LEN];
   char    DestFilename[TFTP_FILENAME_LEN];

   uint8   State;
   uint8   NetIFid; /* Peer for file transfer */

   /*
   ** Network connection Data
   */
   
   uint32   RecvMsgCnt;
   uint32   RecvMsgErrCnt;

} OS_PACK TFTP_APP_HkPkt;
#define TFTP_APP_TLM_HK_LEN sizeof (TFTP_APP_HkPkt)

   
/******************************************************************************
** TFTP Class
*/

typedef struct {

   /* 
   ** App Framework
   */   
   CFE_SB_PipeId_t CmdPipe;
   CMDMGR_Class    CmdMgr;

   /*
   ** Telemetry Packets
   */
   TFTP_APP_HkPkt  HkPkt;

   /*
   ** App Objects
   */   
   TFTP_Class   Tftp;

} TFTP_APP_Class;

/*******************/
/** Exported Data **/
/*******************/

extern TFTP_APP_Class  TftpApp;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: TFTP_AppMain
**
*/
void TFTP_AppMain(void);


/******************************************************************************
** Function: TFTP_APP_NoOpCmd
**
*/
boolean TFTP_APP_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: TFTP_APP_ResetAppCmd
**
*/
boolean TFTP_APP_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _tftp_app_ */
