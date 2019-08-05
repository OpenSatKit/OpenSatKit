/*
** Purpose: Implement the demo object for the demo app.
**
** Notes:
**   1. This serves as an example object that uses a table. It does not perform
**      any realistic functions.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General Public
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>

#include "app_cfg.h"
#include "demobj.h"

/*
** Global File Data
*/

static DEMOBJ_Class*  DemObj = NULL;

/*
** Local Function Prototypes
*/


/******************************************************************************
** Function: DEMOBJ_Constructor
**
*/
void DEMOBJ_Constructor(DEMOBJ_Class*  DemObjPtr)
{
 
   DemObj = DemObjPtr;

   CFE_PSP_MemSet((void*)DemObj, 0, sizeof(DEMOBJ_Class));

    
} /* End DEMOBJ_Constructor() */


/******************************************************************************
** Function: DEMOBJ_GetxxxTblPtr
**
*/
const XMLTBL_Struct* DEMOBJ_GetXmlTblPtr()
{

   return &(DemObj->XmlTbl);

} /* End DEMOBJ_GetXmlTblPtr() */

const SCANFTBL_Struct* DEMOBJ_GetScanfTblPtr()
{

   return &(DemObj->ScanfTbl);

} /* End DEMOBJ_GetSCanfTblPtr() */

const JSONTBL_Struct* DEMOBJ_GetJsonTblPtr()
{

   return &(DemObj->JsonTbl);

} /* End DEMOBJ_GetJsonTblPtr() */


/******************************************************************************
** Function:  DEMOBJ_ResetStatus
**
*/
void DEMOBJ_ResetStatus()
{

   DemObj->EnableDataLoad = TRUE;
   DemObj->TblIndex = 0;
   
   DEMOBJ_LoadDataFromXmlTbl();

} /* End DEMOBJ_ResetStatus() */


/******************************************************************************
** Function: LoadDataFromXxxTbl
**
*/
void DEMOBJ_LoadDataFromXmlTbl(void)
{

   if (DemObj->EnableDataLoad)
   {
  
      DemObj->Data1 = DemObj->XmlTbl.Entry[DemObj->TblIndex].Data1;
      DemObj->Data2 = DemObj->XmlTbl.Entry[DemObj->TblIndex].Data2;
      DemObj->Data3 = DemObj->XmlTbl.Entry[DemObj->TblIndex].Data3;

   } /* End if Data Load Enabled */

} /* End DEMOBJ_LoadDataFromXmlTbl() */


void DEMOBJ_LoadDataFromScanfTbl(void)
{

   if (DemObj->EnableDataLoad)
   {
  
      DemObj->Data1 = DemObj->ScanfTbl.Entry[DemObj->TblIndex].Data1;
      DemObj->Data2 = DemObj->ScanfTbl.Entry[DemObj->TblIndex].Data2;
      DemObj->Data3 = DemObj->ScanfTbl.Entry[DemObj->TblIndex].Data3;

   } /* End if Data Load Enabled */

} /* End DEMOBJ_LoadDataFromScanfTbl() */

void DEMOBJ_LoadDataFromJsonTbl(void)
{

   if (DemObj->EnableDataLoad)
   {
  
      DemObj->Data1 = DemObj->JsonTbl.Entry[DemObj->TblIndex].Data1;
      DemObj->Data2 = DemObj->JsonTbl.Entry[DemObj->TblIndex].Data2;
      DemObj->Data3 = DemObj->JsonTbl.Entry[DemObj->TblIndex].Data3;

   } /* End if Data Load Enabled */

} /* End DEMOBJ_LoadDataFromJsonTbl() */


/******************************************************************************
** Function: DEMOBJ_LoadXxxTbl
**
** Notes:
**   1. This is a simple table copy. More complex table loads may have pass/fail 
**      criteria.
** 
*/
boolean DEMOBJ_LoadXmlTbl(XMLTBL_Struct* NewTbl)
{

   CFE_PSP_MemCpy(&(DemObj->XmlTbl), NewTbl, sizeof(XMLTBL_Struct));
   
   return TRUE;

} /* End DEMOBJ_LoadXmlTbl() */

boolean DEMOBJ_LoadScanfTbl(SCANFTBL_Struct* NewTbl)
{

   CFE_PSP_MemCpy(&(DemObj->ScanfTbl), NewTbl, sizeof(SCANFTBL_Struct));
   
   return TRUE;

} /* End DEMOBJ_LoadScanfTbl() */

boolean DEMOBJ_LoadJsonTbl(JSONTBL_Struct* NewTbl)
{

   CFE_PSP_MemCpy(&(DemObj->JsonTbl), NewTbl, sizeof(JSONTBL_Struct));
   
   return TRUE;

} /* End DEMOBJ_LoadJsonTbl() */


/******************************************************************************
** Function: DEMOBJ_LoadXxxTblEntry
**
** Notes:
**   1. This is a simple table copy. More complex table loads may have pass/fail 
**      criteria.
*/
boolean DEMOBJ_LoadXmlTblEntry(uint16 EntryId, XMLTBL_Entry* NewEntry)
{

   CFE_PSP_MemCpy(&(DemObj->XmlTbl.Entry[EntryId]),NewEntry,sizeof(XMLTBL_Entry));

   return TRUE;

} /* End DEMOBJ_LoadXmlTblEntry() */

boolean DEMOBJ_LoadScanfTblEntry(uint16 EntryId, SCANFTBL_Entry* NewEntry)
{

   CFE_PSP_MemCpy(&(DemObj->ScanfTbl.Entry[EntryId]),NewEntry,sizeof(SCANFTBL_Entry));

   return TRUE;

} /* End DEMOBJ_LoadScanfTblEntry() */

boolean DEMOBJ_LoadJsonTblEntry(uint16 EntryId, JSONTBL_Entry* NewEntry)
{

   CFE_PSP_MemCpy(&(DemObj->JsonTbl.Entry[EntryId]),NewEntry,sizeof(JSONTBL_Entry));

   return TRUE;

} /* End DEMOBJ_LoadJsonTblEntry() */


/******************************************************************************
** Function: DEMOBJ_EnableOutputCmd
**
** Enable the ability to load data from the table to the working data buffer.
**
*/
boolean DEMOBJ_EnableDataLoadCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const DEMOBJ_EnableDataLoadCmdParam *CmdParam = (const DEMOBJ_EnableDataLoadCmdParam *) MsgPtr;
   boolean  RetStatus = TRUE;

   DemObj->EnableDataLoad = (CmdParam->EnableDataLoad == TRUE);
   if (DemObj->EnableDataLoad)
   {
      CFE_EVS_SendEvent(DEMOBJ_CMD_ENA_DATA_LOAD_INFO_EID, CFE_EVS_INFORMATION,"Data load enabled");
   }
   else
   {
      CFE_EVS_SendEvent(DEMOBJ_CMD_ENA_DATA_LOAD_INFO_EID, CFE_EVS_INFORMATION,"Data load disabled");
   }

   return RetStatus;

} /* End DEMOBJ_EnableDataLoadCmd() */


/******************************************************************************
** Function: DEMOBJ_SetTblIndexCmd
**
** Set the table index for the data to be used in the working data buffer.
*/
boolean DEMOBJ_SetTblIndexCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const DEMOBJ_SetTblIndexCmdParam *CmdParam = (const DEMOBJ_SetTblIndexCmdParam *) MsgPtr;
   boolean  RetStatus = FALSE;

   if (CmdParam->TblIndex < OSK_DEMO_TBL_MAX_ENTRY_ID)
   {
      RetStatus = TRUE;
      DemObj->TblIndex = CmdParam->TblIndex;
      CFE_EVS_SendEvent(DEMOBJ_CMD_SET_TBL_INDEX_INFO_EID, CFE_EVS_INFORMATION,
                        "Table index set to %d", DemObj->TblIndex);
   }
   else
   {
      CFE_EVS_SendEvent(DEMOBJ_CMD_SET_TBL_INDEX_ERR_EID, CFE_EVS_ERROR,
                        "Commanded table index %d is too big. Max table index is %d", 
                        CmdParam->TblIndex, (OSK_DEMO_TBL_MAX_ENTRY_ID-1));
   }

   return RetStatus;


} /* End DEMOBJ_LoadDataCmd() */


/* end of file */
