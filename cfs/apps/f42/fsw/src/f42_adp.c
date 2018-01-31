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
**   4. TODO - Fix partial & full table load/commit process. Right now a quick & dirty
**      copy to local structure and then update 42's FSW structure. Get rid of
**      local adapter table copy and add a commit function that is called from
**      the control table load function. Think thru initial values from a table or
**      use #defines
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
#include "fswkit.h"

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

   /* If a class state variable can't default to zero then must be set after this */
   CFE_PSP_MemSet((void*)F42Adp, 0, sizeof(F42_ADP_Class));
   
   InitFSW(&(F42Adp->Fsw));
   
   F42Adp->SunTargetAxis = CUBESATFSW_SUN_TARGET_Y_AXIS_PLUS;
   
   CTRLTBL_Constructor(&(F42Adp->CtrlTbl), F42_ADP_GetCtrlTblPtr, F42_ADP_LoadCtrlTbl, F42_ADP_LoadCtrlTblEntry);
	   
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
   CubeSatFSW(&(F42Adp->Fsw),F42Adp->SunTargetAxis, F42Adp->Fault[F42_ADP_FAULT_CSS]);

   SendActuatorPkt();
   
} /* End F42_ADP_RunController() */


/******************************************************************************
** Function:  F42_ADP_ResetStatus
**
*/
void F42_ADP_ResetStatus(void) {
  
   CTRLTBL_ResetStatus();
  
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
   boolean  RetStatus = FALSE;

   if ( SetModeCmd->NewMode == F42_ADP_MODE_INIT) {
     
     InitFSW(&(F42Adp->Fsw));
     RetStatus = TRUE;
     CFE_EVS_SendEvent(F42_ADP_SET_MODE_INFO_EID, CFE_EVS_INFORMATION,
                       "Set control mode to init. Commanded Mode = %d",SetModeCmd->NewMode);
	 
   }
   else {
     
     CFE_EVS_SendEvent(F42_ADP_INVALID_MODE_ERR_EID, CFE_EVS_ERROR,
                       "Invalid commanded control mode %d",SetModeCmd->NewMode);
     
   }

   return RetStatus;

} /* End F42_ADP_SetModeCmd() */


/******************************************************************************
** Function: F42_ADP_SetFaultCmd
**
** Set/Clear the command specified fault (F42_ADP_FAULT_xxx).
*/
boolean F42_ADP_SetFaultCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

   const F42_ADP_SetFaultCmdPkt *SetFaultCmd = (const F42_ADP_SetFaultCmdPkt *) MsgPtr;
   boolean  RetStatus = FALSE;

   if (SetFaultCmd->Id <= F42_ADP_FAULT_ID_MAX) {
     
      if (SetFaultCmd->State == TRUE || SetFaultCmd->State == FALSE) {
         F42Adp->Fault[SetFaultCmd->Id] = SetFaultCmd->State;
		 RetStatus = TRUE;
         CFE_EVS_SendEvent(F42_ADP_SET_FAULT_INFO_EID, CFE_EVS_INFORMATION,
                           "Set fault identifier %d to boolean state %d", SetFaultCmd->Id, SetFaultCmd->State);
	  }
	  else {
         CFE_EVS_SendEvent(F42_ADP_INVALID_FAULT_STATE_ERR_EID, CFE_EVS_ERROR,
                           "Invalid commanded fault state %d. Must be a boolean (0 or 1)",SetFaultCmd->State);
	  } /* End if valid state */
	  
   } /* End if valid ID */
   
   else {
     
      CFE_EVS_SendEvent(F42_ADP_INVALID_FAULT_ID_ERR_EID, CFE_EVS_ERROR,
                        "Invalid commanded fault identifier %d is greater than max ID %d",SetFaultCmd->Id, F42_ADP_FAULT_ID_MAX);
     
   }

   return RetStatus;

} /* End F42_ADP_SetFaultCmd() */


/******************************************************************************
** Function: F42_ADP_SetSunTargetCmd
**
** Set the spacecraft body axis to be used as the sun target. See
** CUBESATFSW_SUN_TARGET_xxx for axis definitions.
*/
boolean F42_ADP_SetSunTargetCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

   const F42_ADP_SetSunTargetCmdPkt *SetSunTargetCmd = (const F42_ADP_SetSunTargetCmdPkt *) MsgPtr;
   boolean  RetStatus = FALSE;

   if (SetSunTargetCmd->Axis <= CUBESATFSW_SUN_TARGET_ID_MAX) {
     
      F42Adp->SunTargetAxis = SetSunTargetCmd->Axis;
      RetStatus = TRUE;
      CFE_EVS_SendEvent(F42_ADP_SET_SUN_TARGET_INFO_EID, CFE_EVS_INFORMATION,
                        "Set sun target axis to %d", SetSunTargetCmd->Axis);
   }
   else {
      CFE_EVS_SendEvent(F42_ADP_INVALID_SUN_TARGET_ERR_EID, CFE_EVS_ERROR,
                        "Invalid commanded sun target axis %d is greater than max ID %d",SetSunTargetCmd->Axis, CUBESATFSW_SUN_TARGET_ID_MAX);
     
   } /* End if valid state */

   return RetStatus;

} /* End F42_ADP_SetSunTargetCmd() */


/******************************************************************************
** Function: F42_ADP_GetCtrlTblPtr
**
*/
const CTRLTBL_Struct* F42_ADP_GetCtrlTblPtr()
{

   return &(F42Adp->CtrlTbl.Data);

} /* End F42_ADP_GetCtrlTblPtr() */


/******************************************************************************
** Function: F42_ADP_LoadCtrlTbl
**
** Notes:
**   1. This is a simple data copy. Pass/fail criteria could be added.
**   2. TODO - See file header TODO
** 
*/
boolean F42_ADP_LoadCtrlTbl(CTRLTBL_Struct* NewTbl)
{

   int i;

   CFE_PSP_MemCpy(&(F42Adp->CtrlTbl), NewTbl, sizeof(CTRLTBL_Struct));
   
   F42Adp->Fsw.MOI[0] = F42Adp->CtrlTbl.Data.Moi.x;
   F42Adp->Fsw.MOI[1] = F42Adp->CtrlTbl.Data.Moi.y;
   F42Adp->Fsw.MOI[2] = F42Adp->CtrlTbl.Data.Moi.z;

   for(i=0;i<3;i++) 
      FindPDGains(F42Adp->Fsw.MOI[i], F42Adp->CtrlTbl.Data.PdGainParam.w,F42Adp->CtrlTbl.Data.PdGainParam.z,
                  &(F42Adp->Fsw.Kr[i]),&(F42Adp->Fsw.Kp[i]));
   
   OS_printf("F42_ADP_LoadCtrlTbl() Kr[0] = %.6f, Kp[0] = %.6f\n", F42Adp->Fsw.Kr[0], F42Adp->Fsw.Kp[0] );
   
   return TRUE;

} /* End F42_ADP_LoadCtrlTbl() */


/******************************************************************************
** Function: F42_ADP_LoadCtrlTblEntry
**
** Notes:
**   1. This is a simple data copy. Pass/fail criteria could be added.
**   2. TODO - See file header TODO
*/
boolean F42_ADP_LoadCtrlTblEntry(uint16 ObjId, void* ObjData)
{

   int i;
   boolean RetStatus = TRUE;
   
   switch (ObjId) {
      
	  case CTRLTBL_OBJ_MOI:
         CFE_PSP_MemCpy(&(F42Adp->CtrlTbl.Data.Moi), ObjData, sizeof(MomentOfInteria_Struct));
         F42Adp->Fsw.MOI[0] = F42Adp->CtrlTbl.Data.Moi.x;
         F42Adp->Fsw.MOI[1] = F42Adp->CtrlTbl.Data.Moi.y;
         F42Adp->Fsw.MOI[2] = F42Adp->CtrlTbl.Data.Moi.z;
         break;
		 
      case CTRLTBL_OBJ_PD_GAIN_PARAM:
         CFE_PSP_MemCpy(&(F42Adp->CtrlTbl.Data.PdGainParam), ObjData, sizeof(PdGainParam_Struct));
         break;
	  
	  default:
	     RetStatus = FALSE;
   
   } /* End ObjId switch */
   
   for(i=0;i<3;i++) 
      FindPDGains(F42Adp->Fsw.MOI[i], F42Adp->CtrlTbl.Data.PdGainParam.w,F42Adp->CtrlTbl.Data.PdGainParam.z,
                  &(F42Adp->Fsw.Kr[i]),&(F42Adp->Fsw.Kp[i]));

   OS_printf("F42_ADP_LoadCtrlEntry() Kr[0] = %.6f, Kp[0] = %.6f\n", F42Adp->Fsw.Kr[0], F42Adp->Fsw.Kp[0] );
   
   return RetStatus;

} /* End F42_ADP_LoadCtrlTblEntry() */


/* end of file */
