 /*************************************************************************
 ** File: md_dwell_pkt_test.c  
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
 **   This file contains unit test cases for the functions contained in the 
 **   file md_dwell_pkt.c
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

#include "md_dwell_pkt_test.h"
#include "md_dwell_pkt.h"
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
#include "ut_cfe_psp_ram_stubs.h"
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

void MD_DwellLoop_Test_PacketAlreadyFull(void)
{
    MD_AppData.MD_DwellTables[0].AddrCount = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].AddrCount = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].AddrCount = 1;

    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_ENABLED;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Enabled = MD_DWELL_STREAM_ENABLED;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Enabled = MD_DWELL_STREAM_ENABLED;

    MD_AppData.MD_DwellTables[0].Rate = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Rate = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Rate = 1;

    MD_AppData.MD_DwellTables[0].CurrentEntry = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry = 1;

    MD_AppData.MD_DwellTables[0].Entry[0].Delay = 2;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Entry[0].Delay = 2;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Entry[0].Delay = 2;

    /* Execute the function being tested */
    MD_DwellLoop();

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry == 0");

    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].PktOffset == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].PktOffset == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].PktOffset == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].PktOffset == 0");

    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 2, "MD_AppData.MD_DwellTables[0].Countdown == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown == 2, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown == 2, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown == 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_DwellLoop_Test_PacketAlreadyFull */

void MD_DwellLoop_Test_SendDwellPacket(void)
{
    MD_AppData.MD_DwellTables[0].AddrCount = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].AddrCount = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].AddrCount = 1;

    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_ENABLED;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Enabled = MD_DWELL_STREAM_ENABLED;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Enabled = MD_DWELL_STREAM_ENABLED;

    MD_AppData.MD_DwellTables[0].Rate = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Rate = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Rate = 1;

    MD_AppData.MD_DwellTables[0].CurrentEntry = 0;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry = 0;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry = 0;

    MD_AppData.MD_DwellTables[0].Entry[0].Delay = 2;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Entry[0].Delay = 2;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Entry[0].Delay = 2;

    MD_AppData.MD_DwellTables[0].Countdown = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown = 1;

    /* Execute the function being tested */
    MD_DwellLoop();

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry == 0");

    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].PktOffset == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].PktOffset == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].PktOffset == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].PktOffset == 0");

    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 2, "MD_AppData.MD_DwellTables[0].Countdown == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown == 2, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown == 2, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown == 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_DwellLoop_Test_SendDwellPacket */

void MD_DwellLoop_Test_MoreAddressesToRead(void)
{
    MD_AppData.MD_DwellTables[0].AddrCount = 2;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].AddrCount = 2;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].AddrCount = 2;

    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_ENABLED;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Enabled = MD_DWELL_STREAM_ENABLED;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Enabled = MD_DWELL_STREAM_ENABLED;

    MD_AppData.MD_DwellTables[0].Rate = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Rate = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Rate = 1;

    MD_AppData.MD_DwellTables[0].CurrentEntry = 0;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry = 0;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry = 0;

    MD_AppData.MD_DwellTables[0].Entry[0].Delay = 2;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Entry[0].Delay = 2;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Entry[0].Delay = 2;

    MD_AppData.MD_DwellTables[0].Countdown = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown = 1;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown = 1;

    /* Execute the function being tested */
    MD_DwellLoop();

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 2, "MD_AppData.MD_DwellTables[0].Countdown == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown == 2, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown == 2, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown == 2");

    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 1, "MD_AppData.MD_DwellTables[0].CurrentEntry == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry == 1, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry == 1, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_DwellLoop_Test_MoreAddressesToRead */

void MD_GetDwellData_Test_MemRead8Error(void)
{
    int32    Result;
    uint16   TblIndex   = 1;
    uint16   EntryIndex = 1;

    MD_AppData.MD_DwellTables[TblIndex].Entry[EntryIndex].Length = 1;
    MD_AppData.MD_DwellTables[TblIndex].PktOffset = 0;

    /* Cause Status to be set to -1 */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMREAD8_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_GetDwellData(TblIndex, EntryIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[TblIndex].PktOffset == 1, "MD_AppData.MD_DwellTables[TblIndex].PktOffset == 1");

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_GetDwellData_Test_MemRead8Error */

void MD_GetDwellData_Test_MemRead16Error(void)
{
    int32    Result;
    uint16   TblIndex   = 1;
    uint16   EntryIndex = 1;

    MD_AppData.MD_DwellTables[TblIndex].Entry[EntryIndex].Length = 2;
    MD_AppData.MD_DwellTables[TblIndex].PktOffset = 0;

    /* Cause Status to be set to -1 */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMREAD16_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_GetDwellData(TblIndex, EntryIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[TblIndex].PktOffset == 2, "MD_AppData.MD_DwellTables[TblIndex].PktOffset == 2");

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_GetDwellData_Test_MemRead16Error */

void MD_GetDwellData_Test_MemRead32Error(void)
{
    int32    Result;
    uint16   TblIndex   = 1;
    uint16   EntryIndex = 1;

    MD_AppData.MD_DwellTables[TblIndex].Entry[EntryIndex].Length = 4;
    MD_AppData.MD_DwellTables[TblIndex].PktOffset = 0;

    /* Cause Status to be set to -1 */
    Ut_CFE_PSP_RAM_SetReturnCode(UT_CFE_PSP_RAM_MEMREAD32_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_GetDwellData(TblIndex, EntryIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[TblIndex].PktOffset == 4, "MD_AppData.MD_DwellTables[TblIndex].PktOffset == 4");

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_GetDwellData_Test_MemRead32Error */

void MD_GetDwellData_Test_InvalidDwellLength(void)
{
    int32    Result;
    uint16   TblIndex   = 1;
    uint16   EntryIndex = 1;

    MD_AppData.MD_DwellTables[TblIndex].Entry[EntryIndex].Length = 5;
    MD_AppData.MD_DwellTables[TblIndex].PktOffset = 0;

    /* Execute the function being tested */
    Result = MD_GetDwellData(TblIndex, EntryIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[TblIndex].PktOffset == 5, "MD_AppData.MD_DwellTables[TblIndex].PktOffset == 5");

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_GetDwellData_Test_InvalidDwellLength */

void MD_GetDwellData_Test_Success(void)
{
    int32    Result;
    uint16   TblIndex   = 1;
    uint16   EntryIndex = 1;

    MD_AppData.MD_DwellTables[TblIndex].Entry[EntryIndex].Length = 1;
    MD_AppData.MD_DwellTables[TblIndex].PktOffset = 0;

    /* Execute the function being tested */
    Result = MD_GetDwellData(TblIndex, EntryIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[TblIndex].PktOffset == 1, "MD_AppData.MD_DwellTables[TblIndex].PktOffset == 1");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_GetDwellData_Test_Success */

void MD_SendDwellPkt_Test(void)
{
    uint16   TableIndex   = 1;
    MD_AppData.MD_DwellTables[TableIndex].AddrCount = 3;
    MD_AppData.MD_DwellTables[TableIndex].Rate = 4;
    MD_AppData.MD_DwellTables[TableIndex].DataSize = 5;

#if MD_SIGNATURE_OPTION == 1
    strncpy(MD_AppData.MD_DwellTables[TableIndex].Signature, "signature", MD_SIGNATURE_FIELD_LENGTH - 1);
#endif

    /* Execute the function being tested */
    MD_SendDwellPkt(TableIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellPkt[TableIndex].TableId == 2, "MD_AppData.MD_DwellPkt[TableIndex].TableId == 2");
    UtAssert_True(MD_AppData.MD_DwellPkt[TableIndex].AddrCount == 3, "MD_AppData.MD_DwellPkt[TableIndex].AddrCount == 3");
    UtAssert_True(MD_AppData.MD_DwellPkt[TableIndex].Rate == 4, "MD_AppData.MD_DwellPkt[TableIndex].Rate == 4");

#if MD_SIGNATURE_OPTION == 1
    UtAssert_True
        (strncmp(MD_AppData.MD_DwellTables[TableIndex].Signature, "signature", MD_SIGNATURE_FIELD_LENGTH - 1) == 0,
        "MD_AppData.MD_DwellTables[TableIndex].Signature, 'signature', MD_SIGNATURE_FIELD_LENGTH - 1) == 0");

    UtAssert_True
        (MD_AppData.MD_DwellTables[TableIndex].Signature[MD_SIGNATURE_FIELD_LENGTH - 1] == '\0',
        "MD_AppData.MD_DwellTables[TableIndex].Signature[MD_SIGNATURE_FIELD_LENGTH - 1] == '\0'");
#endif

    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].DataSize == 5, "MD_AppData.MD_DwellTables[TableIndex].DataSize == 5");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_SendDwellPkt_Test */

void MD_StartDwellStream_Test(void)
{
    uint16   TableIndex   = 1;

    /* Execute the function being tested */
    MD_StartDwellStream(TableIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].Countdown == 1, "MD_AppData.MD_DwellTables[TableIndex].Countdown == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].CurrentEntry == 0, "MD_AppData.MD_DwellTables[TableIndex].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].PktOffset == 0, "MD_AppData.MD_DwellTables[TableIndex].PktOffset == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_StartDwellStream_Test */

void MD_Dwell_Pkt_Test_AddTestCases(void)
{
    UtTest_Add(MD_DwellLoop_Test_PacketAlreadyFull, MD_Test_Setup, MD_Test_TearDown, "MD_DwellLoop_Test_PacketAlreadyFull");
    UtTest_Add(MD_DwellLoop_Test_SendDwellPacket, MD_Test_Setup, MD_Test_TearDown, "MD_DwellLoop_Test_SendDwellPacket");
    UtTest_Add(MD_DwellLoop_Test_MoreAddressesToRead, MD_Test_Setup, MD_Test_TearDown, "MD_DwellLoop_Test_MoreAddressesToRead");

    UtTest_Add(MD_GetDwellData_Test_MemRead8Error, MD_Test_Setup, MD_Test_TearDown, "MD_GetDwellData_Test_MemRead8Error");
    UtTest_Add(MD_GetDwellData_Test_MemRead16Error, MD_Test_Setup, MD_Test_TearDown, "MD_GetDwellData_Test_MemRead16Error");
    UtTest_Add(MD_GetDwellData_Test_MemRead32Error, MD_Test_Setup, MD_Test_TearDown, "MD_GetDwellData_Test_MemRead32Error");
    UtTest_Add(MD_GetDwellData_Test_InvalidDwellLength, MD_Test_Setup, MD_Test_TearDown, "MD_GetDwellData_Test_InvalidDwellLength");
    UtTest_Add(MD_GetDwellData_Test_Success, MD_Test_Setup, MD_Test_TearDown, "MD_GetDwellData_Test_Success");

    UtTest_Add(MD_SendDwellPkt_Test, MD_Test_Setup, MD_Test_TearDown, "MD_SendDwellPkt_Test");

    UtTest_Add(MD_StartDwellStream_Test, MD_Test_Setup, MD_Test_TearDown, "MD_StartDwellStream_Test");

} /* end MD_Dwell_Pkt_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
