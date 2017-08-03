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

/*
** Event Message IDs
*/

#define F42_ADP_INVALID_MODE_ERR_EID  (F42_ADP_BASE_EID +  0)

/*
** Control Modes
*/

#define F42_ADP_MODE_INIT    1
#define F42_ADP_MODE_SUN_ACQ 2

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

   uint16  ControlMode;
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


#endif /* _42fsw_adp_ */
