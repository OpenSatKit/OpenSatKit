/*
** Purpose: Define a 42 simulator FSW adapter
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application
**   2. This object serves as a wrapper/adapter for the 42 FSW module. The cFS
**      application should use this adapter for all interactions to/from the 
**      42 interface. 
**   3. In a more complex design individual objects should be created for
**      sensors and actuators.
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/

#ifndef _f42_adp_
#define _f42_adp_

/*
** Includes
*/

#include "app_cfg.h"
#include "osk_42_lib.h"
#include "if42.h"
#include "ctrltbl.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Event Message IDs
*/

#define F42_ADP_SET_CTRL_MODE_EID        (F42_ADP_BASE_EID +  0)
#define F42_ADP_DEMO_CTRL_MODE_EID       (F42_ADP_BASE_EID +  1)
#define F42_ADP_INVLD_CTRL_MODE_EID      (F42_ADP_BASE_EID +  2)
#define F42_ADP_SET_OVR_EID              (F42_ADP_BASE_EID +  3)
#define F42_ADP_INVLD_OVR_STATE_EID      (F42_ADP_BASE_EID +  4)
#define F42_ADP_INVLD_OVR_ID_EID         (F42_ADP_BASE_EID +  5)
#define F42_ADP_SET_TARGET_WHL_MOM_EID   (F42_ADP_BASE_EID +  6)
#define F42_ADP_INVLD_TARGET_WHL_MOM_EID (F42_ADP_BASE_EID +  7)
#define F42_ADP_DEBUG_CMD_EID            (F42_ADP_BASE_EID +  8)
#define F42_ADP_DEBUG_CMD_ERR_EID        (F42_ADP_BASE_EID +  9)
#define F42_ADP_INIT_CONTROLLER_EID      (F42_ADP_BASE_EID + 10)
#define F42_ADP_DEBUG_EID                (F42_ADP_BASE_EID + 11)


/*
** Override Identifiers
*/

#define F42_ADP_OVR_ID_MIN     0
#define F42_ADP_OVR_SUN_VALID  0
#define F42_ADP_OVR_SPARE      1
#define F42_ADP_OVR_ID_MAX     1
#define F42_ADP_OVR_ID_CNT     2

#define F42_ADP_OVR_STATE_MIN  1
#define F42_ADP_OVR_USE_SIM    1
#define F42_ADP_OVR_TO_TRUE    2
#define F42_ADP_OVR_TO_FALSE   3
#define F42_ADP_OVR_STATE_MAX  3


#define F42_ADP_CTRL_MODE_INIT  1
#define F42_ADP_CTRL_MODE_TBD   2
#define F42_ADP_CTRL_MODE_MAX   2
#define F42_ADP_CTRL_MODE_DEMO  99   /* Special value to demo HS monitoring F42 events */

/**********************/
/** Type Definitions **/
/**********************/

/******************************************************************************
** Command Packets
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

}  OS_PACK F42_ADP_SendCtrlGainsCmdPkt;
#define F42_ADP_SEND_CTRL_GAINS_CMD_DATA_LEN  (sizeof(F42_ADP_SendCtrlGainsCmdPkt) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   int16   NewMode;

}  OS_PACK F42_ADP_SetCtrlModeCmdPkt;
#define F42_ADP_SET_CTRL_MODE_CMD_DATA_LEN  (sizeof(F42_ADP_SetCtrlModeCmdPkt) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint8    Id;
   uint8    State;

}  OS_PACK F42_ADP_SetOvrCmdPkt;
#define F42_ADP_SET_OVR_CMD_DATA_LEN  (sizeof(F42_ADP_SetOvrCmdPkt) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   float   Whl[AC42_NWHL];

}  OS_PACK F42_ADP_SetTargetWhlMomCmdPkt;
#define F42_ADP_SET_TARGET_WHL_MOM_CMD_DATA_LEN  (sizeof(F42_ADP_SetTargetWhlMomCmdPkt) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16   NewState;                        /* 0=Disable, 1=Enable */

}  OS_PACK F42_ADP_ConfigDbgCmdPkt;
#define F42_ADP_CONFIG_DBG_CMD_DATA_LEN  (sizeof(F42_ADP_ConfigDbgCmdPkt) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

   uint8  Header[CFE_SB_TLM_HDR_SIZE];
   
   float  wbn[3];
   float  wln[3];
   float  qbr[4];
   float  therr[3];
   float  werr[3];

   float  Hvb[3];

   float  svb[3];

   float  Tcmd[3];    /* Torque Command   */
   float  Mcmd[3];    /* Magentic Command */
   float  SaGcmd;     /* SA */
   
   boolean GpsValid;
   boolean StValid;  
   boolean SunValid; 
   boolean WordAlignSpare;

} OS_PACK F42_ADP_CtrlPkt;
#define F42_ADP_TLM_CTRL_PKT_LEN sizeof (F42_ADP_CtrlPkt)


typedef struct {

   uint8  Header[CFE_SB_TLM_HDR_SIZE];
   
   float  Kp[3];
   float  Kr[3];
   float  Kunl;

} OS_PACK F42_ADP_CtrlGainsPkt;
#define F42_ADP_TLM_CTRL_GAINS_PKT_LEN sizeof (F42_ADP_CtrlGainsPkt)


/******************************************************************************
** F42 Adapter Class
*/

typedef struct {

   /*
   ** F42_ADP Data 
   */
   
   uint32   CtrlExeCnt;

   uint16   Override[F42_ADP_OVR_ID_CNT];
   uint16   CtrlMode;
   
   float    Hcmd[AC42_NWHL]; /* TODO - 42 controller command interface */
   
   int32    DbgFileHandle;
   boolean  DbgEnabled;

   /*
   ** Telemetry
   */
   
   F42_ADP_CtrlPkt          CtrlPkt;
   F42_ADP_CtrlGainsPkt     CtrlGainsPkt;
   IF42_ActuatorCmdDataPkt  ActuatorPkt;

   /*
   ** Contained Objects
   */
   
   AC42_Class    *Ac42;
   CTRLTBL_Class CtrlTbl;
     
} F42_ADP_Class;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: F42_ADP_Constructor
**
** Initialize a F42_ADP object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void F42_ADP_Constructor(F42_ADP_Class*  F42_AdpObj);


/******************************************************************************
** Function: F42_ADP_Run42Fsw
**
** Run the 42 simulator's FSW algorithms
**
*/
void F42_ADP_Run42Fsw(IF42_SensorDataPkt*  SensorDataPkt);


/******************************************************************************
** Function:  F42_ADP_ResetStatus
**
*/
void F42_ADP_ResetStatus(void);


/******************************************************************************
** Function: F42_ADP_SendCtrlGainsCmd
**
** Send the control gains telemetry packet containing the gains from the
** control table.
*/
boolean F42_ADP_SendCtrlGainsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

   
/******************************************************************************
** Function: F42_ADP_SetCtrlModeCmd
**
** Currently controller doesn't have modes so this command only lets the
** user force a controller initialization.
*/
boolean F42_ADP_SetCtrlModeCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: F42_ADP_SetOvrCmd
**
** Set the state of the command specified override (F42_ADP_OVR_xxx).
*/
boolean F42_ADP_SetOvrCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: F42_ADP_SetTargetWhlMomCmd
**
** Set the target wheel momentum.
*/
boolean F42_ADP_SetTargetWhlMomCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: F42_ADP_ConfigDbgCmd
**
** TODO - Add file command parameter
*/
boolean F42_ADP_ConfigDbgCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: F42_ADP_GetCtrlTblPtr
**
*/
const CTRLTBL_Struct* F42_ADP_GetCtrlTblPtr();


/******************************************************************************
** Function: F42_ADP_LoadCtrlTbl
**
*/
boolean F42_ADP_LoadCtrlTbl(CTRLTBL_Struct* NewTbl);


/******************************************************************************
** Function: F42_ADP_LoadCtrlTblEntry
**
*/
boolean F42_ADP_LoadCtrlTblEntry(uint16 ObjId, void* ObjData);


#endif /* _f42_adp_ */
