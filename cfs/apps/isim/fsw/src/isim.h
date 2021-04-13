/*
** Purpose: Define Instrument Simulator Object
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


#define ISIM_DETECTOR_ROW_LEN        32
#define ISIM_DETECTOR_ROWS_PER_IMAGE 10

#define ISIM_NULL_FILENAME   "null"

/*
** Event Message IDs
*/

#define ISIM_INIT_COMPLETE_EID         (ISIM_BASE_EID +  0)
#define ISIM_RESET_COMPLETE_EID        (ISIM_BASE_EID +  1)
#define ISIM_INVALID_STATE_EID         (ISIM_BASE_EID +  2)
#define ISIM_PWR_ON_CMD_EID            (ISIM_BASE_EID +  3)
#define ISIM_PWR_ON_CMD_ERR_EID        (ISIM_BASE_EID +  4)
#define ISIM_PWR_OFF_CMD_EID           (ISIM_BASE_EID +  5)
#define ISIM_PWR_RESET_CMD_ERR_EID     (ISIM_BASE_EID +  6)
#define ISIM_PWR_RESET_CMD_EID         (ISIM_BASE_EID +  7)
#define ISIM_START_SCI_CMD_EID         (ISIM_BASE_EID +  8)
#define ISIM_START_SCI_CMD_ERR_EID     (ISIM_BASE_EID +  9)
#define ISIM_STOP_SCI_CMD_EID          (ISIM_BASE_EID + 10)
#define ISIM_SET_FAULT_CMD_EID         (ISIM_BASE_EID + 11)
#define ISIM_CLEAR_FAULT_CMD_EID       (ISIM_BASE_EID + 12)
#define ISIM_LOAD_TBL_EID              (ISIM_BASE_EID + 13)
#define ISIM_LOAD_TBL_OBJ_EID          (ISIM_BASE_EID + 14)
#define ISIM_SCI_FILE_CREATE_EID       (ISIM_BASE_EID + 15)
#define ISIM_SCI_FILE_CREATE_ERR_EID   (ISIM_BASE_EID + 16)
#define ISIM_SCI_FILE_WRITE_ERR_EID    (ISIM_BASE_EID + 17)
#define ISIM_SCI_FILE_CLOSE_EID        (ISIM_BASE_EID + 18)
#define ISIM_DETECTOR_ERR_EID          (ISIM_BASE_EID + 19)


/**********************/
/** Type Definitions **/
/**********************/

typedef enum {

   ISIM_PWR_OFF    = 1,
   ISIM_PWR_INIT   = 2,
   ISIM_PWR_RESET  = 3,
   ISIM_PWR_READY  = 4
   
} ISIM_PwrState;

/*
** This state information is used for communication between the
** command functions and the execute function. The start/stop states
** only persist for one execution cycle.
*/
typedef enum {

   ISIM_SCI_OFF       = 1,
   ISIM_SCI_STARTING  = 2,
   ISIM_SCI_STOPPING  = 3,
   ISIM_SCI_ON        = 4

} ISIM_SciState;


typedef struct {
   
   char Data[ISIM_DETECTOR_ROW_LEN];
   
} ISIM_DetectorRow;

typedef struct {

   uint16   ReadoutRow;   
   uint16   ImageCnt;
   boolean  FaultPresent;

   ISIM_DetectorRow Row;

} ISIM_Detector;

typedef struct {

   ISIM_PwrState  PwrState;   
   uint16         PwrInitCycleCnt;
   uint16         PwrResetCycleCnt;
   uint16         CurrFileImageCnt;
   
   ISIM_SciState  SciState;
   ISIM_Detector  Detector;
        
} ISIM_Instr;


typedef struct {

   int32   Handle;
   boolean IsOpen;
   
   char Name[OS_MAX_PATH_LEN];

} ISIM_SciFile;


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

#define ISIM_PWR_ON_INSTR_CMD_DATA_LEN    ISIM_NO_PARAM_CMD_DATA_LEN
#define ISIM_PWR_OFF_INSTR_CMD_DATA_LEN   ISIM_NO_PARAM_CMD_DATA_LEN
#define ISIM_PWR_RESET_INSTR_CMD_DATA_LEN ISIM_NO_PARAM_CMD_DATA_LEN
#define ISIM_START_SCI_CMD_DATA_LEN       ISIM_NO_PARAM_CMD_DATA_LEN
#define ISIM_STOP_SCI_CMD_DATA_LEN        ISIM_NO_PARAM_CMD_DATA_LEN
#define ISIM_SET_FAULT_CMD_DATA_LEN       ISIM_NO_PARAM_CMD_DATA_LEN
#define ISIM_CLEAR_FAULT_CMD_DATA_LEN     ISIM_NO_PARAM_CMD_DATA_LEN


/******************************************************************************
** Telemetry Packets
*/



/******************************************************************************
** Isim_Class
*/

typedef struct {

   ISIM_Instr    Instr;
   ISIM_SciFile  SciFile;
     
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
** Notes:
**  1. This function must match the ISIMTBL_GetTblPtr definition.
**  2. Supplied as a callback to IsimTbl.
**
*/
const ISIMTBL_Struct* ISIM_GetTblPtr(void);


/******************************************************************************
** Function: ISIM_LoadTbl
**
** Load data into Isim's table.
**
** Notes:
**  1. This function must match the ISIMTBL_LoadTblFunc definition.
**  2. Supplied as a callback to IsimTbl.
**
*/
boolean ISIM_LoadTbl (ISIMTBL_Struct* NewTbl);


/******************************************************************************
** Function: ISIM_LoadEntry
**
** Load data into Isim's table.
**
** Notes:
**  1. This function must match the ISIMTBL_LoadEntryFunc definition.
**  2. Supplied as a callback to IsimTbl.
**
*/
boolean ISIM_LoadTblEntry (uint16 ObjId, void* ObjData);


/******************************************************************************
** Functions: ISIM_PwrOnInstrCmd, ISIM_PwrOffInstrCmd, ISIM_PwrResetInstrCmd
**
** Power on/off/reset science instrument.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. Use separate command function codes & functions as opposed to one 
**     command with a parameter that would need validation
**  3. Reset allows an intermediate level of initialization to be simulated
**     that allows some system state to persist across the reset. For
**     science data may be allowed to resume immediately after a reset.
**
*/
boolean ISIM_PwrOnInstrCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
boolean ISIM_PwrOffInstrCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
boolean ISIM_PwrResetInstrCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Functions: ISIM_StartSciCmd, ISIM_StopSciCmd
**
** Start/stop science instrument data.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. Use separate command function codes & functions as opposed to one 
**     command with a parameter that would need validation
**
*/
boolean ISIM_StartSciCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
boolean ISIM_StopSciCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Functions: ISIM_SetFaultCmd, ISIM_ClearFaultCmd
**
** Set/clear fault state.
**
** Notes:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. Use separate command function codes & functions as opposed to one 
**     command with a parameter that would need validation
**
*/
boolean ISIM_SetFaultCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
boolean ISIM_ClearFaultCmd (void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _isim_ */
