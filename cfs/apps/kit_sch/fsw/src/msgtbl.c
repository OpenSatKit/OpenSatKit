/* 
** Purpose: Implement KIT_SCH's Message Table management functions.
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
#include "msgtbl.h"


#define JSON  &(MsgTbl->Json)  /* Convenience macro */

/*
** Type Definitions
*/


/*
** Global File Data
*/

static MSGTBL_Class* MsgTbl = NULL;

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
static boolean MsgCallback (void* UserData, int TokenIdx);

/******************************************************************************
** Function: SplitStr
**
*/
static char *SplitStr(char *Str, const char *Delim);


/******************************************************************************
** Function: MSGTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void MSGTBL_Constructor(MSGTBL_Class*       ObjPtr,
                        MSGTBL_GetTblPtr    GetTblPtrFunc,
                        MSGTBL_LoadTbl      LoadTblFunc, 
                        MSGTBL_LoadTblEntry LoadTblEntryFunc)
{
   
   MsgTbl = ObjPtr;

   CFE_PSP_MemSet(MsgTbl, 0, sizeof(MSGTBL_Class));

   MsgTbl->GetTblPtrFunc    = GetTblPtrFunc;
   MsgTbl->LoadTblFunc      = LoadTblFunc;
   MsgTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_Constructor(JSON, MsgTbl->JsonFileBuf, MsgTbl->JsonFileTokens);
   
   JSON_ObjConstructor(&(MsgTbl->JsonObj[MSGTBL_OBJ_MSG]),
                       MSGTBL_OBJ_NAME_MSG,
                       MsgCallback,
                       (void *)&(MsgTbl->Tbl.Entry));
   
   JSON_RegContainerCallback(JSON, &(MsgTbl->JsonObj[MSGTBL_OBJ_MSG]));

} /* End MSGTBL_Constructor() */


/******************************************************************************
** Function: MSGTBL_ResetStatus
**
*/
void MSGTBL_ResetStatus(void)
{
   
   MsgTbl->LastLoadStatus  = TBLMGR_STATUS_UNDEF;
   MsgTbl->AttrErrCnt      = 0;
   MsgTbl->ObjErrCnt       = 0;
   MsgTbl->ObjLoadCnt      = 0;
   
   JSON_ObjArrayReset (MsgTbl->JsonObj, MSGTBL_OBJ_CNT);
    
} /* End MSGTBL_ResetStatus() */


/******************************************************************************
** Function: MSGTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean MSGTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int obj, msg;
   
   CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                     "MSGTBL_LoadCmd() Entry. sizeof(MsgTbl->Tbl) = %d\n",sizeof(MsgTbl->Tbl));
   
   /* 
   ** Reset status, object modified flags, and data. A non-zero BufLim
   ** value is used to determine whether a packet was loaded.
   */
   MSGTBL_ResetStatus();  
   CFE_PSP_MemSet(&(MsgTbl->Tbl), 0, sizeof(MsgTbl->Tbl));

   if (JSON_OpenFile(JSON, Filename)) {
  
      CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,"MSGTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
      //DEBUG JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      //DEBUG JSON_PrintTokens(&Json,50);
    
      JSON_ProcessTokens(JSON);

      /* 
      ** Only process command if no attribute errors. No need to send an event
      ** message if there are attribute errors since events are sent for each
      ** error.  Checking at least one packet is a simple check but that's all
      ** that can be done because the table doesn't have any other constraints.
      ** A table with no packets is considered erroneous.
      */
      if (MsgTbl->AttrErrCnt == 0) {
      
         if (MsgTbl->ObjLoadCnt > 0) {
         
            if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
			      MsgTbl->LastLoadStatus = ((MsgTbl->LoadTblFunc)(&(MsgTbl->Tbl)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

	         } /* End if replace entire table */
            else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
		         MsgTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
   
               /* 
               ** Logic is written to easily support the addition of new JSON 
               ** objects. Currently only array of packets is implemented.
               */
               if (MsgTbl->ObjLoadCnt > 0 ) {
                  for (obj=0; obj < MSGTBL_OBJ_CNT; obj++) {
                   
                     if (MsgTbl->JsonObj[obj].Modified) {
                     
                        /* 
                        ** If at least one packet was modified then loop through
                        ** the entire message array and update each non-zero length message.
                        */                     
                        if (obj == MSGTBL_OBJ_MSG) {
                       
                           for (msg=0; msg < MSGTBL_MAX_ENTRIES; msg++) {
                         
                              if  ((MsgTbl->Tbl.Entry[msg].Buffer[2] > 0)) {
                                 if (!(MsgTbl->LoadTblEntryFunc)(msg, (MSGTBL_Entry*)&MsgTbl->Tbl.Entry[msg]))
                                    MsgTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
                              }
                           } /* End message array loop */                
                           
                        } /* End if MSGTBL_OBJ_MSG */ 
                     } /* End if object modified */
                  } /* End JSON object loop */
               } /* End if at least one object */
               else {
                  
                  CFE_EVS_SendEvent(MSGTBL_LOAD_EMPTY_ERR_EID,CFE_EVS_ERROR,"MSGTBL: Invalid table command. No messages defined.");

               } /* End if no objects in file */
               
            } /* End if update records */
			   else {
               
               CFE_EVS_SendEvent(MSGTBL_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"MSGTBL: Invalid table command load type %d",LoadType);            
            
            } /* End if invalid command option */ 
            
         } /* End if valid message index */
         else {
            
            CFE_EVS_SendEvent(MSGTBL_LOAD_EMPTY_ERR_EID,CFE_EVS_ERROR,"MSGTBL: Invalid table command. No message objects defined.");
            
         
         } /* End if no objects loaded */

      } /* End if no attribute errors */
            
   } /* End if valid file */
   else {
      
      //printf("**ERROR** Processing Message Table file %s. Status = %d JSMN Status = %d\n",TEST_FILE, Json.FileStatus, Json.JsmnStatus);
      CFE_EVS_SendEvent(MSGTBL_LOAD_OPEN_ERR_EID,CFE_EVS_ERROR,"MSGTBL: Table open failure for file %s. File Status = %s JSMN Status = %s",
	                     Filename, JSON_GetFileStatusStr(MsgTbl->Json.FileStatus), JSON_GetJsmnErrStr(MsgTbl->Json.JsmnStatus));
   
   } /* End if file processing error */
    
   return (MsgTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of MSGTBL_LoadCmd() */


/******************************************************************************
** Function: MSGTBL_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file. If the
**     filename exists it will be overwritten.
**  3. File is formatted so it can be used as a load file. 
**  4. DumpType is unused.
*/

boolean MSGTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   int32    FileHandle, i, d;
   char     DumpRecord[256];
   const    MSGTBL_Tbl *MsgTblPtr;
   char     SysTimeStr[256];
   uint16   DataWords;
   
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS) {

      MsgTblPtr = (MsgTbl->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"Kit Scheduler (KIT_SCH) Message Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      
      sprintf(DumpRecord,"\"description\": \"KIT_SCH table dumped at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));


      /* 
      ** Message Array 
      **
      ** - Not all fields in ground table are saved in FSW so they are not
      **   populated in the dump file. However, the dump file can still
      **   be loaded.
      **
      **   "name":  Not loaded,
      **   "descr": Not Loaded,
      **   "id": 101,
      **   "stream-id": 65303,
      **   "seq-seg": 192,
      **   "length": 1792,
      **   "data-words": "0,1,2,3,4,5"
      */
      
      sprintf(DumpRecord,"\"message-array\": [\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      for (i=0; i < MSGTBL_MAX_ENTRIES; i++) {
         
         /* 
         ** JSMN accepted the message keyword in an array without being in a
         ** new object but ruby JSON parser doesn't. I think JSMN is wrong
         */
         if (i > 0) { 
            sprintf(DumpRecord,",\n");
            OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
         }
          
         sprintf(DumpRecord,"   {\"message\": {\n");
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
         
         sprintf(DumpRecord,"      \"id\": %d,\n      \"stream-id\": %d,\n      \"seq-seg\": %d,\n      \"length\": %d",
                 i,
                 CFE_MAKE_BIG16(MsgTblPtr->Entry[i].Buffer[0]),
                 CFE_MAKE_BIG16(MsgTblPtr->Entry[i].Buffer[1]),
                 CFE_MAKE_BIG16(MsgTblPtr->Entry[i].Buffer[2]));
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
         
         /*
         ** DataWords is everything past the primary header so they include
         ** the secondary header and don't distinguish between cmd or tlm
         ** packets. 
         */
         DataWords = (CFE_SB_GetTotalMsgLength((const CFE_SB_Msg_t *)MsgTblPtr->Entry[i].Buffer) - PKTUTIL_PRI_HDR_BYTES)/2;

         if (DataWords > (uint8)(MSGTBL_MAX_MSG_WORDS)) {
            
            CFE_EVS_SendEvent(MSGTBL_DUMP_MSG_ERR_EID, CFE_EVS_ERROR,
                              "Error creating dump file message entry %d. Message word length %d is greater than max data buffer %d",
                              i, DataWords, PKTUTIL_PRI_HDR_WORDS);         
         }
         else {

            /* 
            ** Omit "data-words" property if no data
            ** - Properly terminate 'length' line 
            */
            if (DataWords > 0) {
         
               sprintf(DumpRecord,",\n      \"data-words\": \"");         
               OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
                  
               for (d=0; d < DataWords; d++) {
                  
                  if (d == (DataWords-1)) {
                     sprintf(DumpRecord,"%d\"\n   }}",MsgTblPtr->Entry[i].Buffer[PKTUTIL_PRI_HDR_WORDS+d]);
                  }
                  else {
                     sprintf(DumpRecord,"%d,",MsgTblPtr->Entry[i].Buffer[PKTUTIL_PRI_HDR_WORDS+d]);
                  }
                  OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

               } /* End DataWord loop */
                           
            } /* End if non-zero data words */
            else {
               sprintf(DumpRecord,"\n   }}");         
               OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
            }
         } /* End if DataWords within range */

      } /* End message loop */

      /* Close message-array and top-level object */      
      sprintf(DumpRecord,"\n]}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(MSGTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   if (RetStatus) {
      
      CFE_EVS_SendEvent(MSGTBL_DUMP_INFO_EID, CFE_EVS_INFORMATION,
                        "Successfully dumped message table to %s", Filename);
   }
   
   return RetStatus;
   
} /* End of MSGTBL_DumpCmd() */


/******************************************************************************
** Function: MSGTBL_GetMsgPtr
**
*/
boolean MSGTBL_GetMsgPtr(uint16  EntryId, uint16 **MsgPtr)
{

   boolean RetStatus = FALSE;

   if (EntryId < MSGTBL_MAX_ENTRIES) {

      CFE_EVS_SendEvent(MSGTBL_DEBUG_EID, CFE_EVS_DEBUG,"MSGTBL_GetMsgPtr(): EntryId = %d, Buffer[0] = 0x%04x", EntryId, MsgTbl->Tbl.Entry[EntryId].Buffer[0]);

      *MsgPtr = MsgTbl->Tbl.Entry[EntryId].Buffer;
      RetStatus = TRUE;
                

   } /* End if valid EntryId */

   return RetStatus;

} /* End MSGTBL_GetMsgPtr() */


/******************************************************************************
** Function: MsgCallback
**
** Process a message table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
**   3. ObjLoadCnt incremented for every message, valid or invalid.
*/
static boolean MsgCallback (void* UserData, int TokenIdx)
{

   int    AttributeCnt = 0;
   int    JsonIntData, Id, i;
   char   DataStr[(MSGTBL_MAX_MSG_WORDS-3)*10];  /* 6 digits per word plus any extra commas and spaces */
   char  *DataStrPtr;
   boolean DataWords = FALSE;      
   MSGTBL_Entry MsgEntry;


   MsgTbl->JsonObj[MSGTBL_OBJ_MSG].Modified = FALSE;
   
   CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,"\nMSGTBL.MsgCallback: ObjLoadCnt %d, ObjErrCnt %d, AttrErrCnt %d, TokenIdx %d\n",
                     MsgTbl->ObjLoadCnt, MsgTbl->ObjErrCnt, MsgTbl->AttrErrCnt, TokenIdx);

   memset((void*)&MsgEntry,0,sizeof(MSGTBL_Entry));
   
   /* 
   ** Message
   **
   **   "name":  Not saved,
   **   "descr": Not saved,
   **   "id": 101,
   **   "stream-id": 65303,
   **   "seq-seg": 192,
   **   "length": 1792,
   **   "data-words": "0,1,2,3,4,5"
   **
   ** The data words contain the secondary header if it is present. No integrity checks are made on 
   ** the packet.
   */
   if (JSON_GetValShortInt(JSON, TokenIdx, "id",         &JsonIntData)) { AttributeCnt++; Id = JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "stream-id",  &JsonIntData)) { AttributeCnt++; MsgEntry.Buffer[0] = CFE_MAKE_BIG16((uint16)JsonIntData); }
   if (JSON_GetValShortInt(JSON, TokenIdx, "seq-seg",    &JsonIntData)) { AttributeCnt++; MsgEntry.Buffer[1] = CFE_MAKE_BIG16((uint16)JsonIntData); }
   if (JSON_GetValShortInt(JSON, TokenIdx, "length",     &JsonIntData)) { AttributeCnt++; MsgEntry.Buffer[2] = CFE_MAKE_BIG16((uint16)JsonIntData); }
   
   if (JSON_GetValStr(JSON, TokenIdx, "data-words", DataStr)) {
      
      AttributeCnt++; 
      i = 3;
      
      /* No protection against ill-formed data array */
      DataStrPtr = SplitStr(DataStr,",");
      if ( DataStrPtr != NULL) {
         MsgEntry.Buffer[i++] = atoi(DataStrPtr);
         CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                           "MSGTBL.MsgCallback data[%d] = 0x%4X, DataStrPtr=%s\n",i-1,MsgEntry.Buffer[i-1],DataStrPtr);         
         while ( (DataStrPtr = SplitStr(NULL,",")) != NULL) {
            MsgEntry.Buffer[i++] = atoi(DataStrPtr);
            CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,
                              "MSGTBL.MsgCallback data[%d] = 0x%4X, DataStrPtr=%s\n",i-1,MsgEntry.Buffer[i-1],DataStrPtr);
         }
      }
      
      DataWords = TRUE;
      
   } /* End if DataStr */
   
   MsgTbl->ObjLoadCnt++;
   
   /* data-words is optional */
   if ( (DataWords  && AttributeCnt == 5) ||
        (!DataWords && AttributeCnt == 4) ) {
   
      if (Id < MSGTBL_MAX_ENTRIES) {
         
         MsgTbl->Tbl.Entry[Id] = MsgEntry;
         MsgTbl->JsonObj[MSGTBL_OBJ_MSG].Modified = TRUE;
      
      } /* End if Id within limits */
      else {
         MsgTbl->ObjErrCnt++;
      }
     
      CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,"MSGTBL.MsgCallback Id %d (Stream ID, Seq-Seg, Length, Data[0]): %d, %d, %d, %d\n",
                        Id, MsgEntry.Buffer[0], MsgEntry.Buffer[1], MsgEntry.Buffer[2], MsgEntry.Buffer[3]);
   
   } /* End if valid AttributeCnt */
   else {
	   
      MsgTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(MSGTBL_LOAD_MSG_ATTR_ERR_EID, CFE_EVS_ERROR, "Invalid number of packet attributes %d. Should be 5.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return MsgTbl->JsonObj[MSGTBL_OBJ_MSG].Modified;

} /* MsgCallback() */


/******************************************************************************
** Function: SplitStr
**
** Split a string based on a delimiter. 
**
** Example Usage
**    char str[] = "A,B,,,C";
**    printf("1 %s\n",zstring_strtok(s,","));
**    printf("2 %s\n",zstring_strtok(NULL,","));
**    printf("3 %s\n",zstring_strtok(NULL,","));
**    printf("4 %s\n",zstring_strtok(NULL,","));
**    printf("5 %s\n",zstring_strtok(NULL,","));
**    printf("6 %s\n",zstring_strtok(NULL,","));
** Example Output
**    1 A
**    2 B
**    3 ,
**    4 ,
**    5 C
**    6 (null)
**
** Notes:
**   1. Plus: No extra memory required
**   2. Minus: Use of static variable is not rentrant so can't use in app_fw
**   3. Minus: Modifies caller's string
*/
static char *SplitStr(char *Str, const char *Delim) {
   
   static char *StaticStr=NULL;      /* Store last address */
   int i=0, StrLength=0;
   boolean DelimFound = FALSE;                  

   /* Valid Delim and have characters left */
   if (Delim == NULL || (Str == NULL && StaticStr == NULL))
      return NULL;

   if (Str == NULL)
      Str = StaticStr;

   StrLength = strlen(&Str[StrLength]);

   /* find the first occurance of delim */
   for (i=0; i < StrLength; i++) {
      if (Str[i] == Delim[0]) {
         DelimFound = TRUE;
         break;
      }
   }
   
   if (!DelimFound) {
      StaticStr = NULL;
      return Str;
   }

   /* Check for consecutive delimiters */
   if (Str[0] == Delim[0]) {
      StaticStr = (Str + 1);
      return (char *)Delim;
   }

   /* terminate the string
    * this assignment requires char[], so str has to
    * be char[] rather than *char
    */
   Str[i] = '\0';

   /* save the rest of the string */
   if ((Str + i + 1) != 0)
      StaticStr = (Str + i + 1);
   else
      StaticStr = NULL;

   return Str;

} /* End SplitStr() */

