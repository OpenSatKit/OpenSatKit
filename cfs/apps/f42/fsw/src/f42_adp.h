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

#include "cfe.h"
#include "fswtypes.h"      /* 42 FSW Types */
#include "app_cfg.h"
#include "ctrltbl.h"

/*
** Event Message IDs
** - Since this is an educational app many events are defined as informational. A
**   flight app should minimize "event clutter" and define some of these as debug.
*/

#define F42_ADP_SET_CTRL_MODE_INFO_EID      (F42_ADP_BASE_EID + 0)
#define F42_ADP_INVALID_CTRL_MODE_ERR_EID   (F42_ADP_BASE_EID + 1)
#define F42_ADP_SET_OVR_INFO_EID            (F42_ADP_BASE_EID + 2)
#define F42_ADP_INVALID_OVR_STATE_ERR_EID   (F42_ADP_BASE_EID + 3)
#define F42_ADP_INVALID_OVR_ID_ERR_EID      (F42_ADP_BASE_EID + 4)
#define F42_ADP_SET_TARGET_WHL_MOM_INFO_EID (F42_ADP_BASE_EID + 5)
#define F42_ADP_SET_TARGET_WHL_MOM_ERR_EID  (F42_ADP_BASE_EID + 6)
#define F42_ADP_DEBUG_EID                   (F42_ADP_BASE_EID + 7)


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

/*
** Type Definitions
*/

typedef struct FSWType F42_FSW; 

/******************************************************************************
** F42 Sensors
**
** This sensor data is really engineering units that would normally be derived
** from sensor inputs.
*/

typedef struct
{
   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   double  DeltaTime;
   double  PosN[3];
   double  VelN[3];
   double  wbn[3];
   double  qbn[4];
   double  svn[3];
   double  svb[3];
   double  bvb[3];
   double  Hw[3];
   int     SunValid;

} OS_PACK F42_ADP_SensorPkt;
#define F42_ADP_SENSOR_PKT_LEN sizeof (F42_ADP_SensorPkt)


/******************************************************************************
** F42 Actuators
*/

typedef struct
{
   uint8   Header[CFE_SB_TLM_HDR_SIZE];

   double  WhlTorqCmd[3];
   double  MtbCmd[3];
   double  SaGimbalCmd;

} OS_PACK F42_ADP_ActuatorPkt;
#define F42_ADP_ACTUATOR_PKT_LEN sizeof (F42_ADP_ActuatorPkt)


/******************************************************************************
** F42 Adapter Class
*/

typedef struct {

   uint8   Override[F42_ADP_OVR_ID_CNT];   
  
   uint16  CtrlMode;

   CTRLTBL_Class CtrlTbl;
  
   F42_FSW Fsw;
   
   F42_ADP_ActuatorPkt  ActuatorPkt;
   
} F42_ADP_Class;

/******************************************************************************
** Command Functions
*/

typedef struct
{

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   int16   NewMode;

}  OS_PACK F42_ADP_SetCtrlModeCmdPkt;
#define F42_ADP_SET_CTRL_MODE_CMD_DATA_LEN  (sizeof(F42_ADP_SetCtrlModeCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct
{

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint8    Id;
   uint8    State;

}  OS_PACK F42_ADP_SetOvrCmdPkt;
#define F42_ADP_SET_OVR_CMD_DATA_LEN  (sizeof(F42_ADP_SetOvrCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct
{

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   float   Whl[3];

}  OS_PACK F42_ADP_SetTargetWhlMomCmdPkt;
#define F42_ADP_SET_TARGET_WHL_MOM_CMD_DATA_LEN  (sizeof(F42_ADP_SetTargetWhlMomCmdPkt) - CFE_SB_CMD_HDR_SIZE)


/*
** Exported Functions
*/

/******************************************************************************
** Function: F42_ADP_Constructor
**
** Initialize a F42_ADP object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void F42_ADP_Constructor(F42_ADP_Class*  F42_AdpPtr);


/******************************************************************************
** Function: F42_ADP_Run42Fsw
**
** Run the 42 simulator's FSW algorithms
**
*/
void F42_ADP_Run42Fsw(F42_ADP_SensorPkt*  SensorPkt);


/******************************************************************************
** Function:  F42_ADP_ResetStatus
**
*/
void F42_ADP_ResetStatus(void);


/******************************************************************************
** Function: F42_ADP_SetCtrlModeCmd
**
** Currently controller doesn't have modes so this command only lets the
** force a controller initialization.
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
