/*
** Purpose: Implement example JSON table.
**
** Notes:
**   1. The load and dump functions use call back functions to investigate
**      whether the design could be modularized to a point when a user
**      only supplies table unique processing in a callback function. The
**      application framework TBLMGR is at that point and this is looking
**      whether refining it further makes sense.
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
#include "jsontbl.h"

#define  JSON  &(JsonTbl->Json)  /* Convenience macro */

/*
** Type Definitions
*/


/*
** Global File Data
*/

static JSONTBL_Class* JsonTbl = NULL;

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
** Function: JSONTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void JSONTBL_Constructor(JSONTBL_Class* ObjPtr,
                         JSONTBL_GetTblPtr    GetTblPtrFunc,
                         JSONTBL_LoadTbl      LoadTblFunc, 
                         JSONTBL_LoadTblEntry LoadTblEntryFunc)
{

   JsonTbl = ObjPtr;

   CFE_PSP_MemSet(JsonTbl, 0, sizeof(JSONTBL_Class));

   JsonTbl->GetTblPtrFunc    = GetTblPtrFunc;
   JsonTbl->LoadTblFunc      = LoadTblFunc;
   JsonTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_Constructor(JSON, JsonTbl->JsonFileBuf, JsonTbl->JsonFileTokens);

   JSON_ObjConstructor(&(JsonTbl->JsonObj), "entry", EntryCallBack, NULL);
   JSON_RegContainerCallback(JSON, &(JsonTbl->JsonObj));

} /* End JSONTBL_Constructor() */


/******************************************************************************
** Function: JSONTBL_ResetStatus
**
*/
void JSONTBL_ResetStatus(void)
{

   int Entry;

   JsonTbl->LastLoadStatus    = TBLMGR_STATUS_UNDEF;
   JsonTbl->AttrErrCnt        = 0;
   JsonTbl->DataArrayEntryIdx = 0;
   
   for (Entry=0; Entry < JSONTBL_MAX_ENTRY_ID; Entry++) JsonTbl->Modified[Entry] = FALSE;
   
 
} /* End JSONTBL_ResetStatus() */


/******************************************************************************
** Function: JSONTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean JSONTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int Entry;
   
   CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, "JSONTBL_LoadCmd() Entry\n");

   /*
   ** Set all data and flags to zero. If a table replace is commanded and
   ** all of the data is not defined then zeroes will be copied into the table. 
   ** Flight code would validate all data.
   */
   
   CFE_PSP_MemSet(&(JsonTbl->Tbl), 0, sizeof(ExTblData_Param));  /* Wouldn't do in flight but helps debug prototype */
   
   JSONTBL_ResetStatus();  /* Reset status helps isolate errors if they occur */
   
   if (JSON_OpenFile(JSON, Filename)) {
  
      CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, "JSONTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
      //DEBUG JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      //DEBUG JSON_PrintTokens(&Json,50);
  
      JsonTbl->DataArrayEntryIdx = 0;

      JSON_ProcessTokens(JSON);

      if (JsonTbl->DataArrayEntryIdx > 0) {

         
         if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
            JsonTbl->LastLoadStatus = ((JsonTbl->LoadTblFunc)(&(JsonTbl->Tbl)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

         } /* End if replace entire table */
         else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
            JsonTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
   
            for (Entry=0; Entry < JSONTBL_MAX_ENTRY_ID; Entry++) {

               if (JsonTbl->Modified[Entry]) {
                  if (!(JsonTbl->LoadTblEntryFunc)(Entry, &(JsonTbl->Tbl.Entry[Entry])))
                     JsonTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
               }

            } /* End entry loop */

         } /* End if update individual records */
         else {
            CFE_EVS_SendEvent(JSONTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"JSONTBL: Invalid table command load type %d",LoadType);
         }

      } /* End if successfully parsed entries */
      else {
         CFE_EVS_SendEvent(JSONTBL_CMD_LOAD_PARSE_ERR_EID,CFE_EVS_ERROR,"JSONTBL: Table Parsing failure for file %s",Filename);
      }
      
   } /* End if valid file */
   else {
      
      CFE_EVS_SendEvent(JSONTBL_CMD_LOAD_JSON_OPEN_ERR_EID,CFE_EVS_ERROR,"JSONTBL: Table open failure for file %s. JSON Status = %d JSMN Status = %d",
	                    Filename, JsonTbl->Json.FileStatus, JsonTbl->Json.JsmnStatus);
   }
    
   return (JsonTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of JSONTBL_LoadCmd() */


/******************************************************************************
** Function: JSONTBL_DumpCmd
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
boolean JSONTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename) {

   boolean  RetStatus = FALSE;
   int32    FileHandle;
   char     DumpRecord[256];
   int      i;
   const ExTblData_Param *JsonTblPtr;

   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS) {

      JsonTblPtr = (JsonTbl->GetTblPtrFunc)();

      sprintf(DumpRecord,"{\n\"name\": \"Object Table\",\n");
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
              0, JsonTblPtr->Entry[0].Data1, JsonTblPtr->Entry[0].Data2, JsonTblPtr->Entry[0].Data3);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      for (i=1; i < JSONTBL_MAX_ENTRY_ID; i++) {
      
         sprintf(DumpRecord,",\n");
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

         sprintf(DumpRecord,"{\"entry\": {\n  \"index\": %4d,\n  \"data1\": %4d,\n  \"data2\": %4d,\n  \"data3\": %4d\n}}",
                 i, JsonTblPtr->Entry[i].Data1, JsonTblPtr->Entry[i].Data2, JsonTblPtr->Entry[i].Data3);
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      } /* End Entry loop */

      sprintf(DumpRecord,"]\n}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* TODO - Add addition meta data when file dumped */
      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(JSONTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of JSONTBL_DumpCmd() */


/******************************************************************************
** Function: EntryCallBack
**
** Process a JSON entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
*/
boolean EntryCallBack (void* UserData, int TokenIdx)
{

   int  Index, Data1, Data2, Data3, EntryCnt=0;

   JsonTbl->DataArrayEntryIdx++;
   
   CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, "\nEntryCallBack() for DataArrayEntryIdx %d and token index %d\n",JsonTbl->DataArrayEntryIdx, TokenIdx);
      
   if (JSON_GetValShortInt(JSON, TokenIdx, "index", &Index)) EntryCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "data1", &Data1)) EntryCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "data2", &Data2)) EntryCnt++;
   if (JSON_GetValShortInt(JSON, TokenIdx, "data3", &Data3)) EntryCnt++;
   
   if (EntryCnt == 4) {
      
      if (Index < JSONTBL_MAX_ENTRY_ID) {
         
         JsonTbl->Tbl.Entry[Index].Data1 = Data1;
         JsonTbl->Tbl.Entry[Index].Data2 = Data2;
         JsonTbl->Tbl.Entry[Index].Data3 = Data3;
         JsonTbl->Modified[Index] = TRUE;
         
         CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, "JSONTBL-ParseLine: %d, %d, %d, %d\n",Index, Data1, Data2, Data3);
      
      }
      else {
         
         JsonTbl->AttrErrCnt++;     
         CFE_EVS_SendEvent(JSONTBL_LOAD_INDEX_ERR_EID, CFE_EVS_ERROR, "Load file data-array entry %d error, invalid index %d",
                           JsonTbl->DataArrayEntryIdx, Index);
      }
      
   } /* Valid Entry */
   else {
      
      JsonTbl->AttrErrCnt++;
      CFE_EVS_SendEvent(JSONTBL_LOAD_LINE_ERR_EID, CFE_EVS_ERROR, "Load file data-array entry %d error, invalid number of elements %d. Should be 4.",
                        JsonTbl->DataArrayEntryIdx, EntryCnt);
   } /* Invalid Entry */

   return (EntryCnt == 4);

} /* EntryCallBack() */

