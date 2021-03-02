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


#define  JSON  &(IsimTbl->Json)   /* Convenience macro */

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
boolean SciInstrCallback (void* UserData, int TokenIdx);
boolean SciFileCallback  (void* UserData, int TokenIdx);


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

   JSON_Constructor(JSON, IsimTbl->JsonFileBuf, IsimTbl->JsonFileTokens);

   JSON_ObjConstructor(&(IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_INSTR]),
                       ISIMTBL_OBJ_SCI_INSTR_NAME,
                       SciInstrCallback,
                       (void *)&(IsimTbl->Data.SciInstr));
   JSON_RegContainerCallback(JSON, &(IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_INSTR]));

   JSON_ObjConstructor(&(IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_FILE]),
                       ISIMTBL_OBJ_SCI_FILE_NAME,
                       SciFileCallback,
                       (void *)&(IsimTbl->Data.SciFile));
   JSON_RegContainerCallback(JSON, &(IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_FILE]));

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
   
   if (JSON_OpenFile(JSON, Filename)) {
  
      CFE_EVS_SendEvent(ISIM_INIT_DEBUG_EID, ISIM_INIT_EVS_TYPE, "ISIMTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
      //DEBUG JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      //DEBUG JSON_PrintTokens(&Json,50);
  
      JSON_ProcessTokens(JSON);

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
                  if (!(IsimTbl->LoadTblEntryFunc)(i, IsimTbl->JsonObj[i].UserData))
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
      sprintf(DumpRecord,"\"%s\": {\n",IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_INSTR].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"pwr-init-cycles\": %d,\n", IsimTblPtr->SciInstr.PwrInitCycles);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"pwr-reset-cycles\": %d\n},\n", IsimTblPtr->SciInstr.PwrResetCycles);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
			  
      /* Science File */      
      sprintf(DumpRecord,"\"%s\": {\n",IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_FILE].Name);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      sprintf(DumpRecord,"   \"path-base-filename\": \"%s\",\n   \"file-extension\": \"%s\",\n   \"images-per-file\": %d\n}\n",
              IsimTblPtr->SciFile.PathBaseFilename, IsimTblPtr->SciFile.FileExtension, IsimTblPtr->SciFile.ImagesPerFile);
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
** Function: SciInstrCallback
**
** Process Instrument table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
boolean SciInstrCallback (void* UserData, int TokenIdx)
{

   int     AttributeCnt = 0;
   int     PwrInitCycles;
   int     PwrResetCycles;
   
   IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_INSTR].Modified = FALSE;   
   
   CFE_EVS_SendEvent(ISIM_INIT_DEBUG_EID, ISIM_INIT_EVS_TYPE, 
                     "\nISIMTBL.InstrumentCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     IsimTbl->ObjLoadCnt, IsimTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValShortInt(JSON, TokenIdx, "pwr-init-cycles",  &PwrInitCycles))  AttributeCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "pwr-reset-cycles", &PwrResetCycles)) AttributeCnt++;
   
   if (AttributeCnt == 2) {
   
      IsimTbl->Data.SciInstr.PwrInitCycles  = PwrInitCycles;
      IsimTbl->Data.SciInstr.PwrResetCycles = PwrResetCycles;

      IsimTbl->ObjLoadCnt++;
      IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_INSTR].Modified = TRUE;
	  
      CFE_EVS_SendEvent(ISIM_INIT_DEBUG_EID, ISIM_INIT_EVS_TYPE,  "ISIMTBL.InstrumentCallback: PwrInitCycles = %d, PwrResetCycles = %d\n",
                        IsimTbl->Data.SciInstr.PwrInitCycles, IsimTbl->Data.SciInstr.PwrResetCycles);
   
   } /* End if valid AttributeCnt */
   else {
	   
      IsimTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(ISIMTBL_LOAD_SCI_INSTR_ERR_EID, CFE_EVS_ERROR, "Invalid number of attributes %d. Should be 2.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_INSTR].Modified;

} /* SciInstrCallback() */



/******************************************************************************
** Function: SciFileCallback
**
** Process Science File table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
**   3. No error checking done on string lengths.
*/
boolean SciFileCallback (void* UserData, int TokenIdx)
{

   int     AttributeCnt = 0;
   int     ImagesPerFile;
   char    PathBaseFilename[OS_MAX_PATH_LEN];
   char    FileExtension[ISIM_FILE_EXT_MAX_CHAR];
   
   IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_FILE].Modified = FALSE;   
   
   CFE_EVS_SendEvent(ISIM_INIT_DEBUG_EID, ISIM_INIT_EVS_TYPE, 
                     "\nISIMTBL.SciFileCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     IsimTbl->ObjLoadCnt, IsimTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValShortInt(JSON, TokenIdx, "images-per-file", &ImagesPerFile)) AttributeCnt++;
   if (JSON_GetValStr(JSON, TokenIdx, "path-base-filename", PathBaseFilename)) AttributeCnt++;
   if (JSON_GetValStr(JSON, TokenIdx, "file-extension", FileExtension))        AttributeCnt++;
   
   if (AttributeCnt == 3) {
	   
      IsimTbl->Data.SciFile.ImagesPerFile = ImagesPerFile;
      strncpy(IsimTbl->Data.SciFile.PathBaseFilename,PathBaseFilename,OS_MAX_PATH_LEN);
      strncpy(IsimTbl->Data.SciFile.FileExtension,FileExtension,ISIM_FILE_EXT_MAX_CHAR);

      IsimTbl->ObjLoadCnt++;
      IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_FILE].Modified = TRUE;
	  
      CFE_EVS_SendEvent(ISIM_INIT_DEBUG_EID, ISIM_INIT_EVS_TYPE,
                        "ISIMTBL.SciFileCallback: %s, %s, %d\n", 
                        IsimTbl->Data.SciFile.PathBaseFilename,
                        IsimTbl->Data.SciFile.FileExtension,
                        IsimTbl->Data.SciFile.ImagesPerFile);
   
   } /* End if valid AttributeCnt */
   else {
	   
      IsimTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(ISIMTBL_LOAD_SCI_FILE_ERR_EID, CFE_EVS_ERROR, "Invalid number of Science File attributes %d. Should be 3.",
                        AttributeCnt);
						
   } /* End if invalid AttributeCnt */
      
   return IsimTbl->JsonObj[ISIMTBL_OBJ_SCI_FILE].Modified;

} /* SciFileCallback() */
