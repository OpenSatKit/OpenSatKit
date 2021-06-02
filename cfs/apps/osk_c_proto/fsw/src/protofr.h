/*
** Purpose: Demonstrate the osk_c_fw StateRep service
**
** Notes:
**   1. This object is for demonstrative purposes. It 
**      provides ground commands that can be used to simulate 
**      atate reports.  The state reporter is configured to
**      report faults so variable names include 'fault' rather
**      than a generic name like state.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _protofr_
#define _protofr_

/*
** Includes
*/

#include "app_cfg.h"


/*
** Event Message IDs
*/

#define PROTOFR_INVLD_SIM_STEP_MODE_EID       (PROTOFR_BASE_EID + 0)
#define PROTOFR_CMD_SET_TLM_MODE_EID          (PROTOFR_BASE_EID + 1)
#define PROTOFR_CMD_INVLD_SET_TLM_MODE_EID    (PROTOFR_BASE_EID + 2)
#define PROTOFR_CMD_INVLD_SIM_FAULT_MODE_EID  (PROTOFR_BASE_EID + 3)
#define PROTOFR_CMD_INVLD_TIMED_FAULT_ID_EID  (PROTOFR_BASE_EID + 4)
#define PROTOFR_CMD_ADD_TIMED_FAULT_EID       (PROTOFR_BASE_EID + 5)
#define PROTOFR_WALK_FAULT_COMPLETE_EID       (PROTOFR_BASE_EID + 6)
#define PROTOFR_START_WALKING_FAULT_EID       (PROTOFR_BASE_EID + 7)
#define PROTOFR_STOP_SIM_FAULT_EID            (PROTOFR_BASE_EID + 8)


/*
** Type Definitions
*/

typedef enum
{

   PROTOFR_MODE_IDLE          = 0,  /* Set a single fault */
   PROTOFR_MODE_TIMED_FAULT   = 1,  /* Set a single fault */
   PROTOFR_MODE_WALKING_FAULT = 2   /* Cycle through all of the fault IDs */

} PROTOFR_SimMode;


/******************************************************************************
** ProtoFr_Class
*/

typedef struct {

   boolean  Enabled;
   uint16   Step;

} PROTOFR_SimFault;

typedef struct {

   uint16   Id;
   uint16   CurStep;
   uint16   CmdSteps;

} PROTOFR_SimWalkFault;

typedef struct {

   boolean               SimEnabled;
   PROTOFR_SimMode       SimMode;
   PROTOFR_SimFault      SimFault[STATEREP_BIT_ID_MAX];
   PROTOFR_SimWalkFault  SimWalkFault;
   
   STATEREP_Class*      StateRep;

} PROTOFR_Class;


/******************************************************************************
** Command Packets
*/

typedef struct {

   uint8     CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint8     Mode;                           /* See FaultRep_TlmMode */

} PROTOFR_SetTlmModeCmdMsg;
#define PROTOFR_SET_TLM_MODE_CMD_DATA_LEN  (sizeof(PROTOFR_SetTlmModeCmdMsg) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

   uint8     CmdHeader[CFE_SB_CMD_HDR_SIZE];
   boolean   Enable;
   uint8     Mode;            /* See ProtoFr_SimMode */
   uint16    Id;              /* Ignored for walking sim */
   uint16    Steps;           /* Number of sim steps to sim a fault */

} PROTOFR_SimFaultCmdMsg;
#define PROTOFR_SIM_FAULT_CMD_DATA_LEN  (sizeof(PROTOFR_SimFaultCmdMsg) - CFE_SB_CMD_HDR_SIZE)


/*
** Exported Functions
*/

/******************************************************************************
** Function: PROTOFR_Constructor
**
** Initialize the object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**   2. PROTOFR has an reference relationship with FaultRep not an ownership
**      relationship so PROTOFR is not responsible for constructing FaultRep. It
**      simply stores a pointer reference.
**
*/
void PROTOFR_Constructor(PROTOFR_Class *ProtoFrPtr, STATEREP_Class* StateRepPtr);


/******************************************************************************
** Function: PROTOFR_SimStep
**
** Execute a single simulation step.
**
** Notes:
**   1. The SimFault command determines this functions behavior.
*/
void PROTOFR_SimStep(void);


/******************************************************************************
** Function: PROTOFR_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void PROTOFR_ResetStatus(void);


/******************************************************************************
** Function: PROTOFR_SetTlmModeCmd
**
** Set the telemetry reporting mode .
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean PROTOFR_SetTlmModeCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: PROTOFR_SimFaultCmd
**
** Configure the fault simulation. 
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. Multiple individual timed faults can be active at once. The walking
**     fault sim can't be combined with other sim modes. 
*/
boolean PROTOFR_SimFaultCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _protofr_ */
