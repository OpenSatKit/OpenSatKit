/*
** Purpose: KIT_TO Packet Table
**
** Notes:
**   1. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      This is a table-specific file so it doesn't need to be re-entrant.
**   2. The table file is a JSON text file.
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
#ifndef _pkttbl_
#define _pkttbl_

/*
** Includes
*/

#include "app_cfg.h"
#include "json.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define PKTTBL_APP_ID_MASK   (0x07FF)  /* CCSDS v1 ApId mask    */

#define PKTTBL_UNUSED_MSG_ID (CFE_SB_INVALID_MSG_ID)

/*
** Event Message IDs
*/

#define PKTTBL_CREATE_FILE_ERR_EID      (PKTTBL_BASE_EID + 0)
#define PKTTBL_LOAD_TYPE_ERR_EID        (PKTTBL_BASE_EID + 1)
#define PKTTBL_LOAD_EMPTY_ERR_EID       (PKTTBL_BASE_EID + 2)
#define PKTTBL_LOAD_UPDATE_ERR_EID      (PKTTBL_BASE_EID + 3)
#define PKTTBL_LOAD_OPEN_ERR_EID        (PKTTBL_BASE_EID + 4)
#define PKTTBL_LOAD_PKT_ATTR_ERR_EID    (PKTTBL_BASE_EID + 5)
#define PKTTBL_LOAD_UNDEF_FILTERS_EID   (PKTTBL_BASE_EID + 6)
#define PKTTBL_DEBUG_EID                (PKTTBL_BASE_EID + 7)


/*
** Table Structure Objects 
*/

#define  PKTTBL_OBJ_PKT       0
#define  PKTTBL_OBJ_FILTER    1
#define  PKTTBL_OBJ_CNT       2

#define  PKTTBL_OBJ_NAME_PKT     "packet"
#define  PKTTBL_OBJ_NAME_FILTER  "filter"
                                           

/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Table -  Local table copy used for table loads
** 
*/

typedef struct {

   CFE_SB_MsgId_t   StreamId;
   CFE_SB_Qos_t     Qos;
   uint16           BufLim;

   PktUtil_Filter   Filter;
   
} PKTTBL_Pkt;


typedef struct {
   
   PKTTBL_Pkt Pkt[PKTUTIL_MAX_APP_ID];

} PKTTBL_Tbl;


/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const PKTTBL_Tbl* (*PKTTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*PKTTBL_LoadTbl)(PKTTBL_Tbl* NewTbl); 

/* Table Owner's function to load a single table pkt. The JSON object/container is an array */
typedef boolean (*PKTTBL_LoadTblEntry)(uint16 PktIdx, PKTTBL_Pkt* NewPkt);   


typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   PktLoadCnt;
   uint16   FilterLoadCnt;
   uint16   CurAppId;   /* Most recent AppId processed by the callback function */
   
   PKTTBL_Tbl Tbl;

   PKTTBL_GetTblPtr    GetTblPtrFunc;
   PKTTBL_LoadTbl      LoadTblFunc;
   PKTTBL_LoadTblEntry LoadTblEntryFunc; 

   JSON_Class Json;
   JSON_Obj   JsonObj[PKTTBL_OBJ_CNT];
   char       JsonFileBuf[JSON_MAX_FILE_CHAR];   
   jsmntok_t  JsonFileTokens[JSON_MAX_FILE_TOKENS];

} PKTTBL_Class;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: PKTTBL_Constructor
**
** Initialize the Packet Table object.
**
** Notes:
**   1. The table values are not populated. This is done when the table is 
**      registered with the table manager.
*/
void PKTTBL_Constructor(PKTTBL_Class*       ObjPtr,
                        PKTTBL_GetTblPtr    GetTblPtrFunc,
                        PKTTBL_LoadTbl      LoadTblFunc, 
                        PKTTBL_LoadTblEntry LoadTblEntryFunc);


/******************************************************************************
** Function: KTTBL_SetPacketToUnused
**
*/
void PKTTBL_SetPacketToUnused(PKTTBL_Pkt* PktPtr);


/******************************************************************************
** Function: PKTTBL_LoadUnusedPacketArray
**
*/
void PKTTBL_SetTblToUnused(PKTTBL_Tbl* TblPtr);


/******************************************************************************
** Function: PKTTBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void PKTTBL_ResetStatus(void);


/******************************************************************************
** Function: PKTTBL_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean PKTTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: PKTTBL_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean PKTTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);

#endif /* _pkttbl_ */
