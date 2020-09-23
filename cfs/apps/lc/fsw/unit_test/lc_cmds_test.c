 /*************************************************************************
 ** File:
 **   $Id: lc_cmds_test.c 1.6 2017/05/07 23:59:04EDT mdeschu Exp  $
 **
 **  Copyright (c) 2007-2014 United States Government as represented by the 
 **  Administrator of the National Aeronautics and Space Administration. 
 **  All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file lc_cmds.c
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

#include "lc_cmds_test.h"
#include "lc_cmds.h"
#include "lc_app.h"
#include "lc_msg.h"
#include "lc_msgdefs.h"
#include "lc_msgids.h"
#include "lc_events.h"
#include "lc_version.h"
#include "lc_test_utils.h"
#include "ut_osapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
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

uint16 LC_CMDS_TEST_CFE_TBL_GetAddressHookCount;
int32 LC_CMDS_TEST_CFE_TBL_GetAddressHook(void **TblPtr, CFE_TBL_Handle_t TblHandle)
{
    LC_CMDS_TEST_CFE_TBL_GetAddressHookCount++;

    if (LC_CMDS_TEST_CFE_TBL_GetAddressHookCount == 1)
        return CFE_TBL_INFO_UPDATED;
    else
        return -1;
}

void LC_AppPipe_Test_SampleAPRequest(void)
{
    int32               Result;
    LC_NoArgsCmd_t      CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, LC_SAMPLE_AP_MID, 1, TRUE);

    /* Execute the function being tested */
    Result = LC_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_AppPipe_Test_SampleAPRequest */

void LC_AppPipe_Test_HousekeepingRequest(void)
{
    int32               Result;
    LC_NoArgsCmd_t      CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, LC_SEND_HK_MID, 1, TRUE);

    /* Set to prevent unintended error messages in subfunctions */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_AppPipe_Test_HousekeepingRequest */

void LC_AppPipe_Test_Noop(void)
{
    int32               Result;
    LC_NoArgsCmd_t      CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), LC_NOOP_CC);

    /* Execute the function being tested */
    Result = LC_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    /* Note: this event message is generated in a subfunction, but is tested anyway to verify that the correct code path has been reached */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_NOOP_INF_EID, CFE_EVS_INFORMATION, "No-op command: Version 2.1.0.0"),
        "No-op command: Version 2.1.0.0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_AppPipe_Test_Noop */

void LC_AppPipe_Test_Reset(void)
{
    int32              Result;
    LC_NoArgsCmd_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), LC_RESET_CC);

    /* Execute the function being tested */
    Result = LC_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    /* Note: this event message is generated in a subfunction, but is tested anyway to verify that the correct code path has been reached */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_RESET_DBG_EID, CFE_EVS_DEBUG, "Reset counters command"),
        "Reset counters command");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_AppPipe_Test_Reset */

void LC_AppPipe_Test_SetLCState(void)
{
    int32               Result;
    LC_SetLCState_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetLCState_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SET_LC_STATE_CC);

    /* Execute the function being tested */
    Result = LC_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_AppPipe_Test_SetLCState */

void LC_AppPipe_Test_SetAPState(void)
{
    int32               Result;
    LC_SetAPState_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SET_AP_STATE_CC);

    /* Execute the function being tested */
    Result = LC_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_AppPipe_Test_SetAPState */

void LC_AppPipe_Test_SetAPPermoff(void)
{
    int32                 Result;
    LC_SetAPPermOff_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPPermOff_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SET_AP_PERMOFF_CC);

    /* Execute the function being tested */
    Result = LC_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_AppPipe_Test_SetAPPermoff */

void LC_AppPipe_Test_ResetAPStats(void)
{
    int32                 Result;
    LC_ResetAPStats_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_ResetAPStats_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), LC_RESET_AP_STATS_CC);

    /* Execute the function being tested */
    Result = LC_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_AppPipe_Test_ResetAPStats */

void LC_AppPipe_Test_ResetWPStats(void)
{
    int32                 Result;
    LC_SetAPPermOff_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_ResetWPStats_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), LC_RESET_WP_STATS_CC);

    /* Execute the function being tested */
    Result = LC_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_AppPipe_Test_ResetWPStats */

void LC_AppPipe_Test_InvalidCommandCode(void)
{
    int32                 Result;
    LC_SetAPPermOff_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), 99);

    /* Execute the function being tested */
    Result = LC_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_CC_ERR_EID, CFE_EVS_ERROR, "Invalid command code: ID = 0x18A4, CC = 99"),
        "Invalid command code: ID = 0x18A4, CC = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_AppPipe_Test_InvalidCommandCode */

void LC_AppPipe_Test_MonitorPacket(void)
{
    int32                 Result;
    LC_SetAPPermOff_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 99, 1, TRUE);

    /* Execute the function being tested */
    Result = LC_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_AppPipe_Test_MonitorPacket */

void LC_SampleAPReq_Test_AllowSampleAllWatchStale(void)
{
    LC_SampleAP_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SAMPLE_AP_MID, sizeof(LC_SampleAP_t), TRUE);

    LC_AppData.CurrentLCState = 99;
    CmdPacket.StartIndex      = CmdPacket.EndIndex = LC_ALL_ACTIONPOINTS;
    CmdPacket.UpdateAge       = 1;
    LC_OperData.WRTPtr[0].CountdownToStale = 1;

    /* Execute the function being tested */
    LC_SampleAPReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_OperData.WRTPtr[0].WatchResult == LC_WATCH_STALE, "LC_OperData.WRTPtr[0].WatchResult == LC_WATCH_STALE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SampleAPReq_Test_AllowSampleAllWatchStale */

void LC_SampleAPReq_Test_StartLessOrEqualToEndAndEndWithinArrayWatchStale(void)
{
    LC_SampleAP_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SAMPLE_AP_MID, sizeof(LC_SampleAP_t), TRUE);

    LC_AppData.CurrentLCState = 99;
    CmdPacket.StartIndex      = CmdPacket.EndIndex = LC_MAX_ACTIONPOINTS - 1;
    CmdPacket.UpdateAge       = 1;
    LC_OperData.WRTPtr[0].CountdownToStale = 1;

    /* Execute the function being tested */
    LC_SampleAPReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_OperData.WRTPtr[0].WatchResult == LC_WATCH_STALE, "LC_OperData.WRTPtr[0].WatchResult == LC_WATCH_STALE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SampleAPReq_Test_StartLessOrEqualToEndAndEndWithinArrayWatchStale */

void LC_SampleAPReq_Test_ArrayIndexOutOfRange(void)
{
    LC_SampleAP_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SAMPLE_AP_MID, sizeof(LC_SampleAP_t), TRUE);

    LC_AppData.CurrentLCState = 99;
    CmdPacket.StartIndex      = 2;
    CmdPacket.EndIndex        = 1;

    /* Execute the function being tested */
    LC_SampleAPReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSAMPLE_APNUM_ERR_EID, CFE_EVS_ERROR, "Sample AP error: invalid AP number, start = 2, end = 1"),
        "Sample AP error: invalid AP number, start = 2, end = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SampleAPReq_Test_ArrayIndexOutOfRange */

void LC_HousekeepingReq_Test_WatchStale(void)
{
    int32             Result;
    uint16            TableIndex;
    uint16            HKIndex;
    uint8             ExpectedByteData;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_STALE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_STALE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_STALE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_STALE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_STALE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS/2) / 4;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_HousekeepingReq_Test_WatchStale */

void LC_HousekeepingReq_Test_WatchFalse(void)
{
    int32             Result;
    uint16            TableIndex;
    uint16            HKIndex;
    uint8             ExpectedByteData;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_FALSE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_FALSE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_FALSE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_FALSE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_FALSE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS/2) / 4;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_HousekeepingReq_Test_WatchFalse */

void LC_HousekeepingReq_Test_WatchTrue(void)
{
    int32             Result;
    uint16            TableIndex;
    uint16            HKIndex;
    uint8             ExpectedByteData;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_TRUE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_TRUE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_TRUE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_TRUE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_TRUE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS/2) / 4;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_HousekeepingReq_Test_WatchTrue */

void LC_HousekeepingReq_Test_WatchError(void)
{
    int32             Result;
    uint16            TableIndex;
    uint16            HKIndex;
    uint8             ExpectedByteData;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = LC_WATCH_ERROR;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_ERROR << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS/2) / 4;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_HousekeepingReq_Test_WatchError */

void LC_HousekeepingReq_Test_DefaultWatchResult(void)
{
    int32             Result;
    uint16            TableIndex;
    uint16            HKIndex;
    uint8             ExpectedByteData;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = 99;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKWR_ERROR << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKWR_ERROR << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKWR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_WATCHPOINTS/2) / 4;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_WATCHPOINTS - 1) / 4;
    UtAssert_True (LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.WPResults[HKIndex] == ExpectedByteData");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_HousekeepingReq_Test_DefaultWatchResult */

void LC_HousekeepingReq_Test_ActionNotUsedStale(void)
{
    int32             Result;
    uint16            TableIndex;
    uint16            HKIndex;
    uint8             ExpectedByteData;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_ACTION_NOT_USED;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_STALE;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_NOT_USED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STALE << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_NOT_USED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_STALE);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS/2) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_HousekeepingReq_Test_ActionNotUsedStale */

void LC_HousekeepingReq_Test_APStateActiveActionPass(void)
{
    int32             Result;
    uint16            TableIndex;
    uint16            HKIndex;
    uint8             ExpectedByteData;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_ACTIVE;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_PASS;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_ACTIVE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_PASS << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_ACTIVE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_PASS);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS/2) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    UtAssert_True (LC_OperData.HkPacket.ActiveAPs = LC_MAX_ACTIONPOINTS, "LC_OperData.HkPacket.ActiveAPs = LC_MAX_ACTIONPOINTS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_HousekeepingReq_Test_APStateActiveActionPass */

void LC_HousekeepingReq_Test_APStatePassiveActionFail(void)
{
    int32             Result;
    uint16            TableIndex;
    uint16            HKIndex;
    uint8             ExpectedByteData;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_PASSIVE;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_FAIL;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_PASSIVE << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_FAIL << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_PASSIVE << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_FAIL);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS/2) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_HousekeepingReq_Test_APStatePassiveActionFail */

void LC_HousekeepingReq_Test_APStateDisabledActionError(void)
{
    int32             Result;
    uint16            TableIndex;
    uint16            HKIndex;
    uint8             ExpectedByteData;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_DISABLED;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_ERROR;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_DISABLED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_DISABLED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS/2) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_HousekeepingReq_Test_APStateDisabledActionError */

void LC_HousekeepingReq_Test_APStatePermOffActionError(void)
{
    int32             Result;
    uint16            TableIndex;
    uint16            HKIndex;
    uint8             ExpectedByteData;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = LC_APSTATE_PERMOFF;
        LC_OperData.ARTPtr[TableIndex].ActionResult = LC_ACTION_ERROR;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_NOT_USED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_NOT_USED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS/2) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_HousekeepingReq_Test_APStatePermOffActionError */

void LC_HousekeepingReq_Test_DefaultCurrentStateAndActionResult(void)
{
    int32             Result;
    uint16            TableIndex;
    uint16            HKIndex;
    uint8             ExpectedByteData;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
        LC_OperData.ARTPtr[TableIndex].ActionResult = 99;
    }

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    ExpectedByteData = LC_HKAR_STATE_NOT_USED << 6;
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_ERROR << 4));
    ExpectedByteData = (ExpectedByteData | (LC_HKAR_STATE_NOT_USED << 2));
    ExpectedByteData = (ExpectedByteData | LC_HKAR_ERROR);

    /* Checks for first index: */
    HKIndex = 0;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for middle index: */
    HKIndex = (LC_MAX_ACTIONPOINTS/2) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    /* Checks for last index: */
    HKIndex = (LC_MAX_ACTIONPOINTS - 1) / 2;
    UtAssert_True (LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData, "LC_OperData.HkPacket.APResults[HKIndex] == ExpectedByteData");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_HousekeepingReq_Test_DefaultCurrentStateAndActionResult */

void LC_HousekeepingReq_Test_ManageTablesError(void)
{
    int32             Result;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    /* Set to satisfy condition "(Result = LC_ManageTables()) != CFE_SUCCESS" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_HousekeepingReq_Test_ManageTablesError */

void LC_HousekeepingReq_Test_UpdateTaskCDSNotSuccess(void)
{
    int32             Result;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_SEND_HK_MID, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_OperData.HaveActiveCDS = TRUE;

    /* Set to prevent an unintended error message in subfunction LC_ManageTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to satisfy condition "LC_UpdateTaskCDS() != CFE_SUCCESS" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_COPYTOCDS_INDEX, -1, 1);
    Ut_CFE_ES_ContinueReturnCodeAfterCountZero(UT_CFE_ES_COPYTOCDS_INDEX);

    /* Execute the function being tested */
    Result = LC_HousekeepingReq((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True (LC_OperData.HaveActiveCDS == FALSE, "LC_OperData.HaveActiveCDS == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_HousekeepingReq_Test_UpdateTaskCDSNotSuccess */

void LC_ResetCounters_Test(void)
{
    LC_AppData.CmdCount     = 1;               
    LC_AppData.CmdErrCount  = 2;         

    LC_AppData.APSampleCount        = 3;
    LC_AppData.MonitoredMsgCount    = 4;
    LC_AppData.RTSExecCount         = 5;
    LC_AppData.PassiveRTSExecCount  = 6;

    /* Execute the function being tested */
    LC_ResetCounters();
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdCount == 0, "LC_AppData.CmdCount == 0");
    UtAssert_True (LC_AppData.CmdErrCount == 0, "LC_AppData.CmdErrCount == 0");
    UtAssert_True (LC_AppData.APSampleCount == 0, "LC_AppData.APSampleCount == 0");
    UtAssert_True (LC_AppData.MonitoredMsgCount == 0, "LC_AppData.MonitoredMsgCount == 0");
    UtAssert_True (LC_AppData.RTSExecCount == 0, "LC_AppData.RTSExecCount == 0");
    UtAssert_True (LC_AppData.PassiveRTSExecCount == 0, "LC_AppData.PassiveRTSExecCount == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ResetCounters_Test */

void LC_SetLCStateCmd_Test_Active(void)
{
    LC_SetLCState_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetLCState_t), TRUE);

    CmdPacket.NewLCState = LC_STATE_ACTIVE;

    /* Execute the function being tested */
    LC_SetLCStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CurrentLCState == LC_STATE_ACTIVE, "LC_AppData.CurrentLCState == LC_STATE_ACTIVE");
    UtAssert_True (LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_LCSTATE_INF_EID, CFE_EVS_INFORMATION, "Set LC state command: new state = 1"),
        "Set LC state command: new state = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetLCStateCmd_Test_Active */

void LC_SetLCStateCmd_Test_Passive(void)
{
    LC_SetLCState_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetLCState_t), TRUE);

    CmdPacket.NewLCState = LC_STATE_PASSIVE;

    /* Execute the function being tested */
    LC_SetLCStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CurrentLCState == LC_STATE_PASSIVE, "LC_AppData.CurrentLCState == LC_STATE_PASSIVE");
    UtAssert_True (LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_LCSTATE_INF_EID, CFE_EVS_INFORMATION, "Set LC state command: new state = 2"),
        "Set LC state command: new state = 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetLCStateCmd_Test_Passive */

void LC_SetLCStateCmd_Test_Disabled(void)
{
    LC_SetLCState_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetLCState_t), TRUE);

    CmdPacket.NewLCState = LC_STATE_DISABLED;

    /* Execute the function being tested */
    LC_SetLCStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CurrentLCState == LC_STATE_DISABLED, "LC_AppData.CurrentLCState == LC_STATE_DISABLED");
    UtAssert_True (LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_LCSTATE_INF_EID, CFE_EVS_INFORMATION, "Set LC state command: new state = 3"),
        "Set LC state command: new state = 3");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetLCStateCmd_Test_Disabled */

void LC_SetLCStateCmd_Test_Default(void)
{
    LC_SetLCState_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetLCState_t), TRUE);

    CmdPacket.NewLCState = 99;

    /* Execute the function being tested */
    LC_SetLCStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_LCSTATE_ERR_EID, CFE_EVS_ERROR, "Set LC state error: invalid state = 99"),
        "Set LC state error: invalid state = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetLCStateCmd_Test_Default */

void LC_SetAPStateCmd_Test_Default(void)
{
    LC_SetAPState_t     CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetLCState_t), TRUE);

    CmdPacket.APNumber   = 1;
    CmdPacket.NewAPState = 99;

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_NEW_ERR_EID, CFE_EVS_ERROR, "Set AP state error: AP = 1, Invalid new state = 99"),
        "Set AP state error: AP = 1, Invalid new state = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_Default */

void LC_SetAPStateCmd_Test_SetAllActionPointsActive(void)
{
    LC_SetAPState_t   CmdPacket;
    uint16            TableIndex;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_ACTIVE;
    CmdPacket.APNumber   = LC_ALL_ACTIONPOINTS;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
    }

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */

    /* Checks for first index: */
    TableIndex = 0;
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE, "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE");

    /* Checks for middle index: */
    TableIndex = LC_MAX_ACTIONPOINTS/2;
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE, "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE");

    /* Checks for last index: */
    TableIndex = LC_MAX_ACTIONPOINTS - 1;
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE, "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_ACTIVE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_INF_EID, CFE_EVS_INFORMATION, "Set AP state command: AP = 65535, New state = 1"),
        "Set AP state command: AP = 65535, New state = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_SetAllActionPointsActive */

void LC_SetAPStateCmd_Test_SetAllActionPointsPassive(void)
{
    LC_SetAPState_t   CmdPacket;
    uint16            TableIndex;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_PASSIVE;
    CmdPacket.APNumber   = LC_ALL_ACTIONPOINTS;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
    }

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */

    /* Checks for first index: */
    TableIndex = 0;
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE, "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE");

    /* Checks for middle index: */
    TableIndex = LC_MAX_ACTIONPOINTS/2;
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE, "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE");

    /* Checks for last index: */
    TableIndex = LC_MAX_ACTIONPOINTS - 1;
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE, "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_PASSIVE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_INF_EID, CFE_EVS_INFORMATION, "Set AP state command: AP = 65535, New state = 2"),
        "Set AP state command: AP = 65535, New state = 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_SetAllActionPointsPassive */

void LC_SetAPStateCmd_Test_SetAllActionPointsDisabled(void)
{
    LC_SetAPState_t   CmdPacket;
    uint16            TableIndex;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_DISABLED;
    CmdPacket.APNumber   = LC_ALL_ACTIONPOINTS;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
    }

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */

    /* Checks for first index: */
    TableIndex = 0;
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED, "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED");

    /* Checks for middle index: */
    TableIndex = LC_MAX_ACTIONPOINTS/2;
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED, "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED");

    /* Checks for last index: */
    TableIndex = LC_MAX_ACTIONPOINTS - 1;
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED, "LC_OperData.ARTPtr[TableIndex].CurrentState == LC_APSTATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_INF_EID, CFE_EVS_INFORMATION, "Set AP state command: AP = 65535, New state = 3"),
        "Set AP state command: AP = 65535, New state = 3");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_SetAllActionPointsDisabled */

void LC_SetAPStateCmd_Test_UpdateSingleActionPointActive(void)
{
    LC_SetAPState_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_ACTIVE;
    CmdPacket.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState = 99;

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState == LC_APSTATE_ACTIVE, "LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState == LC_APSTATE_ACTIVE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_INF_EID, CFE_EVS_INFORMATION, "Set AP state command: AP = 175, New state = 1"),
        "Set AP state command: AP = 175, New state = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_UpdateSingleActionPointActive */

void LC_SetAPStateCmd_Test_UpdateSingleActionPointPassive(void)
{
    LC_SetAPState_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_PASSIVE;
    CmdPacket.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState = 99;

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState == LC_APSTATE_PASSIVE, "LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState == LC_APSTATE_PASSIVE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_INF_EID, CFE_EVS_INFORMATION, "Set AP state command: AP = 175, New state = 2"),
        "Set AP state command: AP = 175, New state = 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_UpdateSingleActionPointPassive */

void LC_SetAPStateCmd_Test_UpdateSingleActionPointDisabled(void)
{
    LC_SetAPState_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_DISABLED;
    CmdPacket.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState = 99;

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState == LC_APSTATE_DISABLED, "LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState == LC_APSTATE_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_INF_EID, CFE_EVS_INFORMATION, "Set AP state command: AP = 175, New state = 3"),
        "Set AP state command: AP = 175, New state = 3");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_UpdateSingleActionPointDisabled */

void LC_SetAPStateCmd_Test_InvalidCurrentAPStateActive(void)
{
    LC_SetAPState_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_ACTIVE;
    CmdPacket.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_CURR_ERR_EID, CFE_EVS_ERROR, "Set AP state error: AP = 175, Invalid current AP state = 4"),
        "Set AP state error: AP = 175, Invalid current AP state = 4");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_InvalidCurrentAPStateActive */

void LC_SetAPStateCmd_Test_InvalidCurrentAPStatePassive(void)
{
    LC_SetAPState_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_PASSIVE;
    CmdPacket.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_CURR_ERR_EID, CFE_EVS_ERROR, "Set AP state error: AP = 175, Invalid current AP state = 4"),
        "Set AP state error: AP = 175, Invalid current AP state = 4");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_InvalidCurrentAPStatePassive */

void LC_SetAPStateCmd_Test_InvalidCurrentAPStateDisabled(void)
{
    LC_SetAPState_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_DISABLED;
    CmdPacket.APNumber   = LC_MAX_ACTIONPOINTS - 1;

    LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_CURR_ERR_EID, CFE_EVS_ERROR, "Set AP state error: AP = 175, Invalid current AP state = 4"),
        "Set AP state error: AP = 175, Invalid current AP state = 4");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_InvalidCurrentAPStateDisabled */

void LC_SetAPStateCmd_Test_InvalidAPNumberActive(void)
{
    LC_SetAPState_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_ACTIVE;
    CmdPacket.APNumber   = LC_MAX_ACTIONPOINTS;

    LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_APNUM_ERR_EID, CFE_EVS_ERROR, "Set AP state error: Invalid AP number = 176"),
        "Set AP state error: Invalid AP number = 176");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_InvalidAPNumberActive */

void LC_SetAPStateCmd_Test_InvalidAPNumberPassive(void)
{
    LC_SetAPState_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_PASSIVE;
    CmdPacket.APNumber   = LC_MAX_ACTIONPOINTS;

    LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_APNUM_ERR_EID, CFE_EVS_ERROR, "Set AP state error: Invalid AP number = 176"),
        "Set AP state error: Invalid AP number = 176");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_InvalidAPNumberPassive */

void LC_SetAPStateCmd_Test_InvalidAPNumberDisabled(void)
{
    LC_SetAPState_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.NewAPState = LC_APSTATE_DISABLED;
    CmdPacket.APNumber   = LC_MAX_ACTIONPOINTS;

    LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState = LC_APSTATE_PERMOFF;

    /* Execute the function being tested */
    LC_SetAPStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATE_APNUM_ERR_EID, CFE_EVS_ERROR, "Set AP state error: Invalid AP number = 176"),
        "Set AP state error: Invalid AP number = 176");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPStateCmd_Test_InvalidAPNumberDisabled */

void LC_SetAPPermOffCmd_Test_InvalidAPNumberMaxActionpoints(void)
{
    LC_SetAPPermOff_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.APNumber   = LC_MAX_ACTIONPOINTS;

    /* Execute the function being tested */
    LC_SetAPPermOffCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APOFF_APNUM_ERR_EID, CFE_EVS_ERROR, "Set AP perm off error: Invalid AP number = 176"),
        "Set AP perm off error: Invalid AP number = 176");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPPermOffCmd_Test_InvalidAPNumberMaxActionpoints */

void LC_SetAPPermOffCmd_Test_InvalidAPNumberAllActionpoints(void)
{
    LC_SetAPPermOff_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.APNumber   = LC_ALL_ACTIONPOINTS;

    /* Execute the function being tested */
    LC_SetAPPermOffCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APOFF_APNUM_ERR_EID, CFE_EVS_ERROR, "Set AP perm off error: Invalid AP number = 65535"),
        "Set AP perm off error: Invalid AP number = 65535");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPPermOffCmd_Test_InvalidAPNumberAllActionpoints */

void LC_SetAPPermOffCmd_Test_APNotDisabled(void)
{
    LC_SetAPPermOff_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.APNumber = 1;

    LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState = 99;

    /* Execute the function being tested */
    LC_SetAPPermOffCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APOFF_CURR_ERR_EID, CFE_EVS_ERROR, "Set AP perm off error, AP NOT Disabled: AP = 1, Current state = 99"),
        "Set AP perm off error, AP NOT Disabled: AP = 1, Current state = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPPermOffCmd_Test_APNotDisabled */

void LC_SetAPPermOffCmd_Test_Nominal(void)
{
    LC_SetAPPermOff_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_SetAPState_t), TRUE);

    CmdPacket.APNumber = 1;

    LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState = LC_APSTATE_DISABLED;

    /* Execute the function being tested */
    LC_SetAPPermOffCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState == 4, "LC_OperData.ARTPtr[CmdPacket.APNumber].CurrentState == 4");
    UtAssert_True (LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APOFF_INF_EID, CFE_EVS_INFORMATION, "Set AP permanently off command: AP = 1"),
        "Set AP permanently off command: AP = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SetAPPermOffCmd_Test_Nominal */

void LC_ResetAPStatsCmd_Test_AllActionPoints(void)
{
    LC_ResetAPStats_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_ResetAPStats_t), TRUE);

    CmdPacket.APNumber = LC_ALL_ACTIONPOINTS;

    /* Execute the function being tested */
    LC_ResetAPStatsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATS_INF_EID, CFE_EVS_INFORMATION, "Reset AP stats command: AP = 65535"),
        "Reset AP stats command: AP = 65535");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ResetAPStatsCmd_Test_AllActionPoints */

void LC_ResetAPStatsCmd_Test_SingleActionPoint(void)
{
    LC_ResetAPStats_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_ResetAPStats_t), TRUE);

    CmdPacket.APNumber = LC_MAX_ACTIONPOINTS - 1;

    /* Execute the function being tested */
    LC_ResetAPStatsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATS_INF_EID, CFE_EVS_INFORMATION, "Reset AP stats command: AP = 175"),
        "Reset AP stats command: AP = 175");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ResetAPStatsCmd_Test_SingleActionPoint */

void LC_ResetAPStatsCmd_Test_InvalidAPNumber(void)
{
    LC_ResetAPStats_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_ResetAPStats_t), TRUE);

    CmdPacket.APNumber = LC_MAX_ACTIONPOINTS;

    /* Execute the function being tested */
    LC_ResetAPStatsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSTATS_APNUM_ERR_EID, CFE_EVS_ERROR, "Reset AP stats error: invalid AP number = 176"),
        "Reset AP stats error: invalid AP number = 176");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ResetAPStatsCmd_Test_InvalidAPNumber */

void LC_ResetResultsAP_Test(void)
{
    uint32          TableIndex;
    uint32          StartIndex = 0;
    uint32          EndIndex   = 5;
    boolean         ResetStatsCmd = FALSE;

    for (TableIndex = StartIndex; TableIndex <= EndIndex; TableIndex++)
    {
        LC_OperData.ARTPtr[TableIndex].ActionResult = 99;
        LC_OperData.ARTPtr[TableIndex].CurrentState = 99;
        
        LC_OperData.ARTPtr[TableIndex].PassiveAPCount          = 99;
        LC_OperData.ARTPtr[TableIndex].FailToPassCount         = 99;
        LC_OperData.ARTPtr[TableIndex].PassToFailCount         = 99;

        LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount    = 99;
        LC_OperData.ARTPtr[TableIndex].CumulativeFailCount     = 99;
        LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount  = 99;
        LC_OperData.ARTPtr[TableIndex].CumulativeEventMsgsSent = 99;

        LC_OperData.ADTPtr[TableIndex].DefaultState = 1;
    }

    /* Execute the function being tested */
    LC_ResetResultsAP(StartIndex, EndIndex, ResetStatsCmd);
    
    /* Verify results */

    /* Checks for first index: */
    TableIndex = StartIndex;

    UtAssert_True
        (LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE,
        "LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE");

    UtAssert_True
        (LC_OperData.ARTPtr[TableIndex].CurrentState == 1,
        "LC_OperData.ARTPtr[TableIndex].CurrentState == 1");
        
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0, "LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0, "LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0, "LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0");

    UtAssert_True (LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0, "LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0, "LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0, "LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CumulativeEventMsgsSent == 0, "LC_OperData.ARTPtr[TableIndex].CumulativeEventMsgsSent == 0");

    /* Checks for middle index: */
    TableIndex = (StartIndex + EndIndex)/2;

    UtAssert_True
        (LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE,
        "LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE");

    UtAssert_True
        (LC_OperData.ARTPtr[TableIndex].CurrentState == 1,
        "LC_OperData.ARTPtr[TableIndex].CurrentState == 1");
        
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0, "LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0, "LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0, "LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0");

    UtAssert_True (LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0, "LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0, "LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0, "LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0");

    /* Checks for last index: */
    TableIndex = EndIndex;

    UtAssert_True
        (LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE,
        "LC_OperData.ARTPtr[TableIndex].ActionResult == LC_ACTION_STALE");

    UtAssert_True
        (LC_OperData.ARTPtr[TableIndex].CurrentState == 1,
        "LC_OperData.ARTPtr[TableIndex].CurrentState == 1");
        
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0, "LC_OperData.ARTPtr[TableIndex].PassiveAPCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0, "LC_OperData.ARTPtr[TableIndex].FailToPassCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0, "LC_OperData.ARTPtr[TableIndex].PassToFailCount == 0");

    UtAssert_True (LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0, "LC_OperData.ARTPtr[TableIndex].ConsecutiveFailCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0, "LC_OperData.ARTPtr[TableIndex].CumulativeFailCount == 0");
    UtAssert_True (LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0, "LC_OperData.ARTPtr[TableIndex].CumulativeRTSExecCount == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ResetResultsAP_Test */

void LC_ResetWPStatsCmd_Test_AllWatchPoints(void)
{
    LC_ResetWPStats_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_ResetWPStats_t), TRUE);

    CmdPacket.WPNumber = LC_ALL_WATCHPOINTS;

    /* Execute the function being tested */
    LC_ResetWPStatsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WPSTATS_INF_EID, CFE_EVS_INFORMATION, "Reset WP stats command: WP = 65535"),
        "Reset WP stats command: WP = 65535");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ResetWPStatsCmd_Test_AllWatchPoints */

void LC_ResetWPStatsCmd_Test_SingleWatchPoint(void)
{
    LC_ResetWPStats_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_ResetWPStats_t), TRUE);

    CmdPacket.WPNumber = LC_MAX_WATCHPOINTS - 1;

    /* Execute the function being tested */
    LC_ResetWPStatsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdCount == 1, "LC_AppData.CmdCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WPSTATS_INF_EID, CFE_EVS_INFORMATION, "Reset WP stats command: WP = 175"),
        "Reset WP stats command: WP = 175");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ResetWPStatsCmd_Test_SingleWatchPoint */

void LC_ResetWPStatsCmd_Test_InvalidWPNumber(void)
{
    LC_ResetWPStats_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), LC_CMD_MID, sizeof(LC_ResetWPStats_t), TRUE);

    CmdPacket.WPNumber = LC_MAX_WATCHPOINTS;

    /* Execute the function being tested */
    LC_ResetWPStatsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WPSTATS_WPNUM_ERR_EID, CFE_EVS_ERROR, "Reset WP stats error: invalid WP number = 176"),
        "Reset WP stats error: invalid WP number = 176");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ResetWPStatsCmd_Test_InvalidWPNumber */

void LC_ResetResultsWP_Test(void)
{
    uint32          TableIndex;
    uint32          StartIndex = 0;
    uint32          EndIndex   = 5;
    boolean         ResetStatsCmd = FALSE;

    for (TableIndex = StartIndex; TableIndex <= EndIndex; TableIndex++)
    {
        LC_OperData.WRTPtr[TableIndex].WatchResult = 99;
        LC_OperData.WRTPtr[TableIndex].CountdownToStale = 99;

        LC_OperData.WRTPtr[TableIndex].EvaluationCount      = 99;
        LC_OperData.WRTPtr[TableIndex].FalseToTrueCount     = 99;
        LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount = 99;
        LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount  = 99;

        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value                = 99;
        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds    = 99;
        LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds = 99;

        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value                = 99;
        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds    = 99;
        LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds = 99;
    }

    /* Execute the function being tested */
    LC_ResetResultsWP(StartIndex, EndIndex, ResetStatsCmd);
    
    /* Verify results */

    /* Checks for first index: */
    TableIndex = StartIndex;

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE, "LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0, "LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0");

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0, "LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0, "LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0, "LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0, "LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0");

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0, "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0, "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0, "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0");

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0, "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0, "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0, "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0");

    /* Checks for middle index: */
    TableIndex = (StartIndex + EndIndex)/2;

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE, "LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0, "LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0");

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0, "LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0, "LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0, "LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0, "LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0");

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0, "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0, "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0, "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0");

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0, "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0, "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0, "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0");

    /* Checks for last index: */
    TableIndex = EndIndex;

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE, "LC_OperData.WRTPtr[TableIndex].WatchResult == LC_WATCH_STALE");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0, "LC_OperData.WRTPtr[TableIndex].CountdownToStale == 0");

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0, "LC_OperData.WRTPtr[TableIndex].EvaluationCount == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0, "LC_OperData.WRTPtr[TableIndex].FalseToTrueCount == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0, "LC_OperData.WRTPtr[TableIndex].ConsecutiveTrueCount == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0, "LC_OperData.WRTPtr[TableIndex].CumulativeTrueCount == 0");

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0, "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Value == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0, "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Seconds == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0, "LC_OperData.WRTPtr[TableIndex].LastFalseToTrue.Timestamp.Subseconds == 0");

    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0, "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Value == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0, "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Seconds == 0");
    UtAssert_True (LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0, "LC_OperData.WRTPtr[TableIndex].LastTrueToFalse.Timestamp.Subseconds == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ResetResultsWP_Test */

void LC_VerifyMsgLength_Test_HKRequestLengthError(void)
{
    boolean           Result;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, LC_SEND_HK_MID, 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), LC_NOOP_CC);

    /* Execute the function being tested */
    Result = LC_VerifyMsgLength((CFE_SB_MsgPtr_t)(&CmdPacket), sizeof(CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_HKREQ_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid HK request msg length: ID = 0x18A5, CC = 0, Len = 1, Expected = 8"),
        "Invalid HK request msg length: ID = 0x18A5, CC = 0, Len = 1, Expected = 8");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_VerifyMsgLength_Test_HKRequestLengthError */

void LC_VerifyMsgLength_Test_APSampleLengthError(void)
{
    boolean           Result;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, LC_SAMPLE_AP_MID, 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), LC_NOOP_CC);

    /* Execute the function being tested */
    Result = LC_VerifyMsgLength((CFE_SB_MsgPtr_t)(&CmdPacket), sizeof(CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSAMPLE_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid AP sample msg length: ID = 0x18A6, CC = 0, Len = 1, Expected = 8"),
        "Invalid AP sample msg length: ID = 0x18A6, CC = 0, Len = 1, Expected = 8");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_VerifyMsgLength_Test_APSampleLengthError */

void LC_VerifyMsgLength_Test_GenericLengthError(void)
{
    boolean           Result;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, 1, 1, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), LC_NOOP_CC);

    /* Execute the function being tested */
    Result = LC_VerifyMsgLength((CFE_SB_MsgPtr_t)(&CmdPacket), sizeof(CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid msg length: ID = 0x0001, CC = 0, Len = 1, Expected = 8"),
        "Invalid AP sample msg length: ID = 0x0001, CC = 0, Len = 1, Expected = 8");

    UtAssert_True (LC_AppData.CmdErrCount == 1, "LC_AppData.CmdErrCount == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_VerifyMsgLength_Test_GenericLengthError */

void LC_VerifyMsgLength_Test_Nominal(void)
{
    boolean           Result;
    LC_NoArgsCmd_t    CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, 1, sizeof(LC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)(&CmdPacket), LC_NOOP_CC);

    /* Execute the function being tested */
    Result = LC_VerifyMsgLength((CFE_SB_MsgPtr_t)(&CmdPacket), sizeof(CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_VerifyMsgLength_Test_Nominal */

void LC_ManageTables_Test_InfoUpdated(void)
{
    int32   Result;

    /* Set to satisfy all instances of condition "Result == CFE_TBL_INFO_UPDATED" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_ManageTables();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ManageTables_Test_InfoUpdated */

void LC_ManageTables_Test_WDTGetAddressError(void)
{
    int32   Result;
    /* Set to generate event message LC_WDT_GETADDR_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_ManageTables();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDT_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting WDT address, RC=0xFFFFFFFF"),
        "Error getting WDT address, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ManageTables_Test_WDTGetAddressError */

void LC_ManageTables_Test_ADTGetAddressError(void)
{
    int32   Result;

    /* Generates event message LC_ADT_GETADDR_ERR_EID */
    LC_CMDS_TEST_CFE_TBL_GetAddressHookCount = 0;
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &LC_CMDS_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    Result = LC_ManageTables();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADT_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting ADT address, RC=0xFFFFFFFF"),
        "Error getting ADT address, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ManageTables_Test_ADTGetAddressError */

void LC_UpdateTaskCDS_Test_UpdateWatchpointError(void)
{
    int32   Result;

    /* Set to generate error message LC_WRT_NO_SAVE_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_COPYTOCDS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WRT_NO_SAVE_ERR_EID, CFE_EVS_ERROR, "Unable to update watchpoint results in CDS, RC=0xFFFFFFFF"),
        "Unable to update watchpoint results in CDS, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_UpdateTaskCDS_Test_UpdateWatchpointError */

void LC_UpdateTaskCDS_Test_UpdateActionpointError(void)
{
    int32   Result;

    /* Set to generate error message LC_ART_NO_SAVE_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_COPYTOCDS_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ART_NO_SAVE_ERR_EID, CFE_EVS_ERROR, "Unable to update actionpoint results in CDS, RC=0xFFFFFFFF"),
        "Unable to update actionpoint results in CDS, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_UpdateTaskCDS_Test_UpdateActionpointError */

void LC_UpdateTaskCDS_Test_UpdateAppDataError(void)
{
    int32   Result;

    /* Set to generate error message LC_APP_NO_SAVE_START_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_COPYTOCDS_INDEX, -1, 3);

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");
    UtAssert_True (LC_AppData.CDSSavedOnExit == LC_CDS_SAVED, "LC_AppData.CDSSavedOnExit == LC_CDS_SAVED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APP_NO_SAVE_START_ERR_EID, CFE_EVS_ERROR, "Unable to update application data in CDS, RC=0xFFFFFFFF"),
        "Unable to update application data in CDS, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_UpdateTaskCDS_Test_UpdateAppDataError */

void LC_UpdateTaskCDS_Test_Nominal(void)
{
    int32   Result;

    /* Execute the function being tested */
    Result = LC_UpdateTaskCDS();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True (LC_AppData.CDSSavedOnExit == LC_CDS_SAVED, "LC_AppData.CDSSavedOnExit == LC_CDS_SAVED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_UpdateTaskCDS_Test_Nominal */

void LC_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(LC_AppPipe_Test_SampleAPRequest, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SampleAPRequest");
    UtTest_Add(LC_AppPipe_Test_HousekeepingRequest, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_HousekeepingRequest");
    UtTest_Add(LC_AppPipe_Test_Noop, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_Noop");
    UtTest_Add(LC_AppPipe_Test_Reset, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_Reset");
    UtTest_Add(LC_AppPipe_Test_SetLCState, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SetLCState");
    UtTest_Add(LC_AppPipe_Test_SetAPState, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SetAPState");
    UtTest_Add(LC_AppPipe_Test_SetAPPermoff, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_SetAPPermoff");
    UtTest_Add(LC_AppPipe_Test_ResetAPStats, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_ResetAPStats");
    UtTest_Add(LC_AppPipe_Test_ResetWPStats, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_ResetWPStats");
    UtTest_Add(LC_AppPipe_Test_InvalidCommandCode, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_InvalidCommandCode");
    UtTest_Add(LC_AppPipe_Test_MonitorPacket, LC_Test_Setup, LC_Test_TearDown, "LC_AppPipe_Test_MonitorPacket");

    UtTest_Add(LC_SampleAPReq_Test_AllowSampleAllWatchStale, LC_Test_Setup, LC_Test_TearDown, "LC_SampleAPReq_Test_AllowSampleAllWatchStale");
    UtTest_Add(LC_SampleAPReq_Test_StartLessOrEqualToEndAndEndWithinArrayWatchStale, LC_Test_Setup, LC_Test_TearDown, "LC_SampleAPReq_Test_StartLessOrEqualToEndAndEndWithinArrayWatchStale");
    UtTest_Add(LC_SampleAPReq_Test_ArrayIndexOutOfRange, LC_Test_Setup, LC_Test_TearDown, "LC_SampleAPReq_Test_ArrayIndexOutOfRange");

    UtTest_Add(LC_HousekeepingReq_Test_WatchStale, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_WatchStale");
    UtTest_Add(LC_HousekeepingReq_Test_WatchFalse, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_WatchFalse");
    UtTest_Add(LC_HousekeepingReq_Test_WatchTrue, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_WatchTrue");
    UtTest_Add(LC_HousekeepingReq_Test_WatchError, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_WatchError");
    UtTest_Add(LC_HousekeepingReq_Test_DefaultWatchResult, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_DefaultWatchResult");
    UtTest_Add(LC_HousekeepingReq_Test_ActionNotUsedStale, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_ActionNotUsedStale");
    UtTest_Add(LC_HousekeepingReq_Test_APStateActiveActionPass, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_APStateActiveActionPass");
    UtTest_Add(LC_HousekeepingReq_Test_APStatePassiveActionFail, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_APStatePassiveActionFail");
    UtTest_Add(LC_HousekeepingReq_Test_APStateDisabledActionError, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_APStateDisabledActionError");
    UtTest_Add(LC_HousekeepingReq_Test_APStatePermOffActionError, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_APStatePermOffActionError");
    UtTest_Add(LC_HousekeepingReq_Test_DefaultCurrentStateAndActionResult, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_DefaultCurrentStateAndActionResult");
    UtTest_Add(LC_HousekeepingReq_Test_ManageTablesError, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_ManageTablesError");
    UtTest_Add(LC_HousekeepingReq_Test_UpdateTaskCDSNotSuccess, LC_Test_Setup, LC_Test_TearDown, "LC_HousekeepingReq_Test_UpdateTaskCDSNotSuccess");

/* Noop fully tested in LC_AppPipe_Test_Noop */
/* Reset fully tested in LC_AppPipe_Test_Reset */

    UtTest_Add(LC_ResetCounters_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ResetCounters_Test");

    UtTest_Add(LC_SetLCStateCmd_Test_Active, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Active");
    UtTest_Add(LC_SetLCStateCmd_Test_Passive, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Passive");
    UtTest_Add(LC_SetLCStateCmd_Test_Disabled, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Disabled");
    UtTest_Add(LC_SetLCStateCmd_Test_Default, LC_Test_Setup, LC_Test_TearDown, "LC_SetLCStateCmd_Test_Default");

    UtTest_Add(LC_SetAPStateCmd_Test_Default, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_Default");
    UtTest_Add(LC_SetAPStateCmd_Test_SetAllActionPointsActive, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_SetAllActionPointsActive");
    UtTest_Add(LC_SetAPStateCmd_Test_SetAllActionPointsPassive, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_SetAllActionPointsPassive");
    UtTest_Add(LC_SetAPStateCmd_Test_SetAllActionPointsDisabled, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_SetAllActionPointsDisabled");
    UtTest_Add(LC_SetAPStateCmd_Test_UpdateSingleActionPointActive, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_UpdateSingleActionPointActive");
    UtTest_Add(LC_SetAPStateCmd_Test_UpdateSingleActionPointPassive, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_UpdateSingleActionPointPassive");
    UtTest_Add(LC_SetAPStateCmd_Test_UpdateSingleActionPointDisabled, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_UpdateSingleActionPointDisabled");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidCurrentAPStateActive, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_InvalidCurrentAPStateActive");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidCurrentAPStatePassive, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_InvalidCurrentAPStatePassive");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidCurrentAPStateDisabled, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_InvalidCurrentAPStateDisabled");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidAPNumberActive, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_InvalidAPNumberActive");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidAPNumberPassive, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_InvalidAPNumberPassive");
    UtTest_Add(LC_SetAPStateCmd_Test_InvalidAPNumberDisabled, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPStateCmd_Test_InvalidAPNumberDisabled");

    UtTest_Add(LC_SetAPPermOffCmd_Test_InvalidAPNumberMaxActionpoints, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPPermOffCmd_Test_InvalidAPNumberMaxActionpoints");
    UtTest_Add(LC_SetAPPermOffCmd_Test_InvalidAPNumberAllActionpoints, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPPermOffCmd_Test_InvalidAPNumberAllActionpoints");
    UtTest_Add(LC_SetAPPermOffCmd_Test_APNotDisabled, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPPermOffCmd_Test_APNotDisabled");
    UtTest_Add(LC_SetAPPermOffCmd_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_SetAPPermOffCmd_Test_Nominal");

    UtTest_Add(LC_ResetAPStatsCmd_Test_AllActionPoints, LC_Test_Setup, LC_Test_TearDown, "LC_ResetAPStatsCmd_Test_AllActionPoints");
    UtTest_Add(LC_ResetAPStatsCmd_Test_SingleActionPoint, LC_Test_Setup, LC_Test_TearDown, "LC_ResetAPStatsCmd_Test_SingleActionPoint");
    UtTest_Add(LC_ResetAPStatsCmd_Test_InvalidAPNumber, LC_Test_Setup, LC_Test_TearDown, "LC_ResetAPStatsCmd_Test_InvalidAPNumber");

    UtTest_Add(LC_ResetResultsAP_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ResetResultsAP_Test");

    UtTest_Add(LC_ResetWPStatsCmd_Test_AllWatchPoints, LC_Test_Setup, LC_Test_TearDown, "LC_ResetWPStatsCmd_Test_AllWatchPoints");
    UtTest_Add(LC_ResetWPStatsCmd_Test_SingleWatchPoint, LC_Test_Setup, LC_Test_TearDown, "LC_ResetWPStatsCmd_Test_SingleWatchPoint");
    UtTest_Add(LC_ResetWPStatsCmd_Test_InvalidWPNumber, LC_Test_Setup, LC_Test_TearDown, "LC_ResetWPStatsCmd_Test_InvalidWPNumber");

    UtTest_Add(LC_ResetResultsWP_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ResetResultsWP_Test");

    UtTest_Add(LC_VerifyMsgLength_Test_HKRequestLengthError, LC_Test_Setup, LC_Test_TearDown, "LC_VerifyMsgLength_Test_HKRequestLengthError");
    UtTest_Add(LC_VerifyMsgLength_Test_APSampleLengthError, LC_Test_Setup, LC_Test_TearDown, "LC_VerifyMsgLength_Test_APSampleLengthError");
    UtTest_Add(LC_VerifyMsgLength_Test_GenericLengthError, LC_Test_Setup, LC_Test_TearDown, "LC_VerifyMsgLength_Test_GenericLengthError");
    UtTest_Add(LC_VerifyMsgLength_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_VerifyMsgLength_Test_Nominal");

    UtTest_Add(LC_ManageTables_Test_InfoUpdated, LC_Test_Setup, LC_Test_TearDown, "LC_ManageTables_Test_InfoUpdated");
    UtTest_Add(LC_ManageTables_Test_WDTGetAddressError, LC_Test_Setup, LC_Test_TearDown, "LC_ManageTables_Test_WDTGetAddressError");
    UtTest_Add(LC_ManageTables_Test_ADTGetAddressError, LC_Test_Setup, LC_Test_TearDown, "LC_ManageTables_Test_ADTGetAddressError");

    UtTest_Add(LC_UpdateTaskCDS_Test_UpdateWatchpointError, LC_Test_Setup, LC_Test_TearDown, "LC_UpdateTaskCDS_Test_UpdateWatchpointError");
    UtTest_Add(LC_UpdateTaskCDS_Test_UpdateActionpointError, LC_Test_Setup, LC_Test_TearDown, "LC_UpdateTaskCDS_Test_UpdateActionpointError");
    UtTest_Add(LC_UpdateTaskCDS_Test_UpdateAppDataError, LC_Test_Setup, LC_Test_TearDown, "LC_UpdateTaskCDS_Test_UpdateAppDataError");
    UtTest_Add(LC_UpdateTaskCDS_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_UpdateTaskCDS_Test_Nominal");

} /* end LC_Cmds_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
