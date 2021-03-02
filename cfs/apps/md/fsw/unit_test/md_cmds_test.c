 /*************************************************************************
 ** File: md_cmds_test.c 
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
 **   This file contains unit test cases for the functions contained in the file md_cmds.c
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

#include "md_cmds_test.h"
#include "md_cmds.h"
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

int32 MD_CMDS_TEST_CFE_TBL_GetAddressHook( void **TblPtr, CFE_TBL_Handle_t TblHandle )
{
    *TblPtr = &MD_AppData.MD_DwellTables[0];

    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_ENABLED;

    /* Return value doesn't matter in this case */
    return CFE_TBL_INFO_UPDATED;
}

void MD_ProcessStartCmd_Test_ZeroRate(void)
{
    MD_CmdStartStop_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdStartStop_t), TRUE);

    CmdPacket.TableMask = 1;

    MD_AppData.MD_DwellTables[0].Rate = 0;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_CMDS_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    MD_ProcessStartCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED, "MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 1, "MD_AppData.MD_DwellTables[0].Countdown == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_ZERO_RATE_CMD_INF_EID, CFE_EVS_INFORMATION, "Dwell Table 1 is enabled with a delay of zero so no processing will occur"),
        "Dwell Table 1 is enabled with a delay of zero so no processing will occur");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_START_DWELL_INF_EID,  CFE_EVS_INFORMATION, "Start Dwell Table command processed successfully for table mask 0x0001"),
        "Start Dwell Table command processed successfully for table mask 0x0001");


    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end MD_ProcessStartCmd_Test_ZeroRate */

void MD_ProcessStartCmd_Test_Success(void)
{
    MD_CmdStartStop_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdStartStop_t), TRUE);

    CmdPacket.TableMask = 1;

    MD_AppData.MD_DwellTables[0].Rate = 1;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_CMDS_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    MD_ProcessStartCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED, "MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 1, "MD_AppData.MD_DwellTables[0].Countdown == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_START_DWELL_INF_EID,  CFE_EVS_INFORMATION, "Start Dwell Table command processed successfully for table mask 0x0001"),
        "Start Dwell Table command processed successfully for table mask 0x0001");


    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessStartCmd_Test_Success */

void MD_ProcessStartCmd_Test_EmptyTableMask(void)
{
    MD_CmdStartStop_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdStartStop_t), TRUE);

    CmdPacket.TableMask = 0;

    /* Execute the function being tested */
    MD_ProcessStartCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_EMPTY_TBLMASK_ERR_EID, CFE_EVS_ERROR, "Start Dwell command rejected because no tables were specified in table mask (0x0000)"),
        "Start Dwell command rejected because no tables were specified in table mask (0x0000)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessStartCmd_Test_EmptyTableMask */

void MD_ProcessStopCmd_Test_Success(void)
{
    MD_CmdStartStop_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdStartStop_t), TRUE);

    CmdPacket.TableMask = 1;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_CMDS_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    MD_ProcessStopCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_DISABLED, "MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 0, "MD_AppData.MD_DwellTables[0].Countdown == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_STOP_DWELL_INF_EID,  CFE_EVS_INFORMATION, "Stop Dwell Table command processed successfully for table mask 0x0001"),
        "Stop Dwell Table command processed successfully for table mask 0x0001");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessStopCmd_Test_Success */

void MD_ProcessStopCmd_Test_EmptyTableMask(void)
{
    MD_CmdStartStop_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdStartStop_t), TRUE);

    CmdPacket.TableMask = 0;

    /* Execute the function being tested */
    MD_ProcessStopCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_EMPTY_TBLMASK_ERR_EID, CFE_EVS_ERROR, "Stop Dwell command rejected because no tables were specified in table mask (0x0000)"),
        "Stop Dwell command rejected because no tables were specified in table mask (0x0000)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessStopCmd_Test_EmptyTableMask */

void MD_ProcessJamCmd_Test_InvalidJamTable(void)
{
    MD_CmdJam_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdJam_t), TRUE);

    CmdPacket.TableId = 99;
    CmdPacket.EntryId = 2;

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0', "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0'");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_INVALID_JAM_TABLE_ERR_EID, CFE_EVS_ERROR, "Jam Cmd rejected due to invalid Tbl Id arg = 99 (Expect 1.. 4)"),
        "Jam Cmd rejected due to invalid Tbl Id arg = 99 (Expect 1.. 4)");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessJamCmd_Test_InvalidJamTable */

void MD_ProcessJamCmd_Test_InvalidEntryArg(void)
{
    MD_CmdJam_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdJam_t), TRUE);

    CmdPacket.TableId = 1;
    CmdPacket.EntryId = 99;

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0', "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0'");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_INVALID_ENTRY_ARG_ERR_EID, CFE_EVS_ERROR, "Jam Cmd rejected due to invalid Entry Id arg = 99 (Expect 1.. 25)"),
        "Jam Cmd rejected due to invalid Entry Id arg = 99 (Expect 1.. 25)");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessJamCmd_Test_InvalidEntryArg */

void MD_ProcessJamCmd_Test_SuccessNullZeroRate(void)
{
    MD_CmdJam_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdJam_t), TRUE);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 0;

    MD_AppData.MD_DwellTables[0].Rate = 1;

    /* Prevents segmentation fault in call to subfunction MD_UpdateDwellControlInfo */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_CMDS_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0', "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0'");

    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0, "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 0, "MD_AppData.MD_DwellTables[0].Entry[1].Length == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Delay == 0, "MD_AppData.MD_DwellTables[0].Entry[1].Delay == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_JAM_NULL_DWELL_INF_EID, CFE_EVS_INFORMATION, "Successful Jam of a Null Dwell Entry to Dwell Tbl#1 Entry #2"),
        "Successful Jam of a Null Dwell Entry to Dwell Tbl#1 Entry #2");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_ZERO_RATE_CMD_INF_EID, CFE_EVS_INFORMATION, "Dwell Table 1 is enabled with a delay of zero so no processing will occur"),
        "Dwell Table 1 is enabled with a delay of zero so no processing will occur");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end MD_ProcessJamCmd_Test_SuccessNullZeroRate */

void MD_ProcessJamCmd_Test_CantResolveJamAddr(void)
{
    MD_CmdJam_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdJam_t), TRUE);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 1;

    strncpy(CmdPacket.DwellAddress.SymName, "address", 10);

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == FALSE" */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX, -1, 1);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0', "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0'");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_CANT_RESOLVE_JAM_ADDR_ERR_EID, CFE_EVS_ERROR, "Jam Cmd rejected because symbolic address 'address' couldn't be resolved"),
        "Jam Cmd rejected because symbolic address 'address' couldn't be resolved");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessJamCmd_Test_CantResolveJamAddr */

void MD_ProcessJamCmd_Test_InvalidLenArg(void)
{
    MD_CmdJam_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdJam_t), TRUE);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 5;

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0', "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0'");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_INVALID_LEN_ARG_ERR_EID, CFE_EVS_ERROR, "Jam Cmd rejected due to invalid Field Length arg = 5 (Expect 0,1,2,or 4)"),
        "Jam Cmd rejected due to invalid Field Length arg = 5 (Expect 0,1,2,or 4)");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessJamCmd_Test_InvalidLenArg */

void MD_ProcessJamCmd_Test_InvalidJamAddr(void)
{
    MD_CmdJam_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdJam_t), TRUE);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 4;

    /* Set to satisfy condition "!MD_ValidAddrRange(ResolvedAddr, Jam->FieldLength)" */
    Ut_CFE_PSP_MEMRANGE_SetReturnCode(UT_CFE_PSP_MEMRANGE_MEMVALIDATERANGE_INDEX, -1, 1);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0', "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0'");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_INVALID_JAM_ADDR_ERR_EID, CFE_EVS_ERROR, "Jam Cmd rejected because address 0x00000000 is not in a valid range"),
        "Jam Cmd rejected because address 0x00000000 is not in a valid range");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessJamCmd_Test_InvalidJamAddr */

#if MD_ENFORCE_DWORD_ALIGN == 0
void MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength4(void)
{
    MD_CmdJam_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdJam_t), TRUE);

    CmdPacket.TableId             = 1;
    CmdPacket.EntryId             = 2;
    CmdPacket.FieldLength         = 4;
    CmdPacket.DwellAddress.Offset = 1;

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0', "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0'");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_JAM_ADDR_NOT_16BIT_ERR_EID, CFE_EVS_ERROR, "Jam Cmd rejected because address 0x00000001 is not 16-bit aligned"),
        "Jam Cmd rejected because address 0x00000001 is not 16-bit aligned");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength4 */
#endif

#if MD_ENFORCE_DWORD_ALIGN == 1
void MD_ProcessJamCmd_Test_JamAddrNot32Bit(void)
{
    MD_CmdJam_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdJam_t), TRUE);

    CmdPacket.TableId             = 1;
    CmdPacket.EntryId             = 2;
    CmdPacket.FieldLength         = 4;
    CmdPacket.DwellAddress.Offset = 1;

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0', "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0'");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_JAM_ADDR_NOT_32BIT_ERR_EID, CFE_EVS_ERROR, "Jam Cmd rejected because address 0x00000001 is not 32-bit aligned"),
        "Jam Cmd rejected because address 0x00000001 is not 32-bit aligned");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessJamCmd_Test_JamAddrNot32Bit */
#endif

void MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength2(void)
{
    MD_CmdJam_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdJam_t), TRUE);

    CmdPacket.TableId             = 1;
    CmdPacket.EntryId             = 2;
    CmdPacket.FieldLength         = 2;
    CmdPacket.DwellAddress.Offset = 1;

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0', "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0'");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_JAM_ADDR_NOT_16BIT_ERR_EID, CFE_EVS_ERROR, "Jam Cmd rejected because address 0x00000001 is not 16-bit aligned"),
        "Jam Cmd rejected because address 0x00000001 is not 16-bit aligned");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength2 */

void MD_ProcessJamCmd_Test_SuccessNonNullZeroRate(void)
{
    MD_CmdJam_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdJam_t), TRUE);

    CmdPacket.TableId             = 1;
    CmdPacket.EntryId             = 2;
    CmdPacket.FieldLength         = 2;
    CmdPacket.DwellDelay          = 3;

    MD_AppData.MD_DwellTables[0].Entry[1].Delay = 1;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableDwellEntry */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_CMDS_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0', "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0'");

    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0, "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 2, "MD_AppData.MD_DwellTables[0].Entry[1].Length == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Delay  == 3, "MD_AppData.MD_DwellTables[0].Entry[1].Delay  == 3");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_JAM_DWELL_INF_EID, CFE_EVS_INFORMATION, "Successful Jam to Dwell Tbl#1 Entry #2"),
        "Successful Jam to Dwell Tbl#1 Entry #2");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_ZERO_RATE_CMD_INF_EID, CFE_EVS_INFORMATION, "Dwell Table 1 is enabled with a delay of zero so no processing will occur"),
        "Dwell Table 1 is enabled with a delay of zero so no processing will occur");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end MD_ProcessJamCmd_Test_SuccessNonNullZeroRate */

#if MD_SIGNATURE_OPTION == 1
void MD_ProcessSignatureCmd_Test_InvalidSignatureLength(void)
{
    uint16   i;
    MD_CmdSetSignature_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdSetSignature_t), TRUE);

    for (i = 0; i < MD_SIGNATURE_FIELD_LENGTH; i++)
    {
        CmdPacket.Signature[i] = 'x';
    }

    /* Execute the function being tested */
    MD_ProcessSignatureCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_INVALID_SIGNATURE_LENGTH_ERR_EID, CFE_EVS_ERROR, "Set Signature cmd rejected due to invalid Signature length"),
        "Set Signature cmd rejected due to invalid Signature length");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessSignatureCmd_Test_InvalidSignatureLength */
#endif

#if MD_SIGNATURE_OPTION == 1
void MD_ProcessSignatureCmd_Test_InvalidSignatureTable(void)
{
    MD_CmdSetSignature_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdSetSignature_t), TRUE);

    CmdPacket.TableId = 0;

    /* Execute the function being tested */
    MD_ProcessSignatureCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_INVALID_SIGNATURE_TABLE_ERR_EID, CFE_EVS_ERROR, "Set Signature cmd rejected due to invalid Tbl Id arg = 0 (Expect 1.. 4)"),
        "Set Signature cmd rejected due to invalid Tbl Id arg = 0 (Expect 1.. 4)");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessSignatureCmd_Test_InvalidSignatureTable */
#endif

#if MD_SIGNATURE_OPTION == 1
void MD_ProcessSignatureCmd_Test_Success(void)
{
    MD_CmdSetSignature_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdSetSignature_t), TRUE);

    CmdPacket.TableId = 1;
    strncpy(CmdPacket.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableSignature */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_CMDS_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    MD_ProcessSignatureCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (strncmp(MD_AppData.MD_DwellTables[0].Signature, "signature", MD_SIGNATURE_FIELD_LENGTH) == 0,
        "strncmp(MD_AppData.MD_DwellTables[0].Signature, 'signature', MD_SIGNATURE_FIELD_LENGTH) == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_SET_SIGNATURE_INF_EID, CFE_EVS_INFORMATION, "Successfully set signature for Dwell Tbl#1 to 'signature'"),
        "Successfully set signature for Dwell Tbl#1 to 'signature'");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ProcessSignatureCmd_Test_Success */
#endif

void MD_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(MD_ProcessStartCmd_Test_ZeroRate, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessStartCmd_Test_ZeroRate");
    UtTest_Add(MD_ProcessStartCmd_Test_Success, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessStartCmd_Test_Success");
    UtTest_Add(MD_ProcessStartCmd_Test_EmptyTableMask, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessStartCmd_Test_EmptyTableMask");

    UtTest_Add(MD_ProcessStopCmd_Test_Success, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessStopCmd_Test_Success");
    UtTest_Add(MD_ProcessStopCmd_Test_EmptyTableMask, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessStopCmd_Test_EmptyTableMask");

    UtTest_Add(MD_ProcessJamCmd_Test_InvalidJamTable, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessJamCmd_Test_InvalidJamTable");
    UtTest_Add(MD_ProcessJamCmd_Test_InvalidEntryArg, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessJamCmd_Test_InvalidEntryArg");
    UtTest_Add(MD_ProcessJamCmd_Test_SuccessNullZeroRate, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessJamCmd_Test_SuccessNullZeroRate");
    UtTest_Add(MD_ProcessJamCmd_Test_CantResolveJamAddr, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessJamCmd_Test_CantResolveJamAddr");
    UtTest_Add(MD_ProcessJamCmd_Test_InvalidLenArg, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessJamCmd_Test_InvalidLenArg");
    UtTest_Add(MD_ProcessJamCmd_Test_InvalidJamAddr, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessJamCmd_Test_InvalidJamAddr");
#if MD_ENFORCE_DWORD_ALIGN == 0
    UtTest_Add(MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength4, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength4");
#endif

#if MD_ENFORCE_DWORD_ALIGN == 1
    UtTest_Add(MD_ProcessJamCmd_Test_JamAddrNot32Bit, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessJamCmd_Test_JamAddrNot32Bit");
#endif
    UtTest_Add(MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength2, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength2");
    UtTest_Add(MD_ProcessJamCmd_Test_SuccessNonNullZeroRate, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessJamCmd_Test_SuccessNonNullZeroRate");

#if MD_SIGNATURE_OPTION == 1
    UtTest_Add(MD_ProcessSignatureCmd_Test_InvalidSignatureLength, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessSignatureCmd_Test_InvalidSignatureLength");
    UtTest_Add(MD_ProcessSignatureCmd_Test_InvalidSignatureTable, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessSignatureCmd_Test_InvalidSignatureTable");
    UtTest_Add(MD_ProcessSignatureCmd_Test_Success, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessSignatureCmd_Test_Success");
#endif

} /* end MD_Cmds_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
