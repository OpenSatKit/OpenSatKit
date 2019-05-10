/*
** Purpose:  Define a class that provides a mechanism for objects to report faults
**
** Notes:
**   1. This code was originally acquired from the NASA cFS External Code
**      Interface (ECI) NOSA release. Several cosmetic changes have been made
**      but it remains functionally very similar.
**   2. This code must be reentrant and no global data can be used. 
**   3. FaultRep_Constructor() must be called prior to any other FaultRep_ 
**      functions
**   4. The telemetry generation requires that an even number of fault
**      detection points be defined. This file requires the number of fault
**      detection points to be a multiple of 16 so the telemetry generation
**      restriction should always be satisfied.
**   5. This object does not associate any meaning to the IDs.
**   6. Typically multiple detection points are "ORed" together for a single
**      fault correction monitor point. This object assumes the combining
**      of fault detection points is accomplished outside of this object.
**   7. The ReportMode flag has the following definitions
**      - FAULTREP_NEW_REPORT - The ID notifications for the current control cycle
**        are copied into the message.
**      - FAULTREP_MERGE_REPORT - The ID notifications for the current control cycle
**        are merged(logically ORed) with the message
**   8. Each Application can customized the following macros:
**      - FAULTREP_ID_MAX
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

#ifndef  _faultrep_
#define  _faultrep_

/*
** Include Files
*/

#include "app_fw_cfg.h"


/*
** Macro Definitions
*/


/*
** Event Service 
*/

#define  FAULTREP_EVS_INV_DETECTOR_ID      0
#define  FAULTREP_EVS_CONFIG_CMD_ERR       1

#define  FAULTREP_EVS_MSG_CNT              2


/*
** The following macros define all possible fault detection points. Applications
** must use these definitions when using package functions that require a
** FaultDetId.
*/

#define  FAULTREP_SELECT_ALL  0xFFFF  /* Used by functions to select all IDs */
                                      /* Must not be a valid detector ID     */

/*
** Define constants that are based on the total number of fault detection
** points:
**
** - FAULTREP_ID_MAX         = Total number of IDs (1..LIMIT). This must
**                             be a multiple of FAULTREP_BITS_PER_WORD.
**
** - FAULTREP_BITFIELD_WORDS = Number of words that are used to hold
**                             bit information
*/

#define FAULTREP_BITS_PER_WORD    16
#define FAULTREP_BITFIELD_WORDS   (FAULTREP_ID_MAX/FAULTREP_BITS_PER_WORD)

#if (FAULTREP_ID_MAX % FAULTREP_BITS_PER_WORD) != 0
   #error FAULTREP_ID_MAX must be a multiple of FAULTREP_BITS_PER_WORD
#endif

/*
** Type definitions
*/


/*
** Report types used by FaultRep_SendTlmMsg()
*/

typedef enum
{

   FAULTREP_NEW_REPORT,   /* Output a new report                     */
   FAULTREP_MERGE_REPORT  /* Output a new report OR'ed with previous */

} FaultRep_TlmMode;



/*
** Report packet to be monitored by another Application
*/

typedef struct
{

   uint16  Data[FAULTREP_BITFIELD_WORDS];  /* Bit packed status */

} FaultRep_Data;


typedef struct
{

   uint8          Hdr[CFE_SB_TLM_HDR_SIZE];

   FaultRep_Data  Tlm;

} FaultRep_TlmMsg;


/*
** Ground Commands
*/

typedef struct
{

   uint16   FaultDetId;      /* Single identifier or FAULTREP_SELECT_ALL */

} FaultRep_ClearFaultDetCmdParam;


typedef struct
{

   uint16   FaultDetId;      /* Single FD identifier or FAULTREP_SELECT_ALL */
   boolean  Enable;          /* TRUE - Enable a FD; FALSE - Disable a FD    */

} FaultRep_ConfigFaultDetCmdParam;



/*
** Data structures for the Fault Reporter Status
**
** - A single bit is used for each fault detection point for the enable/disable
**   configuration and for latched status.
**
** - A latched bit is set when an enabled fault detector notifies FaultRep of 
**   an error. The bit remains set until a command is received to clear the bit.
**
*/

typedef struct
{

   uint16   IdLimit;
   
   uint16   BitfieldWords;
   uint16   BitfieldRemMask;

   uint16   Enabled[FAULTREP_BITFIELD_WORDS];   /* 0 = Disabled, 1 = Enabled */
   uint16   Latched[FAULTREP_BITFIELD_WORDS];   /* 0 = No Error, 1 = Error   */

} FaultRep_FaultDet;


/*
** Fault Reporter Class Definition
*/

typedef struct
{

   uint16             EvsIdBase;
   FaultRep_TlmMode   TlmMode;
   FaultRep_FaultDet  FaultDet;
   FaultRep_Data      NewReport;  /* Collected between SendTlmMsg() calls */
   FaultRep_TlmMsg    SbMsg;      /* Last TLM message sent                 */

} FaultRep_Class;


/*
** Exported Functions
*/


/*
** Purpose:  Initialize a FaultRep object.
**
** Notes:
**   1. This function must be called prior to any other FaultRep_ functions
**   2. The FaultRep object is initialized to a known state. If the
**      FaultRep state should be preserved across some event then
**      the object managing this object should control when the
**      constructor routine is called. The telemetry reporting mode
**      default is FAULTREP_NEW_REPORT.
**
*/
void FaultRep_Constructor(FaultRep_Class*  FaultRepObjParam, 
                          uint16           FaultIdCnt,       /* Number of fault detector IDs used (not an index, but a count) */
                          uint16*          EvsIdBase);       /* Starting event message ID */
                             


/*
** Report that a fault detector failed
**
** Notes:
**   1. Limit checking is performed on the FaultDetId but this type
**      of error should only occur during integration.
**
*/
void FaultRep_FaultDetFailed(FaultRep_Class*  FaultRepObj,
                             uint16           FaultDetId);  /* Integer identifier (not a bit bit mask */



/*
** Set the telemetry reporting mode.
**
** Notes:
**   1. If fault detectors could not execute during the current control
**      cycle then ReportMode should be set to FAULTREP_MERGE_REPORT which 
**      allows consecutive counters in an external monitoring application to
**      continue to count for a detected fault.
**
*/
void FaultRep_SetTlmMode(FaultRep_Class*  FaultRepObj,
                         FaultRep_TlmMode TlmMode);


/*
** Purpose: Clear the latched status flag and the status flag in the report
**          packet for a single fault detector ID or for all of the IDs.
**
** Note:
**   1. This function must comply with the CMDMGR_CmdFuncPtr definition
**   2. If the command parameter fault ID is set to FAULTREP_SELECT_ALL then
**      all fault detector IDs are affected otherwise it's interpreted as a
**      single ID.
**
** Return:
**   TRUE  - Command accepted purpose achieved.
**   FALSE - Command rejected: An event message is issued describing the
**           cause of the failure.
*/
boolean FaultRep_ClearFaultDetCmd(      void* CmdObjPtr,    /* Pointer to an instance of a FaultRep_Class */
                                  const void* CmdParamPtr); /* Pointer to a FaultRep_ClearCmd structure.  */
                                 



/*
** Purpose:  Configure a fault detector to be enabled or disabled.
**
** Note:
**   1. This function must comply with the CMDMGR_CmdFuncPtr definition
**   2. If the command parameter fault ID is set to FAULTREP_SELECT_ALL then
**      all fault detector IDs are affected otherwise it's interpreted as a
**      single ID. The EnableFlag is defined as:
**         TRUE  - Enable the fault detector
**         FALSE - Disable the fault detector
**
** Return:
**   TRUE  - Command accepted purpose achieved.
**   FALSE - Command rejected: An event message is issued describing the
**           cause of the failure.
*/
boolean FaultRep_ConfigFaultDetCmd(      void* CmdObjPtr,    /* Pointer to an instance of a FaultRep_Class */
                                   const void* CmdParamPtr); /* Pointer to a FaultRep_ConfigCmd structure. */
                                      

/*
** Purpose: Update the fault report telemetry message.
**
** Notes:
**   1. This function MUST only be called once for each Application control
**      cycle. It MUST be called after all fault detectors have executed
**      and prior to ground command processing.
**   2. An Applicaton can use FaultRep_SetTlmMode to change the behavior 
**      of this function. See FaultRep_SetTlmMode prologue.
*/
void FaultRep_GenTlmMsg(void*            TlmObjPtr,   /* Pointer to an instance of a FaultRep_Class */
                        CFE_SB_MsgPtr_t  TlmMsgPtr);  /* Pointer to FaultRep's telemetry message.   */
                           

#endif  /* _faultrep_ */
