/*
** Purpose: Implement example table.
**
** Notes:
**   None
**
** License:
**   Template written by David McComas and licensed under the GNU
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
#include "extbl.h"

#define  JSON  &(ExTbl->Json)  /* Convenience macro */

/*
** Type Definitions
*/


/*
** Global File Data
*/

static EXTBL_Class* ExTbl = NULL;

/*
** Local File Function Prototypes
*/

/******************************************************************************
** Function: EntryCallBack
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
*/
boolean EntryCallBack (void* UserData, int TokenIdx);


/******************************************************************************
** Function: EXTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void EXTBL_Constructor(EXTBL_Class* ObjPtr,
                       EXTBL_GetTblPtr    GetTblPtrFunc,
                       EXTBL_LoadTbl      LoadTblFunc, 
                       EXTBL_LoadTblEntry LoadTblEntryFunc)
{

   ExTbl = ObjPtr;

   CFE_PSP_MemSet(ExTbl, 0, sizeof(EXTBL_Class));

   ExTbl->GetTblPtrFunc    = GetTblPtrFunc;
   ExTbl->LoadTblFunc      = LoadTblFunc;
   ExTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_Constructor(JSON, ExTbl->JsonFileBuf, ExTbl->JsonFileTokens);

   JSON_ObjConstructor(&(ExTbl->JsonObj), "entry", EntryCallBack, NULL);
   JSON_RegContainerCallback(JSON, &(ExTbl->JsonObj));

} /* End EXTBL_Constructor() */


/******************************************************************************
** Function: EXTBL_ResetStatus
**
*/
void EXTBL_ResetStatus(void)
{

   int Entry;

   ExTbl->LastLoadStatus    = TBLMGR_STATUS_UNDEF;
   ExTbl->AttrErrCnt        = 0;
   ExTbl->DataArrayEntryIdx = 0;
   
   for (Entry=0; Entry < EXTBL_MAX_ENTRY_ID; Entry++) ExTbl->Modified[Entry] = FALSE;
   
 
} /* End EXTBL_ResetStatus() */


/******************************************************************************
** Function: EXTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean EXTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int Entry;
   
   OS_printf("EXTBL_LoadCmd() Entry\n");

   /*
   ** Set all data and flags to zero. If a table replace is commanded and
   ** all of the data is not defined the zeroes will be copied into the table. 
   ** Real flight code would validate all data is loaded for a replace.
   */
   
   CFE_PSP_MemSet(&(ExTbl->Tbl), 0, sizeof(EXTBL_Struct));  /* Wouldn't do in flight but helps debug prototype */
   
   EXTBL_ResetStatus();  /* Reset status helps isolate errors if they occur */
   
   if (JSON_OpenFile(JSON, Filename)) {
  
      CFE_EVS_SendEvent(EXTBL_LOAD_CMD_DBG_EID,CFE_EVS_DEBUG,"EXTBL: Successfully prepared file %s\n", Filename);
  
      ExTbl->DataArrayEntryIdx = 0;

      JSON_ProcessTokens(JSON);

      if (ExTbl->DataArrayEntryIdx > 0) {

         
         if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
            ExTbl->LastLoadStatus = ((ExTbl->LoadTblFunc)(&(ExTbl->Tbl)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

         } /* End if replace entire table */
         else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
            ExTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
   
            for (Entry=0; Entry < EXTBL_MAX_ENTRY_ID; Entry++) {

               if (ExTbl->Modified[Entry]) {
                  if (!(ExTbl->LoadTblEntryFunc)(Entry, &(ExTbl->Tbl.Entry[Entry])))
                     ExTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
               }

            } /* End entry loop */

         } /* End if update individual records */
         else {
            CFE_EVS_SendEvent(EXTBL_LOAD_CMD_TYPE_ERR_EID,CFE_EVS_ERROR,"EXTBL: Invalid table command load type %d",LoadType);
         }

      } /* End if successful parse */
      else {
         CFE_EVS_SendEvent(EXTBL_LOAD_CMD_PARSE_ERR_EID,CFE_EVS_ERROR,"EXTBL: Table Parsing failure for file %s",Filename);
      }
      
   } /* End if valid file */
   else {
      CFE_EVS_SendEvent(EXTBL_LOAD_CMD_JSON_OPEN_ERR_EID,CFE_EVS_ERROR,"EXTBL: Table open failure for file %s. JSON Status = %d JSMN Status = %d",
	                    Filename, ExTbl->Json.FileStatus, ExTbl->Json.JsmnStatus);
   }
    
   return (ExTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of ExTBL_LoadCmd() */


/******************************************************************************
** Function: EXTBL_DumpCmd
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
boolean EXTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   int32    FileHandle;
   char     DumpRecord[256];
   int      i;
   const EXTBL_Struct *ExTblPtr;

   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS) {

      ExTblPtr = (ExTbl->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"Example Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"\"description\": \"Example table for object-based application template.\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"\"data-array\": [\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
  
      /* 
      ** Can't end last record with a comma so start of for loop finishes 
      ** previous record's line with ",\n" so write first line before the
      ** loop starts.      
      */
      sprintf(DumpRecord,"{\"entry\": {\n  \"index\": %4d,\n  \"data1\": %4d,\n  \"data2\": %4d,\n  \"data3\": %4d\n}}",
              0, ExTblPtr->Entry[0].Data1, ExTblPtr->Entry[0].Data2, ExTblPtr->Entry[0].Data3);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      for (i=1; i < EXTBL_MAX_ENTRY_ID; i++) {
      
         sprintf(DumpRecord,",\n");
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

         sprintf(DumpRecord,"{\"entry\": {\n  \"index\": %4d,\n  \"data1\": %4d,\n  \"data2\": %4d,\n  \"data3\": %4d\n}}",
                 i, ExTblPtr->Entry[i].Data1, ExTblPtr->Entry[i].Data2, ExTblPtr->Entry[i].Data3);
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      } /* End Entry loop */

      sprintf(DumpRecord,"]\n}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* TODO - Add addition meta data when file dumped */
      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else
   {
   
      CFE_EVS_SendEvent(EXTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of EXTBL_DumpCmd() */


/******************************************************************************
** Function: EntryCallBack
**
** Process a JSON entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
*/
boolean EntryCallBack (void* UserData, int TokenIdx)
{

   int  Index, Data1, Data2, Data3, EntryCnt=0;

   ExTbl->DataArrayEntryIdx++;
   CFE_EVS_SendEvent(EXTBL_LOAD_CMD_DBG_EID,CFE_EVS_DEBUG,
      "EntryCallBack() for DataArrayEntryIdx %d and token index %d\n",ExTbl->DataArrayEntryIdx, TokenIdx);
      
   if (JSON_GetValShortInt(JSON, TokenIdx, "index", &Index)) EntryCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "data1", &Data1)) EntryCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "data2", &Data2)) EntryCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "data3", &Data3)) EntryCnt++;
   
   if (EntryCnt == 4)
   {
      if (Index < EXTBL_MAX_ENTRY_ID)
      {        
         ExTbl->Tbl.Entry[Index].Data1 = Data1;
         ExTbl->Tbl.Entry[Index].Data2 = Data2;
         ExTbl->Tbl.Entry[Index].Data3 = Data3;
         ExTbl->Modified[Index] = TRUE;
         CFE_EVS_SendEvent(EXTBL_LOAD_CMD_DBG_EID,CFE_EVS_DEBUG,
		    "EntryCallBack() index, data1, data2, datat3: %d, %d, %d, %d\n",Index, Data1, Data2, Data3);
      }
      else
      {
         ExTbl->AttrErrCnt++;     
         CFE_EVS_SendEvent(EXTBL_LOAD_INDEX_ERR_EID, CFE_EVS_ERROR, "Load file data-array entry %d error, invalid index %d",
                           ExTbl->DataArrayEntryIdx, Index);
      }
      
   } /* Valid Entry */
   else
   {
      ExTbl->AttrErrCnt++;
      CFE_EVS_SendEvent(EXTBL_LOAD_LINE_ERR_EID, CFE_EVS_ERROR, "Load file data-array entry %d error, invalid number of elements %d. Should be 4.",
                        ExTbl->DataArrayEntryIdx, EntryCnt);
   } /* Invalid Entry */

   return (EntryCnt == 4);

} /* EntryCallBack() */

/* end of file */
