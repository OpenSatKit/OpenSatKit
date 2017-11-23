/*
** Purpose: Implement the 42 simulator FSW adapter
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application.
**   2. The 42 FSW data structure is used unchanged and serves as the I/O
**      between this adapter and the 42 code.
**   3. This object serves as a wrapper/adapter for the 42 FSW module. The cFS
**      application should use this adapter for all interactions to/from the 
**      42 interface. 
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
*/

/*
** Includes
*/

#include "f42_adp.h"
#include "CubeSatFsw.h"


/*
** Global File Data
*/

static F42_ADP_Class* F42Adp = NULL;


/*
** Local Function Prototypes
*/

static void SendActuatorPkt(void);


/******************************************************************************
** Function: F42_ADP_Constructor
**
** Initialize a F42_ADP object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void F42_ADP_Constructor(F42_ADP_Class*  F42AdpPtr) {

   F42Adp = F42AdpPtr;

   CFE_PSP_MemSet((void*)F42Adp, 0, sizeof(F42_ADP_Class));
   
   InitFSW(&(F42Adp->Fsw));
   
   
   CFE_SB_InitMsg(&(F42Adp->ActuatorPkt), F42_ACTUATOR_MID,
                  F42_ADP_ACTUATOR_PKT_LEN, TRUE);

} /* End F42_ADP_Constructor() */


/******************************************************************************
** Function: F42_ADP_RunController
**
** Run the 42 simulator's FSW control law
**
*/
void F42_ADP_RunController(F42_ADP_Sensor*  Sensor) {
  
   F42Adp->Fsw.wbn[0] = Sensor->wbn[0];
   F42Adp->Fsw.wbn[1] = Sensor->wbn[1];
   F42Adp->Fsw.wbn[2] = Sensor->wbn[2];
   
   F42Adp->Fsw.SunValid = Sensor->SunValid;
   F42Adp->Fsw.Css[0].Counts = Sensor->Css[0].Counts;
   F42Adp->Fsw.Css[1].Counts = Sensor->Css[1].Counts;
   F42Adp->Fsw.Css[2].Counts = Sensor->Css[2].Counts;
   F42Adp->Fsw.Css[3].Counts = Sensor->Css[3].Counts;

   /* Run control law on the sensor data */
   CubeSatFSW(&(F42Adp->Fsw));

   SendActuatorPkt();
   
} /* End F42_ADP_RunController() */


/******************************************************************************
** Function:  F42_ADP_ResetStatus
**
*/
void F42_ADP_ResetStatus(void) {
  
   /* Nothing to do */
  
} /* End F42_ADP_ResetStatus() */


/******************************************************************************
** Function:  SendActuatorPkt
**
*/
static void SendActuatorPkt(void) {

   F42Adp->ActuatorPkt.WheelCmd.Torq[0] = F42Adp->Fsw.Twhlcmd[0];
   F42Adp->ActuatorPkt.WheelCmd.Torq[1] = F42Adp->Fsw.Twhlcmd[1];
   F42Adp->ActuatorPkt.WheelCmd.Torq[2] = F42Adp->Fsw.Twhlcmd[2];
    
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &F42Adp->ActuatorPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &F42Adp->ActuatorPkt);

} /* End SendActuatorPkt() */

/******************************************************************************
** Function: F42_ADP_SetModeCmd
**
** Currently controller doesn't have modes so this command only lets the
** user force a controller initialization.
*/
boolean F42_ADP_SetModeCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

   const F42_ADP_SetModeCmdPkt *SetModeCmd = (const F42_ADP_SetModeCmdPkt *) MsgPtr;
   boolean  RetStatus = TRUE;

   if ( SetModeCmd->NewMode == F42_ADP_MODE_INIT) {
     
     InitFSW(&(F42Adp->Fsw));
     
   }
   else {
     
     CFE_EVS_SendEvent(F42_ADP_INVALID_MODE_ERR_EID, CFE_EVS_ERROR,
                       "Invalid commanded control mode %d",SetModeCmd->NewMode);
     
   }

   return RetStatus;

} /* End F42_ADP_SetModeCmd() */

/* end of file */
