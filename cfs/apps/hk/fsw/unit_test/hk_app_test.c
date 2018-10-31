 /*************************************************************************
 ** File:
 **   $Id: hk_app_test.c 1.4 2016/10/28 11:04:15EDT mdeschu Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file hk_app.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: hk_app_test.c  $
 **   Revision 1.4 2016/10/28 11:04:15EDT mdeschu 
 **   Updates to unit tests for 2.4.1
 **   Revision 1.3 2016/10/18 17:28:58EDT czogby 
 **   Fix test HK_AppInit_Test_Nominal
 **   Revision 1.2 2016/10/07 14:33:14EDT czogby 
 **   Code Walkthrough Updates
 **   Revision 1.1 2016/06/24 14:43:43EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/hk/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "hk_app_test.h"
#include "hk_app.h"
#include "hk_msg.h"
#include "hk_msgdefs.h"
#include "hk_msgids.h"
#include "hk_events.h"
#include "hk_tbldefs.h"
#include "hk_version.h"
#include "hk_test_utils.h"
#include "cfe_tbl_internal.h"
#include "ut_osapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Function Definitions
 */

void HK_APP_TEST_CFE_SB_InitMsgHook
                   (void           *MsgPtr,
                    CFE_SB_MsgId_t MsgId,
                    uint16         Length,
                    boolean        Clear )
{
    /* Do nothing */
}

uint16    HK_APP_TEST_CFE_TBL_GetAddressHookRunCount;
int32 HK_APP_TEST_CFE_TBL_GetAddressHook (void **TblPtr, CFE_TBL_Handle_t TblHandle)
{
    HK_APP_TEST_CFE_TBL_GetAddressHookRunCount++;

    if (HK_APP_TEST_CFE_TBL_GetAddressHookRunCount == 1)
    {
        return CFE_TBL_INFO_UPDATED;
    }
    else
    {
        return -1;
    }
}

void HK_APP_TEST_CFE_ES_ExitAppHook(uint32 ExitStatus)
{
    HK_AppData.CombinedPacketsSent++;
}

void HK_AppMain_Test_Nominal(void)
{
    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);
    
    /* Set to prevent call to CFE_SB_RcvMsg from returning an error */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, HK_SEND_HK_MID, 1);

    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 8, 1);

    /* Set to make HK_AppInit return CFE_SUCCESS */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_AppInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);
    
    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

    /* Used to verify completion of HK_AppMain by incrementing HK_AppData.CombinedPacketsSent. */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_EXITAPP_INDEX, &HK_APP_TEST_CFE_ES_ExitAppHook);

    /* Execute the function being tested */
    HK_AppMain();
    
    /* Verify results */
    UtAssert_True (HK_AppData.CombinedPacketsSent == 1, "HK_AppData.CombinedPacketsSent == 1");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppMain_Test_Nominal */

void HK_AppMain_Test_AppInitError(void)
{
    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to fail condition "Status != CFE_SUCCESS" immediately after call to HK_AppInit */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);
    
    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);
    
    /* Set to prevent call to CFE_SB_RcvMsg from returning an error */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, HK_SEND_HK_MID, 1);

    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 8, 1);

    /* Set to make HK_AppInit return CFE_SUCCESS */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_AppInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);
    
    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

    /* Execute the function being tested */
    HK_AppMain();
    
    /* Verify results */
    UtAssert_True (HK_AppData.RunStatus == CFE_ES_APP_ERROR, "HK_AppData.RunStatus == CFE_ES_APP_ERROR");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppMain_Test_AppInitError */

void HK_AppMain_Test_RcvMsgError(void)
{
    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);
    
    /* Set to generate error message HK_RCV_MSG_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 1);

    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, HK_SEND_HK_MID, 1);

    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 8, 1);

    /* Set to make HK_AppInit return CFE_SUCCESS */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_AppInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);
    
    /* Set to prevent segmentation fault in HK_AppInit */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

    /* Execute the function being tested */
    HK_AppMain();
    
    /* Verify results */
    UtAssert_True (HK_AppData.RunStatus == CFE_ES_APP_ERROR, "HK_AppData.RunStatus == CFE_ES_APP_ERROR");

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_RCV_MSG_ERR_EID, CFE_EVS_ERROR, "HK_APP Exiting due to CFE_SB_RcvMsg error 0xFFFFFFFF"),
        "HK_APP Exiting due to CFE_SB_RcvMsg error 0xFFFFFFFF");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HK_APP Exiting due to CFE_SB_RcvMsg error 0xFFFFFFFF\n"),
        "HK_APP Exiting due to CFE_SB_RcvMsg error 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end HK_AppMain_Test_RcvMsgError */

void HK_AppInit_Test_Nominal(void)
{
    int32           Result;
    char            Message[125];
    HK_HkPacket_t   ExpectedPacket;

    CFE_PSP_MemSet(&HK_AppData, 1, sizeof(HK_AppData_t));
    CFE_PSP_MemSet(&ExpectedPacket, 1, sizeof(HK_HkPacket_t));

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;	

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

    /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_AppInit();
    
    /* Verify results */
    sprintf(Message, "HK Initialized.  Version %d.%d.%d.%d", HK_MAJOR_VERSION, HK_MINOR_VERSION, HK_REVISION, HK_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(HK_INIT_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (HK_AppData.RunStatus == CFE_ES_APP_RUN, "HK_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (memcmp(&HK_AppData.HkPacket, &ExpectedPacket, sizeof(HK_HkPacket_t)) == 0, "memcmp(&HK_AppData.HkPacket, &ExpectedPacket, sizeof(HK_HkPacket_t)) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppInit_Test_Nominal */

void HK_AppInit_Test_EVSRegisterError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate system log message "HK: error registering for event services" */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

   /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_AppInit();
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HK: error registering for event services: 0xFFFFFFFF\n"),
        "HK: error registering for event services: 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

    UtAssert_True (HK_AppData.RunStatus == CFE_ES_APP_RUN, "HK_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Result == -1, "Result == -1");

} /* end HK_AppInit_Test_EVSRegisterError */

void HK_AppInit_Test_SBCreatePipeError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_CR_PIPE_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

   /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);
    /* Execute the function being tested */
    Result = HK_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_CR_PIPE_ERR_EID, CFE_EVS_ERROR, "Error Creating SB Pipe,RC=0xFFFFFFFF"),
        "Error Creating SB Pipe,RC=0xFFFFFFFF");

    UtAssert_True (HK_AppData.RunStatus == CFE_ES_APP_RUN, "HK_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppInit_Test_SBCreatePipeError */

void HK_AppInit_Test_SBSubscribeHKSendError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_SUB_CMB_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

   /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_SUB_CMB_ERR_EID, CFE_EVS_ERROR, "Error Subscribing to HK Snd Cmb Pkt, MID=0x189C, RC=0xFFFFFFFF"),
        "Error Subscribing to HK Snd Cmb Pkt, MID=0x189C, RC=0xFFFFFFFF");

    UtAssert_True (HK_AppData.RunStatus == CFE_ES_APP_RUN, "HK_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppInit_Test_SBSubscribeHKSendError */

void HK_AppInit_Test_SBSubscribeHKRequestError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_SUB_REQ_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 2);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

   /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_SUB_REQ_ERR_EID, CFE_EVS_ERROR, "Error Subscribing to HK Request, MID=0x189B, RC=0xFFFFFFFF"),
        "Error Subscribing to HK Request, MID=0x189B, RC=0xFFFFFFFF");

    UtAssert_True (HK_AppData.RunStatus == CFE_ES_APP_RUN, "HK_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppInit_Test_SBSubscribeHKRequestError */

void HK_AppInit_Test_SBSubscribeHKGndCmdsError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_SUB_CMD_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 3);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

   /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_SUB_CMD_ERR_EID, CFE_EVS_ERROR, "Error Subscribing to HK Gnd Cmds, MID=0x189A, RC=0xFFFFFFFF"),
        "Error Subscribing to HK Gnd Cmds, MID=0x189A, RC=0xFFFFFFFF");

    UtAssert_True (HK_AppData.RunStatus == CFE_ES_APP_RUN, "HK_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppInit_Test_SBSubscribeHKGndCmdsError */

void HK_AppInit_Test_PoolCreateError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_CR_POOL_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_POOLCREATE_INDEX, -1, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

   /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_CR_POOL_ERR_EID, CFE_EVS_ERROR, "Error Creating Memory Pool,RC=0xFFFFFFFF"),
        "Error Creating Memory Pool,RC=0xFFFFFFFF");

    UtAssert_True (HK_AppData.RunStatus == CFE_ES_APP_RUN, "HK_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppInit_Test_PoolCreateError */

void HK_AppInit_Test_TableInitError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_CPTBL_REG_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

   /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_CPTBL_REG_ERR_EID, CFE_EVS_ERROR, "Error Registering Copy Table,RC=0xFFFFFFFF"),
        "Error Registering Copy Table,RC=0xFFFFFFFF");

    UtAssert_True (HK_AppData.RunStatus == CFE_ES_APP_RUN, "HK_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppInit_Test_TableInitError */

void HK_AppInit_Test_SendInitEventError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error system log entry "HK App:Error Sending Initialization Event" */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_SENDEVENT_INDEX, -1, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

   /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message in HK_TableInit */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_AppInit();
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("HK App:Error Sending Initialization Event,RC=0xFFFFFFFF\n"),
        "HK App:Error Sending Initialization Event,RC=0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

    UtAssert_True (HK_AppData.RunStatus == CFE_ES_APP_RUN, "HK_AppData.RunStatus == CFE_ES_APP_RUN");

    UtAssert_True (Result == -1, "Result == -1");

} /* end HK_AppInit_Test_SendInitEventError */

void HK_TableInit_Test_Nominal(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

    /* Set to *prevent* error message HK_CPTBL_LD_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_TableInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HK_TableInit_Test_Nominal */

void HK_TableInit_Test_RegisterCopyTableError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_CPTBL_REG_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

    /* Set to *prevent* error message HK_CPTBL_LD_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_TableInit();
    
    /* Verify results */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_CPTBL_REG_ERR_EID, CFE_EVS_ERROR, "Error Registering Copy Table,RC=0xFFFFFFFF"),
        "Error Registering Copy Table,RC=0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_TableInit_Test_RegisterCopyTableError */

void HK_TableInit_Test_RegisterRuntimeTableError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_RTTBL_REG_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 2);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

    /* Set to *prevent* error message HK_CPTBL_LD_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_TableInit();
    
    /* Verify results */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_RTTBL_REG_ERR_EID, CFE_EVS_ERROR, "Error Registering Runtime Table,RC=0xFFFFFFFF"),
        "Error Registering Runtime Table,RC=0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_TableInit_Test_RegisterRuntimeTableError */

void HK_TableInit_Test_LoadCopyTableError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

    /* Set to generate error message HK_CPTBL_LD_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, -1, 1);

    /* Set to prevent error message */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_TableInit();
    
    /* Verify results */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_CPTBL_LD_ERR_EID, CFE_EVS_ERROR, "Error Loading Copy Table,RC=0xFFFFFFFF"),
        "Error Loading Copy Table,RC=0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_TableInit_Test_LoadCopyTableError */

void HK_TableInit_Test_TableManageCopyTableError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_CPTBL_MNG_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_MANAGE_INDEX, -1, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

    /* Set to *prevent* error message HK_CPTBL_LD_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_TableInit();
    
    /* Verify results */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_CPTBL_MNG_ERR_EID, CFE_EVS_ERROR, "Error from TBL Manage call for Copy Table,RC=0xFFFFFFFF"),
        "Error from TBL Manage call for Copy Table,RC=0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_TableInit_Test_TableManageCopyTableError */

void HK_TableInit_Test_TableManageRuntimeTableError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_RTTBL_MNG_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_MANAGE_INDEX, -1, 2);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

    /* Set to *prevent* error message HK_CPTBL_LD_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent error message */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    Result = HK_TableInit();
    
    /* Verify results */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_RTTBL_MNG_ERR_EID, CFE_EVS_ERROR, "Error from TBL Manage call for Runtime Table,RC=0xFFFFFFFF"),
        "Error from TBL Manage call for Runtime Table,RC=0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_TableInit_Test_TableManageRuntimeTableError */

void HK_TableInit_Test_GetAddressCopyTableError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_INITMSG_INDEX, &HK_APP_TEST_CFE_SB_InitMsgHook);

    /* Set to *prevent* error message HK_CPTBL_LD_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Set to generate error message HK_CPTBL_GADR_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = HK_TableInit();
    
    /* Verify results */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_CPTBL_GADR_ERR_EID, CFE_EVS_ERROR, "Error Getting Adr for Cpy Tbl,RC=0xFFFFFFFF"),
        "Error Getting Adr for Cpy Tbl,RC=0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_TableInit_Test_GetAddressCopyTableError */

void HK_TableInit_Test_GetAddressRuntimeTableAddressError(void)
{
    int32 Result;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to *prevent* error message HK_CPTBL_LD_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Generates error message HK_RTTBL_GADR_ERR_EID while preventing generation of error message HK_CPTBL_GADR_ERR_EID */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &HK_APP_TEST_CFE_TBL_GetAddressHook);
    HK_APP_TEST_CFE_TBL_GetAddressHookRunCount = 0;

    /* Execute the function being tested */
    Result = HK_TableInit();
    
    /* Verify results */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_RTTBL_GADR_ERR_EID, CFE_EVS_ERROR, "Error Getting Adr for Runtime Table,RC=0xFFFFFFFF"),
        "Error Getting Adr for Runtime Table,RC=0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_TableInit_Test_GetAddressRuntimeTableAddressError */

void HK_AppPipe_Test_SendCombinedPktNominal(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to reach case HK_SEND_COMBINED_PKT_MID */
    CFE_SB_InitMsg (&CmdPacket, HK_SEND_COMBINED_PKT_MID, sizeof(HK_Send_Out_Msg_t), TRUE);

    /* Set to reach case HK_SEND_COMBINED_PKT_MID (and prevent segmentation fault) */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, HK_SEND_COMBINED_PKT_MID, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETMSGID_INDEX);

    /* Set to *prevent* error message HK_MSG_LEN_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(HK_Send_Out_Msg_t), 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    HK_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1"); /* Gets an error message not directly relevant to this test */

} /* end HK_AppPipe_Test_SendCombinedPktNominal */

void HK_AppPipe_Test_SendCombinedPktError(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to reach case HK_SEND_COMBINED_PKT_MID */
    CFE_SB_InitMsg (&CmdPacket, HK_SEND_COMBINED_PKT_MID, sizeof(HK_Send_Out_Msg_t), TRUE);

    /* Set to generate error message HK_MSG_LEN_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    HK_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_MSG_LEN_ERR_EID, CFE_EVS_ERROR, "Msg with Bad length Rcvd: ID = 0x189C, Exp Len = 10, Len = 1"),
        "Msg with Bad length Rcvd: ID = 0x189C, Exp Len = 10, Len = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppPipe_Test_SendCombinedPktError */

void HK_AppPipe_Test_SendHKNominal(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to reach case HK_SEND_HK_MID */
    CFE_SB_InitMsg (&CmdPacket, HK_SEND_HK_MID, CFE_SB_CMD_HDR_SIZE, TRUE);

    /* Set to fail condition "CFE_SB_GetTotalMsgLength(MessagePtr) != CFE_SB_CMD_HDR_SIZE" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, CFE_SB_CMD_HDR_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    HK_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HK_AppPipe_Test_SendHKNominal */

void HK_AppPipe_Test_SendHKError(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to reach case HK_SEND_HK_MID */
    CFE_SB_InitMsg (&CmdPacket, HK_SEND_HK_MID, CFE_SB_CMD_HDR_SIZE, TRUE);

    /* Set to generate error message HK_MSG_LEN_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    HK_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_MSG_LEN_ERR_EID, CFE_EVS_ERROR, "Msg with Bad length Rcvd: ID = 0x189B, Exp Len = 8, Len = 1"),
        "Msg with Bad length Rcvd: ID = 0x189B, Exp Len = 8, Len = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppPipe_Test_SendHKError */

void HK_AppPipe_Test_Noop(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to reach case HK_CMD_MID */
    CFE_SB_InitMsg (&CmdPacket, HK_CMD_MID, CFE_SB_CMD_HDR_SIZE, TRUE);

    /* Set to reach case HK_NOOP_CC */
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HK_NOOP_CC);

    /* Execute the function being tested */
    HK_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_NOOP_CMD_EID, CFE_EVS_INFORMATION, "HK No-op command, Version 2.4.1.0"),
        "HK No-op command, Version 2.4.1.0");
    /* Note: This message is generated in subfunction HK_NoopCmd.  It is tested here to verify that the subfunction is reached. */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppPipe_Test_Noop */

void HK_AppPipe_Test_Reset(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to reach case HK_CMD_MID */
    CFE_SB_InitMsg (&CmdPacket, HK_CMD_MID, CFE_SB_CMD_HDR_SIZE, TRUE);

    /* Set to reach case HK_RESET_CC */
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HK_RESET_CC);

    /* Execute the function being tested */
    HK_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_RESET_CNTRS_CMD_EID, CFE_EVS_DEBUG, "HK Reset Counters command received"),
        "HK Reset Counters command received");
    /* Note: This message is generated in subfunction HK_ResetCtrsCmd.  It is tested here to verify that the subfunction is reached. */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppPipe_Test_Reset */

void HK_AppPipe_Test_InvalidCommandCode(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to reach case HK_CMD_MID */
    CFE_SB_InitMsg (&CmdPacket, HK_CMD_MID, CFE_SB_CMD_HDR_SIZE, TRUE);

    /* Set to generate error message HK_CC_ERR_EID */
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 99);

    /* Execute the function being tested */
    HK_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_CC_ERR_EID, CFE_EVS_ERROR, "Cmd Msg with Invalid command code Rcvd -- ID = 0x189A, CC = 99"),
        "Cmd Msg with Invalid command code Rcvd -- ID = 0x189A, CC = 99");

    UtAssert_True (HK_AppData.ErrCounter == 1, "HK_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_AppPipe_Test_InvalidCommandCode */

void HK_AppPipe_Test_ProcessIncomingHkData(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to reach default case of "switch (MessageID)" */
    CFE_SB_InitMsg (&CmdPacket, 99, CFE_SB_CMD_HDR_SIZE, TRUE);

    /* Execute the function being tested */
    HK_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HK_AppPipe_Test_ProcessIncomingHkData */

void HK_HousekeepingCmd_Test(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    /* Set to ensure there are no segmentation faults in subfunctions that use CmdPacket */
    CFE_SB_InitMsg (&CmdPacket, HK_CMD_MID, CFE_SB_CMD_HDR_SIZE, TRUE);

    HK_AppData.CmdCounter = 1;
    HK_AppData.ErrCounter = 2;
    HK_AppData.MissingDataCtr = 3;
    HK_AppData.CombinedPacketsSent = 4;
    HK_AppData.MemPoolHandle = 5;

    /* Execute the function being tested */
    HK_HousekeepingCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (HK_AppData.HkPacket.CmdCounter == 1, "HK_AppData.HkPacket.CmdCounter == 1");
    UtAssert_True (HK_AppData.HkPacket.ErrCounter == 2, "HK_AppData.HkPacket.ErrCounter == 2");
    UtAssert_True (HK_AppData.HkPacket.MissingDataCtr == 3, "HK_AppData.HkPacket.MissingDataCtr == 3");
    UtAssert_True (HK_AppData.HkPacket.CombinedPacketsSent == 4, "HK_AppData.HkPacket.CombinedPacketsSent == 4");
    UtAssert_True (HK_AppData.HkPacket.MemPoolHandle == 5, "HK_AppData.HkPacket.MemPoolHandle == 5");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HK_HousekeepingCmd_Test */

void HK_NoopCmd_Test_Nominal(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to satisfy condition "HK_VerifyCmdLength(MessagePtr,CFE_SB_CMD_HDR_SIZE)==HK_BAD_MSG_LENGTH_RC" */
    CFE_SB_InitMsg (&CmdPacket, HK_CMD_MID, CFE_SB_CMD_HDR_SIZE, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HK_NOOP_CC);

    /* Execute the function being tested */
    HK_NoopCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_NOOP_CMD_EID, CFE_EVS_INFORMATION, "HK No-op command, Version 2.4.1.0"),
        "HK No-op command, Version 2.4.1.0");

    UtAssert_True (HK_AppData.CmdCounter == 1, "HK_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_NoopCmd_Test_Nominal */

void HK_NoopCmd_Test_BadMsgLength(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_NOOP_CMD_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 99, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    HK_NoopCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (HK_AppData.ErrCounter == 1, "HK_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1"); /* Gets an error message not directly relevant to this test */

} /* end HK_NoopCmd_Test_BadMsgLength */

void HK_ResetCtrsCmd_Test_Nominal(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to satisfy condition "HK_VerifyCmdLength(MessagePtr,CFE_SB_CMD_HDR_SIZE)==HK_BAD_MSG_LENGTH_RC" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, CFE_SB_CMD_HDR_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    HK_ResetCtrsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_RESET_CNTRS_CMD_EID, CFE_EVS_DEBUG, "HK Reset Counters command received"),
        "HK Reset Counters command received");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_ResetCtrsCmd_Test_Nominal */

void HK_ResetCtrsCmd_Test_BadMsgLength(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to generate error message HK_RESET_CNTRS_CMD_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 99, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    HK_ResetCtrsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (HK_AppData.ErrCounter == 1, "HK_AppData.ErrCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1"); /* Gets an error message not directly relevant to this test */

} /* end HK_ResetCtrsCmd_Test_BadMsgLength */

void HK_ResetHkData_Test(void)
{
    /* Execute the function being tested */
    HK_ResetHkData();
    
    /* Verify results */
    UtAssert_True (HK_AppData.CmdCounter == 0, "HK_AppData.HkPacket.CmdCounter == 0");
    UtAssert_True (HK_AppData.ErrCounter == 0, "HK_AppData.HkPacket.ErrCounter == 0");
    UtAssert_True (HK_AppData.CombinedPacketsSent == 0, "HK_AppData.HkPacket.CombinedPacketsSent == 0");
    UtAssert_True (HK_AppData.MissingDataCtr == 0, "HK_AppData.HkPacket.MissingDataCtr == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HK_ResetHkData_Test */

void HK_VerifyCmdLength_Test_Nominal(void)
{
    int32               Result;
    uint32              ExpectedLength;
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set to satisfy condition "ExpectedLength != ActualLength" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, CFE_SB_CMD_HDR_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    ExpectedLength = CFE_SB_CMD_HDR_SIZE;

    /* Execute the function being tested */
    Result = HK_VerifyCmdLength((CFE_SB_MsgPtr_t)(&CmdPacket), ExpectedLength);
    
    /* Verify results */
    UtAssert_True (Result == HK_SUCCESS, "Result == HK_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end HK_VerifyCmdLength_Test_Nominal */

void HK_VerifyCmdLength_Test_BadMsgLength(void)
{
    int32               Result;
    uint32              ExpectedLength;
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Set up to generate error message HK_CMD_LEN_ERR_EID */
    CFE_SB_InitMsg (&CmdPacket, HK_CMD_MID, CFE_SB_CMD_HDR_SIZE, TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, HK_RESET_CC);

    ExpectedLength = 99;

    /* Execute the function being tested */
    Result = HK_VerifyCmdLength((CFE_SB_MsgPtr_t)(&CmdPacket), ExpectedLength);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_CMD_LEN_ERR_EID, CFE_EVS_ERROR, "Cmd Msg with Bad length Rcvd: ID = 0x189A, CC = 1, Exp Len = 99, Len = 8"),
        "Cmd Msg with Bad length Rcvd: ID = 0x189A, CC = 1, Exp Len = 99, Len = 8");

    UtAssert_True (Result == HK_BAD_MSG_LENGTH_RC, "Result == HK_BAD_MSG_LENGTH_RC");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end HK_VerifyCmdLength_Test_BadMsgLength */

/* No test needed for HK_SendCombinedHKCmd - all lines of code are run by other tests, and there's nothing to verify. */

void HK_App_Test_AddTestCases(void)
{
    UtTest_Add(HK_AppMain_Test_Nominal, HK_Test_Setup, HK_Test_TearDown, "HK_AppMain_Test_Nominal");
    UtTest_Add(HK_AppMain_Test_AppInitError, HK_Test_Setup, HK_Test_TearDown, "HK_AppMain_Test_AppInitError");
    UtTest_Add(HK_AppMain_Test_RcvMsgError, HK_Test_Setup, HK_Test_TearDown, "HK_AppMain_Test_RcvMsgError");

    UtTest_Add(HK_AppInit_Test_Nominal, HK_Test_Setup, HK_Test_TearDown, "HK_AppInit_Test_Nominal");
    UtTest_Add(HK_AppInit_Test_EVSRegisterError, HK_Test_Setup, HK_Test_TearDown, "HK_AppInit_Test_EVSRegisterError");
    UtTest_Add(HK_AppInit_Test_SBCreatePipeError, HK_Test_Setup, HK_Test_TearDown, "HK_AppInit_Test_SBCreatePipeError");
    UtTest_Add(HK_AppInit_Test_SBSubscribeHKSendError, HK_Test_Setup, HK_Test_TearDown, "HK_AppInit_Test_SBSubscribeHKSendError");
    UtTest_Add(HK_AppInit_Test_SBSubscribeHKRequestError, HK_Test_Setup, HK_Test_TearDown, "HK_AppInit_Test_SBSubscribeHKRequestError");
    UtTest_Add(HK_AppInit_Test_SBSubscribeHKGndCmdsError, HK_Test_Setup, HK_Test_TearDown, "HK_AppInit_Test_SBSubscribeHKGndCmdsError");
    UtTest_Add(HK_AppInit_Test_PoolCreateError, HK_Test_Setup, HK_Test_TearDown, "HK_AppInit_Test_PoolCreateError");
    UtTest_Add(HK_AppInit_Test_TableInitError, HK_Test_Setup, HK_Test_TearDown, "HK_AppInit_Test_TableInitError");
    UtTest_Add(HK_AppInit_Test_SendInitEventError, HK_Test_Setup, HK_Test_TearDown, "HK_AppInit_Test_SendInitEventError");

    UtTest_Add(HK_TableInit_Test_Nominal, HK_Test_Setup, HK_Test_TearDown, "HK_TableInit_Test_Nominal");
    UtTest_Add(HK_TableInit_Test_RegisterCopyTableError, HK_Test_Setup, HK_Test_TearDown, "HK_TableInit_Test_RegisterCopyTableError");
    UtTest_Add(HK_TableInit_Test_RegisterRuntimeTableError, HK_Test_Setup, HK_Test_TearDown, "HK_TableInit_Test_RegisterRuntimeTableError");
    UtTest_Add(HK_TableInit_Test_LoadCopyTableError, HK_Test_Setup, HK_Test_TearDown, "HK_TableInit_Test_LoadCopyTableError");
    UtTest_Add(HK_TableInit_Test_TableManageCopyTableError, HK_Test_Setup, HK_Test_TearDown, "HK_TableInit_Test_TableManageCopyTableError");
    UtTest_Add(HK_TableInit_Test_TableManageRuntimeTableError, HK_Test_Setup, HK_Test_TearDown, "HK_TableInit_Test_TableManageRuntimeTableError");
    UtTest_Add(HK_TableInit_Test_GetAddressCopyTableError, HK_Test_Setup, HK_Test_TearDown, "HK_TableInit_Test_GetAddressCopyTableError");
    UtTest_Add(HK_TableInit_Test_GetAddressRuntimeTableAddressError, HK_Test_Setup, HK_Test_TearDown, "HK_TableInit_Test_GetAddressRuntimeTableAddressError");

    UtTest_Add(HK_AppPipe_Test_SendCombinedPktNominal, HK_Test_Setup, HK_Test_TearDown, "HK_AppPipe_Test_SendCombinedPktNominal");
    UtTest_Add(HK_AppPipe_Test_SendCombinedPktError, HK_Test_Setup, HK_Test_TearDown, "HK_AppPipe_Test_SendCombinedPktError");
    UtTest_Add(HK_AppPipe_Test_SendHKNominal, HK_Test_Setup, HK_Test_TearDown, "HK_AppPipe_Test_SendHKNominal");
    UtTest_Add(HK_AppPipe_Test_SendHKError, HK_Test_Setup, HK_Test_TearDown, "HK_AppPipe_Test_SendHKError");
    UtTest_Add(HK_AppPipe_Test_Noop, HK_Test_Setup, HK_Test_TearDown, "HK_AppPipe_Test_Noop");
    UtTest_Add(HK_AppPipe_Test_Reset, HK_Test_Setup, HK_Test_TearDown, "HK_AppPipe_Test_Reset");
    UtTest_Add(HK_AppPipe_Test_InvalidCommandCode, HK_Test_Setup, HK_Test_TearDown, "HK_AppPipe_Test_InvalidCommandCode");
    UtTest_Add(HK_AppPipe_Test_ProcessIncomingHkData, HK_Test_Setup, HK_Test_TearDown, "HK_AppPipe_Test_ProcessIncomingHkData");

    UtTest_Add(HK_HousekeepingCmd_Test, HK_Test_Setup, HK_Test_TearDown, "HK_HousekeepingCmd_Test");

    UtTest_Add(HK_NoopCmd_Test_Nominal, HK_Test_Setup, HK_Test_TearDown, "HK_NoopCmd_Test_Nominal");
    UtTest_Add(HK_NoopCmd_Test_BadMsgLength, HK_Test_Setup, HK_Test_TearDown, "HK_NoopCmd_Test_BadMsgLength");

    UtTest_Add(HK_ResetCtrsCmd_Test_Nominal, HK_Test_Setup, HK_Test_TearDown, "HK_ResetCtrsCmd_Test_Nominal");
    UtTest_Add(HK_ResetCtrsCmd_Test_BadMsgLength, HK_Test_Setup, HK_Test_TearDown, "HK_ResetCtrsCmd_Test_BadMsgLength");

    UtTest_Add(HK_ResetHkData_Test, HK_Test_Setup, HK_Test_TearDown, "HK_ResetHkData_Test");

    UtTest_Add(HK_VerifyCmdLength_Test_Nominal, HK_Test_Setup, HK_Test_TearDown, "HK_VerifyCmdLength_Test_Nominal");
    UtTest_Add(HK_VerifyCmdLength_Test_BadMsgLength, HK_Test_Setup, HK_Test_TearDown, "HK_VerifyCmdLength_Test_BadMsgLength");
} /* end HK_App_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
