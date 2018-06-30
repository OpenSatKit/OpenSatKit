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

/* Convenience macro */
#define  JSON_OBJ  &(MsgTbl->Json)

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
static boolean MsgCallback (int TokenIdx);

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

   JSON_Constructor(JSON_OBJ, MsgTbl->JsonFileBuf, MsgTbl->JsonFileTokens);
   
   JSON_ObjConstructor(&(MsgTbl->JsonObj[MSGTBL_OBJ_PKT]),
                       MSGTBL_OBJ_PKT_NAME,
                       MsgCallback,
                       (void *)&(MsgTbl->Tbl.Entry));
   
   JSON_RegContainerCallback(JSON_OBJ,
	                          MsgTbl->JsonObj[MSGTBL_OBJ_PKT].Name,
	                          MsgTbl->JsonObj[MSGTBL_OBJ_PKT].Callback);

} /* End MSGTBL_Constructor() */


/******************************************************************************
** Function: MSGTBL_ResetStatus
**
*/
void MSGTBL_ResetStatus(void)
{
   
   MsgTbl->LastLoadStatus  = TBLMGR_STATUS_UNDEF;
   MsgTbl->AttrErrCnt      = 0;
   MsgTbl->MaxObjErrCnt    = 0;
   MsgTbl->ObjLoadCnt      = 0;
   MsgTbl->PktLoadIdx      = 0;
   
   JSON_ObjArrayReset (MsgTbl->JsonObj, MSGTBL_OBJ_CNT);
    
} /* End MSGTBL_ResetStatus() */









/*
** Local File Function Prototypes
*/

typedef void (*DumpTableFuncPtr)(int32 FileHandle);   /* File-specific dump table function */

/******************************************************************************
** Function: DumpTableToCfeFile
**
** Dump a table to a cFE compatible dump file. This function is written as a reusable
** function and a function pointer is passed to a function that performs the table-specific
** processing.
**
*/
static boolean DumpTableToCfeFile(const char* Filename, const char* FileDescr, DumpTableFuncPtr DumpTableFunc);

/******************************************************************************
** Function: DumpTableToXmlFile
**
** Dump a table to a well-formed XML file that is suitable for loading. This function is
** written as a reusable function and a function pointer is passed to a function that
** performs the table-specific processing.
**
*/
static boolean DumpTableToXmlFile(const char* Filename, const char* FileDescr, DumpTableFuncPtr DumpTableFunc);

/******************************************************************************
** Function: WriteStartElement
**
** Write a start element with no attributes to an XML dump file.
**
*/
static int32 WriteStartElement(int32 FileHandle, char* ElementName, int Level);

/******************************************************************************
** Function: WriteEndElement
**
** Write an end element to an XML dump file.
**
*/
static int32 WriteEndElement(int32 FileHandle, char* ElementName, int Level);


/******************************************************************************
** Function: ParseXmlFile
**
** General purpose XML file parser.  The Start and End element callback
** functions do the file-specific processing.
**
*/
static boolean ParseXmlFile(const char* Filename,
                            XML_StartElementHandler StartElementHandler,
                            XML_EndElementHandler   EndElementHandler);

/*#############################################################################
** Functions: MsgTblStartElement, MsgTblEndElement, WriteMsgTblToCfeFile
**
** These functions provide the packet table specific processing for XML parsing
** and for dump file formatting.
**
*/
static void XMLCALL MsgTblStartElement(void *data, const char *el, const char **attr);
static void XMLCALL MsgTblEndElement(void *data, const char *el);
static void WriteMsgTblToCfeFile(int32 FileHandle);

/*#############################################################################
** Functions: WriteMsgTblToXmlFile, WriteMsgEntryElement,
**
** These functions provide the packet table-specific processing for XML
** dump file formatting.
**
*/
static void WriteMsgTblToXmlFile(int32 FileHandle);
static int32 WriteMsgEntryElement(int32 FileHandle,const MSGTBL_Entry *TblEntry, int Index, int Level);



/******************************************************************************
** Function: MSGTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void MSGTBL_Constructor(MSGTBL_Class* ObjPtr,
                        MSGTBL_GetTblPtr    GetTblPtrFunc,
                        MSGTBL_LoadTbl      LoadTblFunc, 
                        MSGTBL_LoadTblEntry LoadTblEntryFunc)
{
 
   MsgTbl = ObjPtr;

  /*
    * Set all data and flags to zero.
    * AttrErrCnt - Incremented by XML parser
    */
   CFE_PSP_MemSet(MsgTbl, 0, sizeof(MSGTBL_Class));

   MsgTbl->GetTblPtrFunc    = GetTblPtrFunc;
   MsgTbl->LoadTblFunc      = LoadTblFunc;
   MsgTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

 
} /* End MSGTBL_Constructor() */


/******************************************************************************
** Function:  MSGTBL_ResetStatus
**
*/
void MSGTBL_ResetStatus()
{
   int entry;

   MsgTbl->LastLoadStatus = TBLMGR_STATUS_UNDEF;
   MsgTbl->AttrErrCnt = 0;

   for (entry=0; entry < MSGTBL_MAX_ENTRY_ID; entry++)
      MsgTbl->Modified[entry] = FALSE;

} /* End MSGTBL_ResetStatus() */


/******************************************************************************
** Function: MSGTBL_GetMsgPtr
**
*/
boolean MSGTBL_GetMsgPtr(uint16  EntryId, uint16 **MsgPtr)
{

   boolean RetStatus = FALSE;

   if (EntryId < MSGTBL_MAX_ENTRY_ID) {

      *MsgPtr = MsgTbl->Tbl.Entry[EntryId].Buffer;
      RetStatus = TRUE;
 
   } /* End if valid EntryId */

   return RetStatus;

} /* End MSGTBL_GetMsgPtr() */


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
   
   OS_printf("MSGTBL_LoadCmd() Entry. sizeof(MsgTbl->Tbl) = %d\n",sizeof(MsgTbl->Tbl));
   
   /* 
   ** Reset status, object modified flags, and data. A non-zero BufLim
   ** value is used to determine whether a packet was loaded.
   */
   MSGTBL_ResetStatus();  
   CFE_PSP_MemSet(&(MsgTbl->Tbl), 0, sizeof(MsgTbl->Tbl));
   
   if (JSON_OpenFile(JSON_OBJ, Filename)) {
  
      OS_printf("************************************************\n");
      OS_printf("MSGTBL_LoadCmd() - Successfully prepared file %s\n", Filename);
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
      if (MsgTbl->AttrErrCnt == 0) {
      
         if (MsgTbl->PktLoadIdx > 0) {
         
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
                        ** the packet array and update each packet.
                        */                     
                        if (obj == MSGTBL_OBJ_PKT) {
                       
                           for (msg=0; msg < MsgTbl->PktLoadIdx; msg++) {
                          
                              if (!(MsgTbl->LoadTblEntryFunc)(msg, (MSGTBL_Entry*)&(MsgTbl->JsonObj[obj].Data)))
                                 MsgTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
                          
                           } /* End message array loop */                
                           
                        } /* End if MSGTBL_OBJ_PKT */ 
                     } /* End if object modified */
                  } /* End JSON object loop */
               } /* End if at least one object */
               else {
                  
                  CFE_EVS_SendEvent(MSGTBL_CMD_LOAD_EMPTY_ERR_EID,CFE_EVS_ERROR,"MSGTBL: Invalid table command. No messages defined.");

               } /* End if no objects in file */
               
            } /* End if update records */
			   else {
               
               CFE_EVS_SendEvent(MSGTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"MSGTBL: Invalid table command load type %d",LoadType);            
            
            } /* End if invalid command option */ 
            
         } /* End if valid message index */
         else {
            
            CFE_EVS_SendEvent(MSGTBL_CMD_LOAD_UPDATE_ERR_EID,CFE_EVS_ERROR,
			                     "MSGTBL: Update table command rejected. File contained %d message objects which exceeds the max table size of %d",
							         MsgTbl->ObjLoadCnt, MSGTBL_MAX_PKT_CNT);
         
         } /* End if too many messages in table file */

      } /* End if no attribute errors */
            
   } /* End if valid file */
   else {
      
      //printf("**ERROR** Processing Message Table file %s. Status = %d JSMN Status = %d\n",TEST_FILE, Json.FileStatus, Json.JsmnStatus);
      CFE_EVS_SendEvent(MSGTBL_CMD_LOAD_OPEN_ERR_EID,CFE_EVS_ERROR,"MSGTBL: Table open failure for file %s. File Status = %s JSMN Status = %s",
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
**     the app framework table manager that has verified the file.
**  3. DumpType is unused.
**  4. File is formatted so it can be used as a load file. It does not follow
**     the cFE table file format. 
**  5. Creates a new dump file, overwriting anything that may have existed
**     previously
*/

boolean MSGTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   int32    FileHandle,i, d;
   char     DumpRecord[256], DataByteStr[10];
   const    MSGTBL_Tbl *MsgTblPtr;
   char     SysTimeStr[256];
   uint8    DataBytes;
   
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS) {

      MsgTblPtr = (MsgTbl->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"Kit Scheduler (KIT_SCH) Message Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      
      sprintf(DumpRecord,"\"description\": \"KIT_SCH dump at %s\",\n",SysTimeStr);
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
      **   "data-bytes": "0,1,2,3,4,5"
      */
      
      sprintf(DumpRecord,"\"message-array\": [\n");

      for (i=0; i < MSGTBL_MAX_ENTRY_ID; i++) {
      
         sprintf(DumpRecord,"\"message\": {\n");
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
         sprintf(DumpRecord,"   \"id\": %d,\n   \"stream-id\": %d,\n   \"seq-seg\": %d,\n   \"length\": %d,\n",
                 i,
                 MsgTblPtr->Entry[i].Buffer[0],
                 MsgTblPtr->Entry[i].Buffer[1],
                 MsgTblPtr->Entry[i].Buffer[2]);
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
         
         DataBytes = ((MsgTblPtr->Entry[i].Buffer[2] & 0xFF00) >> 8) - 1;  /* Subtract 1 for CCSDS length definition */

         if (DataBytes > (MSGTBL_MAX_MSG_BYTES-6)) {
            
            CFE_EVS_SendEvent(MSGTBL_DUMP_MSG_ERR_EID, CFE_EVS_ERROR,
                              "Error creating dump file message entry %d. Data byte length %d is greater than max data buffer %d",
                              i, DataBytes, (MSGTBL_MAX_MSG_BYTES-6));         
         }
         else {

            if (DataBytes == 0) {
            
               sprintf(DumpRecord,"   \"data-bytes\": \"%d\"\n   },\n",0);         
   
            }
            else {
            
            }

            OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

         } /* End if DataBytes within range */

         else {
            if (DataBytes == 1) {
               sprintf(DumpRecord,"   \"data-bytes\": \"%d\"\n   },\n",MsgTblPtr->Entry[i].Buffer[2]);         
            }
            sprintf(DumpRecord,"   \"data-bytes\": \"%d\"\n   },\n","0");
            sprintf(DumpRecord,"   \"id\": %d,\n   \"stream-id\": %d,\n   \"seq-seg\": %d\n   \"length\": %d\n   \"data-bytes\": %s\n},\n",
         if (DataBytes > 1 && DataBytes < 10) {
         
         } 
         else {
         }
      MSGTBL_MAX_MSG_WORDS
      
      } /* End packet loop */

      sprintf(DumpRecord,"]\n}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(MSGTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of MSGTBL_DumpCmd() */


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

   int    Entry;

   OS_printf("MSGTBL_LoadCmd() Entry\n");

   /*
   ** Set all data & flags to zero then set all StreamIds to UNUSED.
   ** AttrErrCnt - Incremented by XML parser
   */
   
   CFE_PSP_MemSet(&(MsgTbl->Tbl), 0, sizeof(MSGTBL_Struct));
   MSGTBL_ResetStatus();

   if (ParseXmlFile(Filename, MsgTblStartElement, MsgTblEndElement) &&
       MsgTbl->AttrErrCnt == 0)
   {

      if (LoadType == TBLMGR_LOAD_TBL_REPLACE)
      {
         MsgTbl->LastLoadStatus = (MsgTbl->LoadTblFunc)(&(MsgTbl->Tbl)) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;
         
      } /* End if replace entire table */
      else if (LoadType == TBLMGR_LOAD_TBL_UPDATE)
      {
         MsgTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
         for (Entry=0; Entry < MSGTBL_MAX_ENTRY_ID; Entry++)
         {
            if (MsgTbl->Modified[Entry])
            {
               if (!(MsgTbl->LoadTblEntryFunc)(Entry, &(MsgTbl->Tbl.Entry[Entry])))
                  MsgTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
            }

         } /* End Entry loop */

      } /* End if update individual records */
      else
      {
         CFE_EVS_SendEvent(MSGTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"MSGTBL: Invalid message table command load type %d",LoadType);
      }

   } /* End if successful parse */
   else
   {
      CFE_EVS_SendEvent(MSGTBL_CMD_LOAD_PARSE_ERR_EID,CFE_EVS_ERROR,"MSGTBL: Table Parsing failure for file %s",Filename);
   }
   
   return (MsgTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of MSGTBL_LoadCmd() */


/******************************************************************************
** Function: MSGTBL_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
**  3. Command supplied filename is used for the cFE dump file. A second
**     file is created with an XML extension for an XML dump file.
*/
boolean MSGTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   char*   DotChrPtr;
   boolean RetStatus = FALSE;
   char   XmlFilename[OS_MAX_PATH_LEN];

   
   if (DumpTableToCfeFile(Filename,"KIT_SCH Packet Table",WriteMsgTblToCfeFile))
   {
      strcpy(XmlFilename, Filename);
      DotChrPtr = strrchr(Filename, '.');
      if (DotChrPtr != NULL)
      { 
         strcpy(++DotChrPtr,"xml");
         OS_printf("MSGTBL_DumpCmd: New XML filename = %s",XmlFilename);
     
         RetStatus = DumpTableToXmlFile(XmlFilename,"KIT_TO's Packet Table",WriteMsgTblToXmlFile);

         if (RetStatus) {
	          CFE_EVS_SendEvent(MSGTBL_CMD_DUMP_INFO_EID,CFE_EVS_INFORMATION,"{MSGTBL: Successfully created %s and %s dump files",Filename,XmlFilename);
	       }
         else
         {
            CFE_EVS_SendEvent(999,CFE_EVS_ERROR,"MSGTBL: Failed to create dump file %s",XmlFilename);
         }
      } /* End if DotChrPtr != NULL */
      else
      {
         CFE_EVS_SendEvent(999,CFE_EVS_ERROR,"MSGTBL: Failed to append (missing .) xml file extension to %s",XmlFilename);
      }
   }
   else
   {
	    CFE_EVS_SendEvent(999,CFE_EVS_ERROR,"MSGTBL: Failed to create dump file %s",Filename);
   }

   return RetStatus;

} /* End of MSGTBL_DumpCmd() */


/******************************************************************************
** Function: MsgTblStartElement
**
** Callback function for the XML parser. It assumes global variables have been
** properly setup before the parsing begins.
**
*/
static void XMLCALL MsgTblStartElement(void *data, const char *el, const char **attr)
{
  int i;
  uint16  ProcessedAttr = 0x0F;  /* 4 Attribute bits */
  uint16  Id =0;
  uint16  StreamId = 0;
  uint16  SeqSeg = 0;
  uint16  MsgLen = 0;

  if (strcmp(el,TBL_XML_EL_MSG_ENTRY) == 0)
  {
     /* printf ("MsgTblStartElement() - Entry: "); */
     for (i = 0; attr[i]; i += 2) {
        if (strcmp(attr[i],TBL_XML_AT_ID)==0) {
           /* printf("%s, ", attr[i + 1]); */
           Id = atoi(attr[i + 1]);
           ProcessedAttr &= 0xFE;
        } else if (strcmp(attr[i],TBL_XML_AT_STREAM_ID)==0)
        {
           StreamId = atoi(attr[i + 1]);
           ProcessedAttr &= 0xFD;
        } else if (strcmp(attr[i],TBL_XML_AT_SEQ_SEG)==0)
        {
           SeqSeg = atoi(attr[i + 1]);
           ProcessedAttr &= 0xFB;
        } else if (strcmp(attr[i],TBL_XML_AT_MSG_LEN)==0)
        {
           MsgLen = atoi(attr[i + 1]);
           ProcessedAttr &= 0xF7;
        }
     } /* End attribute loop */
     /* printf("\n"); */

     if (ProcessedAttr == 0)
     {
        /* TODO - Add error checking */
        MsgTbl->Tbl.Entry[Id].Buffer[0] = StreamId;
        MsgTbl->Tbl.Entry[Id].Buffer[1] = SeqSeg;
        MsgTbl->Tbl.Entry[Id].Buffer[2] = MsgLen;
        MsgTbl->Modified[Id] = TRUE;
     }
     else
     {
        MsgTbl->AttrErrCnt++;
     }

     /* CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "MsgTbl File Entry: Id=%d,StreamId=%d,SeqSeg=%d,MsgLen=%d",Id,StreamId,SeqSeg,MsgLen); */

  } /* End if MSGTBL_XML_EL_ENTRY found */

} /* End MsgTblStartElement() */


/*##############################################################################
** Function: MsgTblEndElement
**
** Nothing to do.
**
*/
static void XMLCALL MsgTblEndElement(void *data, const char *el)
{

} /* End MsgTblEndElement() */


/************************************/
/***  START CUSTOM DUMP FUNCTIONS ***/
/************************************/

/*##############################################################################
** WriteMsgTblToCfeFile
**
** Write the packet table contents to a file as text. The function signature
** must correspond to the DumpTableFuncPtr type.
**
*/
static void WriteMsgTblToCfeFile(int32 FileHandle)
{
   const  MSGTBL_Struct  *MsgTblPtr;
   char   DumpRecord[256];
   int    i;

   sprintf(DumpRecord,"\nEntry: Stream ID, Sequence-Segment, Length\n");
   OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

   MsgTblPtr = (MsgTbl->GetTblPtrFunc)();

   for (i=0; i < MSGTBL_MAX_ENTRY_ID; i++)
   {
      sprintf(DumpRecord,"%03d: 0x%04X, 0x%04X, 0x%04X\n",
              i, MsgTblPtr->Entry[i].Buffer[0], MsgTblPtr->Entry[i].Buffer[1], MsgTblPtr->Entry[i].Buffer[2]);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
   }

} /* End WriteMsgTblToCfeFile() */

/*##############################################################################
** WriteMsgTblToXmlFile
**
** Write the packet table contents to an XML file. The function signature
** must correspond to the DumpTableFuncPtr type.
**
*/
static void WriteMsgTblToXmlFile(int32 FileHandle)
{

   const MSGTBL_Struct *MsgTblPtr = (MsgTbl->GetTblPtrFunc)();
   int    i;
   int Level = 0;

   WriteStartElement(FileHandle, TBL_XML_EL_MSG_TBL, Level++);

   for (i=0; i < MSGTBL_MAX_ENTRY_ID; i++)
   {
      WriteMsgEntryElement(FileHandle,&MsgTblPtr->Entry[i],i,Level);
   }

   WriteEndElement(FileHandle, TBL_XML_EL_MSG_TBL,--Level);

} /* End WriteMsgTblToXmlFile() */


/*##############################################################################
** Function: WriteMsgEntryElement
**
** Write an "entry" element with the following format:
**  With Data:  <entry  id="1"  stream-id="2072" seq-seg="192" length="256"/>
**    No Data:  <entry  id="1"  stream-id="2072" seq-seg="192" length="1792">0,1,2,3,4,5</entry>
**
** The message table index serves as the message ID referenced by the scheduler table.
**
*/
static int32 WriteMsgEntryElement(int32 FileHandle,const MSGTBL_Entry *TblEntry, int Index, int Level)
{

   char ElementBuf[256];
   int  i = Level * TBL_XML_LEVEL_INDENT;

   CFE_PSP_MemSet(ElementBuf,' ',sizeof(ElementBuf));

   sprintf(&ElementBuf[i],"<%s  %s=\"%d\"  %s=\"%d\"  %s=\"%d\"  %s=\"%d\"/>\n",TBL_XML_EL_MSG_ENTRY,
		   TBL_XML_AT_ID,        Index,
		   TBL_XML_AT_STREAM_ID, TblEntry->Buffer[0],
		   TBL_XML_AT_SEQ_SEG,   TblEntry->Buffer[1],
		   TBL_XML_AT_MSG_LEN,   TblEntry->Buffer[2]);

   OS_write(FileHandle,ElementBuf,strlen(ElementBuf));

   return CFE_SUCCESS;

} /* End WriteMsgEntryElement() */

/**********************************************/
/***  DO NOT MODIFY CODE BELOW THIS COMMENT ***/
/**********************************************/


/******************************************************************************
**
** ParseXmlFile
**
** Parse an XML file
*/
static boolean ParseXmlFile(const char* FilePathName,
                            XML_StartElementHandler StartElementHandler,
                            XML_EndElementHandler   EndElementHandler)
{

   int      FileHandle;
   int32    ReadStatus;
   boolean  Done;
   boolean  ParseErr  = FALSE;
   boolean  RetStatus = FALSE;

   XML_Parser XmlParser = XML_ParserCreate(NULL);
   if (! XmlParser) {
      CFE_EVS_SendEvent(MSGTBL_CREATE_PARSER_ERR_EID, CFE_EVS_ERROR, "Failed to allocate memory for XML parser");
   }
   else {

      XML_SetElementHandler(XmlParser, StartElementHandler, EndElementHandler);

      FileHandle = OS_open(FilePathName, OS_READ_ONLY, 0);
      
      if (FileHandle >= 0) {

         Done = FALSE;

         while (!Done) {

            ReadStatus = OS_read(FileHandle, TblFileBuff, TBL_BUFFSIZE);

            if ( ReadStatus == OS_FS_ERROR )
            {
               CFE_EVS_SendEvent(MSGTBL_FILE_READ_ERR_EID, CFE_EVS_ERROR, "File read error, EC = 0x%08X",ReadStatus);
               Done = TRUE;
               ParseErr = TRUE;
            }
            else if ( ReadStatus == 0 ) /* EOF reached */
            {
                Done = TRUE;
            }
            else {

               /* ReadStatus contains number of bytes read */
               if (XML_Parse(XmlParser, TblFileBuff, ReadStatus, Done) == XML_STATUS_ERROR) {

                  CFE_EVS_SendEvent(MSGTBL_PARSE_ERR_EID, CFE_EVS_ERROR, "Parse error at line %lu, error code = %ls",
                                    XML_GetCurrentLineNumber(XmlParser),
                                    XML_ErrorString(XML_GetErrorCode(XmlParser)));
                  Done = TRUE;
                  ParseErr = TRUE;

               } /* End if valid parse */
            } /* End if valid fread */
         } /* End file read loop */

         RetStatus = !ParseErr;
         
         OS_close(FileHandle);

      } /* End if file opened */
      else
      {
      
          CFE_EVS_SendEvent(MSGTBL_FILE_OPEN_ERR_EID, CFE_EVS_ERROR, "File open error for %s, Error = %d",
                            FilePathName, FileHandle );
      }

      XML_ParserFree(XmlParser);

   } /* end if parser allocated */

   return RetStatus;

} /* End ParseXmlFile() */


/******************************************************************************
** Function: DumpTableToCfeFile
**
*/
static boolean DumpTableToCfeFile(const char* Filename, const char* FileDescr, DumpTableFuncPtr DumpTableFunc)
{

   CFE_FS_Header_t  CfeStdFileHeader;
   int32            FileHandle;
   int32            FileStatus;
   boolean          RetStatus = FALSE;

   /* Create a new dump file, overwriting anything that may have existed previously */
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS)
   {
      /* Initialize the standard cFE File Header for the Dump File */
      CfeStdFileHeader.SubType = 0x74786574;
      strcpy(&CfeStdFileHeader.Description[0], FileDescr);

      /* Output the Standard cFE File Header to the Dump File */
      FileStatus = CFE_FS_WriteHeader(FileHandle, &CfeStdFileHeader);

      if (FileStatus == sizeof(CFE_FS_Header_t))
      {
         (DumpTableFunc)(FileHandle);
         RetStatus = TRUE;

      } /* End if successfully wrote file header */
      else
      {
          CFE_EVS_SendEvent(MSGTBL_WRITE_CFE_HDR_ERR_EID,
                            CFE_EVS_ERROR,
                            "Error writing cFE File Header to '%s', Status=0x%08X",
                            Filename, FileStatus);

      }
      
      OS_close(FileHandle);
      
   } /* End if file create */
   else
   {

        CFE_EVS_SendEvent(MSGTBL_CREATE_MSG_DUMP_ERR_EID,
                          CFE_EVS_ERROR,
                          "Error creating cFE table dump file '%s', Status=0x%08X",
                          Filename, FileHandle);

    }


   return RetStatus;

} /* End of DumpTableToCfeFile() */

/******************************************************************************
** Function: DumpTableToXmlFile
**
*/
static boolean DumpTableToXmlFile(const char* Filename, const char* FileDescr, DumpTableFuncPtr DumpTableFunc)
{

   int32     FileHandle;
   boolean   RetStatus = FALSE;

   /* Create a new dump file, overwriting anything that may have existed previously */
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS)
   {

      (DumpTableFunc)(FileHandle);
      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else
   {

      CFE_EVS_SendEvent(MSGTBL_CREATE_MSG_DUMP_ERR_EID,
                        CFE_EVS_ERROR,
                        "Error creating XML table dump file '%s', Status=0x%08X",
                        Filename, FileHandle);

    } /* End if open failed */

   return RetStatus;

} /* End of DumpTableToXmlFile() */

/******************************************************************************
** Function: WriteCommentElement
**
*/
static int32 WriteCommentElement(int32 FileHandle,char* ElementName, int Level)
{

   /* TDOD - Implement comment WriteCOmmentElement() */

	return CFE_SUCCESS;

} /* End WriteCommentElement() */

/******************************************************************************
** Function: WriteStartElement
**
*/
static int32 WriteStartElement(int32 FileHandle,char* ElementName, int Level)
{

   char ElementBuf[TBL_XML_MAX_KW_LEN+4];
   int  i = Level * TBL_XML_LEVEL_INDENT;

   CFE_PSP_MemSet(ElementBuf,' ',sizeof(ElementBuf));

   sprintf(&ElementBuf[i],"<%s>\n",ElementName);
   OS_write(FileHandle,ElementBuf,strlen(ElementBuf));

   return CFE_SUCCESS;

} /* End WriteStartElement() */

/******************************************************************************
** Function: WriteEndElement
**
** Callback function for the XML parser. It assumes global variables have been
** properly set up before the parsing begins.
**
*/
static int32 WriteEndElement(int32 FileHandle,char* ElementName, int Level)
{

   char ElementBuf[TBL_XML_MAX_KW_LEN+4];
   int  i = Level * TBL_XML_LEVEL_INDENT;

   CFE_PSP_MemSet(ElementBuf,' ',sizeof(ElementBuf));

   sprintf(&ElementBuf[i],"</%s>\n",ElementName);
   OS_write(FileHandle,ElementBuf,strlen(ElementBuf));

   return CFE_SUCCESS;

} /* End WriteEndElement() */

/* end of file */
