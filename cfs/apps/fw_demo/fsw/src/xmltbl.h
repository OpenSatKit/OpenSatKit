/*
** Purpose: Example XML Table
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**   2. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      This is a table-specific file so it doesn't need to be re-entrant.
**   3. The tables are formatted using XML and rely on the open source expat
**      library. This library has NOT been verified for flight usage. 
**   4. One of the goals of the object-based cFS application effort is to 
**      explore using text-based tables that are managed by the application.
**      Once a 'standard' text format is agreed upon the common utilities 
**      could be provided as libraries to the apps. This is a prototype
**      using XML. The app-specific changes have comment blocks starting 
**      pound signs (#). There are multiple limitations to this design that
**      would need to be address to make it more reusable. For starters a
**      container should be used to allow multiple tables to be used by an
**      app and the app-specific functions should be in separate files so 
**      so the reusable files do not need to be modified by the user. Also
**      the reusable portions should be in a separate file.
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _xmltbl_
#define _xmltbl_

/*
** Includes
*/

#include <expat.h>
#include "app_cfg.h"

/*#############################################################################
** XML tag definitions.
**
** XML Elements/Attributes used by the example table. Table keywords could be
** standardized and imported to increase reuse
*/

#define XMLTBL_XML_EL_PKT_ENTRY  "entry"

#define XMLTBL_XML_AT_INDEX   "index"
#define XMLTBL_XML_AT_DATA1   "data1"
#define XMLTBL_XML_AT_DATA2   "data2"
#define XMLTBL_XML_AT_DATA3   "data3"

#define XMLTBL_BUFFSIZE  8192

/*
** Event Message IDs
*/

#define XMLTBL_CREATE_ERR_EID           (XMLTBL_BASE_EID + 0)
#define XMLTBL_FILE_OPEN_ERR_EID        (XMLTBL_BASE_EID + 1)
#define XMLTBL_FILE_READ_ERR_EID        (XMLTBL_BASE_EID + 2)
#define XMLTBL_PARSE_ERR_EID            (XMLTBL_BASE_EID + 3)
#define XMLTBL_WRITE_CFE_HDR_ERR_EID    (XMLTBL_BASE_EID + 4)
#define XMLTBL_CREATE_MSG_DUMP_ERR_EID  (XMLTBL_BASE_EID + 5)
#define XMLTBL_CMD_LOAD_TYPE_ERR_EID    (XMLTBL_BASE_EID + 6)
#define XMLTBL_CMD_LOAD_PARSE_ERR_EID   (XMLTBL_BASE_EID + 7)



/******************************************************************************
** Table -  Local table copy used for table loads
**
** All of the demo tables must have the same structure.
*/

typedef struct {

   uint16   Data1;
   uint16   Data2;
   uint16   Data3;

} XMLTBL_Entry;

typedef struct
{

   XMLTBL_Entry Entry[XMLTBL_MAX_ENTRY_ID];

} XMLTBL_Struct;


/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const XMLTBL_Struct* (*XMLTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*XMLTBL_LoadTbl)(XMLTBL_Struct* NewTable); 

/* Table Owner's function to load a single table entry */
typedef boolean (*XMLTBL_LoadTblEntry)(uint16 EntryId, XMLTBL_Entry* NewEntry);   

typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   boolean  Modified[XMLTBL_MAX_ENTRY_ID];

   XMLTBL_Struct Tbl;

   XMLTBL_GetTblPtr    GetTblPtrFunc;
   XMLTBL_LoadTbl      LoadTblFunc;
   XMLTBL_LoadTblEntry LoadTblEntryFunc; 

} XMLTBL_Class;

/*
** Exported Functions
*/

/******************************************************************************
** Function: XMLTBL_Constructor
**
** Initialize the Example XML Table object.
**
** Notes:
**   1. The table values are not populated. This is done when the table is 
**      registered with the table manager.
*/
void XMLTBL_Constructor(XMLTBL_Class* ObjPtr,
                        XMLTBL_GetTblPtr    GetTblPtrFunc,
                        XMLTBL_LoadTbl      LoadTblFunc, 
                        XMLTBL_LoadTblEntry LoadTblEntryFunc);


/******************************************************************************
** Function: XMLTBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavour of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void XMLTBL_ResetStatus(void);


/*#############################################################################
** Function: XMLTBL_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean XMLTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/*#############################################################################
** Function: XMLTBL_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean XMLTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);

#endif /* _xmltbl_ */
