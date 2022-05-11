/*
** Purpose: Define the payload management object 
**
** Notes:
**   1. An effort was made to maximize this object's coupling with the
**      payload interface and minimize the pl_mgr app and sci_file object
**      couplings. 
**   2. The payload object encapulates the payload interface (maximize
**      coupling), however some of the interface types are needed by the
**      pl_mgr app and the sci_file object. In an effort to minimze coupling
**      pl_sim_app.h is included here and payload.h should only be included
**      where it is absolutely needed. 
**   3. This is a command driven design as opposed to a data driven design.
**      Commands are used to start/stop science data file management. An 
**      alternative design would be a data driven one where the science
**      files would be generated whenever the detector ouptputs data.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _payload_
#define _payload_

/*
** Includes
*/

#include "app_cfg.h"
#include "pl_sim_lib.h"  /* See prologue notes */
#include "sci_file.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Event Message IDs
*/

#define PAYLOAD_START_SCI_CMD_EID     (PAYLOAD_BASE_EID + 0)
#define PAYLOAD_START_SCI_CMD_ERR_EID (PAYLOAD_BASE_EID + 1)
#define PAYLOAD_STOP_SCI_CMD_EID      (PAYLOAD_BASE_EID + 2)
#define PAYLOAD_STOP_SCI_CMD_ERR_EID  (PAYLOAD_BASE_EID + 3)
#define PAYLOAD_SHUTDOWN_SCI_EID      (PAYLOAD_BASE_EID + 4)


/**********************/
/** Type Definitions **/
/**********************/

/******************************************************************************
** Command Packets
**
** Use separate function codes for start/stop science commands. Therefore they
** have no parameters which makes it easier for automated onboard command
** sequences.
**
*/

#define PAYLOAD_START_SCI_CMD_DATA_LEN  PKTUTIL_NO_PARAM_CMD_DATA_LEN
#define PAYLOAD_STOP_SCI_CMD_DATA_LEN   PKTUTIL_NO_PARAM_CMD_DATA_LEN


/******************************************************************************
** Telemetry Packets
*/


/******************************************************************************
** PL_MGR Class
*/

typedef struct
{
   
   PL_SIM_LIB_Power_t    CurrPower;
   PL_SIM_LIB_Power_t    PrevPower;
   PL_SIM_LIB_Detector_t Detector;

   boolean DetectorFault;

   SCI_FILE_Class_t SciFile;

} PAYLOAD_Class_t;



/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: PAYLOAD_Constructor
**
** Initialize the payload to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void PAYLOAD_Constructor(PAYLOAD_Class_t *PayloadPtr, INITBL_Class *IniTbl);


/******************************************************************************
** Function: PAYLOAD_Execute
**
** Execute a single simulation step.
**
*/
void PAYLOAD_ManageData(void);


/******************************************************************************
** Function: PAYLOAD_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void PAYLOAD_ResetStatus(void);


/******************************************************************************
** Functions: PAYLOAD_StartSciCmd
**
** Start science data retreival and storage.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. Use separate command function codes & functions as opposed to one 
**     command with a parameter that would need validation
**
*/
boolean PAYLOAD_StartSciCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Functions: PAYLOAD_StopSciCmd
**
** Stop science data retreival and storage.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. Use separate command function codes & functions as opposed to one 
**     command with a parameter that would need validation
**
*/
boolean PAYLOAD_StopSciCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _sci_file_ */
