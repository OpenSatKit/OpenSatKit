/*
** Purpose: Implement the object to demo FaultRep module.
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>

#include "app_cfg.h"
#include "demofr.h"

/*
** Global File Data
*/

static DEMOFR_Class*  DemoFr = NULL;

/*
** Local Function Prototypes
*/

static boolean AddTimedFault(DEMOFR_SimFaultCmdParam *SimFaultCmd);
static void ReportTimedSimFaults(void);
const char* SimModeStr(DEMOFR_SimMode  SimMode);
static void StartSim(const DEMOFR_SimFaultCmdParam *SimFaultCmd);
static void StopSim(void);
static boolean WalkFault(void);


/******************************************************************************
** Function: DEMOFR_Constructor
**
** Notes:
**   1. 
*/
void DEMOFR_Constructor(DEMOFR_Class*   DemoFrPtr,
                        FaultRep_Class* FaultRepPtr)
{
 
   DemoFr = DemoFrPtr;

   CFE_PSP_MemSet((void*)DemoFr, 0, sizeof(DEMOFR_Class));

   DemoFr->FaultRep = FaultRepPtr; 
    
} /* End DEMOFR_Constructor() */


/******************************************************************************
** Function:  DEMOFR_ResetStatus
**
*/
void DEMOFR_ResetStatus()
{

   /* Maintain simulation state */

} /* End DEMOFR_ResetStatus() */


/******************************************************************************
** Function: DEMOFR_SimStep
**
** Execute a single simulation step.
**
** Notes:
**   1. The SimFault command determines this function's behavior.
*/
void DEMOFR_SimStep(void)
{
   
   if (DemoFr->SimEnabled) {
   
      switch (DemoFr->SimMode) {
         
         case DEMOFR_MODE_IDLE:
            break;
            
         case DEMOFR_MODE_TIMED_FAULT:
            ReportTimedSimFaults();
            break;
         
         case DEMOFR_MODE_WALKING_FAULT:
            DemoFr->SimEnabled = WalkFault();
            break;
            
         default:
            CFE_EVS_SendEvent(DEMOFR_INVLD_SIM_STEP_MODE_EID, CFE_EVS_ERROR, 
                              "Invalid simulation mode %d. Disabling simulation.", DemoFr->SimMode);
            DemoFr->SimEnabled = FALSE;
            break;

      } /* End SimMode switch */

      if (!DemoFr->SimEnabled)
         DemoFr->SimMode = DEMOFR_MODE_IDLE;
      
   
   } /* End if EnableSim */
   
   
} /* End DEMOFR_SimStep() */


/******************************************************************************
** Function: DEMOFR_SetTlmModeCmd
**
** Set the telemetry reporting mode .
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean DEMOFR_SetTlmModeCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const DEMOFR_SetTlmModeCmdParam *SetTlmModeCmd = (const DEMOFR_SetTlmModeCmdParam *) MsgPtr;
   boolean  RetStatus = TRUE;

   if ( SetTlmModeCmd->Mode == FAULTREP_NEW_REPORT ||
        SetTlmModeCmd->Mode == FAULTREP_MERGE_REPORT) {
   
      CFE_EVS_SendEvent(DEMOFR_CMD_SET_TLM_MODE_EID, CFE_EVS_INFORMATION, 
                        "Fault Report telemetry mode changed from %s to %s", 
                        FaultRep_TlmModeStr(DemoFr->FaultRep->TlmMode),
                        FaultRep_TlmModeStr(SetTlmModeCmd->Mode) );
                        
      FaultRep_SetTlmMode(DemoFr->FaultRep, (FaultRep_TlmMode)SetTlmModeCmd->Mode);
   
   }
   else {
      
      CFE_EVS_SendEvent(DEMOFR_CMD_INVLD_SET_TLM_MODE_EID, CFE_EVS_ERROR, 
                        "Command contains invalid telemetry mode %d", SetTlmModeCmd->Mode);
      RetStatus = FALSE;
   }
   
   return RetStatus;
   
} /* End DEMOFR_SetTlmModeCmd() */


/******************************************************************************
** Function: DEMOFR_SimFaultCmd
**
** Configure the fault simulation. 
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. Helper functions send event messages.
*/
boolean DEMOFR_SimFaultCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const DEMOFR_SimFaultCmdParam *SimFaultCmd = (const DEMOFR_SimFaultCmdParam *) MsgPtr;
   boolean  RetStatus = TRUE;

   /* 
   ** Enable Fault
   ** 
   **  1. If no sim in progress accept the new command
   **  2. If new command is a walking sim then terminate current sim and 
   **     and start a walking sim
   **  3. If new command is a timed fault then add it to an existing timed
   **     fault sim or term a walking sim and start timed sim.
   */
   if (SimFaultCmd->Enable == TRUE) {
      
      if (DemoFr->SimEnabled) {

         switch (SimFaultCmd->Mode) {
         case DEMOFR_MODE_WALKING_FAULT:
            
            StartSim(SimFaultCmd);
            break;
           
         case DEMOFR_MODE_TIMED_FAULT:
      
            if (DemoFr->SimMode == DEMOFR_MODE_TIMED_FAULT) {

              RetStatus = AddTimedFault(SimFaultCmd);
            
            }
            else {
               StartSim(SimFaultCmd);
            }
            break;
            
         default:

            CFE_EVS_SendEvent(DEMOFR_CMD_INVLD_SIM_FAULT_MODE_EID, CFE_EVS_ERROR, 
                              "Command contains invalid sim mode %d", SimFaultCmd->Mode);
            RetStatus = FALSE;
            break;

         } /* End command mode switch */
         
      } /* End if SimEnabled */
      else {

         StartSim(SimFaultCmd);
         
      } /* End if SimEnabled */
   } /* End if enabling a sim */
   else {

      StopSim();
   
   } /* End if disabling a sim */
   
   return RetStatus;

} /* End DEMOFR_SimFaultCmd() */


/******************************************************************************
** Function: AddTimedFault
**
*/
static boolean AddTimedFault(DEMOFR_SimFaultCmdParam *SimFaultCmd)
{
   
   int RetStatus = TRUE;
   
   if (SimFaultCmd->Id >= OSK_DEMO_FAULT_ID_MAX) {
   
      CFE_EVS_SendEvent(DEMOFR_CMD_INVLD_TIMED_FAULT_ID_EID, CFE_EVS_ERROR, 
                        "Command contains invalid id %d. Must be less than %d",
                        SimFaultCmd->Id, OSK_DEMO_FAULT_ID_MAX);
      RetStatus = FALSE;
   
   } /* End invalid ID */
   else {

      DemoFr->SimFault[SimFaultCmd->Id].Step    = SimFaultCmd->Steps;
      DemoFr->SimFault[SimFaultCmd->Id].Enabled = TRUE;
      
      CFE_EVS_SendEvent(DEMOFR_CMD_ADD_TIMED_FAULT_EID, CFE_EVS_INFORMATION, 
                        "Added timed fault for id %d for %d sim steps", 
                        SimFaultCmd->Id, SimFaultCmd->Steps);
   
   } /* End valid ID */
   
   return RetStatus;
      
} /* End AddTimedFault() */


/******************************************************************************
** Function: ReportTimedSimFaults
**
*/
static void ReportTimedSimFaults(void)
{
   
   uint16  i;
   boolean DetFailed = FALSE;
   
   for (i=0; i < OSK_DEMO_FAULT_ID_MAX; i++) {
   
      if (DemoFr->SimFault[i].Enabled) {
      
         FaultRep_FaultDetFailed(DemoFr->FaultRep, i);
         DetFailed = TRUE;
         DemoFr->SimFault[i].Step--;
         DemoFr->SimFault[i].Enabled = (DemoFr->SimFault[i].Step > 0);
      
      } /* End if enabled */
         
   } /* End ID loop */
   
   if (!DetFailed)
      StopSim();
   
} /* End ReportTimedSimFaults() */


/******************************************************************************
** Function: SimModeStr
**
*/
const char* SimModeStr(DEMOFR_SimMode  SimMode)
{

   static char* SimModeStr[] = {
      "IDLE", 
      "TIMED_FAULT",
      "WALKING_FAULT",
      "UNDEF"
   };

   uint8 i = (SimMode < 3) ? SimMode : 3;
   
   return SimModeStr[i];

} /* End SimModeStr() */


/******************************************************************************
** Function: StartSim
**
** Start a new sim clearing any existng state information.
**
** Notes:
**   1. Can assume the commanded SimMode has been validated.
*/
static void StartSim(const DEMOFR_SimFaultCmdParam *SimFaultCmd)
{

   StopSim();
 
   DemoFr->SimMode    = SimFaultCmd->Mode; 
   DemoFr->SimEnabled = TRUE;
   
   if (DemoFr->SimMode == DEMOFR_MODE_TIMED_FAULT) {

      /* AddTimedFault() sends events for all situations */      
      if (!AddTimedFault(SimFaultCmd)) {
         DemoFr->SimMode    = DEMOFR_MODE_IDLE;
         DemoFr->SimEnabled = FALSE;
      } 
      
   } /* End if DEMOFR_MODE_TIMED_FAULT */
   else if (DemoFr->SimMode == DEMOFR_MODE_WALKING_FAULT) {
   
      /* Id is already set to 0 */
      DemoFr->SimWalkFault.CurStep  = SimFaultCmd->Steps;
      DemoFr->SimWalkFault.CmdSteps = SimFaultCmd->Steps;
         
      CFE_EVS_SendEvent(DEMOFR_START_WALKING_FAULT_EID, CFE_EVS_INFORMATION, 
                        "Started walking fault sim at Id %d with %d sim steps per Id", 
                        DemoFr->SimWalkFault.Id, SimFaultCmd->Steps);
   
   } /* End if DEMOFR_MODE_WALKING_FAULT */   
   
} /* End StartSim() */


/******************************************************************************
** Function: StopSim
**
** Stop a sim and clear all state information.
**
** Notes:
**   None
*/
static void StopSim(void)
{

   CFE_EVS_SendEvent(DEMOFR_STOP_SIM_FAULT_EID, CFE_EVS_INFORMATION, 
                     "Stopped %s sim", SimModeStr(DemoFr->SimMode));
                     
   DemoFr->SimMode    = DEMOFR_MODE_IDLE; 
   DemoFr->SimEnabled = FALSE;

   /* Clear all state info for each mode */
   CFE_PSP_MemSet((void*)DemoFr->SimFault,      0, sizeof(DemoFr->SimFault[OSK_DEMO_FAULT_ID_MAX]));
   CFE_PSP_MemSet((void*)&DemoFr->SimWalkFault, 0, sizeof(DEMOFR_SimWalkFault)); 
   
} /* End StopSim() */


/******************************************************************************
** Function: WalkFault
**
*/
static boolean WalkFault(void) 
{
   
   boolean RetStatus = TRUE;
   
   FaultRep_FaultDetFailed(DemoFr->FaultRep, DemoFr->SimWalkFault.Id);

   DemoFr->SimWalkFault.CurStep--;
   
   if (DemoFr->SimWalkFault.CurStep == 0) {
   
      DemoFr->SimWalkFault.Id++;
   
      if (DemoFr->SimWalkFault.Id >= OSK_DEMO_FAULT_ID_MAX) {
         RetStatus = FALSE;
         CFE_EVS_SendEvent(DEMOFR_WALK_FAULT_COMPLETE_EID, CFE_EVS_INFORMATION, 
                           "Completed walking fault sim after Id %d", 
                           (DemoFr->SimWalkFault.Id-1));

      }
      else
         DemoFr->SimWalkFault.CurStep = DemoFr->SimWalkFault.CmdSteps;
   
   } /* End if CurStep 0 */
   
   return RetStatus;
   
} /* End WalkFault() */


/* end of file */
