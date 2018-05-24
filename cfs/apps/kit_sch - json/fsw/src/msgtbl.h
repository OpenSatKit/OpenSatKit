/* 
** Purpose: Define the Message Table that provides the messages to be sent
**          by the scheduler.
**
** Notes:
**   1. The tables are formatted using XML and reply on the open source EXPAT
**      library.  This is a prototype and EXPAT has not been tested so it is
**      not recommended that it be used on a flight project until it has been
**      formally tested.
**   2. The OpenSat application framework defines the table load/dump commands
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

#include <expat.h>
#include "app_cfg.h"

/*
** Event Message IDs
*/

#define MSGTBL_CREATE_PARSER_ERR_EID    (MSGTBL_BASE_EID + 0)
#define MSGTBL_CMD_LOAD_TYPE_ERR_EID    (MSGTBL_BASE_EID + 1)
#define MSGTBL_CMD_LOAD_PARSE_ERR_EID   (MSGTBL_BASE_EID + 2)
#define MSGTBL_CMD_DUMP_INFO_EID        (MSGTBL_BASE_EID + 3)
#define MSGTBL_FILE_READ_ERR_EID        (MSGTBL_BASE_EID + 4)
#define MSGTBL_FILE_OPEN_ERR_EID        (MSGTBL_BASE_EID + 5)
#define MSGTBL_PARSE_ERR_EID            (MSGTBL_BASE_EID + 6)
#define MSGTBL_WRITE_CFE_HDR_ERR_EID    (MSGTBL_BASE_EID + 7)
#define MSGTBL_CREATE_MSG_DUMP_ERR_EID  (MSGTBL_BASE_EID + 8)

#define MSGTBL_SB_SEND_ERR_EID          (MSGTBL_BASE_EID + 9)

/*
** Type Definitions
*/


/******************************************************************************
** Message Table
*/

typedef struct
{
    uint16   Buffer[MSGTBL_MAX_MSG_WORDS];

} MSGTBL_Entry;

typedef struct {

   MSGTBL_Entry Entry[MSGTBL_MAX_ENTRY_ID];

} MSGTBL_Struct;

/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const MSGTBL_Struct* (*MSGTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*MSGTBL_LoadTbl)(MSGTBL_Struct* NewTable); 

/* Table Owner's function to load a single table entry */
typedef boolean (*MSGTBL_LoadTblEntry)(uint16 EntryId, MSGTBL_Entry* NewEntry);   

/*
**  Local table copy used for table load command
*/
typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   boolean  Modified[MSGTBL_MAX_ENTRY_ID];

   MSGTBL_Struct Tbl;

   MSGTBL_GetTblPtr    GetTblPtrFunc;
   MSGTBL_LoadTbl      LoadTblFunc;
   MSGTBL_LoadTblEntry LoadTblEntryFunc; 
   
} MSGTBL_Class;


/*
** Exported Functions
*/

/******************************************************************************
** Function: MSGTBL_Constructor
**
** Initialize the Message Table object.
**
** Notes:
**   1. This must be called prior to any other function.
**   2. The local tabel data is not populated. Thsi can be done when the
**      table is registered with the app framework TBLMGR.
*/
void MSGTBL_Constructor(MSGTBL_Class* ObjPtr,
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


/*#############################################################################
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


/*#############################################################################
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

#endif /* _msgtbl_ */
