/*
** Purpose: Define Example Table
**
** Notes:
**   1. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      This is a table-specific file so it doesn't need to be re-entrant.
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _extbl_
#define _extbl_

/*
** Includes
*/

#include "app_cfg.h"
#include "json.h"

/*
** Macro Definitions
*/

/*
** Event Message IDs
*/

#define EXTBL_CREATE_FILE_ERR_EID        (EXTBL_BASE_EID + 0)
#define EXTBL_LOAD_INDEX_ERR_EID         (EXTBL_BASE_EID + 1)
#define EXTBL_LOAD_LINE_ERR_EID          (EXTBL_BASE_EID + 2)
#define EXTBL_LOAD_CMD_TYPE_ERR_EID      (EXTBL_BASE_EID + 3)
#define EXTBL_LOAD_CMD_PARSE_ERR_EID     (EXTBL_BASE_EID + 4)
#define EXTBL_LOAD_CMD_JSON_OPEN_ERR_EID (EXTBL_BASE_EID + 5)
#define EXTBL_LOAD_CMD_DBG_EID           (EXTBL_BASE_EID + 6)

/*
** Type Definitions
*/

/******************************************************************************
** Table -  Local table copy used for table loads
** 
*/

typedef struct {

   uint16   Data1;
   uint16   Data2;
   uint16   Data3;

} EXTBL_Entry;

typedef struct
{

   EXTBL_Entry Entry[EXTBL_MAX_ENTRY_ID];

} EXTBL_Struct;



/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const EXTBL_Struct* (*EXTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*EXTBL_LoadTbl)(EXTBL_Struct* NewTable); 

/* Table Owner's function to load a single table entry */
typedef boolean (*EXTBL_LoadTblEntry)(uint16 EntryId, EXTBL_Entry* NewEntry);   


typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   DataArrayEntryIdx;
   boolean  Modified[EXTBL_MAX_ENTRY_ID];

   EXTBL_Struct Tbl;

   EXTBL_GetTblPtr    GetTblPtrFunc;
   EXTBL_LoadTbl      LoadTblFunc;
   EXTBL_LoadTblEntry LoadTblEntryFunc; 

   JSON_Class Json;
   JSON_Obj   JsonObj;
   char       JsonFileBuf[JSON_MAX_FILE_CHAR];   
   jsmntok_t  JsonFileTokens[JSON_MAX_FILE_TOKENS];

} EXTBL_Class;


/*
** Exported Functions
*/


/******************************************************************************
** Function: EXTBL_Constructor
**
** Initialize the example table object.
**
** Notes:
**   1. The table values are not populated. This is done when the table is 
**      registered with the table manager.
**
*/
void EXTBL_Constructor(EXTBL_Class* TblObj, 
                       EXTBL_GetTblPtr    GetTblPtrFunc,
                       EXTBL_LoadTbl      LoadTblFunc, 
                       EXTBL_LoadTblEntry LoadTblEntryFunc);


/******************************************************************************
** Function: EXTBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavour of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void EXTBL_ResetStatus(void);


/******************************************************************************
** Function: EXTBL_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean EXTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: EXTBL_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean EXTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);

#endif /* _extbl_ */

