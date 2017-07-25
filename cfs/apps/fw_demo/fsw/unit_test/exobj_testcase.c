/*
 * File:
 *   $Id: $
 *
 * Purpose: This file contains a unit test cases for the example object file exobj.c
 *
 * $Date: $
 * $Revision: $
 * $Log: $
 */

/*
 * Includes
 */

#include "common_types.h"
#include "exobj.h"

#include "uttest.h"
#include "utassert.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_sb_hooks.h"


#define TEST_PIPE_NAME      "Test Pipe"
#define TEST_PIPE_DEPTH     2
#define TEST_OBJ_TBL_LOAD   "exobjtbl.xml"

static EXOBJ_Class  UtExObj;
static EXOBJ_Table  UtObjTbl;

/*
** Function Definitions
*/

/*******************************************************************
** Function: LoadTestTable
**
*/
static void LoadTestTable(void)
{

   int i;
   EXOBJ_Entry *Entry;


   for (i=0; i < EXOBJ_TBL_MAX_ENTRY_ID; i++)
   {
      Entry = &(UtObjTbl.Entry[i]);
      CFE_PSP_MemSet(Entry, 0, sizeof(EXOBJ_Entry));

   } /* End Entry Loop */

   UtObjTbl.Entry[0].Data1  = 3;
   UtObjTbl.Entry[0].Data2  = 2;
   UtObjTbl.Entry[0].Data3  = 1;

   UtObjTbl.Entry[EXOBJ_TBL_MAX_ENTRY_ID/2].Data1 = EXOBJ_TBL_MAX_ENTRY_ID/2 + 1;
   UtObjTbl.Entry[EXOBJ_TBL_MAX_ENTRY_ID/2].Data2 = EXOBJ_TBL_MAX_ENTRY_ID/2 + 2;
   UtObjTbl.Entry[EXOBJ_TBL_MAX_ENTRY_ID/2].Data3 = EXOBJ_TBL_MAX_ENTRY_ID/2 + 3;

   UtObjTbl.Entry[EXOBJ_TBL_MAX_ENTRY_ID-1].Data1 = EXOBJ_TBL_MAX_ENTRY_ID + 1;
   UtObjTbl.Entry[EXOBJ_TBL_MAX_ENTRY_ID-1].Data2 = EXOBJ_TBL_MAX_ENTRY_ID + 2;
   UtObjTbl.Entry[EXOBJ_TBL_MAX_ENTRY_ID-1].Data3 = EXOBJ_TBL_MAX_ENTRY_ID + 3;

} /* End LoadTestTable() */

/* EXOBJ_Test01  - Constructor, ResetStatus, GetTblPtr */
void EXOBJ_Test01(void)
{
   const EXOBJ_Table* ObjTblPtr = NULL;

   printf("EXOBJ_Test01()\n");

   /* Constructor called as part of setup */
   UtAssert_True(UtExObj.Table.Entry[0].Data1 == 0, "UtExObj.Table.Entry[0].Data1 == 0");

   ObjTblPtr = EXOBJ_GetTblPtr();
   UtAssert_True(ObjTblPtr == &(UtExObj.Table), "ObjTbl.Table");

   EXOBJ_Constructor (&UtExObj);

} /* End EXOBJ_Test01() */


/* EXOBJ_Test02  - LoadTable, LoadTableEntry */
void EXOBJ_Test02(void)
{

   printf("EXOBJ_Test02()\n");

} /* End EXOBJ_Test02() */

/* EXOBJ_Test03  - Functional  */
void EXOBJ_Test03(void)
{

   printf("EXOBJ_Test03()\n");

   /* TODO - Functional test of how EXOBJ is used in the template app */

} /* End EXOBJ_Test03() */

/* Initialize test environment to default state for every test */
void EXOBJ_Setup(void)
{
   EXOBJ_Constructor (&UtExObj);
   LoadTestTable();
}

void EXOBJ_TearDown(void)
{
    /* cleanup test environment */
}

void EXOBJ_AddTestCase(void)
{
    UtTest_Add(EXOBJ_Test01, EXOBJ_Setup, EXOBJ_TearDown, "EXOBJ_Test01 - Constructor, ResetStatus, GetTblPtr");
    UtTest_Add(EXOBJ_Test02, EXOBJ_Setup, EXOBJ_TearDown, "EXOBJ_Test02 - LoadTable, LoadTableEntry");
}
