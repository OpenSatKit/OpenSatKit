/*
** Purpose: Implement the 42 simulator FSW adapter
**
** Notes:
**   1. This is part of prototype effort to port a 42 simulator FSW controller
**      component into a cFS-based application.
**   2. The 42 AcType data structure is used unchanged and serves as the I/O
**      between this adapter and the 42 code.
**   3. This object serves as a wrapper/adapter for the 42 FSW module. The cFS
**      FSW controller application should use this adapter for all interactions
**      to/from the 42 interface. I42's interface object IF42 defines the packets
**      that are exchanged between the two apps. If 
**      i42 is changed/replaced to interface with hardware then only this adapter
**      object will need to change.
**   4. Since this is an educational app many events are defined as informational. A
**      flight app should minimize "event clutter" and define some of these as debug.
**   5. TODO - Fix partial & full table load/commit process. Right now a quick & dirty
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

#include "i42_msgids.h"
#include "osk_42_lib.h"
#include "f42_adp.h"

/* Ac struct access macros */
#define AC42          &(F42Adp->Ac42->Obj)  
#define AC42_(field)  (F42Adp->Ac42->Obj.field)

/*
** Global File Data
*/

static F42_ADP_Class* F42Adp = NULL;


/*
** Local Function Prototypes
*/

static void  SensorPktToAcStruct(IF42_SensorDataPkt* SensorDataPkt);
static void  AcStructToTlm(void);
static void  TblToAcStruct(void);
static char* GetOvrStr (uint8 State);


/******************************************************************************
** Function: F42_ADP_Constructor
**
** Initialize a F42_ADP object.
**
** Notes:
**   1. This must be called prior to any other function.
**   2. Assumes IF42 has constructed osk_42_lib's AC42 shared data structure 
**
*/
void F42_ADP_Constructor(F42_ADP_Class*  F42AdpObj) {

   int i;
   
   F42Adp = F42AdpObj;

   /* If a class state variable can't default to zero then must be set after this */
   CFE_PSP_MemSet((void*)F42Adp, 0, sizeof(F42_ADP_Class));
   
   for (i=F42_ADP_OVR_ID_MIN; i<F42_ADP_OVR_ID_MAX; i++) F42Adp->Override[i] = F42_ADP_OVR_USE_SIM;
     
   CTRLTBL_Constructor(&(F42Adp->CtrlTbl), F42_ADP_GetCtrlTblPtr, F42_ADP_LoadCtrlTbl, F42_ADP_LoadCtrlTblEntry);
	   
   CFE_SB_InitMsg(&F42Adp->ActuatorPkt,  I42_ACTUATOR_CMD_DATA_MID, IF42_ACTUATOR_CMD_DATA_PKT_LEN, TRUE);
   CFE_SB_InitMsg(&F42Adp->CtrlPkt,      F42_CTRL_TLM_MID,          F42_ADP_TLM_CTRL_PKT_LEN, TRUE);
   CFE_SB_InitMsg(&F42Adp->CtrlGainsPkt, F42_CTRL_GAINS_TLM_MID,    F42_ADP_TLM_CTRL_GAINS_PKT_LEN, TRUE);

} /* End F42_ADP_Constructor() */


/******************************************************************************
** Function: F42_ADP_Run42Fsw
**
** Run the 42 simulator's FSW control law. 42's Ac structure is used for
** all sensor/actuator data I/O. 
**
*/
void F42_ADP_Run42Fsw(IF42_SensorDataPkt*  SensorDataPkt) {

   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG,
                     "**** F42_ADP_Run42Fsw(%d) ****", (int)F42Adp->CtrlExeCnt);
    
   F42Adp->Ac42 = AC42_GetPtr();
   AC42_(EchoEnabled) = FALSE;
   
   SensorPktToAcStruct(SensorDataPkt);

   if (SensorDataPkt->InitCycle == TRUE) {
      CFE_EVS_SendEvent(F42_ADP_INIT_CONTROLLER_EID, CFE_EVS_INFORMATION, "Initialized contoller");
      InitAC(AC42);
   }
   
   AcFsw(AC42);
   ++F42Adp->CtrlExeCnt;

   AcStructToTlm();

   AC42_ReleasePtr(F42Adp->Ac42);

} /* End F42_ADP_Run42Fsw() */


/******************************************************************************
** Function:  F42_ADP_ResetStatus
**
*/
void F42_ADP_ResetStatus(void) {
  
   CTRLTBL_ResetStatus();
  
   F42Adp->CtrlExeCnt = 0;

} /* End F42_ADP_ResetStatus() */


/******************************************************************************
** Function: F42_ADP_SendCtrlGainsCmd
**
** Send the control gains telemetry packet containing the gains from the
** control table.
*/
boolean F42_ADP_SendCtrlGainsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

   int32    CfeStatus;

   F42Adp->CtrlGainsPkt.Kp[0] = F42Adp->CtrlTbl.Data.Kp.X;
   F42Adp->CtrlGainsPkt.Kp[1] = F42Adp->CtrlTbl.Data.Kp.Y;
   F42Adp->CtrlGainsPkt.Kp[2] = F42Adp->CtrlTbl.Data.Kp.Z;
   
   F42Adp->CtrlGainsPkt.Kr[0] = F42Adp->CtrlTbl.Data.Kr.X;
   F42Adp->CtrlGainsPkt.Kr[1] = F42Adp->CtrlTbl.Data.Kr.Y;
   F42Adp->CtrlGainsPkt.Kr[2] = F42Adp->CtrlTbl.Data.Kr.Z;

   F42Adp->CtrlGainsPkt.Kunl  = F42Adp->CtrlTbl.Data.Kunl;
   
   F42Adp->Ac42 = AC42_GetPtr();
   
   F42Adp->CtrlGainsPkt.Kp[0] = AC42_(CfsCtrl.Kp[0]);
   F42Adp->CtrlGainsPkt.Kp[1] = AC42_(CfsCtrl.Kp[1]);
   F42Adp->CtrlGainsPkt.Kp[2] = AC42_(CfsCtrl.Kp[2]);
   
   F42Adp->CtrlGainsPkt.Kr[0] = AC42_(CfsCtrl.Kr[0]);
   F42Adp->CtrlGainsPkt.Kr[1] = AC42_(CfsCtrl.Kr[1]);
   F42Adp->CtrlGainsPkt.Kr[2] = AC42_(CfsCtrl.Kr[2]);

   F42Adp->CtrlGainsPkt.Kunl  = AC42_(CfsCtrl.Kunl);
   
   AC42_ReleasePtr(F42Adp->Ac42);

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &F42Adp->CtrlGainsPkt);
   CfeStatus = CFE_SB_SendMsg((CFE_SB_Msg_t *) &F42Adp->CtrlGainsPkt);
   
   return (CfeStatus == CFE_SUCCESS);

} /* End F42_ADP_SendCtrlGainsCmd() */


/******************************************************************************
** Function: F42_ADP_SetCtrlModeCmd
**
** Currently controller doesn't have modes so this command only lets the
** user force a controller initialization.
*/
boolean F42_ADP_SetCtrlModeCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

   const F42_ADP_SetCtrlModeCmdPkt *SetCtrlModeCmd = (const F42_ADP_SetCtrlModeCmdPkt *) MsgPtr;
   boolean  RetStatus = FALSE;

   if ( SetCtrlModeCmd->NewMode == F42_ADP_CTRL_MODE_DEMO) {
   
      RetStatus = TRUE;
      CFE_EVS_SendEvent(F42_ADP_DEMO_CTRL_MODE_EID, CFE_EVS_CRITICAL,
                       "HS demo control mode command received. Sending event message %d. Control mode unaffected.", F42_ADP_DEMO_CTRL_MODE_EID);
   }

   if ( SetCtrlModeCmd->NewMode == F42_ADP_CTRL_MODE_INIT) {
     
      //TODO InitFSW(&(F42Adp->Fsw));
      RetStatus = TRUE;
      CFE_EVS_SendEvent(F42_ADP_SET_CTRL_MODE_EID, CFE_EVS_INFORMATION,
                       "Set control mode to init. Commanded Mode = %d",SetCtrlModeCmd->NewMode);
      CFE_EVS_SendEvent(F42_ADP_SET_CTRL_MODE_EID, CFE_EVS_INFORMATION,
                        "**** OSK v2.3: This command has no effect with the current 42 standalone controller ****"); 
	 
   }
   else {
     
     CFE_EVS_SendEvent(F42_ADP_INVLD_CTRL_MODE_EID, CFE_EVS_ERROR,
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
         CFE_EVS_SendEvent(F42_ADP_SET_OVR_EID, CFE_EVS_INFORMATION,
                           "Set override identifier %d to state %s", SetOvrCmd->Id, GetOvrStr(SetOvrCmd->State));
         CFE_EVS_SendEvent(F42_ADP_SET_OVR_EID, CFE_EVS_INFORMATION,
                           "**** OSK v2.3: This command has no effect with the current 42 standalone controller ****"); 
	  }
	  else {
         CFE_EVS_SendEvent(F42_ADP_INVLD_OVR_STATE_EID, CFE_EVS_ERROR,
                           "Invalid commanded override state %d. Must be between %d and %d",SetOvrCmd->State,F42_ADP_OVR_STATE_MIN,F42_ADP_OVR_STATE_MAX);
	  } /* End if valid state */
	  
   } /* End if valid ID */
   
   else {
     
      CFE_EVS_SendEvent(F42_ADP_INVLD_OVR_ID_EID, CFE_EVS_ERROR,
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

   boolean RetStatus = FALSE;
   
   const F42_ADP_SetTargetWhlMomCmdPkt* SetTargetWhlMomCmd = (const F42_ADP_SetTargetWhlMomCmdPkt *) MsgPtr;
   uint8 ValidWheels = 0, i;   

   for (i=0; i<AC42_NWHL; i++) {
   
      if ( SetTargetWhlMomCmd->Whl[i] >= F42Adp->CtrlTbl.Data.HcmdLim.Lower &&
           SetTargetWhlMomCmd->Whl[i] <= F42Adp->CtrlTbl.Data.HcmdLim.Upper ) {
      
         F42Adp->Hcmd[i] = SetTargetWhlMomCmd->Whl[i];
         ValidWheels++;
      
      }
      else {
         
         CFE_EVS_SendEvent(F42_ADP_INVLD_TARGET_WHL_MOM_EID, CFE_EVS_ERROR,
                           "Commanded target wheel %d momentum %0.6e exceeds (lower,upper) limits (%0.6e,%0.6e)",
                           i, SetTargetWhlMomCmd->Whl[i], F42Adp->CtrlTbl.Data.HcmdLim.Lower, F42Adp->CtrlTbl.Data.HcmdLim.Upper);
      }
      
   } /* End wheel loop */ 
      
   if (ValidWheels == AC42_NWHL) {
      
      CFE_EVS_SendEvent(F42_ADP_SET_TARGET_WHL_MOM_EID, CFE_EVS_INFORMATION,
                        "Target wheel momentum set to %0.6e, %0.6e, %0.6e, %0.6e",
                        F42Adp->Hcmd[0],F42Adp->Hcmd[1],F42Adp->Hcmd[2],F42Adp->Hcmd[3]);
      
      CFE_EVS_SendEvent(F42_ADP_SET_TARGET_WHL_MOM_EID, CFE_EVS_INFORMATION,
                        "**** OSK v2.3: This command has no effect with the current 42 standalone controller ****"); 

      RetStatus = TRUE;
      
   } /* End if valid command */
   
   return RetStatus;

} /* End F42_ADP_SetTargetWhlMomCmd() */


/******************************************************************************
** Function: F42_ADP_ConfigDbgCmd
**
** TODO - Add file command parameter
*/

boolean F42_ADP_ConfigDbgCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const F42_ADP_ConfigDbgCmdPkt *ConfigDbgCmd = (const F42_ADP_ConfigDbgCmdPkt *) MsgPtr;
   boolean  RetStatus = FALSE;

   if ( ConfigDbgCmd->NewState == TRUE) {
     
      if (F42Adp->DbgEnabled == TRUE) {
         
         CFE_EVS_SendEvent(F42_ADP_DEBUG_CMD_ERR_EID, CFE_EVS_ERROR, "Enable debug command rejected; debug file currently opened.");
      
      }
      else {
         
         F42Adp->DbgFileHandle = OS_creat(F42_DBG_FILE, OS_WRITE_ONLY);
         
         if (F42Adp->DbgFileHandle >= OS_FS_SUCCESS) {
            
            F42Adp->DbgEnabled = TRUE;
            CFE_EVS_SendEvent(F42_ADP_DEBUG_CMD_EID, CFE_EVS_INFORMATION, "Created debug file %s",F42_DBG_FILE);
            RetStatus = TRUE;
         
         }
         else {

            CFE_EVS_SendEvent(F42_ADP_DEBUG_CMD_ERR_EID, CFE_EVS_ERROR, "Error creating debug file %s",F42_DBG_FILE);
         
         } /* End if error creating file */
      } /* End if debug not enabled */
   } /* End if enable debug command */
   else {
      
      if (F42Adp->DbgEnabled == TRUE) {
         
         F42Adp->DbgEnabled = FALSE;
         OS_close(F42Adp->DbgFileHandle);
         CFE_EVS_SendEvent(F42_ADP_DEBUG_CMD_EID, CFE_EVS_INFORMATION, "Debug file %s closed", F42_DBG_FILE);
      
      }
      else {

         CFE_EVS_SendEvent(F42_ADP_DEBUG_CMD_EID, CFE_EVS_INFORMATION, "Disable debug command rejected; debug not enabled.");
      }
      
   } /* End if disable debug command */
        
   return RetStatus;

} /* End F42_ADP_ConfigDbgCmd() */


/******************************************************************************
** Function:  SensorPktToAcStruct
**
** Notes:
**   1. Assumes caller is performng AC42_GetPtr() and AC42_ReleasePtr() calls.
**
*/
static void SensorPktToAcStruct(IF42_SensorDataPkt* SensorDataPkt) 
{
   
   int i;

   AC42_(Time)         = SensorDataPkt->Time;
   AC42_(GPS[0].Valid) = SensorDataPkt->GpsValid;
   AC42_(StValid)      = SensorDataPkt->StValid;  
   AC42_(SunValid)     = SensorDataPkt->SunValid;  /* CSS/FSS */

   for (i=0; i < 3; i++) {

      AC42_(PosN[i])  = SensorDataPkt->PosN[i];  /* GPS */
      AC42_(VelN[i])  = SensorDataPkt->VelN[i];
   
      AC42_(qbn[i])   = SensorDataPkt->qbn[i];   /* ST */

      AC42_(wbn[i])   = SensorDataPkt->wbn[i];   /* Gyro */
   
      AC42_(svb[i])   = SensorDataPkt->svb[i];   /* CSS/FSS */

      AC42_(bvb[i])   = SensorDataPkt->bvb[i];   /* MTB s*/

      AC42_(Whl[i].H) = SensorDataPkt->WhlH[i];  /* Wheels */

   }
   
   AC42_(qbn[3])   = SensorDataPkt->qbn[3];   /* ST */
   AC42_(Whl[3].H) = SensorDataPkt->WhlH[4];  /* Wheels */

} /* End  SensorPktToAcStruct() */


/******************************************************************************
** Function: AcStructToTlm
**
** Notes:
**   1. Assumes caller is performng AC42_GetPtr() and AC42_ReleasePtr() calls.
**
*/
static void AcStructToTlm(void)
{

   int i;

   F42Adp->CtrlPkt.GpsValid = AC42_(GPS[0].Valid);
   F42Adp->CtrlPkt.StValid  = AC42_(StValid);  
   F42Adp->CtrlPkt.SunValid = AC42_(SunValid); 

   F42Adp->CtrlPkt.SaGcmd     = (float)AC42_(G->Cmd.Ang[0]);
   F42Adp->ActuatorPkt.SaGcmd = AC42_(G->Cmd.Ang[0]);

   for (i=0; i < 3; i++) {

      F42Adp->CtrlPkt.wbn[i]   = (float)AC42_(wbn[i]);
      F42Adp->CtrlPkt.wln[i]   = (float)AC42_(wln[i]);
      F42Adp->CtrlPkt.qbr[i]   = (float)AC42_(qbr[i]);
      F42Adp->CtrlPkt.therr[i] = (float)AC42_(CfsCtrl.therr[i]);
      F42Adp->CtrlPkt.werr[i]  = (float)AC42_(CfsCtrl.werr[i]);
      F42Adp->CtrlPkt.Hvb[i]   = (float)AC42_(Hvb[i]);
      F42Adp->CtrlPkt.svb[i]   = (float)AC42_(svb[i]);

      F42Adp->CtrlPkt.Tcmd[i]     = (float)AC42_(Tcmd[i]); /* Wheel */
      F42Adp->ActuatorPkt.Tcmd[i] = AC42_(Tcmd[i]);
     
      F42Adp->CtrlPkt.Mcmd[i]     = (float)AC42_(Mcmd[i]); /* MTB   */
      F42Adp->ActuatorPkt.Mcmd[i] = AC42_(Mcmd[i]);

   }

   F42Adp->CtrlPkt.qbr[3] = (float)AC42_(qbr[3]);

   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "**** SendActuatorPkt()\n");
   
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &F42Adp->ActuatorPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &F42Adp->ActuatorPkt);

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &F42Adp->CtrlPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &F42Adp->CtrlPkt);

} /* End AcStructToTlm() */
                    

/******************************************************************************
** Function: TblToAcStruct
**
*/
static void TblToAcStruct(void)
{

   F42Adp->Ac42 = AC42_GetPtr();
 
   AC42_(CfsCtrl.Kp[0]) = F42Adp->CtrlTbl.Data.Kp.X;
   AC42_(CfsCtrl.Kp[1]) = F42Adp->CtrlTbl.Data.Kp.Y;
   AC42_(CfsCtrl.Kp[2]) = F42Adp->CtrlTbl.Data.Kp.Z;

   AC42_(CfsCtrl.Kr[0]) = F42Adp->CtrlTbl.Data.Kr.X;
   AC42_(CfsCtrl.Kr[1]) = F42Adp->CtrlTbl.Data.Kr.Y;
   AC42_(CfsCtrl.Kr[2]) = F42Adp->CtrlTbl.Data.Kr.Z;

   AC42_(CfsCtrl.Kunl)  = F42Adp->CtrlTbl.Data.Kunl;

   AC42_ReleasePtr(F42Adp->Ac42);

} /* End TblToAcStruct() */


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

   CFE_PSP_MemCpy(&(F42Adp->CtrlTbl.Data), NewTbl, sizeof(CTRLTBL_Struct));
   
   TblToAcStruct();
   
   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "F42_ADP_LoadCtrlTbl() Kp.X = %.6f, Kr.X = %.6f\n", 
                     F42Adp->CtrlTbl.Data.Kp.X,F42Adp->CtrlTbl.Data.Kr.X);
   
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

   boolean RetStatus = TRUE;
   
   switch (ObjId) {
      
      case CTRLTBL_OBJ_KP:
         CFE_PSP_MemCpy(&(F42Adp->CtrlTbl.Data.Kp), ObjData, sizeof(Kp_Struct));
         break;
		 
      case CTRLTBL_OBJ_KR:
         CFE_PSP_MemCpy(&(F42Adp->CtrlTbl.Data.Kr), ObjData, sizeof(Kr_Struct));
         break;
		 
      case CTRLTBL_OBJ_KUNL:
         CFE_PSP_MemCpy(&(F42Adp->CtrlTbl.Data.Kunl), ObjData, sizeof(float));
         break;
	  
      case CTRLTBL_OBJ_HCMD_LIM:
         CFE_PSP_MemCpy(&(F42Adp->CtrlTbl.Data.HcmdLim), ObjData, sizeof(HcmdLim_Struct));
         break;

      default:
	     RetStatus = FALSE;
   
   } /* End ObjId switch */
   
   TblToAcStruct();

   CFE_EVS_SendEvent(F42_ADP_DEBUG_EID, CFE_EVS_DEBUG, "F42_ADP_LoadCtrlTbl() Kp.X = %.6f, Kr.X = %.6f\n", 
                     F42Adp->CtrlTbl.Data.Kp.X,F42Adp->CtrlTbl.Data.Kr.X);
  
   return RetStatus;

} /* End F42_ADP_LoadCtrlTblEntry() */


/******************************************************************************
** Function: GetOvrStr
**
** Return an override string for event messages
*/
static char* GetOvrStr (uint8 State) {

   static char* OvrStr[] = {"UNDEFINED", "USE 42 SIM", "TRUE", "FALSE"};
   
   if (State >= F42_ADP_OVR_STATE_MIN && State <= F42_ADP_OVR_STATE_MAX)

      return OvrStr[State];
 
   else

      return OvrStr[0];
 
} /* End GetOvrStr() */

