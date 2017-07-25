/*
** Purpose: Define an example object.
**
** Notes:
**   1. Template written by David McComas and licensed under the GNU
**      Lesser General Public License (LGPL).
**   2. This object is to show how an object is integrated into 
**      the opensat application framework.
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _exobj_
#define _exobj_

/*
** Includes
*/

#include "app_cfg.h"
#include "extbl.h"

/*
** Event Message IDs
*/

#define EXOBJ_CMD_ENA_INFO_EID  (EXOBJ_BASE_EID + 0)
#define EXOBJ_DEMO_CMD_INFO_EID (EXOBJ_BASE_EID + 1)

/*
** Type Definitions
*/


/******************************************************************************
** ExObj_Class
*/

typedef struct {

   uint16      ExecCnt;

   EXTBL_Data  TblData;

} EXOBJ_Class;


/******************************************************************************
** Command Functions
*/

typedef struct
{

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16  Parameter;

} EXOBJ_DemoCmdMsg;
#define EXOBJ_DEMO_CMD_DATA_LEN  (sizeof(EXOBJ_DemoCmdMsg) - CFE_SB_CMD_HDR_SIZE)

/*
** Exported Functions
*/

/******************************************************************************
** Function: EXOBJ_Constructor
**
** Initialize the example object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**   2. The table values are not populated. This is done when the table is 
**      registered with the table manager.
**
*/
void EXOBJ_Constructor(EXOBJ_Class *ExObjPtr);


/******************************************************************************
** Function: EXOBJ_Execute
**
** Execute main object function.
**
*/
void EXOBJ_Execute(void);


/******************************************************************************
** Function: EXOBJ_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void EXOBJ_ResetStatus(void);

/******************************************************************************
** Function: EXOBJ_GetDataPtr
**
** Get pointer to ExObj's table data
**
** Note:
**  1. This function must match the EXTBL_GetDataPtrFunc definition.
**  2. Supplied as a callback to ExTbl.
*/
EXTBL_Data* EXOBJ_GetTblDataPtr(void);


/******************************************************************************
** Function: EXOBJ_LoadTbl
**
** Load into ExObj's example table.
**
** Note:
**  1. This function must match the EXTBL_LoadTblFunc definition.
**  2. Supplied as a callback to ExTbl.
*/
boolean EXOBJ_LoadTbl (EXTBL_Data* TblData);


/******************************************************************************
** Function: EXOBJ_LoadEntry
**
** Load into ExObj's example table.
**
** Note:
**  1. This function must match the EXTBL_LoadEntryFunc definition.
**  2. Supplied as a callback to ExTbl.
*/
boolean EXOBJ_LoadTblEntry (uint16 EntryId, EXTBL_Entry* TblEntry);


/******************************************************************************
** Function: EXOBJ_DemoCmd
**
** Demonstrate an 'entity' object having a command.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean EXOBJ_DemoCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _exobj_ */
