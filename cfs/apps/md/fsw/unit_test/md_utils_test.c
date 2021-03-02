 /*************************************************************************
 ** File: md_utils_test.c 
 **
 ** NASA Docket No. GSC-18,450-1, identified as “Core Flight Software System (CFS)
 ** Memory Dwell Application Version 2.3.3” 
 **
 ** Copyright © 2019 United States Government as represented by the Administrator of
 ** the National Aeronautics and Space Administration. All Rights Reserved. 
 **
 ** Licensed under the Apache License, Version 2.0 (the "License"); 
 ** you may not use this file except in compliance with the License. 
 ** You may obtain a copy of the License at 
 ** http://www.apache.org/licenses/LICENSE-2.0 
 **
 ** Unless required by applicable law or agreed to in writing, software 
 ** distributed under the License is distributed on an "AS IS" BASIS, 
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 ** See the License for the specific language governing permissions and 
 ** limitations under the License. 
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in 
 **   the file md_utils.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 ** Notes:
 **
 *************************************************************************/

/*
 * Includes
 */

#include "md_utils_test.h"
#include "md_utils.h"
#include "md_msg.h"
#include "md_msgdefs.h"
#include "md_events.h"
#include "md_version.h"
#include "md_test_utils.h"
#include "ut_osapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_psp_memrange_stubs.h"
#include "ut_cfe_psp_watchdog_stubs.h"
#include "ut_cfe_psp_timer_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Function Definitions
 */

void MD_TableIsInMask_Test_ShiftOddResult(void)
{
    boolean   Result;
    int16     TableId     = 2;
    uint16    TableMask   = 2;

    /* Execute the function being tested */
    Result = MD_TableIsInMask(TableId, TableMask);

    /* Verify results */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_TableIsInMask_Test_ShiftOddResult */

void MD_TableIsInMask_Test_ShiftEvenResult(void)
{
    boolean   Result;
    int16     TableId     = 2;
    uint16    TableMask   = 4;

    /* Execute the function being tested */
    Result = MD_TableIsInMask(TableId, TableMask);

    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_TableIsInMask_Test_ShiftEvenResult */

void MD_UpdateDwellControlInfo_Test(void)
{
    uint16   TableIndex   = 1;

    MD_AppData.MD_DwellTables[TableIndex].Entry[0].Length = 1;
    MD_AppData.MD_DwellTables[TableIndex].Entry[0].Delay  = 2;

    /* Execute the function being tested */
    MD_UpdateDwellControlInfo(TableIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].AddrCount == 1, "MD_AppData.MD_DwellTables[TableIndex].AddrCount == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].DataSize  == 1, "MD_AppData.MD_DwellTables[TableIndex].DataSize  == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].Rate      == 2, "MD_AppData.MD_DwellTables[TableIndex].Rate      == 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_UpdateDwellControlInfo_Test */

void MD_ValidEntryId_Test_Valid(void)
{
    boolean   Result;
    uint16    EntryId   = 1;

    /* Execute the function being tested */
    Result = MD_ValidEntryId(EntryId);

    /* Verify results */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidEntryId_Test_Valid */

void MD_ValidEntryId_Test_Invalid(void)
{
    boolean   Result;
    uint16    EntryId   = 0;

    /* Execute the function being tested */
    Result = MD_ValidEntryId(EntryId);

    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidEntryId_Test_Invalid */

void MD_ValidAddrRange_Test_Valid(void)
{
    boolean   Result;
    cpuaddr   Addr   = 1;
    uint32    Size   = 1;

    /* Execute the function being tested */
    Result = MD_ValidAddrRange(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidAddrRange_Test_Valid */

void MD_ValidAddrRange_Test_Invalid(void)
{
    boolean   Result;
    cpuaddr   Addr   = 1;
    uint32    Size   = 1;

    /* Set to reach "IsValid = FALSE" */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_ValidAddrRange(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidAddrRange_Test_Invalid */

void MD_ValidTableId_Test_Valid(void)
{
    boolean   Result;
    uint16    TableId = 1;

    /* Execute the function being tested */
    Result = MD_ValidTableId(TableId);

    /* Verify results */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidTableId_Test_Valid */

void MD_ValidTableId_Test_Invalid(void)
{
    boolean   Result;
    uint16    TableId = 0;

    /* Execute the function being tested */
    Result = MD_ValidTableId(TableId);

    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidTableId_Test_Invalid */

void MD_ValidFieldLength_Test_ValidFieldLength0(void)
{
    boolean   Result;
    uint16    FieldLength = 0;

    /* Execute the function being tested */
    Result = MD_ValidFieldLength(FieldLength);

    /* Verify results */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidFieldLength_Test_ValidFieldLength0 */

void MD_ValidFieldLength_Test_ValidFieldLength1(void)
{
    boolean   Result;
    uint16    FieldLength = 1;

    /* Execute the function being tested */
    Result = MD_ValidFieldLength(FieldLength);

    /* Verify results */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidFieldLength_Test_ValidFieldLength1 */

void MD_ValidFieldLength_Test_ValidFieldLength2(void)
{
    boolean   Result;
    uint16    FieldLength = 2;

    /* Execute the function being tested */
    Result = MD_ValidFieldLength(FieldLength);

    /* Verify results */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidFieldLength_Test_ValidFieldLength2 */

void MD_ValidFieldLength_Test_ValidFieldLength4(void)
{
    boolean   Result;
    uint16    FieldLength = 4;

    /* Execute the function being tested */
    Result = MD_ValidFieldLength(FieldLength);

    /* Verify results */
    UtAssert_True(Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidFieldLength_Test_ValidFieldLength4 */

void MD_ValidFieldLength_Test_Invalid(void)
{
    boolean   Result;
    uint16    FieldLength = 5;

    /* Execute the function being tested */
    Result = MD_ValidFieldLength(FieldLength);

    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidFieldLength_Test_Invalid */

void MD_Utils_Test_AddTestCases(void)
{
    UtTest_Add(MD_TableIsInMask_Test_ShiftOddResult, MD_Test_Setup, MD_Test_TearDown, "MD_TableIsInMask_Test_ShiftOddResult");
    UtTest_Add(MD_TableIsInMask_Test_ShiftEvenResult, MD_Test_Setup, MD_Test_TearDown, "MD_TableIsInMask_Test_ShiftEvenResult");

    UtTest_Add(MD_UpdateDwellControlInfo_Test, MD_Test_Setup, MD_Test_TearDown, "MD_UpdateDwellControlInfo_Test");

    UtTest_Add(MD_ValidEntryId_Test_Valid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidEntryId_Test_Valid");
    UtTest_Add(MD_ValidEntryId_Test_Invalid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidEntryId_Test_Invalid");

    UtTest_Add(MD_ValidAddrRange_Test_Valid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidAddrRange_Test_Valid");
    UtTest_Add(MD_ValidAddrRange_Test_Invalid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidAddrRange_Test_Invalid");

    UtTest_Add(MD_ValidTableId_Test_Valid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableId_Test_Valid");
    UtTest_Add(MD_ValidTableId_Test_Invalid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableId_Test_Invalid");

    UtTest_Add(MD_ValidFieldLength_Test_ValidFieldLength0, MD_Test_Setup, MD_Test_TearDown, "MD_ValidFieldLength_Test_ValidFieldLength0");
    UtTest_Add(MD_ValidFieldLength_Test_ValidFieldLength1, MD_Test_Setup, MD_Test_TearDown, "MD_ValidFieldLength_Test_ValidFieldLength1");
    UtTest_Add(MD_ValidFieldLength_Test_ValidFieldLength2, MD_Test_Setup, MD_Test_TearDown, "MD_ValidFieldLength_Test_ValidFieldLength2");
    UtTest_Add(MD_ValidFieldLength_Test_ValidFieldLength4, MD_Test_Setup, MD_Test_TearDown, "MD_ValidFieldLength_Test_ValidFieldLength4");
    UtTest_Add(MD_ValidFieldLength_Test_Invalid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidFieldLength_Test_Invalid");

} /* end MD_Utils_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
