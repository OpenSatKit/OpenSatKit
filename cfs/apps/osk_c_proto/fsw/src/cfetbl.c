/*
** Purpose: Implement the cFE table object.
**
** Notes:
**   1. This serves as an example cFE table adapter written in the style of the
**      OSK C application framework.
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
#include "cfetbl.h"


/*
** Global File Data
*/

static CFETBL_Class*  CfeTbl = NULL;

/*
** Local Function Prototypes
*/

static int32 Validate(void* TblPtr);


/******************************************************************************
** Function: CFETBL_Constructor
**
*/
void CFETBL_Constructor(CFETBL_Class*  CfeTblPtr)
{
 
   CfeTbl = CfeTblPtr;

   CFE_PSP_MemSet((void*)CfeTbl, 0, sizeof(CFETBL_Class));
   CfeTbl->DataPtr = (ExTblData_Param *) NULL;

   CfeTbl->Status = CFE_TBL_Register(&CfeTbl->Handle, OSK_C_PROTO_CFE_TBL_NAME,
                                    sizeof(ExTblData_Param), CFE_TBL_OPT_DEFAULT, Validate);
   
   
   CfeTbl->Registered = (CfeTbl->Status == CFE_SUCCESS);
   
   /* DataPtr will remain NULL if data not loaded. */
   if (CfeTbl->Registered) {
   
      CfeTbl->Status = CFE_TBL_Load(CfeTbl->Handle, CFE_TBL_SRC_FILE, OSK_C_PROTO_CFE_TBL_DEF_LOAD_FILE);
      if (CfeTbl->Status == CFE_SUCCESS) {
         CFE_TBL_GetAddress((void **)&(CfeTbl->DataPtr), CfeTbl->Handle);
      }
   }
   else {
      
      CFE_EVS_SendEvent(CFETBL_REG_TBL_ERR_EID, CFE_EVS_ERROR,
                        "Error registering table %s, CFE_TBL_Register() status = 0x%08X",
                        OSK_C_PROTO_CFE_TBL_NAME, CfeTbl->Status);                        
   }

    
} /* End CFETBL_Constructor() */


/******************************************************************************
** Function:  CFETBL_ResetStatus
**
*/
void CFETBL_ResetStatus()
{

   /* Nothing to do */

} /* End CFETBL_ResetStatus() */


/******************************************************************************
** Function: CFETBL_GetTblEntry
**
** Assumes valid index since this is a 'low level' function and should only by
** called by functions that have verified data coming from external sources.
**
*/
void CFETBL_GetTblEntry(ExTblData_Entry* TblEntryData, uint8 Index)
{

   
   if (CfeTbl->DataPtr != NULL) {
      
      memcpy(TblEntryData,&(CfeTbl->DataPtr)->Entry[Index],sizeof(ExTblData_Entry));

   }
   else {

      CFE_EVS_SendEvent(CFETBL_ENTRY_ERR_EID, CFE_EVS_INFORMATION,
                        "CFETBL_GetTblEntry() called but the table has not been loaded. Last table status = 0x%08X",CfeTbl->Status);                        

   }
   
} // CFETBL_GetTblEntry()


/******************************************************************************
** Function: CFETBL_Manage
**
*/
void CFETBL_Manage(void)
{

   CfeTbl->Status = CFE_TBL_ReleaseAddress(CfeTbl->Handle);
   
   CFE_TBL_Manage(CfeTbl->Handle);
   
   CfeTbl->Status = CFE_TBL_GetAddress((void **)&(CfeTbl->DataPtr), CfeTbl->Handle);

   if (CfeTbl->Status == CFE_TBL_ERR_NEVER_LOADED) {
      
      CfeTbl->DataPtr = (ExTblData_Param *) NULL;
   
   }

} // CFETBL_Manage()


/******************************************************************************
** Function: Validate
**
** Callback function used by CFE Table service to allow a table to be validated
** prior to being committed.
**
** Function signature must match CFE_TBL_CallbackFuncPtr_t
**
*/
static int32 Validate(void* TblPtr) 
{
   
   OS_printf("CFETBL::Validate()\n");
   
   return  CFE_SUCCESS;
  
} /* End Validate() */