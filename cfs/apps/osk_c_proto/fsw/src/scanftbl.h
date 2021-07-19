/*
** Purpose: Example Scanf Table
**
** Notes:
**   1. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      This is a table-specific file so it doesn't need to be re-entrant.
**   2. The table is formatted using common format for each row similar to 
**      the cFE executive service's cfe_es_startup.scr file. 
**   3. This example illustrates how to integrate a user-defined text parser
**      into the opensat app framework. A more complex parser is needed
**      for non-homogeneous data files.
**   4. One of the goals of the object-based cFS application effort is to 
**      explore using text-based tables that are managed by the application.
**      Once a 'standard' text format is agreed upon the common utilities 
**      could be provided as libraries to the apps.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _scanftbl_
#define _scanftbl_

/*
** Includes
*/

#include "app_cfg.h"
#include "extbldata.h"

/*
** This length is after the parser has removed whitespace so it is just the
** space separated parameters
*/

#define SCANFTBL_MAX_FILE_LINE_LEN 256

/*
** Type Definitions
*/

/*
** User callback function.
**
** The character buffer is verified to be less than SCANFTBL_MAX_FILE_LINE_LEN and the
** parameters have been separate by a single space each. The easiest method for the
** user to parse the entry is using sscanf().
**
*/
typedef void (*SCANFTBL_LineParser) (char* LineBuff);

/*
** Exported Functions
*/


/*
** Event Message IDs
*/

#define SCANFTBL_CREATE_FILE_ERR_EID      (SCANFTBL_BASE_EID + 0)
#define SCANFTBL_FILE_OPEN_ERR_EID        (SCANFTBL_BASE_EID + 1)
#define SCANFTBL_FILE_READ_ERR_EID        (SCANFTBL_BASE_EID + 2)
#define SCANFTBL_CMD_LOAD_TYPE_ERR_EID    (SCANFTBL_BASE_EID + 3)
#define SCANFTBL_CMD_LOAD_PARSE_ERR_EID   (SCANFTBL_BASE_EID + 4)
#define SCANFTBL_LOAD_INDEX_ERR_EID       (SCANFTBL_BASE_EID + 5)
#define SCANFTBL_LOAD_LINE_ERR_EID        (SCANFTBL_BASE_EID + 6)
#define SCANFTBL_LINE_LEN_ERR_EID         (SCANFTBL_BASE_EID + 7)


/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const ExTblData_Param* (*SCANFTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*SCANFTBL_LoadTbl)(ExTblData_Param* NewTable); 

/* Table Owner's function to load a single table entry */
typedef boolean (*SCANFTBL_LoadTblEntry)(uint16 EntryId, ExTblData_Entry* NewEntry);   


typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   FileLineNum;
   boolean  Modified[SCANFTBL_MAX_ENTRY_ID];

   ExTblData_Param Tbl;

   SCANFTBL_GetTblPtr    GetTblPtrFunc;
   SCANFTBL_LoadTbl      LoadTblFunc;
   SCANFTBL_LoadTblEntry LoadTblEntryFunc; 

} SCANFTBL_Class;


/*
** Exported Functions
*/

/******************************************************************************
** Function: SCANFTBL_Constructor
**
** Initialize the Example Scanf Table object.
**
** Notes:
**   1. The table values are not populated. This is done when the table is 
**      registered with the table manager.
*/
void SCANFTBL_Constructor(SCANFTBL_Class* ObjPtr,
                          SCANFTBL_GetTblPtr    GetTblPtrFunc,
                          SCANFTBL_LoadTbl      LoadTblFunc, 
                          SCANFTBL_LoadTblEntry LoadTblEntryFunc);


/******************************************************************************
** Function: SCANFTBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void SCANFTBL_ResetStatus(void);


/******************************************************************************
** Function: SCANFTBL_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean SCANFTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: SCANFTBL_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean SCANFTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);

/******************************************************************************
** Purpose: Read a user supplied file, verify the text syntax, remove
**          extraneous white spaces and call the user supplied callback
**          function for each non-comment line in the file.
**
** Notes:
**   1. TextFile includes the full path and the file name.
**   2, TODO - Decide how parser callback should work. This function isn't defined.
**
** Returns the number of file entries processed.
*/
uint16 SCANFTBL_ParseFile(char* TextFile, SCANFTBL_LineParser LineParser);


#endif /* _scanftbl_ */
