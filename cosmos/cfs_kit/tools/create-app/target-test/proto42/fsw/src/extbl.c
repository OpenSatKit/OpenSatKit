/*
** Purpose: Implement example table.
**
** Notes:
**   1. Template written by David McComas and licensed under the GNU
**      Lesser General Public License (LGPL).
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
#include "extbl.h"


/*
** Type Definitions
*/


/*
** Global File Data
*/

static EXTBL_Class* ExTbl = NULL;

/*
** Local File Function Prototypes
*/

/******************************************************************************
** Function: ParseLine
**
** Parse a single line in a table load file. 
*/
static void ParseLine(char *FileLineBuff);

/******************************************************************************
** Function: ParseLoadFile
**
** Read a user supplied file, verify the text syntax, remove extraneous 
** white spaces and call the user supplied callback function for each 
** non-comment line in the file.
**
** Filename includes the full path and the file name.
**
** Returns the number of file entries processed.
*/
static uint16 ParseLoadFile(const char* Filename);


/******************************************************************************
** Function: EXTBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void EXTBL_Constructor(EXTBL_Class* TblObj, 
                       EXTBL_LoadTblFunc LoadTbl, 
                       EXTBL_LoadEntryFunc LoadEntry,
                       EXTBL_GetDataPtrFunc GetDataPtr)
{

   ExTbl = TblObj;

   CFE_PSP_MemSet(ExTbl, 0, sizeof(EXTBL_Class));

   ExTbl->LoadTbl    = LoadTbl;
   ExTbl->LoadEntry  = LoadEntry; 
   ExTbl->GetDataPtr = GetDataPtr; 

} /* End EXTBL_Constructor() */


/******************************************************************************
** Function: EXTBL_ResetStatus
**
*/
void EXTBL_ResetStatus(void)
{

   ExTbl->ErrCnt = 0;
   ExTbl->LastLoadValid = TRUE;     /* Avoid alarming ground ops */

} /* End EXTBL_ResetStatus() */


/******************************************************************************
** Function: EXTBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean EXTBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int Entry;
   int EntriesProcessed;

   OS_printf("EXTBL_LoadCmd() Entry\n");

   /*
   ** Set all data and flags to zero.
   */
	
   CFE_PSP_MemSet(&(ExTbl->Data), 0, sizeof(EXTBL_Data));
   ExTbl->LastLoadValid = FALSE;
   ExTbl->ErrCnt = 0;
   ExTbl->FileLineNum = 0;

   EntriesProcessed = ParseLoadFile(Filename);
   
   if (EntriesProcessed > 0)
   {

      if (LoadType == TBLMGR_LOAD_TBL_REPLACE)
      {
         OS_printf("EXTBL_LoadCmd() Call replace table callback function\n");
         ExTbl->LastLoadValid = ((ExTbl->LoadTbl)(&(ExTbl->Data)) == TRUE);

      } /* End if replace entire table */
      else if (LoadType == TBLMGR_LOAD_TBL_UPDATE)
      {

         OS_printf("EXTBL_LoadCmd() Call replace entry callback function\n");
         ExTbl->LastLoadValid = TRUE;
         for (Entry=0; Entry < EXTBL_MAX_ENTRY_ID; Entry++)
         {

            if (ExTbl->Modified[Entry])
            {
              
               /* If any entry fails then set invlaid flag */
               if ( !(ExTbl->LoadEntry)(Entry, &(ExTbl->Data.Entry[Entry])) )
                  ExTbl->LastLoadValid = FALSE;
                
            }

         } /* End entry loop */

      } /* End if update individual records */
      else
      {
         CFE_EVS_SendEvent(EXTBL_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"EXTBL: Invalid table command load type %d",LoadType);
      }

   } /* End if successful parse */
   else
   {
      CFE_EVS_SendEvent(EXTBL_CMD_LOAD_PARSE_ERR_EID,CFE_EVS_ERROR,"EXTBL: Table Parsing failure for file %s",Filename);
   }

   return ExTbl->LastLoadValid;

} /* End of EXTBL_LoadCmd() */


/******************************************************************************
** Function: EXTBL_DumpCmd
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
boolean EXTBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   int32    FileHandle;
   int   i;
   char  DumpRecord[256];
   const EXTBL_Data* TblData;

   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS)
   {

      TblData = (ExTbl->GetDataPtr)();
      for (i=0; i < EXTBL_MAX_ENTRY_ID; i++)
      {
      
         sprintf(DumpRecord,"%03d, %4d, %4d, %4d;\n", i,
                 TblData->Entry[i].Data1, TblData->Entry[i].Data2, TblData->Entry[i].Data3);
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      } /* End Entry loop */

      sprintf(DumpRecord,"!\n!Index, Data 1, Data 2, Data 3\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      /* TODO - Add additional information: When file dumped, etc. */
      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else
   {
   
      CFE_EVS_SendEvent(EXTBL_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of EXTBL_DumpCmd() */


/******************************************************************************
** Function: ParseLoadFile
**
*/
uint16 ParseLoadFile(const char* Filename)
{

   char     FileLineBuff[EXTBL_MAX_FILE_LINE_LEN];
   uint16   BuffLen = 0;
   char     c;
   int      FileHandle;
   int32    ReadStatus;
   boolean  LineTooLong = FALSE;
   boolean  Done;

   ExTbl->ErrCnt = 0;
   ExTbl->FileLineNum = 1;

   FileHandle = OS_open(Filename, OS_READ_ONLY, 0);
      
   if (FileHandle >= 0) {

      Done = FALSE;
      memset(FileLineBuff,0x0,EXTBL_MAX_FILE_LINE_LEN);
      BuffLen = 0;

      /*
      ** Parse the lines from the file. If it has an error
      ** or reaches EOF, then abort the loop.
      */
      while (!Done) {

         ReadStatus = OS_read(FileHandle, &c, 1);

         if ( ReadStatus == OS_FS_ERROR )
         {
            CFE_EVS_SendEvent(EXTBL_FILE_READ_ERR_EID, CFE_EVS_ERROR, "File read error, EC = 0x%08X",ReadStatus);
            Done = TRUE;
         }
         else if ( ReadStatus == 0 ) /* Reached EOF */
         {
            Done = TRUE;
         }
         else if(c != '!')
         {
             if ( c <= ' ') 
                /* Skip all white space in the file */
                ;
             else if ( c == ',' ) 
             {
         
                /* Replace the field delimiter with a space to support sscanf string parsing */
                if ( BuffLen < EXTBL_MAX_FILE_LINE_LEN )
                {
                   FileLineBuff[BuffLen] = ' ';
                }
                else
                {
                   LineTooLong = TRUE;
                }
                BuffLen++;
         
             } /* End if == ',' */
             else if ( c != ';' ) 
             {

                /* Not EOL so copy data into buffer */
                if ( BuffLen < EXTBL_MAX_FILE_LINE_LEN )
                {
                   FileLineBuff[BuffLen] = c;
                }
                else
                {
                   LineTooLong = TRUE;
                }
                BuffLen++;

             }
             else /* Must be ';' */
             {
                if ( LineTooLong == TRUE ) 
                {
                   CFE_EVS_SendEvent(EXTBL_LINE_LEN_ERR_EID, CFE_EVS_ERROR, "Error in file %s. Line %d is too long: %d bytes",
                                     Filename, ExTbl->FileLineNum, BuffLen);
                   LineTooLong = FALSE;
                }
                else
                {
                   ParseLine(FileLineBuff);
                   ExTbl->FileLineNum++;
                }

                memset(FileLineBuff,0x0,EXTBL_MAX_FILE_LINE_LEN);
                BuffLen = 0;

             } /* End if ';' */

         } /* End if != '!' */
         else
         {
            /* DOne when EOF character '!' is reached */
            Done = TRUE;
         }

      } /* End while !Done */
     
      OS_close(FileHandle);

   } /* End if file opened */
   else
   {
      
      CFE_EVS_SendEvent(EXTBL_FILE_OPEN_ERR_EID, CFE_EVS_ERROR, "File open error for %s, Error = %d",
                        Filename, FileHandle );
   }

   return (ExTbl->FileLineNum-1);

} /* End EXTBL_ParseFile() */


/******************************************************************************
** Function: ParseLine
**
*/
void ParseLine(char *FileLineBuff)
{

   int  ScanfCnt;
   int  Index, Data1, Data2, Data3;  

   ScanfCnt = sscanf(FileLineBuff,"%d %d %d %d", &Index, &Data1, &Data2, &Data3);
   
   if (ScanfCnt == 4)
   {
      if (Index < EXTBL_MAX_ENTRY_ID)
      {        
         ExTbl->Data.Entry[Index].Data1 = Data1;
         ExTbl->Data.Entry[Index].Data2 = Data2;
         ExTbl->Data.Entry[Index].Data3 = Data3;
         ExTbl->Modified[Index] = TRUE;
         OS_printf("EXTBL-ParseLine: %d, %d, %d, %d\n",Index, Data1, Data2, Data3);
      }
      else
      {
         ExTbl->ErrCnt++;     
         CFE_EVS_SendEvent(EXTBL_LOAD_INDEX_ERR_EID, CFE_EVS_ERROR, "Load file line %d error, invalid index %d",
                           ExTbl->FileLineNum, Index);
      }
      
   } /* Successful scanf() */
   else
   {
      ExTbl->ErrCnt++;
      CFE_EVS_SendEvent(EXTBL_LOAD_LINE_ERR_EID, CFE_EVS_ERROR, "Load file line %d error, invalid number of elements %d. Shoudl be 4.",
                        ExTbl->FileLineNum, ScanfCnt);
   } /* Unsuccessful scanf() */

} /* End ParseLine() */

/* end of file */
