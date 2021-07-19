/*
** Purpose: Define ISIM Table
**
** Notes:
**   1. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      This is a table-specific file so it doesn't need to be re-entrant.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _isimtbl_
#define _isimtbl_

/*
** Includes
*/

#include "app_cfg.h"


/*
** Macro Definitions
*/

/*
** Event Message IDs
*/

#define ISIMTBL_CREATE_FILE_ERR_EID        (ISIMTBL_BASE_EID + 0)
#define ISIMTBL_LOAD_INDEX_ERR_EID         (ISIMTBL_BASE_EID + 1)
#define ISIMTBL_LOAD_LINE_ERR_EID          (ISIMTBL_BASE_EID + 2)
#define ISIMTBL_CMD_LOAD_TYPE_ERR_EID      (ISIMTBL_BASE_EID + 3)
#define ISIMTBL_CMD_LOAD_PARSE_ERR_EID     (ISIMTBL_BASE_EID + 4)
#define ISIMTBL_CMD_LOAD_OPEN_ERR_EID      (ISIMTBL_BASE_EID + 5)
#define ISIMTBL_CMD_LOAD_REPLACE_ERR_EID   (ISIMTBL_BASE_EID + 6)
#define ISIMTBL_CMD_LOAD_UPDATE_ERR_EID    (ISIMTBL_BASE_EID + 7)
#define ISIMTBL_LOAD_SCI_INSTR_ERR_EID     (ISIMTBL_BASE_EID + 8)
#define ISIMTBL_LOAD_SCI_FILE_ERR_EID      (ISIMTBL_BASE_EID + 9)

/*
** Table Structure Objects 
*/

#define  ISIMTBL_OBJ_SCI_INSTR  0
#define  ISIMTBL_OBJ_SCI_FILE   1
#define  ISIMTBL_OBJ_CNT        2

#define  ISIMTBL_OBJ_SCI_INSTR_NAME  "sci-instr"
#define  ISIMTBL_OBJ_SCI_FILE_NAME   "sci-file"

/*
** Type Definitions
*/

/******************************************************************************
** Table -  Local table copy used for table loads
** 
*/

typedef struct {

   uint16   PwrInitCycles;
   uint16   PwrResetCycles;
   
} ISIMTBL_SciInstr;

typedef struct {

   char     PathBaseFilename[OS_MAX_PATH_LEN];
   char     FileExtension[ISIM_FILE_EXT_MAX_CHAR];
   uint16   ImagesPerFile;

} ISIMTBL_SciFile;


typedef struct {

   ISIMTBL_SciInstr  SciInstr;
   ISIMTBL_SciFile   SciFile;

} ISIMTBL_Struct;

/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const ISIMTBL_Struct* (*ISIMTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*ISIMTBL_LoadTbl)(ISIMTBL_Struct* NewTable); 

/* Table Owner's function to load a single table JSON object/container */
typedef boolean (*ISIMTBL_LoadTblEntry)(uint16 ObjId, void* ObjData);   


typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   ObjLoadCnt;

   ISIMTBL_Struct Data;

   ISIMTBL_GetTblPtr    GetTblPtrFunc;
   ISIMTBL_LoadTbl      LoadTblFunc;
   ISIMTBL_LoadTblEntry LoadTblEntryFunc; 

   JSON_Class Json;
   JSON_Obj   JsonObj[ISIMTBL_OBJ_CNT];
   char       JsonFileBuf[JSON_MAX_FILE_CHAR];   
   jsmntok_t  JsonFileTokens[JSON_MAX_FILE_TOKENS];

} ISIMTBL_Class;



/*
** Exported Functions
*/


/******************************************************************************
** Function: ISIMTBL_Constructor
**
** Initialize the example table object.
**
** Notes:
**   1. The table values are not populated. This is done when the table is 
**      registered with the table manager.
**
*/
void ISIMTBL_Constructor(ISIMTBL_Class* TblObj, 
                         ISIMTBL_GetTblPtr    GetTblPtrFunc,
                         ISIMTBL_LoadTbl      LoadTblFunc, 
                         ISIMTBL_LoadTblEntry LoadTblEntryFunc);


/******************************************************************************
** Function: ISIMTBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void ISIMTBL_ResetStatus(void);


/******************************************************************************
** Function: ISIMTBL_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean ISIMTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: ISIMTBL_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean ISIMTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);


#endif /* _isimtbl_ */



