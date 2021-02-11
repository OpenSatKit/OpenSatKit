/*
** Purpose: Implement Standalone Controller Table.
**
** Notes:
**   None
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General
**   Public License (GPL).
**
*/

/*
** Include Files:
*/

#include <string.h>
#include "ctrltbl.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define JSON  &(CtrlTbl->Json)  /* Convenience macro */


/**********************/
/** Type Definitions **/
/**********************/

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
static boolean KpCallback (void* UserData, int TokenIdx);
static boolean KrCallback (void* UserData, int TokenIdx);
static boolean KunlCallback (void* UserData, int TokenIdx);
static boolean HcmdLimCallback (void* UserData, int TokenIdx);

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

   JSON_Constructor(JSON, CtrlTbl->JsonFileBuf, CtrlTbl->JsonFileTokens);
   
   JSON_ObjConstructor(&(CtrlTbl->JsonObj[CTRLTBL_OBJ_KP]),
                       CTRLTBL_OBJ_NAME_KP,
                       KpCallback,
                       (void *)&(CtrlTbl->Data.Kp));
   JSON_RegContainerCallback(JSON, &(CtrlTbl->JsonObj[CTRLTBL_OBJ_KP]));

   JSON_ObjConstructor(&(CtrlTbl->JsonObj[CTRLTBL_OBJ_KR]),
                       CTRLTBL_OBJ_NAME_KR,
                       KrCallback,
                       (void *)&(CtrlTbl->Data.Kr));
   JSON_RegContainerCallback(JSON, &(CtrlTbl->JsonObj[CTRLTBL_OBJ_KR]));

   JSON_ObjConstructor(&(CtrlTbl->JsonObj[CTRLTBL_OBJ_KUNL]),
                       CTRLTBL_OBJ_NAME_KUNL,
                       KunlCallback,
                       (void *)&(CtrlTbl->Data.Kunl));
   JSON_RegContainerCallback(JSON, &(CtrlTbl->JsonObj[CTRLTBL_OBJ_KUNL]));

   JSON_ObjConstructor(&(CtrlTbl->JsonObj[CTRLTBL_OBJ_HCMD_LIM]),
                       CTRLTBL_OBJ_NAME_HCMD_LIM,
                       HcmdLimCallback,
                       (void *)&(CtrlTbl->Data.HcmdLim));
   JSON_RegContainerCallback(JSON, &(CtrlTbl->JsonObj[CTRLTBL_OBJ_HCMD_LIM]));

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
   
   CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE, "CTRLTBL_LoadCmd() Entry\n");
   
   CTRLTBL_ResetStatus();  /* Reset status & object modified flags */
   
   if (JSON_OpenFile(JSON, Filename)) {
  
      CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE, "CTRLTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
      //DEBUG JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      //DEBUG JSON_PrintTokens(&Json,50);
  
      JSON_ProcessTokens(JSON);

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
                  if (!(CtrlTbl->LoadTblEntryFunc)(i, CtrlTbl->JsonObj[i].UserData))
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
   char     SysTimeStr[256];
   const CTRLTBL_Struct *CtrlTblPtr;

   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS) {

      CtrlTblPtr = (CtrlTbl->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"42 Standalone App Controller Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      sprintf(DumpRecord,"\"description\": \"F42 controller gains dumped at %s\",\n", SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* Kp */
      sprintf(DumpRecord,"\"%s\": {\n",CtrlTbl->JsonObj[CTRLTBL_OBJ_KP].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"x\": %f,\n   \"y\": %f,\n   \"z\": %f\n},\n",
              CtrlTblPtr->Kp.X, CtrlTblPtr->Kp.Y, CtrlTblPtr->Kp.Z);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
			  
      /* Kr */      
      sprintf(DumpRecord,"\"%s\": {\n",CtrlTbl->JsonObj[CTRLTBL_OBJ_KR].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"x\": %f,\n   \"y\": %f,\n   \"z\": %f\n},\n",
                CtrlTblPtr->Kr.X, CtrlTblPtr->Kr.Y, CtrlTblPtr->Kr.Z);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* Kunl*/
      sprintf(DumpRecord,"\"%s\": {\n\"k\": %f\n},\n",
              CtrlTbl->JsonObj[CTRLTBL_OBJ_KUNL].Name,CtrlTblPtr->Kunl);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* Hcmd*/
      sprintf(DumpRecord,"\"%s\": {\n",CtrlTbl->JsonObj[CTRLTBL_OBJ_HCMD_LIM].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"lower\": %f,\n   \"upper\": %f\n}\n",
              CtrlTblPtr->HcmdLim.Lower, CtrlTblPtr->HcmdLim.Upper);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* TODO - Add additional meta data when file dumped */
      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(CTRLTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of CTRLTBL_DumpCmd() */


/******************************************************************************
** Function: KpCallback
**
** Process a Kp table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
static boolean KpCallback (void* UserData, int TokenIdx)
{

   int     AxisCnt=0;
   double  x, y, z;
   
   
   CtrlTbl->JsonObj[CTRLTBL_OBJ_KP].Modified = FALSE;   
   
   CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE, 
                     "\nCTRLTBL.KpCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     CtrlTbl->ObjLoadCnt, CtrlTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValDouble(JSON, TokenIdx, "x", &x)) AxisCnt++;
   if (JSON_GetValDouble(JSON, TokenIdx, "y", &y)) AxisCnt++;
   if (JSON_GetValDouble(JSON, TokenIdx, "z", &z)) AxisCnt++;
   
   if (AxisCnt == 3) {
   
      CtrlTbl->Data.Kp.X = x;
      CtrlTbl->Data.Kp.Y = y;
      CtrlTbl->Data.Kp.Z = z;

      CtrlTbl->ObjLoadCnt++;
      CtrlTbl->JsonObj[CTRLTBL_OBJ_KP].Modified = TRUE;
	  
      CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE,  "CTRLTBL.KpCallback: %f, %f, %f\n",
                        CtrlTbl->Data.Kp.X,CtrlTbl->Data.Kp.Y,CtrlTbl->Data.Kp.Z);
   
   } /* End if AxisCnt == 3 */
   else {
	   
      CtrlTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(CTRLTBL_LOAD_KP_ERR_EID, CFE_EVS_ERROR, "Invalid number of Kp axes %d. Should be 3.",
                        AxisCnt);
   
   } /* End if AxisCnt != 3 */
      
   return CtrlTbl->JsonObj[CTRLTBL_OBJ_KP].Modified;

} /* KpCallback() */


/******************************************************************************
** Function: KrCallback
**
** Process a Kr table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
static boolean KrCallback (void* UserData, int TokenIdx)
{

   int     AxisCnt=0;
   double  x, y, z;
   
   
   CtrlTbl->JsonObj[CTRLTBL_OBJ_KR].Modified = FALSE;   
   
   CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE, 
                     "\nCTRLTBL.KrCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     CtrlTbl->ObjLoadCnt, CtrlTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValDouble(JSON, TokenIdx, "x", &x)) AxisCnt++;
   if (JSON_GetValDouble(JSON, TokenIdx, "y", &y)) AxisCnt++;
   if (JSON_GetValDouble(JSON, TokenIdx, "z", &z)) AxisCnt++;
   
   if (AxisCnt == 3) {
   
      CtrlTbl->Data.Kr.X = x;
      CtrlTbl->Data.Kr.Y = y;
      CtrlTbl->Data.Kr.Z = z;

      CtrlTbl->ObjLoadCnt++;
      CtrlTbl->JsonObj[CTRLTBL_OBJ_KR].Modified = TRUE;
	  
      CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE,  "CTRLTBL.KrCallback: %f, %f, %f\n",
                        CtrlTbl->Data.Kr.X,CtrlTbl->Data.Kr.Y,CtrlTbl->Data.Kr.Z);
   
   } /* End if AxisCnt == 3 */
   else {
	   
      CtrlTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(CTRLTBL_LOAD_KP_ERR_EID, CFE_EVS_ERROR, "Invalid number of Kr axes %d. Should be 3.",
                        AxisCnt);
   
   } /* End if AxisCnt != 3 */
      
   return CtrlTbl->JsonObj[CTRLTBL_OBJ_KR].Modified;

} /* KrCallback() */


/******************************************************************************
** Function: KunlCallback
**
** Process a Kunl table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
static boolean KunlCallback (void* UserData, int TokenIdx)
{

   int     EntryCnt=0;
   double  k;
   
   
   CtrlTbl->JsonObj[CTRLTBL_OBJ_KUNL].Modified = FALSE;   
   
   CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE, 
                     "\nCTRLTBL.KunlCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     CtrlTbl->ObjLoadCnt, CtrlTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValDouble(JSON, TokenIdx, "k", &k)) EntryCnt++;
   
   if (EntryCnt == 1) {
	   
      CtrlTbl->Data.Kunl = k;
      CtrlTbl->ObjLoadCnt++;
      CtrlTbl->JsonObj[CTRLTBL_OBJ_KUNL].Modified = TRUE;
	  
      CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE,
                        "CTRLTBL.KunlCallback: %f\n", CtrlTbl->Data.Kunl);
   
   } /* End if EntryCnt == 1 */
   else {
	   
      CtrlTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(CTRLTBL_LOAD_KUNL_ERR_EID, CFE_EVS_ERROR, "Invalid number of Kunl Parameters %d. Should be 1.",
                       EntryCnt);
						
   } /* End if EntryCnt != 1 */
      
   return CtrlTbl->JsonObj[CTRLTBL_OBJ_KUNL].Modified;

} /* KunlCallback() */


/******************************************************************************
** Function: HcmdLimCallback
**
** Wheel target momentum limits.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
boolean HcmdLimCallback (void* UserData, int TokenIdx)
{

   int     LimCnt=0;
   double  Lower, Upper;
   
   CtrlTbl->JsonObj[CTRLTBL_OBJ_HCMD_LIM].Modified = FALSE;
   CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE,
                     "\nCTRLTBL.HcmdCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     CtrlTbl->ObjLoadCnt, CtrlTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValDouble(JSON, TokenIdx, "lower", &Lower)) LimCnt++;
   if (JSON_GetValDouble(JSON, TokenIdx, "upper", &Upper)) LimCnt++;
   
   if (LimCnt == 2)
   {
	   
      CtrlTbl->Data.HcmdLim.Lower = Lower;
      CtrlTbl->Data.HcmdLim.Upper = Upper;

      CtrlTbl->ObjLoadCnt++;
      CtrlTbl->JsonObj[CTRLTBL_OBJ_HCMD_LIM].Modified = TRUE;
	  
      CFE_EVS_SendEvent(F42_INIT_DEBUG_EID, F42_INIT_EVS_TYPE, 
                        "CTRLTBL.HcmdLimCallback: %f, %f\n",
                        CtrlTbl->Data.HcmdLim.Lower,CtrlTbl->Data.HcmdLim.Upper);
   
   } /* End if LimCnt == 2 */
   else
   {
	   
      CtrlTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(CTRLTBL_LOAD_HCMD_ERR_EID, CFE_EVS_ERROR, 
                        "Invalid number of wheel target momentum limit parameters %d. Should be 2.",
                        LimCnt);
						
   } /* End if LimCnt != 2 */
      
   return CtrlTbl->JsonObj[CTRLTBL_OBJ_HCMD_LIM].Modified;

} /* HcmdLimCallback() */

