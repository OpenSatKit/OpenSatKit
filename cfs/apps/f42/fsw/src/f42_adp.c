/*
** Purpose: Implement the 42 simulator FSW adapter
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application.
**   2. The 42 FSW data structure is used unchanged and serves as the I/O
**      between this adapter and the 42 code.
**   3. This object serves as a wrapper/adapter for the 42 FSW module. The cFS
**      FSW controller application should use this adapter for all interactions
**      to/from the 42 interface. It is tightly coupled with i42's netif. If 
**      i42 is changed/replaced to interface with hardware then only this
**      adapter object will need to change.
**   4. TODO - Fix partial & full table load/commit process. Right now a quick & dirty
**      copy to local structure and then update 42's FSW structure. Get rid of
**      local adapter table copy and add a commit function that is called from
**      the control table load function. Think through initial values from a
**      table or use #defines
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
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
#include "ThreeAxisFsw.h"
#include "fswkit.h"

/*
** Global File Data
*/

static F42_ADP_Class* F42Adp = NULL;


/*
** Local Function Prototypes
*/

static void  CopySensorPktToFswStruct(F42_ADP_SensorPkt*  SensorPkt);
static void  CopyFswStructToActuatorPkt(F42_ADP_ActuatorPkt* ActuatorPkt);
static char* GetOvrStr (uint8 State);
static void  SendActuatorPkt(F42_ADP_ActuatorPkt* ActuatorPkt);

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

   int i;
   
   F42Adp = F42AdpPtr;

   /* If a class state variable can't default to zero then must be set after this */
   CFE_PSP_MemSet((void*)F42Adp, 0, sizeof(F42_ADP_Class));
   
   for (i=F42_ADP_OVR_ID_MIN; i<F42_ADP_OVR_ID_MAX; i++) F42Adp->Override[i] = F42_ADP_OVR_USE_SIM;
   
   InitFSW(&(F42Adp->Fsw));

   CTRLTBL_Constructor(&(F42Adp->CtrlTbl), F42_ADP_GetCtrlTblPtr, F42_ADP_LoadCtrlTbl, F42_ADP_LoadCtrlTblEntry);
	   
   CFE_SB_InitMsg(&(F42Adp->ActuatorPkt), F42_ACTUATOR_MID,
                  F42_ADP_ACTUATOR_PKT_LEN, TRUE);

} /* End F42_ADP_Constructor() */


/******************************************************************************
** Function: F42_ADP_Run42Fsw
**
** Run the 42 simulator's FSW control law. 42's FSW data structure is used for
** all sensor/actuator data I/O. 
**
*/
void F42_ADP_Run42Fsw(F42_ADP_SensorPkt*  SensorPkt) {



   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "F42_ADP_Run42Fsw() - About to run ThreeAxisFSW()\n");
   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "DeltaTime = %f, SunValid  = %d\n",SensorPkt->DeltaTime,SensorPkt->SunValid);
   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "PosN[0] = %.6f, PosN[1] = %.6f, PosN[2] = %.6f\n",SensorPkt->PosN[0],SensorPkt->PosN[1],SensorPkt->PosN[2]);
   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "VelN[0] = %.6f, VelN[1] = %.6f, VelN[2] = %.6f\n",SensorPkt->VelN[0],SensorPkt->VelN[1],SensorPkt->VelN[2]);
   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "wbn[0] = %.6f, wbn[1] = %.6f, wbn[2] = %.6f\n",SensorPkt->wbn[0],SensorPkt->wbn[1],SensorPkt->wbn[2]);
   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "qbn[0] = %.6f, qbn[1] = %.6f, qbn[2] = %.6f, qbn[3] = %.6f\n",SensorPkt->qbn[0],SensorPkt->qbn[1],SensorPkt->qbn[2],SensorPkt->qbn[3]);
   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "svn[0] = %.6f, svn[1] = %.6f, svn[2] = %.6f\n",SensorPkt->svn[0],SensorPkt->svn[1],SensorPkt->svn[2]);
   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "svb[0] = %.6f, svb[1] = %.6f, svb[2] = %.6f\n",SensorPkt->svb[0],SensorPkt->svb[1],SensorPkt->svb[2]);

   CopySensorPktToFswStruct(SensorPkt);
   ThreeAxisFSW(&(F42Adp->Fsw));
   CopyFswStructToActuatorPkt(&(F42Adp->ActuatorPkt));

   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "F42_ADP_Run42Fsw(): %0.6e\n", F42Adp->Fsw.GimCmd[0].Ang[0]);

   SendActuatorPkt(&(F42Adp->ActuatorPkt));
   
} /* End F42_ADP_RunRun42Fsw() */


/******************************************************************************
** Function:  F42_ADP_ResetStatus
**
*/
void F42_ADP_ResetStatus(void) {
  
   CTRLTBL_ResetStatus();
  
} /* End F42_ADP_ResetStatus() */


/******************************************************************************
** Function:  CopySensorPktToFswStruct
**
*/
static void CopySensorPktToFswStruct(F42_ADP_SensorPkt*  SensorPkt) {
   
   int i;
   
   F42Adp->Fsw.DT       = SensorPkt->DeltaTime;
   
   F42Adp->Fsw.SunValid = SensorPkt->SunValid;

   if (F42Adp->Override[F42_ADP_OVR_SUN_VALID] == F42_ADP_OVR_TO_TRUE)
      F42Adp->Fsw.SunValid = TRUE;
   if (F42Adp->Override[F42_ADP_OVR_SUN_VALID] == F42_ADP_OVR_TO_FALSE)
      F42Adp->Fsw.SunValid = FALSE;
   
	
   for (i=0; i < 3; i++) {
      F42Adp->Fsw.PosN[i] = SensorPkt->PosN[i];
      F42Adp->Fsw.VelN[i] = SensorPkt->VelN[i];
      F42Adp->Fsw.wbn[i]  = SensorPkt->wbn[i];
      F42Adp->Fsw.qbn[i]  = SensorPkt->qbn[i];
      F42Adp->Fsw.svn[i]  = SensorPkt->svn[i];
      F42Adp->Fsw.svb[i]  = SensorPkt->svb[i];
      F42Adp->Fsw.bvb[i]  = SensorPkt->bvb[i];
      F42Adp->Fsw.Hw[i]   = SensorPkt->Hw[i];
   }
   F42Adp->Fsw.qbn[3] = SensorPkt->qbn[3];  

} /* End CopySensorPktToFswStruct() */


/******************************************************************************
** Function:  CopyFswStructToActuatorPkt
**
*/
static void CopyFswStructToActuatorPkt(F42_ADP_ActuatorPkt* ActuatorPkt) {

   int i;
   
   ActuatorPkt->SaGimbalCmd = F42Adp->Fsw.GimCmd[0].Ang[0];
	
   for (i=0; i < 3; i++) {
      ActuatorPkt->WhlTorqCmd[i] = F42Adp->Fsw.Twhlcmd[i];
      ActuatorPkt->MtbCmd[i]     = F42Adp->Fsw.Mmtbcmd[i];
   }
    
} /* End CopyFswStructToActuatorPkt() */


/******************************************************************************
** Function:  SendActuatorPkt
**
*/
static void SendActuatorPkt(F42_ADP_ActuatorPkt* ActuatorPkt) {
    
   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "SendActuatorPkt()\n");
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) ActuatorPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) ActuatorPkt);

} /* End SendActuatorPkt() */


/******************************************************************************
** Function: F42_ADP_SetCtrlModeCmd
**
** Currently controller doesn't have modes so this command only lets the
** user force a controller initialization.
*/
boolean F42_ADP_SetCtrlModeCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

   const F42_ADP_SetCtrlModeCmdPkt *SetCtrlModeCmd = (const F42_ADP_SetCtrlModeCmdPkt *) MsgPtr;
   boolean  RetStatus = FALSE;

   if ( SetCtrlModeCmd->NewMode == TAF_CTRL_MODE_INIT) {
     
     InitFSW(&(F42Adp->Fsw));
     RetStatus = TRUE;
     CFE_EVS_SendEvent(F42_ADP_SET_CTRL_MODE_INFO_EID, CFE_EVS_INFORMATION,
                       "Set control mode to init. Commanded Mode = %d",SetCtrlModeCmd->NewMode);
	 
   }
   else {
     
     CFE_EVS_SendEvent(F42_ADP_INVALID_CTRL_MODE_ERR_EID, CFE_EVS_ERROR,
                       "Invalid commanded control mode %d",SetCtrlModeCmd->NewMode);
     
   }

   return RetStatus;

} /* End F42_ADP_SetCtrlModeCmd() */



/******************************************************************************
** Function: F42_ADP_SetOvrCmd
**
** Set/Clear the command specified fault (F42_ADP_FAULT_xxx).
*/
boolean F42_ADP_SetOvrCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

   const F42_ADP_SetOvrCmdPkt *SetOvrCmd = (const F42_ADP_SetOvrCmdPkt *) MsgPtr;
   boolean  RetStatus = FALSE;

   if (SetOvrCmd->Id <= F42_ADP_OVR_ID_MAX) {
     
      if (SetOvrCmd->State >= F42_ADP_OVR_STATE_MIN && SetOvrCmd->State <= F42_ADP_OVR_STATE_MAX) {
         F42Adp->Override[SetOvrCmd->Id] = SetOvrCmd->State;
         RetStatus = TRUE;
         CFE_EVS_SendEvent(F42_ADP_SET_OVR_INFO_EID, CFE_EVS_INFORMATION,
                           "Set override identifier %d to state %s", SetOvrCmd->Id, GetOvrStr(SetOvrCmd->State));
	  }
	  else {
         CFE_EVS_SendEvent(F42_ADP_INVALID_OVR_STATE_ERR_EID, CFE_EVS_ERROR,
                           "Invalid commanded override state %d. Must be between %d and %d",SetOvrCmd->State,F42_ADP_OVR_STATE_MIN,F42_ADP_OVR_STATE_MAX);
	  } /* End if valid state */
	  
   } /* End if valid ID */
   
   else {
     
      CFE_EVS_SendEvent(F42_ADP_INVALID_OVR_ID_ERR_EID, CFE_EVS_ERROR,
                        "Invalid commanded override identifier %d is greater than max ID %d",SetOvrCmd->Id, F42_ADP_OVR_ID_MAX);
     
   }

   return RetStatus;

} /* End F42_ADP_SetOvrCmd() */


/******************************************************************************
** Function: F42_ADP_SetTargetWhlMomCmd
**
** Set the target wheel momentum. 
**
*/
boolean F42_ADP_SetTargetWhlMomCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

   const F42_ADP_SetTargetWhlMomCmdPkt* SetTargetWhlMomCmd = (const F42_ADP_SetTargetWhlMomCmdPkt *) MsgPtr;
   uint8 ValidWheels = 0, i;   

   for (i=0; i<3; i++) {
   
      if ( SetTargetWhlMomCmd->Whl[i] >= F42Adp->CtrlTbl.Data.WhlTgtMomLim.Lower &&
           SetTargetWhlMomCmd->Whl[i] <= F42Adp->CtrlTbl.Data.WhlTgtMomLim.Upper ) {
      
         F42Adp->Fsw.Hwcmd[i] = SetTargetWhlMomCmd->Whl[i];
         ValidWheels++;
      
      }
      else {
         
         CFE_EVS_SendEvent(F42_ADP_SET_TARGET_WHL_MOM_ERR_EID, CFE_EVS_ERROR,
                           "Commanded target wheel %d momentum %0.6e exceeds (lower,upper) limits (%0.6e,%0.6e)",
                           i, SetTargetWhlMomCmd->Whl[i], F42Adp->CtrlTbl.Data.WhlTgtMomLim.Lower, F42Adp->CtrlTbl.Data.WhlTgtMomLim.Upper);
      }
      
   } /* End wheel loop */ 
      
   if (ValidWheels == 3) {
      
      CFE_EVS_SendEvent(F42_ADP_SET_TARGET_WHL_MOM_INFO_EID, CFE_EVS_ERROR,
                        "Target wheel momentum set to %0.6e, %0.6e, %0.6e",
                        F42Adp->Fsw.Hwcmd[0],F42Adp->Fsw.Hwcmd[1],F42Adp->Fsw.Hwcmd[2]);
      return TRUE;
      
   } /* End if valid command */
   
   return FALSE;

} /* End F42_ADP_SetTargetWhlMomCmd() */


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

   CFE_PSP_MemCpy(&(F42Adp->CtrlTbl.Data), NewTbl, sizeof(CTRLTBL_Struct));
   
   F42Adp->Fsw.MOI[0] = F42Adp->CtrlTbl.Data.Moi.x;
   F42Adp->Fsw.MOI[1] = F42Adp->CtrlTbl.Data.Moi.y;
   F42Adp->Fsw.MOI[2] = F42Adp->CtrlTbl.Data.Moi.z;

   for(i=0;i<3;i++) 
      FindPDGains(F42Adp->Fsw.MOI[i], F42Adp->CtrlTbl.Data.PdGainParam.w,F42Adp->CtrlTbl.Data.PdGainParam.z,
                  &(F42Adp->Fsw.Kr[i]),&(F42Adp->Fsw.Kp[i]));
   
   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "F42_ADP_LoadCtrlTbl() Kr[0] = %.6f, Kp[0] = %.6f\n", F42Adp->Fsw.Kr[0], F42Adp->Fsw.Kp[0] );
   
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
	  
      case CTRLTBL_OBJ_WHL_TGT_MOM_LIM:
         CFE_PSP_MemCpy(&(F42Adp->CtrlTbl.Data.WhlTgtMomLim), ObjData, sizeof(WhlTgtMomLim_Struct));
         break;

      default:
	     RetStatus = FALSE;
   
   } /* End ObjId switch */
   
   for(i=0;i<3;i++) 
      FindPDGains(F42Adp->Fsw.MOI[i], F42Adp->CtrlTbl.Data.PdGainParam.w,F42Adp->CtrlTbl.Data.PdGainParam.z,
                  &(F42Adp->Fsw.Kr[i]),&(F42Adp->Fsw.Kp[i]));

   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "F42_ADP_LoadCtrlEntry() Kr[0] = %.6f, Kp[0] = %.6f\n", F42Adp->Fsw.Kr[0], F42Adp->Fsw.Kp[0] );
   
   return RetStatus;

} /* End F42_ADP_LoadCtrlTblEntry() */

/******************************************************************************
** Function: F42_ADP_LoadCtrlTblEntry
**
** Notes:
**   1. This is a simple data copy. Pass/fail criteria could be added.
**   2. TODO - See file header TODO
*/
char* GetOvrStr (uint8 State) {

   static char* OvrStr[] = {"UNDEFINED", "USE 42 SIM", "TRUE", "FALSE"};
   
   if (State >= F42_ADP_OVR_STATE_MIN && State <= F42_ADP_OVR_STATE_MAX)

      return OvrStr[State];
 
   else

      return OvrStr[0];
 
 
} /* End GetOvrStr() */

/* end of file */
