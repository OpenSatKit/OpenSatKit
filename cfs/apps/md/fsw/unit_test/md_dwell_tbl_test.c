 /*************************************************************************
 ** File: md_dwell_tbl_test.c 
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
 **   the file md_dwell_tbl.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 *************************************************************************/

/*
 * Includes
 */

#include "md_dwell_tbl_test.h"
#include "md_dwell_tbl.h"
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

MD_DwellTableLoad_t   MD_DWELL_TBL_TEST_GlobalLoadTable;
int32 MD_DWELL_TBL_TEST_CFE_TBL_GetAddressHook(void **TblPtr, CFE_TBL_Handle_t TblHandle)
{
    *TblPtr = &MD_DWELL_TBL_TEST_GlobalLoadTable;
        
    return CFE_SUCCESS;

}

void MD_TableValidationFunc_Test_InvalidEnableFlag(void)
{
    int32                 Result;
    MD_DwellTableLoad_t   Table;

    Table.Enabled = 99;

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_TBL_ENA_FLAG_EID, CFE_EVS_ERROR, "Dwell Table rejected because value of enable flag (99) is invalid"),
        "Dwell Table rejected because value of enable flag (99) is invalid");

    UtAssert_True(Result == MD_TBL_ENA_FLAG_ERROR, "Result == MD_TBL_ENA_FLAG_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_TableValidationFunc_Test_InvalidEnableFlag */

#if MD_SIGNATURE_OPTION == 1
void MD_TableValidationFunc_Test_InvalidSignatureLength(void)
{
    int32                 Result;
    MD_DwellTableLoad_t   Table;
    uint16                i;

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

    for (i = 0; i <= MD_SIGNATURE_FIELD_LENGTH; i++)
    {
        Table.Signature[i] = 'x';
    }

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_TBL_SIG_LEN_ERR_EID, CFE_EVS_ERROR, "Dwell Table rejected because Signature length was invalid"),
        "Dwell Table rejected because Signature length was invalid");

    UtAssert_True(Result == MD_SIG_LEN_TBL_ERROR, "Result == MD_SIG_LEN_TBL_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_TableValidationFunc_Test_InvalidSignatureLength */
#endif

void MD_TableValidationFunc_Test_ResolveError(void)
{
    int32                 Result;
    MD_DwellTableLoad_t   Table;

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    Table.Entry[0].DwellAddress.Offset = 0;

    /* Set to make MD_CheckTableEntries return MD_RESOLVE_ERROR */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_RESOLVE_ERR_EID, CFE_EVS_ERROR, "Dwell Table rejected because address (sym='symname'/offset=0x00000000) in entry #1 couldn't be resolved"),
        "Dwell Table rejected because address (sym='symname'/offset=0x00000000) in entry #1 couldn't be resolved");

    
    UtAssert_True(Result == MD_RESOLVE_ERROR, "Result == MD_RESOLVE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 message we don't care about */
} /* end MD_TableValidationFunc_Test_ResolveError */

void MD_TableValidationFunc_Test_InvalidAddress(void)
{
    int32                 Result;
    MD_DwellTableLoad_t   Table;

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    Table.Entry[0].DwellAddress.Offset = 0;

    /* Set to make MD_CheckTableEntries return MD_INVALID_ADDR_ERROR */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_RANGE_ERR_EID, CFE_EVS_ERROR, "Dwell Table rejected because address (sym='symname'/offset=0x00000000) in entry #1 was out of range"),
        "Dwell Table rejected because address (sym='symname'/offset=0x00000000) in entry #1 was out of range");

    UtAssert_True(Result == MD_INVALID_ADDR_ERROR, "Result == MD_INVALID_ADDR_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 message we don't care about here */

} /* end MD_TableValidationFunc_Test_InvalidAddress */

void MD_TableValidationFunc_Test_InvalidLength(void)
{
    int32                 Result;
    MD_DwellTableLoad_t   Table;

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    Table.Entry[0].DwellAddress.Offset = 0;
    Table.Entry[0].Length = 5;

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_TBL_HAS_LEN_ERR_EID, CFE_EVS_ERROR, "Dwell Table rejected because length (5) in entry #1 was invalid"),
        "Dwell Table rejected because length (5) in entry #1 was invalid");

    UtAssert_True(Result == MD_INVALID_LEN_ERROR, "Result == MD_INVALID_LEN_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 message we don't care about here */

} /* end MD_TableValidationFunc_Test_InvalidLength */

void MD_TableValidationFunc_Test_NotAligned(void)
{
    int32                 Result;
    MD_DwellTableLoad_t   Table;

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    Table.Entry[0].DwellAddress.Offset = 1;
    Table.Entry[0].Length = 2;

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_TBL_ALIGN_ERR_EID, CFE_EVS_ERROR, "Dwell Table rejected because address (sym='symname'/offset=0x00000001) in entry #1 not properly aligned for 2-byte dwell"),
        "Dwell Table rejected because address (sym='symname'/offset=0x00000001) in entry #1 not properly aligned for 2-byte dwell");

    UtAssert_True(Result == MD_NOT_ALIGNED_ERROR, "Result == MD_NOT_ALIGNED_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 message we don't care about here */

} /* end MD_TableValidationFunc_Test_NotAligned */

void MD_TableValidationFunc_Test_ZeroRate(void)
{
    int32                 Result;
    MD_DwellTableLoad_t   Table;
    uint16                i;

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    for (i = 0; i < MD_DWELL_TABLE_SIZE; i++)
    {
        Table.Entry[i].DwellAddress.Offset = 0;
        Table.Entry[i].Length = 1;
        Table.Entry[i].Delay = 0;
    }

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_ZERO_RATE_TBL_INF_EID, CFE_EVS_INFORMATION, "Dwell Table is enabled but no processing will occur for table being loaded (rate is zero)"),
        "Dwell Table is enabled but no processing will occur for table being loaded (rate is zero)");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 message we don't care about here */

} /* end MD_TableValidationFunc_Test_ZeroRate */

void MD_TableValidationFunc_Test_Success(void)
{
    int32                 Result;
    MD_DwellTableLoad_t   Table;
    uint16                i;

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    for (i = 0; i < MD_DWELL_TABLE_SIZE; i++)
    {
        Table.Entry[i].DwellAddress.Offset = 0;
        Table.Entry[i].Length = 1;
        Table.Entry[i].Delay = 1;
    }

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 message we don't care about here */

} /* end MD_TableValidationFunc_Test_Success */

void MD_ReadDwellTable_Test(void)
{
    int32                 Result;
    MD_DwellTableLoad_t   Table;
    uint16                i;
    uint16                ActiveAddrCount = 0;
    uint16                Size = 0;
    uint32                Rate = 0;

    for (i = 0; i < MD_DWELL_TABLE_SIZE; i++)
    {
        Table.Entry[i].Length = 1;
        Table.Entry[i].Delay = 1;
    }

    /* Execute the function being tested */
    Result = MD_ReadDwellTable(&Table, &ActiveAddrCount, &Size, &Rate);

    /* Verify results */
    UtAssert_True(ActiveAddrCount == MD_DWELL_TABLE_SIZE, "ActiveAddrCount == MD_DWELL_TABLE_SIZE");
    UtAssert_True(Size == MD_DWELL_TABLE_SIZE, "Size == MD_DWELL_TABLE_SIZE");
    UtAssert_True(Rate == MD_DWELL_TABLE_SIZE, "Rate == MD_DWELL_TABLE_SIZE");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ReadDwellTable_Test */

void MD_CheckTableEntries_Test_Error(void)
{
    int32                 Result;
    MD_DwellTableLoad_t   Table;
    uint16                ErrorEntryArg = 0;

    Table.Entry[0].Length = 5;

    /* Execute the function being tested */
    Result = MD_CheckTableEntries(&Table, &ErrorEntryArg);

    /* Verify results */
    UtAssert_True(ErrorEntryArg == 0, "ErrorEntryArg == 0");

    UtAssert_True(Result == MD_INVALID_LEN_ERROR, "Result == MD_INVALID_LEN_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_DWELL_TBL_INF_EID, CFE_EVS_INFORMATION,
                     "MD Dwell Tbl verify results: good = 0, bad = 1, unused = 24"),
                                          "MD Dwell Tbl verify results: good = 0, bad = 1, unused = 24");

} /* end MD_CheckTableEntries_Test_Error */

void MD_CheckTableEntries_Test_Success(void)
{
    int32                 Result;
    MD_DwellTableLoad_t   Table;
    uint16                ErrorEntryArg = 0;
    uint16                i;

    for (i = 0; i < MD_DWELL_TABLE_SIZE; i++)
    {
        Table.Entry[i].DwellAddress.Offset = 0;
        Table.Entry[i].Length = 1;
        Table.Entry[i].Delay = 1;
    }

    /* Test the 'unused' entry count */
    Table.Entry[MD_DWELL_TABLE_SIZE - 1].DwellAddress.Offset = 0;
    Table.Entry[MD_DWELL_TABLE_SIZE - 1].Length = 0;
    Table.Entry[MD_DWELL_TABLE_SIZE - 1].Delay = 1;
    
    /* Execute the function being tested */
    Result = MD_CheckTableEntries(&Table, &ErrorEntryArg);

    /* Verify results */
    UtAssert_True(ErrorEntryArg == 0, "ErrorEntryArg == 0");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_DWELL_TBL_INF_EID, CFE_EVS_INFORMATION,
                     "MD Dwell Tbl verify results: good = 24, bad = 0, unused = 1"),
                                          "MD Dwell Tbl verify results: good = 24, bad = 0, unused = 1");

} /* end MD_CheckTableEntries_Test_Success */

void MD_ValidTableEntry_Test_SuccessDwellLengthZero(void)
{
    int32                 Result;
    MD_TableLoadEntry_t   Entry;

    Entry.Length = 0;

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidTableEntry_Test_SuccessDwellLengthZero */

void MD_ValidTableEntry_Test_ResolveError(void)
{
    int32                 Result;
    MD_TableLoadEntry_t   Entry;

    Entry.Length = 1;

    /* Set to make CFS_ResolveSymAddr return MD_RESOLVE_ERROR */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_RESOLVE_ERROR, "Result == MD_RESOLVE_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidTableEntry_Test_ResolveError */

void MD_ValidTableEntry_Test_InvalidAddress(void)
{
    int32                 Result;
    MD_TableLoadEntry_t   Entry;

    Entry.Length = 1;

    /* Set to make MD_ValidAddrRange return MD_INVALID_ADDR_ERROR */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_INVALID_ADDR_ERROR, "Result == MD_INVALID_ADDR_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidTableEntry_Test_InvalidAddress */

void MD_ValidTableEntry_Test_InvalidLength(void)
{
    int32                 Result;
    MD_TableLoadEntry_t   Entry;

    Entry.Length = -1;

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_INVALID_LEN_ERROR, "Result == MD_INVALID_LEN_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidTableEntry_Test_InvalidLength */

#if MD_SIGNATURE_OPTION == 1
void MD_ValidTableEntry_Test_NotAligned16DwellLength4(void)
{
    int32                 Result;
    MD_TableLoadEntry_t   Entry;

    Entry.Length = 4;
    Entry.DwellAddress.Offset = 1;

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_NOT_ALIGNED_ERROR, "Result == MD_NOT_ALIGNED_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidTableEntry_Test_NotAligned16DwellLength4 */
#endif

#if MD_SIGNATURE_OPTION == 0
void MD_ValidTableEntry_Test_NotAligned32(void)
{
    int32                 Result;
    MD_TableLoadEntry_t   Entry;

    Entry.Length = 4;
    Entry.DwellAddress.Offset = 1;

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_NOT_ALIGNED_ERROR, "Result == MD_NOT_ALIGNED_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidTableEntry_Test_NotAligned32 */
#endif

void MD_ValidTableEntry_Test_NotAligned16DwellLength2(void)
{
    int32                 Result;
    MD_TableLoadEntry_t   Entry;

    Entry.Length = 2;
    Entry.DwellAddress.Offset = 1;

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_NOT_ALIGNED_ERROR, "Result == MD_NOT_ALIGNED_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidTableEntry_Test_NotAligned16DwellLength2 */

void MD_ValidTableEntry_Test_ElseSuccess(void)
{
    int32                 Result;
    MD_TableLoadEntry_t   Entry;

    Entry.Length = 2;
    Entry.DwellAddress.Offset = 0;

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ValidTableEntry_Test_ElseSuccess */

void MD_CopyUpdatedTbl_Test(void)
{
    MD_DwellTableLoad_t   LoadTable;
    uint8                 TblIndex = 0;

    LoadTable.Enabled = 1;

#if MD_SIGNATURE_OPTION == 1
    strncpy(LoadTable.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    /* Execute the function being tested */
    MD_CopyUpdatedTbl(&LoadTable, TblIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[TblIndex].Enabled == 1, "MD_AppData.MD_DwellTables[TblIndex].Enabled == 1");

#if MD_SIGNATURE_OPTION == 1
    UtAssert_True
        (strncmp(MD_AppData.MD_DwellTables[TblIndex].Signature, "signature", MD_SIGNATURE_FIELD_LENGTH) == 0,
        "strncmp(MD_AppData.MD_DwellTables[TblIndex].Signature, 'signature', MD_SIGNATURE_FIELD_LENGTH) == 0");

    UtAssert_True(
        MD_AppData.MD_DwellTables[TblIndex].Signature[MD_SIGNATURE_FIELD_LENGTH - 1] == '\0',
        "MD_AppData.MD_DwellTables[TblIndex].Signature[MD_SIGNATURE_FIELD_LENGTH - 1] == '\0'");
#endif

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_CopyUpdatedTbl_Test */

void MD_UpdateTableEnabledField_Test_DwellStreamEnabled(void)
{
    uint16 TableIndex = 0;
    uint16 FieldValue = MD_DWELL_STREAM_ENABLED;

    /* Set MD_LoadTablePtr = &MD_DWELL_TBL_TEST_GlobalLoadTable */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_DWELL_TBL_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    MD_UpdateTableEnabledField(TableIndex, FieldValue);

    /* Verify results */
    UtAssert_True
        (MD_DWELL_TBL_TEST_GlobalLoadTable.Enabled == MD_DWELL_STREAM_ENABLED,
        "MD_DWELL_TBL_TEST_GlobalLoadTable.Enabled == MD_DWELL_STREAM_ENABLED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_UpdateTableEnabledField_Test_DwellStreamEnabled */

void MD_UpdateTableEnabledField_Test_DwellStreamDisabled(void)
{
    uint16 TableIndex = 0;
    uint16 FieldValue = MD_DWELL_STREAM_DISABLED;

    /* Set MD_LoadTablePtr = &MD_DWELL_TBL_TEST_GlobalLoadTable */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_DWELL_TBL_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    MD_UpdateTableEnabledField(TableIndex, FieldValue);

    /* Verify results */
    UtAssert_True
        (MD_DWELL_TBL_TEST_GlobalLoadTable.Enabled == MD_DWELL_STREAM_DISABLED,
        "MD_DWELL_TBL_TEST_GlobalLoadTable.Enabled == MD_DWELL_STREAM_DISABLED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_UpdateTableEnabledField_Test_DwellStreamDisabled */

void MD_UpdateTableDwellEntry_Test(void)
{
    uint16 TableIndex = 0;
    uint16 EntryIndex = 0;
    uint16 NewLength  = 1;
    uint16 NewDelay   = 1;
    CFS_SymAddr_t NewDwellAddress;

    NewDwellAddress.Offset = 1;

    strncpy(NewDwellAddress.SymName, "symname", 10);

    /* Set MD_LoadTablePtr = &MD_DWELL_TBL_TEST_GlobalLoadTable */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_DWELL_TBL_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    MD_UpdateTableDwellEntry(TableIndex, EntryIndex, NewLength, NewDelay, NewDwellAddress);

    /* Verify results */
    UtAssert_True (MD_DWELL_TBL_TEST_GlobalLoadTable.Entry[EntryIndex].Length == 1, "MD_DWELL_TBL_TEST_GlobalLoadTable.Entry[EntryIndex].Length == 1");
    UtAssert_True (MD_DWELL_TBL_TEST_GlobalLoadTable.Entry[EntryIndex].Delay == 1, "MD_DWELL_TBL_TEST_GlobalLoadTable.Entry[EntryIndex].Delay == 1");
    UtAssert_True (MD_DWELL_TBL_TEST_GlobalLoadTable.Entry[EntryIndex].DwellAddress.Offset == 1, "MD_DWELL_TBL_TEST_GlobalLoadTable.Entry[EntryIndex].DwellAddress.Offset == 1");

    UtAssert_True
        (strncmp(MD_DWELL_TBL_TEST_GlobalLoadTable.Entry[EntryIndex].DwellAddress.SymName, "symname", 10) == 0,
        "strncmp(MD_DWELL_TBL_TEST_GlobalLoadTable.Entry[EntryIndex].DwellAddress.SymName, 'symname', 10) == 0");

    UtAssert_True
        (MD_DWELL_TBL_TEST_GlobalLoadTable.Entry[EntryIndex].DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
        "MD_DWELL_TBL_TEST_GlobalLoadTable.Entry[EntryIndex].DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_UpdateTableDwellEntry_Test */

#if MD_SIGNATURE_OPTION == 1
void MD_UpdateTableSignature_Test(void)
{
    uint16 TableIndex = 0;

    /* Set MD_LoadTablePtr = &MD_DWELL_TBL_TEST_GlobalLoadTable */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_DWELL_TBL_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    MD_UpdateTableSignature(TableIndex, "newsignature");

    /* Verify results */
    UtAssert_True
        (strncmp(MD_DWELL_TBL_TEST_GlobalLoadTable.Signature, "newsignature", 15) == 0,
        "strncmp(MD_DWELL_TBL_TEST_GlobalLoadTable.Signature, 'newsignature', 15) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_UpdateTableSignature_Test */
#endif

void MD_Dwell_Tbl_Test_AddTestCases(void)
{
    UtTest_Add(MD_TableValidationFunc_Test_InvalidEnableFlag, MD_Test_Setup, MD_Test_TearDown, "MD_TableValidationFunc_Test_InvalidEnableFlag");
#if MD_SIGNATURE_OPTION == 1
    UtTest_Add(MD_TableValidationFunc_Test_InvalidSignatureLength, MD_Test_Setup, MD_Test_TearDown, "MD_TableValidationFunc_Test_InvalidSignatureLength");
#endif
    UtTest_Add(MD_TableValidationFunc_Test_ResolveError, MD_Test_Setup, MD_Test_TearDown, "MD_TableValidationFunc_Test_ResolveError");
    UtTest_Add(MD_TableValidationFunc_Test_InvalidAddress, MD_Test_Setup, MD_Test_TearDown, "MD_TableValidationFunc_Test_InvalidAddress");
    UtTest_Add(MD_TableValidationFunc_Test_InvalidLength, MD_Test_Setup, MD_Test_TearDown, "MD_TableValidationFunc_Test_InvalidLength");
    UtTest_Add(MD_TableValidationFunc_Test_NotAligned, MD_Test_Setup, MD_Test_TearDown, "MD_TableValidationFunc_Test_NotAligned");
    UtTest_Add(MD_TableValidationFunc_Test_ZeroRate, MD_Test_Setup, MD_Test_TearDown, "MD_TableValidationFunc_Test_ZeroRate");
    UtTest_Add(MD_TableValidationFunc_Test_Success, MD_Test_Setup, MD_Test_TearDown, "MD_TableValidationFunc_Test_Success");

    UtTest_Add(MD_ReadDwellTable_Test, MD_Test_Setup, MD_Test_TearDown, "MD_ReadDwellTable_Test");

    UtTest_Add(MD_CheckTableEntries_Test_Error, MD_Test_Setup, MD_Test_TearDown, "MD_CheckTableEntries_Test_Error");
    UtTest_Add(MD_CheckTableEntries_Test_Success, MD_Test_Setup, MD_Test_TearDown, "MD_CheckTableEntries_Test_Success");

    UtTest_Add(MD_ValidTableEntry_Test_SuccessDwellLengthZero, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableEntry_Test_SuccessDwellLengthZero");
    UtTest_Add(MD_ValidTableEntry_Test_ResolveError, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableEntry_Test_ResolveError");
    UtTest_Add(MD_ValidTableEntry_Test_InvalidAddress, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableEntry_Test_InvalidAddress");
    UtTest_Add(MD_ValidTableEntry_Test_InvalidLength, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableEntry_Test_InvalidLength");
#if MD_SIGNATURE_OPTION == 1
    UtTest_Add(MD_ValidTableEntry_Test_NotAligned16DwellLength4, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableEntry_Test_NotAligned16DwellLength4");
#endif

#if MD_SIGNATURE_OPTION == 0
    UtTest_Add(MD_ValidTableEntry_Test_NotAligned32, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableEntry_Test_NotAligned32");
#endif
    UtTest_Add(MD_ValidTableEntry_Test_NotAligned16DwellLength2, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableEntry_Test_NotAligned16DwellLength2");
    UtTest_Add(MD_ValidTableEntry_Test_ElseSuccess, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableEntry_Test_ElseSuccess");

    UtTest_Add(MD_CopyUpdatedTbl_Test, MD_Test_Setup, MD_Test_TearDown, "MD_CopyUpdatedTbl_Test");

    UtTest_Add(MD_UpdateTableEnabledField_Test_DwellStreamEnabled, MD_Test_Setup, MD_Test_TearDown, "MD_UpdateTableEnabledField_Test_DwellStreamEnabled");
    UtTest_Add(MD_UpdateTableEnabledField_Test_DwellStreamDisabled, MD_Test_Setup, MD_Test_TearDown, "MD_UpdateTableEnabledField_Test_DwellStreamDisabled");

    UtTest_Add(MD_UpdateTableDwellEntry_Test, MD_Test_Setup, MD_Test_TearDown, "MD_UpdateTableDwellEntry_Test");

#if MD_SIGNATURE_OPTION == 1
    UtTest_Add(MD_UpdateTableSignature_Test, MD_Test_Setup, MD_Test_TearDown, "MD_UpdateTableSignature_Test");
#endif

} /* end MD_Dwell_Tbl_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
