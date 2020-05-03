/*
** Purpose: Define a demo object for the demo app.
**
** Notes:
**   1. This object is for illustrative purposes only. The computational
**      functionality isn't very pratical but the goal is to illustrate
**      how an app's object can define/use commands, tables, and telemtry.
**   2. The example object table (DemObjTbl) defines an array of elements and
**      each element is a 3-tuple (Data1, Data2, Data3). Two commands are
**      defined:
**      A. Select the table and which table entry is used in demobj's
**         computation
**      B. Enable/disable the use of the table data in the computation
**   3. Different text-based tables were protoyped prior to selecting JSON.
**      They ar eall retained so the ser can see how they vary.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _demobj_
#define _demobj_

/*
** Includes
*/

#include "app_cfg.h"
#include "scanftbl.h"
#include "xmltbl.h"
#include "jsontbl.h"
#include "cfetbl.h"

/*
** Event Message IDs
*/

#define DEMOBJ_CMD_ENA_TBL_DATA_EID       (DEMOBJ_BASE_EID + 0)
#define DEMOBJ_CMD_SET_TBL_INDEX_EID      (DEMOBJ_BASE_EID + 1)
#define DEMOBJ_CMD_SET_TBL_INDEX_ERR_EID  (DEMOBJ_BASE_EID + 2)
#define DEMOBJ_INVALID_TBL_ID_EID         (DEMOBJ_BASE_EID + 2)


/*
** Type Definitions
*/

typedef enum {
   
   DEMOBJ_XML_TBL_ID = 0,
   DEMOBJ_SCANF_TBL_ID,
   DEMOBJ_JSON_TBL_ID,
   DEMOBJ_CFE_TBL_ID,
   DEMOBJ_UNDEF_TBL_ID
   
} DEMOBJ_TBL_ID;


/******************************************************************************
** DemObj_Class
*/

typedef struct {

   boolean         TblDataEnabled;
   DEMOBJ_TBL_ID   TblId;
   uint8           TblIndex;
   
   ExTblData_Entry TblEntry;  /* "Working buffer" with active data entry */
 
   ExTblData_Param  XmlTbl;
   ExTblData_Param  ScanfTbl;
   ExTblData_Param  JsonTbl;

   CFETBL_Class    CfeTbl;
   
} DEMOBJ_Class;


/******************************************************************************
** Command Functions
*/

typedef struct {

   uint8     CmdHeader[CFE_SB_CMD_HDR_SIZE];
   boolean   EnableTblData;

} DEMOBJ_EnableTblDataCmdParam;
#define DEMOBJ_ENABLE_TBL_DATA_CMD_DATA_LEN  (sizeof(DEMOBJ_EnableTblDataCmdParam) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint8   TblId;       /* See DEMOBJ_TBL_ID. Enum not used here so storage size could be controlled */
   uint8   TblIndex;  

}  DEMOBJ_SetActiveTblCmdParam;
#define DEMOBJ_SET_ACTIVE_TBL_CMD_DATA_LEN  (sizeof(DEMOBJ_SetActiveTblCmdParam) - CFE_SB_CMD_HDR_SIZE)

/*
** Exported Functions
*/

/******************************************************************************
** Function: DEMOBJ_Constructor
**
** Initialize the example object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void DEMOBJ_Constructor(DEMOBJ_Class *DemObjPtr);


/******************************************************************************
** Function: DEMOBJ_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void DEMOBJ_ResetStatus(void);


/******************************************************************************
** Function: DEMOBJ_Execute
**
** Execute demo object functionality during each application runloop cycle.
**
*/
void DEMOBJ_Execute(void);


/******************************************************************************
** Function: DEMOBJ_GetTblData
**
** If table data is enabled then the data entry from the active table[index]
** is loaded into the parameter. If the table is disabled then the parameter
** data is set to zero. 
**
** Return TRUE is table table is enabled, otherwise return FALSE.
*/
boolean DEMOBJ_GetTblData(ExTblData_Entry* TblEntryData);


/******************************************************************************
** Function: DEMOBJ_EnableTblDataCmd
**
** Enable a table's data to be used in DemoObj's working buffer and sent in 
** telemetry
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean DEMOBJ_EnableTblDataCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: DEMOBJ_SetActiveTblCmd
**
** Activate a specific table & data entry to be used in the working buffer.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean DEMOBJ_SetActiveTblCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);









/******************************************************************************
** Function: DEMOBJ_GetXxxTblPtr
**
** Return a pointer to the table.
**
** Notes:
**   1. Function signature must match table objects's XXX_GetTblPtr():
**      XMLTBL_GetTblPtr, SCANFTBL_GetTblPtr, JSONTBL_GetTblPtr.
**   2. In a real application there would only be one function, but since this
**      is a demo, one function is defined for each example table type.
*/
const ExTblData_Param*  DEMOBJ_GetXmlTblPtr(void);
const ExTblData_Param*  DEMOBJ_GetScanfTblPtr(void);
const ExTblData_Param*  DEMOBJ_GetJsonTblPtr(void);


/******************************************************************************
** Function: DEMOBJ_LoadXxxTbl
**
** Loads the entire new table.
**
** Notes:
**   1. No validity checks are performed on the table data.
**   2. Function signature must match table objects's XXX_LoadTbl():
**      XMLTBL_LoadTbl, SCANFTBL_LoadTbl, JSONTBL_LoadTbl.
**   3. In a real application there would only be one function, but since this
**      is a demo, one function is defined for each example table type.
**
*/
boolean DEMOBJ_LoadXmlTbl(ExTblData_Param* NewTbl);
boolean DEMOBJ_LoadScanfTbl(ExTblData_Param* NewTbl);
boolean DEMOBJ_LoadJsonTbl(ExTblData_Param* NewTbl);


/******************************************************************************
** Function: DEMOBJ_LoadXxxTblEntry
**
** Load a single table entry
**
** Notes:
**   1. Range checking is not performed on the parameter and no validitychecks
**      checks are performed on the table data.
**   2. Function signature must match table objects's XXX_LoadTblEntry():
**      XMLTBL_LoadTblEntry, SCANFTBL_LoadTblEntry, JSONTBL_LoadTblEntry.
**   3. In a real application there would only be one function, but since this
**      is a demo, one function is defined for each example table type.
**
*/
boolean DEMOBJ_LoadXmlTblEntry(uint16 EntryId, ExTblData_Entry* NewEntry);
boolean DEMOBJ_LoadScanfTblEntry(uint16 EntryId, ExTblData_Entry* NewEntry);
boolean DEMOBJ_LoadJsonTblEntry(uint16 EntryId, ExTblData_Entry* NewEntry);

#endif /* _demobj_ */
