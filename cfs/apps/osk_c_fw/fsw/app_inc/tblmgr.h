/*
** Purpose: Manage tables for an application
**
** Notes:
**   1. This utility does not dictate a specific table format. It 
**      only specifies an API for managing an application's table.
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

#ifndef _tblmgr_
#define _tblmgr_

/*
** Includes
*/

#include "osk_c_fw_cfg.h"

/*
** Event Message IDs
*/

#define TBLMGR_REG_EXCEEDED_MAX_EID  (TBLMGR_BASE_EID + 0)
#define TBLMGR_LOAD_ID_ERR_EID       (TBLMGR_BASE_EID + 1)
#define TBLMGR_DUMP_ID_ERR_EID       (TBLMGR_BASE_EID + 2)
#define TBLMGR_LOAD_STUB_ERR_EID     (TBLMGR_BASE_EID + 3)
#define TBLMGR_DUMP_STUB_ERR_EID     (TBLMGR_BASE_EID + 4)
#define TBLMGR_LOAD_SUCCESS_EID      (TBLMGR_BASE_EID + 5)
#define TBLMGR_DUMP_SUCCESS_EID      (TBLMGR_BASE_EID + 6)

/*
** Table status
*/

#define TBLMGR_STATUS_UNDEF    (0)  /* Code assumes 0=UNDEF */
#define TBLMGR_STATUS_VALID    (1)
#define TBLMGR_STATUS_INVALID  (2)

/*
** Table actions
*/

#define TBLMGR_ACTION_UNDEF    (0)  /* Code assumes 0=UNDEF */
#define TBLMGR_ACTION_REGISTER (1)
#define TBLMGR_ACTION_LOAD     (2)
#define TBLMGR_ACTION_DUMP     (3)

#define TBLMGR_UNDEF_STR       "Undefined"
 
/*
** Load Type options
**
** - If the user table format allows sparse definitions then it is up to teh user 
**   to define how the replace table option behaves when all of the table elements 
**   are not defined.
*/

#define TBLMGR_LOAD_TBL_REPLACE    0   /* Replace the entire table. */
#define TBLMGR_LOAD_TBL_UPDATE     1   /* Update individual entries */

/*
** Type Definitions
*/

/* 
** Table Class
** - ID is assigned by TBLMGR
** - Application's table processing object supply the load/dump functions
*/

typedef struct TBLMGR_Tbl TBLMGR_Tbl;

typedef boolean (*TBLMGR_LoadTblFuncPtr) (TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);
typedef boolean (*TBLMGR_DumpTblFuncPtr) (TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);

struct TBLMGR_Tbl
{
  
   uint8   Id; 
   uint8   LastAction;
   boolean LastActionStatus;
   boolean Loaded;
   char    Filename[OS_MAX_PATH_LEN];
   
   TBLMGR_LoadTblFuncPtr  LoadFuncPtr;
   TBLMGR_DumpTblFuncPtr  DumpFuncPtr;

};

/* 
** Table Manager Class
*/

typedef struct
{

   uint8       NextAvailableId;
   uint8       LastActionTblId;
   TBLMGR_Tbl  Tbl[TBLMGR_MAX_TBL_PER_APP];

} TBLMGR_Class;



/*
** Exported Functions
*/

/******************************************************************************
** Function: TBLMGR_Constructor
**
** Notes:
**    1. This function must be called prior to any other functions being
**       called using the same tblmgr instance.
*/
void TBLMGR_Constructor(TBLMGR_Class* TblMgr);


/******************************************************************************
** Function: TBLMGR_RegisterTbl
**
** Register a table without loading a default table.
** Returns table ID assigned to new table or TBLMGR_MAX_TBL_PER_APP if no IDs left.
*/
uint8 TBLMGR_RegisterTbl(TBLMGR_Class* TblMgr, TBLMGR_LoadTblFuncPtr LoadFuncPtr, 
                         TBLMGR_DumpTblFuncPtr DumpFuncPtr); 


/******************************************************************************
** Function: TBLMGR_RegisterTblWithDef
**
** Register a table and load a default table
** Returns table ID assigned to new table or TBLMGR_MAX_TBL_PER_APP if no IDs left.
*/
uint8 TBLMGR_RegisterTblWithDef(TBLMGR_Class* TblMgr, TBLMGR_LoadTblFuncPtr LoadFuncPtr, 
                                TBLMGR_DumpTblFuncPtr DumpFuncPtr, const char* TblFilename); 


/******************************************************************************
** Function: TBLMGR_ResetStatus
**
*/
void TBLMGR_ResetStatus(TBLMGR_Class* TblMgr);


/******************************************************************************
** Function: TBLMGR_GetLastTblStatus
**
** Returns a pointer to the table status structure for the table that the
** last action was performed upon. Returns NULL if table manager's last action
** is in an invalid state.
*/
const TBLMGR_Tbl* TBLMGR_GetLastTblStatus(TBLMGR_Class* TblMgr);


/******************************************************************************
** Function: TBLMGR_GetTblStatus
**
** Returns a pointer to the table status for TblId. Returns NULL if TblId 
** is invalid.
*/
const TBLMGR_Tbl* TBLMGR_GetTblStatus(TBLMGR_Class* TblMgr, uint8 TblId);


/******************************************************************************
** Function: TBLMGR_LoadTblCmd
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. It calls the TBLMGR_LoadTblFuncPtr function that the user provided
**     during registration 
** 
*/
boolean TBLMGR_LoadTblCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: TBLMGR_DumpTblCmd
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
**  2. It calls the TBLMGR_DumpTblFuncPtr function that the user provided
**     during registration 
** 
*/
boolean TBLMGR_DumpTblCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: TBLMGR_LoadTypeStr
**
** Note:
**  1. Returns a pointer to a string that describes the load type 
** 
*/
const char* TBLMGR_LoadTypeStr(int8 LoadType);


#endif /* _tblmgr_ */
