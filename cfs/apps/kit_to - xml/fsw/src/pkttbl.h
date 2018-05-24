/* 
** Purpose: Manage KIT_TO's packet table.
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

#ifndef _pkttbl_
#define _pkttbl_

/*
** Includes
*/

#include <expat.h>
#include "app_cfg.h"

/*#############################################################################
** XML tag definitions.
**
** XML Elements/Attributes used by the packet table.
*/

#define PKTTBL_XML_LEVEL_INDENT 3
#define PKTTBL_XML_MAX_KW_LEN   128   /* Maximum keyword length in bytes */

#define PKTTBL_XML_EL_PKT_TBL      "pkt-tbl"
#define PKTTBL_XML_EL_PKT_ENTRY    "entry"

#define PKTTBL_XML_AT_STREAM_ID    "stream-id"
#define PKTTBL_XML_AT_PRIORITY     "priority"
#define PKTTBL_XML_AT_RELIABILITY  "reliability"
#define PKTTBL_XML_AT_BUF_LIM      "buf-limit"

#define PKTTBL_BUFFSIZE  8192


/*
** Event Message IDs
*/

#define PKTTBL_CREATE_PARSER_ERR_EID    (PKTTBL_BASE_EID + 0)
#define PKTTBL_CMD_LOAD_TYPE_ERR_EID    (PKTTBL_BASE_EID + 1)
#define PKTTBL_CMD_LOAD_PARSE_ERR_EID   (PKTTBL_BASE_EID + 2)
#define PKTTBL_CMD_DUMP_INFO_EID        (PKTTBL_BASE_EID + 3)
#define PKTTBL_FILE_READ_ERR_EID        (PKTTBL_BASE_EID + 4)
#define PKTTBL_FILE_OPEN_ERR_EID        (PKTTBL_BASE_EID + 5)
#define PKTTBL_PARSE_ERR_EID            (PKTTBL_BASE_EID + 6)
#define PKTTBL_WRITE_CFE_HDR_ERR_EID    (PKTTBL_BASE_EID + 7)
#define PKTTBL_CREATE_MSG_DUMP_ERR_EID  (PKTTBL_BASE_EID + 8)


/*
** Type Definitions
*/

/******************************************************************************
** Packet Table
*/

typedef struct {

   CFE_SB_MsgId_t   StreamId;
   CFE_SB_Qos_t     Qos;
   uint16           BuffLim;

} PKTTBL_Entry;

typedef struct
{
   PKTTBL_Entry Entry[PKTTBL_MAX_ENTRY_ID];

} PKTTBL_Struct;

/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const PKTTBL_Struct* (*PKTTBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*PKTTBL_LoadTbl)(PKTTBL_Struct* NewTable); 

/* Table Owner's function to load a single table entry */
typedef boolean (*PKTTBL_LoadTblEntry)(uint16 EntryId, PKTTBL_Entry* NewEntry);   


/*
**  Local table copy used for table load command
*/
typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   boolean  Modified[PKTTBL_MAX_ENTRY_ID];

   PKTTBL_Struct Tbl;

   PKTTBL_GetTblPtr    GetTblPtrFunc;
   PKTTBL_LoadTbl      LoadTblFunc;
   PKTTBL_LoadTblEntry LoadTblEntryFunc; 
   
} PKTTBL_Class;


/*
** Exported Functions
*/

/******************************************************************************
** Function: PKTTBL_Constructor
**
** Initialize the Packet Table object.
**
*/
void PKTTBL_Constructor(PKTTBL_Class* ObjPtr,
                        PKTTBL_GetTblPtr    GetTblPtrFunc,
                        PKTTBL_LoadTbl      LoadTblFunc, 
                        PKTTBL_LoadTblEntry LoadTblEntryFunc);   


/******************************************************************************
** Function: PKTTBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavour of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void PKTTBL_ResetStatus(void);


/*#############################################################################
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


/*#############################################################################
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
