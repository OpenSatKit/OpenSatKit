/* 
** Purpose: Implement the packet table management functions.
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
#include "pkttbl.h"


/*
** Global File Data
*/

static PKTTBL_Class* PktTbl = NULL;

static int  TblEntryId;
static char TblFileBuff[PKTTBL_BUFFSIZE];  /* XML table file buffer could be large so don't put on the stack */

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
** Functions: PktTblStartElement, PktTblEndElement, WritePktTblToCfeFile
**
** These functions provide the packet table specific processing for XML parsing
** and for dump file formatting.
**
*/
static void XMLCALL PktTblStartElement(void *data, const char *el, const char **attr);
static void XMLCALL PktTblEndElement(void *data, const char *el);
static void WritePktTblToCfeFile(int32 FileHandle);

/*#############################################################################
** Functions: WritePktTblToXmlFile, WriteEntryElement,
**
** These functions provide the packet table-specific processing for XML
** dump file formatting.
**
*/
static void WritePktTblToXmlFile(int32 FileHandle);
static int32 WriteEntryElement(int32 FileHandle,const PKTTBL_Entry *TblEntry, int Level);



/*##############################################################################
** Function: PKTTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void PKTTBL_Constructor(PKTTBL_Class* ObjPtr,
                        PKTTBL_GetTblPtr    GetTblPtrFunc,
                        PKTTBL_LoadTbl      LoadTblFunc, 
                        PKTTBL_LoadTblEntry LoadTblEntryFunc)
{

   uint16 entry;
   PktTbl = ObjPtr;

   /*
    * Set all data and flags to zero.
    * AttrErrCnt - Incremented by XML parser
    */
   CFE_PSP_MemSet(PktTbl, 0, sizeof(PKTTBL_Class));
   for (entry=0; entry < PKTTBL_MAX_ENTRY_ID; entry++)
      PktTbl->Tbl.Entry[entry].StreamId = PKTTBL_UNUSED_MSG_ID;

   PktTbl->GetTblPtrFunc    = GetTblPtrFunc;
   PktTbl->LoadTblFunc      = LoadTblFunc;
   PktTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

 
} /* End PKTTBL_Constructor() */


/*##############################################################################
** Function: PKTTBL_ResetStatus
**
*/
void PKTTBL_ResetStatus()
{

   int entry;

   PktTbl->LastLoadStatus = TBLMGR_STATUS_UNDEF;
   PktTbl->AttrErrCnt = 0;

   for (entry=0; entry < PKTTBL_MAX_ENTRY_ID; entry++)
      PktTbl->Modified[entry] = FALSE;

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

   int    Entry;

   OS_printf("PKTTBL_LoadCmd() Entry\n");


   /*
   ** Set all data & flags to zero then set all StreamIds to UNUSED.
   ** AttrErrCnt - Incremented by XML parser
   */
   CFE_PSP_MemSet(&(PktTbl->Tbl), 0, sizeof(PKTTBL_Struct));
   PKTTBL_ResetStatus();

   for (Entry=0; Entry < PKTTBL_MAX_ENTRY_ID; Entry++)
      PktTbl->Tbl.Entry[Entry].StreamId = PKTTBL_UNUSED_MSG_ID;

   OS_printf("PKTTBL_LoadCmd() Past MemSet\n");
   TblEntryId = 0;
   if (ParseXmlFile(Filename, PktTblStartElement, PktTblEndElement) &&
       PktTbl->AttrErrCnt == 0)
   {

      if (LoadType == TBLMGR_LOAD_TBL_REPLACE)
      {
         OS_printf("PKTTBL_LoadCmd() Before LoadTblFunc\n");
         PktTbl->LastLoadStatus = (PktTbl->LoadTblFunc)(&(PktTbl->Tbl)) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;
         OS_printf("PKTTBL_LoadCmd() After LoadTblFunc\n");

      } /* End if replace entire table */
      else if (LoadType == TBLMGR_LOAD_TBL_UPDATE)
      {
         PktTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
         for (Entry=0; Entry < PKTTBL_MAX_ENTRY_ID; Entry++)
         {

            if (PktTbl->Modified[Entry])
            {
               if (!(PktTbl->LoadTblEntryFunc)(Entry, &(PktTbl->Tbl.Entry[Entry])))
                  PktTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
            }

         } /* End entry loop */

      } /* End if update individual records */
      else
      {
         CFE_EVS_SendEvent(PKTTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"PKTTBL: Invalid packet table command load type %d",LoadType);
      }

   } /* End if successful parse */
   else
   {
      CFE_EVS_SendEvent(PKTTBL_CMD_LOAD_PARSE_ERR_EID,CFE_EVS_ERROR,"PKTTBL: Table Parsing failure for file %s",Filename);
   }

   return (PktTbl->LastLoadStatus = TBLMGR_STATUS_VALID);

} /* End of PKTTBL_LoadCmd() */


/******************************************************************************
** Function: PKTTBL_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
**  3. Command supplied filename is used for the cFE dump file. A second
**     file is created with an XML extension for an XML dump file.
*/
boolean PKTTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   char*   DotChrPtr;
   boolean RetStatus = FALSE;
   char   XmlFilename[OS_MAX_PATH_LEN];

   
   if (DumpTableToCfeFile(Filename,"KIT_TO Packet Table",WritePktTblToCfeFile))
   {
      strcpy(XmlFilename, Filename);
      DotChrPtr = strrchr(Filename, '.');
      if (DotChrPtr != NULL)
      { 
         strcpy(++DotChrPtr,"xml");
         OS_printf("PKTTBL_DumpCmd: New XML filename = %s",XmlFilename);
     
	       RetStatus = DumpTableToXmlFile(XmlFilename,"KIT_TO's Packet Table",WritePktTblToXmlFile);

	       if (RetStatus)
	       {
            CFE_EVS_SendEvent(PKTTBL_CMD_DUMP_INFO_EID,CFE_EVS_INFORMATION,"{PKTTBL: Successfully created %s and %s dump files",Filename,XmlFilename);
         }
         else
         {
            CFE_EVS_SendEvent(999,CFE_EVS_ERROR,"PKTTBL: Failed to create dump file %s",XmlFilename);
         }
      } /* End if DotChrPtr != NULL */
      else
      {
         CFE_EVS_SendEvent(999,CFE_EVS_ERROR,"PKTTBL: Failed to append (missing .) xml file extension to %s",XmlFilename);
      }
   }
   else
   {
	    CFE_EVS_SendEvent(999,CFE_EVS_ERROR,"PKTTBL: Failed to create dump file %s",Filename);
   }

   return RetStatus;

} /* End of PKTTBL_DumpCmd() */

/*##############################################################################
**
** PktTblStartElement
**
** Callback function for the XML parser. It assumes global variables have been
** properly setup before the parsing begins.
**
*/
static void XMLCALL PktTblStartElement(void *data, const char *el, const char **attr)
{

   int     i;
   uint16  ProcessedAttr = 0x0F;  /* 4 Attribute bits */
   uint16  StreamId    = 0;
   uint16  Priority    = 0;
   uint16  Reliability = 0;
   uint16  BuffLim     = 0;

   if (strcmp(el,PKTTBL_XML_EL_PKT_ENTRY) == 0)
   {

      for (i = 0; attr[i]; i += 2) {
         if (strcmp(attr[i],PKTTBL_XML_AT_STREAM_ID)==0) {
            /* printf("%s\n", attr[i + 1]); */
            StreamId = atoi(attr[i + 1]);
            ProcessedAttr &= 0xFE;
         } else if (strcmp(attr[i],PKTTBL_XML_AT_PRIORITY)==0)
         {
            Priority = atoi(attr[i + 1]);
            ProcessedAttr &= 0xFD;
         } else if (strcmp(attr[i],PKTTBL_XML_AT_RELIABILITY)==0)
         {
            Reliability = atoi(attr[i + 1]);
            ProcessedAttr &= 0xFB;
         } else if (strcmp(attr[i],PKTTBL_XML_AT_BUF_LIM)==0)
         {
            BuffLim = atoi(attr[i + 1]);
            ProcessedAttr &= 0xF7;
         }
      } /* End attribute loop */

      if (ProcessedAttr == 0)
      {
        /* TODO - Add error checking */
        PktTbl->Tbl.Entry[TblEntryId].StreamId        = StreamId;
        PktTbl->Tbl.Entry[TblEntryId].Qos.Priority    = Priority;
        PktTbl->Tbl.Entry[TblEntryId].Qos.Reliability = Reliability;
        PktTbl->Tbl.Entry[TblEntryId].BuffLim         = BuffLim;
        PktTbl->Modified[TblEntryId] = TRUE;
        /* printf ("PktTbl->Tbl.Entry[%d].StreamId = 0x%04X\n", TblEntryId, PktTbl->Tbl.Entry[TblEntryId].StreamId); */
        TblEntryId++;
      }
      else
      {
         PktTbl->AttrErrCnt++;
      }


  } /* End if PKTTBL_XML_EL_ENTRY found */

} /* End PktTblStartElement() */

/*##############################################################################
** Function: PktTblEndElement
**
** Nothing to do.
**
*/
static void XMLCALL PktTblEndElement(void *data, const char *el)
{

} /* End PktTblEndElement() */


/************************************/
/***  START CUSTOM DUMP FUNCTIONS ***/
/************************************/

/*##############################################################################
** WritePktTblToCfeFile
**
** Write the packet table contents to a file as text. The function signature
** must correspond to the DumpTableFuncPtr type.
**
*/
static void WritePktTblToCfeFile(int32 FileHandle)
{
   const  PKTTBL_Struct  *PktTblPtr;
   char   DumpRecord[256];
   int    i;

   sprintf(DumpRecord,"\nEntry: Stream ID, Buffer Limit, QoS (Priority,Reliability)\n");
   OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

   PktTblPtr = (PktTbl->GetTblPtrFunc)();

   for (i=0; i < PKTTBL_MAX_ENTRY_ID; i++)
   {
      sprintf(DumpRecord,"%03d: 0x%04X, %03d, (%d,%d)\n",
              i, PktTblPtr->Entry[i].StreamId,PktTblPtr->Entry[i].BuffLim,
              PktTblPtr->Entry[i].Qos.Priority, PktTblPtr->Entry[i].Qos.Reliability);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
   }

} /* End WritePktTblToCfeFile() */

/*##############################################################################
** WritePktTblToXmlFile
**
** Write the packet table contents to an XML file. The function signature
** must correspond to the DumpTableFuncPtr type.
**
*/
static void WritePktTblToXmlFile(int32 FileHandle)
{
   const PKTTBL_Struct *PktTblPtr = (PktTbl->GetTblPtrFunc)();
   int    i;
   int Level = 0;

   WriteStartElement(FileHandle, PKTTBL_XML_EL_PKT_TBL, Level++);

   for (i=0; i < PKTTBL_MAX_ENTRY_ID; i++)
   {
      WriteEntryElement(FileHandle,&PktTblPtr->Entry[i],Level);
   }

   WriteEndElement(FileHandle, PKTTBL_XML_EL_PKT_TBL,--Level);

} /* End WritePktTblToXmlFile() */


/*##############################################################################
** Function: WriteEntryElement
**
** Write an "entry" element with the following format:
**    <entry  stream-id="2176" priority="0" reliability="0" buf-limit="4"/>
*/
static int32 WriteEntryElement(int32 FileHandle,const PKTTBL_Entry *TblEntry, int Level)
{

   char ElementBuf[256];
   int  i = Level * PKTTBL_XML_LEVEL_INDENT;

   CFE_PSP_MemSet(ElementBuf,' ',sizeof(ElementBuf));

   sprintf(&ElementBuf[i],"<%s  %s=\"%d\"  %s=\"%d\"  %s=\"%d\"  %s=\"%d\"/>\n",PKTTBL_XML_EL_PKT_ENTRY,
		   PKTTBL_XML_AT_STREAM_ID,   TblEntry->StreamId,
		   PKTTBL_XML_AT_PRIORITY,    TblEntry->Qos.Priority,
		   PKTTBL_XML_AT_RELIABILITY, TblEntry->Qos.Reliability,
		   PKTTBL_XML_AT_BUF_LIM,     TblEntry->BuffLim);

   OS_write(FileHandle,ElementBuf,strlen(ElementBuf));

   return CFE_SUCCESS;

} /* End WriteEntryElement() */



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
      CFE_EVS_SendEvent(PKTTBL_CREATE_PARSER_ERR_EID, CFE_EVS_ERROR, "Failed to allocate memory for XML parser");
   }
   else {

      XML_SetElementHandler(XmlParser, StartElementHandler, EndElementHandler);

      FileHandle = OS_open(FilePathName, OS_READ_ONLY, 0);
      
      if (FileHandle >= 0) {

         Done = FALSE;

         while (!Done) {

            ReadStatus = OS_read(FileHandle, TblFileBuff, PKTTBL_BUFFSIZE);

            if ( ReadStatus == OS_FS_ERROR )
            {
               CFE_EVS_SendEvent(PKTTBL_FILE_READ_ERR_EID, CFE_EVS_ERROR, "File read error, EC = 0x%08X",ReadStatus);
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

                  CFE_EVS_SendEvent(PKTTBL_PARSE_ERR_EID, CFE_EVS_ERROR, "Parse error at line %lu, error code = %ls",
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
      
          CFE_EVS_SendEvent(PKTTBL_FILE_OPEN_ERR_EID, CFE_EVS_ERROR, "File open error for %s, Error = %d",
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
          CFE_EVS_SendEvent(PKTTBL_WRITE_CFE_HDR_ERR_EID,
                            CFE_EVS_ERROR,
                            "Error writing cFE File Header to '%s', Status=0x%08X",
                            Filename, FileStatus);

      }
      
      OS_close(FileHandle);
      
   } /* End if file create */
   else
   {

        CFE_EVS_SendEvent(PKTTBL_CREATE_MSG_DUMP_ERR_EID,
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

      CFE_EVS_SendEvent(PKTTBL_CREATE_MSG_DUMP_ERR_EID,
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

   char ElementBuf[PKTTBL_XML_MAX_KW_LEN+4];
   int  i = Level * PKTTBL_XML_LEVEL_INDENT;

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

   char ElementBuf[PKTTBL_XML_MAX_KW_LEN+4];
   int  i = Level * PKTTBL_XML_LEVEL_INDENT;

   CFE_PSP_MemSet(ElementBuf,' ',sizeof(ElementBuf));

   sprintf(&ElementBuf[i],"</%s>\n",ElementName);
   OS_write(FileHandle,ElementBuf,strlen(ElementBuf));

   return CFE_SUCCESS;

} /* End WriteEndElement() */

/* end of file */
