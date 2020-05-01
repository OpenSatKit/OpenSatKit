/*
** Purpose: Define the OpenSat Kit Telemetry Output application. This app
**          receives telemetry packets from the software bus and uses its
**          packet table to determine whether packets should be sent over
**          a UDP socket.
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
#ifndef _kit_to_app_
#define _kit_to_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "pkttbl.h"
#include "pktmgr.h"

/*
** Macro Definitions
*/

#define KIT_TO_APP_INIT_EID        (KIT_TO_APP_BASE_EID + 0)
#define KIT_TO_APP_NOOP_EID        (KIT_TO_APP_BASE_EID + 1)
#define KIT_TO_APP_EXIT_EID        (KIT_TO_APP_BASE_EID + 2)
#define KIT_TO_APP_INVALID_MID_EID (KIT_TO_APP_BASE_EID + 3)


/*
** Type Definitions
*/

typedef struct
{

   CMDMGR_Class CmdMgr;
   TBLMGR_Class TblMgr;
   PKTTBL_Class PktTbl;
   PKTMGR_Class PktMgr;

   CFE_SB_PipeId_t CmdPipe;

} KIT_TO_Class;

typedef struct
{

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** PKTTBL Data
   */

   uint8    PktTblLastLoadStatus;
   boolean  SpareAlignByte;
   uint16   PktTblAttrErrCnt;

   /*
   ** PKTMGR Data
   */

   uint16   TlmSockId;
   char     TlmDestIp[PKTMGR_IP_STR_LEN];

} OS_PACK KIT_TO_HkPkt;

#define KIT_TO_TLM_HK_LEN sizeof (KIT_TO_HkPkt)


typedef struct
{

   uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
   uint16             synch;
#if 0
   bit_field          bit1:1;
   bit_field          bit2:1;
   bit_field          bit34:2;
   bit_field          bit56:2;
   bit_field          bit78:2;

   bit_field          nibble1:4;
   bit_field          nibble2:4;
#endif
   uint8              bl1, bl2;       /* boolean */
   int8               b1, b2, b3, b4;
   int16              w1,w2;
   int32              dw1, dw2;
   float              f1, f2;
   double             df1, df2;
   char               str[10];

} OS_PACK KIT_TO_DataTypePkt;

#define KIT_TO_TLM_DATA_TYPE_LEN   sizeof (KIT_TO_DataTypePkt)


/*
** Exported Data
*/

extern KIT_TO_Class  KitTo;


/*
** Exported Functions
*/

/******************************************************************************
** Function: KIT_TO_AppMain
**
*/
void KIT_TO_AppMain(void);


/******************************************************************************
** Function: KIT_TO_NoOpCmd
**
** Notes:
**   1. Function signature must match the CMDMGR_CmdFuncPtr definition
**
*/
boolean KIT_TO_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: KIT_TO_ResetAppCmd
**
** Notes:
**   1. Function signature must match the CMDMGR_CmdFuncPtr definition
**
*/
boolean KIT_TO_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _kit_to_app_ */
