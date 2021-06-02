/*
** Purpose: Implement example XML table.
**
** Notes:
**   1. Functions with comment blocks starting with pound signs (#) have
**      dependencies on the XML Element/Attribute definitions
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
#include "xmltbl.h"

/*
** Global File Data
*/

static XMLTBL_Class* XmlTbl = NULL;

static int  TblEntryId;
static char TblFileBuff[XMLTBL_BUFFSIZE];  /* XML table file buffer could be large so don't put on the stack */

/*
** Local File Function Prototypes
*/

typedef void (*DumpTblFuncPtr)(int32 FileHandle);   /* File-specific dump table function */

/******************************************************************************
** Function: DumpTblToFile
**
** General purpose function that takes a file pointer to the table-specific
** processing.
**
*/
static boolean DumpTblToFile(const char* Filename, DumpTblFuncPtr DumpTblFunc);


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

/*##############################################################################
** Functions: WriteXmlTblToFile, XmlTblStartElement, XmlTblEndElement
**
** These functions provide the example table-specific processing for XML parsing
** and for dump file formatting.
**
*/
static void WriteXmlTblToFile(int32 FileHandle);
static void XMLCALL XmlTblStartElement(void *data, const char *el, const char **attr);
static void XMLCALL XmlTblEndElement(void *data, const char *el);


/******************************************************************************
** Function: XMLTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void XMLTBL_Constructor(XMLTBL_Class* ObjPtr,
                        XMLTBL_GetTblPtr    GetTblPtrFunc,
                        XMLTBL_LoadTbl      LoadTblFunc, 
                        XMLTBL_LoadTblEntry LoadTblEntryFunc)
{

   XmlTbl = ObjPtr;

   CFE_PSP_MemSet(XmlTbl, 0, sizeof(XMLTBL_Class));

   XmlTbl->GetTblPtrFunc    = GetTblPtrFunc;
   XmlTbl->LoadTblFunc      = LoadTblFunc;
   XmlTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

} /* End XMLTBL_Constructor() */


/******************************************************************************
** Function: XMLTBL_ResetStatus
**
*/
void XMLTBL_ResetStatus(void)
{

   XmlTbl->LastLoadStatus = TBLMGR_STATUS_UNDEF;
   XmlTbl->AttrErrCnt = 0;

} /* End XMLTBL_ResetStatus() */


/******************************************************************************
** Function: XMLTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean XMLTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int    entry;

   CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, "XMLTBL_LoadCmd() Entry\n");

   /*
   ** Set all data and flags to zero. If a table replace is commanded and
   ** all of the data is not defined the zeroes will be copied into the table. 
   ** Real flight code would validate all data is loaded for a replace.
   **
   ** AttrErrCnt - Incremented by XML parser
   */
	
   CFE_PSP_MemSet(&(XmlTbl->Tbl), 0, sizeof(ExTblData_Param));
   XMLTBL_ResetStatus();

   TblEntryId = 0;
   if (ParseXmlFile(Filename, XmlTblStartElement, XmlTblEndElement) &&
       XmlTbl->AttrErrCnt == 0)
   {

      if (LoadType == TBLMGR_LOAD_TBL_REPLACE)
      {
         CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, "XMLTBL_LoadCmd() Before LoadTblFunc\n");
         XmlTbl->LastLoadStatus = ((XmlTbl->LoadTblFunc)(&(XmlTbl->Tbl)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;
         CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, "XMLTBL_LoadCmd() After LoadTblFunc\n");

      } /* End if replace entire table */
      else if (LoadType == TBLMGR_LOAD_TBL_UPDATE)
      {
         XmlTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
         for (entry=0; entry < XMLTBL_MAX_ENTRY_ID; entry++)
         {

            if (XmlTbl->Modified[entry])
            {
               if (!(XmlTbl->LoadTblEntryFunc)(entry, &(XmlTbl->Tbl.Entry[entry])))
                   XmlTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
            }

         } /* End pkt loop */

      } /* End if update individual records */
      else
      {
         CFE_EVS_SendEvent(XMLTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"XMLTBL: Invalid table command load type %d",LoadType);
      }

   } /* End if successful parse */
   else
   {
      CFE_EVS_SendEvent(XMLTBL_CMD_LOAD_PARSE_ERR_EID,CFE_EVS_ERROR,"XMLTBL: Table Parsing failure for file %s",Filename);
   }

   return (XmlTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of XMLTBL_LoadCmd() */


/******************************************************************************
** Function: XMLTBL_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean XMLTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   return DumpTblToFile(Filename,WriteXmlTblToFile);

} /* End of XMLTBL_DumpCmd() */


/*##############################################################################**
** WriteXmlTblToFile
**
** Dump the packet table contents to a file as text. The function signature
** must correspond to the DumpTblFuncPtr type.
**
*/
static void WriteXmlTblToFile(int32 FileHandle)
{
   const  ExTblData_Param  *XmlTblPtr;
   char   DumpRecord[256];
   int    i;

   sprintf(DumpRecord,"\nIndex: Data 1, Data 2, Data 3\n");
   OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

   XmlTblPtr = (XmlTbl->GetTblPtrFunc)();
   
   for (i=0; i < XMLTBL_MAX_ENTRY_ID; i++)
   {
      sprintf(DumpRecord,"%03d: %4d, %4d, %4d\n", i,
              XmlTblPtr->Entry[i].Data1, XmlTblPtr->Entry[i].Data2, XmlTblPtr->Entry[i].Data3);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
   }

} /* End WriteXmlTblToFile() */


/*##############################################################################
** Function: XmlTblStartElement
**
** Callback function for the XML parser. It assumes global variables have been
** properly set up before the parsing begins.
**
*/
static void XMLCALL XmlTblStartElement(void *data, const char *el, const char **attr)
{

   int     i;
   uint16  ProcessedAttr = 0x0F;  /* 4 Attribute bits */
   uint16  Index = 0;
   uint16  Data1 = 0;
   uint16  Data2 = 0;
   uint16  Data3 = 0;

   if (strcmp(el,XMLTBL_XML_EL_PKT_ENTRY) == 0)
   {
      for (i = 0; attr[i]; i += 2) {
         if (strcmp(attr[i],XMLTBL_XML_AT_INDEX)==0) {
            Index = atoi(attr[i + 1]);
            ProcessedAttr &= 0xFE;
         } else if (strcmp(attr[i],XMLTBL_XML_AT_DATA1)==0) {
            Data1 = atoi(attr[i + 1]);
            ProcessedAttr &= 0xFD;
         } else if (strcmp(attr[i],XMLTBL_XML_AT_DATA2)==0) {
            Data2 = atoi(attr[i + 1]);
            ProcessedAttr &= 0xFB;
         } else if (strcmp(attr[i],XMLTBL_XML_AT_DATA3)==0) {
            Data3 = atoi(attr[i + 1]);
            ProcessedAttr &= 0xF7;
         }
      } /* End attribute loop */

      if (ProcessedAttr == 0)
      {
         /* TODO - Add error checking */
         XmlTbl->Tbl.Entry[Index].Data1 = Data1;
         XmlTbl->Tbl.Entry[Index].Data2 = Data2;
         XmlTbl->Tbl.Entry[Index].Data3 = Data3;
         XmlTbl->Modified[Index] = TRUE;
      }
      else
      {
         XmlTbl->AttrErrCnt++;
      }

  } /* End if XMLTBL_XML_EL_ENTRY found */

} /* End XmlTblStartElement() */

/*##############################################################################
** Function: XmlTblEndElement
**
** Nothing to do.
**
*/
static void XMLCALL XmlTblEndElement(void *data, const char *el)
{

} /* End XmlTblEndElement() */


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
      CFE_EVS_SendEvent(XMLTBL_CREATE_ERR_EID, CFE_EVS_ERROR, "Failed to allocate memory for XML parser");
   }
   else {

      XML_SetElementHandler(XmlParser, StartElementHandler, EndElementHandler);

      FileHandle = OS_open(FilePathName, OS_READ_ONLY, 0);
      
      if (FileHandle >= 0) {

         Done = FALSE;

         while (!Done) {

            ReadStatus = OS_read(FileHandle, TblFileBuff, XMLTBL_BUFFSIZE);

            if ( ReadStatus == OS_FS_ERROR )
            {
               CFE_EVS_SendEvent(XMLTBL_FILE_READ_ERR_EID, CFE_EVS_ERROR, "File read error, EC = 0x%08X",ReadStatus);
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

                  CFE_EVS_SendEvent(XMLTBL_PARSE_ERR_EID, CFE_EVS_ERROR, "Parse error at line %lu, error code = %ls",
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
      
          CFE_EVS_SendEvent(XMLTBL_FILE_OPEN_ERR_EID, CFE_EVS_ERROR, "File open error for %s, Error = %d",
                            FilePathName, FileHandle );
      }

      XML_ParserFree(XmlParser);

   } /* end if parser allocated */

   return RetStatus;

} /* End ParseXmlFile() */

/******************************************************************************
** Function: DumpTblToFile
**
*/
static boolean DumpTblToFile(const char* Filename, DumpTblFuncPtr DumpTblFunc)
{

   int32            FileHandle;
   boolean          RetStatus = FALSE;

   /* Create a new dump file, overwriting anything that may have existed previously */
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS)
   {

      (DumpTblFunc)(FileHandle);
      RetStatus = TRUE;

   } /* End if file create */
   else
   {

        CFE_EVS_SendEvent(XMLTBL_CREATE_MSG_DUMP_ERR_EID,
                          CFE_EVS_ERROR,
                          "Error creating CDS dump file '%s', Status=0x%08X",
                          Filename, FileHandle);

    }

   OS_close(FileHandle);

   return RetStatus;

} /* End of DumpTblToFile() */
