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
**      A. Select which table tuple is used in demobj's computation
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


/*
** Event Message IDs
*/

#define DEMOBJ_CMD_ENA_DATA_LOAD_INFO_EID   (DEMOBJ_BASE_EID + 0)
#define DEMOBJ_CMD_SET_TBL_INDEX_INFO_EID   (DEMOBJ_BASE_EID + 1)
#define DEMOBJ_CMD_SET_TBL_INDEX_ERR_EID    (DEMOBJ_BASE_EID + 2)


/*
** Type Definitions
*/



/******************************************************************************
** DemObj_Class
*/

typedef struct {

   boolean    EnableDataLoad;
   uint8      TblIndex;
   uint16     Data1;
   uint16     Data2;
   uint16     Data3;

   XMLTBL_Struct   XmlTbl;
   SCANFTBL_Struct ScanfTbl;
   JSONTBL_Struct  JsonTbl;

} DEMOBJ_Class;


/******************************************************************************
** Command Functions
*/

typedef struct
{

   uint8     CmdHeader[CFE_SB_CMD_HDR_SIZE];
   boolean   EnableDataLoad;

} DEMOBJ_EnableDataLoadCmdParam;
#define DEMOBJ_ENABLE_DATA_LOAD_CMD_DATA_LEN  (sizeof(DEMOBJ_EnableDataLoadCmdParam) - CFE_SB_CMD_HDR_SIZE)


typedef struct
{

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16   TblIndex;  

}  DEMOBJ_SetTblIndexCmdParam;
#define DEMOBJ_SET_TBL_INDEX_CMD_DATA_LEN  (sizeof(DEMOBJ_SetTblIndexCmdParam) - CFE_SB_CMD_HDR_SIZE)

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
const XMLTBL_Struct*   DEMOBJ_GetXmlTblPtr(void);
const SCANFTBL_Struct* DEMOBJ_GetScanfTblPtr(void);
const JSONTBL_Struct*  DEMOBJ_GetJsonTblPtr(void);


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
** Function: DEMOBJ_LoadDataFromXxxTbl
**
** If enabled the latest data from the table is loaded into the working buffer
** for the example computation.
*/
void DEMOBJ_LoadDataFromXmlTbl(void);
void DEMOBJ_LoadDataFromScanfTbl(void);
void DEMOBJ_LoadDataFromJsosnTbl(void);


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
boolean DEMOBJ_LoadXmlTbl(XMLTBL_Struct* NewTbl);
boolean DEMOBJ_LoadScanfTbl(SCANFTBL_Struct* NewTbl);
boolean DEMOBJ_LoadJsonTbl(JSONTBL_Struct* NewTbl);


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
boolean DEMOBJ_LoadXmlTblEntry(uint16 EntryId, XMLTBL_Entry* NewEntry);
boolean DEMOBJ_LoadScanfTblEntry(uint16 EntryId, SCANFTBL_Entry* NewEntry);
boolean DEMOBJ_LoadJsonTblEntry(uint16 EntryId, JSONTBL_Entry* NewEntry);


/******************************************************************************
** Function: DEMOBJ_EnableDataLoadCmd
**
** Enable the ability to load data from the table to the working data buffer.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean DEMOBJ_EnableDataLoadCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: DEMOBJ_SetTblIndexCmd
**
** Set the index that determines which set of data will be used.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean DEMOBJ_SetTblIndexCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _demobj_ */
