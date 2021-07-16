/*
** Purpose: Example cFE Table
**
** Notes:
**   1. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      Note the cFE's buffers are used to store the actual data itself.
**      This is a table-specific file so it doesn't need to be re-entrant.
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
#ifndef _cfetbl_
#define _cfetbl_

/*
** Includes
*/

#include "app_cfg.h"
#include "extbldata.h"

/*
** Event Message IDs
*/

#define CFETBL_REG_TBL_ERR_EID  (CFETBL_BASE_EID + 0)
#define CFETBL_ENTRY_ERR_EID    (CFETBL_BASE_EID + 1)

/*
** Type Definitions
*/

typedef struct {

   boolean Registered;
   
   int32             Status;  /* Status of last cFE Table service call */
   CFE_TBL_Handle_t  Handle;
   ExTblData_Param*  DataPtr;

   ExTblData_Param   Tbl;

} CFETBL_Class;


/*
** Exported Functions
*/

/******************************************************************************
** Function: CFETBL_Constructor
**
** Initialize the example cFE Table object.
**
** Notes:
**   1. The table values are not populated. This is done when the table is 
**      registered with the table manager.
*/
void CFETBL_Constructor(CFETBL_Class* ObjPtr);


/******************************************************************************
** Function: CFETBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavior of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void CFETBL_ResetStatus(void);


/******************************************************************************
** Function: CFETBL_GetTblEntry
**
** Assumes valid index since this is a 'low level' function and should only by
** called by functions that have verified data coming from external sourcesfunctions
** like commands.
**
*/
void CFETBL_GetTblEntry(ExTblData_Entry* TblEntryData, uint8 Index);


#endif /* _cfetbl_ */
