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

/* Convenience macro */
#define  JSON_OBJ  &(PktTbl->Json)

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

/******************************************************************************
** Function: xxxCallback
**
** Notes:
**   1. These functions must have the same function signature as 
**      JSON_ContainerFuncPtr.
*/
static boolean PktCallback (int TokenIdx);

/// TODO - Move to OSK_APP_FW
/******************************************************************************
** Function: JSON_ObjConstructor
**
** Notes:
**    1. This must be called prior to any other functions using the JSON_OBJ
**    2. The object name must be identical (case sensitive) to the name in the
**       JSON file. 
**
*/
void JSON_ObjConstructor(JSON_Obj*              Obj,
                         char*                  Name,
                         JSON_ContainerFuncPtr  Callback,
                         void*                  Data)
{
	
   strncpy(&(Obj->Name[0]), Name, JSON_OBJ_NAME_MAX_CHAR);
   Obj->Modified = FALSE;
   Obj->Callback = Callback;
   Obj->Data     = Data;
   

} /* End JSON_ObjConstructor() */

/******************************************************************************
** Function: JSON_ObjArrayReset
**
** Notes:
**    None
**
*/
void JSON_ObjArrayReset(JSON_Obj* ObjArray,
                       uint16    ObjCnt)
{
	int i;
   
   for (i=0; i < ObjCnt; i++) ObjArray[i].Modified = FALSE;
   
} /* End JSON_ObjArrayReset() */


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

   PktTbl->GetTblPtrFunc    = GetTblPtrFunc;
   PktTbl->LoadTblFunc      = LoadTblFunc;
   PktTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_Constructor(JSON_OBJ, PktTbl->JsonFileBuf, PktTbl->JsonFileTokens);
   
   JSON_ObjConstructor(&(PktTbl->JsonObj[PKTTBL_OBJ_PKT]),
                       PKTTBL_OBJ_PKT_NAME,
                       PktCallback,
                       (void *)&(PktTbl->Tbl.Pkt));
   
   JSON_RegContainerCallback(JSON_OBJ,
	                          PktTbl->JsonObj[PKTTBL_OBJ_PKT].Name,
	                          PktTbl->JsonObj[PKTTBL_OBJ_PKT].Callback);

} /* End PKTTBL_Constructor() */



/******************************************************************************
** Function: PKTTBL_ResetStatus
**
*/
void PKTTBL_ResetStatus(void)
{
   
   PktTbl->LastLoadStatus  = TBLMGR_STATUS_UNDEF;
   PktTbl->AttrErrCnt      = 0;
   PktTbl->MaxObjErrCnt    = 0;
   PktTbl->ObjLoadCnt      = 0;
   PktTbl->PktLoadIdx      = 0;
   
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

   int obj, pkt;
   
   OS_printf("PKTTBL_LoadCmd() Entry. sizeof(PktTbl->Tbl) = %d\n",sizeof(PktTbl->Tbl));
   
   /* 
   ** Reset status, object modified flags, and data. A non-zero BufLim
   ** value is used to determine whether a packet was loaded.
   */
   PKTTBL_ResetStatus();  
   CFE_PSP_MemSet(&(PktTbl->Tbl), 0, sizeof(PktTbl->Tbl));
   
   if (JSON_OpenFile(JSON_OBJ, Filename)) {
  
      OS_printf("************************************************\n");
      OS_printf("PKTTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
      ///JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      ///JSON_PrintTokens(&Json,50);
    
      JSON_ProcessTokens(JSON_OBJ);

      /* 
      ** Only process command if no attribute errors. No need to send an event
      ** message if there are attribute errors since events are sent for each
      ** error.  Checking at least one packet is a simple check but that's all
      ** that can be done because the table doesn't have any other constraints.
      ** A table with no packets is considered erroneous.
      */
      if (PktTbl->AttrErrCnt == 0) {
      
         if (PktTbl->PktLoadIdx > 0) {
         
            if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
			      PktTbl->LastLoadStatus = ((PktTbl->LoadTblFunc)(&(PktTbl->Tbl)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

	         } /* End if replace entire table */
            else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
		         PktTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
   
               /* 
               ** Logic is written to easily support the addition of new JSON 
               ** objects. Currently only array of packets is implemented.
               */
               if (PktTbl->ObjLoadCnt > 0 ) {
                  for (obj=0; obj < PKTTBL_OBJ_CNT; obj++) {
                   
                     if (PktTbl->JsonObj[obj].Modified) {
                     
                        /* 
                        ** If at least one packet was modified then loop through
                        ** the packet array and update each packet.
                        */                     
                        if (obj == PKTTBL_OBJ_PKT) {
                       
                           for (pkt=0; pkt < PktTbl->PktLoadIdx; pkt++) {
                          
                              if (!(PktTbl->LoadTblEntryFunc)(pkt, (PKTTBL_Pkt*)&(PktTbl->JsonObj[obj].Data)))
                                 PktTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
                          
                           } /* End packet array loop */                
                           
                        } /* End if PKTTBL_OBJ_PKT */ 
                     } /* End if object modified */
                  } /* End JSON object loop */
               } /* End if at least one object */
               else {
                  
                  CFE_EVS_SendEvent(PKTTBL_CMD_LOAD_EMPTY_ERR_EID,CFE_EVS_ERROR,"PKTTBL: Invalid table command. No packets defined.");

               } /* End if no objects in file */
               
            } /* End if update records */
			   else {
               
               CFE_EVS_SendEvent(PKTTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"PKTTBL: Invalid table command load type %d",LoadType);            
            
            } /* End if invalid command option */ 
            
         } /* End if valid packet index */
         else {
            
            CFE_EVS_SendEvent(PKTTBL_CMD_LOAD_UPDATE_ERR_EID,CFE_EVS_ERROR,
			                     "PKTTBL: Update table command rejected. File contained %d packet objects which exceeds the max table size of %d",
							         PktTbl->ObjLoadCnt, PKTTBL_MAX_PKT_CNT);
         
         } /* End if too many packets in table file */

      } /* End if no attribute errors */
            
   } /* End if valid file */
   else {
      
      //printf("**ERROR** Processing Packet Table file %s. Status = %d JSMN Status = %d\n",TEST_FILE, Json.FileStatus, Json.JsmnStatus);
      CFE_EVS_SendEvent(PKTTBL_CMD_LOAD_OPEN_ERR_EID,CFE_EVS_ERROR,"PKTTBL: Table open failure for file %s. File Status = %s JSMN Status = %s",
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
   int32    FileHandle,i;
   char     DumpRecord[256];
   const PKTTBL_Tbl *PktTblPtr;
   char SysTimeStr[256];
   
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS)
   {

      PktTblPtr = (PktTbl->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"Kit Telemetry Output (KIT_TO) Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      
      sprintf(DumpRecord,"\"description\": \"KIT_TO dump at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));


      /* 
      ** Packet Array 
      **
      ** - Not all fields in ground table are saved in FSW so they are not
      **   populated in the dump file. However, the dump file can still
      **   be loaded.
      */
      
      sprintf(DumpRecord,"\"packet-array\": [\n");

      for (i=0; i < PKTTBL_MAX_PKT_CNT; i++) {
      
         sprintf(DumpRecord,"\"packet\": {\n");
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
         sprintf(DumpRecord,"   \"dec-id\": %d,\n   \"priority\": %d,\n   \"reliability\": %d,\n   \"buf-lim\": %d\n},\n",
                 PktTblPtr->Pkt[i].StreamId,
                 PktTblPtr->Pkt[i].Qos.Priority,
                 PktTblPtr->Pkt[i].Qos.Reliability,
                 PktTblPtr->Pkt[i].BufLim);
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      } /* End packet loop */

      sprintf(DumpRecord,"]\n}\n");
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
** Function: PktCallback
**
** Process a packet table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. ObjLoadCnt incremented for every packet, valid or invalid.
**      PktLoadIdx index to stored new pkt and incremented for valid packets
*/
static boolean PktCallback (int TokenIdx)
{

   int  AttributeCnt = 0;
   int  JsonIntData;
   boolean RetStatus = FALSE;      
   PKTTBL_Pkt Pkt;

   OS_printf("\nPKTTBL.PktCallback: ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d\n",
             PktTbl->ObjLoadCnt, PktTbl->AttrErrCnt, TokenIdx);
      
   if (JSON_GetValShortInt(JSON_OBJ, TokenIdx, "dec-id",      &JsonIntData)) { AttributeCnt++; Pkt.StreamId        = (CFE_SB_MsgId_t) JsonIntData; }
   if (JSON_GetValShortInt(JSON_OBJ, TokenIdx, "priority",    &JsonIntData)) { AttributeCnt++; Pkt.Qos.Priority    = (uint8) JsonIntData; }
   if (JSON_GetValShortInt(JSON_OBJ, TokenIdx, "reliability", &JsonIntData)) { AttributeCnt++; Pkt.Qos.Reliability = (uint8) JsonIntData; }
   if (JSON_GetValShortInt(JSON_OBJ, TokenIdx, "buf-limit",   &JsonIntData)) { AttributeCnt++; Pkt.BufLim          = (uint16)JsonIntData; }
   
   PktTbl->ObjLoadCnt++;
   
   if (AttributeCnt == 4) {
   
      if (PktTbl->PktLoadIdx < PKTTBL_MAX_PKT_CNT) {
         
         PktTbl->Tbl.Pkt[PktTbl->PktLoadIdx] = Pkt;
         PktTbl->PktLoadIdx++;      
         RetStatus = TRUE;
      
      } /* End if PktLoadIdx within limits */
      else {
         PktTbl->MaxObjErrCnt++;
      }
     
      OS_printf("PKTTBL.PktCallback (Stream ID, BufLim, Priority, Reliability): %d, %d, %d, %d\n",
                Pkt.StreamId, Pkt.Qos.Priority, Pkt.Qos.Reliability, Pkt.BufLim);
   
   } /* End if valid AttributeCnt */
   else {
	   
      PktTbl->AttrErrCnt++;     
      CFE_EVS_SendEvent(PKTTBL_LOAD_PKT_ATTR_ERR_EID, CFE_EVS_ERROR, "Invalid number of packet attributes %d. Should be 4.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return RetStatus;

} /* PktCallback() */


/* end of file */
