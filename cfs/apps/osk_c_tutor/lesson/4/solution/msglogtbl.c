/* LESSON 4 - Solution
** Purpose: Implement the Message Log table.
**
** Notes:
**   1. The static "TblData" serves as a table load buffer. Table dump data is
**      read directly from table owner's table storage.
**
** License:
**   Written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Application Developer's Guide
**   2. cFS Application Developer's Guide
**
*/

/*
** Include Files:
*/

#include <string.h>
#include "msglogtbl.h"


/***********************/
/** Macro Definitions **/
/***********************/


/**********************/
/** Type Definitions **/
/**********************/


/**********************/
/** Global File Data **/
/**********************/


static MSGLOGTBL_Class* MsgLogTbl = NULL;

static MSGLOGTBL_Data TblData;

static CJSON_Obj JsonTblObjs[] = {

   { TblData.File.PathBaseName, OS_MAX_PATH_LEN,            FALSE, JSONString, { "file.path-base-name", strlen("file.path-base-name")} },
   { TblData.File.Extension,    MSGLOGTBL_FILE_EXT_MAX_LEN, FALSE, JSONString, { "file.extension",      strlen("file.extension")}      },
   { &TblData.File.EntryCnt,    3,                          FALSE, JSONNumber, { "file.entry-cnt",      strlen("file.entry-cnt")}      },
   { &TblData.PlaybkDelay,      2,                          FALSE, JSONNumber, { "playbk-delay",        strlen("playbk-delay")}        }
   
};


/************************************/
/** Local File Function Prototypes **/
/************************************/

static boolean LoadJsonData(size_t JsonFileLen);


/******************************************************************************
** Function: MSGLOGTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void MSGLOGTBL_Constructor(MSGLOGTBL_Class* MsgLogTblPtr, INITBL_Class* IniTbl)
{

   MsgLogTbl = MsgLogTblPtr;

   CFE_PSP_MemSet(MsgLogTbl, 0, sizeof(MSGLOGTBL_Class));

   MsgLogTbl->AppName = INITBL_GetStrConfig(IniTbl, CFG_APP_CFE_NAME);
   MsgLogTbl->JsonObjCnt = (sizeof(JsonTblObjs)/sizeof(CJSON_Obj));
   
} /* End MSGLOGTBL_Constructor() */


/******************************************************************************
** Function: MSGLOGTBL_ResetStatus
**
*/
void MSGLOGTBL_ResetStatus(void)
{

   MsgLogTbl->LastLoadStatus = TBLMGR_STATUS_UNDEF;
   MsgLogTbl->LastLoadCnt = 0;
 
} /* End MSGLOGTBL_ResetStatus() */


/******************************************************************************
** Function: MSGLOGTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. This could migrate into table manager but I think I'll keep it here so
**     user's can add table processing code if needed.
*/
boolean MSGLOGTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   boolean  RetStatus = FALSE;

   if (CJSON_ProcessFile(Filename, MsgLogTbl->JsonBuf, MSGLOGTBL_JSON_FILE_MAX_CHAR, LoadJsonData)) {
      
      MsgLogTbl->Loaded = TRUE;
      MsgLogTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
      RetStatus = TRUE;
   
   }
   else {

      MsgLogTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;

   }

   return RetStatus;
   
} /* End MSGLOGTBL_LoadCmd() */


/******************************************************************************
** Function: MSGLOGTBL_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table filename because this is a callback from 
**     the app framework table manager that has verified the file.
**  3. DumpType is unused.
**  4. File is formatted so it can be used as a load file. It does not follow
**     the cFE table file format. 
**  5. Creates a new dump file, overwriting anything that may have existed
**     previously
*/
boolean MSGLOGTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   int32    FileHandle;
   char     DumpRecord[256];
   char     SysTimeStr[256];

   
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS) {

      sprintf(DumpRecord,"{\n   \"app-name\": \"%s\",\n   \"tbl-name\": \"Message Log\",\n",MsgLogTbl->AppName);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      sprintf(DumpRecord,"   \"description\": \"Table dumped at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"   \"file\": {\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      sprintf(DumpRecord,"     \"path-base-name\": \"%s\",\n", MsgLogTbl->Data.File.PathBaseName);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"     \"extension\": \"%s\",\n", MsgLogTbl->Data.File.Extension);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"     \"entry-cnt\": %d\n   },\n", MsgLogTbl->Data.File.EntryCnt);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"   \"playbk-delay\": %d\n}\n", MsgLogTbl->Data.PlaybkDelay);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      OS_close(FileHandle);

      RetStatus = TRUE;

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(MSGLOGTBL_DUMP_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of MSGLOGTBL_DumpCmd() */


/******************************************************************************
** Function: LoadJsonData
**
** Notes:
**  1. See file prologue for full/partial table load scenarios
*/
static boolean LoadJsonData(size_t JsonFileLen)
{

   boolean      RetStatus = FALSE;
   size_t       ObjLoadCnt;


   MsgLogTbl->JsonFileLen = JsonFileLen;

   /* 
   ** 1. Copy table owner data into local table buffer
   ** 2. Process JSON file which updates local table buffer with JSON supplied values
   ** 3. If valid, copy local buffer over owner's data 
   */
   
   memcpy(&TblData, &MsgLogTbl->Data, sizeof(MSGLOGTBL_Data));
   
   ObjLoadCnt = CJSON_LoadObjArray(JsonTblObjs, MsgLogTbl->JsonObjCnt, MsgLogTbl->JsonBuf, MsgLogTbl->JsonFileLen);

   if (!MsgLogTbl->Loaded && (ObjLoadCnt != MsgLogTbl->JsonObjCnt)) {
   
      CFE_EVS_SendEvent(MSGLOGTBL_LOAD_ERR_EID, CFE_EVS_ERROR, 
                        "Table has never been loaded and new table only contains %d of %d data objects",
                        ObjLoadCnt, MsgLogTbl->JsonObjCnt);
   
   }
   else {
      
      memcpy(&MsgLogTbl->Data,&TblData, sizeof(MSGLOGTBL_Data));
      MsgLogTbl->LastLoadCnt = ObjLoadCnt;
      RetStatus = TRUE;
      
   }
   
   return RetStatus;
   
} /* End LoadJsonData() */

