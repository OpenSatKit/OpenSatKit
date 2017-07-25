/*
 * File:
 *   $Id: $
 *
 * Purpose: This file contains a unit test cases for tblmgr.c
 *
 * Notes:
 *   1. These tests are intentionally incomplete WRT testing EXPAT dependent
 *      functions because EXPAT is not flight worthy and the goal of this
 *      template UT is to provide potentially reusable code.
 *
 * $Date: $
 * $Revision: $
 * $Log: $
 */

/*
 * Includes
 */

#include "common_types.h"
#include "tblmgr.h"
#include "exobj.h"

#include "utassert.h"
#include "uttest.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_sb_hooks.h"
/* trouble with dirent.h and too lazy to figure it out. I don't use it so redefined DIR */
#define  DIR  char
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_fs_stubs.h"

#define TEST_PIPE_NAME      "Test Pipe"
#define TEST_PIPE_DEPTH     2
#define TEST_OBJ_TBL_LOAD   "exobjtbl.xml"
#define TEST_OBJ_TBL_DUMP   "exobjtbl.txt"
#define TEST_OBJ_TBL_LOAD_PARTIAL  "exobjtbl_partial.xml"
#define TEST_OBJ_TBL_LOAD_INVALID  "exobjtbl_invalid.xml"

/*
 * File Global Variables
 */

static TBLMGR_Class UtTblMgr;
static EXOBJ_Class UtExObj;

FILE* FilePtr;


/*
 * Function Definitions
 */

/******************************************************************************
** Event Hook
**
*/
static uint16  ExpectedEventID;
static uint16  ExpectedEventType;
static boolean DisplayEventText = FALSE;

/* Local function used to setup expected values for the callback function */
static void UtSetExpectedEvent(uint16 EventID, uint16 EventType, boolean DisplayEvent)
{

	ExpectedEventID = EventID;
	ExpectedEventType = EventType;
	DisplayEventText = DisplayEvent;

} /* End UtSetExpectedEvent() */

/* Callback function called by the UT library send event stub */
static int32 UtSendEventHook(uint16 EventID, uint16 EventType, char *EventText)
{

    UtAssert_True((strlen(EventText) < CFE_EVS_MAX_MESSAGE_LENGTH),"Event message too long");
    UtAssert_True(EventID == ExpectedEventID, "Event ID Error");
    UtAssert_True(EventType == ExpectedEventType, "Event ID Error");
    if (DisplayEventText)
    {
    	printf("Event %d Text: ", EventID); printf (EventText); printf("\n");
    }
    return CFE_SUCCESS;

} /* End UtSendEventHook() */

/******************************************************************************
** File Function Hooks
**
*/
int32 UT_fcreate (const char *path, int32  access)
{

   int32 RetStatus;

   /* I only use it for write */
   FilePtr = fopen(path,"w");

   RetStatus = (FilePtr == NULL ? OS_FS_ERROR : OS_FS_SUCCESS);

   return RetStatus;

} /* End UT_fcreate() */

int32 UT_fopen (const char *path,  int32 access,  uint32  mode)
{
   int32 RetStatus;

   /* I only use it for read or write */
   if (access == OS_READ_ONLY)
      FilePtr = fopen(path,"r");
   else
      FilePtr = fopen(path,"w");

   RetStatus = (FilePtr == NULL ? OS_FS_ERROR : OS_FS_SUCCESS);

   /* printf ("UT_fopen() - Open status = 0x%8X\n",RetStatus); */

   return RetStatus;

} /* End UT_FileOpen () */


int32 UT_fclose (int32  filedes)
{

   if (FilePtr != NULL)
   {
      fclose (FilePtr);
      return (OS_FS_SUCCESS);
   }
   return OS_FS_SUCCESS;

} /* End OS_close () */

int32 UT_fread  (int32  filedes, void *buffer, uint32 nbytes)
{

   int32 BytesRead;

   if (FilePtr != NULL)
   {
      BytesRead = (int)fread(buffer, 1, nbytes, FilePtr);
      return (BytesRead);
   }
   return OS_FS_ERROR;

} /* End UT_fread() */



int32 UT_fwrite (int32  filedes, void *buffer, uint32 nbytes)
{

   int32 BytesWritten;

   if (FilePtr != NULL)
   {
      BytesWritten = fwrite(buffer, 1, nbytes, FilePtr);
      return (BytesWritten);
   }
   return OS_FS_ERROR;

} /* End UT_fwrite() */


int32 UT_CFE_FS_WriteHeader(int32 FileDes, CFE_FS_Header_t *Hdr)
{
   /*printf("UT_CFE_FS_WriteHeader()\n"); */
   return (UT_fwrite (FileDes, Hdr, sizeof(CFE_FS_Header_t)));
}

/******************************************************************************
** Function: TBLMGR_Test01
**
** - Constructor
** - LoadExObjTableCmd: Covers replace entire table logic
** - ResetStatus
**
*/
void TBLMGR_Test01(void)
{

   int i;

   printf("TBLMGR_Test01() - A\n");

   EXOBJ_Constructor(&UtExObj);

   CFE_PSP_MemSet(&UtTblMgr, 1, sizeof(TBLMGR_Class));
   UtTblMgr.ExObjTbl.LastLoadValid=0;

   TBLMGR_Constructor(&UtTblMgr, TEST_OBJ_TBL_LOAD);

   UtAssert_True((UtTblMgr.ExObjTbl.LoadActive==0),"UtTblMgr.ExObjTbl.LoadActive==0");
   UtAssert_True((UtTblMgr.ExObjTbl.LastLoadValid==1),"UtTblMgr.ExObjTbl.LastLoadValid==1");
   UtAssert_True((UtTblMgr.ExObjTbl.AttrErrCnt==0),"UtTblMgr.ExObjTbl.AttrErrCnt==0");

   /* See contents of TEST_OBJ_TBL_LOAD fro expected results */
   for (i=0; i<EXOBJ_TBL_MAX_ENTRY_ID; i++)
   {
	  switch (i) {
	  case 0:
         UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 0),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 0");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 1),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 1");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 2),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 2");
		 break;
	  case 1:
         UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 3),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 3");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 4),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 4");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 5),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 5");
		 break;
	  case 2:
         UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 6),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 6");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 7),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 7");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 8),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 8");
		 break;
	  case 3:
         UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 9),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 9");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 10),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 10");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 11),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 11");
		 break;
	  case 4:
         UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 12),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 12");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 13),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 13");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 14),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 14");
		 break;
	  case 15:
         UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 15),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 15");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 16),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 16");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 17),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 17");
		 break;
	  case 16:
         UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 18),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 18");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 19),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 19");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 20),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 20");
		 break;
	  case (EXOBJ_TBL_MAX_ENTRY_ID-1):
         UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 21),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 21");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 22),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 22");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 23),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 23");
		 break;
	  default:
         UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == FALSE),"UtTblMgr.ExObjTbl.Modified==FALSE");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 0),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 0");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 0),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 0");
         UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 0),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 0");
		 break;
	   } /* End switch */

   } /* End entry loop */


   printf("TBLMGR_Test01() - B\n");

   /* Verify ResetStatus() */

   UtTblMgr.ExObjTbl.LoadActive    = TRUE;
   UtTblMgr.ExObjTbl.LastLoadValid = 0;
   UtTblMgr.ExObjTbl.AttrErrCnt    = 1;

   TBLMGR_ResetStatus();

   UtAssert_True((UtTblMgr.ExObjTbl.LoadActive==0),"UtTblMgr.ExObjTbl.LoadActive==0");
   UtAssert_True((UtTblMgr.ExObjTbl.LastLoadValid==1),"UtTblMgr.ExObjTbl.LastLoadValid==1");
   UtAssert_True((UtTblMgr.ExObjTbl.AttrErrCnt==0),"UtTblMgr.ExObjTbl.AttrErrCnt==0");

   for (i=0; i<EXOBJ_TBL_MAX_ENTRY_ID; i++)
   {

	  UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == FALSE),"UtTblMgr.ExObjTbl.Modified==FALSE");
      UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 0),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 0");
      UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 0),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 0");
      UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 0),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 0");

   } /* End entry loop */

} /* End TBLMGR_Test01() */

/******************************************************************************
** Function: TBLMGR_Test02
**
** - Assumes Test01 run first and has loaded the TEST_OBJ_TBL_LOAD file.
**
** - LoadExObjTableCmd: Covers logic not tested by Constructor & full load
** - DumpExObjTableCmd
** - DumpExObjTable
** - ExObjTblStartElement
** - DumpTableToFile
**
** TODO
** 1. Binary header contents not verified
** 2. File I/O error paths not verified
*/
void TBLMGR_Test02(void)
{

   int i;
   FILE*  DmpFile;
   CFE_FS_Header_t  DmpFileHdr;
   char             DmpFileStr[132];
   int Index, Data1, Data2, Data3;

   TBLMGR_LoadTblCmd  LoadTblCmd;
   TBLMGR_DumpTblCmd  DumpTblCmd;

   printf("TBLMGR_Test02() - A\n");

   strncpy(LoadTblCmd.FileName, TEST_OBJ_TBL_LOAD_PARTIAL, OS_MAX_PATH_LEN);
   LoadTblCmd.LoadType = TBLMGR_LOAD_TBL_UPDATE;
   TBLMGR_LoadExObjTableCmd(&LoadTblCmd);

   UtAssert_True((UtTblMgr.ExObjTbl.LoadActive==0),"UtTblMgr.ExObjTbl.LoadActive==0");
   UtAssert_True((UtTblMgr.ExObjTbl.LastLoadValid==1),"UtTblMgr.ExObjTbl.LastLoadValid==1");
   UtAssert_True((UtTblMgr.ExObjTbl.AttrErrCnt==0),"UtTblMgr.ExObjTbl.AttrErrCnt==0");

   /* See contents of TEST_OBJ_TBL_LOAD_PARTIAL to understand expected results */
   for (i=0; i<EXOBJ_TBL_MAX_ENTRY_ID; i++)
   {
      switch (i) {
      case 0:
         UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 2),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 2");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 1),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 1");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 0),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 0");
		 break;
	  case 15:
		 UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 5),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 5");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 4),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 4");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 3),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 3");
		 break;
	  case 16:
		 UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 8),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 8");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 7),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 7");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 6),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 6");
		 break;
	  case (EXOBJ_TBL_MAX_ENTRY_ID-1):
		 UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 11),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 11");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 10),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 10");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 ==  9),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 9");
		 break;
	  default:
		 if (i>0 && i<5)
         {
            UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == TRUE),"UtTblMgr.ExObjTbl.Modified==TRUE");
         }
		 else
		 {
		    UtAssert_True((UtTblMgr.ExObjTbl.Modified[i] == FALSE),"UtTblMgr.ExObjTbl.Modified==FALSE");
		 }
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 0),"UtTblMgr.ExObjTbl.Local.Entry[i].Data1 == 0");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 0),"UtTblMgr.ExObjTbl.Local.Entry[i].Data2 == 0");
		 UtAssert_True((UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 0),"UtTblMgr.ExObjTbl.Local.Entry[i].Data3 == 0");
		 break;
	   } /* End switch */

	} /* End entry loop */

   printf("TBLMGR_Test02() - B\n");

   UtSetExpectedEvent(TBLMGR_CMD_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,FALSE);
   LoadTblCmd.LoadType = TBLMGR_LOAD_TBL_UPDATE+1;
   TBLMGR_LoadExObjTableCmd(&LoadTblCmd);

   UtSetExpectedEvent(TBLMGR_CMD_LOAD_PARSE_ERR_EID,CFE_EVS_ERROR,FALSE);
   strncpy(LoadTblCmd.FileName, TEST_OBJ_TBL_LOAD_INVALID, OS_MAX_PATH_LEN);
   LoadTblCmd.LoadType = TBLMGR_LOAD_TBL_UPDATE;
   TBLMGR_LoadExObjTableCmd(&LoadTblCmd);

   /* Verify DumpExObjTableCmd() */
   CFE_PSP_MemCpy((void*)&(DumpTblCmd.FileName),TEST_OBJ_TBL_DUMP, OS_MAX_PATH_LEN);
   TBLMGR_DumpExObjTableCmd((CFE_SB_Msg_t*)&DumpTblCmd);

   DmpFile = fopen(TEST_OBJ_TBL_DUMP,"r");
   fseek(DmpFile, sizeof(CFE_FS_Header_t)+1, SEEK_CUR);
   /* TODO - Verify binary header contents */
   fgets(&DmpFileStr, 130, DmpFile);
   UtAssert_True((strcmp(DmpFileStr,"Index: Data 1, Data 2, Data 3\n") == 0),"Index: Data 1, Data 2, Data 3");

   for (i=0; i<EXOBJ_TBL_MAX_ENTRY_ID; i++)
   {

	  /* Verifies dumped values, not the exact syntax with spaces */
	  fgets(&DmpFileStr, 130, DmpFile);
	  printf ("Dump Str: %s", DmpFileStr);
	  sscanf(&DmpFileStr,"%d: %d, %d, %d", &Index, &Data1, &Data2, &Data3);
	  printf ("Dump Str: %d, %d, %d, %d\n", Index, Data1, Data2, Data3);

      UtAssert_True((Index == i),"Index");
	  switch (i) {
      case 0:
		 UtAssert_True((Data1 == 2),"Data1 == 2");
		 UtAssert_True((Data2 == 1),"Data2 == 1");
		 UtAssert_True((Data3 == 0),"Data3 == 0");
		 break;
	  case 15:
	     UtAssert_True((Data1 == 5),"Data1 == 5");
	     UtAssert_True((Data2 == 4),"Data2 == 4");
	     UtAssert_True((Data3 == 3),"Data3 == 3");
		 break;
	  case 16:
         UtAssert_True((Data1 == 8),"Data1 == 8");
         UtAssert_True((Data2 == 7),"Data2 == 7");
         UtAssert_True((Data3 == 6),"Data3 == 6");
		 break;
	  case (EXOBJ_TBL_MAX_ENTRY_ID-1):
         UtAssert_True((Data1 == 11),"Data1 == 11");
         UtAssert_True((Data2 == 10),"Data2 == 10");
         UtAssert_True((Data3 == 9),"Data3 == 9");
		 break;
	  default:
         UtAssert_True((Data1 == 0),"Data1 == 0");
         UtAssert_True((Data2 == 0),"Data2 == 0");
         UtAssert_True((Data3 == 0),"Data3 == 0");
		 break;
	   } /* End switch */

	} /* End entry loop */


} /* End TBLMGR_Test02() */

/******************************************************************************
** Function: TBLMGR_Test03
**
** Placeholder to verify text parser. ParseXmlFile() would be verified if it
** was worthy of flight.
*/
void TBLMGR_Test03(void)
{

	/* TODO - verify text parser */

} /* End TBLMGR_Test03() */

/* initialize test environment to default state for every test */
void TBLMGR_Setup(void)
{

   Ut_CFE_EVS_SetFunctionHook(UT_CFE_EVS_SENDEVENT_INDEX,(void *)UtSendEventHook);

   Ut_OSFILEAPI_Reset();
   Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_CREAT_INDEX, (void*)UT_fcreate);
   Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_OPEN_INDEX,  (void*)UT_fopen);
   Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX,  (void*)UT_fread);
   Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_WRITE_INDEX, (void*)UT_fwrite);
   Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_CLOSE_INDEX, (void*)UT_fclose);

   Ut_CFE_FS_Reset();
   Ut_CFE_FS_SetFunctionHook(UT_CFE_FS_WRITEHDR_INDEX, (void*)UT_CFE_FS_WriteHeader);


} /* TBLMGR_Setup() */

void TBLMGR_TearDown(void)
{
    /* cleanup test environment */
}

void TBLMGR_AddTestCase(void)
{
    UtTest_Add(TBLMGR_Test01, TBLMGR_Setup, TBLMGR_TearDown, "TBLMGR_Test01 - Constructor");
    UtTest_Add(TBLMGR_Test02, TBLMGR_Setup, TBLMGR_TearDown, "TBLMGR_Test02 - Load Command");
}
