/*
** Purpose: Implement the ISIM table.
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
#include "isimtbl.h"

/* Convenience macro */
#define  JSON_OBJ  &(IsimTbl->Json)

/*
** Type Definitions
*/


/*
** Global File Data
*/

static ISIMTBL_Class* IsimTbl = NULL;

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
boolean InstrumentCallback (int TokenIdx);
boolean SciFileCallback (int TokenIdx);


/******************************************************************************
** Function: ISIMTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void ISIMTBL_Constructor(ISIMTBL_Class* ObjPtr,
                         ISIMTBL_GetTblPtr    GetTblPtrFunc,
                         ISIMTBL_LoadTbl      LoadTblFunc, 
                         ISIMTBL_LoadTblEntry LoadTblEntryFunc)
{

   IsimTbl = ObjPtr;

   CFE_PSP_MemSet(IsimTbl, 0, sizeof(ISIMTBL_Class));

   IsimTbl->GetTblPtrFunc    = GetTblPtrFunc;
   IsimTbl->LoadTblFunc      = LoadTblFunc;
   IsimTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_ObjConstructor(&(IsimTbl->JsonObj[ISIMTBL_OBJ_INSTRUMENT]),
                       ISIMTBL_OBJ_INSTRMENT_NAME,
                       InstrumentCallback,
                       (void *)&(IsimTbl->Data.Instrument));

   JSON_ObjConstructor(&(IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_FILE]),
                       ISIMTBL_OBJ_SCI_FILE_NAME,
                       SciFileCallback,
                       (void *)&(IsimTbl->Data.SciFile));

} /* End ISIMTBL_Constructor() */


/******************************************************************************
** Function: ISIMTBL_ResetStatus
**
*/
void ISIMTBL_ResetStatus(void)
{

   int i;
   
   IsimTbl->LastLoadStatus = TBLMGR_STATUS_UNDEF;
   IsimTbl->AttrErrCnt     = 0;
   IsimTbl->ObjLoadCnt     = 0;
   
   for (i=0; i < ISIMTBL_OBJ_CNT; i++) IsimTbl->JsonObj[i].Modified = FALSE;
   
 
} /* End ISIMTBL_ResetStatus() */


/******************************************************************************
** Function: ISIMTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean ISIMTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int i;
   
   CFE_EVS_SendEvent(ISIM_INIT_DEBUG_EID, ISIM_INIT_EVS_TYPE, "ISIMTBL_LoadCmd() Entry\n");
   
   ISIMTBL_ResetStatus();  /* Reset status & object modified flags */

   JSON_Constructor(JSON_OBJ, IsimTbl->JsonFileBuf, IsimTbl->JsonFileTokens);
   
   if (JSON_OpenFile(JSON_OBJ, Filename)) {
  
      CFE_EVS_SendEvent(ISIM_INIT_DEBUG_EID, ISIM_INIT_EVS_TYPE, "ISIMTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
      //DEBUG JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      //DEBUG JSON_PrintTokens(&Json,50);
  
      JSON_RegContainerCallback(JSON_OBJ,
	                             IsimTbl->JsonObj[ISIMTBL_OBJ_INSTRUMENT].Name,
	                             IsimTbl->JsonObj[ISIMTBL_OBJ_INSTRUMENT].Callback);
      JSON_RegContainerCallback(JSON_OBJ,
	                             IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_FILE].Name,
	                             IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_FILE].Callback);

      JSON_ProcessTokens(JSON_OBJ);

	  /* 
	  ** ObjLoadCnt is used as a crude sanity check for REPLACE & UPDATE. Tighter
     ** checks such as verifying one of each JSON object is received for a REPLACE
     ** could be done if desired.	  
	  */
      if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
         if (IsimTbl->ObjLoadCnt == ISIMTBL_OBJ_CNT) {
			 
            IsimTbl->LastLoadStatus = ((IsimTbl->LoadTblFunc)(&(IsimTbl->Data)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

         } /* End if valid object count */
         else {
            CFE_EVS_SendEvent(ISIMTBL_CMD_LOAD_REPLACE_ERR_EID,CFE_EVS_ERROR,
			                     "ISIMTBL: Replace table command rejected. File contained %d objects when expecting %d",
							         IsimTbl->ObjLoadCnt, ISIMTBL_OBJ_CNT);
         } /* End if not all objects loaded for a replace table command */
		 
	  } /* End if replace entire table */
      else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
         if (IsimTbl->ObjLoadCnt > 0 && IsimTbl->ObjLoadCnt <= ISIMTBL_OBJ_CNT) {
			 
		      IsimTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
   
            for (i=0; i < ISIMTBL_OBJ_CNT; i++) {

               if (IsimTbl->JsonObj[i].Modified) {
                  if (!(IsimTbl->LoadTblEntryFunc)(i, &(IsimTbl->JsonObj[i].Data)))
                     IsimTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
               }

            } /* End entry loop */
			
         } /* End if valid object count */
         else {
            CFE_EVS_SendEvent(ISIMTBL_CMD_LOAD_UPDATE_ERR_EID,CFE_EVS_ERROR,
			                     "ISIMTBL: Update table command rejected. File contained %d objects when expecting 1 to %d",
							         IsimTbl->ObjLoadCnt, ISIMTBL_OBJ_CNT);
         } /* End if not all objects loaded for a replace table command */

      } /* End if update individual records */
      else {
         CFE_EVS_SendEvent(ISIMTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"ISIMTBL: Invalid table command load type %d",LoadType);
      }
      
   } /* End if valid file */
   else {
      CFE_EVS_SendEvent(ISIMTBL_CMD_LOAD_OPEN_ERR_EID,CFE_EVS_ERROR,"ISIMTBL: Table open failure for file %s. File Status = %s JSMN Status = %s",
	                     Filename, JSON_GetFileStatusStr(IsimTbl->Json.FileStatus), JSON_GetJsmnErrStr(IsimTbl->Json.JsmnStatus));
   }
    
    
   return (IsimTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of ISIMTBL_LoadCmd() */


/******************************************************************************
** Function: ISIMTBL_DumpCmd
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
boolean ISIMTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   int32    FileHandle;
   char     DumpRecord[256];
   char     SysTimeStr[256];
   const ISIMTBL_Struct *IsimTblPtr;

   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS)
   {

      IsimTblPtr = (IsimTbl->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"Instrument Simulator Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      sprintf(DumpRecord,"\"description\": \"ISIM table dumped at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* Instrument */
      sprintf(DumpRecord,"\"%s\": {\n",IsimTbl->JsonObj[ISIMTBL_OBJ_INSTRUMENT].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"pwr-init-cycles\": %d\n},\n", IsimTblPtr->Instrument.PwrInitCycles);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
			  
      /* Science File */      
      sprintf(DumpRecord,"\"%s\": {\n",IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_FILE].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"path-base-filename\": \"%s\",\n   \"file-extension\": \"%s\"\n   \"cycles-per-file\": %d\n}\n",
              IsimTblPtr->SciFile.PathBaseFilename, IsimTblPtr->SciFile.FileExtension, IsimTblPtr->SciFile.CyclesPerFile);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* TODO - Add addition meta data when file dumped */
      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else
   {
   
      CFE_EVS_SendEvent(ISIMTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of ISIMTBL_DumpCmd() */


/******************************************************************************
** Function: InstrumentCallback
**
** Process Instrument table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
*/
boolean InstrumentCallback (int TokenIdx)
{

   int     AttributeCnt = 0;
   int     PwrInitCycles;
   boolean RetStatus = FALSE;   
   
   CFE_EVS_SendEvent(ISIM_INIT_DEBUG_EID, ISIM_INIT_EVS_TYPE, 
                     "\nISIMTBL.InstrumentCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     IsimTbl->ObjLoadCnt, IsimTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValShortInt(JSON_OBJ, TokenIdx, "pwr-init-cycles", &PwrInitCycles)) AttributeCnt++;
   
   if (AttributeCnt == 1) {
   
      IsimTbl->Data.Instrument.PwrInitCycles = PwrInitCycles;

      IsimTbl->ObjLoadCnt++;
      RetStatus = TRUE;
	  
      CFE_EVS_SendEvent(ISIM_INIT_DEBUG_EID, ISIM_INIT_EVS_TYPE,  "ISIMTBL.InstrumentCallback: %d\n",
                        IsimTbl->Data.Instrument.PwrInitCycles);
   
   } /* End if valid AttributeCnt */
   else {
	   
      IsimTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(ISIMTBL_LOAD_INSTRUMENT_ERR_EID, CFE_EVS_ERROR, "Invalid number of attributes %d. Should be 1.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return RetStatus;

} /* InstrumentCallback() */



/******************************************************************************
** Function: SciFileCallback
**
** Process Science File table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. No error checking done on string lengths.
*/
boolean SciFileCallback (int TokenIdx)
{

   int     AttributeCnt = 0;
   int     CyclesPerFile;
   char    PathBaseFilename[OS_MAX_PATH_LEN];
   char    FileExtension[ISIM_FILE_EXT_MAX_CHAR];
   boolean RetStatus = FALSE;   
   
   CFE_EVS_SendEvent(ISIM_INIT_DEBUG_EID, ISIM_INIT_EVS_TYPE, 
                     "\nISIMTBL.SciFileCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     IsimTbl->ObjLoadCnt, IsimTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValShortInt(JSON_OBJ, TokenIdx, "cycles-per-file", &CyclesPerFile))  AttributeCnt++;
   if (JSON_GetValStr(JSON_OBJ, TokenIdx, "path-base-filename", PathBaseFilename)) AttributeCnt++;
   if (JSON_GetValStr(JSON_OBJ, TokenIdx, "file-extension", FileExtension))        AttributeCnt++;
   
   if (AttributeCnt == 3) {
	   
      IsimTbl->Data.SciFile.CyclesPerFile = CyclesPerFile;
      strncpy(IsimTbl->Data.SciFile.PathBaseFilename,PathBaseFilename,OS_MAX_PATH_LEN);
      strncpy(IsimTbl->Data.SciFile.FileExtension,FileExtension,ISIM_FILE_EXT_MAX_CHAR);

      IsimTbl->ObjLoadCnt++;
      RetStatus = TRUE;
	  
      CFE_EVS_SendEvent(ISIM_INIT_DEBUG_EID, ISIM_INIT_EVS_TYPE,
                        "ISIMTBL.SciFileCallback: %s, %s, %d\n", 
                        IsimTbl->Data.SciFile.PathBaseFilename,
                        IsimTbl->Data.SciFile.FileExtension,
                        IsimTbl->Data.SciFile.CyclesPerFile);
   
   } /* End if valid AttributeCnt */
   else {
	   
      IsimTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(ISIMTBL_LOAD_SCI_FILE_ERR_EID, CFE_EVS_ERROR, "Invalid number of Science File attributes %d. Should be 3.",
                        AttributeCnt);
						
   } /* End if invalid AttributeCnt */
      
   return RetStatus;

} /* SciFileCallback() */

