/*
** Purpose: Define an example object.
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**   2. This object is for illustrative purposes only. The computational
**      functionality isn't very pratical but the goal is to illustrate
**      how an app's object can define/use commands, tables, and telemtry.
**   3. It is not designed as a template object and it's not clear a generic
**      object template would be very helup.
**   4. The example object table (ExObjTbl) defines an array of elements and
**      each element is a 3-tuple (Data1, Data2, Data3). Two commands are
**      defined:
**      A. Select which table tuple is used in exobj's computation
**      B. Enable/disable the use of the table data in the computation
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _exobj_
#define _exobj_

/*
** Includes
*/

#include "app_cfg.h"
#include "scanftbl.h"
#include "xmltbl.h"


/*
** Event Message IDs
*/

#define EXOBJ_CMD_ENA_DATA_LOAD_INFO_EID   (EXOBJ_BASE_EID + 0)
#define EXOBJ_CMD_SET_TBL_INDEX_INFO_EID   (EXOBJ_BASE_EID + 1)
#define EXOBJ_CMD_SET_TBL_INDEX_ERR_EID    (EXOBJ_BASE_EID + 2)


/*
** Type Definitions
*/



/******************************************************************************
** ExObj_Class
*/

typedef struct {

   boolean    EnableDataLoad;
   uint8      TblIndex;
   uint16     Data1;
   uint16     Data2;
   uint16     Data3;

   XMLTBL_Struct   XmlTbl;
   SCANFTBL_Struct ScanfTbl;

} EXOBJ_Class;


/******************************************************************************
** Command Functions
*/

typedef struct
{

   uint8     CmdHeader[CFE_SB_CMD_HDR_SIZE];
   boolean   EnableDataLoad;

} EXOBJ_EnableDataLoadCmdParam;
#define EXOBJ_ENABLE_DATA_LOAD_CMD_DATA_LEN  (sizeof(EXOBJ_EnableDataLoadCmdParam) - CFE_SB_CMD_HDR_SIZE)


typedef struct
{

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16   TblIndex;  

}  EXOBJ_SetTblIndexCmdParam;
#define EXOBJ_SET_TBL_INDEX_CMD_DATA_LEN  (sizeof(EXOBJ_SetTblIndexCmdParam) - CFE_SB_CMD_HDR_SIZE)

/*
** Exported Functions
*/

/******************************************************************************
** Function: EXOBJ_Constructor
**
** Initial the example object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void EXOBJ_Constructor(EXOBJ_Class *ExObjPtr);


/******************************************************************************
** Function: EXOBJ_GetXxxTblPtr
**
** Return a pointer to the table.
**
** Notes:
**   1. Function signature must match table objects's XXX_GetTblPtr():
**      XMLTBL_GetTblPtr, SCANFTBL_GetTblPtr.
**   2. In a real application there would only be one function, but since this
**      is a demo, one function is defined for each example table type.
*/
const XMLTBL_Struct* EXOBJ_GetXmlTblPtr(void);
const SCANFTBL_Struct* EXOBJ_GetScanfTblPtr(void);


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
** Function: EXOBJ_LoadDataFromXxxTbl
**
** If enabled the latest data from the table is loaded into the working buffer
** for the example computation.
*/
void EXOBJ_LoadDataFromXmlTbl(void);
void EXOBJ_LoadDataFromScanfTbl(void);


/******************************************************************************
** Function: EXOBJ_LoadXxxTbl
**
** Loads the entire new table.
**
** Notes:
**   1. No validity checks are performed on the table data.
**   2. Function signature must match table objects's XXX_LoadTbl():
**      XMLTBL_LoadTbl, SCANFTBL_LoadTbl.
**   3. In a real application there would only be one function, but since this
**      is a demo, one function is defined for each example table type.
**
*/
boolean EXOBJ_LoadXmlTbl(XMLTBL_Struct* NewTbl);
boolean EXOBJ_LoadScanfTbl(SCANFTBL_Struct* NewTbl);


/******************************************************************************
** Function: EXOBJ_LoadXxxTblEntry
**
** Load a single table entry
**
** Notes:
**   1. Range checking is not performed on the parameter and no validitychecks
**      checks are performed on the table data.
**   2. Function signature must match table objects's XXX_LoadTblEntry():
**      XMLTBL_LoadTblEntry, SCANFTBL_LoadTblEntry.
**   3. In a real application there would only be one function, but since this
**      is a demo, one function is defined for each example table type.
**
*/
boolean EXOBJ_LoadXmlTblEntry(uint16 EntryId, XMLTBL_Entry* NewEntry);
boolean EXOBJ_LoadScanfTblEntry(uint16 EntryId, SCANFTBL_Entry* NewEntry);


/******************************************************************************
** Function: EXOBJ_EnableDataLoadCmd
**
** Enable the ability to load data from the table to the working data buffer.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean EXOBJ_EnableDataLoadCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: EXOBJ_SetTblIndexCmd
**
** Set the index that determines which set of data will be used.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean EXOBJ_SetTblIndexCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _exobj_ */
