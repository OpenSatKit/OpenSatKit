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

   CfeTbl->Status = CFE_TBL_Register(&CfeTbl->Handle, OSK_C_DEMO_CFE_TBL_NAME,
                                    sizeof(ExTblData_Param), CFE_TBL_OPT_DEFAULT, Validate);
   
   
   CfeTbl->Registered = (CfeTbl->Status == CFE_SUCCESS);
   
   if (CfeTbl->Registered) {
   
      CfeTbl->Status = CFE_TBL_Load(CfeTbl->Handle, CFE_TBL_SRC_FILE, OSK_C_DEMO_CFE_TBL_DEF_LOAD_FILE);
   
   }
   else {
      
      CFE_EVS_SendEvent(999, CFE_EVS_ERROR,
                        "Error registering table %s, CFE_TBL_Register() status = 0x%08X\n",
                        OSK_C_DEMO_CFE_TBL_NAME, CfeTbl->Status);                        
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
** called by functions that have verified data coming from external sourcesfunctions
** like commands.
**
*/
void CFETBL_GetTblEntry(ExTblData_Entry* TblEntryData, uint8 Index)
{

   ExTblData_Param* TblData = NULL;
   
   if (CfeTbl->Registered) {

      CfeTbl->Status = CFE_TBL_GetAddress((void **)&TblData, CfeTbl->Handle);
   
      if (CfeTbl->Status == CFE_SUCCESS) {
      
         memcpy(TblEntryData,&TblData->Entry[Index],sizeof(ExTblData_Entry));

         CfeTbl->Status = CFE_TBL_ReleaseAddress(CfeTbl->Handle);
      
      }

   }
   
} // CFETBL_GetTblEntry()


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