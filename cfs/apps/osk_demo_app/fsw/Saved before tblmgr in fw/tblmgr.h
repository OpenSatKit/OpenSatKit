/*
** $Id: $
** 
** Purpose: Provide a class to manage the Demo application tables
**
** Notes:
**   1. The tables are formatted using XML and rely on the open source expat
**      library. This library has NOT been verified for flight usage. 
**   2. One of the goals of the object-based cFE application effort is to 
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
**   1. OpenSat Object-based Application Developers Guide.
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
#include "exobj.h"

/*#############################################################################
** XML tag definitions.
**
** XML Elements/Attributes used by the example table. Table keywords could be
** standardized and imported to increase reuse
*/

#define TBLMGR_XML_EL_PKT_ENTRY  "entry"

#define TBLMGR_XML_AT_INDEX   "index"
#define TBLMGR_XML_AT_DATA1   "data1"
#define TBLMGR_XML_AT_DATA2   "data2"
#define TBLMGR_XML_AT_DATA3   "data3"

#define TBLMGR_BUFFSIZE  8192

/*
** TBLMGR_LoadTblCmd Load Type options
*/

#define TBLMGR_LOAD_TBL_REPLACE    0   /* Replace the entire table. See src for how unspecified entries are handled */
#define TBLMGR_LOAD_TBL_UPDATE     1   /* Update individual entries */


/*
** Event Message IDs
*/

#define TBLMGR_CREATE_ERR_EID           (TBLMGR_BASE_EID + 0)
#define TBLMGR_FILE_OPEN_ERR_EID        (TBLMGR_BASE_EID + 1)
#define TBLMGR_FILE_READ_ERR_EID        (TBLMGR_BASE_EID + 2)
#define TBLMGR_PARSE_ERR_EID            (TBLMGR_BASE_EID + 3)
#define TBLMGR_WRITE_CFE_HDR_ERR_EID    (TBLMGR_BASE_EID + 4)
#define TBLMGR_CREATE_MSG_DUMP_ERR_EID  (TBLMGR_BASE_EID + 5)
#define TBLMGR_CMD_LOAD_TYPE_ERR_EID    (TBLMGR_BASE_EID + 6)
#define TBLMGR_CMD_LOAD_PARSE_ERR_EID   (TBLMGR_BASE_EID + 7)


/*
** Type Definitions
*/

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


/*#############################################################################
**  Local table copies used for table load commands
**  - If more than one table then create a new TBLMGR_XxxTbl struct and add it
**    to the TBLMGR_Class. Each table will require the same state data defined
**    prior to the local copy (i.e. LoadActive, etc) and the table-specific 
**    parser functions will access the data.
*/
typedef struct {

   boolean       LoadActive;
   uint8         LastLoadStatus;
   uint16        AttrErrCnt;

   EXOBJ_Table   Local;
   boolean       Modified[EXOBJ_TBL_MAX_ENTRY_ID];

} TBLMGR_ExObjTbl;

typedef struct {

   TBLMGR_ExObjTbl  ExObjTbl;

} TBLMGR_Class;

/*
** Exported Functions
*/

/******************************************************************************
** Function: TBLMGR_Constructor
**
** Initialize the Table Manager object.
**
*/
void TBLMGR_Constructor2(TBLMGR_Class* TblMgrPtr,
                        char* TblMgrFilePathName);


/******************************************************************************
** Function: TBLMGR_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behaviour of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void TBLMGR_ResetStatus2(void);


/*#############################################################################
** Function: TBLMGR_Load_ExObj_TableCmd
**
** Command to load the _exobj_ table.
**
** Notes:
**  1. Function signature must match cmdmgr's CMDMGR_CmdFuncPtr.
**
*/
boolean TBLMGR_LoadExObjTableCmd2(const CFE_SB_MsgPtr_t MsgPtr);


/*#############################################################################
** Function: TBLMGR_Dump_ExObj_TableCmd
**
** Command to dump the _exobj_ table.
**
** Notes:
**  1. Function signature must match cmdmgr's CMDMGR_CmdFuncPtr.
**
*/
boolean TBLMGR_DumpExObjTableCmd2(const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _tblmgr_ */
