 /*************************************************************************
 ** File:
 **   $Id: lc_action_test.c 1.6 2017/07/06 12:00:29EDT mdeschu Exp  $
 **
 **  Copyright (c) 2007-2014 United States Government as represented by the 
 **  Administrator of the National Aeronautics and Space Administration. 
 **  All Other Rights Reserved.  
 **
 **  This software was created at NASA's Goddard Space Flight Center.
 **  This software is governed by the NASA Open Source Agreement and may be 
 **  used, distributed and modified only pursuant to the terms of that 
 **  agreement.
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file lc_action.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 ** Notes:
 **
 **
 *************************************************************************/

/*
 * Includes
 */

#include "lc_action_test.h"
#include "lc_action.h"
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

void LC_SampleAPs_Test_SingleActionPointNominal(void)
{
    uint16          StartIndex = 0;
    uint16          EndIndex   = 0;

    LC_OperData.ARTPtr[StartIndex].CurrentState = 99;

    /* Execute the function being tested */
    LC_SampleAPs(StartIndex, EndIndex);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SampleAPs_Test_SingleActionPointNominal */

void LC_SampleAPs_Test_SingleActionPointError(void)
{
    uint16          StartIndex = 0;
    uint16          EndIndex   = 0;

    LC_OperData.ARTPtr[StartIndex].CurrentState = LC_ACTION_NOT_USED;

    /* Execute the function being tested */
    LC_SampleAPs(StartIndex, EndIndex);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APSAMPLE_CURR_ERR_EID, CFE_EVS_ERROR, "Sample AP error, invalid current AP state: AP = 0, State = 255"),
        "Sample AP error, invalid current AP state: AP = 0, State = 255");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SampleAPs_Test_SingleActionPointError */

void LC_SampleAPs_Test_MultiActionPointNominal(void)
{
    uint16          StartIndex = 0;
    uint16          EndIndex   = 1;

    /* Execute the function being tested */
    LC_SampleAPs(StartIndex, EndIndex);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SampleAPs_Test_MultiActionPointNominal */

void LC_SampleSingleAP_Test_StateChangePassToFail(void)
{
    uint16            APNumber = 0;

    LC_OperData.ARTPtr[APNumber].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult = LC_ACTION_PASS;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;
    LC_OperData.ADTPtr[APNumber].MaxPassFailEvents = 100;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);
    
    /* Verify results */
    UtAssert_True (LC_AppData.APSampleCount == 1, "LC_AppData.APSampleCount == 1");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_FAIL, "LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_FAIL");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 1, "LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 1");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].CumulativeFailCount == 1, "LC_OperData.ARTPtr[APNumber].CumulativeFailCount == 1");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].PassToFailCount == 1, "LC_OperData.ARTPtr[APNumber].PassToFailCount == 1");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1, "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_AP_PASSTOFAIL_INF_EID, CFE_EVS_INFORMATION, "AP state change from PASS to FAIL: AP = 0"),
        "AP state change from PASS to FAIL: AP = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_SampleSingleAP_Test_StateChangePassToFail */

void LC_SampleSingleAP_Test_ActiveRequestRTS(void)
{
    uint16            APNumber = 0;

    LC_OperData.ARTPtr[APNumber].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult = LC_ACTION_FAIL;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;
    LC_OperData.ADTPtr[APNumber].MaxPassFailEvents = 100;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;

    LC_OperData.ADTPtr[APNumber].EventID   = 1;
    LC_OperData.ADTPtr[APNumber].EventType = 2;

    LC_AppData.CurrentLCState = LC_STATE_ACTIVE;

    LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS    = 5;
    LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS;
    strncpy(LC_OperData.ADTPtr[APNumber].EventText, "Event Message", 50);

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);
    
    /* Verify results */
    UtAssert_True (LC_OperData.ARTPtr[APNumber].CurrentState == LC_APSTATE_PASSIVE, "LC_OperData.ARTPtr[APNumber].CurrentState == LC_APSTATE_PASSIVE");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].CumulativeRTSExecCount == 1, "LC_OperData.ARTPtr[APNumber].CumulativeRTSExecCount == 1");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1, "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1");
    UtAssert_True (LC_AppData.RTSExecCount == 1, "LC_AppData.RTSExecCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_OperData.ADTPtr[APNumber].EventID, LC_OperData.ADTPtr[APNumber].EventType, "Event Message: AP = 0, FailCount = 6, RTS = 0"),
        "Event Message: AP = 0, FailCount = 6, RTS = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SampleSingleAP_Test_ActiveRequestRTS */

void LC_SampleSingleAP_Test_APFailWhileLCStatePassive(void)
{
    uint16            APNumber = 0;

    LC_OperData.ARTPtr[APNumber].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxPassFailEvents = 100;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;

    LC_OperData.ADTPtr[APNumber].EventID   = 1;
    LC_OperData.ADTPtr[APNumber].EventType = 2;

    LC_AppData.CurrentLCState = LC_STATE_PASSIVE;

    LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS    = 5;
    LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS;
    strncpy(LC_OperData.ADTPtr[APNumber].EventText, "Event Message", 50);

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);
    
    /* Verify results */
    UtAssert_True (LC_OperData.ARTPtr[APNumber].CurrentState == LC_APSTATE_PASSIVE, "LC_OperData.ARTPtr[APNumber].CurrentState == LC_APSTATE_PASSIVE");
    UtAssert_True (LC_AppData.PassiveRTSExecCount == 1, "LC_AppData.PassiveRTSExecCount == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_PASSIVE_FAIL_DBG_EID, CFE_EVS_DEBUG, "AP failed while LC App passive: AP = 0, FailCount = 6, RTS = 0"),
        "AP failed while LC App passive: AP = 0, FailCount = 6, RTS = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SampleSingleAP_Test_APFailWhileLCStatePassive */

void LC_SampleSingleAP_Test_APFailWhilePassive(void)
{
    uint16            APNumber = 0;

    LC_OperData.ARTPtr[APNumber].CurrentState = LC_APSTATE_PASSIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxPassFailEvents = 100;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;

    LC_OperData.ADTPtr[APNumber].EventID   = 1;
    LC_OperData.ADTPtr[APNumber].EventType = 2;

    LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS    = 5;
    LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount = LC_OperData.ADTPtr[APNumber].MaxFailsBeforeRTS;
    strncpy(LC_OperData.ADTPtr[APNumber].EventText, "Event Message", 50);

    LC_OperData.ADTPtr[APNumber].MaxPassiveEvents = 5;
    LC_OperData.ARTPtr[APNumber].PassiveAPCount   = LC_OperData.ADTPtr[APNumber].MaxPassiveEvents - 1;
    
    LC_AppData.PassiveRTSExecCount   = 0;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);
    
    /* Verify results */
    UtAssert_True (LC_OperData.ARTPtr[APNumber].PassiveAPCount == 5, "LC_OperData.ARTPtr[APNumber].PassiveAPCount == 5");
    UtAssert_True (LC_AppData.PassiveRTSExecCount == 1, "LC_OperData.ARTPtr[APNumber].PassiveAPCount == 1");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1, "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1");
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_AP_PASSIVE_FAIL_INF_EID, CFE_EVS_INFORMATION, "AP failed while passive: AP = 0, FailCount = 6, RTS = 0"),
        "AP failed while passive: AP = 0, FailCount = 6, RTS = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SampleSingleAP_Test_APFailWhilePassive */

void LC_SampleSingleAP_Test_StateChangeFailToPass(void)
{
    uint16            APNumber = 0;

    LC_OperData.ARTPtr[APNumber].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxFailPassEvents = 1;
    LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_FALSE;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);
    
    /* Verify results */
    UtAssert_True (LC_AppData.APSampleCount == 1, "LC_AppData.APSampleCount == 1");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_PASS, "LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_PASS");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].FailToPassCount == 1, "LC_OperData.ARTPtr[APNumber].FailToPassCount == 1");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1, "LC_OperData.ARTPtr[APNumber].CumulativeEventMsgsSent == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_AP_FAILTOPASS_INF_EID, CFE_EVS_INFORMATION, "AP state change from FAIL to PASS: AP = 0"),
        "AP state change from FAIL to PASS: AP = 0");

    UtAssert_True (LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0, "LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SampleSingleAP_Test_StateChangeFailToPass */

void LC_SampleSingleAP_Test_ActionStale(void)
{
    uint16            APNumber = 0;

    LC_OperData.ARTPtr[APNumber].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxFailPassEvents = 1;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);
    
    /* Verify results */
    UtAssert_True (LC_AppData.APSampleCount == 1, "LC_AppData.APSampleCount == 1");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_STALE, "LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_STALE");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0, "LC_OperData.ARTPtr[APNumber].ConsecutiveFailCount == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SampleSingleAP_Test_ActionStale */

void LC_SampleSingleAP_Test_ActionError(void)
{
    uint16            APNumber = 0;

    LC_OperData.ARTPtr[APNumber].CurrentState = LC_APSTATE_ACTIVE;
    LC_OperData.ARTPtr[APNumber].ActionResult = LC_ACTION_FAIL;
    LC_OperData.ADTPtr[APNumber].MaxFailPassEvents = 1;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;

    /* Execute the function being tested */
    LC_SampleSingleAP(APNumber);
    
    /* Verify results */
    UtAssert_True (LC_AppData.APSampleCount == 1, "LC_AppData.APSampleCount == 1");
    UtAssert_True (LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_ERROR, "LC_OperData.ARTPtr[APNumber].ActionResult == LC_ACTION_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ACTION_ERROR_ERR_EID, CFE_EVS_ERROR, "AP evaluated to error: AP = 0, Result = 2"),
        "AP evaluated to error: AP = 0, Result = 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SampleSingleAP_Test_ActionError */

void LC_EvaluateRPN_Test_AndWatchFalseOp1(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 0;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_FALSE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_PASS, "Result == LC_ACTION_PASS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_AndWatchFalseOp1 */

void LC_EvaluateRPN_Test_AndWatchFalseOp2(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_FALSE;
    LC_OperData.WRTPtr[1].WatchResult = 0;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_PASS, "Result == LC_ACTION_PASS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_AndWatchFalseOp2 */

void LC_EvaluateRPN_Test_OrWatchTrueOp1(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 0;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_TRUE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_FAIL, "Result == LC_ACTION_FAIL");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_OrWatchTrueOp1 */

void LC_EvaluateRPN_Test_OrWatchTrueOp2(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_TRUE;
    LC_OperData.WRTPtr[1].WatchResult = 0;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_FAIL, "Result == LC_ACTION_FAIL");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_OrWatchTrueOp2 */

void LC_EvaluateRPN_Test_AndWatchErrorOp1(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_AndWatchErrorOp1 */

void LC_EvaluateRPN_Test_AndWatchErrorOp2(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_ERROR;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_AndWatchErrorOp2 */

void LC_EvaluateRPN_Test_OrWatchErrorOp1(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_OrWatchErrorOp1 */

void LC_EvaluateRPN_Test_OrWatchErrorOp2(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_ERROR;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_OrWatchErrorOp2 */

void LC_EvaluateRPN_Test_XorWatchErrorOp1(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_XorWatchErrorOp1 */

void LC_EvaluateRPN_Test_XorWatchErrorOp2(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_ERROR;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_XorWatchErrorOp2 */

void LC_EvaluateRPN_Test_NotWatchError(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_NOT;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_ERROR;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_NotWatchError */

void LC_EvaluateRPN_Test_AndStaleOp1(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_AndStaleOp1 */

void LC_EvaluateRPN_Test_AndStaleOp2(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_STALE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_AndStaleOp2 */

void LC_EvaluateRPN_Test_OrStaleOp1(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_OrStaleOp1 */

void LC_EvaluateRPN_Test_OrStaleOp2(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_STALE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_OrStaleOp2 */

void LC_EvaluateRPN_Test_XorStaleOp1(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_XorStaleOp1 */

void LC_EvaluateRPN_Test_XorStaleOp2(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 99;
    LC_OperData.WRTPtr[1].WatchResult = LC_WATCH_STALE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_XorStaleOp2 */

void LC_EvaluateRPN_Test_NotStale(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_NOT;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = LC_WATCH_STALE;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_STALE, "Result == LC_ACTION_STALE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_NotStale */

void LC_EvaluateRPN_Test_AndNominal(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_FAIL, "Result == LC_ACTION_FAIL");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_AndNominal */

void LC_EvaluateRPN_Test_OrNominal(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_PASS, "Result == LC_ACTION_PASS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_OrNominal */

void LC_EvaluateRPN_Test_XorNominal(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[APNumber].RPNEquation[3] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;
    LC_OperData.WRTPtr[1].WatchResult = 99;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_XorNominal */

void LC_EvaluateRPN_Test_NotNominal(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_NOT;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_NotNominal */

void LC_EvaluateRPN_Test_EqualIllegalRPN(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = 1;
    LC_OperData.ADTPtr[APNumber].RPNEquation[2] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;
    LC_OperData.WRTPtr[1].WatchResult = 77;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_INVALID_RPN_ERR_EID, CFE_EVS_ERROR, "AP has illegal RPN expression: AP = 0, LastOperand = 2, StackPtr = 1"),
        "AP has illegal RPN expression: AP = 0, LastOperand = 2, StackPtr = 1");

    UtAssert_True (Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_EvaluateRPN_Test_EqualIllegalRPN */

void LC_EvaluateRPN_Test_WatchpointNumberNominal(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = 0;
    LC_OperData.ADTPtr[APNumber].RPNEquation[1] = LC_RPN_EQUAL;

    LC_OperData.WRTPtr[0].WatchResult = 77;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == 77, "Result == 77");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvaluateRPN_Test_WatchpointNumberNominal */

void LC_EvaluateRPN_Test_DefaultIllegalRPN(void)
{
    uint8             Result;
    uint16            APNumber = 0;

    LC_OperData.ADTPtr[APNumber].RPNEquation[0] = LC_MAX_WATCHPOINTS;

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_INVALID_RPN_ERR_EID, CFE_EVS_ERROR, "AP has illegal RPN expression: AP = 0, LastOperand = 0, StackPtr = 0"),
        "AP has illegal RPN expression: AP = 0, LastOperand = 0, StackPtr = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_EvaluateRPN_Test_DefaultIllegalRPN */

void LC_EvaluateRPN_Test_EndOfBufferWhenNotDone(void)
{
    uint8             Result;
    uint16            APNumber = 0;
    uint32            i;

    for (i = 0; i <= LC_MAX_RPN_EQU_SIZE; i++)
    {
        LC_OperData.ADTPtr[APNumber].RPNEquation[i] = 0;
        LC_OperData.WRTPtr[i].WatchResult = 77;
    }

    /* Execute the function being tested */
    Result = LC_EvaluateRPN(APNumber);
    
    /* Verify results */
    UtAssert_True (Result == LC_ACTION_ERROR, "Result == LC_ACTION_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_INVALID_RPN_ERR_EID, CFE_EVS_ERROR, "AP has illegal RPN expression: AP = 0, LastOperand = 19, StackPtr = 20"),
        "AP has illegal RPN expression: AP = 0, LastOperand = 19, StackPtr = 20");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_EvaluateRPN_Test_EndOfBufferWhenNotDone */

void LC_ValidateADT_Test_ActionNotUsed(void)
{
    uint8   Result;
    uint32  TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++ )
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState = LC_ACTION_NOT_USED;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_INF_EID, CFE_EVS_INFORMATION, "ADT verify results: good = 0, bad = 0, unused = 176"),
        "ADT verify results: good = 0, bad = 0, unused = 176");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ValidateADT_Test_ActionNotUsed */

void LC_ValidateADT_Test_InvalidDefaultState(void)
{
    uint8             Result;
    uint32            TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++ )
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState = 99;
        LC_OperData.ADTPtr[TableIndex].RTSId = 88;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 77;
        LC_OperData.ADTPtr[TableIndex].EventType = 66;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_ERR_DEFSTATE, "Result == LC_ADTVAL_ERR_DEFSTATE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_ERR_EID, CFE_EVS_ERROR, "ADT verify err: AP = 0, Err = 1, State = 99, RTS = 88, FailCnt = 77, EvtType = 66"),
        "ADT verify err: AP = 0, Err = 1, State = 99, RTS = 88, FailCnt = 77, EvtType = 66");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_INF_EID, CFE_EVS_INFORMATION, "ADT verify results: good = 0, bad = 176, unused = 0"),
        "ADT verify results: good = 0, bad = 176, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end LC_ValidateADT_Test_InvalidDefaultState */

void LC_ValidateADT_Test_BadRtsID(void)
{
    uint8             Result;
    uint32            TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++ )
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState = LC_APSTATE_ACTIVE;
        LC_OperData.ADTPtr[TableIndex].RTSId = LC_MAX_VALID_ADT_RTSID + 1;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 77;
        LC_OperData.ADTPtr[TableIndex].EventType = 66;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_ERR_RTSID, "Result == LC_ADTVAL_ERR_RTSID");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_ERR_EID, CFE_EVS_ERROR, "ADT verify err: AP = 0, Err = 2, State = 1, RTS = 65521, FailCnt = 77, EvtType = 66"),
        "ADT verify err: AP = 0, Err = 2, State = 1, RTS = 65521, FailCnt = 77, EvtType = 66");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_INF_EID, CFE_EVS_INFORMATION, "ADT verify results: good = 0, bad = 176, unused = 0"),
        "ADT verify results: good = 0, bad = 176, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end LC_ValidateADT_Test_BadRtsID */

void LC_ValidateADT_Test_BadFailCount(void)
{
    uint8             Result;
    uint32            TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++ )
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState = LC_APSTATE_ACTIVE;
        LC_OperData.ADTPtr[TableIndex].RTSId = 1;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 0;
        LC_OperData.ADTPtr[TableIndex].EventType = 66;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_ERR_FAILCNT, "Result == LC_ADTVAL_ERR_FAILCNT");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_ERR_EID, CFE_EVS_ERROR, "ADT verify err: AP = 0, Err = 3, State = 1, RTS = 1, FailCnt = 0, EvtType = 66"),
        "ADT verify err: AP = 0, Err = 3, State = 1, RTS = 1, FailCnt = 0, EvtType = 66");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_INF_EID, CFE_EVS_INFORMATION, "ADT verify results: good = 0, bad = 176, unused = 0"),
        "ADT verify results: good = 0, bad = 176, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end LC_ValidateADT_Test_BadFailCount */

void LC_ValidateADT_Test_InvalidEventType(void)
{
    uint8             Result;
    uint32            TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++ )
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState = LC_APSTATE_ACTIVE;
        LC_OperData.ADTPtr[TableIndex].RTSId = 1;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 88;
        LC_OperData.ADTPtr[TableIndex].EventType = 99;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_ERR_EVTTYPE, "Result == LC_ADTVAL_ERR_EVTTYPE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_ERR_EID, CFE_EVS_ERROR, "ADT verify err: AP = 0, Err = 4, State = 1, RTS = 1, FailCnt = 88, EvtType = 99"),
        "ADT verify err: AP = 0, Err = 4, State = 1, RTS = 1, FailCnt = 88, EvtType = 99");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_INF_EID, CFE_EVS_INFORMATION, "ADT verify results: good = 0, bad = 176, unused = 0"),
        "ADT verify results: good = 0, bad = 176, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end LC_ValidateADT_Test_InvalidEventType */

void LC_ValidateADT_Test_ValidateRpnAdtValError(void)
{
    uint8             Result;
    uint32            TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++ )
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState = LC_APSTATE_ACTIVE;
        LC_OperData.ADTPtr[TableIndex].RTSId = 1;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 88;
        LC_OperData.ADTPtr[TableIndex].EventType = CFE_EVS_DEBUG;
        LC_OperData.ADTPtr[TableIndex].RPNEquation[0] = 999;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_ERR_RPN, "Result == LC_ADTVAL_ERR_RPN");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_RPNERR_EID, CFE_EVS_ERROR, "ADT verify RPN err: AP = 0, Index = 0, StackDepth = 0"),
        "ADT verify RPN err: AP = 0, Index = 0, StackDepth = 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_INF_EID, CFE_EVS_INFORMATION, "ADT verify results: good = 0, bad = 176, unused = 0"),
        "ADT verify results: good = 0, bad = 176, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end LC_ValidateADT_Test_ValidateRpnAdtValError */

void LC_ValidateADT_Test_ValidateRpnAdtValNoError(void)
{
    uint8             Result;
    uint32            TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_ACTIONPOINTS; TableIndex++ )
    {
        LC_OperData.ADTPtr[TableIndex].DefaultState = LC_APSTATE_ACTIVE;
        LC_OperData.ADTPtr[TableIndex].RTSId = 1;
        LC_OperData.ADTPtr[TableIndex].MaxFailsBeforeRTS = 88;
        LC_OperData.ADTPtr[TableIndex].EventType = CFE_EVS_DEBUG;

        LC_OperData.ADTPtr[TableIndex].RPNEquation[0] = LC_MAX_WATCHPOINTS - 1;
        LC_OperData.ADTPtr[TableIndex].RPNEquation[1] = LC_RPN_NOT;
        LC_OperData.ADTPtr[TableIndex].RPNEquation[2] = LC_RPN_EQUAL;
    }

    /* Execute the function being tested */
    Result = LC_ValidateADT(LC_OperData.ADTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_NO_ERR, "Result == LC_ADTVAL_NO_ERR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADTVAL_INF_EID, CFE_EVS_INFORMATION, "ADT verify results: good = 176, bad = 0, unused = 0"),
        "ADT verify results: good = 176, bad = 0, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ValidateADT_Test_ValidateRpnAdtValNoError */

void LC_ValidateRPN_Test_RpnAnd(void)
{
    uint8             Result;

    int32  IndexValue, StackDepthValue;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_MAX_WATCHPOINTS - 1;
    LC_OperData.ADTPtr[0].RPNEquation[1] = LC_MAX_WATCHPOINTS - 2;
    LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_AND;
    LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_NO_ERR, "Result == LC_ADTVAL_NO_ERR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ValidateRPN_Test_RpnAnd */

void LC_ValidateRPN_Test_RpnOr(void)
{
    uint8             Result;

    int32  IndexValue, StackDepthValue;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_MAX_WATCHPOINTS - 1;
    LC_OperData.ADTPtr[0].RPNEquation[1] = LC_MAX_WATCHPOINTS - 2;
    LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_OR;
    LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_NO_ERR, "Result == LC_ADTVAL_NO_ERR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ValidateRPN_Test_RpnOr */

void LC_ValidateRPN_Test_RpnXor(void)
{
    uint8             Result;

    int32  IndexValue, StackDepthValue;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_MAX_WATCHPOINTS - 1;
    LC_OperData.ADTPtr[0].RPNEquation[1] = LC_MAX_WATCHPOINTS - 2;
    LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_XOR;
    LC_OperData.ADTPtr[0].RPNEquation[3] = LC_RPN_EQUAL;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_NO_ERR, "Result == LC_ADTVAL_NO_ERR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ValidateRPN_Test_RpnXor */

void LC_ValidateRPN_Test_RpnNot(void)
{
    uint8             Result;

    int32  IndexValue, StackDepthValue;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_MAX_WATCHPOINTS - 1;
    LC_OperData.ADTPtr[0].RPNEquation[1] = LC_RPN_NOT;
    LC_OperData.ADTPtr[0].RPNEquation[2] = LC_RPN_EQUAL;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_NO_ERR, "Result == LC_ADTVAL_NO_ERR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ValidateRPN_Test_RpnNot */

void LC_ValidateRPN_Test_StackDepthZero(void)
{
    uint8             Result;

    int32  IndexValue, StackDepthValue;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_RPN_NOT;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_ERR_RPN, "Result == LC_ADTVAL_ERR_RPN");
    UtAssert_True (IndexValue == 0, "IndexValue == 0");
    UtAssert_True (StackDepthValue == 0, "StackDepthValue == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ValidateRPN_Test_StackDepthZero */

void LC_ValidateRPN_Test_InvalidBufferItem(void)
{
    uint8             Result;

    int32  IndexValue, StackDepthValue;

    LC_OperData.ADTPtr[0].RPNEquation[0] = LC_MAX_WATCHPOINTS;

    /* Execute the function being tested */
    Result = LC_ValidateRPN(LC_OperData.ADTPtr[0].RPNEquation, &IndexValue, &StackDepthValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_ADTVAL_ERR_RPN, "Result == LC_ADTVAL_ERR_RPN");
    UtAssert_True (IndexValue == 0, "IndexValue == 0");
    UtAssert_True (StackDepthValue == 0, "StackDepthValue == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ValidateRPN_Test_InvalidBufferItem */

void LC_Action_Test_AddTestCases(void)
{
    UtTest_Add(LC_SampleAPs_Test_SingleActionPointNominal, LC_Test_Setup, LC_Test_TearDown, "LC_SampleAPs_Test_SingleActionPointNominal");
    UtTest_Add(LC_SampleAPs_Test_SingleActionPointError, LC_Test_Setup, LC_Test_TearDown, "LC_SampleAPs_Test_SingleActionPointError");
    UtTest_Add(LC_SampleAPs_Test_MultiActionPointNominal, LC_Test_Setup, LC_Test_TearDown, "LC_SampleAPs_Test_MultiActionPointNominal");

    UtTest_Add(LC_SampleSingleAP_Test_StateChangePassToFail, LC_Test_Setup, LC_Test_TearDown, "LC_SampleSingleAP_Test_StateChangePassToFail");
    UtTest_Add(LC_SampleSingleAP_Test_ActiveRequestRTS, LC_Test_Setup, LC_Test_TearDown, "LC_SampleSingleAP_Test_ActiveRequestRTS");
    UtTest_Add(LC_SampleSingleAP_Test_APFailWhileLCStatePassive, LC_Test_Setup, LC_Test_TearDown, "LC_SampleSingleAP_Test_APFailWhileLCStatePassive");
    UtTest_Add(LC_SampleSingleAP_Test_APFailWhilePassive, LC_Test_Setup, LC_Test_TearDown, "LC_SampleSingleAP_Test_APFailWhilePassive");
    UtTest_Add(LC_SampleSingleAP_Test_StateChangeFailToPass, LC_Test_Setup, LC_Test_TearDown, "LC_SampleSingleAP_Test_StateChangeFailToPass");
    UtTest_Add(LC_SampleSingleAP_Test_ActionStale, LC_Test_Setup, LC_Test_TearDown, "LC_SampleSingleAP_Test_ActionStale");
    UtTest_Add(LC_SampleSingleAP_Test_ActionError, LC_Test_Setup, LC_Test_TearDown, "LC_SampleSingleAP_Test_ActionError");

    UtTest_Add(LC_EvaluateRPN_Test_AndWatchFalseOp1, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_AndWatchFalseOp1");
    UtTest_Add(LC_EvaluateRPN_Test_AndWatchFalseOp2, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_AndWatchFalseOp2");
    UtTest_Add(LC_EvaluateRPN_Test_OrWatchTrueOp1, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_OrWatchTrueOp1");
    UtTest_Add(LC_EvaluateRPN_Test_OrWatchTrueOp2, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_OrWatchTrueOp2");
    UtTest_Add(LC_EvaluateRPN_Test_AndWatchErrorOp1, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_AndWatchErrorOp1");
    UtTest_Add(LC_EvaluateRPN_Test_AndWatchErrorOp2, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_AndWatchErrorOp2");
    UtTest_Add(LC_EvaluateRPN_Test_OrWatchErrorOp1, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_OrWatchErrorOp1");
    UtTest_Add(LC_EvaluateRPN_Test_OrWatchErrorOp2, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_OrWatchErrorOp2");
    UtTest_Add(LC_EvaluateRPN_Test_XorWatchErrorOp1, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_XorWatchErrorOp1");
    UtTest_Add(LC_EvaluateRPN_Test_XorWatchErrorOp2, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_XorWatchErrorOp2");
    UtTest_Add(LC_EvaluateRPN_Test_NotWatchError, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_NotWatchError");
    UtTest_Add(LC_EvaluateRPN_Test_AndStaleOp1, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_AndStaleOp1");
    UtTest_Add(LC_EvaluateRPN_Test_AndStaleOp2, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_AndStaleOp2");
    UtTest_Add(LC_EvaluateRPN_Test_OrStaleOp1, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_OrStaleOp1");
    UtTest_Add(LC_EvaluateRPN_Test_OrStaleOp2, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_OrStaleOp2");
    UtTest_Add(LC_EvaluateRPN_Test_XorStaleOp1, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_XorStaleOp1");
    UtTest_Add(LC_EvaluateRPN_Test_XorStaleOp2, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_XorStaleOp2");
    UtTest_Add(LC_EvaluateRPN_Test_NotStale, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_NotStale");
    UtTest_Add(LC_EvaluateRPN_Test_AndNominal, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_AndNominal");
    UtTest_Add(LC_EvaluateRPN_Test_OrNominal, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_OrNominal");
    UtTest_Add(LC_EvaluateRPN_Test_XorNominal, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_XorNominal");
    UtTest_Add(LC_EvaluateRPN_Test_NotNominal, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_NotNominal");
    UtTest_Add(LC_EvaluateRPN_Test_EqualIllegalRPN, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_EqualIllegalRPN");
    UtTest_Add(LC_EvaluateRPN_Test_WatchpointNumberNominal, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_WatchpointNumberNominal");
    UtTest_Add(LC_EvaluateRPN_Test_DefaultIllegalRPN, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_DefaultIllegalRPN");
    UtTest_Add(LC_EvaluateRPN_Test_EndOfBufferWhenNotDone, LC_Test_Setup, LC_Test_TearDown, "LC_EvaluateRPN_Test_EndOfBufferWhenNotDone");

    UtTest_Add(LC_ValidateADT_Test_ActionNotUsed, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateADT_Test_ActionNotUsed");
    UtTest_Add(LC_ValidateADT_Test_InvalidDefaultState, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateADT_Test_InvalidDefaultState");
    UtTest_Add(LC_ValidateADT_Test_BadRtsID, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateADT_Test_BadRtsID");
    UtTest_Add(LC_ValidateADT_Test_BadFailCount, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateADT_Test_BadFailCount");
    UtTest_Add(LC_ValidateADT_Test_InvalidEventType, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateADT_Test_InvalidEventType");
    UtTest_Add(LC_ValidateADT_Test_ValidateRpnAdtValError, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateADT_Test_ValidateRpnAdtValError");
    UtTest_Add(LC_ValidateADT_Test_ValidateRpnAdtValNoError, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateADT_Test_ValidateRpnAdtValNoError");

    UtTest_Add(LC_ValidateRPN_Test_RpnAnd, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateRPN_Test_RpnAnd");
    UtTest_Add(LC_ValidateRPN_Test_RpnOr, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateRPN_Test_RpnOr");
    UtTest_Add(LC_ValidateRPN_Test_RpnXor, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateRPN_Test_RpnXor");
    UtTest_Add(LC_ValidateRPN_Test_RpnNot, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateRPN_Test_RpnNot");
    UtTest_Add(LC_ValidateRPN_Test_StackDepthZero, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateRPN_Test_StackDepthZero");
    UtTest_Add(LC_ValidateRPN_Test_InvalidBufferItem, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateRPN_Test_InvalidBufferItem");

} /* end LC_Action_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
