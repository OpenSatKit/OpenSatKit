/*
** Purpose: Implement an example object.
**
** Notes:
**   1. Template written by David McComas and licensed under the GNU
**      Lesser General Public License (LGPL).
**   2. This serves as an example object that uses a table. It does not perform
**      any realistic funcions.
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>

#include "app_cfg.h"
#include "exobj.h"

/*
** Global File Data
*/

static EXOBJ_Class*  ExObj = NULL;

/*
** Local Function Prototypes
*/


/******************************************************************************
** Function: EXOBJ_Constructor
**
*/
void EXOBJ_Constructor(EXOBJ_Class*  ExObjPtr)
{
 
   ExObj = ExObjPtr;

   CFE_PSP_MemSet((void*)ExObj, 0, sizeof(EXOBJ_Class));

    
} /* End EXOBJ_Constructor() */


/******************************************************************************
** Function: EXOBJ_Execute
**
** Execute main object function.
**
*/
void EXOBJ_Execute(void)
{

   ExObj->ExecCnt++;

} /* EXOBJ_Execute() */

/******************************************************************************
** Function:  EXOBJ_ResetStatus
**
*/
void EXOBJ_ResetStatus(void)
{

   ExObj->ExecCnt = 0;
   
} /* End EXOBJ_ResetStatus() */


/******************************************************************************
** Function: EXOBJ_GetTblPtr
**
*/
EXTBL_Data* EXOBJ_GetTblDataPtr(void)
{

   return &(ExObj->TblData);

} /* End EXOBJ_GetTblPtr() */


/******************************************************************************
** Function: EXOBJ_LoadTbl
**
*/
boolean EXOBJ_LoadTbl(EXTBL_Data* NewTblData)
{

   boolean  RetStatus = TRUE;

   OS_printf("EXOBJ_LoadTbl()\n");
   /* 
   ** This is a simple table copy. More complex table loads may have pass/fail 
   ** criteria.
   */

   CFE_PSP_MemCpy(&(ExObj->TblData), NewTblData, sizeof(EXTBL_Data));

   return RetStatus;

} /* End EXOBJ_LoadTbl() */


/******************************************************************************
** Function: EXOBJ_LoadTblEntry
**
*/
boolean EXOBJ_LoadTblEntry(uint16 EntryId, EXTBL_Entry* NewEntry)
{

   boolean  RetStatus = TRUE;

   /* 
   ** This is a simple table entry copy. More complex table load may have 
   ** pass/fail criteria.
   */

   CFE_PSP_MemCpy(&(ExObj->TblData.Entry[EntryId]),NewEntry,sizeof(EXTBL_Entry));

   return RetStatus;

} /* End EXOBJ_LoadTblEntry() */


/******************************************************************************
** Function: EXOBJ_DemoCmd
**
** Send an event message showing that the example object's command is executed.
**
*/
boolean EXOBJ_DemoCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const EXOBJ_DemoCmdMsg *CmdMsg = (const EXOBJ_DemoCmdMsg *) MsgPtr;

   CFE_EVS_SendEvent (EXOBJ_DEMO_CMD_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "Example demo command received with parameter %d",
                      CmdMsg->Parameter);

   return TRUE;

} /* End EXOBJ_EnableDataLoadCmd() */


/* end of file */
