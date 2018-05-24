/* 
** Purpose: Define the Scheduler Table that defiines the activities to 
**          be performed by teh scheduler app.
**
** Notes:
**   1. The tables are formatted using XML and reply on the open source EXPAT
**      library.  This is a prototype and EXPAT has not been tested so it is
**      not recommended that it be used on a flight project until it has been
**      formally tested.
**   2. This design intentionally decouples the scheduler table from application
**      specific processing such as command callback functions and file processing.
**   3. The OpenSat application framework defines the table load/dump commands
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

#include <expat.h>
#include "app_cfg.h"

/*
**  Macro Definitions
*/

#define SCHTBL_INDEX(slot,entry)  ((slot*SCHTBL_ENTRIES_PER_SLOT) + entry)

#define SCHTBL_CREATE_PARSER_ERR_EID    (SCHTBL_BASE_EID + 0)
#define SCHTBL_CMD_LOAD_TYPE_ERR_EID    (SCHTBL_BASE_EID + 1)
#define SCHTBL_CMD_LOAD_PARSE_ERR_EID   (SCHTBL_BASE_EID + 2)
#define SCHTBL_CMD_DUMP_INFO_EID        (SCHTBL_BASE_EID + 3)
#define SCHTBL_FILE_READ_ERR_EID        (SCHTBL_BASE_EID + 4)
#define SCHTBL_FILE_OPEN_ERR_EID        (SCHTBL_BASE_EID + 5)
#define SCHTBL_PARSE_ERR_EID            (SCHTBL_BASE_EID + 6)
#define SCHTBL_WRITE_CFE_HDR_ERR_EID    (SCHTBL_BASE_EID + 7)
#define SCHTBL_CREATE_DUMP_FILE_ERR_EID (SCHTBL_BASE_EID + 8)

/*
** Type Definitions
*/

/******************************************************************************
** Scheduler Table
*/

typedef struct {

   boolean Enabled;
   uint16  Frequency;
   uint16  Offset;
   uint16  MsgTblEntryId;

} SCHTBL_Entry;

typedef struct {

   SCHTBL_Entry Entry[SCHTBL_MAX_ENTRIES];

} SCHTBL_Struct;

/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const SCHTBL_Struct* (*SCHTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*SCHTBL_LoadTbl)(SCHTBL_Struct* NewTable); 

/* Table Owner's function to load a single table entry */
typedef boolean (*SCHTBL_LoadTblEntry)(uint16 EntryId, SCHTBL_Entry* NewEntry);   


/*
**  Local table copy used for table load command
*/

typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   boolean  Modified[SCHTBL_MAX_ENTRIES];

   SCHTBL_Struct Tbl;
   
   SCHTBL_GetTblPtr    GetTblPtrFunc;
   SCHTBL_LoadTbl      LoadTblFunc;
   SCHTBL_LoadTblEntry LoadTblEntryFunc; 
   
} SCHTBL_Class;

/*
** Exported Functions
*/


/******************************************************************************
** Function: SCHTBL_Constructor
**
** This method initializes a new scheduler table instance.
**
** Notes:
**   1. This method must be called prior to all other methods. The SchTbl
**      instance variable only needs to be passed to the constructor
**      because a reference is stored by schtbl.c.
**
*/
void SCHTBL_Constructor(SCHTBL_Class* ObjPtr,
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
**   1. See the SCHTBL_Class definition for the effected data.
**
*/
void SCHTBL_ResetStatus(void);


/******************************************************************************
** Function: SCHTBL_ProcessTable
**
** Process the scheduler table performing activities.
**
** Notes:
**   1.
**
*/
boolean SCHTBL_ProcessTable(void);


/******************************************************************************
** Function: SCHTBL_StartTimers
**
** Notes:
**   1.
**
*/
int32 SCHTBL_StartTimers(void);


/******************************************************************************
** Function: SCHTBL_ConfigureTableEntry
**
** Configure (enable/disable) a single entry
**
** Notes:
**   1. Range checking is not performed on the parameters.
**
*/
void SCHTBL_ConfigureTableEntry(uint16 SlotId, uint16 SlotEntry, boolean EnableFlag);


/*#############################################################################
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


/*#############################################################################
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


#endif /* _schtbl_ */
