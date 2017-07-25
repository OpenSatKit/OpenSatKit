/*
** Purpose: Implement an example object.
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**   2. This serves as an example object that uses a table. It does not perform
**      any realistic functions.
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
** Function: EXOBJ_GetxxxTblPtr
**
*/
const XMLTBL_Struct* EXOBJ_GetXmlTblPtr()
{

   return &(ExObj->XmlTbl);

} /* End EXOBJ_GetXmlTblPtr() */

const SCANFTBL_Struct* EXOBJ_GetScanfTblPtr()
{

   return &(ExObj->ScanfTbl);

} /* End EXOBJ_GetSCanfTblPtr() */

/******************************************************************************
** Function:  EXOBJ_ResetStatus
**
*/
void EXOBJ_ResetStatus()
{

   ExObj->EnableDataLoad = TRUE;
   ExObj->TblIndex = 0;
   
   EXOBJ_LoadDataFromXmlTbl();

} /* End EXOBJ_ResetStatus() */


/******************************************************************************
** Function: LoadDataFromXxxTbl
**
*/
void EXOBJ_LoadDataFromXmlTbl(void)
{

   if (ExObj->EnableDataLoad)
   {
  
      ExObj->Data1 = ExObj->XmlTbl.Entry[ExObj->TblIndex].Data1;
      ExObj->Data2 = ExObj->XmlTbl.Entry[ExObj->TblIndex].Data2;
      ExObj->Data3 = ExObj->XmlTbl.Entry[ExObj->TblIndex].Data3;

   } /* End if Data Load Enabled */

} /* End EXOBJ_LoadDataFromXmlTbl() */


void EXOBJ_LoadDataFromScanfTbl(void)
{

   if (ExObj->EnableDataLoad)
   {
  
      ExObj->Data1 = ExObj->ScanfTbl.Entry[ExObj->TblIndex].Data1;
      ExObj->Data2 = ExObj->ScanfTbl.Entry[ExObj->TblIndex].Data2;
      ExObj->Data3 = ExObj->ScanfTbl.Entry[ExObj->TblIndex].Data3;

   } /* End if Data Load Enabled */

} /* End EXOBJ_LoadDataFromScanfTbl() */


/******************************************************************************
** Function: EXOBJ_LoadXxxTbl
**
** Notes:
**   1. This is a simple table copy. More complex table loads may have pass/fail 
**      criteria.
** 
*/
boolean EXOBJ_LoadXmlTbl(XMLTBL_Struct* NewTbl)
{

   CFE_PSP_MemCpy(&(ExObj->XmlTbl), NewTbl, sizeof(XMLTBL_Struct));
   
   return TRUE;

} /* End EXOBJ_LoadXmlTbl() */

boolean EXOBJ_LoadScanfTbl(SCANFTBL_Struct* NewTbl)
{

   CFE_PSP_MemCpy(&(ExObj->ScanfTbl), NewTbl, sizeof(SCANFTBL_Struct));
   
   return TRUE;

} /* End EXOBJ_LoadScanfTbl() */


/******************************************************************************
** Function: EXOBJ_LoadXxxTblEntry
**
** Notes:
**   1. This is a simple table copy. More complex table loads may have pass/fail 
**      criteria.
*/
boolean EXOBJ_LoadXmlTblEntry(uint16 EntryId, XMLTBL_Entry* NewEntry)
{

   CFE_PSP_MemCpy(&(ExObj->XmlTbl.Entry[EntryId]),NewEntry,sizeof(XMLTBL_Entry));

   return TRUE;

} /* End EXOBJ_LoadXmlTblEntry() */

boolean EXOBJ_LoadScanfTblEntry(uint16 EntryId, SCANFTBL_Entry* NewEntry)
{

   CFE_PSP_MemCpy(&(ExObj->ScanfTbl.Entry[EntryId]),NewEntry,sizeof(SCANFTBL_Entry));

   return TRUE;

} /* End EXOBJ_LoadScanfTblEntry() */


/******************************************************************************
** Function: EXOBJ_EnableOutputCmd
**
** Enable the ability to load data from the table to the working data buffer.
**
*/
boolean EXOBJ_EnableDataLoadCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const EXOBJ_EnableDataLoadCmdParam *CmdParam = (const EXOBJ_EnableDataLoadCmdParam *) MsgPtr;
   boolean  RetStatus = TRUE;

   ExObj->EnableDataLoad = (CmdParam->EnableDataLoad == TRUE);
   if (ExObj->EnableDataLoad)
   {
      CFE_EVS_SendEvent(EXOBJ_CMD_ENA_DATA_LOAD_INFO_EID, CFE_EVS_INFORMATION,"Data load enabled");
   }
   else
   {
      CFE_EVS_SendEvent(EXOBJ_CMD_ENA_DATA_LOAD_INFO_EID, CFE_EVS_INFORMATION,"Data load disabled");
   }

   return RetStatus;

} /* End EXOBJ_EnableDataLoadCmd() */


/******************************************************************************
** Function: EXOBJ_SetTblIndexCmd
**
** Set the table index for the data to be used in the working data buffer.
*/
boolean EXOBJ_SetTblIndexCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const EXOBJ_SetTblIndexCmdParam *CmdParam = (const EXOBJ_SetTblIndexCmdParam *) MsgPtr;
   boolean  RetStatus = FALSE;

   if (CmdParam->TblIndex < FW_DEMO_TBL_MAX_ENTRY_ID)
   {
      RetStatus = TRUE;
      ExObj->TblIndex = CmdParam->TblIndex;
      CFE_EVS_SendEvent(EXOBJ_CMD_SET_TBL_INDEX_INFO_EID, CFE_EVS_INFORMATION,
                        "Table index set to %d", ExObj->TblIndex);
   }
   else
   {
      CFE_EVS_SendEvent(EXOBJ_CMD_SET_TBL_INDEX_ERR_EID, CFE_EVS_ERROR,
                        "Commanded table index %d is too big. Max table index is %d", 
                        CmdParam->TblIndex, (FW_DEMO_TBL_MAX_ENTRY_ID-1));
   }

   return RetStatus;


} /* End EXOBJ_LoadDataCmd() */


/* end of file */
