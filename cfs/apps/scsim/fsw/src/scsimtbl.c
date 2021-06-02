/*
** Purpose: Implement the spacecraft simulator table.
**
** Notes:
**   None
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
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
#include "scsimtbl.h"


#define  JSON  &(ScSimTbl->Json)   /* Convenience macro */

/*
** Type Definitions
*/


/*
** Global File Data
*/

static SCSIMTBL_Class* ScSimTbl = NULL;

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
boolean AcsCallback  (void* UserData, int TokenIdx);
boolean CdhCallback  (void* UserData, int TokenIdx);
boolean CommCallback (void* UserData, int TokenIdx);
boolean FswCallback  (void* UserData, int TokenIdx);
boolean PwrCallback  (void* UserData, int TokenIdx);
boolean ThrmCallback (void* UserData, int TokenIdx);


/******************************************************************************
** Function: SCSIMTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void SCSIMTBL_Constructor(SCSIMTBL_Class* ObjPtr,
                          SCSIMTBL_GetTblPtr    GetTblPtrFunc,
                          SCSIMTBL_LoadTbl      LoadTblFunc, 
                          SCSIMTBL_LoadTblEntry LoadTblEntryFunc)
{

   ScSimTbl = ObjPtr;

   CFE_PSP_MemSet(ScSimTbl, 0, sizeof(SCSIMTBL_Class));

   ScSimTbl->GetTblPtrFunc    = GetTblPtrFunc;
   ScSimTbl->LoadTblFunc      = LoadTblFunc;
   ScSimTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_Constructor(JSON, ScSimTbl->JsonFileBuf, ScSimTbl->JsonFileTokens);

   JSON_ObjConstructor(&(ScSimTbl->JsonObj[SCSIMTBL_OBJ_ACS]),
                       SCSIMTBL_OBJ_ACS_NAME,
                       AcsCallback,
                       (void *)&(ScSimTbl->Data.Acs));
   JSON_RegContainerCallback(JSON, &(ScSimTbl->JsonObj[SCSIMTBL_OBJ_ACS]));

   JSON_ObjConstructor(&(ScSimTbl->JsonObj[SCSIMTBL_OBJ_CDH]),
                       SCSIMTBL_OBJ_CDH_NAME,
                       CdhCallback,
                       (void *)&(ScSimTbl->Data.Cdh));
   JSON_RegContainerCallback(JSON, &(ScSimTbl->JsonObj[SCSIMTBL_OBJ_CDH]));

   JSON_ObjConstructor(&(ScSimTbl->JsonObj[SCSIMTBL_OBJ_COMM]),
                       SCSIMTBL_OBJ_COMM_NAME,
                       CommCallback,
                       (void *)&(ScSimTbl->Data.Comm));
   JSON_RegContainerCallback(JSON, &(ScSimTbl->JsonObj[SCSIMTBL_OBJ_COMM]));

   JSON_ObjConstructor(&(ScSimTbl->JsonObj[SCSIMTBL_OBJ_FSW]),
                       SCSIMTBL_OBJ_FSW_NAME,
                       FswCallback,
                       (void *)&(ScSimTbl->Data.Fsw));
   JSON_RegContainerCallback(JSON, &(ScSimTbl->JsonObj[SCSIMTBL_OBJ_FSW]));

   JSON_ObjConstructor(&(ScSimTbl->JsonObj[SCSIMTBL_OBJ_PWR]),
                       SCSIMTBL_OBJ_PWR_NAME,
                       PwrCallback,
                       (void *)&(ScSimTbl->Data.Pwr));
   JSON_RegContainerCallback(JSON, &(ScSimTbl->JsonObj[SCSIMTBL_OBJ_PWR]));

   JSON_ObjConstructor(&(ScSimTbl->JsonObj[SCSIMTBL_OBJ_THRM]),
                       SCSIMTBL_OBJ_THRM_NAME,
                       ThrmCallback,
                       (void *)&(ScSimTbl->Data.Thrm));
   JSON_RegContainerCallback(JSON, &(ScSimTbl->JsonObj[SCSIMTBL_OBJ_THRM]));

} /* End SCSIMTBL_Constructor() */


/******************************************************************************
** Function: SCSIMTBL_ResetStatus
**
*/
void SCSIMTBL_ResetStatus(void)
{

   int i;
   
   ScSimTbl->LastLoadStatus = TBLMGR_STATUS_UNDEF;
   ScSimTbl->AttrErrCnt     = 0;
   ScSimTbl->ObjLoadCnt     = 0;
   
   for (i=0; i < SCSIMTBL_OBJ_CNT; i++) ScSimTbl->JsonObj[i].Modified = FALSE;
   
 
} /* End SCSIMTBL_ResetStatus() */


/******************************************************************************
** Function: SCSIMTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean SCSIMTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int i;
   
   CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE, "SCSIMTBL_LoadCmd() Entry\n");
   
   SCSIMTBL_ResetStatus();  /* Reset status & object modified flags */
   
   if (JSON_OpenFile(JSON, Filename)) {
  
      CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE, "SCSIMTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
      JSON_ProcessTokens(JSON);

	   /* 
	   ** ObjLoadCnt is used as a crude sanity check for REPLACE & UPDATE. Tighter
      ** checks such as verifying one of each JSON object is received for a REPLACE
      ** could be done if desired.	  
	   */
      if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
         if (ScSimTbl->ObjLoadCnt == SCSIMTBL_OBJ_CNT) {
			 
            ScSimTbl->LastLoadStatus = ((ScSimTbl->LoadTblFunc)(&(ScSimTbl->Data)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

         } /* End if valid object count */
         else {
            
            CFE_EVS_SendEvent(SCSIMTBL_CMD_LOAD_REPLACE_ERR_EID,CFE_EVS_ERROR,
			                     "SCSIMTBL: Replace table command rejected. File contained %d objects when expecting %d",
							         ScSimTbl->ObjLoadCnt, SCSIMTBL_OBJ_CNT);
         
         } /* End if not all objects loaded for a replace table command */
		 
	  } /* End if replace entire table */
      else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
         if (ScSimTbl->ObjLoadCnt > 0 && ScSimTbl->ObjLoadCnt <= SCSIMTBL_OBJ_CNT) {
			 
		      ScSimTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
   
            for (i=0; i < SCSIMTBL_OBJ_CNT; i++) {

               if (ScSimTbl->JsonObj[i].Modified) {
                  if (!(ScSimTbl->LoadTblEntryFunc)(i, ScSimTbl->JsonObj[i].UserData))
                     ScSimTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
               }

            } /* End entry loop */
			
         } /* End if valid object count */
         else {
            
            CFE_EVS_SendEvent(SCSIMTBL_CMD_LOAD_UPDATE_ERR_EID,CFE_EVS_ERROR,
			                     "SCSIMTBL: Update table command rejected. File contained %d objects when expecting 1 to %d",
							         ScSimTbl->ObjLoadCnt, SCSIMTBL_OBJ_CNT);
         
         } /* End if not all objects loaded for a replace table command */

      } /* End if update individual records */
      else {
      
         CFE_EVS_SendEvent(SCSIMTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"SCSIMTBL: Invalid table command load type %d",LoadType);
      
      }
      
   } /* End if valid file */
   else {
   
      CFE_EVS_SendEvent(SCSIMTBL_CMD_LOAD_OPEN_ERR_EID,CFE_EVS_ERROR,"SCSIMTBL: Table open failure for file %s. File Status = %s JSMN Status = %s",
	                     Filename, JSON_GetFileStatusStr(ScSimTbl->Json.FileStatus), JSON_GetJsmnErrStr(ScSimTbl->Json.JsmnStatus));
   }
    
    
   return (ScSimTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of SCSIMTBL_LoadCmd() */


/******************************************************************************
** Function: SCSIMTBL_DumpCmd
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
boolean SCSIMTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   int32    FileHandle;
   char     DumpRecord[256];
   char     SysTimeStr[256];
   const SCSIMTBL_Struct *ScSimTblPtr;

   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS)
   {

      ScSimTblPtr = (ScSimTbl->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"Spacecraft Simulator Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      sprintf(DumpRecord,"\"description\": \"SCSIM table dumped at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* ACS */
      sprintf(DumpRecord,"\"%s\": {\n",ScSimTbl->JsonObj[SCSIMTBL_OBJ_ACS].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"tbd-1\": %d,\n", ScSimTblPtr->Acs.Tbd1);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"tbd-2\": %d\n},\n", ScSimTblPtr->Acs.Tbd2);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* CDH */
      sprintf(DumpRecord,"\"%s\": {\n",ScSimTbl->JsonObj[SCSIMTBL_OBJ_CDH].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"tbd-1\": %d,\n", ScSimTblPtr->Cdh.Tbd1);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"tbd-2\": %d\n},\n", ScSimTblPtr->Cdh.Tbd2);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* Comm */
      sprintf(DumpRecord,"\"%s\": {\n",ScSimTbl->JsonObj[SCSIMTBL_OBJ_COMM].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"tbd-1\": %d,\n", ScSimTblPtr->Comm.Tbd1);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"tbd-2\": %d\n},\n", ScSimTblPtr->Comm.Tbd2);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* Power */
      sprintf(DumpRecord,"\"%s\": {\n",ScSimTbl->JsonObj[SCSIMTBL_OBJ_PWR].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"tbd-1\": %d,\n", ScSimTblPtr->Pwr.Tbd1);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"tbd-2\": %d\n},\n", ScSimTblPtr->Pwr.Tbd2);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* Thermal */
      sprintf(DumpRecord,"\"%s\": {\n",ScSimTbl->JsonObj[SCSIMTBL_OBJ_THRM].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"tbd-1\": %d,\n", ScSimTblPtr->Thrm.Tbd1);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"tbd-2\": %d\n}\n", ScSimTblPtr->Thrm.Tbd2);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
			  
      sprintf(DumpRecord,"}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* TODO - Add addition meta data when file dumped */
      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else
   {
   
      CFE_EVS_SendEvent(SCSIMTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of SCSIMTBL_DumpCmd() */


/******************************************************************************
** Function: AcsCallback
**
** Process ACS table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
boolean AcsCallback (void* UserData, int TokenIdx)
{

   int     AttributeCnt = 0;
   int     Tbd1;
   int     Tbd2;
   
   ScSimTbl->JsonObj[SCSIMTBL_OBJ_ACS].Modified = FALSE;   
   
   CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE, 
                     "\nSCSIMTBL.AcsCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     ScSimTbl->ObjLoadCnt, ScSimTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-1", &Tbd1)) AttributeCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-2", &Tbd2)) AttributeCnt++;
   
   if (AttributeCnt == 2) {
   
      ScSimTbl->Data.Acs.Tbd1 = Tbd1;
      ScSimTbl->Data.Acs.Tbd2 = Tbd2;

      ScSimTbl->ObjLoadCnt++;
      ScSimTbl->JsonObj[SCSIMTBL_OBJ_ACS].Modified = TRUE;
	  
      CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE,  "SCSIMTBL.AcsCallback: Tbd1 = %d, Tbd2 = %d\n",
                        ScSimTbl->Data.Acs.Tbd1, ScSimTbl->Data.Acs.Tbd2);
   
   } /* End if valid AttributeCnt */
   else {
	   
      ScSimTbl->AttrErrCnt++;
      CFE_EVS_SendEvent(SCSIMTBL_LOAD_ACS_ERR_EID, CFE_EVS_ERROR, "Invalid number of attributes %d. Should be 2.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return ScSimTbl->JsonObj[SCSIMTBL_OBJ_ACS].Modified;

} /* AcsCallback() */


/******************************************************************************
** Function: CdhCallback
**
** Process CDH table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
boolean CdhCallback (void* UserData, int TokenIdx)
{

   int     AttributeCnt = 0;
   int     Tbd1;
   int     Tbd2;
   
   ScSimTbl->JsonObj[SCSIMTBL_OBJ_CDH].Modified = FALSE;   
   
   CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE, 
                     "\nSCSIMTBL.CdhCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     ScSimTbl->ObjLoadCnt, ScSimTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-1", &Tbd1)) AttributeCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-2", &Tbd2)) AttributeCnt++;
   
   if (AttributeCnt == 2) {
   
      ScSimTbl->Data.Cdh.Tbd1 = Tbd1;
      ScSimTbl->Data.Cdh.Tbd2 = Tbd2;

      ScSimTbl->ObjLoadCnt++;
      ScSimTbl->JsonObj[SCSIMTBL_OBJ_CDH].Modified = TRUE;
	  
      CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE,  "SCSIMTBL.CdhCallback: Tbd1 = %d, Tbd2 = %d\n",
                        ScSimTbl->Data.Cdh.Tbd1, ScSimTbl->Data.Cdh.Tbd2);
   
   } /* End if valid AttributeCnt */
   else {
	   
      ScSimTbl->AttrErrCnt++;
      CFE_EVS_SendEvent(SCSIMTBL_LOAD_CDH_ERR_EID, CFE_EVS_ERROR, "Invalid number of attributes %d. Should be 2.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return ScSimTbl->JsonObj[SCSIMTBL_OBJ_CDH].Modified;

} /* CdhCallback() */


/******************************************************************************
** Function: CommCallback
**
** Process Comm table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
boolean CommCallback (void* UserData, int TokenIdx)
{

   int     AttributeCnt = 0;
   int     Tbd1;
   int     Tbd2;
   
   ScSimTbl->JsonObj[SCSIMTBL_OBJ_COMM].Modified = FALSE;   
   
   CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE, 
                     "\nSCSIMTBL.CommCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     ScSimTbl->ObjLoadCnt, ScSimTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-1", &Tbd1)) AttributeCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-2", &Tbd2)) AttributeCnt++;
   
   if (AttributeCnt == 2) {
   
      ScSimTbl->Data.Comm.Tbd1 = Tbd1;
      ScSimTbl->Data.Comm.Tbd2 = Tbd2;

      ScSimTbl->ObjLoadCnt++;
      ScSimTbl->JsonObj[SCSIMTBL_OBJ_COMM].Modified = TRUE;
	  
      CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE,  "SCSIMTBL.CommCallback: Tbd1 = %d, Tbd2 = %d\n",
                        ScSimTbl->Data.Comm.Tbd1, ScSimTbl->Data.Comm.Tbd2);
   
   } /* End if valid AttributeCnt */
   else {
	   
      ScSimTbl->AttrErrCnt++;
      CFE_EVS_SendEvent(SCSIMTBL_LOAD_COMM_ERR_EID, CFE_EVS_ERROR, "Invalid number of attributes %d. Should be 2.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return ScSimTbl->JsonObj[SCSIMTBL_OBJ_COMM].Modified;

} /* CommCallback() */


/******************************************************************************
** Function: FswCallback
**
** Process Fsw table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
boolean FswCallback (void* UserData, int TokenIdx)
{

   int     AttributeCnt = 0;
   int     Tbd1;
   int     Tbd2;
   
   ScSimTbl->JsonObj[SCSIMTBL_OBJ_FSW].Modified = FALSE;   
   
   CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE, 
                     "\nSCSIMTBL.FswCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     ScSimTbl->ObjLoadCnt, ScSimTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-1", &Tbd1)) AttributeCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-2", &Tbd2)) AttributeCnt++;
   
   if (AttributeCnt == 2) {
   
      ScSimTbl->Data.Pwr.Tbd1 = Tbd1;
      ScSimTbl->Data.Pwr.Tbd2 = Tbd2;

      ScSimTbl->ObjLoadCnt++;
      ScSimTbl->JsonObj[SCSIMTBL_OBJ_FSW].Modified = TRUE;
	  
      CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE,  "SCSIMTBL.FswCallback: Tbd1 = %d, Tbd2 = %d\n",
                        ScSimTbl->Data.Fsw.Tbd1, ScSimTbl->Data.Fsw.Tbd2);
   
   } /* End if valid AttributeCnt */
   else {
	   
      ScSimTbl->AttrErrCnt++;
      CFE_EVS_SendEvent(SCSIMTBL_LOAD_FSW_ERR_EID, CFE_EVS_ERROR, "Invalid number of attributes %d. Should be 2.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return ScSimTbl->JsonObj[SCSIMTBL_OBJ_FSW].Modified;

} /* FswCallback() */


/******************************************************************************
** Function: PwrCallback
**
** Process Pwr table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
boolean PwrCallback (void* UserData, int TokenIdx)
{

   int     AttributeCnt = 0;
   int     Tbd1;
   int     Tbd2;
   
   ScSimTbl->JsonObj[SCSIMTBL_OBJ_PWR].Modified = FALSE;   
   
   CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE, 
                     "\nSCSIMTBL.PwrCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     ScSimTbl->ObjLoadCnt, ScSimTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-1", &Tbd1)) AttributeCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-2", &Tbd2)) AttributeCnt++;
   
   if (AttributeCnt == 2) {
   
      ScSimTbl->Data.Pwr.Tbd1 = Tbd1;
      ScSimTbl->Data.Pwr.Tbd2 = Tbd2;

      ScSimTbl->ObjLoadCnt++;
      ScSimTbl->JsonObj[SCSIMTBL_OBJ_PWR].Modified = TRUE;
	  
      CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE,  "SCSIMTBL.PwrCallback: Tbd1 = %d, Tbd2 = %d\n",
                        ScSimTbl->Data.Pwr.Tbd1, ScSimTbl->Data.Pwr.Tbd2);
   
   } /* End if valid AttributeCnt */
   else {
	   
      ScSimTbl->AttrErrCnt++;
      CFE_EVS_SendEvent(SCSIMTBL_LOAD_PWR_ERR_EID, CFE_EVS_ERROR, "Invalid number of attributes %d. Should be 2.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return ScSimTbl->JsonObj[SCSIMTBL_OBJ_PWR].Modified;

} /* PwrCallback() */


/******************************************************************************
** Function: ThrmCallback
**
** Process Thrm table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
boolean ThrmCallback (void* UserData, int TokenIdx)
{

   int     AttributeCnt = 0;
   int     Tbd1;
   int     Tbd2;
   
   ScSimTbl->JsonObj[SCSIMTBL_OBJ_THRM].Modified = FALSE;   
   
   CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE, 
                     "\nSCSIMTBL.ThrmCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     ScSimTbl->ObjLoadCnt, ScSimTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-1", &Tbd1)) AttributeCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "tbd-2", &Tbd2)) AttributeCnt++;
   
   if (AttributeCnt == 2) {
   
      ScSimTbl->Data.Thrm.Tbd1 = Tbd1;
      ScSimTbl->Data.Thrm.Tbd2 = Tbd2;

      ScSimTbl->ObjLoadCnt++;
      ScSimTbl->JsonObj[SCSIMTBL_OBJ_THRM].Modified = TRUE;
	  
      CFE_EVS_SendEvent(SCSIM_INIT_DEBUG_EID, SCSIM_INIT_EVS_TYPE,  "SCSIMTBL.ThrmCallback: Tbd1 = %d, Tbd2 = %d\n",
                        ScSimTbl->Data.Thrm.Tbd1, ScSimTbl->Data.Thrm.Tbd2);
   
   } /* End if valid AttributeCnt */
   else {
	   
      ScSimTbl->AttrErrCnt++;
      CFE_EVS_SendEvent(SCSIMTBL_LOAD_THRM_ERR_EID, CFE_EVS_ERROR, "Invalid number of attributes %d. Should be 2.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return ScSimTbl->JsonObj[SCSIMTBL_OBJ_THRM].Modified;

} /* ThrmCallback() */
