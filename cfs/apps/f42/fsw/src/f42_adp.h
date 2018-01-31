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
#include "fswtypes.h"  /* 42 FSW Types */
#include "app_cfg.h"
#include "ctrltbl.h"

/*
** Event Message IDs
** - Since this is an educational app many events are defined as informational. A
**   flight app should minimize "event clutter" and define soem of these as debug.
*/

#define F42_ADP_SET_MODE_INFO_EID            (F42_ADP_BASE_EID + 0)
#define F42_ADP_INVALID_MODE_ERR_EID         (F42_ADP_BASE_EID + 1)
#define F42_ADP_SET_FAULT_INFO_EID           (F42_ADP_BASE_EID + 2)
#define F42_ADP_INVALID_FAULT_STATE_ERR_EID  (F42_ADP_BASE_EID + 3)
#define F42_ADP_INVALID_FAULT_ID_ERR_EID     (F42_ADP_BASE_EID + 4)
#define F42_ADP_SET_SUN_TARGET_INFO_EID      (F42_ADP_BASE_EID + 5)
#define F42_ADP_INVALID_SUN_TARGET_ERR_EID   (F42_ADP_BASE_EID + 6)


/*
** Control Modes
*/

#define F42_ADP_MODE_INIT    1
#define F42_ADP_MODE_SUN_ACQ 2

/*
** Fault Identifiers
*/

#define F42_ADP_FAULT_CSS    0
#define F42_ADP_FAULT_SPARE  1
#define F42_ADP_FAULT_ID_MAX 1
#define F42_ADP_FAULT_ID_CNT 2

/*
** Sun Targets
*/

#define F42_ADP_SUN_TARGET_X_AXIS_PLUS    0
#define F42_ADP_SUN_TARGET_X_AXIS_MINUS   1
#define F42_ADP_SUN_TARGET_Y_AXIS_PLUS    2
#define F42_ADP_SUN_TARGET_Y_AXIS_MINUS   3
#define F42_ADP_SUN_TARGET_Z_AXIS_PLUS    4
#define F42_ADP_SUN_TARGET_Z_AXIS_MINUS   5
#define F42_ADP_SUN_TARGET_ID_MAX         5

/*
** Type Definitions
*/

typedef struct FSWType F42_FSW; 

/******************************************************************************
** F42 Sensors
**
** F42_ADP_CSS has the same fields as 42's "struct FswCssType" but they've 
** been reordered to facilitate bit packing.
*/

typedef struct {
   
   double AmpsToCounts;
   double CountsToIllum;
   double Axis[3];
   double Amps;
   double Illum;
   long   Counts;
   
} F42_ADP_CSS;

typedef struct {

   int     SunValid;
   double  wbn[3];
   F42_ADP_CSS Css[4];

} F42_ADP_Sensor;

typedef struct
{
   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   F42_ADP_Sensor  Sensor;

} OS_PACK F42_ADP_SensorPkt;
#define F42_ADP_SENSOR_PKT_LEN sizeof (F42_ADP_SensorPkt)


/******************************************************************************
** F42 Actuators
*/

typedef struct {

   double Torq[3];

} F42_ADP_WheelCmd;

typedef struct
{
   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   F42_ADP_WheelCmd  WheelCmd;

} OS_PACK F42_ADP_ActuatorPkt;
#define F42_ADP_ACTUATOR_PKT_LEN sizeof (F42_ADP_ActuatorPkt)


/******************************************************************************
** F42 Adapter Class
*/

typedef struct {

   boolean Fault[F42_ADP_FAULT_ID_CNT];   
  
   uint16  ControlMode;
   uint16  SunTargetAxis;

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

}  OS_PACK F42_ADP_SetModeCmdPkt;
#define F42_ADP_SET_MODE_CMD_DATA_LEN  (sizeof(F42_ADP_SetModeCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct
{

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   int8     Id;
   boolean  State;

}  OS_PACK F42_ADP_SetFaultCmdPkt;
#define F42_ADP_SET_FAULT_CMD_DATA_LEN  (sizeof(F42_ADP_SetFaultCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct
{

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16   Axis;

}  OS_PACK F42_ADP_SetSunTargetCmdPkt;
#define F42_ADP_SET_SUN_TARGET_CMD_DATA_LEN  (sizeof(F42_ADP_SetSunTargetCmdPkt) - CFE_SB_CMD_HDR_SIZE)


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
** Function: F42_ADP_RunController
**
** Run the 42 simulator's control law
**
*/
void F42_ADP_RunController(F42_ADP_Sensor* Sensor);


/******************************************************************************
** Function:  F42_ADP_ResetStatus
**
*/
void F42_ADP_ResetStatus(void);


/******************************************************************************
** Function: F42_ADP_SetModeCmd
**
** Currently controller doesn't have modes so this command only lets the
** force a controller initialization.
*/
boolean F42_ADP_SetModeCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: F42_ADP_SetFaultCmd
**
** Set/Clear the command specified fault (F42_ADP_FAULT_xxx).
*/
boolean F42_ADP_SetFaultCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: F42_ADP_SetSunTargetCmd
**
** Set the spacecraft body axis to be used as the sun target. See
** F42_ADP_SUN_TARGET_xxx for axis definitions.
*/
boolean F42_ADP_SetSunTargetCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


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
