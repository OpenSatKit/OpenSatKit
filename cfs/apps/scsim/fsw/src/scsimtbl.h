/*
** Purpose: Define SCSIM Table
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
#ifndef _scsimtbl_
#define _scsimtbl_

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

#define SCSIMTBL_CREATE_FILE_ERR_EID       (SCSIMTBL_BASE_EID +  0)
#define SCSIMTBL_CMD_LOAD_TYPE_ERR_EID     (SCSIMTBL_BASE_EID +  1)
#define SCSIMTBL_CMD_LOAD_OPEN_ERR_EID     (SCSIMTBL_BASE_EID +  2)
#define SCSIMTBL_CMD_LOAD_REPLACE_ERR_EID  (SCSIMTBL_BASE_EID +  3)
#define SCSIMTBL_CMD_LOAD_UPDATE_ERR_EID   (SCSIMTBL_BASE_EID +  4)
#define SCSIMTBL_LOAD_ACS_ERR_EID          (SCSIMTBL_BASE_EID +  5)
#define SCSIMTBL_LOAD_CDH_ERR_EID          (SCSIMTBL_BASE_EID +  6)
#define SCSIMTBL_LOAD_COMM_ERR_EID         (SCSIMTBL_BASE_EID +  7)
#define SCSIMTBL_LOAD_FSW_ERR_EID          (SCSIMTBL_BASE_EID +  8)
#define SCSIMTBL_LOAD_PWR_ERR_EID          (SCSIMTBL_BASE_EID +  9)
#define SCSIMTBL_LOAD_THRM_ERR_EID         (SCSIMTBL_BASE_EID + 10)


/*
** Table Structure Objects 
*/

#define  SCSIMTBL_OBJ_ACS   0
#define  SCSIMTBL_OBJ_CDH   1
#define  SCSIMTBL_OBJ_COMM  2
#define  SCSIMTBL_OBJ_FSW   3
#define  SCSIMTBL_OBJ_PWR   4
#define  SCSIMTBL_OBJ_THRM  5
#define  SCSIMTBL_OBJ_CNT   6

#define  SCSIMTBL_OBJ_ACS_NAME   "acs"
#define  SCSIMTBL_OBJ_CDH_NAME   "cdh"
#define  SCSIMTBL_OBJ_COMM_NAME  "comm"
#define  SCSIMTBL_OBJ_FSW_NAME   "fsw"
#define  SCSIMTBL_OBJ_PWR_NAME   "pwr"
#define  SCSIMTBL_OBJ_THRM_NAME  "thrm"

\
/*
** Type Definitions
*/


/******************************************************************************
** Table -  Local table copy used for table loads
** 
*/

typedef struct {

   uint32  Tbd1;
   uint32  Tbd2;
   
} SCSIMTBL_Acs;


typedef struct {

   uint32  Tbd1;
   uint32  Tbd2;
   
} SCSIMTBL_Cdh;

typedef struct {

   uint32  Tbd1;
   uint32  Tbd2;
   
} SCSIMTBL_Comm;

typedef struct {

   uint32  Tbd1;
   uint32  Tbd2;
   
} SCSIMTBL_Fsw;

typedef struct {

   uint32  Tbd1;
   uint32  Tbd2;
   
} SCSIMTBL_Pwr;

typedef struct {

   uint32  Tbd1;
   uint32  Tbd2;
   
} SCSIMTBL_Thrm;


typedef struct {

   SCSIMTBL_Acs   Acs;
   SCSIMTBL_Acs   Cdh;
   SCSIMTBL_Acs   Comm;
   SCSIMTBL_Acs   Fsw;
   SCSIMTBL_Acs   Pwr;
   SCSIMTBL_Acs   Thrm;

} SCSIMTBL_Struct;

/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const SCSIMTBL_Struct* (*SCSIMTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*SCSIMTBL_LoadTbl)(SCSIMTBL_Struct* NewTable); 

/* Table Owner's function to load a single table JSON object/container */
typedef boolean (*SCSIMTBL_LoadTblEntry)(uint16 ObjId, void* ObjData);   


typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   ObjLoadCnt;

   SCSIMTBL_Struct Data;

   SCSIMTBL_GetTblPtr    GetTblPtrFunc;
   SCSIMTBL_LoadTbl      LoadTblFunc;
   SCSIMTBL_LoadTblEntry LoadTblEntryFunc; 

   JSON_Class Json;
   JSON_Obj   JsonObj[SCSIMTBL_OBJ_CNT];
   char       JsonFileBuf[JSON_MAX_FILE_CHAR];   
   jsmntok_t  JsonFileTokens[JSON_MAX_FILE_TOKENS];

} SCSIMTBL_Class;



/*
** Exported Functions
*/


/******************************************************************************
** Function: SCSIMTBL_Constructor
**
** Initialize the example table object.
**
** Notes:
**   1. The table values are not populated. This is done when the table is 
**      registered with the table manager.
**
*/
void SCSIMTBL_Constructor(SCSIMTBL_Class* TblObj, 
                          SCSIMTBL_GetTblPtr    GetTblPtrFunc,
                          SCSIMTBL_LoadTbl      LoadTblFunc, 
                          SCSIMTBL_LoadTblEntry LoadTblEntryFunc);


/******************************************************************************
** Function: SCSIMTBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void SCSIMTBL_ResetStatus(void);


/******************************************************************************
** Function: SCSIMTBL_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean SCSIMTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: SCSIMTBL_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean SCSIMTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);


#endif /* _scsimtbl_ */



