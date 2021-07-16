/*
** Purpose: KIT_TO Packet Table.
**
** Notes:
**   None
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
#include "pkttbl.h"


#define  JSON  &(PktTbl->Json)  /* Convenience macro */

/*
** Type Definitions
*/


/*
** Global File Data
*/

static PKTTBL_Class* PktTbl = NULL;


/*
** Local File Function Prototypes
*/

static boolean WriteJsonPkt(int32 FileHandle, const PKTTBL_Pkt* Pkt, boolean FirstPktWritten);
   
/******************************************************************************
** Function: xxxCallback
**
** Notes:
**   1. These functions must have the same function signature as 
**      JSON_ContainerFuncPtr.
*/
static boolean PktCallback (void* UserData, int TokenIdx);
static boolean FilterCallback (void* UserData, int TokenIdx);


/******************************************************************************
** Function: PKTTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void PKTTBL_Constructor(PKTTBL_Class*       ObjPtr,
                        PKTTBL_GetTblPtr    GetTblPtrFunc,
                        PKTTBL_LoadTbl      LoadTblFunc, 
                        PKTTBL_LoadTblEntry LoadTblEntryFunc)
{
   
   PktTbl = ObjPtr;

   CFE_PSP_MemSet(PktTbl, 0, sizeof(PKTTBL_Class));
   PKTTBL_SetTblToUnused(&(PktTbl->Tbl));

   PktTbl->GetTblPtrFunc    = GetTblPtrFunc;
   PktTbl->LoadTblFunc      = LoadTblFunc;
   PktTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_Constructor(JSON, PktTbl->JsonFileBuf, PktTbl->JsonFileTokens);
   
   JSON_ObjConstructor(&(PktTbl->JsonObj[PKTTBL_OBJ_PKT]),
                       PKTTBL_OBJ_NAME_PKT,
                       PktCallback,
                       (void *)&(PktTbl->Tbl.Pkt));

   JSON_RegContainerCallback(JSON, &(PktTbl->JsonObj[PKTTBL_OBJ_PKT]));
   
   JSON_ObjConstructor(&(PktTbl->JsonObj[PKTTBL_OBJ_FILTER]),
                       PKTTBL_OBJ_NAME_FILTER,
                       FilterCallback,
                       (void *)&(PktTbl->Tbl.Pkt));

   JSON_RegContainerCallback(JSON, &(PktTbl->JsonObj[PKTTBL_OBJ_FILTER]));

} /* End PKTTBL_Constructor() */


/******************************************************************************
** Function: PKTTBL_SetPacketToUnused
**
**
*/
void PKTTBL_SetPacketToUnused(PKTTBL_Pkt* PktPtr)
{
   
   CFE_PSP_MemSet(PktPtr, 0, sizeof(PKTTBL_Pkt));

   PktPtr->StreamId    = PKTTBL_UNUSED_MSG_ID;
   PktPtr->Filter.Type = PKTUTIL_FILTER_ALWAYS;
   
} /* End PKTTBL_SetPacketToUnused() */


/******************************************************************************
** Function: PKTTBL_SetTblToUnused
**
**
*/
void PKTTBL_SetTblToUnused(PKTTBL_Tbl* TblPtr)
{
  
   uint16 AppId;
   
   CFE_PSP_MemSet(TblPtr, 0, sizeof(PKTTBL_Tbl));

   for (AppId=0; AppId < PKTUTIL_MAX_APP_ID; AppId++) {
      
      TblPtr->Pkt[AppId].StreamId    = PKTTBL_UNUSED_MSG_ID;
      TblPtr->Pkt[AppId].Filter.Type = PKTUTIL_FILTER_ALWAYS;
   
   }
   
} /* End PKTTBL_SetTblToUnused() */


/******************************************************************************
** Function: PKTTBL_ResetStatus
**
*/
void PKTTBL_ResetStatus(void)
{
   
   PktTbl->LastLoadStatus  = TBLMGR_STATUS_UNDEF;
   PktTbl->AttrErrCnt      = 0;
   PktTbl->PktLoadCnt      = 0;
   PktTbl->FilterLoadCnt   = 0; 
   
   JSON_ObjArrayReset (PktTbl->JsonObj, PKTTBL_OBJ_CNT);
    
} /* End PKTTBL_ResetStatus() */


/******************************************************************************
** Function: PKTTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean PKTTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int AppId;
   
   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "PKTTBL_LoadCmd() Entry. sizeof(PktTbl->Tbl) = %d\n", sizeof(PktTbl->Tbl));
   
   /* 
   ** Reset status, object modified flags, and data. A non-zero BufLim
   ** value is used to determine whether a packet was loaded.
   */
   PKTTBL_ResetStatus();  
   
   PKTTBL_SetTblToUnused(&(PktTbl->Tbl));

   if (JSON_OpenFile(JSON, Filename)) {
  
      CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, "PKTTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
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
      if (PktTbl->AttrErrCnt == 0) {
      
         if (PktTbl->PktLoadCnt > 0) {
         
            if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
			      PktTbl->LastLoadStatus = ((PktTbl->LoadTblFunc)(&(PktTbl->Tbl)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

	         } /* End if replace entire table */
            else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
		         PktTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
   
               if (PktTbl->JsonObj[PKTTBL_OBJ_PKT].Modified) {
                         
                  for (AppId=0; (AppId < PKTUTIL_MAX_APP_ID) && (PktTbl->LastLoadStatus == TBLMGR_STATUS_VALID); AppId++) {
                          
                     if (PktTbl->Tbl.Pkt[AppId].StreamId != PKTTBL_UNUSED_MSG_ID) {

                        if (!(PktTbl->LoadTblEntryFunc)(AppId, (PKTTBL_Pkt*)PktTbl->JsonObj[PKTTBL_OBJ_PKT].UserData))
                            PktTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
                     }     
                  } /* End packet array loop */                
               } /* End if at least one object modified */
               else {
                  
                  /* This is an un explainable error */
                  CFE_EVS_SendEvent(PKTTBL_LOAD_EMPTY_ERR_EID, CFE_EVS_ERROR, "Load packet table update rejected.");

               } /* End if no objects in file */
               
            } /* End if update records */
			   else {
               
               CFE_EVS_SendEvent(PKTTBL_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"Load packet table rejected. Invalid table command load type %d", LoadType);            
            
            } /* End if invalid command option */ 
            
         } /* End if at least one packet loaded */
         else {
            
            CFE_EVS_SendEvent(PKTTBL_LOAD_UPDATE_ERR_EID, CFE_EVS_ERROR,
			                     "Load packet table command rejected. %s didn't contain any packet defintions", Filename);
         
         } /* End if too many packets in table file */

         if (PktTbl->PktLoadCnt != PktTbl->FilterLoadCnt) {
            
            CFE_EVS_SendEvent(PKTTBL_LOAD_UNDEF_FILTERS_EID, CFE_EVS_INFORMATION, 
                              "Packet table loaded with %d packets and %d undefined filters",
                              PktTbl->PktLoadCnt, (PktTbl->PktLoadCnt-PktTbl->FilterLoadCnt));
                              
         }
      } /* End if no attribute errors */
            
   } /* End if valid file */
   else {
      
      //printf("**ERROR** Processing Packet Table file %s. Status = %d JSMN Status = %d\n",TEST_FILE, Json.FileStatus, Json.JsmnStatus);
      CFE_EVS_SendEvent(PKTTBL_LOAD_OPEN_ERR_EID,CFE_EVS_ERROR,"Load packet table open failure for file %s. File Status = %s JSMN Status = %s",
	                     Filename, JSON_GetFileStatusStr(PktTbl->Json.FileStatus), JSON_GetJsmnErrStr(PktTbl->Json.JsmnStatus));
   
   } /* End if file processing error */

    
   return (PktTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of PKTTBL_LoadCmd() */


/******************************************************************************
** Function: PKTTBL_DumpCmd
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

boolean PKTTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   boolean  FirstPktWritten = FALSE;
   int32    FileHandle;
   uint16   AppId;
   char     DumpRecord[256];
   const PKTTBL_Tbl *PktTblPtr;
   char SysTimeStr[256];
   
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS) {

      PktTblPtr = (PktTbl->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"Kit Telemetry Output (KIT_TO) Packet Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      
      sprintf(DumpRecord,"\"description\": \"KIT_TO dumped at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));


      /* 
      ** Packet Array 
      **
      ** - Not all fields in ground table are saved in FSW so they are not
      **   populated in the dump file. However, the dump file can still
      **   be loaded.
      */
      
      sprintf(DumpRecord,"\"packet-array\": [\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      for (AppId=0; AppId < PKTUTIL_MAX_APP_ID; AppId++) {
               
         if (WriteJsonPkt(FileHandle, &(PktTblPtr->Pkt[AppId]), FirstPktWritten)) FirstPktWritten = TRUE;
              
      } /* End packet loop */

      sprintf(DumpRecord,"\n]}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(PKTTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of PKTTBL_DumpCmd() */



/******************************************************************************
** Function: WriteJsonPkt
**
** Notes:
**   1. Can't end last record with a comma so logic checks that commas only
**      start to be written after the first packet has been written
*/
static boolean WriteJsonPkt(int32 FileHandle, const PKTTBL_Pkt* Pkt, boolean FirstPktWritten) {
   
   boolean PktWritten = FALSE;
   char DumpRecord[256];

   if (Pkt->StreamId != PKTTBL_UNUSED_MSG_ID) {
      
      if (FirstPktWritten) {
         sprintf(DumpRecord,",\n");
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      }
      
      sprintf(DumpRecord,"\"packet\": {\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"   \"dec-id\": %d,\n   \"priority\": %d,\n   \"reliability\": %d,\n   \"buf-limit\": %d,\n",
              Pkt->StreamId, Pkt->Qos.Priority, Pkt->Qos.Reliability, Pkt->BufLim);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      sprintf(DumpRecord,"   \"filter\": { \"type\": %d, \"X\": %d, \"N\": %d, \"O\": %d}\n}",
              Pkt->Filter.Type, Pkt->Filter.Param.X, Pkt->Filter.Param.N, Pkt->Filter.Param.O);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
   
      PktWritten = TRUE;
      
   } /* End if StreamId record has been loaded */
   
   return PktWritten;
   
} /* End WriteJsonPkt() */


/******************************************************************************
** Function: PktCallback
**
** Process a packet table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
**   3. ObjLoadCnt incremented for every packet, valid or invalid.
**      PktLoadIdx index to stored new pkt and incremented for valid packets
*/
static boolean PktCallback (void* UserData, int TokenIdx)
{

   int  AttributeCnt = 0;
   int  JsonIntData;   
   PKTTBL_Pkt Pkt;
   
   PktTbl->JsonObj[PKTTBL_OBJ_PKT].Modified = FALSE; 
   
   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE,
                     "PKTTBL.PktCallback: PktLoadCnt %d, AttrErrCnt %d, TokenIdx %d",
                     PktTbl->PktLoadCnt, PktTbl->AttrErrCnt, TokenIdx);
      
   PKTTBL_SetPacketToUnused(&Pkt); /* Default filter to always filtered in case not loaded by FilterCallback() */
  
   if (JSON_GetValShortInt(JSON, TokenIdx, "dec-id",      &JsonIntData)) { AttributeCnt++; Pkt.StreamId        = (CFE_SB_MsgId_t) JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "priority",    &JsonIntData)) { AttributeCnt++; Pkt.Qos.Priority    = (uint8) JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "reliability", &JsonIntData)) { AttributeCnt++; Pkt.Qos.Reliability = (uint8) JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "buf-limit",   &JsonIntData)) { AttributeCnt++; Pkt.BufLim          = (uint16)JsonIntData; }
   
   
   if (AttributeCnt == 4) {
   
      ++PktTbl->PktLoadCnt;
      
      PktTbl->CurAppId = Pkt.StreamId & PKTTBL_APP_ID_MASK;     
      PktTbl->Tbl.Pkt[PktTbl->CurAppId] = Pkt;   
      
      PktTbl->JsonObj[PKTTBL_OBJ_PKT].Modified = TRUE;
      
      CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, 
                        "PKTTBL.PktCallback (Stream ID, BufLim, Priority, Reliability): %d, %d, %d, %d",
                        Pkt.StreamId, Pkt.Qos.Priority, Pkt.Qos.Reliability, Pkt.BufLim);
   
   } /* End if valid AttributeCnt */
   else {
	   
      ++PktTbl->AttrErrCnt;     
      CFE_EVS_SendEvent(PKTTBL_LOAD_PKT_ATTR_ERR_EID, CFE_EVS_ERROR, "Invalid number of packet attributes %d. Should be 4.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return PktTbl->JsonObj[PKTTBL_OBJ_PKT].Modified;

} /* PktCallback() */


/******************************************************************************
** Function: FilterCallback
**
** Process a filter table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. UserData is unused.
**   3. ObjLoadCnt incremented for every packet, valid or invalid.
**      PktLoadIdx index to stored new pkt and incremented for valid packets
**   4. Filter must be defined within a packet structure and this code assumes
**      the filter corresponds to most recent packet callback
*/
static boolean FilterCallback (void* UserData, int TokenIdx)
{

   int  AttributeCnt = 0;
   int  JsonIntData;     
   PktUtil_Filter Filter;
   
   PktTbl->JsonObj[PKTTBL_OBJ_FILTER].Modified = FALSE;
   
   CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE,
                     "PKTTBL.FilterCallback: Current ApId 0x%04X, ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d",
                     PktTbl->CurAppId, PktTbl->PktLoadCnt, PktTbl->AttrErrCnt, TokenIdx);

   if (JSON_GetValShortInt(JSON, TokenIdx, "type", &JsonIntData)) { AttributeCnt++; Filter.Type    = (uint16)JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "X",    &JsonIntData)) { AttributeCnt++; Filter.Param.X = (uint16)JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "N",    &JsonIntData)) { AttributeCnt++; Filter.Param.N = (uint16)JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "O",    &JsonIntData)) { AttributeCnt++; Filter.Param.O = (uint16)JsonIntData; }
   
   
   if (AttributeCnt == 4) {
   
      ++PktTbl->FilterLoadCnt;

      PktTbl->Tbl.Pkt[PktTbl->CurAppId].Filter = Filter;   

      PktTbl->JsonObj[PKTTBL_OBJ_FILTER].Modified = TRUE;
               
      CFE_EVS_SendEvent(KIT_TO_INIT_DEBUG_EID, KIT_TO_INIT_EVS_TYPE, 
                        "PKTTBL.FilterCallback (Type, X, N, O): %d, %d, %d, %d",
                        Filter.Type, Filter.Param.X, Filter.Param.N, Filter.Param.O);
   
   } /* End if valid AttributeCnt */
   else {
	   
      ++PktTbl->AttrErrCnt;     
      CFE_EVS_SendEvent(PKTTBL_LOAD_PKT_ATTR_ERR_EID, CFE_EVS_ERROR, "Invalid number of packet attributes %d. Should be 4.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return PktTbl->JsonObj[PKTTBL_OBJ_FILTER].Modified;

} /* FilterCallback() */
