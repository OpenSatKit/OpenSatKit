/*
** Purpose: Implement the proto object for the proto app.
**
** Notes:
**   1. This serves as an example object that uses a tables. It does not perform
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
#include "protobj.h"

/*
** Global File Data
*/

static PROTOBJ_Class*  ProtObj = NULL;

/*
** Local Function Prototypes
*/
static boolean ValidTblId(uint8 TblId);
static const char* TblIdStr(uint8 TblId);


static const ExTblData_Param* GetXmlParamPtr();
static const ExTblData_Param* GetScanfParamPtr();
static const ExTblData_Param* GetJsonParamPtr();
static boolean LoadXmlParam(ExTblData_Param* NewParam);
static boolean LoadScanfParam(ExTblData_Param* NewParam);
static boolean LoadJsonParam(ExTblData_Param* NewParam);
static boolean LoadXmlParamEntry(uint16 EntryId, ExTblData_Entry* NewEntry);
static boolean LoadScanfParamEntry(uint16 EntryId, ExTblData_Entry* NewEntry);
static boolean LoadJsonParamEntry(uint16 EntryId, ExTblData_Entry* NewEntry);


/******************************************************************************
** Function: PROTOBJ_Constructor
**
*/
void PROTOBJ_Constructor(PROTOBJ_Class*  ProtObjPtr)
{
 
   ProtObj = ProtObjPtr;

   CFE_PSP_MemSet((void*)ProtObj, 0, sizeof(PROTOBJ_Class));

   XMLTBL_Constructor(&ProtObj->XmlTbl, GetXmlParamPtr, LoadXmlParam, LoadXmlParamEntry);
   SCANFTBL_Constructor(&ProtObj->ScanfTbl, GetScanfParamPtr, LoadScanfParam, LoadScanfParamEntry);
   JSONTBL_Constructor(&ProtObj->JsonTbl, GetJsonParamPtr, LoadJsonParam, LoadJsonParamEntry);

   CFETBL_Constructor(&ProtObj->CfeTbl);
    
} /* End PROTOBJ_Constructor */


/******************************************************************************
** Function:  PROTOBJ_ResetStatus
**
*/
void PROTOBJ_ResetStatus()
{
 
   XMLTBL_ResetStatus();
   SCANFTBL_ResetStatus();
   JSONTBL_ResetStatus();
   CFETBL_ResetStatus();

   ProtObj->TblDataEnabled = FALSE;
   ProtObj->TblIndex = 0;
   ProtObj->TblId = PROTOBJ_UNDEF_TBL_ID;
   
} /* End PROTOBJ_ResetStatus() */


/******************************************************************************
** Function: PROTOBJ_Execute
**
** Execute proto object functionality during each application runloop cycle.
**
*/
void PROTOBJ_Execute(void)
{

   /*
   ** Load table data from the last commanded table. This is just an example of
   ** doing something during the execution cycle.
   */   
   if (ProtObj->TblDataEnabled) {
      
      switch (ProtObj->TblId) {
      case PROTOBJ_XML_TBL_ID:
         ProtObj->TblEntry = ProtObj->XmlParam.Entry[ProtObj->TblIndex];
         break;
      case PROTOBJ_SCANF_TBL_ID:
         ProtObj->TblEntry = ProtObj->ScanfParam.Entry[ProtObj->TblIndex];
         break;
      case PROTOBJ_JSON_TBL_ID:
         ProtObj->TblEntry = ProtObj->JsonParam.Entry[ProtObj->TblIndex];
         break;
      case PROTOBJ_CFE_TBL_ID:
         CFETBL_GetTblEntry(&ProtObj->TblEntry, ProtObj->TblIndex);
      case PROTOBJ_UNDEF_TBL_ID:
         /* Valid state, but nothing to do */
         break;
      default:
         CFE_EVS_SendEvent(PROTOBJ_INVALID_TBL_ID_EID, CFE_EVS_CRITICAL,
                           "Invalid proto object table ID %d. Set ID to %s",
                           ProtObj->TblId, TblIdStr(PROTOBJ_UNDEF_TBL_ID));
         ProtObj->TblId = PROTOBJ_UNDEF_TBL_ID;
         break;
      }
   
   } /* End if table data enabled */
   
} /* end PROTOBJ_Execute() */


/******************************************************************************
** Function: PROTOBJ_EnableTblDataCmd
**
** Enable the ability to load data from a table to the PROTOBJ's data buffer.
**
*/
boolean PROTOBJ_EnableTblDataCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const PROTOBJ_EnableTblDataCmdMsg *CmdMsg = (const PROTOBJ_EnableTblDataCmdMsg *) MsgPtr;

   ProtObj->TblDataEnabled = (CmdMsg->EnableTblData == TRUE);
   if (ProtObj->TblDataEnabled) {
      CFE_EVS_SendEvent(PROTOBJ_CMD_ENA_TBL_DATA_EID, CFE_EVS_INFORMATION,"Proto object active table data enabled");
   }
   else {
      CFE_EVS_SendEvent(PROTOBJ_CMD_ENA_TBL_DATA_EID, CFE_EVS_INFORMATION,"Proto object active table data disabled");
   }

   return TRUE;

} /* End PROTOBJ_EnableTblDataCmd() */


/******************************************************************************
** Function: PROTOBJ_SetActiveTblCmd
**
** Activate a specific table & data entry to be used in the working buffer.
*/
boolean PROTOBJ_SetActiveTblCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const PROTOBJ_SetActiveTblCmdMsg *CmdMsg = (const PROTOBJ_SetActiveTblCmdMsg *) MsgPtr;
   boolean  RetStatus = FALSE;

   /* ValidateTblId() sends an event of ID is invalid */
   if (ValidTblId(CmdMsg->TblId)) {
      
      if (CmdMsg->TblIndex < OSK_C_PROTO_TBL_MAX_ENTRY_ID) {
      
         RetStatus = TRUE;
         ProtObj->TblId    = CmdMsg->TblId;
         ProtObj->TblIndex = CmdMsg->TblIndex;
         CFE_EVS_SendEvent(PROTOBJ_CMD_SET_TBL_INDEX_EID, CFE_EVS_INFORMATION,
                           "Active table set to %s at index %d", 
                           TblIdStr(ProtObj->TblId), ProtObj->TblIndex);
      }
      else
      {
         CFE_EVS_SendEvent(PROTOBJ_CMD_SET_TBL_INDEX_ERR_EID, CFE_EVS_ERROR,
                           "Commanded table index %d is too big. Max table index is %d", 
                           CmdMsg->TblIndex, (OSK_C_PROTO_TBL_MAX_ENTRY_ID-1));
      }
   }

   return RetStatus;


} /* End PROTOBJ_SetActiveTblCmd() */


/******************************************************************************
** Function: PROTOBJ_GetTblData
**
*/
boolean PROTOBJ_GetTblData(ExTblData_Entry* TblEntryData) {
   
   boolean RetStatus = FALSE;
   
   if (ProtObj->TblDataEnabled) {
      memcpy(TblEntryData,&ProtObj->TblEntry,sizeof(ExTblData_Entry));
      RetStatus = TRUE;
   }
   else {
      memset(TblEntryData,0,sizeof(ExTblData_Entry));
   }

   return RetStatus;
   
} /* End PROTOBJ_GetTblData() */


/******************************************************************************
** Function: ValidTblId
**
** Notes:
**   1. Order of PROTOBJ_TBL_ID enum assumed.
**   2. Typically used to verify commands so pass in uint8 since enum not used
**      in the command parameter type definitions.
**
*/
static boolean ValidTblId(uint8 TblId) {
   
   boolean RetStatus = FALSE;
   
   if (TblId >= PROTOBJ_XML_TBL_ID && TblId <= PROTOBJ_CFE_TBL_ID) {
      RetStatus = TRUE;
   }
   else {
      CFE_EVS_SendEvent(PROTOBJ_INVALID_TBL_ID_EID, CFE_EVS_ERROR,
                        "Invalid table ID %d. Must be in the range from %d to %d.",
                        TblId, PROTOBJ_XML_TBL_ID, PROTOBJ_CFE_TBL_ID);
   }
   
   return RetStatus;
   
} /* End ValidTblId() */


/******************************************************************************
** Function: TblIdStr
**
** Notes:
**   1. Order of PROTOBJ_TBL_ID enum assumed.
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
** Function: GetxxxParamPtr
**
*/
static const ExTblData_Param* GetXmlParamPtr() {

   return &(ProtObj->XmlParam);

} /* End GetXmlParamPtr() */

static const ExTblData_Param* GetScanfParamPtr() {

   return &(ProtObj->ScanfParam);

} /* End GetSCanfParamPtr() */

static const ExTblData_Param* GetJsonParamPtr() {

   return &(ProtObj->JsonParam);

} /* End GetJsonParamPtr() */


/******************************************************************************
** Function: LoadXxxParam
**
** Notes:
**   1. This is a simple table copy. More complex table loads may have pass/fail 
**      criteria.
** 
*/
static boolean LoadXmlParam(ExTblData_Param* NewParam) {

   CFE_PSP_MemCpy(&(ProtObj->XmlParam), NewParam, sizeof(ExTblData_Param));
   
   return TRUE;

} /* End LoadXmlParam() */

static boolean LoadScanfParam(ExTblData_Param* NewParam) {

   CFE_PSP_MemCpy(&(ProtObj->ScanfParam), NewParam, sizeof(ExTblData_Param));
   
   return TRUE;

} /* End LoadScanfParam() */


static boolean LoadJsonParam(ExTblData_Param* NewParam) {

   CFE_PSP_MemCpy(&(ProtObj->JsonParam), NewParam, sizeof(ExTblData_Param));
   
   return TRUE;

} /* End LoadJsonParam() */


/******************************************************************************
** Function: LoadXxxParamEntry
**
** Notes:
**   1. This is a simple table entry copy. More complex entry loads may have
**      pass/fail criteria.
*/
static boolean LoadXmlParamEntry(uint16 EntryId, ExTblData_Entry* NewEntry) {

   CFE_PSP_MemCpy(&(ProtObj->XmlParam.Entry[EntryId]),NewEntry,sizeof(ExTblData_Entry));

   return TRUE;

} /* End LoadXmlParamEntry() */


static boolean LoadScanfParamEntry(uint16 EntryId, ExTblData_Entry* NewEntry) {

   CFE_PSP_MemCpy(&(ProtObj->ScanfParam.Entry[EntryId]),NewEntry,sizeof(ExTblData_Entry));

   return TRUE;

} /* End LoadScanfParamEntry() */


static boolean LoadJsonParamEntry(uint16 EntryId, ExTblData_Entry* NewEntry) {

   CFE_PSP_MemCpy(&(ProtObj->JsonParam.Entry[EntryId]),NewEntry,sizeof(ExTblData_Entry));

   return TRUE;

} /* End LoadJsonParamEntry() */

