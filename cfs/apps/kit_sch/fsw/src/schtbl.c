/* 
** Purpose: Implement the schedule table management functions.
**
** Notes:
**   1. Written by David McComas, licensed under the copyleft GNU
**      General Public License (GPL).
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Include Files:
*/

#include <string.h>
#include "schtbl.h"


/*
** File Function Prototypes
*/

static SCHTBL_Class* SchTbl = NULL;

static int  TblEntryId;
static char TblFileBuff[TBL_BUFFSIZE];  /* XML table file buffer could be large so don't put on the stack */

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
** Functions: SchTblStartElement, SchTblEndElement, WriteSchTblToCfeFile
**
** These functions provide the packet table specific processing for XML parsing
** and for dump file formatting.
**
*/
static void XMLCALL SchTblStartElement(void *data, const char *el, const char **attr);
static void XMLCALL SchTblEndElement(void *data, const char *el);
static void WriteSchTblToCfeFile(int32 FileHandle);

/*#############################################################################
** Functions: WriteSchTblToXmlFile, WriteEntryElement,
**
** These functions provide the packet table-specific processing for XML
** dump file formatting.
**
*/
static void WriteSchTblToXmlFile(int32 FileHandle);
static int32 WriteSchSlotElement(int32 FileHandle,const SCHTBL_Entry *TblEntry, int Slot, int Entry, int Level);


/*
** Exported Functions
*/

/******************************************************************************
** Function: SCHTBL_Constructor
**
** This method initializes a new scheduler table instance.
**
** Notes:
**   1. This method must be called prior to all other methods. The SchTbl
**      instance variable only needs to be passed to the constructor
**      because a reference is stored by schtbl.c.
**
*/
void SCHTBL_Constructor(SCHTBL_Class* ObjPtr,
                        SCHTBL_GetTblPtr    GetTblPtrFunc,
                        SCHTBL_LoadTbl      LoadTblFunc, 
                        SCHTBL_LoadTblEntry LoadTblEntryFunc)
{

   SchTbl = ObjPtr;

   /*
    * Set all data and flags to zero.
    * AttrErrCnt - Incremented by XML parser
    */
   CFE_PSP_MemSet(SchTbl, 0, sizeof(SCHTBL_Class));

   SchTbl->GetTblPtrFunc    = GetTblPtrFunc;
   SchTbl->LoadTblFunc      = LoadTblFunc;
   SchTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

 
} /* End SCHTBL_Constructor    () */


/*******************************************************************
** Function: SCHTBL_ResetStatus
**
*/
void SCHTBL_ResetStatus()
{

   int entry;

   SchTbl->LastLoadStatus = TBLMGR_STATUS_UNDEF;
   SchTbl->AttrErrCnt = 0;

   for (entry=0; entry < SCHTBL_MAX_ENTRIES; entry++)
      SchTbl->Modified[entry] = FALSE;

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

   uint16 Entry;

   OS_printf("SCHTBL_LoadCmd() Entry\n");

   /*
   ** Set all data & flags to zero then set all StreamIds to UNUSED.
   ** AttrErrCnt - Incremented by XML parser
   */
   CFE_PSP_MemSet(&(SchTbl->Tbl), 0, sizeof(SCHTBL_Struct));
   SCHTBL_ResetStatus();

   TblEntryId = 0;
   if (ParseXmlFile(Filename, SchTblStartElement, SchTblEndElement) &&
       SchTbl->AttrErrCnt == 0)
   {

      if (LoadType == TBLMGR_LOAD_TBL_REPLACE)
      {
         SchTbl->LastLoadStatus = (SchTbl->LoadTblFunc)(&(SchTbl->Tbl)) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

      } /* End if replace entire table */
      else if (LoadType == TBLMGR_LOAD_TBL_UPDATE)
      {
         SchTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
         for (Entry=0; Entry < SCHTBL_MAX_ENTRIES; Entry++)
         {

            if (SchTbl->Modified[Entry])
            {
               if (!(SchTbl->LoadTblEntryFunc)(Entry, &(SchTbl->Tbl.Entry[Entry])))
                  SchTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
            }

         } /* End entry loop */

      } /* End if update individual records */
      else
      {
         CFE_EVS_SendEvent(SCHTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"SCHTBL: Invalid packet table command load type %d",LoadType);
      }

   } /* End if successful parse */
   else
   {
      CFE_EVS_SendEvent(SCHTBL_CMD_LOAD_PARSE_ERR_EID,CFE_EVS_ERROR,"SCHTBL: Table Parsing failure for file %s",Filename);
   }

   return (SchTbl->LastLoadStatus = TBLMGR_STATUS_VALID);

} /* End of SCHTBL_LoadCmd() */

/******************************************************************************
** Function: SCHTBL_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
**  3. Command supplied filename is used for the cFE dump file. A second
**     file is created with an XML extension for an XML dump file.
*/
boolean SCHTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   char*   DotChrPtr;
   boolean RetStatus = FALSE;
   char   XmlFilename[OS_MAX_PATH_LEN];

   
   if (DumpTableToCfeFile(Filename,"KIT_TO Packet Table",WriteSchTblToCfeFile))
   {
      strcpy(XmlFilename, Filename);
      DotChrPtr = strrchr(Filename, '.');
      if (DotChrPtr != NULL)
      { 
         strcpy(++DotChrPtr,"xml");
         OS_printf("SCHTBL_DumpCmd: New XML filename = %s",XmlFilename);
     
	       RetStatus = DumpTableToXmlFile(XmlFilename,"KIT_TO's Packet Table",WriteSchTblToXmlFile);

	       if (RetStatus)
	       {
            CFE_EVS_SendEvent(SCHTBL_CMD_DUMP_INFO_EID,CFE_EVS_INFORMATION,"{SCHTBL: Successfully created %s and %s dump files",Filename,XmlFilename);
         }
         else
         {
            CFE_EVS_SendEvent(999,CFE_EVS_ERROR,"SCHTBL: Failed to create dump file %s",XmlFilename);
         }
      } /* End if DotChrPtr != NULL */
      else
      {
         CFE_EVS_SendEvent(999,CFE_EVS_ERROR,"SCHTBL: Failed to append (missing .) xml file extension to %s",XmlFilename);
      }
   }
   else
   {
	    CFE_EVS_SendEvent(999,CFE_EVS_ERROR,"SCHTBL: Failed to create dump file %s",Filename);
   }

   return RetStatus;

} /* End of SCHTBL_DumpCmd() */

/*##############################################################################
** Function: SchTblStartElement
**
** Callback function for the XML parser. It assumes global variables have been
** properly setup before the parsing begins.
**
*/
static void XMLCALL SchTblStartElement(void *data, const char *el, const char **attr)
{
  int     i;
  uint16  ProcessedAttr = 0x3F;  /* 6 Attribute bits */
  uint16  SlotId = 0;
  uint16  Entry = 0;
  boolean Enabled = FALSE;
  uint16  Frequency = 0;
  uint16  Offset = 0;
  uint16  MsgTblEntryId = 0;

  if (strcmp(el,TBL_XML_EL_SCH_SLOT) == 0)
  {

     for (i = 0; attr[i]; i += 2) {
        if (strcmp(attr[i],TBL_XML_AT_ID)==0) {
           SlotId = atoi(attr[i + 1]);
           ProcessedAttr &= 0xFE;
        } else if (strcmp(attr[i],TBL_XML_AT_ENTRY)==0)
        {
           Entry = atoi(attr[i + 1]);
           ProcessedAttr &= 0xFD;
        } else if (strcmp(attr[i],TBL_XML_AT_ENABLE)==0)
        {
           Enabled = (strcmp(attr[i+1],TBL_XML_AT_VAL_TRUE)==0);
           ProcessedAttr &= 0xFB;
        } else if (strcmp(attr[i],TBL_XML_AT_FREQ)==0)
        {
           Frequency = atoi(attr[i + 1]);
           ProcessedAttr &= 0xF7;
        } else if (strcmp(attr[i],TBL_XML_AT_OFFSET)==0)
        {
           Offset = atoi(attr[i + 1]);
           ProcessedAttr &= 0xEF;
        } else if (strcmp(attr[i],TBL_XML_AT_MSG_ID)==0)
        {
           MsgTblEntryId = atoi(attr[i + 1]);
           ProcessedAttr &= 0xDF;
        }

     } /* End attribute loop */

     if (ProcessedAttr == 0)
     {
        /* TODO - Add error checking */
        i = SCHTBL_INDEX(SlotId,Entry);
        SchTbl->Tbl.Entry[i].Enabled       = Enabled;
        SchTbl->Tbl.Entry[i].Frequency     = Frequency;
        SchTbl->Tbl.Entry[i].Offset        = Offset;
        SchTbl->Tbl.Entry[i].MsgTblEntryId = MsgTblEntryId;
        SchTbl->Modified[i] = TRUE;
     }
     else
     {
        SchTbl->AttrErrCnt++;
     }

     /* CFE_EVS_SendEvent(999, CFE_EVS_INFORMATION, "SchTbl File Slot: Id=%d,Entry=%d,Ena=%d,Freq=%d,Offset=%d,MsgId=%d", SlotId,Entry,Enabled,Frequency,Offset,MsgTblEntryId); */

     /*
     printf("SchTbl File Slot: Id=%d,Entry=%d,Ena=%d,Freq=%d,Offset=%d,MsgId=%d\n",
                       SlotId,Entry,Enabled,Frequency,Offset,MsgTblEntryId);
     */

  } /* End if Slot Element found */

} /* End SchTblStartElement() */

/*##############################################################################
** Function: SchTblEndElement
**
** Nothing to do.
**
*/static void XMLCALL SchTblEndElement(void *data, const char *el)
{

} /* End SchTblEndElement() */


/************************************/
/***  START CUSTOM DUMP FUNCTIONS ***/
/************************************/

/******************************************************************************
** Function: WriteSchTblToCfeFile
**
** Dump the message table contents to a file as text. The function signature
** must correspond to the DumpTableFuncPtr type.
**
*/
static void WriteSchTblToCfeFile(int32 FileHandle)
{
   const  SCHTBL_Struct  *SchTblPtr;
   char   DumpRecord[256];
   int    i,slot,entry;

   sprintf(DumpRecord,"\n(Slot,Entry): Enabled, Frequency, Offset, MsgTblEntryId\n");
   OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

   SchTblPtr = (SchTbl->GetTblPtrFunc)();

   for (slot=0; slot < SCHTBL_TOTAL_SLOTS; slot++)
   {
      for (entry=0; entry < SCHTBL_ENTRIES_PER_SLOT; entry++)
      {

         i = SCHTBL_INDEX(slot,entry);
         sprintf(DumpRecord,"(%03d,%02d): %d, %d, %d, %d\n", slot, entry,
                 SchTblPtr->Entry[i].Enabled, SchTblPtr->Entry[i].Frequency,
                 SchTblPtr->Entry[i].Offset,  SchTblPtr->Entry[i].MsgTblEntryId);
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      } /* End entry loop */
   } /* End slot loop */

} /* End WriteSchTblToCfeFile() */

/*##############################################################################
** Function: WriteSchTblToXmlFile
**
** Write the scheduler table contents to an XML file. The function signature
** must correspond to the DumpTableFuncPtr type.
**
*/
static void WriteSchTblToXmlFile(int32 FileHandle)
{
   const SCHTBL_Struct *SchTblPtr = (SchTbl->GetTblPtrFunc)();
   int   Slot, Entry, i;
   int   Level = 0;

   WriteStartElement(FileHandle,TBL_XML_EL_SCH_TBL, Level++);

   for (Slot=0; Slot < SCHTBL_TOTAL_SLOTS; Slot++)
   {
      for (Entry=0; Entry < SCHTBL_ENTRIES_PER_SLOT; Entry++)
      {
    	  i = (Slot * SCHTBL_ENTRIES_PER_SLOT) + Entry;
          WriteSchSlotElement(FileHandle,&SchTblPtr->Entry[i],Slot, Entry, Level);
      }
   }

   WriteEndElement(FileHandle,TBL_XML_EL_SCH_TBL,--Level);

} /* End WriteSchTblToXmlFile() */


/*##############################################################################
** Function: WriteSchSlotElement
**
** Write an "slot" element with the following format:
**    <slot id="0" entry="0" enable="true" frequency="1" offset="0" msg_id="8" />
**
** The message table index serves as the message ID referenced by the scheduler table.
*/
static int32 WriteSchSlotElement(int32 FileHandle,const SCHTBL_Entry *TblEntry, int Slot, int Entry, int Level)
{

   static const char *BoolStr[] = {"false","true"};
   char ElementBuf[256];
   int  i = Level * TBL_XML_LEVEL_INDENT;
   int  BoolIndex = 0;

   CFE_PSP_MemSet(ElementBuf,' ',sizeof(ElementBuf));

   if (TblEntry->Enabled) BoolIndex = 1;

   sprintf(&ElementBuf[i],"<%s  %s=\"%d\"  %s=\"%d\"  %s=\"%s\"  %s=\"%d\"  %s=\"%d\"  %s=\"%d\"/>\n",
		   TBL_XML_EL_SCH_SLOT,
		   TBL_XML_AT_ID,     Slot,
		   TBL_XML_AT_ENTRY,  Entry,
		   TBL_XML_AT_ENABLE, BoolStr[BoolIndex],
		   TBL_XML_AT_FREQ,   TblEntry->Frequency,
		   TBL_XML_AT_OFFSET, TblEntry->Offset,
		   TBL_XML_AT_MSG_ID, TblEntry->MsgTblEntryId);

   OS_write(FileHandle,ElementBuf,strlen(ElementBuf));

   return CFE_SUCCESS;

} /* End WriteSchSlotElement() */

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
      CFE_EVS_SendEvent(SCHTBL_CREATE_PARSER_ERR_EID, CFE_EVS_ERROR, "Failed to allocate memory for XML parser");
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
               CFE_EVS_SendEvent(SCHTBL_FILE_READ_ERR_EID, CFE_EVS_ERROR, "File read error, EC = 0x%08X",ReadStatus);
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

                  CFE_EVS_SendEvent(SCHTBL_PARSE_ERR_EID, CFE_EVS_ERROR, "Parse error at line %lu, error code = %ls",
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
      
          CFE_EVS_SendEvent(SCHTBL_FILE_OPEN_ERR_EID, CFE_EVS_ERROR, "File open error for %s, Error = %d",
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
          CFE_EVS_SendEvent(SCHTBL_WRITE_CFE_HDR_ERR_EID,
                            CFE_EVS_ERROR,
                            "Error writing cFE File Header to '%s', Status=0x%08X",
                            Filename, FileStatus);

      }
      
      OS_close(FileHandle);
      
   } /* End if file create */
   else
   {

        CFE_EVS_SendEvent(SCHTBL_CREATE_DUMP_FILE_ERR_EID,
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

      CFE_EVS_SendEvent(SCHTBL_CREATE_DUMP_FILE_ERR_EID,
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
