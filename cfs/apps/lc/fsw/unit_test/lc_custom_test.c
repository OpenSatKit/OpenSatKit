 /*************************************************************************
 ** File:
 **   $Id: lc_custom_test.c 1.3 2017/01/22 17:24:34EST sstrege Exp  $
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
 **   This file contains unit test cases for the functions contained in the file lc_custom.c
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

#include "lc_custom_test.h"
#include "lc_custom.h"
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

void LC_ExecuteRTS_Test(void)
{
    uint16   RTSId = 1;

    /* Execute the function being tested */
    LC_ExecuteRTS(RTSId);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ExecuteRTS_Test */

void LC_CustomFunction_Test_WatchIndex0(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;

    /* Execute the function being tested */
    Result = LC_CustomFunction(WatchIndex, 0, 0, 0);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_CFCALL_ERR_EID, CFE_EVS_ERROR, "Unexpected LC_CustomFunction call: WP = 0"),
        "Unexpected LC_CustomFunction call: WP = 0");

    UtAssert_True (Result == LC_WATCH_FALSE, "Result == LC_WATCH_FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CustomFunction_Test_WatchIndex0 */

void LC_CustomFunction_Test_WatchIndex1(void)
{
    uint8    Result;
    uint16   WatchIndex = 1;

    /* Execute the function being tested */
    Result = LC_CustomFunction(WatchIndex, 0, 0, 0);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_CFCALL_ERR_EID, CFE_EVS_ERROR, "Unexpected LC_CustomFunction call: WP = 1"),
        "Unexpected LC_CustomFunction call: WP = 1");

    UtAssert_True (Result == LC_WATCH_FALSE, "Result == LC_WATCH_FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CustomFunction_Test_WatchIndex1 */

void LC_Custom_Test_AddTestCases(void)
{
    UtTest_Add(LC_ExecuteRTS_Test, LC_Test_Setup, LC_Test_TearDown, "LC_ExecuteRTS_Test");
    UtTest_Add(LC_CustomFunction_Test_WatchIndex0, LC_Test_Setup, LC_Test_TearDown, "LC_CustomFunction_Test_WatchIndex0");
    UtTest_Add(LC_CustomFunction_Test_WatchIndex1, LC_Test_Setup, LC_Test_TearDown, "LC_CustomFunction_Test_WatchIndex1");

} /* end LC_Custom_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
