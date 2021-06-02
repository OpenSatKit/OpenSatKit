/*
** Purpose: Implement the object to demo StateRep module.
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
#include "protofr.h"

/*
** Global File Data
*/

static PROTOFR_Class*  ProtoFr = NULL;

/*
** Local Function Prototypes
*/

static boolean AddTimedFault(const PROTOFR_SimFaultCmdMsg *SimFaultCmd);
static void ReportTimedSimFaults(void);
const char* SimModeStr(PROTOFR_SimMode  SimMode);
static void StartSim(const PROTOFR_SimFaultCmdMsg *SimFaultCmd);
static void StopSim(void);
static boolean WalkFault(void);


/******************************************************************************
** Function: PROTOFR_Constructor
**
** Notes:
**   1. 
*/
void PROTOFR_Constructor(PROTOFR_Class*   ProtoFrPtr,
                         STATEREP_Class* StateRepPtr)
{
 
   ProtoFr = ProtoFrPtr;

   CFE_PSP_MemSet((void*)ProtoFr, 0, sizeof(PROTOFR_Class));

   ProtoFr->StateRep = StateRepPtr; 
    
} /* End PROTOFR_Constructor() */


/******************************************************************************
** Function:  PROTOFR_ResetStatus
**
*/
void PROTOFR_ResetStatus()
{

   /* Maintain simulation state */

} /* End PROTOFR_ResetStatus() */


/******************************************************************************
** Function: PROTOFR_SimStep
**
** Execute a single simulation step.
**
** Notes:
**   1. The SimFault command determines this function's behavior.
*/
void PROTOFR_SimStep(void)
{
   
   if (ProtoFr->SimEnabled) {
   
      switch (ProtoFr->SimMode) {
         
         case PROTOFR_MODE_IDLE:
            break;
            
         case PROTOFR_MODE_TIMED_FAULT:
            ReportTimedSimFaults();
            break;
         
         case PROTOFR_MODE_WALKING_FAULT:
            ProtoFr->SimEnabled = WalkFault();
            break;
            
         default:
            CFE_EVS_SendEvent(PROTOFR_INVLD_SIM_STEP_MODE_EID, CFE_EVS_ERROR, 
                              "Invalid simulation mode %d. Disabling simulation.", ProtoFr->SimMode);
            ProtoFr->SimEnabled = FALSE;
            break;

      } /* End SimMode switch */

      if (!ProtoFr->SimEnabled)
         ProtoFr->SimMode = PROTOFR_MODE_IDLE;
      
   
   } /* End if EnableSim */
   
   
} /* End PROTOFR_SimStep() */


/******************************************************************************
** Function: PROTOFR_SetTlmModeCmd
**
** Set the telemetry reporting mode .
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean PROTOFR_SetTlmModeCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const PROTOFR_SetTlmModeCmdMsg *SetTlmModeCmd = (const PROTOFR_SetTlmModeCmdMsg *) MsgPtr;
   boolean  RetStatus = TRUE;

   if ( SetTlmModeCmd->Mode == STATEREP_NEW_REPORT ||
        SetTlmModeCmd->Mode == STATEREP_MERGE_REPORT) {
   
      CFE_EVS_SendEvent(PROTOFR_CMD_SET_TLM_MODE_EID, CFE_EVS_INFORMATION, 
                        "State Report telemetry mode changed from %s to %s", 
                        STATEREP_TlmModeStr(ProtoFr->StateRep->TlmMode),
                        STATEREP_TlmModeStr(SetTlmModeCmd->Mode) );
                        
      STATEREP_SetTlmMode(ProtoFr->StateRep, (STATEREP_TlmMode)SetTlmModeCmd->Mode);
   
   }
   else {
      
      CFE_EVS_SendEvent(PROTOFR_CMD_INVLD_SET_TLM_MODE_EID, CFE_EVS_ERROR, 
                        "Command contains invalid telemetry mode %d", SetTlmModeCmd->Mode);
      RetStatus = FALSE;
   }
   
   return RetStatus;
   
} /* End PROTOFR_SetTlmModeCmd() */


/******************************************************************************
** Function: PROTOFR_SimFaultCmd
**
** Configure the fault simulation. 
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. Helper functions send event messages.
*/
boolean PROTOFR_SimFaultCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const PROTOFR_SimFaultCmdMsg *SimFaultCmd = (const PROTOFR_SimFaultCmdMsg *) MsgPtr;
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
      
      if (ProtoFr->SimEnabled) {

         switch (SimFaultCmd->Mode) {
         case PROTOFR_MODE_WALKING_FAULT:
            
            StartSim(SimFaultCmd);
            break;
           
         case PROTOFR_MODE_TIMED_FAULT:
      
            if (ProtoFr->SimMode == PROTOFR_MODE_TIMED_FAULT) {

              RetStatus = AddTimedFault(SimFaultCmd);
            
            }
            else {
               StartSim(SimFaultCmd);
            }
            break;
            
         default:

            CFE_EVS_SendEvent(PROTOFR_CMD_INVLD_SIM_FAULT_MODE_EID, CFE_EVS_ERROR, 
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

} /* End PROTOFR_SimFaultCmd() */


/******************************************************************************
** Function: AddTimedFault
**
*/
static boolean AddTimedFault(const PROTOFR_SimFaultCmdMsg *SimFaultCmd)
{
   
   int RetStatus = TRUE;
   
   if (SimFaultCmd->Id >= OSK_C_PROTO_STATEREP_BIT_ID_MAX) {
   
      CFE_EVS_SendEvent(PROTOFR_CMD_INVLD_TIMED_FAULT_ID_EID, CFE_EVS_ERROR, 
                        "Command contains invalid id %d. Must be less than %d",
                        SimFaultCmd->Id, OSK_C_PROTO_STATEREP_BIT_ID_MAX);
      RetStatus = FALSE;
   
   } /* End invalid ID */
   else {

      ProtoFr->SimFault[SimFaultCmd->Id].Step    = SimFaultCmd->Steps;
      ProtoFr->SimFault[SimFaultCmd->Id].Enabled = TRUE;
      
      CFE_EVS_SendEvent(PROTOFR_CMD_ADD_TIMED_FAULT_EID, CFE_EVS_INFORMATION, 
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
   
   for (i=0; i < OSK_C_PROTO_STATEREP_BIT_ID_MAX; i++) {
   
      if (ProtoFr->SimFault[i].Enabled) {
      
         STATEREP_SetBit(ProtoFr->StateRep, i);
         DetFailed = TRUE;
         ProtoFr->SimFault[i].Step--;
         ProtoFr->SimFault[i].Enabled = (ProtoFr->SimFault[i].Step > 0);
      
      } /* End if enabled */
         
   } /* End ID loop */
   
   if (!DetFailed)
      StopSim();
   
} /* End ReportTimedSimFaults() */


/******************************************************************************
** Function: SimModeStr
**
*/
const char* SimModeStr(PROTOFR_SimMode  SimMode)
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
** Start a new sim clearing any existing state information.
**
** Notes:
**   1. Can assume the commanded SimMode has been validated.
*/
static void StartSim(const PROTOFR_SimFaultCmdMsg *SimFaultCmd)
{

   StopSim();
 
   ProtoFr->SimMode    = SimFaultCmd->Mode; 
   ProtoFr->SimEnabled = TRUE;
   
   if (ProtoFr->SimMode == PROTOFR_MODE_TIMED_FAULT) {

      /* AddTimedFault() sends events for all situations */      
      if (!AddTimedFault(SimFaultCmd)) {
         ProtoFr->SimMode    = PROTOFR_MODE_IDLE;
         ProtoFr->SimEnabled = FALSE;
      } 
      
   } /* End if PROTOFR_MODE_TIMED_FAULT */
   else if (ProtoFr->SimMode == PROTOFR_MODE_WALKING_FAULT) {
   
      /* Id is already set to 0 */
      ProtoFr->SimWalkFault.CurStep  = SimFaultCmd->Steps;
      ProtoFr->SimWalkFault.CmdSteps = SimFaultCmd->Steps;
         
      CFE_EVS_SendEvent(PROTOFR_START_WALKING_FAULT_EID, CFE_EVS_INFORMATION, 
                        "Started walking fault sim at Id %d with %d sim steps per Id", 
                        ProtoFr->SimWalkFault.Id, SimFaultCmd->Steps);
   
   } /* End if PROTOFR_MODE_WALKING_FAULT */   
   
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

   CFE_EVS_SendEvent(PROTOFR_STOP_SIM_FAULT_EID, CFE_EVS_INFORMATION, 
                     "Stopped %s sim", SimModeStr(ProtoFr->SimMode));
                     
   ProtoFr->SimMode    = PROTOFR_MODE_IDLE; 
   ProtoFr->SimEnabled = FALSE;

   /* Clear all state info for each mode */
   CFE_PSP_MemSet((void*)ProtoFr->SimFault,      0, sizeof(ProtoFr->SimFault[OSK_C_PROTO_STATEREP_BIT_ID_MAX]));
   CFE_PSP_MemSet((void*)&ProtoFr->SimWalkFault, 0, sizeof(PROTOFR_SimWalkFault)); 
   
} /* End StopSim() */


/******************************************************************************
** Function: WalkFault
**
*/
static boolean WalkFault(void) 
{
   
   boolean RetStatus = TRUE;
   
   STATEREP_SetBit(ProtoFr->StateRep, ProtoFr->SimWalkFault.Id);

   ProtoFr->SimWalkFault.CurStep--;
   
   if (ProtoFr->SimWalkFault.CurStep == 0) {
   
      ProtoFr->SimWalkFault.Id++;
   
      if (ProtoFr->SimWalkFault.Id >= OSK_C_PROTO_STATEREP_BIT_ID_MAX) {
         RetStatus = FALSE;
         CFE_EVS_SendEvent(PROTOFR_WALK_FAULT_COMPLETE_EID, CFE_EVS_INFORMATION, 
                           "Completed walking fault sim after Id %d", 
                           (ProtoFr->SimWalkFault.Id-1));

      }
      else
         ProtoFr->SimWalkFault.CurStep = ProtoFr->SimWalkFault.CmdSteps;
   
   } /* End if CurStep 0 */
   
   return RetStatus;
   
} /* End WalkFault() */

