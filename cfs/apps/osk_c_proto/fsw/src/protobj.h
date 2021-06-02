/*
** Purpose: Define a prototype object for the proto app.
**
** Notes:
**   1. This object is for illustrative purposes only. The computational
**      functionality isn't very pratical but the goal is to illustrate
**      how an app's object can define/use commands, tables, and telemtry.
**   2. The example object table (ProtObjTbl) defines an array of elements and
**      each element is a 3-tuple (Data1, Data2, Data3). Two commands are
**      defined:
**      A. Select the table and which table entry is used in protobj's
**         computation
**      B. Enable/disable the use of the table data in the computation
**   3. Different text-based tables were protoyped prior to selecting JSON.
**      They are all retained so the user can see how they vary.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _protobj_
#define _protobj_

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

#define PROTOBJ_CMD_ENA_TBL_DATA_EID       (PROTOBJ_BASE_EID + 0)
#define PROTOBJ_CMD_SET_TBL_INDEX_EID      (PROTOBJ_BASE_EID + 1)
#define PROTOBJ_CMD_SET_TBL_INDEX_ERR_EID  (PROTOBJ_BASE_EID + 2)
#define PROTOBJ_INVALID_TBL_ID_EID         (PROTOBJ_BASE_EID + 3)


/*
** Type Definitions
*/

typedef enum {
   
   PROTOBJ_XML_TBL_ID = 0,
   PROTOBJ_SCANF_TBL_ID,
   PROTOBJ_JSON_TBL_ID,
   PROTOBJ_CFE_TBL_ID,
   PROTOBJ_UNDEF_TBL_ID
   
} PROTOBJ_TBL_ID;


/******************************************************************************
** ProtObj_Class
*/

typedef struct {

   boolean         TblDataEnabled;
   PROTOBJ_TBL_ID  TblId;
   uint8           TblIndex;
   
   /*
   ** 'Active' table data entry managed by ground commands and sent in
   ** housekeeping telemetry
   */
   
   ExTblData_Entry TblEntry;  
 
   /*
   ** Table values used by proto object. The cFE maintains
   ** 'working buffer' on behalf of table owner so a buffer
   ** is not needed for the cFE table.   
   */
   ExTblData_Param  XmlParam;
   ExTblData_Param  ScanfParam;
   ExTblData_Param  JsonParam;

   /*
   ** Table objects that manage the ground interface   
   */

   XMLTBL_Class    XmlTbl;
   SCANFTBL_Class  ScanfTbl;
   JSONTBL_Class   JsonTbl;   
   CFETBL_Class    CfeTbl;
   
} PROTOBJ_Class;


/******************************************************************************
** Command Packets
*/

typedef struct {

   uint8     CmdHeader[CFE_SB_CMD_HDR_SIZE];
   boolean   EnableTblData;

} PROTOBJ_EnableTblDataCmdMsg;
#define PROTOBJ_ENABLE_TBL_DATA_CMD_DATA_LEN  (sizeof(PROTOBJ_EnableTblDataCmdMsg) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint8   TblId;       /* See PROTOBJ_TBL_ID. Enum not used here so storage size could be controlled */
   uint8   TblIndex;  

}  PROTOBJ_SetActiveTblCmdMsg;
#define PROTOBJ_SET_ACTIVE_TBL_CMD_DATA_LEN  (sizeof(PROTOBJ_SetActiveTblCmdMsg) - CFE_SB_CMD_HDR_SIZE)

/*
** Exported Functions
*/

/******************************************************************************
** Function: PROTOBJ_Constructor
**
** Initialize the example object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void PROTOBJ_Constructor(PROTOBJ_Class *ProtObjPtr);


/******************************************************************************
** Function: PROTOBJ_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void PROTOBJ_ResetStatus(void);


/******************************************************************************
** Function: PROTOBJ_Execute
**
** Execute proto object functionality during each application runloop cycle.
**
*/
void PROTOBJ_Execute(void);


/******************************************************************************
** Function: PROTOBJ_GetTblData
**
** If table data is enabled then the data entry from the active table[index]
** is loaded into the parameter. If the table is disabled then the parameter
** data is set to zero. 
**
** Return TRUE is table table is enabled, otherwise return FALSE.
*/
boolean PROTOBJ_GetTblData(ExTblData_Entry* TblEntryData);


/******************************************************************************
** Function: PROTOBJ_EnableTblDataCmd
**
** Enable a table's data to be used in ProtoObj's working buffer and sent in 
** telemetry
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean PROTOBJ_EnableTblDataCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: PROTOBJ_SetActiveTblCmd
**
** Activate a specific table & data entry to be used in the working buffer.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean PROTOBJ_SetActiveTblCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);









/******************************************************************************
** Function: PROTOBJ_GetXxxTblPtr
**
** Return a pointer to the table.
**
** Notes:
**   1. Function signature must match table objects's XXX_GetTblPtr():
**      XMLTBL_GetTblPtr, SCANFTBL_GetTblPtr, JSONTBL_GetTblPtr.
**   2. In a real application there would only be one function, but since this
**      is a proto, one function is defined for each example table type.
*/
const ExTblData_Param*  PROTOBJ_GetXmlTblPtr(void);
const ExTblData_Param*  PROTOBJ_GetScanfTblPtr(void);
const ExTblData_Param*  PROTOBJ_GetJsonTblPtr(void);


/******************************************************************************
** Function: PROTOBJ_LoadXxxTbl
**
** Loads the entire new table.
**
** Notes:
**   1. No validity checks are performed on the table data.
**   2. Function signature must match table objects's XXX_LoadTbl():
**      XMLTBL_LoadTbl, SCANFTBL_LoadTbl, JSONTBL_LoadTbl.
**   3. In a real application there would only be one function, but since this
**      is a proto, one function is defined for each example table type.
**
*/
boolean PROTOBJ_LoadXmlTbl(ExTblData_Param* NewTbl);
boolean PROTOBJ_LoadScanfTbl(ExTblData_Param* NewTbl);
boolean PROTOBJ_LoadJsonTbl(ExTblData_Param* NewTbl);


/******************************************************************************
** Function: PROTOBJ_LoadXxxTblEntry
**
** Load a single table entry
**
** Notes:
**   1. Range checking is not performed on the parameter and no validitychecks
**      checks are performed on the table data.
**   2. Function signature must match table objects's XXX_LoadTblEntry():
**      XMLTBL_LoadTblEntry, SCANFTBL_LoadTblEntry, JSONTBL_LoadTblEntry.
**   3. In a real application there would only be one function, but since this
**      is a proto, one function is defined for each example table type.
**
*/
boolean PROTOBJ_LoadXmlTblEntry(uint16 EntryId, ExTblData_Entry* NewEntry);
boolean PROTOBJ_LoadScanfTblEntry(uint16 EntryId, ExTblData_Entry* NewEntry);
boolean PROTOBJ_LoadJsonTblEntry(uint16 EntryId, ExTblData_Entry* NewEntry);

#endif /* _protobj_ */
