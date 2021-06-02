/* 
** Purpose: Define a proto application. This app exercises the OpenSatKit 
**          application framework and serves as a good starting point for
**          trying new ideas.
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _osk_c_proto_
#define _osk_c_proto_

/*
** Includes
*/

#include "app_cfg.h"
#include "protobj.h"
#include "protofr.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define OSK_C_PROTO_INIT_INFO_EID            (OSK_C_PROTO_BASE_EID + 0)
#define OSK_C_PROTO_EXIT_ERR_EID             (OSK_C_PROTO_BASE_EID + 1)
#define OSK_C_PROTO_CMD_NOOP_INFO_EID        (OSK_C_PROTO_BASE_EID + 2)
#define OSK_C_PROTO_CMD_INVALID_MID_ERR_EID  (OSK_C_PROTO_BASE_EID + 3)

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
   ** Example Table Data 
   ** - Loaded with status from the last table action 
   */

   uint8    LastTblId;
   uint8    LastTblAction;
   uint8    LastTblActionStatus;

   /*
   ** STATEREP & PROTOFR
   */

   uint8    SimEnabled;
   uint8    SimMode;
   uint8    StateRepTlmMode;
   uint16   StateRepEnabled;
   uint16   StateRepLatched;

   /*
   ** PROTOBJ Data
   */

   boolean  TblDataEnabled;
   uint8    TblId;
   uint8    TblIndex;
   ExTblData_Entry TblData;

} OS_PACK OSK_C_PROTO_HkPkt;
#define OSK_C_PROTO_TLM_HK_LEN sizeof (OSK_C_PROTO_HkPkt)


/******************************************************************************
** KIT_SCH_Class
*/
typedef struct {

   /*
   ** App Frameowrk
   */

   CFE_SB_PipeId_t CmdPipe;
   CMDMGR_Class    CmdMgr;
   TBLMGR_Class    TblMgr;
   STATEREP_Class  StateRep;
   
   /*
   ** Telemetry Packets
   */
   
   OSK_C_PROTO_HkPkt  HkPkt;
   STATEREP_TlmMsg    SrPkt;
   
   /*
   ** App Objects
   */

   PROTOBJ_Class   ProtObj;
   PROTOFR_Class   ProtoFr;
   

} OSK_C_PROTO_Class;



/*
** Exported Data
*/

extern OSK_C_PROTO_Class  OskCProto;

/*
** Exported Functions
*/

/******************************************************************************
** Function: OSK_C_PROTO_Main
**
*/
void OSK_C_PROTO_AppMain(void);

#endif /* _osk_c_proto_ */
