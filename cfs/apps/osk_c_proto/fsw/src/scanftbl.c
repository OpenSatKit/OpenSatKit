/*
** Purpose: Implement example scanf table.
**
** Notes:
**   1. The load and dump functions use call back functions to investigate
**      whether the design could be modularized to a point when a user
**      only supplies table unique processing in a callback function. The
**      application framework TBLMGR is at that point and this is looking
**      whether refining it further makes sense.
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
#include "scanftbl.h"


/*
** Type Definitions
*/

/******************************************************************************
** Load table callback functions for line parsing
**
** The character buffer is verified to be less than ESTXT_MAX_FILE_ENTRY_CHAR and the
** parameters have been separate by a single space each. The easiest method for the
** user to parse the entry is using sscanf().
*/
  
typedef void   (*ParseLineFunc)(char *FileLineBuff);

/*
** Global File Data
*/

static SCANFTBL_Class* ScanfTbl = NULL;

/*
** Local File Function Prototypes
*/

/******************************************************************************
** Function: ParseLine
**
** Parse a single line in a table load file. 
** Signature must match ParseLineFunc typedef
*/
static void ParseLine(char *FileLineBuff);

/******************************************************************************
** Function: ParseLoadFile
**
*/
static uint16 ParseLoadFile(const char* Filename, ParseLineFunc ParseLine);


/******************************************************************************
** Function: SCANFTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void SCANFTBL_Constructor(SCANFTBL_Class* ObjPtr,
                          SCANFTBL_GetTblPtr    GetTblPtrFunc,
                          SCANFTBL_LoadTbl      LoadTblFunc, 
                          SCANFTBL_LoadTblEntry LoadTblEntryFunc)
{

   ScanfTbl = ObjPtr;

   CFE_PSP_MemSet(ScanfTbl, 0, sizeof(SCANFTBL_Class));

   ScanfTbl->GetTblPtrFunc    = GetTblPtrFunc;
   ScanfTbl->LoadTblFunc      = LoadTblFunc;
   ScanfTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

} /* End SCANFTBL_Constructor() */


/******************************************************************************
** Function: SCANFTBL_ResetStatus
**
*/
void SCANFTBL_ResetStatus(void)
{

   ScanfTbl->LastLoadStatus = TBLMGR_STATUS_UNDEF;
   ScanfTbl->AttrErrCnt    = 0;

} /* End SCANFTBL_ResetStatus() */


/******************************************************************************
** Function: SCANFTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean SCANFTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int Entry;
   int EntriesProcessed;

   CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, "SCANFTBL_LoadCmd() Entry\n");

   /*
   ** Set all data and flags to zero. If a table replace is commanded and
   ** all of the data is not defined the zeroes will be copied into the table. 
   ** Real flight code would validate all data is loaded for a replace.
   */
	
   CFE_PSP_MemSet(&(ScanfTbl->Tbl), 0, sizeof(ExTblData_Param));
   
   SCANFTBL_ResetStatus(); /* Reset status helps isolate errors if they occur */
   
   EntriesProcessed = ParseLoadFile(Filename, ParseLine);
   
   if (EntriesProcessed > 0) {

      if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
         ScanfTbl->LastLoadStatus = ((ScanfTbl->LoadTblFunc)(&(ScanfTbl->Tbl)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

      } /* End if replace entire table */
      else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
         ScanfTbl->LastLoadStatus = TBLMGR_STATUS_VALID;
         for (Entry=0; Entry < SCANFTBL_MAX_ENTRY_ID; Entry++) {
           
            if (ScanfTbl->Modified[Entry]) {
               
               if (!(ScanfTbl->LoadTblEntryFunc)(Entry, &(ScanfTbl->Tbl.Entry[Entry])))
                   ScanfTbl->LastLoadStatus = TBLMGR_STATUS_INVALID;
            }

         } /* End pkt loop */

      } /* End if update individual records */
      else {
         
         CFE_EVS_SendEvent(SCANFTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"SCANFTBL: Invalid table command load type %d",LoadType);
      }

   } /* End if successful parse */
   else {
      
      CFE_EVS_SendEvent(SCANFTBL_CMD_LOAD_PARSE_ERR_EID,CFE_EVS_ERROR,"SCANFTBL: Table Parsing failure for file %s",Filename);
   
   }

   return (ScanfTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of SCANFTBL_LoadCmd() */


/******************************************************************************
** Function: SCANFTBL_DumpCmd
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
boolean SCANFTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   int32    FileHandle;
   const ExTblData_Param *ScanfTblPtr;
   char  DumpRecord[256];
   int   i;

   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);


   if (FileHandle >= OS_FS_SUCCESS) {

      ScanfTblPtr = (ScanfTbl->GetTblPtrFunc)();
   
      for (i=0; i < SCANFTBL_MAX_ENTRY_ID; i++) {
      
         sprintf(DumpRecord,"%03d, %4d, %4d, %4d;\n", i,
                 ScanfTblPtr->Entry[i].Data1, ScanfTblPtr->Entry[i].Data2, ScanfTblPtr->Entry[i].Data3);
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      } /* End Entry loop */

      sprintf(DumpRecord,"!\n!Index, Data 1, Data 2, Data 3\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* TODO - Add addition meta data when file dumped */
      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(SCANFTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of SCANFTBL_DumpCmd() */


/******************************************************************************
** Function: ParseLoadFile
**
** See file prologue notes for callback rationale.
*/
uint16 ParseLoadFile(const char* Filename, ParseLineFunc ParseLine)
{

   char     FileLineBuff[SCANFTBL_MAX_FILE_LINE_LEN];
   uint16   BuffLen = 0;
   char     c;
   int      FileHandle;
   int32    ReadStatus;
   boolean  LineTooLong = FALSE;
   boolean  Done;

   ScanfTbl->AttrErrCnt  = 0;
   ScanfTbl->FileLineNum = 1;

   FileHandle = OS_open(Filename, OS_READ_ONLY, 0);
      
   if (FileHandle >= 0) {

      Done = FALSE;
      memset(FileLineBuff,0x0,SCANFTBL_MAX_FILE_LINE_LEN);
      BuffLen = 0;

      /*
      ** Parse the lines from the file. If it has an error
      ** or reaches EOF, then abort the loop.
      */
      while (!Done) {

         ReadStatus = OS_read(FileHandle, &c, 1);

         if ( ReadStatus == OS_FS_ERROR ) {
            
            CFE_EVS_SendEvent(SCANFTBL_FILE_READ_ERR_EID, CFE_EVS_ERROR, "File read error, EC = 0x%08X",ReadStatus);
            Done = TRUE;
         
         }
         else if ( ReadStatus == 0 ) {/* Reached EOF */
            
            Done = TRUE;
         
         }
         else if(c != '!') {
            
            if ( c <= ' ') 
                /* Skip all white space in the file */
                ;
            else if ( c == ',' ) {
         
               /* Replace the field delimiter with a space to support sscanf string parsing */
               if ( BuffLen < SCANFTBL_MAX_FILE_LINE_LEN ) {
                  
                  FileLineBuff[BuffLen] = ' ';
               }
               else {
                  
                  LineTooLong = TRUE;
               
               }
               BuffLen++;
         
            } /* End if == ',' */
            else if ( c != ';' ) {

               /* Not EOL so copy data into buffer */
               if ( BuffLen < SCANFTBL_MAX_FILE_LINE_LEN ) {
               
                  FileLineBuff[BuffLen] = c;
                
               }
               else {
                
                  LineTooLong = TRUE;
               }
               BuffLen++;

            }
            else { /* Must be ';' */
                
               if ( LineTooLong == TRUE ) { 
               
                  CFE_EVS_SendEvent(SCANFTBL_LINE_LEN_ERR_EID, CFE_EVS_ERROR, "Error in file %s. Line %d is too long: %d bytes",
                                    Filename, ScanfTbl->FileLineNum, BuffLen);
                  LineTooLong = FALSE;
               
               }
               else {
                  
                  (ParseLine)(FileLineBuff);
                  ScanfTbl->FileLineNum++;
               }

               memset(FileLineBuff,0x0,SCANFTBL_MAX_FILE_LINE_LEN);
               BuffLen = 0;

            } /* End if ';' */

         } /* End if != '!' */
         else {
            
            /* Done when EOF character '!' is reached */
            Done = TRUE;
         }

      } /* End while !Done */
     
      OS_close(FileHandle);

   } /* End if file opened */
   else {
      
      CFE_EVS_SendEvent(SCANFTBL_FILE_OPEN_ERR_EID, CFE_EVS_ERROR, "File open error for %s, Error = %d",
                        Filename, FileHandle );
   }

   return (ScanfTbl->FileLineNum-1);

} /* End ParseLoadFile() */


/******************************************************************************
** Function: ParseLine
**
*/
void ParseLine(char *FileLineBuff)
{

   int  ScanfCnt;
   int  Index, Data1, Data2, Data3;  

   ScanfCnt = sscanf(FileLineBuff,"%d %d %d %d", &Index, &Data1, &Data2, &Data3);
   
   if (ScanfCnt == 4) {
      
      if (Index < SCANFTBL_MAX_ENTRY_ID) {
         
         ScanfTbl->Tbl.Entry[Index].Data1 = Data1;
         ScanfTbl->Tbl.Entry[Index].Data2 = Data2;
         ScanfTbl->Tbl.Entry[Index].Data3 = Data3;
         ScanfTbl->Modified[Index] = TRUE;
         
         CFE_EVS_SendEvent(OSK_C_PROTO_INIT_DEBUG_EID, OSK_C_PROTO_INIT_EVS_TYPE, 
                           "SCANFTBL-ParseLine: %d, %d, %d, %d\n",
                           Index, Data1, Data2, Data3);
      }
      else {
         
         ScanfTbl->AttrErrCnt++;     
         CFE_EVS_SendEvent(SCANFTBL_LOAD_INDEX_ERR_EID, CFE_EVS_ERROR, "Load file line %d error, invalid index %d",
                           ScanfTbl->FileLineNum, Index);
      }
      
   } /* Successful scanf() */
   else
   {
      ScanfTbl->AttrErrCnt++;
      CFE_EVS_SendEvent(SCANFTBL_LOAD_LINE_ERR_EID, CFE_EVS_ERROR, "Load file line %d error, invalid number of elements %d. Should be 4.",
                        ScanfTbl->FileLineNum, ScanfCnt);
   } /* Unsuccessful scanf() */

} /* End ParseLine() */

