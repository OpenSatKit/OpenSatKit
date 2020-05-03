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
static boolean ValidTblId(uint8 TblId);
static const char* TblIdStr(uint8 TblId);


/******************************************************************************
** Function: DEMOBJ_Constructor
**
*/
void DEMOBJ_Constructor(DEMOBJ_Class*  DemObjPtr)
{
 
   DemObj = DemObjPtr;

   CFE_PSP_MemSet((void*)DemObj, 0, sizeof(DEMOBJ_Class));

   CFETBL_Constructor(&DemObj->CfeTbl);
    
} /* End DEMOBJ_Constructor(&DemObj->CfeTbl) */


/******************************************************************************
** Function:  DEMOBJ_ResetStatus
**
*/
void DEMOBJ_ResetStatus()
{
 
   XMLTBL_ResetStatus();
   SCANFTBL_ResetStatus();
   JSONTBL_ResetStatus();
   CFETBL_ResetStatus();

   DemObj->TblDataEnabled = FALSE;
   DemObj->TblIndex = 0;
   DemObj->TblId = DEMOBJ_UNDEF_TBL_ID;
   
} /* End DEMOBJ_ResetStatus() */


/******************************************************************************
** Function: DEMOBJ_Execute
**
** Execute demo object functionality during each application runloop cycle.
**
*/
void DEMOBJ_Execute(void)
{

   /*
   ** Load table data from the last commanded table. This is just an exmaple of
   ** doing something during the execution cycle.
   */   
   if (DemObj->TblDataEnabled) {
      
      switch (DemObj->TblId) {
      case DEMOBJ_XML_TBL_ID:
         DemObj->TblEntry = DemObj->XmlTbl.Entry[DemObj->TblIndex];
         break;
      case DEMOBJ_SCANF_TBL_ID:
         DemObj->TblEntry = DemObj->ScanfTbl.Entry[DemObj->TblIndex];
         break;
      case DEMOBJ_JSON_TBL_ID:
         DemObj->TblEntry = DemObj->JsonTbl.Entry[DemObj->TblIndex];
         break;
      case DEMOBJ_CFE_TBL_ID:
         CFETBL_GetTblEntry(&DemObj->TblEntry, DemObj->TblIndex);
      case DEMOBJ_UNDEF_TBL_ID:
         /* Valid state, but nothing to do */
         break;
      default:
         CFE_EVS_SendEvent(999, CFE_EVS_CRITICAL,
                           "Invalid demo object table ID %d. Set ID to %s",
                           DemObj->TblId, TblIdStr(DEMOBJ_UNDEF_TBL_ID));
         DemObj->TblId = DEMOBJ_UNDEF_TBL_ID;
         break;
      }
   
   } /* End if table data enabled */
   
} /* end DEMOBJ_Execute() */


/******************************************************************************
** Function: DEMOBJ_EnableTblDataCmd
**
** Enable the ability to load data from a table to the DEMOBJ's data buffer.
**
*/
boolean DEMOBJ_EnableTblDataCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const DEMOBJ_EnableTblDataCmdParam *CmdParam = (const DEMOBJ_EnableTblDataCmdParam *) MsgPtr;

   DemObj->TblDataEnabled = (CmdParam->EnableTblData == TRUE);
   if (DemObj->TblDataEnabled) {
      CFE_EVS_SendEvent(DEMOBJ_CMD_ENA_TBL_DATA_EID, CFE_EVS_INFORMATION,"Table Data load enabled");
   }
   else {
      CFE_EVS_SendEvent(DEMOBJ_CMD_ENA_TBL_DATA_EID, CFE_EVS_INFORMATION,"Data load disabled");
   }

   return TRUE;

} /* End DEMOBJ_EnableTblDataCmd() */


/******************************************************************************
** Function: DEMOBJ_SetActiveTblCmd
**
** Activate a specific table & data entry to be used in the working buffer.
*/
boolean DEMOBJ_SetActiveTblCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const DEMOBJ_SetActiveTblCmdParam *CmdParam = (const DEMOBJ_SetActiveTblCmdParam *) MsgPtr;
   boolean  RetStatus = FALSE;

   /* ValidateTblId() sends an event of ID is invalid */
   if (ValidTblId(CmdParam->TblId)) {
      
      if (CmdParam->TblIndex < OSK_C_DEMO_TBL_MAX_ENTRY_ID) {
      
         RetStatus = TRUE;
         DemObj->TblId    = CmdParam->TblId;
         DemObj->TblIndex = CmdParam->TblIndex;
         CFE_EVS_SendEvent(DEMOBJ_CMD_SET_TBL_INDEX_EID, CFE_EVS_INFORMATION,
                           "Active table set to %s at index %d", 
                           TblIdStr(DemObj->TblId), DemObj->TblIndex);
      }
      else
      {
         CFE_EVS_SendEvent(DEMOBJ_CMD_SET_TBL_INDEX_ERR_EID, CFE_EVS_ERROR,
                           "Commanded table index %d is too big. Max table index is %d", 
                           CmdParam->TblIndex, (OSK_C_DEMO_TBL_MAX_ENTRY_ID-1));
      }
   }

   return RetStatus;


} /* End DEMOBJ_SetActiveTblCmd() */


/******************************************************************************
** Function: DEMOBJ_GetTblData
**
*/
boolean DEMOBJ_GetTblData(ExTblData_Entry* TblEntryData) {
   
   boolean RetStatus = FALSE;
   
   if (DemObj->TblDataEnabled) {
      memcpy(TblEntryData,&DemObj->TblEntry,sizeof(ExTblData_Entry));
      RetStatus = TRUE;
   }
   else {
      memset(TblEntryData,0,sizeof(ExTblData_Entry));
   }

   return RetStatus;
   
} /* End DEMOBJ_GetTblData() */


/******************************************************************************
** Function: ValidTblId
**
** Notes:
**   1. Order of DEMOBJ_TBL_ID enum assumed.
**   2. Typically used to verify commands so pass in uint8 since enum not used
**      in the command parameter type definitions.
**
*/
static boolean ValidTblId(uint8 TblId) {
   
   boolean RetStatus = FALSE;
   
   if (TblId >= DEMOBJ_XML_TBL_ID && TblId <= DEMOBJ_CFE_TBL_ID) {
      RetStatus = TRUE;
   }
   else {
      CFE_EVS_SendEvent(DEMOBJ_INVALID_TBL_ID_EID, CFE_EVS_ERROR,
                        "Invalid table ID %d. Must be in the range from %d to %d.",
                        TblId, DEMOBJ_XML_TBL_ID, DEMOBJ_CFE_TBL_ID);
   }
   
   return RetStatus;
   
} /* End ValidTblId() */


/******************************************************************************
** Function: TblIdStr
**
** Notes:
**   1. Order of DEMOBJ_TBL_ID enum assumed.
**   2. Typically used in command function event messages so pass in uint8 
**      since enum not used in the command parameter type definitions.
*/
static const char* TblIdStr(uint8 TblId) {

   static char* IdStr[] = { "XML", "SCANF", "JSON", "cFE", "UNDEF" };
   
   uint8 i;
   
   ValidTblId(TblId) ? i = TblId : 4;
           
   return IdStr[i];

} /* End TblIdStr() */















/******************************************************************************
** Function: DEMOBJ_GetxxxTblPtr
**
*/
const ExTblData_Param* DEMOBJ_GetXmlTblPtr()
{

   return &(DemObj->XmlTbl);

} /* End DEMOBJ_GetXmlTblPtr() */

const ExTblData_Param* DEMOBJ_GetScanfTblPtr()
{

   return &(DemObj->ScanfTbl);

} /* End DEMOBJ_GetSCanfTblPtr() */

const ExTblData_Param* DEMOBJ_GetJsonTblPtr()
{

   return &(DemObj->JsonTbl);

} /* End DEMOBJ_GetJsonTblPtr() */


/******************************************************************************
** Function: DEMOBJ_LoadXxxTbl
**
** Notes:
**   1. This is a simple table copy. More complex table loads may have pass/fail 
**      criteria.
** 
*/
boolean DEMOBJ_LoadXmlTbl(ExTblData_Param* NewTbl)
{

   CFE_PSP_MemCpy(&(DemObj->XmlTbl), NewTbl, sizeof(ExTblData_Param));
   
   return TRUE;

} /* End DEMOBJ_LoadXmlTbl() */

boolean DEMOBJ_LoadScanfTbl(ExTblData_Param* NewTbl)
{

   CFE_PSP_MemCpy(&(DemObj->ScanfTbl), NewTbl, sizeof(ExTblData_Param));
   
   return TRUE;

} /* End DEMOBJ_LoadScanfTbl() */

boolean DEMOBJ_LoadJsonTbl(ExTblData_Param* NewTbl)
{

   CFE_PSP_MemCpy(&(DemObj->JsonTbl), NewTbl, sizeof(ExTblData_Param));
   
   return TRUE;

} /* End DEMOBJ_LoadJsonTbl() */


/******************************************************************************
** Function: DEMOBJ_LoadXxxTblEntry
**
** Notes:
**   1. This is a simple table copy. More complex table loads may have pass/fail 
**      criteria.
*/
boolean DEMOBJ_LoadXmlTblEntry(uint16 EntryId, ExTblData_Entry* NewEntry)
{

   CFE_PSP_MemCpy(&(DemObj->XmlTbl.Entry[EntryId]),NewEntry,sizeof(ExTblData_Entry));

   return TRUE;

} /* End DEMOBJ_LoadXmlTblEntry() */

boolean DEMOBJ_LoadScanfTblEntry(uint16 EntryId, ExTblData_Entry* NewEntry)
{

   CFE_PSP_MemCpy(&(DemObj->ScanfTbl.Entry[EntryId]),NewEntry,sizeof(ExTblData_Entry));

   return TRUE;

} /* End DEMOBJ_LoadScanfTblEntry() */

boolean DEMOBJ_LoadJsonTblEntry(uint16 EntryId, ExTblData_Entry* NewEntry)
{

   CFE_PSP_MemCpy(&(DemObj->JsonTbl.Entry[EntryId]),NewEntry,sizeof(ExTblData_Entry));

   return TRUE;

} /* End DEMOBJ_LoadJsonTblEntry() */


