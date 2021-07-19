/*
** Purpose: Example JSON Table
**
** Notes:
**   1. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      This is a table-specific file so it doesn't need to be re-entrant.
**   2. The table file is a JSON text file.
**   3. One of the goals of the object-based cFS application effort is to 
**      explore using text-based tables that are managed by the application.
**      Once a 'standard' text format is agreed upon the common utilities 
**      could be provided as libraries to the apps.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _jsontbl_
#define _jsontbl_

/*
** Includes
*/

#include "app_cfg.h"
#include "json.h"
#include "extbldata.h"

/*
** Macro Definitions
*/

/*
** Event Message IDs
*/

#define JSONTBL_CREATE_FILE_ERR_EID        (JSONTBL_BASE_EID + 0)
#define JSONTBL_LOAD_INDEX_ERR_EID         (JSONTBL_BASE_EID + 1)
#define JSONTBL_LOAD_LINE_ERR_EID          (JSONTBL_BASE_EID + 2)
#define JSONTBL_CMD_LOAD_TYPE_ERR_EID      (JSONTBL_BASE_EID + 3)
#define JSONTBL_CMD_LOAD_PARSE_ERR_EID     (JSONTBL_BASE_EID + 4)
#define JSONTBL_CMD_LOAD_JSON_OPEN_ERR_EID (JSONTBL_BASE_EID + 5)


/*
** Type Definitions
*/

/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const ExTblData_Param* (*JSONTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*JSONTBL_LoadTbl)(ExTblData_Param* NewTable); 

/* Table Owner's function to load a single table entry */
typedef boolean (*JSONTBL_LoadTblEntry)(uint16 EntryId, ExTblData_Entry* NewEntry);   


typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   DataArrayEntryIdx;
   boolean  Modified[JSONTBL_MAX_ENTRY_ID];

   ExTblData_Param Tbl;

   JSONTBL_GetTblPtr    GetTblPtrFunc;
   JSONTBL_LoadTbl      LoadTblFunc;
   JSONTBL_LoadTblEntry LoadTblEntryFunc; 

   JSON_Class Json;
   JSON_Obj   JsonObj;
   char       JsonFileBuf[JSON_MAX_FILE_CHAR];   
   jsmntok_t  JsonFileTokens[JSON_MAX_FILE_TOKENS];

} JSONTBL_Class;


/*
** Exported Functions
*/

/******************************************************************************
** Function: JSONTBL_Constructor
**
** Initialize the Example JSON Table object.
**
** Notes:
**   1. The table values are not populated. This is done when the table is 
**      registered with the table manager.
*/
void JSONTBL_Constructor(JSONTBL_Class* ObjPtr,
                         JSONTBL_GetTblPtr    GetTblPtrFunc,
                         JSONTBL_LoadTbl      LoadTblFunc, 
                         JSONTBL_LoadTblEntry LoadTblEntryFunc);


/******************************************************************************
** Function: JSONTBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void JSONTBL_ResetStatus(void);


/******************************************************************************
** Function: JSONTBL_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean JSONTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: JSONTBL_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean JSONTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);

#endif /* _jsontbl_ */
