/*
** $Id: $
** 
** Purpose: Provide a class to manage the scheduler lab tables
**
** Notes:
**   1. The tables are formatted using XML and reply on the open source expat library.
**
** References:
**   1. Core Flight Executive Application Developers Guide.
**
** $Date: $
** $Revision: $
** $Log: $
**
*/

#ifndef _tblmgr_
#define _tblmgr_

/*
** Includes
*/

#include <expat.h>
#include "app_config.h"
#include "msgtbl.h"
#include "schtbl.h"

/*
** XML tag definitions. Attribute names don't distinguish between scheduler
** table or message table so they use consistent names.
*/
#define TBLMGR_XML_LEVEL_INDENT 3
#define TBLMGR_XML_MAX_KW_LEN   128   /* Maximum keyword length in bytes */

#define TBLMGR_XML_EL_SCH_TABLE  "sch-tbl"
#define TBLMGR_XML_EL_SCH_SLOT   "slot"
#define TBLMGR_XML_EL_MSG_TABLE  "msg-tbl"
#define TBLMGR_XML_EL_MSG_ENTRY  "entry"

#define TBLMGR_XML_AT_ID         "id"
#define TBLMGR_XML_AT_ENTRY      "entry"
#define TBLMGR_XML_AT_ENABLE     "enable"
#define TBLMGR_XML_AT_FREQ       "frequency"
#define TBLMGR_XML_AT_OFFSET     "offset"
#define TBLMGR_XML_AT_MSG_ID     "msg_id"
#define TBLMGR_XML_AT_STREAM_ID  "stream-id"
#define TBLMGR_XML_AT_SEQ_SEG    "seq-seg"
#define TBLMGR_XML_AT_MSG_LEN    "length"

#define TBLMGR_XML_AT_VAL_TRUE   "true"
#define TBLMGR_XML_AT_VAL_FALSE  "false"

#define TBLMGR_BUFFSIZE  8192

/*
** TBLMGR_LoadTblCmd Load Type options
*/
#define TBLMGR_LOAD_TBL_REPLACE    0
#define TBLMGR_LOAD_TBL_UPDATE     1

#define TBLMGR_CFE_DUMP_FILE_EXT   ".dmp"
#define TBLMGR_XML_DUMP_FILE_EXT   ".xml"

/*
** Event Message IDs
*/

#define TBLMGR_CREATE_ERR_EID           (TBLMGR_BASE_EID + 0)
#define TBLMGR_FILE_OPEN_ERR_EID        (TBLMGR_BASE_EID + 1)
#define TBLMGR_FILE_READ_ERR_EID        (TBLMGR_BASE_EID + 2)
#define TBLMGR_PARSE_ERR_EID            (TBLMGR_BASE_EID + 3)
#define TBLMGR_WRITE_CFE_HDR_ERR_EID    (TBLMGR_BASE_EID + 4)
#define TBLMGR_CREATE_MSG_DUMP_ERR_EID  (TBLMGR_BASE_EID + 5)
#define TBLMGR_CMD_DUMP_INFO_EID        (TBLMGR_BASE_EID + 6)

#define TBLMGR_TOTAL_EID  7

/*
** Type Definitions
*/

typedef struct
{
   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /* cFE Software Bus Command Message Header     */
   uint16   Slot;
   uint16   EntryInSlot;
   boolean  ConfigFlag;   /* 0=Disable, 1=Enable */

} TBLMGR_ConfigSchCmd;
#define TBLMGR_CONFIG_SCH_CMD_DATA_LEN  (sizeof(TBLMGR_DumpTblCmd) - CFE_SB_CMD_HDR_SIZE)

typedef struct
{
   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /* cFE Software Bus Command Message Header     */
   char    FileName[OS_MAX_PATH_LEN];      /* ASCII text string of full path and filename */
   uint16  LoadType;                       /* Replace or update table records             */

} TBLMGR_LoadTblCmd;
#define TBLMGR_LOAD_TBL_CMD_DATA_LEN  (sizeof(TBLMGR_LoadTblCmd) - CFE_SB_CMD_HDR_SIZE)

typedef struct
{
   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE]; /* cFE Software Bus Command Message Header    */
   char    FileName[OS_MAX_PATH_LEN];      /*ASCII text string of full path and filename */

} TBLMGR_DumpTblCmd;
#define TBLMGR_DUMP_TBL_CMD_DATA_LEN  (sizeof(TBLMGR_DumpTblCmd) - CFE_SB_CMD_HDR_SIZE)


/*
**  Local table copies used for table load commands
*/
typedef struct {

   boolean       LoadActive;
   uint8         LastLoadStatus;
   uint16        AttrErrCnt;
   MSGTBL_Table  Local;
   boolean       Modified[MSGTBL_MAX_ENTRY_ID];

} TBLMGR_MsgTbl;

typedef struct {

   boolean       LoadActive;
   uint8         LastLoadStatus;
   uint16        AttrErrCnt;
   SCHTBL_Table  Local;
   boolean       Modified[SCHTBL_MAX_ENTRIES];

} TBLMGR_SchTbl;

typedef struct {

   TBLMGR_MsgTbl MsgTbl;
   TBLMGR_SchTbl SchTbl;

} TBLMGR_Class;

/*
** Exported Functions
*/

/******************************************************************************
** Function: TBLMGR_Constructor
**
** Initialize the Table Manager object.
*/
void TBLMGR_Constructor(TBLMGR_Class* TblMgrPtr,
                        char* MsgTblFilePathName,
                        char* SchTblFilePathName);


/******************************************************************************
** Function: TBLMGR_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of the scheduler
** should not be impacted. The intent is to clear counters/flags for telemetry.
**
** Notes:
**   1. See the SCHTBL_Class definition for the effected data.
**
*/
void TBLMGR_ResetStatus(void);


/******************************************************************************
** Function: TBLMGR_LoadMsgTableCmd
**
** Command to load the message table.
**
** Notes:
**  1. Function signature must match cmdmgr's CMDMGR_CmdFuncPtr.
**
*/
boolean TBLMGR_LoadMsgTableCmd(const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: TBLMGR_DumpMsgTableCmd
**
** Command to load the message table.
**
** Notes:
**  1. Function signature must match cmdmgr's CMDMGR_CmdFuncPtr.
**
*/
boolean TBLMGR_DumpMsgTableCmd(const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: TBLMGR_ConfigSchEntryCmd
**
** Command to configure (enable/disable) a scheduler table slot/entry.
**
** Notes:
**  1. Function signature must match cmdmgr's CMDMGR_CmdFuncPtr.
**
*/
boolean TBLMGR_ConfigSchEntryCmd(const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: TBLMGR_LoadSchTableCmd
**
** Command to load the scheduler table.
**
** Notes:
**  1. Function signature must match cmdmgr's CMDMGR_CmdFuncPtr.
**
*/
boolean TBLMGR_LoadSchTableCmd(const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: TBLMGR_DumpSchTableCmd
**
** Command to dump the scheduler table.
**
** Notes:
**  1. Function signature must match cmdmgr's CMDMGR_CmdFuncPtr.
**
*/
boolean TBLMGR_DumpSchTableCmd(const CFE_SB_MsgPtr_t MsgPtr);

#endif /* _tblmgr_ */
