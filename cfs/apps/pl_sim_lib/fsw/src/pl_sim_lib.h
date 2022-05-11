/*
** Purpose: Define the Payload Simulator Object
**
** Notes:
**   1. PL_SIM serves as an example of how a library interface
**      to a hardware device can be replaced with simulated
**      behavior. The functions perform the following roles:
**      A. Model instrinsic payload behavior
**         - PL_SIM_LIB_Constructor()
**         - PL_SIM_LIB_ExecuteStep()
**         - PL_SIM_LIB_SetFault()
**         - PL_SIM_LIB_ClearFault()
**      B. Model payload Electrical Interface
**         - PL_SIM_LIB_PowerOn()
**         - PL_SIM_LIB_PowerOff()
**         - PL_SIM_LIB_PowerReset()
**      C. Model payload data interface
**         - PL_SIM_LIB_ReadState()
**   2. This header can't include lib_cfg.h because it would
**      cause an initbl CFG definition conflict with apps that
**      include this header. The initbl design assumes the
**      the CFG_ definitions are unique to an app or a lib.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _pl_sim_lib_
#define _pl_sim_lib_

/*
** Includes
*/


/***********************/
/** Macro Definitions **/
/***********************/


#define PL_SIM_LIB_DETECTOR_ROW_LEN        32
#define PL_SIM_LIB_DETECTOR_ROWS_PER_IMAGE 10

/*
** Event Message IDs
*/

#define PL_SIM_LIB_PWR_INIT_COMPLETE_EID   (OSK_C_FW_LIB_BASE_EID + 0)
#define PL_SIM_LIB_PWR_RESET_COMPLETE_EID  (OSK_C_FW_LIB_BASE_EID + 1)
#define PL_SIM_LIB_PWR_INVALID_STATE_EID   (OSK_C_FW_LIB_BASE_EID + 2)
#define PL_SIM_LIB_PWR_TRANSITION_EID      (OSK_C_FW_LIB_BASE_EID + 3)
#define PL_SIM_LIB_DETECTOR_ERR_EID        (OSK_C_FW_LIB_BASE_EID + 4)

/**********************/
/** Type Definitions **/
/**********************/

typedef enum 
{
   
   PL_SIM_LIB_POWER_OFF    = 1,
   PL_SIM_LIB_POWER_INIT   = 2,
   PL_SIM_LIB_POWER_RESET  = 3,
   PL_SIM_LIB_POWER_READY  = 4
   
} PL_SIM_LIB_Power_t;


typedef struct
{
   
   char Data[PL_SIM_LIB_DETECTOR_ROW_LEN];
   
} PL_SIM_LIB_DetectorRow_t;

typedef struct
{
   
   uint16  ReadoutRow;   
   uint16  ImageCnt;
   PL_SIM_LIB_DetectorRow_t Row;

} PL_SIM_LIB_Detector_t;


typedef struct
{

   uint16   PowerInitCycleLim;
   uint16   PowerResetCycleLim;

} PL_SIM_LIB_Config_t;

typedef struct
{

   PL_SIM_LIB_Power_t Power;
   uint16   PowerInitCycleCnt;
   uint16   PowerResetCycleCnt;
   uint16   DetectorReadoutRow;
   boolean  DetectorFaultPresent;

} PL_SIM_LIB_State_t;

typedef struct
{

   /* 
   ** App Framework
   */ 
   
   INITBL_Class  IniTbl;
   
   /* 
   ** PL_SIM State
   */ 

   PL_SIM_LIB_Detector_t Detector;  
   PL_SIM_LIB_State_t    State;
   PL_SIM_LIB_Config_t   Config;
   
} PL_SIM_LIB_Class_t;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: PL_SIM_LIB_Constructor
**
** Initialize the payload simulator to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
boolean PL_SIM_LIB_Constructor(PL_SIM_LIB_Class_t *PlSimPtr);


/******************************************************************************
** Function: PL_SIM_LIB_ExecuteStep
**
** Execute a single simulation step.
**
** Notes:
**   None
*/
void PL_SIM_LIB_ExecuteStep(void);


/******************************************************************************
** Functions: PL_SIM_LIB_SetFault
**
** Set/clear fault state.
**
** Notes:
**   1. No return status required, the simulated fault is always set.
**
*/
void PL_SIM_LIB_SetFault(void);


/******************************************************************************
** Functions: PL_SIM_LIB_ClearFault
**
** Clear fault state.
**
** Notes:
**   1. No return status required, the simulated fault is always cleared.
**
*/
void PL_SIM_LIB_ClearFault(void);


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
void PL_SIM_LIB_PowerOn(void);


/******************************************************************************
** Functions: PL_SIM_LIB_PowerOff
**
** Power off the simulated payload
**
** Notes:
**   1. No return status required, power is always set to false.
**
*/
void PL_SIM_LIB_PowerOff(void);


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
void PL_SIM_LIB_PowerReset(void);


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
PL_SIM_LIB_Power_t PL_SIM_LIB_ReadPowerState(void);


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
void PL_SIM_LIB_ReadDetector(PL_SIM_LIB_Detector_t *Detector);


/******************************************************************************
** Functions: PL_SIM_LIB_ReadLibraryState
**
** Read the state of the library simulation
**
** Notes:
**  1. In a non-simulated environment this would not exist.
**
*/
void PL_SIM_LIB_ReadState(PL_SIM_LIB_Class_t *PlSimObj);


/******************************************************************************
** Functions: PL_SIM_LIB_GetPowerStateStr
**
** Read the state of the library simulation
**
** Notes:
**  1. In a non-simulated environment this would not exist.
**
*/
const char* PL_SIM_LIB_GetPowerStateStr(PL_SIM_LIB_Power_t Power);


#endif /* _pl_sim_lib_ */
