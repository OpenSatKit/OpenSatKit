/*
** Purpose: Implement the Payload Simulator Library
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

#include "lib_cfg.h"
#include "pl_sim_lib.h"

/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ    (&(PlSimLib->IniTbl))

/**********************/
/** Type Definitions **/
/**********************/

typedef enum
{

   DETECTOR_READOUT_FALSE = 1,
   DETECTOR_READOUT_TRUE  = 2,
   DETECTOR_READOUT_LAST  = 3   /* Last readout in an image */

} DetectorReadout_Enum_t;

typedef struct
{
   boolean        Received;
   PL_SIM_LIB_Power_t NewState;
     
} PowerConfigCmd_t;

/**********************/
/** File Global Data **/
/**********************/

/* 
** Must match DECLARE ENUM() declaration in lib_cfg.h
** Defines "static INILIB_CfgEnum IniCfgEnum"
*/

DEFINE_ENUM(Config, LIB_CONFIG)  

static PL_SIM_LIB_Class_t *PlSimLib = NULL;
static PowerConfigCmd_t   PowerConfigCmd;

/* 
** String lookup tables for Payload power state
*/
static const char* PwrStateStr[] = 
{
   
   "UNDEFINED",     /* 0: Invalid state          */ 
   "OFF",           /* 1: PL_SIM_LIB_POWER_OFF   */
   "INITIALIZING",  /* 2: PL_SIM_LIB_POWER_INIT  */
   "RESETTING",     /* 3: PL_SIM_LIB_POWER_RESET */
   "READY"          /* 4: PL_SIM_LIB_POWER_READY */

};

/* 
** Detector rows are strings to keep things simple for demonstration
** purposes. Science files are text.
*/
static const PL_SIM_LIB_DetectorRow_t DetectorRowImage[PL_SIM_LIB_DETECTOR_ROWS_PER_IMAGE] = 
{
   { "00010203040506070809\n" }, 
   { "10111213141516171819\n" }, 
   { "20212223242526272829\n" },
   { "30313233343536373839\n" }, 
   { "40414243444546474849\n" },
   { "50515253545556575859\n" },
   { "60616263646566676869\n" },
   { "70717273747576777879\n" },
   { "80818283848586878889\n" },
   { "90919293949596979899\n" }
};

static const char DetectorRowInitState[PL_SIM_LIB_DETECTOR_ROW_LEN] = 
   { ',','I','n','i','t','i','a','l',' ','S','t','a','t','e','\n' };

/*
** Only one character is substituted per sample when a fault is present
** An image containing a character in the first byte is how a fault is
** detected by the payload manager app.  
*/ 
static const char FaultCorruptedChar[PL_SIM_LIB_DETECTOR_ROWS_PER_IMAGE] = 
   { 70, 65, 85, 76, 84, 65, 76, 69, 82, 84 };


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static void Detector_Init(boolean ClearImageCnt);
static DetectorReadout_Enum_t Detector_Readout(void);


/******************************************************************************
** Function: PL_SIM_LIB_Constructor
**
*/
boolean PL_SIM_LIB_Constructor(PL_SIM_LIB_Class_t* PlSimLibPtr)
{
 
   boolean RetStatus = FALSE;
 
   PlSimLib = PlSimLibPtr;

   CFE_PSP_MemSet((void*)PlSimLib, 0, sizeof(PL_SIM_LIB_Class_t));
   CFE_PSP_MemSet((void*)&PowerConfigCmd, 0, sizeof(PowerConfigCmd_t));
   
   PlSimLib->State.Power = PL_SIM_LIB_POWER_OFF;
   
   Detector_Init(TRUE);

   /* IniTbl will report errors */
   if (INITBL_Constructor(INITBL_OBJ, PL_SIM_LIB_INI_FILENAME, &IniCfgEnum))
   {
      PlSimLib->Config.PowerInitCycleLim  = INITBL_GetIntConfig(INITBL_OBJ, CFG_PWR_INIT_CYCLES);
      PlSimLib->Config.PowerResetCycleLim = INITBL_GetIntConfig(INITBL_OBJ, CFG_PWR_RESET_CYCLES);
      RetStatus = TRUE;
   }

   return RetStatus;
   
} /* End PL_SIM_LIB_Constructor() */


/******************************************************************************
** Function: PL_SIM_LIB_ExecuteStep
**
** Execute instrument simulation cycle. Command functions can set state flags
** but the resulting behavior will be implemented in this method. 
**
** Notes:
**   1. PL_SIM_LIB_POWER_READY can't be commanded so errors checks dont need
**      to be performed. The new power state is simply set to the commanded
**      value.
**
*/
void PL_SIM_LIB_ExecuteStep(void)
{
      
   if (PowerConfigCmd.Received)
   {
      
      CFE_EVS_SendEvent (PL_SIM_LIB_PWR_TRANSITION_EID, CFE_EVS_INFORMATION,
                         "PL_SIM power transitioned from %s to %s",
                         PL_SIM_LIB_GetPowerStateStr(PlSimLib->State.Power),
                         PL_SIM_LIB_GetPowerStateStr(PowerConfigCmd.NewState));
      
      PlSimLib->State.Power = PowerConfigCmd.NewState;
      PlSimLib->State.PowerInitCycleCnt  = 0;
      PlSimLib->State.PowerResetCycleCnt = 0;
      if (PlSimLib->State.Power == PL_SIM_LIB_POWER_RESET)
      {
         Detector_Init(FALSE);
      }
      else
      {
         Detector_Init(TRUE);
      }
      
      CFE_PSP_MemSet((void*)&PowerConfigCmd, 0, sizeof(PowerConfigCmd_t));
   }
   
   switch (PlSimLib->State.Power) 
   {
   
      case PL_SIM_LIB_POWER_OFF:
         break;
   
      case PL_SIM_LIB_POWER_INIT:
      
         if (++PlSimLib->State.PowerInitCycleCnt >= PlSimLib->Config.PowerInitCycleLim) 
         {
            
            PlSimLib->State.Power = PL_SIM_LIB_POWER_READY;
            
            CFE_EVS_SendEvent (PL_SIM_LIB_PWR_INIT_COMPLETE_EID, CFE_EVS_INFORMATION,
                               "PL_SIM completed initialization after power on in %d cycles.",
                               PlSimLib->State.PowerInitCycleCnt);
            
            PlSimLib->State.PowerInitCycleCnt  = 0;
         
         } /* End if init cycle complete */
         break;
         
         
      case PL_SIM_LIB_POWER_RESET:
                 
         if (++PlSimLib->State.PowerResetCycleCnt >= PlSimLib->Config.PowerResetCycleLim) 
         {
            
            CFE_EVS_SendEvent (PL_SIM_LIB_PWR_RESET_COMPLETE_EID, CFE_EVS_INFORMATION,
                               "PL_SIM completed initialization after reset in %d cycles.",
                               PlSimLib->State.PowerResetCycleCnt);

            PlSimLib->State.Power = PL_SIM_LIB_POWER_READY;
            PlSimLib->State.PowerResetCycleCnt  = 0;
            PlSimLib->State.DetectorFaultPresent = FALSE;
         
         } /* End if init cycle complete */

      case PL_SIM_LIB_POWER_READY:
         break;
                  
      default:
         
         CFE_EVS_SendEvent (PL_SIM_LIB_PWR_INVALID_STATE_EID, CFE_EVS_CRITICAL,
                            "Invalid PL_SIM power state %d. Powering off instrument.",
                            PlSimLib->State.Power);
         
         PlSimLib->State.Power = PL_SIM_LIB_POWER_OFF;
         PlSimLib->State.PowerInitCycleCnt  = 0;
         PlSimLib->State.PowerResetCycleCnt = 0;
         Detector_Init(TRUE);

   } /* End power state switch */
      
   
} /* PL_SIM_LIB_ExecuteStep() */


/******************************************************************************
** Functions: PL_SIM_LIB_SetFault
**
** Set fault state to true
**
** Notes:
**   1. No return status required, the simulated fault is always set.
**   2. Fault is set immediately unlike the power configurations that are
**      processed during the ExecuteStep() function
**
*/
void PL_SIM_LIB_SetFault(void)
{
   
   PlSimLib->State.DetectorFaultPresent = TRUE;
   
} /* End PL_SIM_LIB_SetFault() */


/******************************************************************************
** Functions: PL_SIM_LIB_ClearFault
**
** Clear fault state.
**
** Notes:
**   1. No return status required, the simulated fault is always cleared.
**   2. Fault is set immediately unlike the power configurations that are
**      processed during the ExecuteStep() function
**
*/
void PL_SIM_LIB_ClearFault(void)
{

   PlSimLib->State.DetectorFaultPresent = FALSE;
   
} /* End PL_SIM_LIB_ClearFault() */



/******************************************************************************
** Functions: PL_SIM_LIB_PowerOn
**
** Power on the simulated payload to a known default state
**
** Notes:
**   1. The JSON init file defines configurable default state parameters
**   2. No return status required, power is always set to true.
**
*/
void PL_SIM_LIB_PowerOn(void)
{

   PowerConfigCmd.Received = TRUE;
   PowerConfigCmd.NewState = PL_SIM_LIB_POWER_INIT;
 
} /* End PL_SIM_LIB_PowerOn() */


/******************************************************************************
** Functions: PL_SIM_LIB_PowerOff
**
** Power off the simulated payload
**
** Notes:
**   1. No return status required, power is always set to false.
**
*/
void PL_SIM_LIB_PowerOff(void)
{

   PowerConfigCmd.Received = TRUE;
   PowerConfigCmd.NewState = PL_SIM_LIB_POWER_OFF;
   
} /* End PL_SIM_LIB_PowerOff() */


/******************************************************************************
** Functions: PL_SIM_LIB_PowerReset
**
** Reset power on the simulated payload
**
** Notes:
**   1. Reset allows some system state to persist across the reset. 
**   2. No return status required, power is always set to true.
**
*/
void PL_SIM_LIB_PowerReset(void)
{

   PowerConfigCmd.Received = TRUE;
   PowerConfigCmd.NewState = PL_SIM_LIB_POWER_RESET;
   
} /* End PL_SIM_LIB_PowerReset() */


/******************************************************************************
** Functions: PL_SIM_LIB_ReadPowerState
**
** Read the current power state of the payload
**
** Notes:
**  1. In a non-simulated environment this would be read across a hardware
**     interface.
**
*/
PL_SIM_LIB_Power_t PL_SIM_LIB_ReadPowerState(void)
{
   
   return PlSimLib->State.Power;
   
} /* End PL_SIM_LIB_ReadPowerState() */


/******************************************************************************
** Functions: PL_SIM_LIB_ReadDetector
**
** Read the detector data and state information
**
** Notes:
**  1. In a non-simulated environment this would be read across a hardware
**     interface.
**
*/
void PL_SIM_LIB_ReadDetector(PL_SIM_LIB_Detector_t *Detector)
{
   
   if (Detector_Readout() != DETECTOR_READOUT_FALSE)
   {
      memcpy(Detector, &PlSimLib->Detector, sizeof(PL_SIM_LIB_Detector_t));
   }
   
} /* End PL_SIM_LIB_ReadDetector() */


/******************************************************************************
** Functions: PL_SIM_LIB_ReadState
**
** Read the state of the library simulation
**
** Notes:
**  1. In a non-simulated environment this would not exist.
**
*/
void PL_SIM_LIB_ReadState(PL_SIM_LIB_Class_t *PlSimLibObj)
{

   memcpy(PlSimLibObj, PlSimLib, sizeof(PL_SIM_LIB_Class_t));
   
} /* End PL_SIM_LIB_ReadState() */


/******************************************************************************
** Functions: PL_SIM_LIB_GetPowerStateStr
**
** Read the state of the library simulation
**
** Notes:
**  1. In a non-simulated environment this would not exist.
**
*/
const char* PL_SIM_LIB_GetPowerStateStr(PL_SIM_LIB_Power_t PowerState)
{
   uint8 i = 0;
   
   if (PowerState <= PL_SIM_LIB_POWER_READY)
   {
      i = PowerState;
   }
   
   return PwrStateStr[i];
   
} /* PL_SIM_LIB_GetPowerStateStr() */


/******************************************************************************
** Functions: Detector_Init
**
** Initialize the detector to a know state.
**
** Notes:
**   None
*/
static void Detector_Init(boolean ClearImageCnt)
{

   PlSimLib->State.DetectorReadoutRow = 0;
   if (ClearImageCnt) PlSimLib->Detector.ImageCnt = 0;
   
   strncpy(PlSimLib->Detector.Row.Data, DetectorRowInitState, PL_SIM_LIB_DETECTOR_ROW_LEN);

} /* End Detector_Init() */


/******************************************************************************
** Functions: Detector_Readout
**
** Load the next instrument detector readout row.
**
** Return Values:
**   DETECTOR_READOUT_FALSE: Detector->Row.Data[] not loaded
**   DETECTOR_READOUT_TRUE:  Detector->Row.Data[] loaded & its not the last row in an image
**   DETECTOR_READOUT_LAST:  Detector->Row.Data[] loaded & it is the last row in an image
**
*/
static DetectorReadout_Enum_t Detector_Readout(void)
{

   DetectorReadout_Enum_t DetectorReadout = DETECTOR_READOUT_FALSE;
   
   if (PlSimLib->State.DetectorReadoutRow < PL_SIM_LIB_DETECTOR_ROWS_PER_IMAGE)
   {
   
      PlSimLib->Detector.ReadoutRow = PlSimLib->State.DetectorReadoutRow;
      strncpy (PlSimLib->Detector.Row.Data, DetectorRowImage[PlSimLib->Detector.ReadoutRow].Data, PL_SIM_LIB_DETECTOR_ROW_LEN);
      
      DetectorReadout = DETECTOR_READOUT_TRUE;

      /* Corrupt first column of data when a fault is present */
      if (PlSimLib->State.DetectorFaultPresent)
      {
         PlSimLib->Detector.Row.Data[0] = FaultCorruptedChar[PlSimLib->Detector.ReadoutRow];
      }
         
      PlSimLib->State.DetectorReadoutRow++;
      if (PlSimLib->State.DetectorReadoutRow >= PL_SIM_LIB_DETECTOR_ROWS_PER_IMAGE)
      {
      
         PlSimLib->State.DetectorReadoutRow = 0;
         PlSimLib->Detector.ImageCnt++;
         
         DetectorReadout = DETECTOR_READOUT_LAST;
      
      }
      
   } /* If valid Detector->ReadoutRow */
   else
   {
      
      CFE_EVS_SendEvent (PL_SIM_LIB_DETECTOR_ERR_EID, CFE_EVS_CRITICAL, 
                         "Invalid detector row %d index exceeds fixed maximum row index of %d.",
                         PlSimLib->State.DetectorReadoutRow, (PL_SIM_LIB_DETECTOR_ROWS_PER_IMAGE-1));

      PlSimLib->State.DetectorReadoutRow = 0;
   
   }
   
   return DetectorReadout;
   
} /* Detector_Readout() */


