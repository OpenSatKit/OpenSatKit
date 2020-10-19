/* 
** Purpose: Define KIT_SCH's Message Table that provides the messages to 
**          be sent by the scheduler.
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
#ifndef _msgtbl_
#define _msgtbl_

/*
** Includes
*/

#include "app_cfg.h"
#include "json.h"


/***********************/
/** Macro Definitions **/
/***********************/


/*
** Event Message IDs
*/

#define MSGTBL_CREATE_FILE_ERR_EID    (MSGTBL_BASE_EID + 0)
#define MSGTBL_LOAD_TYPE_ERR_EID      (MSGTBL_BASE_EID + 1)
#define MSGTBL_LOAD_EMPTY_ERR_EID     (MSGTBL_BASE_EID + 2)
#define MSGTBL_LOAD_UPDATE_ERR_EID    (MSGTBL_BASE_EID + 3)
#define MSGTBL_LOAD_OPEN_ERR_EID      (MSGTBL_BASE_EID + 4)
#define MSGTBL_LOAD_MSG_ATTR_ERR_EID  (MSGTBL_BASE_EID + 5)
#define MSGTBL_DUMP_MSG_ERR_EID       (MSGTBL_BASE_EID + 6)
#define MSGTBL_DUMP_INFO_EID          (MSGTBL_BASE_EID + 7)
#define MSGTBL_DEBUG_EID              (MSGTBL_BASE_EID + 8)


/*
** Table Structure Objects 
*/

#define  MSGTBL_OBJ_MSG    0
#define  MSGTBL_OBJ_CNT    1

#define  MSGTBL_OBJ_NAME_MSG  "message"
                                           

/**********************/
/** Type Definitions **/
/**********************/

/******************************************************************************
** Message Table -  Local table copy used for table loads
** 
*/

typedef struct {
   
   uint16   Buffer[MSGTBL_MAX_MSG_WORDS];

} MSGTBL_Entry;

typedef struct {

   MSGTBL_Entry Entry[MSGTBL_MAX_ENTRIES];

} MSGTBL_Tbl;


/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const MSGTBL_Tbl* (*MSGTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*MSGTBL_LoadTbl)(MSGTBL_Tbl* NewTbl); 

/* Table Owner's function to load a single table message entry. The JSON object/container is an array */
typedef boolean (*MSGTBL_LoadTblEntry)(uint16 MsgIdx, MSGTBL_Entry* NewMsg);   


typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   ObjErrCnt;
   uint16   ObjLoadCnt;
   
   MSGTBL_Tbl Tbl;

   MSGTBL_GetTblPtr    GetTblPtrFunc;
   MSGTBL_LoadTbl      LoadTblFunc;
   MSGTBL_LoadTblEntry LoadTblEntryFunc; 

   JSON_Class Json;
   JSON_Obj   JsonObj[MSGTBL_OBJ_CNT];
   char       JsonFileBuf[JSON_MAX_FILE_CHAR];   
   jsmntok_t  JsonFileTokens[JSON_MAX_FILE_TOKENS];

} MSGTBL_Class;


/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: MSGTBL_Constructor
**
** Initialize the Message Table object.
**
** Notes:
**   1. This must be called prior to any other function.
**   2. The local table data is not populated. This is done when the table is 
**      registered with the app framework table manager.
*/
void MSGTBL_Constructor(MSGTBL_Class*       ObjPtr,
                        MSGTBL_GetTblPtr    GetTblPtrFunc,
                        MSGTBL_LoadTbl      LoadTblFunc, 
                        MSGTBL_LoadTblEntry LoadTblEntryFunc); 


/******************************************************************************
** Function: MSGTBL_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. See the MSGTBL_Class definition for the affected data.
**
*/
void MSGTBL_ResetStatus(void);


/******************************************************************************
** Function: MSGTBL_GetMsgPtr
**
*/
boolean MSGTBL_GetMsgPtr(uint16  EntryId, uint16 **MsgPtr);


/******************************************************************************
** Function: MSGTBL_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean MSGTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: MSGTBL_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean MSGTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);


/******************************************************************************
** Function: MSGTBL_SendMsg
**
** Send a SB message containing the message table entry at location EntryId.
**
** Notes:
**  1. Range checking is performed on EntryId and an event message is sent for
**     an invalid ID.
**
*/
boolean MSGTBL_SendMsg(uint16  EntryId);


#endif /* _msgtbl_ */
