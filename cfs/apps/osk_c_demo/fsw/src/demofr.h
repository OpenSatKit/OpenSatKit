/*
** Purpose: Demonstrate the osk_c_fw FaultRep service
**
** Notes:
**   1. This object is for demonstrative purposes. It 
**      provides ground commands that can be used to simulate 
**      fault detection reports.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _demofr_
#define _demofr_

/*
** Includes
*/

#include "app_cfg.h"


/*
** Event Message IDs
*/

#define DEMOFR_INVLD_SIM_STEP_MODE_EID       (DEMOFR_BASE_EID + 0)
#define DEMOFR_CMD_SET_TLM_MODE_EID          (DEMOFR_BASE_EID + 1)
#define DEMOFR_CMD_INVLD_SET_TLM_MODE_EID    (DEMOFR_BASE_EID + 2)
#define DEMOFR_CMD_INVLD_SIM_FAULT_MODE_EID  (DEMOFR_BASE_EID + 3)
#define DEMOFR_CMD_INVLD_TIMED_FAULT_ID_EID  (DEMOFR_BASE_EID + 4)
#define DEMOFR_CMD_ADD_TIMED_FAULT_EID       (DEMOFR_BASE_EID + 5)
#define DEMOFR_WALK_FAULT_COMPLETE_EID       (DEMOFR_BASE_EID + 6)
#define DEMOFR_START_WALKING_FAULT_EID       (DEMOFR_BASE_EID + 7)
#define DEMOFR_STOP_SIM_FAULT_EID            (DEMOFR_BASE_EID + 8)


/*
** Type Definitions
*/

typedef enum
{

   DEMOFR_MODE_IDLE          = 0,  /* Set a single fault */
   DEMOFR_MODE_TIMED_FAULT   = 1,  /* Set a single fault */
   DEMOFR_MODE_WALKING_FAULT = 2   /* Cycle through all of the fault IDs */

} DEMOFR_SimMode;


/******************************************************************************
** DemoFr_Class
*/

typedef struct {

   boolean  Enabled;
   uint16   Step;

} DEMOFR_SimFault;

typedef struct {

   uint16   Id;
   uint16   CurStep;
   uint16   CmdSteps;

} DEMOFR_SimWalkFault;

typedef struct {

   boolean              SimEnabled;
   DEMOFR_SimMode       SimMode;
   DEMOFR_SimFault      SimFault[FAULTREP_ID_MAX];
   DEMOFR_SimWalkFault  SimWalkFault;
   
   FaultRep_Class* FaultRep;

} DEMOFR_Class;


/******************************************************************************
** Command Functions
*/

typedef struct {

   uint8     CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint8     Mode;                           /* See FaultRep_TlmMode */

} DEMOFR_SetTlmModeCmdParam;
#define DEMOFR_SET_TLM_MODE_CMD_DATA_LEN  (sizeof(DEMOFR_SetTlmModeCmdParam) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

   uint8     CmdHeader[CFE_SB_CMD_HDR_SIZE];
   boolean   Enable;
   uint8     Mode;            /* See DemoFr_SimMode */
   uint16    Id;              /* Ignored for walking sim */
   uint16    Steps;           /* Number of sim steps to sim a fault */

} DEMOFR_SimFaultCmdParam;
#define DEMOFR_SIM_FAULT_CMD_DATA_LEN  (sizeof(DEMOFR_SimFaultCmdParam) - CFE_SB_CMD_HDR_SIZE)


/*
** Exported Functions
*/

/******************************************************************************
** Function: DEMOFR_Constructor
**
** Initialize the object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**   2. DEMOFR has an reference relationship with FaultRep not an ownership
**      relationship so DEMOFR is not responsible for constructing FaultRep. It
**      simply stores a pointer reference.
**
*/
void DEMOFR_Constructor(DEMOFR_Class *DemoFrPtr, FaultRep_Class* FaultRepPtr);


/******************************************************************************
** Function: DEMOFR_SimStep
**
** Execute a single simulation step.
**
** Notes:
**   1. The SimFault command determines this functions behavior.
*/
void DEMOFR_SimStep(void);


/******************************************************************************
** Function: DEMOFR_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void DEMOFR_ResetStatus(void);


/******************************************************************************
** Function: DEMOFR_SetTlmModeCmd
**
** Set the telemetry reporting mode .
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean DEMOFR_SetTlmModeCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: DEMOFR_SimFaultCmd
**
** Configure the fault simulation. 
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. Multiple individual timed faults can be active at once. The walking
**     fault sim can't be combined with other sim modes. 
*/
boolean DEMOFR_SimFaultCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _demofr_ */
