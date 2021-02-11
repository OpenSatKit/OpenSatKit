/*
** Purpose:  Define a class that provides a mechanism for objects to report 
**           boolean states represented by a single bit that are aggregated
**           into an app state report packet.
**
** Notes:
**   1. This code was originally acquired from the NASA cFS External Code
**      Interface (ECI) NOSA release and that was named FaultRep because it
**      was exicitly used to report faults using one bit for each fault
**      which made it very easy to configure Limit Checker to logically
**      combine faults. For OSK it was generalized to be a boolean state
**      reporter and it remains functionally very similar.
**   2. This code must be reentrant and no global data can be used. 
**   3. STATEREP_Constructor() must be called prior to any other STATEREP_ 
**      functions
**   4. The telemetry generation requires that an even number of state
**      16-bit state words be defined.
**   5. This object does not associate any meaning to the state bit IDs.
**   6. Typically multiple state definition points are "ORed" together to
**      an meta-state especially when states represents faults. For example,
**      if fault A or fault B occur then take corrective action X. This
**      utility assumes the combining of state definition points is 
**      performed by the received of this utility's telemetry packet.
**   7. The ReportMode flag has the following definitions
**      - STATEREP_NEW_REPORT - The ID notifications for an app's current
**        execution cycle are copied into the message.
**      - STATEREP_MERGE_REPORT - The ID notifications for an app's current
**        execution cycle are merged(logically ORed) with the message
**   8. STATEREP_BIT_ID_MAX must be defined prior to including this header.
**
** License:
**
**   Copyright © 2007-2014 United States Government as represented by the 
**   Administrator of the National Aeronautics and Space Administration. 
**   All Other Rights Reserved.  
**
**   This software was created at NASA's Goddard Space Flight Center.
**   This software is governed by the NASA Open Source Agreement and may be 
**   used, distributed and modified only pursuant to the terms of that 
**   agreement.
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef  _staterep_
#define  _staterep_

/*
** Include Files
*/

#include "osk_c_fw_cfg.h"


/*
** Macro Definitions
*/


/*
** Event Service 
*/

#define  STATEREP_INVALID_ID_EID      (STATEREP_BASE_EID + 0)
#define  STATEREP_CONFIG_CMD_ERR_EID  (STATEREP_BASE_EID + 1)



/*
** The following macros define all possible state definition points. Applications
** must use these definitions when using package functions that require a
** StateId.
*/

#define  STATEREP_SELECT_ALL  0xFFFF  /* Used by functions to select all IDs */
                                      /* Must not be a valid state ID        */

/*
** Define constants that are based on the total number of state points:
**
** - STATEREP_ID_MAX         = Total number of IDs (1..LIMIT). This must
**                             be a multiple of STATEREP_BITS_PER_WORD.
**
** - STATEREP_BITFIELD_WORDS = Number of words that are used to hold
**                             bit information
*/

#define STATEREP_BITS_PER_WORD    16
#define STATEREP_BITFIELD_WORDS   (STATEREP_BIT_ID_MAX/STATEREP_BITS_PER_WORD)

#if (STATEREP_BIT_ID_MAX % STATEREP_BITS_PER_WORD) != 0
   #error STATEREP_BIT_ID_MAX must be a multiple of STATEREP_BITS_PER_WORD
#endif

/*
** Type definitions
*/


/*
** Report types used by STATEREP_SendTlmMsg()
*/

typedef enum
{

   STATEREP_NEW_REPORT   = 1,  /* Only report new state since last report    */
   STATEREP_MERGE_REPORT = 2   /* Boolean OR new states with previous report */

} STATEREP_TlmMode;



/*
** Report packet typically monitored by another Application
*/

typedef struct
{

   uint16  Word[STATEREP_BITFIELD_WORDS];  /* Bit packed status */

} STATEREP_Bits;


typedef struct
{

   uint8          Hdr[CFE_SB_TLM_HDR_SIZE];
   STATEREP_Bits  Bits;

} STATEREP_TlmMsg;
#define STATEREP_TLM_PKT_LEN  sizeof(STATEREP_TlmMsg)


/******************************************************************************
** Command Messages
*/

typedef struct
{

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16   Id;        /* Single identifier: 0..(STATEREP_BIT_ID_MAX-1) or STATEREP_SELECT_ALL */

} STATEREP_ClearBitCmdMsg;
#define STATEREP_CLEAR_BIT_CMD_DATA_LEN  (sizeof(STATEREP_ClearBitCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct
{

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16   Id;           /* Single identifier: 0..(STATEREP_BIT_ID_MAX-1) or STATEREP_SELECT_ALL */
   uint16   Enable;       /* TRUE - Enable an ID, FALSE - Disable an ID (keep word aligned)       */

} STATEREP_ConfigBitCmdMsg;
#define STATEREP_CONFIG_BIT_CMD_DATA_LEN  (sizeof(STATEREP_ConfigBitCmdMsg) - CFE_SB_CMD_HDR_SIZE)



/*
** Data structures for the State Reporter Status
**
** - A single bit is used for each state definition point for the enable/disable
**   configuration and for latched status.
**
** - A latched bit is set when an enabled state ID notifies StateRep of 
**   a true state. The bit remains set until a command is received to clear the bit.
**
*/

typedef struct
{

   uint16   IdLimit;
   
   uint16   BitfieldWords;
   uint16   BitfieldRemMask;

   uint16   Enabled[STATEREP_BITFIELD_WORDS];   /* 0 = Disabled, 1 = Enabled */
   uint16   Latched[STATEREP_BITFIELD_WORDS];   /* 0 = Never set to 1(true), 1 = Set to 1 since last cleared */

} STATEREP_BitConfig;


/*
** State Reporter Class Definition
*/

typedef struct
{

   STATEREP_TlmMode    TlmMode;
   STATEREP_BitConfig  BitConfig;
   STATEREP_Bits       CurrBits;   /* Collected between SendTlmMsg() calls */
   STATEREP_TlmMsg     TlmMsg;     /* Last TLM message sent                */

} STATEREP_Class;


/*
** Exported Functions
*/


/******************************************************************************
** Function: STATEREP_Constructor
**
** Purpose:  Initialize a StateRep object.
**
** Notes:
**   1. This function must be called prior to any other STATEREP_ functions
**   2. The StateRep object is initialized to a known state. If the
**      StateRep state should be preserved across some event then
**      the object managing this object should control when the
**      constructor routine is called. The telemetry reporting mode
**      default is STATEREP_NEW_REPORT.
**
*/
void STATEREP_Constructor(STATEREP_Class*  StateRep, 
                          uint16           IdCnt);      /* Number of state definition IDs used (not an index, but a count) */
                             


/******************************************************************************
** Function: STATEREP_ClearBitCmd
**
** Purpose: Clear the latched status flag and the status flag in the report
**          packet for a single state bit or for all of the state bits.
**
** Note:
**   1. This function must comply with the CMDMGR_CmdFuncPtr definition
**   2. If the command message parameter Id is set to STATEREP_SELECT_ALL
**      then all state IDs are affected otherwise it's interpreted as a
**      single ID.
**
** Return:
**   TRUE  - Command accepted purpose achieved.
**   FALSE - Command rejected: An event message is issued describing the
**           cause of the failure.
*/
boolean STATEREP_ClearBitCmd(                void* ObjDataPtr,  /* Pointer to an instance of a STATEREP_Class */
                             const CFE_SB_MsgPtr_t MsgPtr);     /* Pointer to STATEREP_ClearBitCmd struct     */
                                 


/******************************************************************************
** Function: STATEREP_ConfigBitCmd
**
** Purpose:  Configure a state definition bit to be enabled or disabled.
**
** Note:
**   1. This function must comply with the CMDMGR_CmdFuncPtr definition
**   2. If the command parameter bit ID is set to STATEREP_SELECT_ALL then
**      all bit IDs are affected otherwise it's interpreted as a  single ID.
**   3. The EnableFlag is defined as:
**         TRUE  - Enable the state bit
**         FALSE - Disable the state bit
**
** Return:
**   TRUE  - Command accepted purpose achieved.
**   FALSE - Command rejected: An event message is issued describing the
**           cause of the failure.
*/
boolean STATEREP_ConfigBitCmd(                void* ObjDataPtr,  /* Pointer to an instance of a STATEREP_Class */
                              const CFE_SB_MsgPtr_t MsgPtr);     /* Pointer to STATEREP_ConfigBitCmd struct    */
                                      

/******************************************************************************
** Function: STATEREP_GenTlmMsg
**
** Purpose: Update the state report telemetry message.
**
** Notes:
**   1. This function MUST only be called once for each Application control
**      cycle. It MUST be called after all state definition points have
**      executed and prior to ground command processing.
**   2. An Applicaton can use STATEREP_SetTlmMode to change the behavior 
**      of this function. See STATEREP_SetTlmMode prologue.
*/
void STATEREP_GenTlmMsg(STATEREP_Class*  StateRep,
                        STATEREP_TlmMsg* TlmMsg);
                           
                           
/******************************************************************************
** Function: STATEREP_SetBit
**
** Set a state identifier bit to 1
**
** Notes:
**   1. Limit checking is performed on the Id but this type of error should
**      only occur during integration.
**
*/
void STATEREP_SetBit(STATEREP_Class*  StateRep,
                     uint16           Id);  /* Integer identifier (not a bit bit mask) */



/******************************************************************************
** Function: STATEREP_SetTlmMode
**
** Set the telemetry reporting mode.
**
** Notes:
**   1. If the source of a state bit couldn't not execute during the current
**      state reporting cycle then you should consider setting ReportMode to
**      STATEREP_MERGE_REPORT which allows consecutive counters in an external
**      monitoring application to continue to count for a consecutive state.
**      This is most useful when a state represents a fault so the fault can
**      be assumed to persist if the source that reported the fault loses
**      communication.
**
*/
void STATEREP_SetTlmMode(STATEREP_Class*  StateRep,
                         STATEREP_TlmMode TlmMode);


/******************************************************************************
** Function: STATEREP_TlmModeStr
**
** Purpose: Return a pointer to a string describing the enumerated type
**
** Notes:
**   None
*/
const char* STATEREP_TlmModeStr(STATEREP_TlmMode  TlmMode);


#endif  /* _staterep_ */
