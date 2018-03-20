/*
** Purpose: Implement ThreeAxisFsw Controller Table.
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Include Files:
*/

#include <string.h>
#include "ctrltbl.h"

/* Convenience macro */
#define  JSON_OBJ  &(CtrlTbl->Json)

/*
** Type Definitions
*/


/*
** Global File Data
*/

static CTRLTBL_Class* CtrlTbl = NULL;

/*
** Local File Function Prototypes
*/

/******************************************************************************
** Function: xxxCallback
**
** Notes:
**   1. These functions must have the same function signature as 
**      JSON_ContainerFuncPtr.
*/
boolean MoiCallback (int TokenIdx);
boolean PdGainParamCallback (int TokenIdx);
boolean WhlTgtMomLimCallback (int TokenIdx);

/// TODO - Move to OSK_APP_FW
/******************************************************************************
** Function: JSON_ObjConstructor
**
** Notes:
**    1. This must be called prior to any other functions using the JSON_OBJ
**    2. The object name must be identical (case sensitive) to the name in the
**       JSON file. 
**
*/
void JSON_ObjConstructor(JSON_Obj*              Obj,
                         char*                  Name,
                         JSON_ContainerFuncPtr  Callback,
                         void*                  Data)
{
	
   strncpy(&(Obj->Name[0]), Name, JSON_OBJ_NAME_MAX_CHAR);
   Obj->Modified = FALSE;
   Obj->Callback = Callback;
   Obj->Data     = Data;
   

} /* End JSON_ObjConstructor() */


/******************************************************************************
** Function: CTRLTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void CTRLTBL_Constructor(CTRLTBL_Class* ObjPtr,
                         CTRLTBL_GetTblPtr    GetTblPtrFunc,
                         CTRLTBL_LoadTbl      LoadTblFunc, 
                         CTRLTBL_LoadTblEntry LoadTblEntryFunc)
{
   
   CtrlTbl = ObjPtr;

   CFE_PSP_MemSet(CtrlTbl, 0, sizeof(CTRLTBL_Class));

   CtrlTbl->GetTblPtrFunc    = GetTblPtrFunc;
   CtrlTbl->LoadTblFunc      = LoadTblFunc;
   CtrlTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_ObjConstructor(&(CtrlTbl->JsonObj[CTRLTBL_OBJ_MOI]),
                       CTRLTBL_OBJ_MOI_NAME,
                       MoiCallback,
                       (void *)&(CtrlTbl->Data.Moi));

   JSON_ObjConstructor(&(CtrlTbl->JsonObj[CTRLTBL_OBJ_PD_GAIN_PARAM]),
                       CTRLTBL_OBJ_PD_GAIN_PARAM_NAME,
                       PdGainParamCallback,
                       (void *)&(CtrlTbl->Data.PdGainParam));

   JSON_ObjConstructor(&(CtrlTbl->JsonObj[CTRLTBL_OBJ_WHL_TGT_MOM_LIM]),
                       CTRLTBL_OBJ_WHL_TGT_MOM_LIM_NAME,
                       WhlTgtMomLimCallback,
                       (void *)&(CtrlTbl->Data.WhlTgtMomLim));


} /* End CTRLTBL_Constructor() */



/******************************************************************************
** Function: CTRLTBL_ResetStatus
**
*/
void CTRLTBL_ResetStatus(void)
{
   int i;
   
   CtrlTbl->LastLoadStatus    = TBLMGR_STATUS_UNDEF;
   CtrlTbl->AttrErrCnt        = 0;
   CtrlTbl->ObjLoadCnt        = 0;
   
   for (i=0; i < CTRLTBL_OBJ_CNT; i++) CtrlTbl->JsonObj[i].Modified = FALSE;
    
} /* End CTRLTBL_ResetStatus() */


/******************************************************************************
** Function: CTRLTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean CTRLTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int i;
   
   OS_printf("CTRLTBL_LoadCmd() Entry\n");
   
   CTRLTBL_ResetStatus();  /* Reset status & object modified flags */

   /* TODO - Why is this here? I copied from demo which may be f'd up */
   JSON_Constructor(JSON_OBJ, CtrlTbl->JsonFileBuf, CtrlTbl->JsonFileTokens);
   
   if (JSON_OpenFile(JSON_OBJ, Filename)) {
  
      OS_printf("CTRLTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
      ///JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      ///JSON_PrintTokens(&Json,50);
  
      JSON_RegContainerCallback(JSON_OBJ,
	                            CtrlTbl->JsonObj[CTRLTBL_OBJ_MOI].Name,
	                            CtrlTbl->JsonObj[CTRLTBL_OBJ_MOI].Callback);
      JSON_RegContainerCallback(JSON_OBJ,
	                            CtrlTbl->JsonObj[CTRLTBL_OBJ_PD_GAIN_PARAM].Name,
	                            CtrlTbl->JsonObj[CTRLTBL_OBJ_PD_GAIN_PARAM].Callback);
      JSON_RegContainerCallback(JSON_OBJ,
	                            CtrlTbl->JsonObj[CTRLTBL_OBJ_WHL_TGT_MOM_LIM].Name,
	                            CtrlTbl->JsonObj[CTRLTBL_OBJ_WHL_TGT_MOM_LIM].Callback);

      JSON_ProcessTokens(JSON_OBJ);

	  /* 
	  ** ObjLoadCnt is used as a crude sanity check for REPLACE & UPDATE. Tighter
     ** checks such as verifying one of each JSON object is received for a REPLACE
     ** could be done if desired.	  
	  */
      if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
         if (CtrlTbl->ObjLoadCnt == CTRLTBL_OBJ_CNT) {
			 
            CtrlTbl->LastLoadStatus = ((CtrlTbl->LoadTblFunc)(&(CtrlTbl->Data)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

         } /* End if valid object count */
         else {
            CFE_EVS_SendEvent(CTRLTBL_CMD_LOAD_REPLACE_ERR_EID,CFE_EVS_ERROR,
			                     "CTRLTBL: Replace table command rejected. File contained %d objects when expecting %d",
							         CtrlTbl->ObjLoadCnt, CTRLTBL_OBJ_CNT);
         } /* End if not all objects loaded for a replace table command */
		 
	  } /* End if replace entire table */
      else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
         if (CtrlTbl->ObjLoadCnt > 0 && CtrlTbl->ObjLoadCnt <= CTRLTBL_OBJ_CNT) {
			 
		    CtrlTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
   
            for (i=0; i < CTRLTBL_OBJ_CNT; i++) {

               if (CtrlTbl->JsonObj[i].Modified) {
                  if (!(CtrlTbl->LoadTblEntryFunc)(i, &(CtrlTbl->JsonObj[i].Data)))
                     CtrlTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
               }

            } /* End entry loop */
			
         } /* End if valid object count */
         else {
            CFE_EVS_SendEvent(CTRLTBL_CMD_LOAD_UPDATE_ERR_EID,CFE_EVS_ERROR,
			                  "CTRLTBL: Update table command rejected. File contained %d objects when expecting 1 to %d",
							  CtrlTbl->ObjLoadCnt, CTRLTBL_OBJ_CNT);
         } /* End if not all objects loaded for a replace table command */

      } /* End if update individual records */
      else {
         CFE_EVS_SendEvent(CTRLTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"CTRLTBL: Invalid table command load type %d",LoadType);
      }
      
   } /* End if valid file */
   else {
      //printf("**ERROR** Processing CTRL file %s. Status = %d JSMN Status = %d\n",TEST_FILE, Json.FileStatus, Json.JsmnStatus);
      CFE_EVS_SendEvent(CTRLTBL_CMD_LOAD_OPEN_ERR_EID,CFE_EVS_ERROR,"CTRLTBL: Table open failure for file %s. File Status = %s JSMN Status = %s",
	                    Filename, JSON_GetFileStatusStr(CtrlTbl->Json.FileStatus), JSON_GetJsmnErrStr(CtrlTbl->Json.JsmnStatus));
   }
    
    
   return (CtrlTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of CTRLTBL_LoadCmd() */


/******************************************************************************
** Function: CTRLTBL_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
**  3. DumpType is unused.
**  4. File is formatted so it can be used as a load file. It does not follow
**     the cFE table file format. 
**  5. Creates a new dump file, overwriting anything that may have existed
**     previously
*/

boolean CTRLTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   int32    FileHandle;
   char     DumpRecord[256];
   const CTRLTBL_Struct *CtrlTblPtr;

   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS)
   {

      CtrlTblPtr = (CtrlTbl->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"F42 Control Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"\"description\": \"Example 42 FSW application attitude control parameters.\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* MOI */
      sprintf(DumpRecord,"\"%s\": {\n",CtrlTbl->JsonObj[CTRLTBL_OBJ_MOI].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"x\": %f,\n   \"y\": %f,\n   \"z\": %f\n},\n",
              CtrlTblPtr->Moi.x, CtrlTblPtr->Moi.y, CtrlTblPtr->Moi.z);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
			  
      /* PD Gain Parameters */      
      sprintf(DumpRecord,"\"%s\": {\n",CtrlTbl->JsonObj[CTRLTBL_OBJ_PD_GAIN_PARAM].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"w\": %f,\n   \"z\": %f\n},\n",
              CtrlTblPtr->PdGainParam.w, CtrlTblPtr->PdGainParam.z);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* Wheel Target Momentum Limits*/
      sprintf(DumpRecord,"\"%s\": {\n",CtrlTbl->JsonObj[CTRLTBL_OBJ_WHL_TGT_MOM_LIM].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"lower\": %f,\n   \"upper\": %f\n}\n",
              CtrlTblPtr->WhlTgtMomLim.Lower, CtrlTblPtr->WhlTgtMomLim.Upper);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* TODO - Add addition meta data when file dumped */
      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else
   {
   
      CFE_EVS_SendEvent(CTRLTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of CTRLTBL_DumpCmd() */


/******************************************************************************
** Function: MoiCallback
**
** Process a moment-of-inertia table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
*/
boolean MoiCallback (int TokenIdx)
{

   int     AxisCnt=0;
   double  x, y, z;
   boolean RetStatus = FALSE;   
   
   OS_printf("\nCTRLTBL.MoiCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
             CtrlTbl->ObjLoadCnt, CtrlTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValDouble(JSON_OBJ, TokenIdx, "x", &x)) AxisCnt++;
   if (JSON_GetValDouble(JSON_OBJ, TokenIdx, "y", &y)) AxisCnt++;
   if (JSON_GetValDouble(JSON_OBJ, TokenIdx, "z", &z)) AxisCnt++;
   
   if (AxisCnt == 3) {
   
      CtrlTbl->Data.Moi.x = x;
      CtrlTbl->Data.Moi.y = y;
      CtrlTbl->Data.Moi.z = z;

      CtrlTbl->ObjLoadCnt++;
      RetStatus = TRUE;
	  
      OS_printf("CTRLTBL.MoiCallback: %f, %f, %f\n",CtrlTbl->Data.Moi.x,CtrlTbl->Data.Moi.y,CtrlTbl->Data.Moi.z);
   
   } /* End if AxisCnt == 3 */
   else {
	   
      CtrlTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(CTRLTBL_LOAD_MOI_ERR_EID, CFE_EVS_ERROR, "Invalid number of MOI axes %d. Should be 3.",
                        AxisCnt);
   
   } /* End if AxisCnt != 3 */
      
   return RetStatus;

} /* MoiCallback() */



/******************************************************************************
** Function: PdGainParamCallback
**
** Parameters used to compute the PD controller gains.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
*/
boolean PdGainParamCallback (int TokenIdx)
{

   int     AxisCnt=0;
   double  w, z;
   boolean RetStatus = FALSE;   
   
   OS_printf("\nCTRLTBL.PdGainParamCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
             CtrlTbl->ObjLoadCnt, CtrlTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValDouble(JSON_OBJ, TokenIdx, "w", &w)) AxisCnt++;
   if (JSON_GetValDouble(JSON_OBJ, TokenIdx, "z", &z)) AxisCnt++;
   
   if (AxisCnt == 2)
   {
	   
      CtrlTbl->Data.PdGainParam.w = w;
      CtrlTbl->Data.PdGainParam.z = z;

      CtrlTbl->ObjLoadCnt++;
      RetStatus = TRUE;
	  
      OS_printf("CTRLTBL.PdGainParamCallback: %f, %f\n",CtrlTbl->Data.PdGainParam.w,CtrlTbl->Data.PdGainParam.z);
   
   } /* End if AxisCnt == 2 */
   else
   {
	   
      CtrlTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(CTRLTBL_LOAD_MOI_ERR_EID, CFE_EVS_ERROR, "Invalid number of PD Gain Parameters %d. Should be 2.",
                        AxisCnt);
						
   } /* End if AxisCnt != 2 */
      
   return RetStatus;

} /* PdGainParamCallback() */


/******************************************************************************
** Function: WhlTgtMomLimCallback
**
** Wheel target momentum limits.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
*/
boolean WhlTgtMomLimCallback (int TokenIdx)
{

   int     LimCnt=0;
   double  Lower, Upper;
   boolean RetStatus = FALSE;   
   
   OS_printf("\nCTRLTBL.WhlTgtMomLimCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
             CtrlTbl->ObjLoadCnt, CtrlTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValDouble(JSON_OBJ, TokenIdx, "lower", &Lower)) LimCnt++;
   if (JSON_GetValDouble(JSON_OBJ, TokenIdx, "upper", &Upper)) LimCnt++;
   
   if (LimCnt == 2)
   {
	   
      CtrlTbl->Data.WhlTgtMomLim.Lower = Lower;
      CtrlTbl->Data.WhlTgtMomLim.Upper = Upper;

      CtrlTbl->ObjLoadCnt++;
      RetStatus = TRUE;
	  
      OS_printf("CTRLTBL.WhlTgtMomLimCallback: %f, %f\n",CtrlTbl->Data.WhlTgtMomLim.Lower,CtrlTbl->Data.WhlTgtMomLim.Upper);
   
   } /* End if LimCnt == 2 */
   else
   {
	   
      CtrlTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(CTRLTBL_LOAD_WHL_TGT_MOM_LIM_ERR_EID, CFE_EVS_ERROR, 
                        "Invalid number of wheel target momentum limit parameters %d. Should be 2.",
                        LimCnt);
						
   } /* End if LimCnt != 2 */
      
   return RetStatus;

} /* WhlTgtMomLimCallback() */

/* end of file */
