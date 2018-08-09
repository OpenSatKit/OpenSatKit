/* 
** Purpose: Implement the schedule table management functions.
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
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
#include "schtbl.h"


/* Convenience macro */
#define  JSON_OBJ  &(SchTbl->Json)

/*
** Type Definitions
*/


/*
** Global File Data
*/

static SCHTBL_Class* SchTbl = NULL;

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
static boolean SlotCallback (int TokenIdx);
static boolean ActivityCallback (int TokenIdx);


/******************************************************************************
** Function: SCHTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void SCHTBL_Constructor(SCHTBL_Class*       ObjPtr,
                        SCHTBL_GetTblPtr    GetTblPtrFunc,
                        SCHTBL_LoadTbl      LoadTblFunc, 
                        SCHTBL_LoadTblEntry LoadTblEntryFunc)
{
   
   SchTbl = ObjPtr;

   CFE_PSP_MemSet(SchTbl, 0, sizeof(SCHTBL_Class));

   SchTbl->GetTblPtrFunc    = GetTblPtrFunc;
   SchTbl->LoadTblFunc      = LoadTblFunc;
   SchTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_Constructor(JSON_OBJ, SchTbl->JsonFileBuf, SchTbl->JsonFileTokens);
   
   JSON_ObjConstructor(&(SchTbl->JsonObj[SCHTBL_OBJ_SLOT]),
                       SCHTBL_OBJ_SLOT_NAME,
                       SlotCallback,
                       (void *)&(SchTbl->Tbl.Entry));
   
   JSON_RegContainerCallback(JSON_OBJ,
	                          SchTbl->JsonObj[SCHTBL_OBJ_SLOT].Name,
	                          SchTbl->JsonObj[SCHTBL_OBJ_SLOT].Callback);

   JSON_ObjConstructor(&(SchTbl->JsonObj[SCHTBL_OBJ_ACTIVITY]),
                       SCHTBL_OBJ_ACTIVITY_NAME,
                       ActivityCallback,
                       (void *)&(SchTbl->Tbl.Entry));
   
   JSON_RegContainerCallback(JSON_OBJ,
	                          SchTbl->JsonObj[SCHTBL_OBJ_ACTIVITY].Name,
	                          SchTbl->JsonObj[SCHTBL_OBJ_ACTIVITY].Callback);


} /* End SCHTBL_Constructor() */


/******************************************************************************
** Function: SCHTBL_ResetStatus
**
*/
void SCHTBL_ResetStatus(void)
{

   SchTbl->LastLoadStatus  = TBLMGR_STATUS_UNDEF;
   SchTbl->AttrErrCnt      = 0;
   SchTbl->ObjErrCnt       = 0;
   SchTbl->ObjLoadCnt      = 0;
   SchTbl->CurSlotIdx      = -1;
   
   JSON_ObjArrayReset (SchTbl->JsonObj, SCHTBL_OBJ_CNT);
    
} /* End SCHTBL_ResetStatus() */


/******************************************************************************
** Function: SCHTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean SCHTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int entry;
   
   CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE, "SCHTBL_LoadCmd() Entry. sizeof(SchTbl->Tbl) = %d\n",sizeof(SchTbl->Tbl));
   
   /* 
   ** Reset status, object modified flags, and data. A valid
   ** MsgTblEntryId is used to determine whether an entry was loaded.
   */
   SCHTBL_ResetStatus();  
   CFE_PSP_MemSet(&(SchTbl->Tbl), 0, sizeof(SchTbl->Tbl));
   for (entry=0; entry < SCHTBL_MAX_ENTRIES; entry++) SchTbl->Tbl.Entry[entry].MsgTblEntryId = MSGTBL_MAX_ENTRIES;
   
   if (JSON_OpenFile(JSON_OBJ, Filename)) {
  
      CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE, "SCHTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
      //DEBUG JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      //DEBUG JSON_PrintTokens(&Json,50);
    
      SchTbl->CurSlotIdx = -1;
    
      JSON_ProcessTokens(JSON_OBJ);

      /* 
      ** Only process command if no attribute errors. No need to send an event
      ** message if there are attribute errors since events are sent for each
      ** error.  Checking at least one slot/entry is a simple check but that's all
      ** that can be done because the table doesn't have any other constraints.
      ** A table with no slots/entries is considered erroneous.
      */
      if (SchTbl->AttrErrCnt == 0) {
      
         if (SchTbl->ObjLoadCnt > 0) {
         
            if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
               SchTbl->LastLoadStatus = ((SchTbl->LoadTblFunc)(&(SchTbl->Tbl)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

	         } /* End if replace entire table */
            else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
               SchTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
   
               /* 
               ** Must have a least one slot & activity updated
               */
               if (SchTbl->ObjLoadCnt > 0 ) {
                                     
                  if (SchTbl->JsonObj[SCHTBL_OBJ_SLOT].Modified && 
                      SchTbl->JsonObj[SCHTBL_OBJ_ACTIVITY].Modified) {
                     
                     /*
                     ** Loop through entire table since updates can
                     ** be noncontiguous
                     */                     
                     for (entry=0; entry < SCHTBL_MAX_ENTRIES; entry++) {
                          
                        if (SchTbl->Tbl.Entry[entry].MsgTblEntryId < MSGTBL_MAX_ENTRIES) {
                           
                           if (!(SchTbl->LoadTblEntryFunc)(entry, (SCHTBL_Entry*)&(SchTbl->Tbl.Entry[entry])))
                              SchTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
                        
                        } /* End if entry modified */
                     } /* End entry loop */
                  } /* End if modified */
                  
               } /* End if at least one object */
               else {
                  
                  CFE_EVS_SendEvent(SCHTBL_LOAD_EMPTY_ERR_EID,CFE_EVS_ERROR,"SCHTBL: Invalid table command Must have at least one slot(%d) adn activity(%d)",
                                    SchTbl->JsonObj[SCHTBL_OBJ_SLOT].Modified, SchTbl->JsonObj[SCHTBL_OBJ_ACTIVITY].Modified);
 
               } /* End if no objects in file */
               
            } /* End if update records */
			   else {
               
               CFE_EVS_SendEvent(SCHTBL_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"SCHTBL: Invalid table command load type %d",LoadType);            
            
            } /* End if invalid command option */ 
            
         } /* End if valid message index */
         else {
            
            CFE_EVS_SendEvent(SCHTBL_LOAD_EMPTY_ERR_EID,CFE_EVS_ERROR,"SCHTBL: Invalid table command. No slots/activities objects defined.");
                              
         } /* End if no objects loaded */

      } /* End if no attribute errors */
            
   } /* End if valid file */
   else {
      
      //printf("**ERROR** Processing Message Table file %s. Status = %d JSMN Status = %d\n",TEST_FILE, Json.FileStatus, Json.JsmnStatus);
      CFE_EVS_SendEvent(SCHTBL_LOAD_OPEN_ERR_EID,CFE_EVS_ERROR,"SCHTBL: Table open failure for file %s. File Status = %s JSMN Status = %s",
	                     Filename, JSON_GetFileStatusStr(SchTbl->Json.FileStatus), JSON_GetJsmnErrStr(SchTbl->Json.JsmnStatus));
   
   } /* End if file processing error */
    
   return (SchTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of SchTBL_LoadCmd() */


/******************************************************************************
** Function: SCHTBL_DumpCmd
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

boolean SCHTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   int32    FileHandle;
   uint16   EntryIdx, Slot, Activity;
   char     DumpRecord[256];
   const    SCHTBL_Tbl *SchTblPtr;
   char     SysTimeStr[256];
   
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS) {

      SchTblPtr = (SchTbl->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"Kit Scheduler (KIT_SCH) Scheduler Activity Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      
      sprintf(DumpRecord,"\"description\": \"KIT_SCH dump at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));


      /* 
      ** - Not all fields in ground table are saved in FSW so they are not
      **   populated in the dump file. However, the dump file can still
      **   be loaded.
      ** - The MsgTblEntryId field is used to indicate whether an activity
      **   has been loaded.      
      ** 
      **   "slot": {
      **      "index": 4, 
      **   },   
      **      "activity": {
      **         "name":  "cFE ES Housekeeping",
      **         "descr": "",
      **         "index": 0,
      **         "enable": true,
      **         "frequency": 4,
      **         "offset": 0,
      **         "msg-id": 0,
      **      },
      */
      
      for (Slot=0; Slot < SCHTBL_SLOTS; Slot++) {
         
         sprintf(DumpRecord,"\"slot\": {\n   \"index\": %d,\n},\n",Slot);
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
         
         for (Activity=0; Activity < SCHTBL_ACTIVITIES_PER_SLOT; Activity++) {
            
            EntryIdx = Slot * SCHTBL_ACTIVITIES_PER_SLOT + Activity;

            sprintf(DumpRecord,"\"activity\": {\n");
            OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
            sprintf(DumpRecord,"   \"index\": %d,\n   \"enable\": \"%s\",\n   \"frequency\": %d,\n    \"offset\": %d,\n    \"msg-id\": %d\n},\n",
                 Activity,
                 JSON_GetBoolStr(SchTblPtr->Entry[EntryIdx].Enabled),
                 SchTblPtr->Entry[EntryIdx].Frequency,
                 SchTblPtr->Entry[EntryIdx].Offset,
                 SchTblPtr->Entry[EntryIdx].MsgTblEntryId); 
            OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
         
         } /* End activity loop */             
      
      } /* End slot loop */
 
 
      sprintf(DumpRecord,"}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(SCHTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of SCHTBL_DumpCmd() */


/******************************************************************************
** Function: SCHTBL_GetEntryPtr
**
*/
boolean SCHTBL_GetEntryPtr(uint16  EntryId, SCHTBL_Entry **EntryPtr)
{

   boolean RetStatus = FALSE;

   if (EntryId < SCHTBL_MAX_ENTRIES) {

      *EntryPtr = &SchTbl->Tbl.Entry[EntryId];
      RetStatus = TRUE;
 
   } /* End if valid EntryId */

   return RetStatus;

} /* End SCHTBL_GetEntryPtr() */


/******************************************************************************
** Function: SlotCallback
**
** Process a slot table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. ObjLoadCnt incremented for every message, valid or invalid.
**   3. Assumes JSON file processed sequentially because this callback sets 
**      the slot ID and the activity callback assumes it it processing the
**      current slot. 
*/
static boolean SlotCallback (int TokenIdx)
{

   int     AttributeCnt = 0;
   int     JsonIntData, SlotIdx;
   boolean RetStatus = FALSE;      

   CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                     "\nSCHTBL.SlotCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     SchTbl->ObjLoadCnt, SchTbl->AttrErrCnt, TokenIdx);
             
   CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                     "\nSCHTBL.SlotCallback: ContainerSize %d\n", 
                     JSON_GetContainerSize(JSON_OBJ, TokenIdx));

   /* 
   ** Slot
   **
   **   "index":  
   **   []  : Array of activities.  
   */
   
   if (JSON_GetValShortInt(JSON_OBJ, TokenIdx, "index", &JsonIntData)) { 
      AttributeCnt++; 
      SlotIdx = JsonIntData; 
   }
      
   SchTbl->ObjLoadCnt++;
   
   /* TODO - Add sanity check to verify array object exists within the slot container */
   if (AttributeCnt == 1) {
   
      if (SlotIdx >= 0 && SlotIdx < SCHTBL_SLOTS) {
         
         SchTbl->CurSlotIdx = SlotIdx;
         RetStatus = TRUE;
      
      } /* End if Id within limits */
      else {
         SchTbl->ObjErrCnt++;
      }
     
      CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE, "SCHTBL.SlotCallback Received slot %d\n", SlotIdx);
   
   } /* End if valid AttributeCnt */
   else {
	   
      SchTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(SCHTBL_LOAD_ATTR_ERR_EID, CFE_EVS_ERROR, "Invalid number of slot attributes %d. Should be 1.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return RetStatus;

} /* SlotCallback() */


/******************************************************************************
** Function: ActivityCallback
**
** Process a message table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. ObjLoadCnt incremented for every message, valid or invalid.
*/
static boolean ActivityCallback (int TokenIdx)
{

   int    AttributeCnt = 0;
   int    JsonIntData, ActivityIdx, EntryIdx;
   char   DataStr[128];  /* Need room for 'true' or 'false'. Spare in case erroneous entry */
   boolean RetStatus = FALSE;      
   SCHTBL_Entry SchEntry;

   CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                     "\nSCHTBL.ActivityCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d, CurSlotIdx %d\n",
                     SchTbl->ObjLoadCnt, SchTbl->AttrErrCnt, TokenIdx, SchTbl->CurSlotIdx);
   CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                     "\nSCHTBL.ActivityCallback: ContainerSize %d\n",
                     JSON_GetContainerSize(JSON_OBJ, TokenIdx));
   
   memset((void*)&SchEntry,0,sizeof(SCHTBL_Entry));
   
   /* 
   ** Activity entry 
   **
   ** "activity": {
   **    "name":  "Time Housekeeping",
   **    "descr": "",
   **    "index": 0,
   **    "enable": true,
   **    "frequency": 4,
   **    "offset": 0,
   **    "msg-id": 4,
   ** }
   */
   if (JSON_GetValStr(JSON_OBJ, TokenIdx, "name", DataStr)) { CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,"SCHTBL.ActivityCallback: %s\n",DataStr); }   
   if (JSON_GetValShortInt(JSON_OBJ, TokenIdx, "index",      &JsonIntData)) { AttributeCnt++; ActivityIdx            = JsonIntData; }
   if (JSON_GetValShortInt(JSON_OBJ, TokenIdx, "frequency",  &JsonIntData)) { AttributeCnt++; SchEntry.Frequency     = (uint16) JsonIntData; }
   if (JSON_GetValShortInt(JSON_OBJ, TokenIdx, "offset",     &JsonIntData)) { AttributeCnt++; SchEntry.Offset        = (uint16) JsonIntData; }
   if (JSON_GetValShortInt(JSON_OBJ, TokenIdx, "msg-id",     &JsonIntData)) { AttributeCnt++; SchEntry.MsgTblEntryId = (uint16) JsonIntData; }
   if (JSON_GetValStr(JSON_OBJ,      TokenIdx, "enable", DataStr)) {
      
      AttributeCnt++; 
      
      /*
      ** n != to exact length because that would only verify prefix is equal 
      */
      if (strncmp(DataStr,"true",6) == 0)
         SchEntry.Enabled = TRUE;
      else if (strncmp(DataStr,"false",6) == 0)
         SchEntry.Enabled = FALSE;
      
   } /* End if DataStr */
   
   SchTbl->ObjLoadCnt++;
   
   if (AttributeCnt == 5) {
   
      if ( ActivityIdx >= 0 && ActivityIdx < SCHTBL_ACTIVITIES_PER_SLOT) {
         
         if ( SchEntry.MsgTblEntryId >= 0 && SchEntry.MsgTblEntryId < MSGTBL_MAX_ENTRIES) {
            
            /* CurSlotIdx should never get set beyond max, but check ensure array access is valid */
            if (SchTbl->CurSlotIdx >= 0 && SchTbl->CurSlotIdx < SCHTBL_SLOTS) {
         
               EntryIdx = SchTbl->CurSlotIdx * SCHTBL_ACTIVITIES_PER_SLOT + ActivityIdx;
               SchTbl->Tbl.Entry[EntryIdx] = SchEntry;
               RetStatus = TRUE;
               
            } /* End if CurSlotId within limits */

            CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                              "SCHTBL.ActivityCallback (activty idx, freq, offset, msg idx, enabled): %d, %d, %d, %d, %d\n",
                              ActivityIdx, SchEntry.Frequency, SchEntry.Offset, SchEntry.MsgTblEntryId, SchEntry.Enabled);
      
         } /* End if valid message entry id */
         else {
            
            CFE_EVS_SendEvent(SCHTBL_LOAD_PARSE_ERR_EID, CFE_EVS_ERROR, "Invalid activity msg id %d. Must be between 0 and %d.",
                              SchEntry.MsgTblEntryId, MSGTBL_MAX_ENTRIES);
         
         } /* End if valid message entry id  */
      } /* End if valid activity index */
      else {
         CFE_EVS_SendEvent(SCHTBL_LOAD_ATTR_ERR_EID, CFE_EVS_ERROR, "Invalid activity index %d. Must be between 0 and %d.",
                           ActivityIdx, SCHTBL_ACTIVITIES_PER_SLOT);
      } /* End if invalid activity index  */
   
   } /* End if valid AttributeCnt */
   else {
	   
      SchTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(SCHTBL_LOAD_PARSE_ERR_EID, CFE_EVS_ERROR, "Invalid number of activity attributes %d. Should be 5.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */

   /* Incremented if CurSlotIdx invalid which technically is not this object's error, but something wrong with JSON file */
   if (!RetStatus) SchTbl->ObjErrCnt++;
   
   return RetStatus;

} /* ActivityCallback() */

/* end of file */
