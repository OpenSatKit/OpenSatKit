/*
** Purpose: Implement the payload management object 
**
** Notes:
**   1. Information events are used in order to trace execution for
**      demonstrations.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
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
#include "payload.h"


/**********************/
/** Global File Data **/
/**********************/

static PAYLOAD_Class_t *Payload = NULL;


/*******************************/
/** Local Function Prototypes **/
/*******************************/


/******************************************************************************
** Function: PAYLOAD_Constructor
**
*/ 
void PAYLOAD_Constructor(PAYLOAD_Class_t *PayloadPtr, INITBL_Class *IniTbl)
{
 
   Payload = PayloadPtr;

   CFE_PSP_MemSet((void*)Payload, 0, sizeof(PAYLOAD_Class_t));
   
   Payload->CurrPower = PL_SIM_LIB_POWER_OFF;
   Payload->PrevPower = PL_SIM_LIB_POWER_OFF;
   
   SCI_FILE_Constructor(&Payload->SciFile, IniTbl);
         
} /* End PAYLOAD_Constructor() */


/******************************************************************************
** Function:  PAYLOAD_ResetStatus
**
*/
void PAYLOAD_ResetStatus(void)
{

   /* All state data managed by the science start and stop commands */
   
} /* End PAYLOAD_ResetStatus() */

/******************************************************************************
** Functions: PAYLOAD_StartSciCmd
**
** Start collecting and saving payload detector data to a file.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean PAYLOAD_StartSciCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   boolean RetStatus = FALSE;
   
   if (Payload->CurrPower == PL_SIM_LIB_POWER_READY)
   {
      
      if (SCI_FILE_Start() == TRUE)
      {

         CFE_EVS_SendEvent (PAYLOAD_START_SCI_CMD_EID, CFE_EVS_INFORMATION, 
                            "Start science data collection accepted");
      
         RetStatus = TRUE;
      }
   }  
   else
   { 
   
      CFE_EVS_SendEvent (PAYLOAD_START_SCI_CMD_ERR_EID, CFE_EVS_ERROR, 
                         "Start science data collection rejected. Payload in %s state and not the READY power state",
                        PL_SIM_LIB_GetPowerStateStr(Payload->CurrPower));
   
   }
   
   return RetStatus;

} /* End PAYLOAD_StartSciCmd() */



/******************************************************************************
** Functions: PAYLOAD_StopSciCmd
**
** Stop collecting and saving payload detector data to a file.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean PAYLOAD_StopSciCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   char EventStr[132];
   
   SCI_FILE_Stop(EventStr, 132);
   
   CFE_EVS_SendEvent (PAYLOAD_STOP_SCI_CMD_EID, CFE_EVS_INFORMATION, 
                      "%s", EventStr);
                            
   return TRUE;

} /* End PAYLOAD_StopSciCmd() */


/******************************************************************************
** Functions: PAYLOAD_ManageData
**
** Reading detector data manage science files
**
** Notes:
**   1. This function is called every PL_MGR 'execution cycle' regardless of
**      the power and payload state.
**
*/
void PAYLOAD_ManageData(void)
{

   Payload->CurrPower = PL_SIM_LIB_ReadPowerState();
   
   if (Payload->CurrPower == PL_SIM_LIB_POWER_READY)
   {   
      PL_SIM_LIB_ReadDetector(&Payload->Detector);
      
      /* Simulation puts a character in first byte when a fault is present */
      Payload->DetectorFault = isalpha(Payload->Detector.Row.Data[0]);
         
      if (Payload->Detector.ReadoutRow == 0)
         SCI_FILE_WriteDetectorData(&Payload->Detector, SCI_FILE_FIRST_ROW);
      else if (Payload->Detector.ReadoutRow >= (PL_SIM_LIB_DETECTOR_ROWS_PER_IMAGE-1))
         SCI_FILE_WriteDetectorData(&Payload->Detector, SCI_FILE_LAST_ROW);
      else
         SCI_FILE_WriteDetectorData(&Payload->Detector, SCI_FILE_ROW);
   }
   else
   {
      /* Check whether transitioned from READY to non-READY state */
      if (Payload->PrevPower == PL_SIM_LIB_POWER_READY)
      {
         if (Payload->SciFile.State == SCI_FILE_ENABLED)
         {
            CFE_EVS_SendEvent(PAYLOAD_SHUTDOWN_SCI_EID, CFE_EVS_ERROR, 
                              "Terminating science data collection. Payload power transitioned from %s to %s",
                              PL_SIM_LIB_GetPowerStateStr(Payload->PrevPower),
                              PL_SIM_LIB_GetPowerStateStr(Payload->CurrPower));
            SCI_FILE_WriteDetectorData(&Payload->Detector, SCI_FILE_SHUTDOWN);
         }
      }
   }
   
   Payload->PrevPower = Payload->CurrPower;

} /* End PAYLOAD_ManageData() */



