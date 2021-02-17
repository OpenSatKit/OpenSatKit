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


#define  JSON  &(SchTbl->Json)  /* Convenience macro */

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
static boolean SlotCallback (void* UserData, int TokenIdx);
static boolean ActivityCallback (void* UserData, int TokenIdx);


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

   JSON_Constructor(JSON, SchTbl->JsonFileBuf, SchTbl->JsonFileTokens);
   
   JSON_ObjConstructor(&(SchTbl->JsonObj[SCHTBL_OBJ_SLOT]),
                       SCHTBL_OBJ_NAME_SLOT,
                       SlotCallback,
                       (void *)&(SchTbl->Tbl.Entry));
   
   JSON_RegContainerCallback(JSON, &(SchTbl->JsonObj[SCHTBL_OBJ_SLOT]));

   JSON_ObjConstructor(&(SchTbl->JsonObj[SCHTBL_OBJ_ACTIVITY]),
                       SCHTBL_OBJ_NAME_ACTIVITY,
                       ActivityCallback,
                       (void *)&(SchTbl->Tbl.Entry));
   
   JSON_RegContainerCallback(JSON, &(SchTbl->JsonObj[SCHTBL_OBJ_ACTIVITY]));


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
   SchTbl->CurSlotIdx      = SCHTBL_UNDEF_SLOT;
   
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
   ** MsgTblIndex is used to determine whether an entry was loaded.
   */
   SCHTBL_ResetStatus();  
   CFE_PSP_MemSet(&(SchTbl->Tbl), 0, sizeof(SchTbl->Tbl));
   for (entry=0; entry < SCHTBL_MAX_ENTRIES; entry++) SchTbl->Tbl.Entry[entry].MsgTblIndex = MSGTBL_MAX_ENTRIES;
   
   if (JSON_OpenFile(JSON, Filename)) {
  
      CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE, "SCHTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
      //DEBUG JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      //DEBUG JSON_PrintTokens(&Json,50);
    
      SchTbl->CurSlotIdx = SCHTBL_UNDEF_SLOT;
    
      JSON_ProcessTokens(JSON);

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
                          
                        if (SchTbl->Tbl.Entry[entry].MsgTblIndex < MSGTBL_MAX_ENTRIES) {
                           
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
**     the app framework table manager that has verified the file. If the
**     filename exists it will be overwritten.
**  3. File is formatted so it can be used as a load file. However all of the
**     entries are dumped so you will get errors on the load for unused entries
**     because unused entries have invalid  message indices.
**  4. DumpType is unused.
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
      
      sprintf(DumpRecord,"\"description\": \"KIT_SCH table dumped at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));


      /* 
      ** - Not all fields in ground table are saved in FSW so they are not
      **   populated in the dump file. However, the dump file can still
      **   be loaded.
      ** - The MsgTblIndex field is used to indicate whether an activity
      **   has been loaded.      
      ** 
      **   "slot-array": [
      **
      **      {"slot": {
      **         "index": 4
      **         "activity-array" : [
      **   
      **            {"activity": {
      **            "name":   "cFE ES Housekeeping",
      **            "descr":  "",
      **            "index":   0,
      **            "enabled": "true",
      **            "period":  4,
      **            "offset":  0,
      **            "msg-idx": 0
      **         }},
      **         ...
      **      ...
      */
      
      sprintf(DumpRecord,"\"slot-array\": [\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      for (Slot=0; Slot < SCHTBL_SLOTS; Slot++) {
         
         if (Slot > 0) {
            sprintf(DumpRecord,",\n");            
            OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
         }
            
         sprintf(DumpRecord,"   {\"slot\": {\n      \"index\": %d,\n      \"activity-array\" : [\n",Slot);         
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
         
         for (Activity=0; Activity < SCHTBL_ACTIVITIES_PER_SLOT; Activity++) {
            
            EntryIdx = SCHTBL_INDEX(Slot,Activity);

            if (Activity > 0) {
               sprintf(DumpRecord,",\n");
               OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
            }
            
            sprintf(DumpRecord,"         {\"activity\": {\n");
            OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
            
            sprintf(DumpRecord,"         \"index\": %d,\n         \"enabled\": \"%s\",\n         \"period\": %d,\n         \"offset\": %d,\n         \"msg-idx\": %d\n      }}",
                 Activity,
                 JSON_GetBoolStr(SchTblPtr->Entry[EntryIdx].Enabled),
                 SchTblPtr->Entry[EntryIdx].Period,
                 SchTblPtr->Entry[EntryIdx].Offset,
                 SchTblPtr->Entry[EntryIdx].MsgTblIndex); 
            OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
         
         } /* End activity loop */             
      
         sprintf(DumpRecord,"\n      ]\n   }}");
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      } /* End slot loop */
 
      /* Close slot-array and top-level object */
      sprintf(DumpRecord,"\n   ]\n}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(SCHTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   if (RetStatus) {
      
      CFE_EVS_SendEvent(SCHTBL_DUMP_INFO_EID, CFE_EVS_INFORMATION,
                        "Successfully dumped scheduler table to %s", Filename);
   }
   
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
** SCHTBL_GetEntryIndex
**
** Compute and load EntryIndex if the SlotIndex and ActivityIndex are valid.
** Event message text assumes commands are being validated 
*/
boolean SCHTBL_GetEntryIndex(const char* EventStr, uint16 SlotIndex, 
                             uint16 ActivityIndex, uint16* EntryIndex)
{
   
   boolean RetStatus = FALSE;
   
   if (SlotIndex < SCHTBL_SLOTS) {

      if (ActivityIndex < SCHTBL_ACTIVITIES_PER_SLOT) {
         
         *EntryIndex = SCHTBL_INDEX(SlotIndex, ActivityIndex);
         RetStatus = TRUE;
         
      }
      else {
         
         CFE_EVS_SendEvent (SCHTBL_CMD_ACTIVITY_ERR_EID, CFE_EVS_ERROR, 
                            "%s. Invalid activity index %d greater than max %d",
                            EventStr, ActivityIndex, (SCHTBL_ACTIVITIES_PER_SLOT-1));
      }

   } /* End if valid activity ID */
   else {
      
      CFE_EVS_SendEvent (SCHTBL_CMD_SLOT_ERR_EID, CFE_EVS_ERROR, 
                         "%s. Invalid slot index %d greater than max %d",
                         EventStr, SlotIndex, (SCHTBL_SLOTS-1));

   } /* End if invalid slot ID */

   return RetStatus;
   
} /* End SCHTBL_GetEntryIndex() */


/******************************************************************************
** Function: SCHTBL_ValidEntry
**
** A pointer to a structure isn't passed because this function is used to
** validate command and table parametetrs that may not be packed identically
** to the internal structure.
*/
boolean SCHTBL_ValidEntry(const char* EventStr, uint16 Enabled, uint16 Period, 
                          uint16 Offset, uint16 MsgTblIndex)
{

   boolean RetStatus = FALSE;

   if (CMDMGR_ValidBoolArg(Enabled)) {
      
      /* 
      ** In theory Period and Offset can have any 16-bit value so an absolute
      ** limit isn't checked. However if an Offset is greater than the Period 
      ** then it will never execute. The Enabled flag should be used if a user
      ** wants to explicitly disable a slot.
      */
      if (Offset <= Period) {
         
         if ( MsgTblIndex >= 0 && MsgTblIndex < MSGTBL_MAX_ENTRIES) {
         
           
           RetStatus = TRUE;
            
         }
         else {
         
            CFE_EVS_SendEvent(SCHTBL_MSG_TBL_INDEX_ERR_EID, CFE_EVS_ERROR, 
                              "%s. Invalid msg index %d. Valid index: 0 <= Index < %d.",
                              EventStr, MsgTblIndex, MSGTBL_MAX_ENTRIES);
      
         }
      } /* End if valid offset */
      else {
         
         CFE_EVS_SendEvent(SCHTBL_OFFSET_ERR_EID, CFE_EVS_ERROR,
                           "%s. Offset %d is greater than Period %d",
                           EventStr, Offset, Period);    
      
      } /* End if invalid offset */            
   } /* End if valid boolean config */
   else {
   
      CFE_EVS_SendEvent(SCHTBL_ENABLED_ERR_EID, CFE_EVS_ERROR,
                        "%s. Invalid Enabled value %d. Must be True(%d) or False(%d)",
                        EventStr, Enabled, TRUE, FALSE);    
         
   } /* End if invalid boolean config */

   return RetStatus;

} /* End SCHTBL_ValidEntry() */


/******************************************************************************
** Function: SlotCallback
**
** Process a slot table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
**   3. ObjLoadCnt incremented for every message, valid or invalid.
**   4. Assumes JSON file processed sequentially because this callback sets 
**      the slot ID and the activity callback assumes it is processing the
**      current slot. 
*/
static boolean SlotCallback (void* UserData, int TokenIdx)
{

   int     AttributeCnt = 0;
   int     JsonIntData, SlotIdx;
 
   SchTbl->JsonObj[SCHTBL_OBJ_SLOT].Modified = FALSE;
   
   CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                     "\nSCHTBL.SlotCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     SchTbl->ObjLoadCnt, SchTbl->AttrErrCnt, TokenIdx);
             
   CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                     "\nSCHTBL.SlotCallback: ContainerSize %d\n", 
                     JSON_GetContainerSize(JSON, TokenIdx));

   /* 
   ** Slot
   **
   **   "index":  
   **   []  : Array of activities.  
   */
   
   if (JSON_GetValShortInt(JSON, TokenIdx, "index", &JsonIntData)) { 
      AttributeCnt++; 
      SlotIdx = JsonIntData; 
   }
      
   SchTbl->ObjLoadCnt++;
   
   if (AttributeCnt == 1) {
   
      if (SlotIdx >= 0 && SlotIdx < SCHTBL_SLOTS) {
         
         SchTbl->CurSlotIdx = SlotIdx;
         SchTbl->JsonObj[SCHTBL_OBJ_SLOT].Modified = TRUE;
      
      } /* End if Id within limits */
      else {
         
         CFE_EVS_SendEvent (SCHTBL_LOAD_ATTR_ERR_EID, CFE_EVS_ERROR, 
                            "Scheduler Table error. Invalid slot index %d greater than max %d",
                            SlotIdx, (SCHTBL_SLOTS-1));
         SchTbl->CurSlotIdx = SCHTBL_UNDEF_SLOT;
         SchTbl->ObjErrCnt++;
      }
     
      CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE, "SCHTBL.SlotCallback Received slot %d\n", SlotIdx);
   
   } /* End if valid AttributeCnt */
   else {
	   
      SchTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(SCHTBL_LOAD_ATTR_ERR_EID, CFE_EVS_ERROR, "Scheduler Table error. Invalid number of slot attributes %d. Should be 1.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return SchTbl->JsonObj[SCHTBL_OBJ_SLOT].Modified;

} /* SlotCallback() */


/******************************************************************************
** Function: ActivityCallback
**
** Process a message table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
**   3. ObjLoadCnt incremented for every message, valid or invalid.
**   4. Validation functions send events messages
*/
static boolean ActivityCallback (void* UserData, int TokenIdx)
{

   int    AttributeCnt = 0;
   int    JsonIntData, ActivityIdx;
   char   DataStr[128];  /* Need room for 'true' or 'false'. Spare in case erroneous entry */
   char   EventStr[128];
   SCHTBL_Entry SchEntry;

   SchTbl->JsonObj[SCHTBL_OBJ_ACTIVITY].Modified = FALSE;
   
   CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                     "\nSCHTBL.ActivityCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d, CurSlotIdx %d\n",
                     SchTbl->ObjLoadCnt, SchTbl->AttrErrCnt, TokenIdx, SchTbl->CurSlotIdx);
   CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                     "\nSCHTBL.ActivityCallback: ContainerSize %d\n",
                     JSON_GetContainerSize(JSON, TokenIdx));
   
   /*
   ** If current slot is invalid an event message should have been sent and the
   ** activity array can be ignored.   
   */
   if (SchTbl->CurSlotIdx != SCHTBL_UNDEF_SLOT) {
      
      memset((void*)&SchEntry,0,sizeof(SCHTBL_Entry));
      
      /* 
      ** Activity entry 
      **
      ** "activity": {
      **    "name":    "Time Housekeeping",
      **    "descr":   "",
      **    "index":   0,
      **    "enabled": "true",
      **    "period":  4,
      **    "offset":  0,
      **    "msg-idx": 4,
      ** }
      */
      if (JSON_GetValStr(JSON, TokenIdx, "name", DataStr)) { CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,"SCHTBL.ActivityCallback: %s\n",DataStr); }   
      if (JSON_GetValShortInt(JSON, TokenIdx, "index",   &JsonIntData)) { AttributeCnt++; ActivityIdx          = JsonIntData; }
      if (JSON_GetValShortInt(JSON, TokenIdx, "period",  &JsonIntData)) { AttributeCnt++; SchEntry.Period      = (uint16) JsonIntData; }
      if (JSON_GetValShortInt(JSON, TokenIdx, "offset",  &JsonIntData)) { AttributeCnt++; SchEntry.Offset      = (uint16) JsonIntData; }
      if (JSON_GetValShortInt(JSON, TokenIdx, "msg-idx", &JsonIntData)) { AttributeCnt++; SchEntry.MsgTblIndex = (uint16) JsonIntData; }
      if (JSON_GetValStr(JSON,      TokenIdx, "enabled", DataStr)) {
         
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
      
         uint16 Index;
         
         if (SCHTBL_GetEntryIndex("Scheduler Table JSON error",
             SchTbl->CurSlotIdx, ActivityIdx, &Index)) {

            /* Prepare an error string in case it's needed */
            sprintf(EventStr,"JSON parse error for scheduler table slot %d, activity %d",
                    SchTbl->CurSlotIdx, ActivityIdx);
            
            if (SCHTBL_ValidEntry(EventStr, SchEntry.Enabled, SchEntry.Period,
                             SchEntry.Offset,SchEntry.MsgTblIndex)) {
            
               SchTbl->Tbl.Entry[Index] = SchEntry;
               SchTbl->JsonObj[SCHTBL_OBJ_ACTIVITY].Modified = TRUE;
            
               CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                                 "SCHTBL.ActivityCallback (activty idx, period, offset, msg idx, enabled): %d, %d, %d, %d, %d\n",
                                 ActivityIdx, SchEntry.Period, SchEntry.Offset, SchEntry.MsgTblIndex, SchEntry.Enabled);
            }
         }
            
      } /* End if valid AttributeCnt */
      else {
         
         SchTbl->AttrErrCnt++;     
         CFE_EVS_SendEvent(SCHTBL_LOAD_PARSE_ERR_EID, CFE_EVS_ERROR, "Invalid number of activity attributes %d. Should be 5.",
                           AttributeCnt);
      
      } /* End if invalid AttributeCnt */

      /* Incremented if CurSlotIdx invalid which technically is not this object's error, but something wrong with JSON file */
      if (!SchTbl->JsonObj[SCHTBL_OBJ_ACTIVITY].Modified) SchTbl->ObjErrCnt++;
   
   } /* End if valid SchTbl->CurSlotIdx */
   
   return SchTbl->JsonObj[SCHTBL_OBJ_ACTIVITY].Modified;

} /* ActivityCallback() */


