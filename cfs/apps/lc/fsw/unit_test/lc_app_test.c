 /*************************************************************************
 ** File:
 **   $Id: lc_app_test.c 1.5 2017/01/22 17:24:33EST sstrege Exp  $
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
 **   This file contains unit test cases for the functions contained in the file lc_app.c
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

#include "lc_app_test.h"
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

uint16 LC_APP_TEST_CFE_TBL_RegisterHookCount;
int32 LC_APP_TEST_CFE_TBL_RegisterHook1(CFE_TBL_Handle_t *TblHandlePtr, const char *Name, uint32  Size, uint16 TblOptionFlags, CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr)
{
    LC_APP_TEST_CFE_TBL_RegisterHookCount++;

    if (LC_APP_TEST_CFE_TBL_RegisterHookCount == 1)
        return 99;
    else
        return CFE_TBL_INFO_RECOVERED_TBL;
}

int32 LC_APP_TEST_CFE_TBL_RegisterHook2(CFE_TBL_Handle_t *TblHandlePtr, const char *Name, uint32  Size, uint16 TblOptionFlags, CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr)
{
    LC_APP_TEST_CFE_TBL_RegisterHookCount++;

    if (LC_APP_TEST_CFE_TBL_RegisterHookCount == 2)
        return 99;
    else
        return CFE_TBL_INFO_RECOVERED_TBL;
}

int32 LC_APP_TEST_CFE_TBL_RegisterHook3(CFE_TBL_Handle_t *TblHandlePtr, const char *Name, uint32  Size, uint16 TblOptionFlags, CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr)
{
    LC_APP_TEST_CFE_TBL_RegisterHookCount++;

    if (LC_APP_TEST_CFE_TBL_RegisterHookCount == 1)
        return CFE_TBL_INFO_RECOVERED_TBL;
    else
        return CFE_SUCCESS;
}

int32 LC_APP_TEST_CFE_TBL_RegisterHook4(CFE_TBL_Handle_t *TblHandlePtr, const char *Name, uint32  Size, uint16 TblOptionFlags, CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr)
{
    LC_APP_TEST_CFE_TBL_RegisterHookCount++;

    if (LC_APP_TEST_CFE_TBL_RegisterHookCount == 1)
        return CFE_TBL_INFO_RECOVERED_TBL;
    if (LC_APP_TEST_CFE_TBL_RegisterHookCount == 2)
        return CFE_SUCCESS;
    else
        return -1;
}

void LC_AppMain_Test_Nominal(void)
{
    LC_OperData.HaveActiveCDS = TRUE;
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 99, 1);

    /* Set to make HK_AppInit return CFE_SUCCESS */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to prevent call to CFE_SB_RcvMsg from returning an error */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Execute the function being tested */
    LC_AppMain();
    
    /* Verify results */

#ifndef LC_SAVE_TO_CDS /* When this is not defined EVENT ID=23 is always issued (LC use of Critical Data Store disabled) */
    /* Generates 2 event messages we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
#else /* Alt confg unit tests */
    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
#endif

} /* end LC_AppMain_Test_Nominal */

void LC_AppMain_Test_RegisterAppError(void)
{
    LC_OperData.HaveActiveCDS = TRUE;
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    /* Set to satisfy condition "Status != CFE_SUCCESS" immediately following call to CFE_ES_RegisterApp */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERAPP_INDEX, -1, 1);

    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 99, 1);

    /* Set to make HK_AppInit return CFE_SUCCESS */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to prevent call to CFE_SB_RcvMsg from returning an error */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Execute the function being tested */
    LC_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_TASK_EXIT_EID, CFE_EVS_CRITICAL, "Task terminating, err = 0xFFFFFFFF"),
        "Task terminating, err = 0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("LC task terminating, err = 0xFFFFFFFF\n"),
        "LC task terminating, err = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_AppMain_Test_RegisterAppError */

void LC_AppMain_Test_AppInitError(void)
{
    LC_OperData.HaveActiveCDS = TRUE;
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 99, 1);

    /* Set to make HK_AppInit return CFE_SUCCESS */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to prevent call to CFE_SB_RcvMsg from returning an error */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Set to make subfunction LC_AppInit return -1, in order to fail subsequent condition "Status != CFE_SUCCESS" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    LC_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_TASK_EXIT_EID, CFE_EVS_CRITICAL, "Task terminating, err = 0xFFFFFFFF"),
        "Task terminating, err = 0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("LC task terminating, err = 0xFFFFFFFF\n"),
        "LC task terminating, err = 0xFFFFFFFF");

    /* Generates 2 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end LC_AppMain_Test_AppInitError */

void LC_AppMain_Test_RcvMsgError(void)
{
    LC_OperData.HaveActiveCDS = TRUE;
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 99, 1);

    /* Set to make HK_AppInit return CFE_SUCCESS */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to fail condition "Status != CFE_SUCCESS" immediately following call to CFE_SB_RcvMsg */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 1);

    /* Execute the function being tested */
    LC_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_TASK_EXIT_EID, CFE_EVS_CRITICAL, "Task terminating, err = 0xFFFFFFFF"),
        "Task terminating, err = 0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("LC task terminating, err = 0xFFFFFFFF\n"),
        "LC task terminating, err = 0xFFFFFFFF");

    /* Generates 3 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end LC_AppMain_Test_RcvMsgError */

void LC_AppInit_Test_Nominal(void)
{
    int32   Result;
    char    Message[125];

    CFE_PSP_MemSet(&LC_OperData, 1, sizeof(LC_OperData_t));
    CFE_PSP_MemSet(&LC_AppData, 1, sizeof(LC_AppData_t));

    /* Set to prevent errors when initializing tables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = LC_AppInit();
    
    /* Verify results */
    sprintf(Message, "LC Initialized. Version %d.%d.%d.%d", LC_MAJOR_VERSION, LC_MINOR_VERSION, LC_REVISION, LC_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(LC_INIT_INF_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(LC_OperData.MsgPtr == 0, "LC_OperData.MsgPtr == 0");
    UtAssert_True(LC_OperData.CmdPipe == 0, "LC_OperData.CmdPipe == 0");
/* Note: not testing LC_OperData.WDTPtr == 0, because it is modified by a subfunction */
/* Note: not testing LC_OperData.ADTPtr == 0, because it is modified by a subfunction */
/* Note: not testing LC_OperData.WRTPtr == 0, because it is modified by a subfunction */
/* Note: not testing LC_OperData.ARTPtr == 0, because it is modified by a subfunction */
/* Note: not testing LC_OperData.WDTHandle == 0, because it is modified by a subfunction */
/* Note: not testing LC_OperData.ADTHandle == 0, because it is modified by a subfunction */
    UtAssert_True(LC_OperData.WRTHandle == 0, "LC_OperData.WRTHandle == 0");
/* Note: not testing LC_OperData.ARTHandle == 0, because it is modified by a subfunction */
    UtAssert_True(LC_OperData.WRTDataCDSHandle == 0, "LC_OperData.WRTDataCDSHandle == 0");
    UtAssert_True(LC_OperData.ARTDataCDSHandle == 0, "LC_OperData.ARTDataCDSHandle == 0");
    UtAssert_True(LC_OperData.AppDataCDSHandle == 0, "LC_OperData.AppDataCDSHandle == 0");
/* Note: not testing LC_OperData.WatchpointCount == 0, because it is modified by a subfunction */
/* Note: not testing LC_OperData.MessageIDsCount == 0, because it is modified by a subfunction */
/* Note: not testing LC_OperData.HkPacket == 0, because it is modified by a subfunction */
/* Note: not testing LC_OperData.TableResults == 0, because it is modified by a subfunction */
/* Note: not testing LC_OperData.HashTable == 0, because it is modified by a subfunction */
/* Note: not testing LC_OperData.MessageLinks == 0, because it is modified by a subfunction */
/* Note: not testing LC_OperData.WatchPtLinks == 0, because it is modified by a subfunction */
    UtAssert_True(LC_OperData.HaveActiveCDS == 0, "LC_OperData.HaveActiveCDS == 0");

    UtAssert_True(LC_AppData.CmdCount == 0, "LC_AppData.CmdCount == 0");
    UtAssert_True(LC_AppData.CmdErrCount == 0, "LC_AppData.CmdErrCount == 0");
    UtAssert_True(LC_AppData.APSampleCount == 0, "LC_AppData.APSampleCount == 0");
    UtAssert_True(LC_AppData.MonitoredMsgCount == 0, "LC_AppData.MonitoredMsgCount == 0");
    UtAssert_True(LC_AppData.RTSExecCount == 0, "LC_AppData.RTSExecCount == 0");
    UtAssert_True(LC_AppData.PassiveRTSExecCount == 0, "LC_AppData.PassiveRTSExecCount == 0");
    UtAssert_True(LC_AppData.CDSSavedOnExit == 0, "LC_AppData.CDSSavedOnExit == 0");
/* Note: not testing LC_AppData.CurrentLCState == 0, because it is modified by a subfunction */

#ifndef LC_SAVE_TO_CDS /* When this is not defined EVENT ID=23 is always issued (LC use of Critical Data Store disabled) */
    /* Generates 3 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
#else  /* Alt confg unit tests */
    /* Generates 2 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
#endif

} /* end LC_AppInit_Test_Nominal */

void LC_AppInit_Test_EVSInitError(void)
{
    int32 Result;

    /* Set to prevent errors when initializing tables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to cause LC_EvsInit to return -1, in order to satisfy subsequent condition "Status != CFE_SUCCESS" */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_AppInit();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_AppInit_Test_EVSInitError */

void LC_AppInit_Test_SBInitError(void)
{
    int32 Result;

    /* Set to prevent errors when initializing tables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to cause LC_SbInit to return -1, in order to satisfy subsequent condition "Status != CFE_SUCCESS" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_AppInit();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    /* Generates one event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_AppInit_Test_SBInitError */

void LC_AppInit_Test_TableInitError(void)
{
    int32 Result;

    /* Set to cause LC_TableInit to return -1, in order to satisfy subsequent condition "Status != CFE_SUCCESS" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = LC_AppInit();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    /* Generates one event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_AppInit_Test_TableInitError */

void LC_EvsInit_Test_Nominal(void)
{
    int32 Result;

    /* Execute the function being tested */
    Result = LC_EvsInit();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvsInit_Test_Nominal */

void LC_EvsInit_Test_EVSRegisterError(void)
{
    int32 Result;

    /* Set to generate system log message "LC App: Error Registering For Event Services" */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_EvsInit();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("LC App: Error Registering For Event Services, RC = 0xFFFFFFFF\n"),
        "LC App: Error Registering For Event Services, RC = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_EvsInit_Test_EVSRegisterError */

void LC_SbInit_Test_Nominal(void)
{
    int32 Result;

    /* Execute the function being tested */
    Result = LC_SbInit();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SbInit_Test_Nominal */

void LC_SbInit_Test_CreatePipeError(void)
{
    int32 Result;

    /* Set to generate error message LC_CR_PIPE_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_SbInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_CR_PIPE_ERR_EID, CFE_EVS_ERROR, "Error Creating LC Pipe, RC=0xFFFFFFFF"),
        "Error Creating LC Pipe, RC=0xFFFFFFFF");

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SbInit_Test_CreatePipeError */

void LC_SbInit_Test_SubscribeHKReqError(void)
{
    int32 Result;

    /* Set to generate error message LC_SUB_HK_REQ_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_SbInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_SUB_HK_REQ_ERR_EID, CFE_EVS_ERROR, "Error Subscribing to HK Request, MID=0x18A5, RC=0xFFFFFFFF"),
        "Error Subscribing to HK Request, MID=0x18A5, RC=0xFFFFFFFF");

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SbInit_Test_SubscribeHKReqError */

void LC_SbInit_Test_SubscribeGndCmdError(void)
{
    int32 Result;

    /* Set to generate error message LC_SUB_GND_CMD_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = LC_SbInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_SUB_GND_CMD_ERR_EID, CFE_EVS_ERROR, "Error Subscribing to GND CMD, MID=0x18A4, RC=0xFFFFFFFF"),
        "Error Subscribing to GND CMD, MID=0x18A4, RC=0xFFFFFFFF");

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SbInit_Test_SubscribeGndCmdError */

void LC_SbInit_Test_SubscribeSampleCmdError(void)
{
    int32 Result;

    /* Set to generate error message LC_SUB_SAMPLE_CMD_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 3);

    /* Execute the function being tested */
    Result = LC_SbInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_SUB_SAMPLE_CMD_ERR_EID, CFE_EVS_ERROR, "Error Subscribing to Sample CMD, MID=0x18A6, RC=0xFFFFFFFF"),
        "Error Subscribing to Sample CMD, MID=0x18A6, RC=0xFFFFFFFF");

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SbInit_Test_SubscribeSampleCmdError */

void LC_CreateResultTables_Test_Nominal(void)
{
    int32 Result;

    LC_OperData.TableResults = 0;

    /* Execute the function being tested */
    Result = LC_CreateResultTables();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.TableResults == (LC_WRT_TBL_CREATED | LC_ART_TBL_CREATED), "LC_OperData.TableResults == (LC_WRT_TBL_CREATED | LC_ART_TBL_CREATED)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_CreateResultTables_Test_Nominal */

void LC_CreateResultTables_Test_WRTRegisterError(void)
{
    int32 Result;

    LC_OperData.TableResults = 0;

    /* Set to generate error message LC_WRT_REGISTER_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_CreateResultTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WRT_REGISTER_ERR_EID, CFE_EVS_ERROR, "Error registering WRT, RC=0xFFFFFFFF"),
        "Error registering WRT, RC=0xFFFFFFFF");

    UtAssert_True(LC_OperData.TableResults == 0, "LC_OperData.TableResults == 0");
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CreateResultTables_Test_WRTRegisterError */

void LC_CreateResultTables_Test_WRTGetAddressError(void)
{
    int32 Result;

    LC_OperData.TableResults = 0;

    /* Set to generate error message LC_WRT_GETADDR_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_CreateResultTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WRT_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting WRT address, RC=0xFFFFFFFF"),
        "Error getting WRT address, RC=0xFFFFFFFF");

    UtAssert_True(LC_OperData.TableResults == 0, "LC_OperData.TableResults == 0");
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CreateResultTables_Test_WRTGetAddressError */

void LC_CreateResultTables_Test_ARTRegisterError(void)
{
    int32 Result;

    LC_OperData.TableResults = 0;

    /* Set to generate error message LC_ART_REGISTER_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = LC_CreateResultTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ART_REGISTER_ERR_EID, CFE_EVS_ERROR, "Error registering ART, RC=0xFFFFFFFF"),
        "Error registering ART, RC=0xFFFFFFFF");

    UtAssert_True(LC_OperData.TableResults == LC_WRT_TBL_CREATED, "LC_OperData.TableResults == LC_WRT_TBL_CREATED");
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CreateResultTables_Test_ARTRegisterError */

void LC_CreateResultTables_Test_ARTGetAddressError(void)
{
    int32 Result;

    LC_OperData.TableResults = 0;

    /* Set to generate error message LC_ART_GETADDR_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = LC_CreateResultTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ART_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting ART address, RC=0xFFFFFFFF"),
        "Error getting ART address, RC=0xFFFFFFFF");

    UtAssert_True(LC_OperData.TableResults == LC_WRT_TBL_CREATED, "LC_OperData.TableResults == LC_WRT_TBL_CREATED");
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CreateResultTables_Test_ARTGetAddressError */

void LC_CreateDefinitionTables_Test_NominalCDSActive(void)
{
    int32 Result;

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = TRUE;

    /* Set to reach all nominal code branches */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_REGISTER_INDEX);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_ADT_CRITICAL_TBL), "LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_ADT_CRITICAL_TBL)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_CreateDefinitionTables_Test_NominalCDSActive */

void LC_CreateDefinitionTables_Test_NominalCDSInactive(void)
{
    int32 Result;

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = FALSE;

    /* Set to reach all nominal code branches */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_REGISTER_INDEX);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.TableResults == (LC_WDT_NOT_CRITICAL | LC_ADT_NOT_CRITICAL), "LC_OperData.TableResults == (LC_WDT_NOT_CRITICAL | LC_ADT_NOT_CRITICAL)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_CreateDefinitionTables_Test_NominalCDSInactive */

void LC_CreateDefinitionTables_Test_WDTReRegisterThenInfoRecovered(void)
{
    int32 Result;

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = TRUE;

    /* Satisfies condition "(LC_OperData.HaveActiveCDS) && ((Result != CFE_TBL_INFO_RECOVERED_TBL) && (Result != CFE_SUCCESS))"
       and then satisfies condition "Result == CFE_TBL_INFO_RECOVERED_TBL" */
    LC_APP_TEST_CFE_TBL_RegisterHookCount = 0;
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_REGISTER_INDEX, &LC_APP_TEST_CFE_TBL_RegisterHook1);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.HaveActiveCDS == FALSE, "LC_OperData.HaveActiveCDS == FALSE");
    UtAssert_True(LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_CRITICAL_TBL | LC_ADT_TBL_RESTORED),
                 "LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_CRITICAL_TBL | LC_ADT_TBL_RESTORED)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_CreateDefinitionTables_Test_WDTReRegisterThenInfoRecovered */

void LC_CreateDefinitionTables_Test_ADTReRegisterThenInfoRecovered(void)
{
    int32 Result;

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = TRUE;

    /* Satisfies condition "(LC_OperData.HaveActiveCDS) && ((Result != CFE_TBL_INFO_RECOVERED_TBL) && (Result != CFE_SUCCESS))"
       and then satisfies condition "Result == CFE_TBL_INFO_RECOVERED_TBL" */
    LC_APP_TEST_CFE_TBL_RegisterHookCount = 0;
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_REGISTER_INDEX, &LC_APP_TEST_CFE_TBL_RegisterHook2);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.HaveActiveCDS == FALSE, "LC_OperData.HaveActiveCDS == FALSE");
    UtAssert_True(LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_CRITICAL_TBL | LC_ADT_TBL_RESTORED),
                 "LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_CRITICAL_TBL | LC_ADT_TBL_RESTORED)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_CreateDefinitionTables_Test_ADTReRegisterThenInfoRecovered */

void LC_CreateDefinitionTables_Test_WDTCriticalADTNoncritical(void)
{
    int32 Result;

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = FALSE;

    /* Satisfies condition "((LC_OperData.TableResults & LC_WDT_CRITICAL_TBL) == LC_WDT_CRITICAL_TBL) && 
       ((LC_OperData.TableResults & LC_ADT_NOT_CRITICAL) == LC_ADT_NOT_CRITICAL)" */
    LC_APP_TEST_CFE_TBL_RegisterHookCount = 0;
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_REGISTER_INDEX, &LC_APP_TEST_CFE_TBL_RegisterHook3);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_NOT_CRITICAL | LC_WDT_NOT_CRITICAL),
                 "LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_NOT_CRITICAL | LC_WDT_NOT_CRITICAL)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_CreateDefinitionTables_Test_WDTCriticalADTNoncritical */

void LC_CreateDefinitionTables_Test_WDTReRegisterError(void)
{
    int32 Result;

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = FALSE;

    /* Satisfies condition "((LC_OperData.TableResults & LC_WDT_CRITICAL_TBL) == LC_WDT_CRITICAL_TBL) && 
       ((LC_OperData.TableResults & LC_ADT_NOT_CRITICAL) == LC_ADT_NOT_CRITICAL)"
       and then FAILS condition "Result == CFE_SUCCESS", in order to generate error message LC_WDT_REREGISTER_ERR_EID */
    LC_APP_TEST_CFE_TBL_RegisterHookCount = 0;
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_REGISTER_INDEX, &LC_APP_TEST_CFE_TBL_RegisterHook4);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
    UtAssert_True(LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_NOT_CRITICAL),
                 "LC_OperData.TableResults == (LC_WDT_CRITICAL_TBL | LC_WDT_TBL_RESTORED | LC_ADT_NOT_CRITICAL)");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDT_REREGISTER_ERR_EID, CFE_EVS_ERROR, "Error re-registering WDT, RC=0xFFFFFFFF"),
        "Error re-registering WDT, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CreateDefinitionTables_Test_WDTReRegisterError */

void LC_CreateDefinitionTables_Test_WDTRegisterError(void)
{
    int32 Result;

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = FALSE;

    /* Set to generate error message LC_WDT_REGISTER_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDT_REGISTER_ERR_EID, CFE_EVS_ERROR, "Error registering WDT, RC=0xFFFFFFFF"),
        "Error registering WDT, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CreateDefinitionTables_Test_WDTRegisterError */

void LC_CreateDefinitionTables_Test_ADTRegisterError(void)
{
    int32 Result;

    LC_OperData.TableResults  = 0;
    LC_OperData.HaveActiveCDS = FALSE;

    /* Set to generate error message LC_ADT_REGISTER_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = LC_CreateDefinitionTables();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADT_REGISTER_ERR_EID, CFE_EVS_ERROR, "Error registering ADT, RC=0xFFFFFFFF"),
        "Error registering ADT, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CreateDefinitionTables_Test_ADTRegisterError */

void LC_CreateTaskCDS_Test_NominalPowerOnReset(void)
{
    int32 Result;

    LC_OperData.TableResults = 0;

    /* Execute the function being tested */
    Result = LC_CreateTaskCDS();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");
    UtAssert_True(LC_OperData.TableResults == (LC_WRT_CDS_CREATED | LC_ART_CDS_CREATED | LC_APP_CDS_CREATED), "LC_OperData.TableResults == (LC_WRT_CDS_CREATED | LC_ART_CDS_CREATED | LC_APP_CDS_CREATED)");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_CreateTaskCDS_Test_NominalPowerOnReset */

void LC_CreateTaskCDS_Test_NominalProcessorReset(void)
{
    int32 Result;

    LC_OperData.TableResults  = 0;
    LC_AppData.CDSSavedOnExit = LC_CDS_SAVED;

    /* Set to satisfy all instances of condition "Result == CFE_ES_CDS_ALREADY_EXISTS" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, CFE_ES_CDS_ALREADY_EXISTS, 1);
    Ut_CFE_ES_ContinueReturnCodeAfterCountZero(UT_CFE_ES_REGISTERCDS_INDEX);

    /* Execute the function being tested */
    Result = LC_CreateTaskCDS();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(LC_OperData.TableResults == (LC_WRT_CDS_CREATED | LC_ART_CDS_CREATED | LC_APP_CDS_CREATED | LC_WRT_CDS_RESTORED | LC_ART_CDS_RESTORED | LC_APP_CDS_RESTORED),
                 "LC_OperData.TableResults == (LC_WRT_CDS_CREATED | LC_ART_CDS_CREATED | LC_APP_CDS_CREATED | LC_WRT_CDS_RESTORED | LC_ART_CDS_RESTORED | LC_APP_CDS_RESTORED)");

// FIXME: This depends on a preprocessor constant value having a different value than default.  The comparison is not done with a #if - should it?
//    UtAssert_True(LC_AppData.CurrentLCState == LC_STATE_WHEN_CDS_RESTORED, "LC_AppData.CurrentLCState == LC_STATE_WHEN_CDS_RESTORED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_CreateTaskCDS_Test_NominalProcessorReset */

void LC_CreateTaskCDS_Test_WRTRegisterCDSError(void)
{
    int32 Result;

    LC_OperData.TableResults = 0;

    /* Set to generate error message LC_WRT_CDS_REGISTER_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_CreateTaskCDS();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
    UtAssert_True(LC_OperData.TableResults == 0, "LC_OperData.TableResults == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WRT_CDS_REGISTER_ERR_EID, CFE_EVS_ERROR, "Error registering WRT CDS Area, RC=0xFFFFFFFF"),
        "Error registering WRT CDS Area, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CreateTaskCDS_Test_WRTRegisterCDSError */

void LC_CreateTaskCDS_Test_ARTRegisterCDSError(void)
{
    int32 Result;

    LC_OperData.TableResults = 0;

    /* Set to generate error message LC_ART_CDS_REGISTER_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = LC_CreateTaskCDS();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
    UtAssert_True(LC_OperData.TableResults == LC_WRT_CDS_CREATED, "LC_OperData.TableResults == LC_WRT_CDS_CREATED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ART_CDS_REGISTER_ERR_EID, CFE_EVS_ERROR, "Error registering ART CDS Area, RC=0xFFFFFFFF"),
        "Error registering ART CDS Area, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CreateTaskCDS_Test_ARTRegisterCDSError */

void LC_CreateTaskCDS_Test_AppDataRegisterCDSError(void)
{
    int32 Result;

    LC_OperData.TableResults = 0;

    /* Set to generate error message LC_APP_CDS_REGISTER_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, -1, 3);

    /* Execute the function being tested */
    Result = LC_CreateTaskCDS();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
    UtAssert_True(LC_OperData.TableResults == (LC_WRT_CDS_CREATED | LC_ART_CDS_CREATED), "LC_OperData.TableResults == (LC_WRT_CDS_CREATED | LC_ART_CDS_CREATED)");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_APP_CDS_REGISTER_ERR_EID, CFE_EVS_ERROR, "Error registering application data CDS Area, RC=0xFFFFFFFF"),
        "Error registering application data CDS Area, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CreateTaskCDS_Test_AppDataRegisterCDSError */

void LC_LoadDefaultTables_Test_NominalActiveCDS(void)
{
    int32             Result;

    LC_OperData.HaveActiveCDS = TRUE;

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_Load */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_GetAddress */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA | LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA | LC_CDS_UPDATED),
                 "LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA | LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA | LC_CDS_UPDATED)");

    UtAssert_True(LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET, "LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_LoadDefaultTables_Test_NominalActiveCDS */

void LC_LoadDefaultTables_Test_ActiveCDSNotSuccess(void)
{
    int32             Result;

    LC_OperData.HaveActiveCDS = TRUE;

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_Load */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_GetAddress */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to make subfunction LC_UpdateTaskCDS return -1, in order to satisfy condition "LC_UpdateTaskCDS() == CFE_SUCCESS" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_COPYTOCDS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA | LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA),
                 "LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA | LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA");

    UtAssert_True(LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET, "LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET");

    UtAssert_True(LC_OperData.HaveActiveCDS == FALSE, "LC_OperData.HaveActiveCDS == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates one event message we don't care about in this test */

} /* end LC_LoadDefaultTables_Test_ActiveCDSNotSuccess */

void LC_LoadDefaultTables_Test_NominalInactiveCDS(void)
{
    int32             Result;
    LC_OperData.HaveActiveCDS = FALSE;

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_Load */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_GetAddress */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA | LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA),
                 "LC_OperData.TableResults == (LC_WDT_DEFAULT_TBL | LC_ADT_DEFAULT_TBL | LC_WRT_DEFAULT_DATA | LC_ART_DEFAULT_DATA | LC_APP_DEFAULT_DATA)");

    UtAssert_True(LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET, "LC_AppData.CurrentLCState == LC_STATE_POWER_ON_RESET");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_LoadDefaultTables_Test_NominalInactiveCDS */

void LC_LoadDefaultTables_Test_LoadWDTError(void)
{
    int32             Result;

    LC_OperData.HaveActiveCDS = FALSE;

    /* Set to generate error message LC_WDT_LOAD_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDT_LOAD_ERR_EID, CFE_EVS_ERROR, "Error (RC=0xFFFFFFFF) Loading WDT with '" LC_WDT_FILENAME "'"),
        "Error (RC=0xFFFFFFFF) Loading WDT with '" LC_WDT_FILENAME "'");

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_LoadDefaultTables_Test_LoadWDTError */

void LC_LoadDefaultTables_Test_GetWDTAddressError(void)
{
    int32             Result;

    LC_OperData.HaveActiveCDS = FALSE;

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_Load */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to generate error message LC_WDT_GETADDR_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDT_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting WDT address, RC=0xFFFFFFFF"),
        "Error getting WDT address, RC=0xFFFFFFFF");

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_LoadDefaultTables_Test_GetWDTAddressError */

void LC_LoadDefaultTables_Test_LoadADTError(void)
{
    int32             Result;

    LC_OperData.HaveActiveCDS = FALSE;

    /* Set to satisfy condition "Result == CFE_SUCCESS" after first call to CFE_TBL_Load */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_GetAddress */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADT_LOAD_ERR_EID, CFE_EVS_ERROR, "Error (RC=0xCC000013) Loading ADT with '" LC_ADT_FILENAME "'"),
        "Error (RC=0xCC000013) Loading ADT with '" LC_ADT_FILENAME "'");

    UtAssert_True(Result == 0xCC000013, "Result == 0xCC000013");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_LoadDefaultTables_Test_LoadADTError */

void LC_LoadDefaultTables_Test_GetADTAddressError(void)
{
    int32             Result;

    LC_OperData.HaveActiveCDS = FALSE;

    /* Set to satisfy all instances of condition "Result == CFE_SUCCESS" after calls to CFE_TBL_Load */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to satisfy condition "Result == CFE_SUCCESS" after first call to CFE_TBL_GetAddress */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);

    /* Execute the function being tested */
    Result = LC_LoadDefaultTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADT_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting ADT address, RC=0xCC000001"),
        "Error getting ADT address, RC=0xCC000001");

    UtAssert_True(Result == 0xCC000001, "Result == 0xCC000001");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_LoadDefaultTables_Test_GetADTAddressError */

void LC_TableInit_Test_CreateResultTablesError(void)
{
    int32             Result;

    /* Set to make subfunction LC_CreateResultTables return -1, in order to satisfy condition "Result = LC_CreateResultTables()) != CFE_SUCCESS" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_TableInit();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates one event message we don't care about in this test */

} /* end LC_TableInit_Test_CreateResultTablesError */

void LC_TableInit_Test_CreateDefinitionTablesError(void)
{
    int32             Result;

    /* Set to make subfunction LC_CreateDefinitionTables return -1, in order to satisfy condition "Result = LC_CreateDefinitionTables()) != CFE_SUCCESS" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 3);

    /* Execute the function being tested */
    Result = LC_TableInit();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates one event message we don't care about in this test */

} /* end LC_TableInit_Test_CreateDefinitionTablesError */

void LC_TableInit_Test_LoadDefaultTables(void)
{
    int32             Result;

    /* Set to make subfunction LC_LoadDefaultTables return -1, in order to satisfy condition "Result = LC_LoadDefaultTables()) != CFE_SUCCESS" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_TableInit();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates one event message we don't care about in this test */

} /* end LC_TableInit_Test_LoadDefaultTables */

void LC_TableInit_Test_GetWDTAddressError(void)
{
    int32             Result;

    LC_OperData.TableResults = LC_WRT_CDS_RESTORED | LC_ART_CDS_RESTORED | LC_APP_CDS_RESTORED | LC_WDT_TBL_RESTORED | LC_ADT_TBL_RESTORED;

    /* Set to generate error message LC_WDT_GETADDR_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 3);

    /* Execute the function being tested */
    Result = LC_TableInit();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDT_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting WDT address, RC=0xFFFFFFFF"),
        "Error getting WDT address, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_TableInit_Test_GetWDTAddressError */

void LC_TableInit_Test_GetADTAddressError(void)
{
    int32             Result;

    LC_OperData.TableResults = LC_WRT_CDS_RESTORED | LC_ART_CDS_RESTORED | LC_APP_CDS_RESTORED | LC_WDT_TBL_RESTORED | LC_ADT_TBL_RESTORED;

    /* Set to generate error message LC_ADT_GETADDR_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 4);

    /* Execute the function being tested */
    Result = LC_TableInit();
    
    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_ADT_GETADDR_ERR_EID, CFE_EVS_ERROR, "Error getting ADT address, RC=0xFFFFFFFF"),
        "Error getting ADT address, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_TableInit_Test_GetADTAddressError */

void LC_TableInit_Test_CreateTaskCDSError(void)
{
    int32             Result;

    /* Set to make subfunction LC_CreateTaskCDS return -1, in order to satisfy condition "LC_CreateTaskCDS" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_TableInit();
    
    /* Verify results */
    UtAssert_True(LC_OperData.HaveActiveCDS == FALSE, "LC_OperData.HaveActiveCDS == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates two event messages we don't care about in this test */

} /* end LC_TableInit_Test_CreateTaskCDSError */

void LC_TableInit_Test_CDSRestored(void)
{
    int32             Result;

    LC_OperData.TableResults = LC_CDS_RESTORED;

    /* Set to prevent unintended error in LC_LoadDefaultTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = LC_TableInit();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_CDS_RESTORED_INF_EID, CFE_EVS_INFORMATION, "Previous state restored from Critical Data Store"),
        "Previous state restored from Critical Data Store");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_TableInit_Test_CDSRestored */

void LC_TableInit_Test_CDSUpdated(void)
{
    int32             Result;

    LC_OperData.TableResults = LC_CDS_UPDATED;

    /* Set to prevent unintended error in LC_LoadDefaultTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to generate event message LC_CDS_UPDATED_INF_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_TableInit();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_CDS_UPDATED_INF_EID, CFE_EVS_INFORMATION, "Default state loaded and written to CDS, activity mask = 0x00991999"),
        "Default state loaded and written to CDS, activity mask = 0x00991999");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates one event message we don't care about in this test */

} /* end LC_TableInit_Test_CDSUpdated */

void LC_TableInit_Test_NeitherRestoredNorUpdated(void)
{
    int32             Result;

    LC_OperData.TableResults = 0;

    /* Set to prevent unintended error in LC_LoadDefaultTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to generate event message LC_CDS_DISABLED_INF_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCDS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = LC_TableInit();
    
    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_CDS_DISABLED_INF_EID, CFE_EVS_INFORMATION, "LC use of Critical Data Store disabled, activity mask = 0x00991991"),
        "LC use of Critical Data Store disabled, activity mask = 0x00991991");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates one event message we don't care about in this test */

} /* end LC_TableInit_Test_NeitherRestoredNorUpdated */

void LC_App_Test_AddTestCases(void)
{
#ifndef LC_SAVE_TO_CDS /* default config unit test */
    UtTest_Add(LC_AppMain_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_AppMain_Test_Nominal");
#endif

    UtTest_Add(LC_AppMain_Test_RegisterAppError, LC_Test_Setup, LC_Test_TearDown, "LC_AppMain_Test_RegisterAppError");

#ifndef LC_SAVE_TO_CDS /* default config unit test */
    UtTest_Add(LC_AppMain_Test_AppInitError, LC_Test_Setup, LC_Test_TearDown, "LC_AppMain_Test_AppInitError");
    UtTest_Add(LC_AppMain_Test_RcvMsgError, LC_Test_Setup, LC_Test_TearDown, "LC_AppMain_Test_RcvMsgError");
#endif

#ifndef LC_SAVE_TO_CDS /* default config unit test */
    UtTest_Add(LC_AppInit_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_AppInit_Test_Nominal");
#endif

    UtTest_Add(LC_AppInit_Test_EVSInitError, LC_Test_Setup, LC_Test_TearDown, "LC_AppInit_Test_EVSInitError");
    UtTest_Add(LC_AppInit_Test_SBInitError, LC_Test_Setup, LC_Test_TearDown, "LC_AppInit_Test_SBInitError");
    UtTest_Add(LC_AppInit_Test_TableInitError, LC_Test_Setup, LC_Test_TearDown, "LC_AppInit_Test_TableInitError");

    UtTest_Add(LC_EvsInit_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_EvsInit_Test_Nominal");
    UtTest_Add(LC_EvsInit_Test_EVSRegisterError, LC_Test_Setup, LC_Test_TearDown, "LC_EvsInit_Test_EVSRegisterError");

    UtTest_Add(LC_SbInit_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_SbInit_Test_Nominal");
    UtTest_Add(LC_SbInit_Test_CreatePipeError, LC_Test_Setup, LC_Test_TearDown, "LC_SbInit_Test_CreatePipeError");
    UtTest_Add(LC_SbInit_Test_SubscribeHKReqError, LC_Test_Setup, LC_Test_TearDown, "LC_SbInit_Test_SubscribeHKReqError");
    UtTest_Add(LC_SbInit_Test_SubscribeGndCmdError, LC_Test_Setup, LC_Test_TearDown, "LC_SbInit_Test_SubscribeGndCmdError");
    UtTest_Add(LC_SbInit_Test_SubscribeSampleCmdError, LC_Test_Setup, LC_Test_TearDown, "LC_SbInit_Test_SubscribeSampleCmdError");

#ifdef LC_SAVE_TO_CDS  /* Alt confg unit tests */
    UtTest_Add(LC_TableInit_Test_CreateTaskCDSError, LC_Test_Setup, LC_Test_TearDown, "LC_TableInit_Test_CreateTaskCDSError");
    UtTest_Add(LC_TableInit_Test_CDSRestored, LC_Test_Setup, LC_Test_TearDown, "LC_TableInit_Test_CDSRestored");
    UtTest_Add(LC_TableInit_Test_CDSUpdated, LC_Test_Setup, LC_Test_TearDown, "LC_TableInit_Test_CDSUpdated");
#endif

    UtTest_Add(LC_TableInit_Test_CreateResultTablesError, LC_Test_Setup, LC_Test_TearDown, "LC_TableInit_Test_CreateResultTablesError");

#ifndef LC_SAVE_TO_CDS /* default config unit test */
    UtTest_Add(LC_TableInit_Test_CreateDefinitionTablesError, LC_Test_Setup, LC_Test_TearDown, "LC_TableInit_Test_CreateDefinitionTablesError");
#endif

    UtTest_Add(LC_TableInit_Test_LoadDefaultTables, LC_Test_Setup, LC_Test_TearDown, "LC_TableInit_Test_LoadDefaultTables");
    UtTest_Add(LC_TableInit_Test_GetWDTAddressError, LC_Test_Setup, LC_Test_TearDown, "LC_TableInit_Test_GetWDTAddressError");
    UtTest_Add(LC_TableInit_Test_GetADTAddressError, LC_Test_Setup, LC_Test_TearDown, "LC_TableInit_Test_GetADTAddressError");

    UtTest_Add(LC_CreateResultTables_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_CreateResultTables_Test_Nominal");
    UtTest_Add(LC_CreateResultTables_Test_WRTRegisterError, LC_Test_Setup, LC_Test_TearDown, "LC_CreateResultTables_Test_WRTRegisterError");
    UtTest_Add(LC_CreateResultTables_Test_WRTGetAddressError, LC_Test_Setup, LC_Test_TearDown, "LC_CreateResultTables_Test_WRTGetAddressError");
    UtTest_Add(LC_CreateResultTables_Test_ARTRegisterError, LC_Test_Setup, LC_Test_TearDown, "LC_CreateResultTables_Test_ARTRegisterError");
    UtTest_Add(LC_CreateResultTables_Test_ARTGetAddressError, LC_Test_Setup, LC_Test_TearDown, "LC_CreateResultTables_Test_ARTGetAddressError");

    UtTest_Add(LC_CreateDefinitionTables_Test_NominalCDSActive, LC_Test_Setup, LC_Test_TearDown, "LC_CreateDefinitionTables_Test_NominalCDSActive");
    UtTest_Add(LC_CreateDefinitionTables_Test_NominalCDSInactive, LC_Test_Setup, LC_Test_TearDown, "LC_CreateDefinitionTables_Test_NominalCDSInactive");
    UtTest_Add(LC_CreateDefinitionTables_Test_WDTReRegisterThenInfoRecovered, LC_Test_Setup, LC_Test_TearDown, "LC_CreateDefinitionTables_Test_WDTReRegisterThenInfoRecovered");
    UtTest_Add(LC_CreateDefinitionTables_Test_ADTReRegisterThenInfoRecovered, LC_Test_Setup, LC_Test_TearDown, "LC_CreateDefinitionTables_Test_ADTReRegisterThenInfoRecovered");
    UtTest_Add(LC_CreateDefinitionTables_Test_WDTReRegisterError, LC_Test_Setup, LC_Test_TearDown, "LC_CreateDefinitionTables_Test_WDTReRegisterError");
    UtTest_Add(LC_CreateDefinitionTables_Test_WDTCriticalADTNoncritical, LC_Test_Setup, LC_Test_TearDown, "LC_CreateDefinitionTables_Test_WDTCriticalADTNoncritical");
    UtTest_Add(LC_CreateDefinitionTables_Test_WDTRegisterError, LC_Test_Setup, LC_Test_TearDown, "LC_CreateDefinitionTables_Test_WDTRegisterError");
    UtTest_Add(LC_CreateDefinitionTables_Test_ADTRegisterError, LC_Test_Setup, LC_Test_TearDown, "LC_CreateDefinitionTables_Test_ADTRegisterError");

    UtTest_Add(LC_CreateTaskCDS_Test_NominalPowerOnReset, LC_Test_Setup, LC_Test_TearDown, "LC_CreateTaskCDS_Test_NominalPowerOnReset");
    UtTest_Add(LC_CreateTaskCDS_Test_NominalProcessorReset, LC_Test_Setup, LC_Test_TearDown, "LC_CreateTaskCDS_Test_NominalProcessorReset"); // FIXME: need #if statement around this?  Depends on a DCR.  Currently skips a line.
    UtTest_Add(LC_CreateTaskCDS_Test_WRTRegisterCDSError, LC_Test_Setup, LC_Test_TearDown, "LC_CreateTaskCDS_Test_WRTRegisterCDSError");
    UtTest_Add(LC_CreateTaskCDS_Test_ARTRegisterCDSError, LC_Test_Setup, LC_Test_TearDown, "LC_CreateTaskCDS_Test_ARTRegisterCDSError");
    UtTest_Add(LC_CreateTaskCDS_Test_AppDataRegisterCDSError, LC_Test_Setup, LC_Test_TearDown, "LC_CreateTaskCDS_Test_AppDataRegisterCDSError");

    UtTest_Add(LC_LoadDefaultTables_Test_NominalActiveCDS, LC_Test_Setup, LC_Test_TearDown, "LC_LoadDefaultTables_Test_NominalActiveCDS");
    UtTest_Add(LC_LoadDefaultTables_Test_ActiveCDSNotSuccess, LC_Test_Setup, LC_Test_TearDown, "LC_LoadDefaultTables_Test_ActiveCDSNotSuccess");
    UtTest_Add(LC_LoadDefaultTables_Test_NominalInactiveCDS, LC_Test_Setup, LC_Test_TearDown, "LC_LoadDefaultTables_Test_NominalInactiveCDS");
    UtTest_Add(LC_LoadDefaultTables_Test_LoadWDTError, LC_Test_Setup, LC_Test_TearDown, "LC_LoadDefaultTables_Test_LoadWDTError");
    UtTest_Add(LC_LoadDefaultTables_Test_GetWDTAddressError, LC_Test_Setup, LC_Test_TearDown, "LC_LoadDefaultTables_Test_GetWDTAddressError");
    UtTest_Add(LC_LoadDefaultTables_Test_LoadADTError, LC_Test_Setup, LC_Test_TearDown, "LC_LoadDefaultTables_Test_LoadADTError");
    UtTest_Add(LC_LoadDefaultTables_Test_GetADTAddressError, LC_Test_Setup, LC_Test_TearDown, "LC_LoadDefaultTables_Test_GetADTAddressError");

} /* end LC_App_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
