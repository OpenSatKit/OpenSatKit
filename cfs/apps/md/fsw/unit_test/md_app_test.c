 /*************************************************************************
 ** File: md_app_test.c 
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
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file md_app.c
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

#include "md_app_test.h"
#include "md_app.h"
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

int32 MD_APP_TEST_CFE_TBL_GetAddressHook( void **TblPtr, CFE_TBL_Handle_t TblHandle )
{
    *TblPtr = &MD_AppData.MD_DwellTables[0];

    CFE_PSP_MemSet(&MD_AppData, 0, sizeof(MD_AppData_t));

    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_ENABLED;

    return CFE_TBL_INFO_UPDATED;
}

int32 MD_APP_TEST_CFE_TBL_GetAddressHook2( void **TblPtr, CFE_TBL_Handle_t TblHandle )
{
    *TblPtr = &MD_AppData.MD_DwellTables[0];

    CFE_PSP_MemSet(&MD_AppData, 0, sizeof(MD_AppData_t));

    MD_AppData.MD_DwellTables[0].Enabled = 99;

    return CFE_TBL_INFO_UPDATED;
}

void MD_AppMain_Test_RegisterAppError(void)
{
    /* Set to generate error message "MD_APP: Call to CFE_ES_RegisterApp Failed" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERAPP_INDEX, -1, 1);

    /* Execute the function being tested */
    MD_AppMain();
    
    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_APP_ERROR, "MD_AppData.RunStatus == CFE_ES_APP_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MD_APP: Call to CFE_ES_RegisterApp Failed:RC=0xFFFFFFFF\n"),
        "MD_APP: Call to CFE_ES_RegisterApp Failed:RC=0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end MD_AppMain_Test_RegisterAppError */

void MD_AppMain_Test_AppInitError(void)
{
    /* Set to make MD_AppInit return -1, in order to generate log message "Application Init Failed" */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    MD_AppMain();
    
    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_APP_ERROR, "MD_AppData.RunStatus == CFE_ES_APP_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MD:Application Init Failed,RC=0xFFFFFFFF\n"),
        "MD:Application Init Failed,RC=0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 2, "Ut_CFE_ES_GetSysLogQueueDepth() == 2");
    /* Generates 1 log message we don't care about in this test */

} /* end MD_AppMain_Test_AppInitError */

void MD_AppMain_Test_RcvMsgError(void)
{
    MD_NoArgsCmd_t   CmdPacket;

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to generate error message MD_PIPE_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 1);

    CFE_SB_InitMsg (&CmdPacket, MD_WAKEUP_MID, sizeof(MD_NoArgsCmd_t), TRUE);

    MD_AppData.MsgPtr = (CFE_SB_MsgPtr_t)(&CmdPacket);

    /* Execute the function being tested */
    MD_AppMain();
    
    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_APP_ERROR, "MD_AppData.RunStatus == CFE_ES_APP_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_PIPE_ERR_EID, CFE_EVS_ERROR, "SB Pipe Read Error, App will exit. Pipe Return Status = 0xFFFFFFFF"),
        "SB Pipe Read Error, App will exit. Pipe Return Status = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");
    /* Generates 2 event messages we don't care about in this test */

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 0, "Ut_CFE_ES_GetSysLogQueueDepth() == 0");

} /* end MD_AppMain_Test_RcvMsgError */

void MD_AppMain_Test_WakeupNominal(void)
{
    MD_NoArgsCmd_t   CmdPacket;

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to make while-loop run exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    CFE_SB_InitMsg (&CmdPacket, MD_WAKEUP_MID, sizeof(MD_NoArgsCmd_t), TRUE);

    MD_AppData.MsgPtr = (CFE_SB_MsgPtr_t)(&CmdPacket);

    /* Execute the function being tested */
    MD_AppMain();
    
    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_APP_RUN, "MD_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 2 event messages we don't care about in this test */

} /* end MD_AppMain_Test_WakeupNominal */

void MD_AppMain_Test_WakeupLengthError(void)
{
    MD_NoArgsCmd_t   CmdPacket;

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to make while-loop run exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    CFE_SB_InitMsg (&CmdPacket, MD_WAKEUP_MID, 1, TRUE);

    MD_AppData.MsgPtr = (CFE_SB_MsgPtr_t)(&CmdPacket);

    /* Execute the function being tested */
    MD_AppMain();
    
    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_APP_RUN, "MD_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_MSG_LEN_ERR_EID, CFE_EVS_ERROR, "Msg with Bad length Rcvd: ID = 0x1892, Exp Len = 8, Len = 1"),
        "Msg with Bad length Rcvd: ID = 0x1892, Exp Len = 8, Len = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");
    /* Generates 2 event messages we don't care about in this test */

} /* end MD_AppMain_Test_WakeupLengthError */

void MD_AppMain_Test_CmdNominal(void)
{
    MD_NoArgsCmd_t   CmdPacket;

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to make while-loop run exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_NoArgsCmd_t), TRUE);

    MD_AppData.MsgPtr = (CFE_SB_MsgPtr_t)(&CmdPacket);

    /* Execute the function being tested */
    MD_AppMain();
    
    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_APP_RUN, "MD_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");
    /* Generates 3 event messages we don't care about in this test */

} /* end MD_AppMain_Test_CmdNominal */

void MD_AppMain_Test_SendHkNominal(void)
{
    MD_NoArgsCmd_t   CmdPacket;

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to make while-loop run exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    CFE_SB_InitMsg (&CmdPacket, MD_SEND_HK_MID, sizeof(MD_NoArgsCmd_t), TRUE);

    MD_AppData.MsgPtr = (CFE_SB_MsgPtr_t)(&CmdPacket);

    /* Execute the function being tested */
    MD_AppMain();
    
    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_APP_RUN, "MD_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 2 event messages we don't care about in this test */

} /* end MD_AppMain_Test_SendHkNominal */

void MD_AppMain_Test_SendHkLengthError(void)
{
    MD_NoArgsCmd_t   CmdPacket;

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to make while-loop run exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    CFE_SB_InitMsg (&CmdPacket, MD_SEND_HK_MID, 1, TRUE);

    MD_AppData.MsgPtr = (CFE_SB_MsgPtr_t)(&CmdPacket);

    /* Execute the function being tested */
    MD_AppMain();
    
    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_APP_RUN, "MD_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_MSG_LEN_ERR_EID, CFE_EVS_ERROR, "Msg with Bad length Rcvd: ID = 0x1891, Exp Len = 8, Len = 1"),
        "Msg with Bad length Rcvd: ID = 0x1891, Exp Len = 8, Len = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");
    /* Generates 2 event messages we don't care about in this test */

} /* end MD_AppMain_Test_SendHkLengthError */

void MD_AppMain_Test_InvalidMessageID(void)
{
    MD_NoArgsCmd_t   CmdPacket;

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Set to make while-loop run exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    CFE_SB_InitMsg (&CmdPacket, -1, 1, TRUE);

    MD_AppData.MsgPtr = (CFE_SB_MsgPtr_t)(&CmdPacket);

    /* Execute the function being tested */
    MD_AppMain();
    
    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_APP_RUN, "MD_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_MID_ERR_EID, CFE_EVS_ERROR, "Msg with Invalid message ID Rcvd -- ID = 0xFFFF"),
        "Msg with Invalid message ID Rcvd -- ID = 0xFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");
    /* Generates 2 event messages we don't care about in this test */

} /* end MD_AppMain_Test_InvalidMessageID */

void MD_AppInit_Test_Nominal(void)
{
    int32   Result;

    /* Set to prevent unintended error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = MD_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_INIT_INF_EID, CFE_EVS_INFORMATION, "MD Initialized.  Version 2.3.1.0"),
        "MD Initialized.  Version 2.3.1.0");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

} /* end MD_AppInit_Test_Nominal */

void MD_AppInit_Test_EvsRegisterNotSuccess(void)
{
    int32   Result;

    /* Set to generate log message "Call to CFE_EVS_Register Failed" */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_AppInit();
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MD_APP:Call to CFE_EVS_Register Failed:RC=0xFFFFFFFF\n"),
        "MD_APP:Call to CFE_EVS_Register Failed:RC=0xFFFFFFFF\n");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end MD_AppInit_Test_EvsRegisterNotSuccess */

void MD_AppInit_Test_InitSoftwareBusServicesNotSuccess(void)
{
    int32   Result;

    /* Set to make MD_InitSoftwareBusServices return -1 */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");
    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");
    /* Generates 1 system log message we don't care about in this test */

} /* end MD_AppInit_Test_InitSoftwareBusServicesNotSuccess */

void MD_AppInit_Test_InitTableServicesNotSuccess(void)
{
    int32   Result;

    /* Execute the function being tested */
    Result = MD_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == 0xCC000013, "Result == 0xCC000013");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");
    /* Generates 1 system log message we don't care about in this test */

} /* end MD_AppInit_Test_InitTableServicesNotSuccess */

void MD_InitControlStructures_Test(void)
{
    /* Execute the function being tested */
    MD_InitControlStructures();
    
    /* Verify results */
    UtAssert_True (MD_AppData.MD_DwellTables[0].Enabled == 0, "MD_AppData.MD_DwellTables[0].Enabled == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].AddrCount == 0, "MD_AppData.MD_DwellTables[0].AddrCount == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].Rate == 0, "MD_AppData.MD_DwellTables[0].Rate == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].Countdown == 1, "MD_AppData.MD_DwellTables[0].Countdown == 1");
    UtAssert_True (MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].DataSize == 0, "MD_AppData.MD_DwellTables[0].DataSize == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].Filler == 0, "MD_AppData.MD_DwellTables[0].Filler == 0");

    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Enabled == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Enabled == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].AddrCount == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].AddrCount == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Rate == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Rate == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown == 1, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown == 1");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].PktOffset == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].PktOffset == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].DataSize == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].DataSize == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Filler == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Filler == 0");

    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Enabled == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Enabled == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].AddrCount == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].AddrCount == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Rate == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Rate == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown == 1, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown == 1");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].PktOffset == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].PktOffset == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].DataSize == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].DataSize == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Filler == 0, "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Filler == 0");

    UtAssert_True (MD_AppData.MD_DwellTables[0].Entry[0].Length == 0, "MD_AppData.MD_DwellTables[0].Entry[0].Length == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].Entry[0].Delay == 0, "MD_AppData.MD_DwellTables[0].Entry[0].Delay == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].Entry[0].ResolvedAddress == 0, "MD_AppData.MD_DwellTables[0].Entry[0].ResolvedAddress == 0");

    UtAssert_True (MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].Length == 0, "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].Length == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].Delay == 0, "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].Delay == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].ResolvedAddress == 0, "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].ResolvedAddress == 0");

    UtAssert_True (MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].Length == 0, "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].Length == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].Delay == 0, "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].Delay == 0");
    UtAssert_True (MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].ResolvedAddress == 0, "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].ResolvedAddress == 0");

#if MD_SIGNATURE_OPTION == 1   
    UtAssert_True (strncmp(MD_AppData.MD_DwellTables[0].Signature, "", MD_SIGNATURE_FIELD_LENGTH) == 0, "strncmp(MD_AppData.MD_DwellTables[0].Signature, "", MD_SIGNATURE_FIELD_LENGTH) == 0");
    UtAssert_True (strncmp(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Signature, "", MD_SIGNATURE_FIELD_LENGTH) == 0, "strncmp(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Signature, "", MD_SIGNATURE_FIELD_LENGTH) == 0");
    UtAssert_True (strncmp(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Signature, "", MD_SIGNATURE_FIELD_LENGTH) == 0, "strncmp(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Signature, "", MD_SIGNATURE_FIELD_LENGTH) == 0");
#endif

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_InitControlStructures_Test */

void MD_InitSoftwareBusServices_Test_Nominal(void)
{
    int32   Result;

    /* Execute the function being tested */
    Result = MD_InitSoftwareBusServices();
    
    /* Verify results */
#if MD_SIGNATURE_OPTION == 1   
       UtAssert_True (MD_AppData.MD_DwellPkt[0].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[0].Signature[0] == '\0'");
       UtAssert_True (MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == '\0'");
       UtAssert_True (MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0'");
#endif

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 0, "Ut_CFE_ES_GetSysLogQueueDepth() == 0");

} /* end MD_InitSoftwareBusServices_Test_Nominal */

void MD_InitSoftwareBusServices_Test_CreatePipeError(void)
{
    int32   Result;

    /* Set to generate error message "MD_APP: Error creating cmd pipe" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_InitSoftwareBusServices();
    
    /* Verify results */
#if MD_SIGNATURE_OPTION == 1   
       UtAssert_True (MD_AppData.MD_DwellPkt[0].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[0].Signature[0] == '\0'");
       UtAssert_True (MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == '\0'");
       UtAssert_True (MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0'");
#endif

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MD_APP: Error creating cmd pipe:RC=0xFFFFFFFF\n"),
        "MD_APP: Error creating cmd pipe:RC=0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end MD_InitSoftwareBusServices_Test_CreatePipeError */

void MD_InitSoftwareBusServices_Test_SubscribeHkError(void)
{
    int32   Result;

    /* Set to generate error message "MD_APP: Error creating cmd pipe" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_InitSoftwareBusServices();
    
    /* Verify results */
#if MD_SIGNATURE_OPTION == 1   
       UtAssert_True (MD_AppData.MD_DwellPkt[0].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[0].Signature[0] == '\0'");
       UtAssert_True (MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == '\0'");
       UtAssert_True (MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0'");
#endif

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MD_APP: Error subscribing to HK Request:RC=0xFFFFFFFF\n"),
        "MD_APP: Error subscribing to HK Request:RC=0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end MD_InitSoftwareBusServices_Test_SubscribeHkError */

void MD_InitSoftwareBusServices_Test_SubscribeCmdError(void)
{
    int32   Result;

    /* Set to generate error message "MD_APP: Error creating cmd pipe" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = MD_InitSoftwareBusServices();
    
    /* Verify results */
#if MD_SIGNATURE_OPTION == 1   
       UtAssert_True (MD_AppData.MD_DwellPkt[0].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[0].Signature[0] == '\0'");
       UtAssert_True (MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == '\0'");
       UtAssert_True (MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0'");
#endif

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MD_APP:Error subscribing to gnd cmds:RC=0xFFFFFFFF\n"),
        "MD_APP:Error subscribing to gnd cmds:RC=0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end MD_InitSoftwareBusServices_Test_SubscribeCmdError */

void MD_InitSoftwareBusServices_Test_SubscribeWakeupError(void)
{
    int32   Result;

    /* Set to generate error message "MD_APP: Error creating cmd pipe" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 3);

    /* Execute the function being tested */
    Result = MD_InitSoftwareBusServices();
    
    /* Verify results */
#if MD_SIGNATURE_OPTION == 1   
       UtAssert_True (MD_AppData.MD_DwellPkt[0].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[0].Signature[0] == '\0'");
       UtAssert_True (MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == '\0'");
       UtAssert_True (MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0'");
#endif

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MD_APP:Error subscribing to wakeup message:RC=0xFFFFFFFF\n"),
        "MD_APP:Error subscribing to wakeup message:RC=0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end MD_InitSoftwareBusServices_Test_SubscribeWakeupError */

void MD_InitTableServices_Test_GetAddressErrorAndLoadError(void)
{
    int32   Result;

    /* Set to satisfy condition "Status == CFE_TBL_INFO_RECOVERED_TBL" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, CFE_TBL_INFO_RECOVERED_TBL, 1);

    /* Set to fail condition "GetAddressResult != CFE_TBL_INFO_UPDATED" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set to generate error message "MD_APP: Error 0x%08X received loading tbl#%d" (and to terminate loop after first run) */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_InitTableServices();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_NO_TBL_COPY_ERR_EID, CFE_EVS_ERROR, "Didn't update MD tbl #1 due to unexpected CFE_TBL_GetAddress return: 0xFFFFFFFF"),
        "Didn't update MD tbl #1 due to unexpected CFE_TBL_GetAddress return: 0xFFFFFFFF");

    UtAssert_True (MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED, "MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True (MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED, "MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True (MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED, "MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MD_APP: Error 0xFFFFFFFF received loading tbl#1\n"),
        "MD_APP: Error 0xFFFFFFFF received loading tbl#1\n");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_TBL_INIT_INF_EID, CFE_EVS_INFORMATION, "Dwell Tables Recovered: 0, Dwell Tables Initialized: 0"),
        "Dwell Tables Recovered: 0, Dwell Tables Initialized: 0");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end MD_InitTableServices_Test_GetAddressErrorAndLoadError */

void MD_InitTableServices_Test_TblRecoveredValidThenTblInits(void)
{
    int32   Result;

    /* Set to satisfy condition "Status == CFE_TBL_INFO_RECOVERED_TBL" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, CFE_TBL_INFO_RECOVERED_TBL, 1);

    /* Set to fail condition "GetAddressResult != CFE_TBL_INFO_UPDATED" and to prevent a core dump by assigning MD_LoadTablePtr */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_APP_TEST_CFE_TBL_GetAddressHook);

    /* Set to reach line "TblInits++" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = MD_InitTableServices();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_RECOVERED_TBL_VALID_INF_EID, CFE_EVS_INFORMATION, "Recovered Dwell Table #1 is valid and has been copied to the MD App"),
        "Recovered Dwell Table #1 is valid and has been copied to the MD App");

    UtAssert_True (MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED, "MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True (MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED, "MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True (MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED, "MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_TBL_INIT_INF_EID, CFE_EVS_INFORMATION, "Dwell Tables Recovered: 1, Dwell Tables Initialized: 3"),
        "Dwell Tables Recovered: 1, Dwell Tables Initialized: 3");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");
    /* Generates 2 event message we don't care about in this test */

} /* end MD_InitTableServices_Test_TblRecoveredValidThenTblInits */

void MD_InitTableServices_Test_TblRecoveredNotValid(void)
{
    int32   Result;

    /* Set to satisfy condition "Status == CFE_TBL_INFO_RECOVERED_TBL" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, CFE_TBL_INFO_RECOVERED_TBL, 1);

    /* Set to fail condition "GetAddressResult != CFE_TBL_INFO_UPDATED", to 
       prevent a core dump by assigning MD_LoadTablePtr, and to make MD_TableValidateionFunc() return non-success */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_APP_TEST_CFE_TBL_GetAddressHook2);

    /* Set to reach line "TblInits++" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = MD_InitTableServices();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_RECOVERED_TBL_NOT_VALID_ERR_EID, CFE_EVS_ERROR, "MD App will reinitialize Dwell Table #1 because recovered table is not valid"),
        "MD App will reinitialize Dwell Table #1 because recovered table is not valid");

    UtAssert_True (MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED, "MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True (MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED, "MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True (MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED, "MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_TBL_INIT_INF_EID, CFE_EVS_INFORMATION, "Dwell Tables Recovered: 0, Dwell Tables Initialized: 4"),
        "Dwell Tables Recovered: 0, Dwell Tables Initialized: 4");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");
    /* Generates 1 event message we don't care about in this test */

} /* end MD_InitTableServices_Test_TblRecoveredNotValid */

void MD_InitTableServices_Test_TblTooLarge(void)
{
    int32   Result;

    /* Set to satisfy condition "Status == CFE_TBL_ERR_INVALID_SIZE" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, CFE_TBL_ERR_INVALID_SIZE, 1);

    /* Set to reach line "TblInits++" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = MD_InitTableServices();
    
    /* Verify results */
#if MD_SIGNATURE_OPTION == 1
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_DWELL_TBL_TOO_LARGE_CRIT_EID, CFE_EVS_CRITICAL, "Dwell Table(s) are too large to register: 1836 > 16384 bytes, 25 > 227 entries"),
        "Dwell Table(s) are too large to register: 1836 > 16384 bytes, 25 > 227 entries");
#else
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_DWELL_TBL_TOO_LARGE_CRIT_EID, CFE_EVS_CRITICAL, "Dwell Table(s) are too large to register: 1804 > 16384 bytes, 25 > 227 entries"),
        "Dwell Table(s) are too large to register: 1836 > 16384 bytes, 25 > 227 entries");
#endif

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_TBL_INIT_INF_EID, CFE_EVS_INFORMATION, "Dwell Tables Recovered: 0, Dwell Tables Initialized: 0"),
        "Dwell Tables Recovered: 0, Dwell Tables Initialized: 0");

    UtAssert_True (Result == CFE_TBL_ERR_INVALID_SIZE, "Result == CFE_TBL_ERR_INVALID_SIZE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

} /* end MD_InitTableServices_Test_TblTooLarge */

void MD_InitTableServices_Test_TblRegisterCriticalError(void)
{
    int32   Result;

    /* Set to satisfy condition "Status != CFE_SUCCESS" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Set to reach line "TblInits++" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_LOAD_INDEX);

    /* Execute the function being tested */
    Result = MD_InitTableServices();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_TBL_REGISTER_CRIT_EID, CFE_EVS_CRITICAL, "CFE_TBL_Register error 0xFFFFFFFF received for tbl#1"),
        "CFE_TBL_Register error 0xFFFFFFFF received for tbl#1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_TBL_INIT_INF_EID, CFE_EVS_INFORMATION, "Dwell Tables Recovered: 0, Dwell Tables Initialized: 0"),
        "Dwell Tables Recovered: 0, Dwell Tables Initialized: 0");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

} /* end MD_InitTableServices_Test_TblRegisterCriticalError */

void MD_ManageDwellTable_Test_ValidationPendingSucceedThenFail(void)
{
    int32   Result;
    uint8   TblIndex = 0;

    /* Set to satisfy condition "Status == CFE_TBL_INFO_VALIDATION_PENDING" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETSTATUS_INDEX, CFE_TBL_INFO_VALIDATION_PENDING, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETSTATUS_INDEX);

    /* Set to satisfy condition "Status != CFE_SUCCESS" on the 2nd loop */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_VALIDATE_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ManageDwellTable_Test_ValidationPendingSucceedThenFail */

void MD_ManageDwellTable_Test_UpdatePendingDwellStreamEnabled(void)
{
    int32   Result;
    uint8   TblIndex = 0;

    /* Set to satisfy condition "Status == CFE_TBL_INFO_UPDATE_PENDING" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETSTATUS_INDEX, CFE_TBL_INFO_UPDATE_PENDING, 1);

    /* Set to satisfy condition "MD_LoadTablePtr->Enabled == MD_DWELL_STREAM_ENABLED" and to prevent a core dump by assigning MD_LoadTablePtr */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &MD_APP_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ManageDwellTable_Test_UpdatePendingDwellStreamEnabled */

void MD_ManageDwellTable_Test_UpdatePendingTblCopyError(void)
{
    int32   Result;
    uint8   TblIndex = 0;

    /* Set to satisfy condition "Status == CFE_TBL_INFO_UPDATE_PENDING" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETSTATUS_INDEX, CFE_TBL_INFO_UPDATE_PENDING, 1);

    /* Set to generate error message MD_NO_TBL_COPY_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_NO_TBL_COPY_ERR_EID, CFE_EVS_ERROR, "Didn't update MD tbl #1 due to unexpected CFE_TBL_GetAddress return: 0xFFFFFFFF"),
        "Didn't update MD tbl #1 due to unexpected CFE_TBL_GetAddress return: 0xFFFFFFFF");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ManageDwellTable_Test_UpdatePendingTblCopyError */

void MD_ManageDwellTable_Test_TblStatusErr(void)
{
    int32   Result;
    uint8   TblIndex = 0;

    /* Set to satisfy condition "(Status & CFE_SEVERITY_BITMASK) == CFE_SEVERITY_ERROR" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETSTATUS_INDEX, CFE_SEVERITY_BITMASK, 1);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_TBL_STATUS_ERR_EID, CFE_EVS_ERROR, "Received unexpected error 0xC0000000 from CFE_TBL_GetStatus for tbl #1"),
        "Received unexpected error 0xC0000000 from CFE_TBL_GetStatus for tbl #1");

    UtAssert_True (Result == CFE_SEVERITY_BITMASK, "Result == CFE_SEVERITY_BITMASK");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ManageDwellTable_Test_TblStatusErr */

void MD_ManageDwellTable_Test_OtherStatus(void)
{
    int32   Result;
    uint8   TblIndex = 0;

    /* Set to reach final else-case */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETSTATUS_INDEX, 99, 1);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);
    
    /* Verify results */
    UtAssert_True (Result == 99, "Result == 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_ManageDwellTable_Test_OtherStatus */

void MD_ExecRequest_Test_SearchCmdHndlrTblError(void)
{
    MD_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, 99, sizeof(MD_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    MD_ExecRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_CC_NOT_IN_TBL_ERR_EID,  CFE_EVS_ERROR, "Command Code 0 not found in MD_CmdHandlerTbl structure"),
        "Command Code 0 not found in MD_CmdHandlerTbl structure");

    UtAssert_True (MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ExecRequest_Test_SearchCmdHndlrTblError */

void MD_ExecRequest_Test_CmdLengthError(void)
{
    MD_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, 1, TRUE);

    /* Set to generate error message MD_CMD_LEN_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, 0, 1);

    /* Execute the function being tested */
    MD_ExecRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "Cmd Msg with Bad length Rcvd: ID = 0x1890, CC = 0, Exp Len = 8, Len = 1"),
        "Cmd Msg with Bad length Rcvd: ID = 0x1890, CC = 0, Exp Len = 8, Len = 1");

    UtAssert_True (MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ExecRequest_Test_CmdLengthError */

void MD_ExecRequest_Test_Noop(void)
{
    MD_NoArgsCmd_t   CmdPacket;
    char             Message[150];

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_NoArgsCmd_t), TRUE);

    /* Set to reach case MD_NOOP_CC */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, MD_NOOP_CC, 1);

    /* Execute the function being tested */
    MD_ExecRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    sprintf(Message, "No-op command, Version %d.%d.%d.%d", MD_MAJOR_VERSION, MD_MINOR_VERSION, MD_REVISION, MD_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(MD_NOOP_INF_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ExecRequest_Test_Noop */

void MD_ExecRequest_Test_ResetCounters(void)
{
    MD_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_NoArgsCmd_t), TRUE);

    /* Set to reach case MD_RESET_CNTRS_CC */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, MD_RESET_CNTRS_CC, 1);

    /* Execute the function being tested */
    MD_ExecRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MD_RESET_CNTRS_DBG_EID,  CFE_EVS_DEBUG, "Reset Counters Cmd Received"),
        "Reset Counters Cmd Received");

    UtAssert_True (MD_AppData.CmdCounter == 0, "MD_AppData.CmdCounter == 0");
    UtAssert_True (MD_AppData.ErrCounter == 0, "MD_AppData.ErrCounter == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MD_ExecRequest_Test_ResetCounters */

void MD_ExecRequest_Test_StartDwell(void)
{
    MD_CmdStartStop_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdStartStop_t), TRUE);

    /* Set to reach case MD_START_DWELL_CC */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, MD_START_DWELL_CC, 1);

    /* Execute the function being tested */
    MD_ExecRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end MD_ExecRequest_Test_StartDwell */

void MD_ExecRequest_Test_StopDwell(void)
{
    MD_CmdStartStop_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdStartStop_t), TRUE);

    /* Set to reach case MD_STOP_DWELL_CC */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, MD_STOP_DWELL_CC, 1);

    /* Execute the function being tested */
    MD_ExecRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end MD_ExecRequest_Test_StopDwell */

void MD_ExecRequest_Test_JamDwell(void)
{
    MD_CmdJam_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdJam_t), TRUE);

    /* Set to reach case MD_JAM_DWELL_CC */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, MD_JAM_DWELL_CC, 1);

    /* Execute the function being tested */
    MD_ExecRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end MD_ExecRequest_Test_JamDwell */

#if MD_SIGNATURE_OPTION == 1
void MD_ExecRequest_Test_SetSignature(void)
{
    MD_CmdSetSignature_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdSetSignature_t), TRUE);

    /* Set to reach case MD_SET_SIGNATURE_CC */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, MD_SET_SIGNATURE_CC, 1);

    /* Execute the function being tested */
    MD_ExecRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end MD_ExecRequest_Test_SetSignature */
#endif

void MD_HkStatus_Test(void)
{
    MD_AppData.CmdCounter = 1;
    MD_AppData.ErrCounter = 2;

    MD_AppData.MD_DwellTables[0].Enabled = 1;

    MD_AppData.MD_DwellTables[0].AddrCount = 1;
    MD_AppData.MD_DwellTables[0].Rate = 2;
    MD_AppData.MD_DwellTables[0].PktOffset = 3;
    MD_AppData.MD_DwellTables[0].DataSize = 4;
    MD_AppData.MD_DwellTables[0].CurrentEntry = 5;
    MD_AppData.MD_DwellTables[0].Countdown = 6;

    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].AddrCount = 7;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Rate = 8;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].PktOffset = 9;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].DataSize = 10;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry = 11;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown = 12;

    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].AddrCount = 13;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Rate = 14;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].PktOffset = 15;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].DataSize = 16;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry = 17;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown = 18;

    /* Execute the function being tested */
    MD_HkStatus();
    
    /* Verify results */
    UtAssert_True (MD_AppData.HkPkt.ValidCmdCntr == 1, "MD_AppData.HkPkt.ValidCmdCntr == 1");
    UtAssert_True (MD_AppData.HkPkt.InvalidCmdCntr == 2, "MD_AppData.HkPkt.InvalidCmdCntr == 2");

    UtAssert_True (MD_AppData.HkPkt.DwellTblAddrCount[0] == 1, "MD_AppData.HkPkt.DwellTblAddrCount[0] == 1");
    UtAssert_True (MD_AppData.HkPkt.NumWaitsPerPkt[0]    == 2, "MD_AppData.HkPkt.NumWaitsPerPkt[0]    == 2");
    UtAssert_True (MD_AppData.HkPkt.DwellPktOffset[0]    == 3, "MD_AppData.HkPkt.DwellPktOffset[0]    == 3");
    UtAssert_True (MD_AppData.HkPkt.ByteCount[0]         == 4, "MD_AppData.HkPkt.ByteCount[0]         == 4");
    UtAssert_True (MD_AppData.HkPkt.DwellTblEntry[0]     == 5, "MD_AppData.HkPkt.DwellTblEntry[0]     == 5");
    UtAssert_True (MD_AppData.HkPkt.Countdown[0]         == 6, "MD_AppData.HkPkt.Countdown[0]         == 6");

    UtAssert_True (MD_AppData.HkPkt.DwellTblAddrCount[MD_NUM_DWELL_TABLES / 2] == 7, "MD_AppData.HkPkt.DwellTblAddrCount[MD_NUM_DWELL_TABLES / 2] == 7");
    UtAssert_True (MD_AppData.HkPkt.NumWaitsPerPkt[MD_NUM_DWELL_TABLES / 2]    == 8, "MD_AppData.HkPkt.NumWaitsPerPkt[MD_NUM_DWELL_TABLES / 2]    == 8");
    UtAssert_True (MD_AppData.HkPkt.DwellPktOffset[MD_NUM_DWELL_TABLES / 2]    == 9, "MD_AppData.HkPkt.DwellPktOffset[MD_NUM_DWELL_TABLES / 2]    == 9");
    UtAssert_True (MD_AppData.HkPkt.ByteCount[MD_NUM_DWELL_TABLES / 2]         == 10, "MD_AppData.HkPkt.ByteCount[MD_NUM_DWELL_TABLES / 2]         == 10");
    UtAssert_True (MD_AppData.HkPkt.DwellTblEntry[MD_NUM_DWELL_TABLES / 2]     == 11, "MD_AppData.HkPkt.DwellTblEntry[MD_NUM_DWELL_TABLES / 2]     == 11");
    UtAssert_True (MD_AppData.HkPkt.Countdown[MD_NUM_DWELL_TABLES / 2]         == 12, "MD_AppData.HkPkt.Countdown[MD_NUM_DWELL_TABLES / 2]         == 12");

    UtAssert_True (MD_AppData.HkPkt.DwellTblAddrCount[MD_NUM_DWELL_TABLES - 1] == 13, "MD_AppData.HkPkt.DwellTblAddrCount[MD_NUM_DWELL_TABLES - 1] == 13");
    UtAssert_True (MD_AppData.HkPkt.NumWaitsPerPkt[MD_NUM_DWELL_TABLES - 1]    == 14, "MD_AppData.HkPkt.NumWaitsPerPkt[MD_NUM_DWELL_TABLES - 1]    == 14");
    UtAssert_True (MD_AppData.HkPkt.DwellPktOffset[MD_NUM_DWELL_TABLES - 1]    == 15, "MD_AppData.HkPkt.DwellPktOffset[MD_NUM_DWELL_TABLES - 1]    == 15");
    UtAssert_True (MD_AppData.HkPkt.ByteCount[MD_NUM_DWELL_TABLES - 1]         == 16, "MD_AppData.HkPkt.ByteCount[MD_NUM_DWELL_TABLES - 1]         == 16");
    UtAssert_True (MD_AppData.HkPkt.DwellTblEntry[MD_NUM_DWELL_TABLES - 1]     == 17, "MD_AppData.HkPkt.DwellTblEntry[MD_NUM_DWELL_TABLES - 1]     == 17");
    UtAssert_True (MD_AppData.HkPkt.Countdown[MD_NUM_DWELL_TABLES - 1]         == 18, "MD_AppData.HkPkt.Countdown[MD_NUM_DWELL_TABLES - 1]         == 18");

    UtAssert_True (MD_AppData.HkPkt.DwellEnabledMask == 1, "MD_AppData.HkPkt.DwellEnabledMask == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_HkStatus_Test */

void MD_SearchCmdHndlrTbl_Test_FoundMatch1(void)
{
    int16            Result;
    CFE_SB_MsgId_t   MessageID   = MD_CMD_MID;
    uint16           CommandCode = MD_RESET_CNTRS_CC;

    /* Execute the function being tested */
    Result = MD_SearchCmdHndlrTbl(MessageID, CommandCode);
    
    /* Verify results */
    UtAssert_True (Result == 1, "Result == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_SearchCmdHndlrTbl_Test_FoundMatch1 */

void MD_SearchCmdHndlrTbl_Test_FoundMatch2(void)
{
    int16            Result;
    CFE_SB_MsgId_t   MessageID   = 0;
    uint16           CommandCode = 0;

    /* Execute the function being tested */
    Result = MD_SearchCmdHndlrTbl(MessageID, CommandCode);
    
    /* Verify results */
#if MD_SIGNATURE_OPTION == 1   
    UtAssert_True (Result == 7, "Result == 7");
#else
    UtAssert_True (Result == 6, "Result == 6");
#endif

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_SearchCmdHndlrTbl_Test_FoundMatch2 */

void MD_SearchCmdHndlrTbl_Test_BadCmdCode(void)
{
    int16            Result;
    CFE_SB_MsgId_t   MessageID   = MD_CMD_MID;
    uint16           CommandCode = 99;

    /* Execute the function being tested */
    Result = MD_SearchCmdHndlrTbl(MessageID, CommandCode);
    
    /* Verify results */
    UtAssert_True (Result == MD_BAD_CMD_CODE, "Result == MD_BAD_CMD_CODE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_SearchCmdHndlrTbl_Test_BadCmdCode */

void MD_SearchCmdHndlrTbl_Test_BasMsgID(void)
{
    int16            Result;
    CFE_SB_MsgId_t   MessageID   = 99;
    uint16           CommandCode = MD_RESET_CNTRS_CC;

    /* Execute the function being tested */
    Result = MD_SearchCmdHndlrTbl(MessageID, CommandCode);
    
    /* Verify results */
    UtAssert_True (Result == MD_BAD_MSG_ID, "Result == MD_BAD_MSG_ID");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MD_SearchCmdHndlrTbl_Test_BasMsgID */

void MD_App_Test_AddTestCases(void)
{
    UtTest_Add(MD_AppMain_Test_RegisterAppError, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_RegisterAppError");
    UtTest_Add(MD_AppMain_Test_AppInitError, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_AppInitError");
    UtTest_Add(MD_AppMain_Test_RcvMsgError, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_RcvMsgError");
    UtTest_Add(MD_AppMain_Test_WakeupNominal, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_WakeupNominal");
    UtTest_Add(MD_AppMain_Test_WakeupLengthError, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_WakeupLengthError");
    UtTest_Add(MD_AppMain_Test_CmdNominal, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_CmdNominal");
    UtTest_Add(MD_AppMain_Test_SendHkNominal, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_SendHkNominal");
    UtTest_Add(MD_AppMain_Test_SendHkLengthError, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_SendHkLengthError");
    UtTest_Add(MD_AppMain_Test_InvalidMessageID, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_InvalidMessageID");

    UtTest_Add(MD_AppInit_Test_Nominal, MD_Test_Setup, MD_Test_TearDown, "MD_AppInit_Test_Nominal");
    UtTest_Add(MD_AppInit_Test_EvsRegisterNotSuccess, MD_Test_Setup, MD_Test_TearDown, "MD_AppInit_Test_EvsRegisterNotSuccess");
    UtTest_Add(MD_AppInit_Test_InitSoftwareBusServicesNotSuccess, MD_Test_Setup, MD_Test_TearDown, "MD_AppInit_Test_InitSoftwareBusServicesNotSuccess");
    UtTest_Add(MD_AppInit_Test_InitTableServicesNotSuccess, MD_Test_Setup, MD_Test_TearDown, "MD_AppInit_Test_InitTableServicesNotSuccess");

    UtTest_Add(MD_InitControlStructures_Test, MD_Test_Setup, MD_Test_TearDown, "MD_InitControlStructures_Test");

    UtTest_Add(MD_InitSoftwareBusServices_Test_Nominal, MD_Test_Setup, MD_Test_TearDown, "MD_InitSoftwareBusServices_Test_Nominal");
    UtTest_Add(MD_InitSoftwareBusServices_Test_CreatePipeError, MD_Test_Setup, MD_Test_TearDown, "MD_InitSoftwareBusServices_Test_CreatePipeError");
    UtTest_Add(MD_InitSoftwareBusServices_Test_SubscribeHkError, MD_Test_Setup, MD_Test_TearDown, "MD_InitSoftwareBusServices_Test_SubscribeHkError");
    UtTest_Add(MD_InitSoftwareBusServices_Test_SubscribeCmdError, MD_Test_Setup, MD_Test_TearDown, "MD_InitSoftwareBusServices_Test_SubscribeCmdError");
    UtTest_Add(MD_InitSoftwareBusServices_Test_SubscribeWakeupError, MD_Test_Setup, MD_Test_TearDown, "MD_InitSoftwareBusServices_Test_SubscribeWakeupError");

    UtTest_Add(MD_InitTableServices_Test_GetAddressErrorAndLoadError, MD_Test_Setup, MD_Test_TearDown, "MD_InitTableServices_Test_GetAddressErrorAndLoadError");
    UtTest_Add(MD_InitTableServices_Test_TblRecoveredValidThenTblInits, MD_Test_Setup, MD_Test_TearDown, "MD_InitTableServices_Test_TblRecoveredValidThenTblInits");
    UtTest_Add(MD_InitTableServices_Test_TblRecoveredNotValid, MD_Test_Setup, MD_Test_TearDown, "MD_InitTableServices_Test_TblRecoveredNotValid");
    UtTest_Add(MD_InitTableServices_Test_TblTooLarge, MD_Test_Setup, MD_Test_TearDown, "MD_InitTableServices_Test_TblTooLarge");
    UtTest_Add(MD_InitTableServices_Test_TblRegisterCriticalError, MD_Test_Setup, MD_Test_TearDown, "MD_InitTableServices_Test_TblRegisterCriticalError");

    UtTest_Add(MD_ManageDwellTable_Test_ValidationPendingSucceedThenFail, MD_Test_Setup, MD_Test_TearDown, "MD_ManageDwellTable_Test_ValidationPendingSucceedThenFail");
    UtTest_Add(MD_ManageDwellTable_Test_UpdatePendingDwellStreamEnabled, MD_Test_Setup, MD_Test_TearDown, "MD_ManageDwellTable_Test_UpdatePendingDwellStreamEnabled");
    UtTest_Add(MD_ManageDwellTable_Test_UpdatePendingTblCopyError, MD_Test_Setup, MD_Test_TearDown, "MD_ManageDwellTable_Test_UpdatePendingTblCopyError");
    UtTest_Add(MD_ManageDwellTable_Test_TblStatusErr, MD_Test_Setup, MD_Test_TearDown, "MD_ManageDwellTable_Test_TblStatusErr");
    UtTest_Add(MD_ManageDwellTable_Test_OtherStatus, MD_Test_Setup, MD_Test_TearDown, "MD_ManageDwellTable_Test_OtherStatus");

    UtTest_Add(MD_ExecRequest_Test_SearchCmdHndlrTblError, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_SearchCmdHndlrTblError");
    UtTest_Add(MD_ExecRequest_Test_CmdLengthError, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_CmdLengthError");
    UtTest_Add(MD_ExecRequest_Test_Noop, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_Noop");
    UtTest_Add(MD_ExecRequest_Test_ResetCounters, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_ResetCounters");
    UtTest_Add(MD_ExecRequest_Test_StartDwell, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_StartDwell");
    UtTest_Add(MD_ExecRequest_Test_StopDwell, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_StopDwell");
    UtTest_Add(MD_ExecRequest_Test_JamDwell, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_JamDwell");
#if MD_SIGNATURE_OPTION == 1
    UtTest_Add(MD_ExecRequest_Test_SetSignature, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_SetSignature");
#endif
    /* Note: Cannot test default case in ExecRequest, because that would require the pre-defined constant structure MD_CmdHandlerTbl to have an entry
       with a command code other than the handled cases.  It does not, except for the last entry, which does not work because it's of type MD_TERM_MSGTYPE */

    UtTest_Add(MD_HkStatus_Test, MD_Test_Setup, MD_Test_TearDown, "MD_HkStatus_Test");

    UtTest_Add(MD_SearchCmdHndlrTbl_Test_FoundMatch1, MD_Test_Setup, MD_Test_TearDown, "MD_SearchCmdHndlrTbl_Test_FoundMatch1");
    /* Note: Cannot test 2nd case of "FoundMatch = TRUE" in MD_SearchCmdHndlrTbl, because that would require an entry in MD_CmdHandlerTbl other than 
       MD_CMD_MSGTYPE and MD_TERM_MSGTYPE, which it does not have. */

    UtTest_Add(MD_SearchCmdHndlrTbl_Test_BadCmdCode, MD_Test_Setup, MD_Test_TearDown, "MD_SearchCmdHndlrTbl_Test_BadCmdCode");
    UtTest_Add(MD_SearchCmdHndlrTbl_Test_BasMsgID, MD_Test_Setup, MD_Test_TearDown, "MD_SearchCmdHndlrTbl_Test_BasMsgID");

} /* end MD_App_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
