/*
** Purpose: Define Example Table
**
** Notes:
**   1. Template written by David McComas and licensed under the GNU
**      Lesser General Public License (LGPL).
**   2. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      This is a table-specific file so it doesn't need to be re-entrant.
**   3. The table is formatted using common format for each row similar to 
**      the cFE executive service's cfe_es_startup.scr file. 
**   4. This example illustrates how to integrate a user-defined text parser
**      into the opensat app framework. A more complex parser is needed
**      for non-homogeneous data files.
**   5. One of the goals of the object-based cFS application effort is to 
**      explore using text-based tables that are managed by the application.
**      Once a 'standard' text format is agreed upon the common utilities 
**      could be provided as libraries to the apps.
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _extbl_
#define _extbl_

/*
** Includes
*/

#include "app_cfg.h"


/*
** Line length is after the parser has removed whitespace so it is just the
** space separated parameters
*/

#define EXTBL_MAX_ENTRY_ID       32
#define EXTBL_MAX_FILE_LINE_LEN 256

/*
** Event Message IDs
*/

#define EXTBL_CREATE_FILE_ERR_EID      (EXTBL_BASE_EID + 0)
#define EXTBL_FILE_OPEN_ERR_EID        (EXTBL_BASE_EID + 1)
#define EXTBL_FILE_READ_ERR_EID        (EXTBL_BASE_EID + 2)
#define EXTBL_CMD_LOAD_TYPE_ERR_EID    (EXTBL_BASE_EID + 3)
#define EXTBL_CMD_LOAD_PARSE_ERR_EID   (EXTBL_BASE_EID + 4)
#define EXTBL_LOAD_INDEX_ERR_EID       (EXTBL_BASE_EID + 5)
#define EXTBL_LOAD_LINE_ERR_EID        (EXTBL_BASE_EID + 6)
#define EXTBL_LINE_LEN_ERR_EID         (EXTBL_BASE_EID + 7)

/*
** Type Definitions
*/

/******************************************************************************
** Packet Table
*/

typedef struct {

   uint16   Data1;
   uint16   Data2;
   uint16   Data3;

} EXTBL_Entry;

typedef struct
{

   EXTBL_Entry Entry[EXTBL_MAX_ENTRY_ID];

} EXTBL_Data;


/******************************************************************************
** Table owner callback: Load entire table
**
*/
typedef boolean (*EXTBL_LoadTblFunc) (EXTBL_Data* TblData);

/******************************************************************************
** Table owner callback: Load a single table entry
**
*/
typedef boolean (*EXTBL_LoadEntryFunc) (uint16 EntryId, EXTBL_Entry* TblEntry);

/******************************************************************************
** Table owner callback: Get pointer to table owner's data
**
*/
typedef EXTBL_Data* (*EXTBL_GetDataPtrFunc) (void);

/******************************************************************************
**  Local table data copy used for table load command
**
*/
typedef struct {

   boolean     LastLoadValid;
   uint16      ErrCnt;
   uint16      FileLineNum;

   EXTBL_LoadTblFunc    LoadTbl;
   EXTBL_LoadEntryFunc  LoadEntry;
   EXTBL_GetDataPtrFunc GetDataPtr;
   
   EXTBL_Data  Data;
   boolean     Modified[EXTBL_MAX_ENTRY_ID];

} EXTBL_Class;


/*
** Exported Functions
*/

/******************************************************************************
** Function: EXTBL_Constructor
**
** Initialize the example table object.
**
*/
void EXTBL_Constructor(EXTBL_Class* TblObj, 
                       EXTBL_LoadTblFunc LoadTbl, 
                       EXTBL_LoadEntryFunc LoadEntry,
                       EXTBL_GetDataPtrFunc GetDataPtr);

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
