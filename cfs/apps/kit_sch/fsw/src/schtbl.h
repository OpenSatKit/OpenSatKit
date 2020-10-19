/* 
** Purpose: Define KIT_SCH's Scheduler Table that defiines when messages
**          defined in the message table are sent on the software bus.
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
#ifndef _schtbl_
#define _schtbl_

/*
** Includes
*/

#include "app_cfg.h"
#include "json.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define SCHTBL_UNDEF_SLOT 9999

#define SCHTBL_INDEX(slot_index,entry_index)  ((slot_index*SCHTBL_ACTIVITIES_PER_SLOT) + entry_index)

#define SCHTBL_CREATE_FILE_ERR_EID   (SCHTBL_BASE_EID + 0)
#define SCHTBL_LOAD_TYPE_ERR_EID     (SCHTBL_BASE_EID + 1)
#define SCHTBL_LOAD_EMPTY_ERR_EID    (SCHTBL_BASE_EID + 2)
#define SCHTBL_LOAD_ATTR_ERR_EID     (SCHTBL_BASE_EID + 3)
#define SCHTBL_LOAD_OPEN_ERR_EID     (SCHTBL_BASE_EID + 4)
#define SCHTBL_LOAD_PARSE_ERR_EID    (SCHTBL_BASE_EID + 5)
#define SCHTBL_DUMP_INFO_EID         (SCHTBL_BASE_EID + 6)
#define SCHTBL_MSG_TBL_INDEX_ERR_EID (SCHTBL_BASE_EID + 7)
#define SCHTBL_OFFSET_ERR_EID        (SCHTBL_BASE_EID + 8)
#define SCHTBL_ENABLED_ERR_EID       (SCHTBL_BASE_EID + 9)
#define SCHTBL_CMD_SLOT_ERR_EID      (SCHTBL_BASE_EID + 10)
#define SCHTBL_CMD_ACTIVITY_ERR_EID  (SCHTBL_BASE_EID + 11)


/*
** Table Structure Objects 
*/

#define  SCHTBL_OBJ_SLOT        0
#define  SCHTBL_OBJ_ACTIVITY    1
#define  SCHTBL_OBJ_CNT         2

#define  SCHTBL_OBJ_NAME_SLOT      "slot"
#define  SCHTBL_OBJ_NAME_ACTIVITY  "activity"
                                           

/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Scheduler Table
**
** - Minimized SCHTBL_Entry and made word-aligned for telemetry 
*/


typedef struct {

   boolean Enabled;
   uint8   Period;
   uint8   Offset;
   uint8   MsgTblIndex;

} SCHTBL_Entry;

typedef struct {

   SCHTBL_Entry Entry[SCHTBL_MAX_ENTRIES];

} SCHTBL_Tbl;

/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const SCHTBL_Tbl* (*SCHTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*SCHTBL_LoadTbl)(SCHTBL_Tbl* NewTable); 

/* Table Owner's function to load a single table entry */
typedef boolean (*SCHTBL_LoadTblEntry)(uint16 EntryId, SCHTBL_Entry* NewEntry);   


/*
** Local table copy used for table load command
*/

typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   ObjErrCnt;
   uint16   ObjLoadCnt;
   uint16   CurSlotIdx;

   SCHTBL_Tbl Tbl;
   
   SCHTBL_GetTblPtr    GetTblPtrFunc;
   SCHTBL_LoadTbl      LoadTblFunc;
   SCHTBL_LoadTblEntry LoadTblEntryFunc; 


   JSON_Class Json;
   JSON_Obj   JsonObj[SCHTBL_OBJ_CNT];
   char       JsonFileBuf[JSON_MAX_FILE_CHAR];   
   jsmntok_t  JsonFileTokens[JSON_MAX_FILE_TOKENS];

   
} SCHTBL_Class;

/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: SCHTBL_Constructor
**
** Initialize a new scheduler table instance.
**
** Notes:
**   1. This method must be called prior to all other methods. The SchTbl
**      instance variable only needs to be passed to the constructor
**      because a reference is stored by schtbl.c.
**
*/
void SCHTBL_Constructor(SCHTBL_Class*       ObjPtr,
                        SCHTBL_GetTblPtr    GetTblPtrFunc,
                        SCHTBL_LoadTbl      LoadTblFunc, 
                        SCHTBL_LoadTblEntry LoadTblEntryFunc);   


/******************************************************************************
** Function: SCHTBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of 
** the scheduler should not be impacted. The intent is to clear counters/flags 
** for telemetry.
**
** Notes:
**   1. See the SCHTBL_Class definition for the affected data.
**
*/
void SCHTBL_ResetStatus(void);


/******************************************************************************
** Function: SCHTBL_ProcessTable
**
** Process the scheduler table performing activities.
**
** Notes:
**   None
**
*/
boolean SCHTBL_ProcessTable(void);


/******************************************************************************
** Function: SCHTBL_StartTimers
**
** Notes:
**   None
**
*/
int32 SCHTBL_StartTimers(void);


/******************************************************************************
** Function: SCHTBL_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean SCHTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: SCHTBL_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean SCHTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);


/******************************************************************************
** SCHTBL_GetEntryIndex
**
** Compute and load EntryIndex if the SlotIndex and ActivityIndex are valid.
** Event message text assumes commands are being validated 
*/
boolean SCHTBL_GetEntryIndex(const char* EventStr, uint16 SlotIndex, 
                             uint16 ActivityIndex, uint16* EntryIndex);


/******************************************************************************
** Function: SCHTBL_ValidEntry
**
** Validate table entry fields. A pointer to a structure isn't passed because
** this function is used to validate command and table parametetrs that may not
** be packed identically to the internal structure.
**
** The event string should identify the calling context such as whcih ground
** command.
*/
boolean SCHTBL_ValidEntry(const char* EventStr, uint16 Enabled, uint16 Period, 
                          uint16 Offset, uint16 MsgTblIndex);
                          
#endif /* _schtbl_ */
