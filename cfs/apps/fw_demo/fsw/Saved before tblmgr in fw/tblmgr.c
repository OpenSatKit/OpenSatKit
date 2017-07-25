/* 
** File:
**   $Id: $
**
** Purpose: Provide a class to manage the application's tables.
**
** Notes
**   1. Functions with comment blocks starting with pound signs (#) have
**      dependencies on the XML Element/Attribute definitions
**
** References:
**   1. OpenSat Object-based Application Developers Guide.
**
**
** $Date: $
** $Revision: $
** $Log: $
*/

/*
** Include Files:
*/

#include <string.h>
#include "tblmgr.h"


/*
** Global File Data
*/

static TBLMGR_Class* TblMgr2 = NULL;

static int  TblEntryId;
static char TblFileBuff[TBLMGR_BUFFSIZE];  /* XML table file buffer could be large so don't put on the stack */

/*
** Local File Function Prototypes
*/

typedef void (*DumpTableFuncPtr)(int32 FileHandle);   /* File-specific dump table function */

/******************************************************************************
** Function: DumpTableToFile
**
** General purpose function that takes a file pointer to the table-specific
** processing.
**
*/
static boolean DumpTableToFile(const char* FileName, const char* FileDescr, DumpTableFuncPtr DumpTableFunc);


/******************************************************************************
** Function: ParseXmlFile
**
** General purpose XML file parser.  The Start and End element callback
** functions do the file-specific processing.
**
*/
static boolean ParseXmlFile(const char* FileName,
                            XML_StartElementHandler StartElementHandler,
                            XML_EndElementHandler   EndElementHandler);

/*##############################################################################
** Functions: DumpExObjTable, ExObjTblStartElement, ExObjTblEndElement
**
** These functions provide the object table-specific processing for XML parsing
** and for dump file formatting.
**
*/
static void DumpExObjTable(int32 FileHandle);
static void XMLCALL ExObjTblStartElement(void *data, const char *el, const char **attr);
static void XMLCALL ExObjTblEndElement(void *data, const char *el);


/******************************************************************************
** Function: TBLMGR_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void TBLMGR_Constructor2(TBLMGR_Class* TblMgrPtr,
                        char* TblMgrFilePathName)
{

   TblMgr2 = TblMgrPtr;
   TBLMGR_LoadTblCmd  LoadTblCmd;

   /*
   ** Set all data and flags to zero.
   ** Use table load command to simplify constructor and reduce code
   ** duplication. All state variable managed by command function.
   **
   */
	
   CFE_PSP_MemSet(&(TblMgr2->ExObjTbl), 0, sizeof(TBLMGR_ExObjTbl));

   strncpy(LoadTblCmd.FileName,TblMgrFilePathName, OS_MAX_PATH_LEN);
   LoadTblCmd.LoadType = TBLMGR_LOAD_TBL_REPLACE;
   TBLMGR_LoadExObjTableCmd2((CFE_SB_MsgPtr_t)&LoadTblCmd);

} /* End TBLMGR_Constructor() */


/******************************************************************************
** Function: TBLMGR_ResetStatus
**
*/
void TBLMGR_ResetStatus2()
{

   CFE_PSP_MemSet((void *)TblMgr2, 0, sizeof(TBLMGR_Class));
   TblMgr2->ExObjTbl.LastLoadStatus = TBLMGR_STATUS_UNDEF;


} /* End TBLMGR_ResetStatus() */


/******************************************************************************
** Function: TBLMGR_LoadExObjTableCmd
**
*/
boolean TBLMGR_LoadExObjTableCmd2(const CFE_SB_MsgPtr_t MsgPtr)
{
   const  TBLMGR_LoadTblCmd *LoadTblCmd = (const TBLMGR_LoadTblCmd *) MsgPtr;
   int    entry;


   /*
   ** Set all data and flags to zero.
   ** AttrErrCnt - Incremented by XML parser
   */
	
   CFE_PSP_MemSet(&TblMgr2->ExObjTbl, 0, sizeof(TBLMGR_ExObjTbl));
   TblMgr2->ExObjTbl.LoadActive = TRUE;

   TblEntryId = 0;
   if (ParseXmlFile(LoadTblCmd->FileName, ExObjTblStartElement, ExObjTblEndElement) &&
       TblMgr2->ExObjTbl.AttrErrCnt == 0)
   {

      if (LoadTblCmd->LoadType == TBLMGR_LOAD_TBL_REPLACE)
      {
         if (EXOBJ_LoadTable(&(TblMgr2->ExObjTbl.Local)) == TRUE)
            TblMgr2->ExObjTbl.LastLoadValid = TBLMGR_STATUS_VALID;
         else
            TblMgr2->ExObjTbl.LastLoadValid = TBLMGR_STATUS_INVALID;
      } /* End if replace entire table */
      else if (LoadTblCmd->LoadType == TBLMGR_LOAD_TBL_UPDATE)
      {
         TblMgr2->ExObjTbl.LastLoadStatus = TBLMGR_STATUS_VALID;
         for (entry=0; entry < EXOBJ_TBL_MAX_ENTRY_ID; entry++)
         {

            if (TblMgr2->ExObjTbl.Modified[entry])
            {
               if (!EXOBJ_LoadTableEntry(entry, &(TblMgr2->ExObjTbl.Local.Entry[entry])))
                  TblMgr2->ExObjTbl.LastLoadStatus = TBLMGR_STATUS_INVALID;
                
            }

         } /* End pkt loop */

      } /* End if update individual records */
      else
      {
         CFE_EVS_SendEvent(TBLMGR_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"TBLMGR: Invalid table command load type %d",LoadTblCmd->LoadType);
      }

   } /* End if successful parse */
   else
   {
      CFE_EVS_SendEvent(TBLMGR_CMD_LOAD_PARSE_ERR_EID,CFE_EVS_ERROR,"TBLMGR: Table Parsing failure for file %s",LoadTblCmd->FileName);
   }

   TblMgr2->ExObjTbl.LoadActive = FALSE;  /* TODO - Is this flag helpful. WHen can process be interrupted if in same thread as tlm production? */

   return (TblMgr2->ExObjTbl.LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of TBLMGR_LoadExObjTableCmd() */


/******************************************************************************
** Function: TBLMGR_DumpExObjTableCmd
**
*/
boolean TBLMGR_DumpExObjTableCmd2(const CFE_SB_MsgPtr_t MsgPtr)
{
   const  TBLMGR_DumpTblCmd *DumpTblCmd = (const TBLMGR_DumpTblCmd *) MsgPtr;
   char   FileName[OS_MAX_PATH_LEN];

   /* Copy the commanded filename into local buffer to ensure size limitation and to allow for modification */
   CFE_PSP_MemCpy(FileName, (void *)DumpTblCmd->FileName, OS_MAX_PATH_LEN);

   /* Check to see if a default filename should be used */
   if (FileName[0] == '\0')
   {
      strncpy(FileName, DEMO_EXOBJ_TBL_DEF_DUMP_FILE, OS_MAX_PATH_LEN);
   }

   /* Make sure all strings are null terminated before attempting to process them */
   FileName[OS_MAX_PATH_LEN-1] = '\0';

   return DumpTableToFile(FileName,"OB App's Packet Table",DumpExObjTable);

} /* End of TBLMGR_DumpExObjTableCmd() */


/*##############################################################################**
** DumpExObjTable
**
** Dump the packet table contents to a file as text. The function signature
** must correspond to the DumpTableFuncPtr type.
**
*/
static void DumpExObjTable(int32 FileHandle)
{
   const  EXOBJ_Table  *ObjTblPtr;
   char   DumpRecord[256];
   int    i;

   sprintf(DumpRecord,"\nIndex: Data 1, Data 2, Data 3\n");
   OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

   ObjTblPtr = EXOBJ_GetTblPtr();

   for (i=0; i < EXOBJ_TBL_MAX_ENTRY_ID; i++)
   {
      sprintf(DumpRecord,"%03d: %4d, %4d, %4d\n", i,
              ObjTblPtr->Entry[i].Data1, ObjTblPtr->Entry[i].Data2, ObjTblPtr->Entry[i].Data3);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
   }

} /* End DumpExObjTable() */


/*##############################################################################
** Function: ExObjTblStartElement
**
** Callback function for the XML parser. It assumes global variables have been
** properly set up before the parsing begins.
**
*/
static void XMLCALL ExObjTblStartElement(void *data, const char *el, const char **attr)
{

   int     i;
   uint16  ProcessedAttr = 0x0F;  /* 4 Attribute bits */
   uint16  Index = 0;
   uint16  Data1 = 0;
   uint16  Data2 = 0;
   uint16  Data3 = 0;

   if (strcmp(el,TBLMGR_XML_EL_PKT_ENTRY) == 0)
   {
      for (i = 0; attr[i]; i += 2) {
         if (strcmp(attr[i],TBLMGR_XML_AT_INDEX)==0) {
            Index = atoi(attr[i + 1]);
            ProcessedAttr &= 0xFE;
         } else if (strcmp(attr[i],TBLMGR_XML_AT_DATA1)==0) {
            Data1 = atoi(attr[i + 1]);
            ProcessedAttr &= 0xFD;
         } else if (strcmp(attr[i],TBLMGR_XML_AT_DATA2)==0) {
            Data2 = atoi(attr[i + 1]);
            ProcessedAttr &= 0xFB;
         } else if (strcmp(attr[i],TBLMGR_XML_AT_DATA3)==0) {
            Data3 = atoi(attr[i + 1]);
            ProcessedAttr &= 0xF7;
         }
      } /* End attribute loop */

      if (ProcessedAttr == 0)
      {
        /* TODO - Add error checking */
        TblMgr2->ExObjTbl.Local.Entry[Index].Data1 = Data1;
        TblMgr2->ExObjTbl.Local.Entry[Index].Data2 = Data2;
        TblMgr2->ExObjTbl.Local.Entry[Index].Data3 = Data3;
        TblMgr2->ExObjTbl.Modified[Index] = TRUE;
      }
      else
      {
         TblMgr2->ExObjTbl.AttrErrCnt++;
      }

  } /* End if TBLMGR_XML_EL_ENTRY found */

} /* End ExObjTblStartElement() */

static void XMLCALL ExObjTblEndElement(void *data, const char *el)
{

} /* End ExObjTblEndElement() */


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
      CFE_EVS_SendEvent(TBLMGR_CREATE_ERR_EID, CFE_EVS_ERROR, "Failed to allocate memory for XML parser");
   }
   else {

      XML_SetElementHandler(XmlParser, StartElementHandler, EndElementHandler);

      FileHandle = OS_open(FilePathName, OS_READ_ONLY, 0);
      
      if (FileHandle >= 0) {

         Done = FALSE;

         while (!Done) {

            ReadStatus = OS_read(FileHandle, TblFileBuff, TBLMGR_BUFFSIZE);

            if ( ReadStatus == OS_FS_ERROR )
            {
               CFE_EVS_SendEvent(TBLMGR_FILE_READ_ERR_EID, CFE_EVS_ERROR, "File read error, EC = 0x%08X",ReadStatus);
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

                  CFE_EVS_SendEvent(TBLMGR_PARSE_ERR_EID, CFE_EVS_ERROR, "Parse error at line %lu, error code = %ls",
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
      
          CFE_EVS_SendEvent(TBLMGR_FILE_OPEN_ERR_EID, CFE_EVS_ERROR, "File open error for %s, Error = %d",
                            FilePathName, FileHandle );
      }

      XML_ParserFree(XmlParser);

   } /* end if parser allocated */

   return RetStatus;

} /* End ParseXmlFile() */

/******************************************************************************
** Function: DumpTableToFile
**
*/
static boolean DumpTableToFile(const char* FileName, const char* FileDescr, DumpTableFuncPtr DumpTableFunc)
{

   CFE_FS_Header_t  CfeStdFileHeader;
   int32            FileHandle;
   int32            FileStatus;
   boolean          RetStatus = FALSE;

   /* Create a new dump file, overwriting anything that may have existed previously */
   FileHandle = OS_creat(FileName, OS_WRITE_ONLY);

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
          CFE_EVS_SendEvent(TBLMGR_WRITE_CFE_HDR_ERR_EID,
                            CFE_EVS_ERROR,
                            "Error writing cFE File Header to '%s', Status=0x%08X",
                            FileName, FileStatus);

      }
   } /* End if file create */
   else
   {

        CFE_EVS_SendEvent(TBLMGR_CREATE_MSG_DUMP_ERR_EID,
                          CFE_EVS_ERROR,
                          "Error creating CDS dump file '%s', Status=0x%08X",
                          FileName, FileHandle);

    }

   OS_close(FileHandle);

   return RetStatus;

} /* End of DumpTableToFile() */


/* end of file */
