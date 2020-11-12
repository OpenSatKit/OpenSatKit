/*
** Purpose: Define Instrument Simulator App
**
** Notes:
**   None
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _isim_
#define _isim_

/*
** Includes
*/

#include "app_cfg.h"
#include "isimtbl.h"

/***********************/
/** Macro Definitions **/
/***********************/


#define ISIM_DATA_SAMPLE_MAX 10
#define ISIM_NULL_FILENAME   "null"

/*
** Event Message IDs
*/

#define ISIM_INIT_COMPLETE_EID         (ISIM_BASE_EID +  0)
#define ISIM_INVALID_STATE_EID         (ISIM_BASE_EID +  1)
#define ISIM_PWR_ON_SCI_CMD_EID        (ISIM_BASE_EID +  2)
#define ISIM_PWR_ON_SCI_CMD_ERR_EID    (ISIM_BASE_EID +  3)
#define ISIM_PWR_OFF_SCI_CMD_EID       (ISIM_BASE_EID +  4)
#define ISIM_START_SCI_CMD_EID         (ISIM_BASE_EID +  5)
#define ISIM_START_SCI_CMD_ERR_EID     (ISIM_BASE_EID +  6)
#define ISIM_STOP_SCI_CMD_EID          (ISIM_BASE_EID +  7)
#define ISIM_CFG_FAULT_CMD_EID         (ISIM_BASE_EID +  8)
#define ISIM_LOAD_TBL_EID              (ISIM_BASE_EID +  9)
#define ISIM_LOAD_TBL_OBJ_EID          (ISIM_BASE_EID + 10)
#define ISIM_CREATE_SCI_FILE_EID       (ISIM_BASE_EID + 11)
#define ISIM_CLOSE_SCI_FILE_EID        (ISIM_BASE_EID + 12)
#define ISIM_CREATE_SCI_FILE_ERROR_EID (ISIM_BASE_EID + 13)

/**********************/
/** Type Definitions **/
/**********************/

typedef enum {

   ISIM_OFF          = 0,
   ISIM_INITIALIZING = 1,
   ISIM_READY        = 2

} ISIM_InstrState;

/*
** This state information is used for communication between the
** command functions and the execute function. The start/stop states
** only persist for one execution cycle.
*/
typedef enum {

   ISIM_SCI_OFF   = 0,
   ISIM_SCI_START = 1,
   ISIM_SCI_STOP  = 2,
   ISIM_SCI_ON    = 3

} ISIM_SciState;


/******************************************************************************
** Command Packets
**
** Use separate function codes for power on/off and start/stop science
** commands. Therefore they all have command length with no parameters.
**
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];

} ISIM_NoParamCmdMsg;
#define ISIM_NO_PARAM_CMD_DATA_LEN  (sizeof(ISIM_NoParamCmdMsg) - CFE_SB_CMD_HDR_SIZE)

#define ISIM_PWR_ON_SCI_CMD_DATA_LEN  ISIM_NO_PARAM_CMD_DATA_LEN
#define ISIM_PWR_OFF_SCI_CMD_DATA_LEN ISIM_NO_PARAM_CMD_DATA_LEN
#define ISIM_START_SCI_CMD_DATA_LEN   ISIM_NO_PARAM_CMD_DATA_LEN
#define ISIM_STOP_SCI_CMD_DATA_LEN    ISIM_NO_PARAM_CMD_DATA_LEN


typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16  NewState;

} ISIM_CfgFaultCmdMsg;
#define ISIM_CFG_FAULT_CMD_DATA_LEN  (sizeof(ISIM_CfgFaultCmdMsg) - CFE_SB_CMD_HDR_SIZE)



/******************************************************************************
** Telemetry Packets
*/



/******************************************************************************
** Isim_Class
*/

typedef struct {

   ISIM_InstrState State;
   uint16          InitCycleCnt;

} ISIM_Instr;

typedef struct {

   ISIM_SciState State;

   int32   FileHandle;  
   uint16  FileCnt;   
   uint16  FileCycleCnt;
   boolean FileOpen;
   
   char Filename[OS_MAX_PATH_LEN];

} ISIM_Sci;

typedef struct {

   ISIM_Instr Instr;
   ISIM_Sci   Sci;
  
   boolean  Fault;
   
   ISIMTBL_Struct Tbl;

} ISIM_Class;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: ISIM_Constructor
**
** Initialize the intrument simulator to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**   2. The table values are not populated. This is done when the table is 
**      registered with the table manager.
**
*/
void ISIM_Constructor(ISIM_Class *IsimPtr);


/******************************************************************************
** Function: ISIM_Execute
**
** Execute a single simulation step.
**
*/
void ISIM_Execute(void);


/******************************************************************************
** Function: ISIM_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void ISIM_ResetStatus(void);


/******************************************************************************
** Function: ISIM_GetTblPtr
**
** Get pointer to Isim's table data
**
** Note:
**  1. This function must match the ISIMTBL_GetTblPtr definition.
**  2. Supplied as a callback to IsimTbl.
*/
const ISIMTBL_Struct* ISIM_GetTblPtr(void);


/******************************************************************************
** Function: ISIM_LoadTbl
**
** Load data into Isim's table.
**
** Note:
**  1. This function must match the ISIMTBL_LoadTblFunc definition.
**  2. Supplied as a callback to IsimTbl.
*/
boolean ISIM_LoadTbl (ISIMTBL_Struct* NewTbl);


/******************************************************************************
** Function: ISIM_LoadEntry
**
** Load data into Isim's table.
**
** Note:
**  1. This function must match the ISIMTBL_LoadEntryFunc definition.
**  2. Supplied as a callback to IsimTbl.
*/
boolean ISIM_LoadTblEntry (uint16 ObjId, void* ObjData);


/******************************************************************************
** Functions: ISIM_PwrOnSciCmd, ISIM_PwrOffSciCmd
**
** Power on/off science instrument.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. One function for each command function code.
*/
boolean ISIM_PwrOnSciCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
boolean ISIM_PwrOffSciCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Functions: ISIM_StartSciCmd, ISIM_StopSciCmd
**
** Start/stop science instrument data.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. One function for each command function code.
*/
boolean ISIM_StartSciCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
boolean ISIM_StopSciCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Functions: ISIM_CfgFaultCmd
**
** Set/clear fault state.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean ISIM_CfgFaultCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _isim_ */
