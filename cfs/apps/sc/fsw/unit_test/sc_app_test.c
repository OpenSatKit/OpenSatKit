 /*************************************************************************
 ** File:
 **   $Id: sc_app_test.c 1.3 2016/09/09 19:25:55EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the for the functions contained in the file sc_app.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_app_test.c  $
 **   Revision 1.3 2016/09/09 19:25:55EDT czogby 
 **   Update based on code inspection log
 **   Revision 1.2 2016/02/04 16:03:12EST czogby 
 **   Add End-of-file and end-of-function comments
 **   Revision 1.1 2015/08/18 14:02:56EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/sc/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "cfe_tbl_internal.h"
#include "sc_rts.h"
#include "sc_app.h"
#include "sc_cmds.h"
#include "sc_state.h"
#include "sc_atsrq.h"
#include "sc_rtsrq.h"
#include "sc_utils.h"
#include "sc_loads.h"
#include "sc_msgids.h"
#include "sc_events.h"
#include "sc_app_test.h"
#include "sc_test_utils.h"
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

int32 CFE_TBL_GetAddressHookNominal( void **TblPtr, CFE_TBL_Handle_t TblHandle )
{
    return CFE_SUCCESS;
} /* end CFE_TBL_GetAddressHookNominal */

int32 CFE_TBL_GetAddressHookNominal2( void )
{
    return CFE_TBL_ERR_NEVER_LOADED;
} /* end CFE_TBL_GetAddressHookNominal2 */

void SC_AppMain_Test_Nominal(void)
{
    /* Same return value as default, but bypasses default hook function to simplify test */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Called in a subfunction.  Set here to prevent segmentation fault. */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, SC_SEND_HK_MID, 1);

    /* Set to make loop execute exactly once */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Called in a subfunction.  Set here to prevent segmentation fault. */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_NoArgsCmd_t), 1);

    /* Prevents error messages in call to SC_GetLoadTablePointers */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 8);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    CFE_SB_InitMsg (&SC_OperData.MsgPtr, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&SC_OperData.MsgPtr, SC_NOOP_CC);

    /* Execute the function being tested */
    SC_AppMain();
    
    /* Verify results */
    /* Generates 2 event messages we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_AppMain_Test_Nominal */

void SC_AppMain_Test_RegisterAppError(void)
{
     /* Set CFE_ES_RegisterApp to return -1 in order to reach "Result = SC_AppInit()" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERAPP_INDEX, -1, 1);

    /* Execute the function being tested */
    SC_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_APP_EXIT_ERR_EID, CFE_EVS_ERROR, "App terminating, Result = 0xFFFFFFFF"),
        "App terminating, Result = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("SC App terminating, Result = 0xFFFFFFFF\n"),
        "SC App terminating, Result = 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end SC_AppMain_Test_RegisterAppError */

void SC_AppMain_Test_AppInitError(void)
{
     /* Set CFE_EVS_Register to return -1 in order to make SC_AppInit return -1, in order to reach "RunStatus = CFE_ES_APP_ERROR" */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    SC_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_APP_EXIT_ERR_EID, CFE_EVS_ERROR, "App terminating, Result = 0xFFFFFFFF"),
        "App terminating, Result = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("SC App terminating, Result = 0xFFFFFFFF\n"),
        "SC App terminating, Result = 0xFFFFFFFF\n");

    /* Generates 1 system log message we don't care about in this test */
    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 2, "Ut_CFE_ES_GetSysLogQueueDepth() == 2");

} /* end SC_AppMain_Test_AppInitError */

void SC_AppMain_Test_RcvMsgError(void)
{
    /* Set CFE_SB_RcvMsg to return -1 in order to reach "RunStatus = CFE_ES_APP_ERROR" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 1);

    /* Execute the function being tested */
    SC_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_APP_EXIT_ERR_EID, CFE_EVS_ERROR, "App terminating, Result = 0xFFFFFFFF"),
        "App terminating, Result = 0xFFFFFFFF");

    /* Generates 3 event messages we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("SC App terminating, Result = 0xFFFFFFFF\n"),
        "SC App terminating, Result = 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end SC_AppMain_Test_RcvMsgError */

void SC_AppInit_Test_NominalPowerOnReset(void)
{
    uint32         ReturnValue;
    SC_OperData_t  Expected_SC_OperData;
    SC_AppData_t   Expected_SC_AppData;
    char           Message[CFE_EVS_MAX_MESSAGE_LENGTH];

    /* Set CFE_ES_GetResetType to return CFE_ES_POWERON_RESET in order to reach "SC_AppData.AutoStartRTS = RTS_ID_AUTO_POWER_ON" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETRESETTYPE_INDEX, CFE_ES_POWERON_RESET, 1);

    /* Prevents error messages in call to SC_GetLoadTablePointers */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 8);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Set global data structures to non-zero values, to verify that SC_AppInit sets values to 0 (with some exceptions)  */
    CFE_PSP_MemSet(&SC_OperData, 1, sizeof(SC_OperData_t));
    CFE_PSP_MemSet(&SC_AppData,  1, sizeof(SC_AppData_t));

    CFE_PSP_MemSet(&Expected_SC_OperData, 0, sizeof(SC_OperData_t));
    CFE_PSP_MemSet(&Expected_SC_AppData,  0, sizeof(SC_AppData_t));

    CFE_SB_InitMsg(&Expected_SC_OperData.HkPacket, SC_HK_TLM_MID, sizeof(SC_HkTlm_t), FALSE);

    Expected_SC_AppData.ContinueAtsOnFailureFlag  = SC_CONT_ON_FAILURE_START;
    Expected_SC_AppData.NextProcNumber            = SC_NONE;
    Expected_SC_AppData.NextCmdTime[SC_ATP]       = SC_MAX_TIME;
    Expected_SC_AppData.NextCmdTime[SC_RTP]       = SC_MAX_TIME;
    Expected_SC_AppData.AutoStartRTS              = RTS_ID_AUTO_POWER_ON;

    /* Execute the function being tested */
    ReturnValue = SC_AppInit();
    
    /* Verify results */
    UtAssert_True (ReturnValue == CFE_SUCCESS, "ReturnValue == CFE_SUCCESS");
    UtAssert_MemCmp(&SC_AppData, &Expected_SC_AppData, sizeof(SC_AppData_t), "SC_AppData == Expected_SC_AppData");

    Expected_SC_OperData.AtsInfoHandle = 2;
    Expected_SC_OperData.AppendInfoHandle = 3;
    Expected_SC_OperData.RtsCtrlBlckHandle = 1;
    Expected_SC_OperData.AtsCtrlBlckHandle = 4;
    Expected_SC_OperData.AtsCmdStatusHandle[0] = 5;
    Expected_SC_OperData.AtsCmdStatusHandle[1] = 6;

    UtAssert_MemCmp(&SC_OperData.MsgPtr, &Expected_SC_OperData.MsgPtr, sizeof(Expected_SC_OperData.MsgPtr), "1");
    UtAssert_MemCmp(&SC_OperData.CmdPipe, &Expected_SC_OperData.CmdPipe, sizeof(Expected_SC_OperData.CmdPipe), "2");
    UtAssert_MemCmp(&SC_OperData.AtsTblAddr, &Expected_SC_OperData.AtsTblAddr, sizeof(Expected_SC_OperData.AtsTblAddr), "4");
    UtAssert_MemCmp(&SC_OperData.AppendTblAddr, &Expected_SC_OperData.AppendTblAddr, sizeof(Expected_SC_OperData.AppendTblAddr), "6");
    UtAssert_MemCmp(&SC_OperData.RtsTblAddr, &Expected_SC_OperData.RtsTblAddr, sizeof(Expected_SC_OperData.RtsTblAddr), "8");
    UtAssert_MemCmp(&SC_OperData.AtsInfoHandle, &Expected_SC_OperData.AtsInfoHandle, sizeof(Expected_SC_OperData.AtsInfoHandle), "AtsInfoHandle");
    UtAssert_MemCmp(&SC_OperData.AppendInfoHandle, &Expected_SC_OperData.AppendInfoHandle, sizeof(Expected_SC_OperData.AppendInfoHandle), "AppendInfoHandle");
    UtAssert_MemCmp(&SC_OperData.RtsInfoHandle, &Expected_SC_OperData.RtsInfoHandle, sizeof(Expected_SC_OperData.RtsInfoHandle), "13");
    UtAssert_MemCmp(&SC_OperData.RtsCtrlBlckHandle, &Expected_SC_OperData.RtsCtrlBlckHandle, sizeof(Expected_SC_OperData.RtsCtrlBlckHandle), "RtsCtrlBlckHandle");
    UtAssert_MemCmp(&SC_OperData.AtsCtrlBlckHandle, &Expected_SC_OperData.AtsCtrlBlckHandle, sizeof(Expected_SC_OperData.AtsCtrlBlckHandle), "AtsCtrlBlckHandle");
    UtAssert_MemCmp(&SC_OperData.AtsCmdStatusHandle, &Expected_SC_OperData.AtsCmdStatusHandle, sizeof(Expected_SC_OperData.AtsCmdStatusHandle), "AtsCmdStatusHandle");
    UtAssert_MemCmp(&SC_OperData.AtsDupTestArray, &Expected_SC_OperData.AtsDupTestArray, sizeof(Expected_SC_OperData.AtsDupTestArray), "21");
    UtAssert_MemCmp(&SC_OperData.NumCmdsSec, &Expected_SC_OperData.NumCmdsSec, sizeof(Expected_SC_OperData.NumCmdsSec), "22");
    UtAssert_MemCmp(&SC_OperData.HkPacket, &Expected_SC_OperData.HkPacket, sizeof(Expected_SC_OperData.HkPacket), "23");

    sprintf(Message, "SC Initialized. Version %d.%d.%d.%d", SC_MAJOR_VERSION, SC_MINOR_VERSION, SC_REVISION, SC_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(SC_INIT_INF_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_AppInit_Test_NominalPowerOnReset */

void SC_AppInit_Test_Nominal(void)
{
    uint32         ReturnValue;
    SC_OperData_t  Expected_SC_OperData;
    SC_AppData_t   Expected_SC_AppData;
    char           Message[CFE_EVS_MAX_MESSAGE_LENGTH];

    /* Set CFE_ES_GetResetType to return something other than CFE_ES_POWERON_RESET in order to reach "SC_AppData.AutoStartRTS = RTS_ID_AUTO_PROCESSOR" */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETRESETTYPE_INDEX, -99, 1);

    /* Prevents error messages in call to SC_GetLoadTablePointers */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 8);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    CFE_PSP_MemSet(&Expected_SC_OperData, 0, sizeof(SC_OperData_t));
    CFE_PSP_MemSet(&Expected_SC_AppData,  0, sizeof(SC_AppData_t));

    CFE_SB_InitMsg(&Expected_SC_OperData.HkPacket, SC_HK_TLM_MID, sizeof(SC_HkTlm_t), FALSE);

    Expected_SC_AppData.ContinueAtsOnFailureFlag  = SC_CONT_ON_FAILURE_START;
    Expected_SC_AppData.NextProcNumber            = SC_NONE;
    Expected_SC_AppData.NextCmdTime[SC_ATP]       = SC_MAX_TIME;
    Expected_SC_AppData.NextCmdTime[SC_RTP]       = SC_MAX_TIME;
    Expected_SC_AppData.AutoStartRTS              = RTS_ID_AUTO_PROCESSOR;

    /* Execute the function being tested */
    ReturnValue = SC_AppInit();
    
    /* Verify results */
    UtAssert_True (ReturnValue == CFE_SUCCESS, "ReturnValue == CFE_SUCCESS");
    UtAssert_MemCmp(&SC_AppData, &Expected_SC_AppData, sizeof(SC_AppData_t), "SC_AppData == Expected_SC_AppData");

    Expected_SC_OperData.AtsInfoHandle = 2;
    Expected_SC_OperData.AppendInfoHandle = 3;
    Expected_SC_OperData.RtsCtrlBlckHandle = 1;
    Expected_SC_OperData.AtsCtrlBlckHandle = 4;
    Expected_SC_OperData.AtsCmdStatusHandle[0] = 5;
    Expected_SC_OperData.AtsCmdStatusHandle[1] = 6;

    UtAssert_MemCmp(&SC_OperData.MsgPtr, &Expected_SC_OperData.MsgPtr, sizeof(Expected_SC_OperData.MsgPtr), "1");
    UtAssert_MemCmp(&SC_OperData.CmdPipe, &Expected_SC_OperData.CmdPipe, sizeof(Expected_SC_OperData.CmdPipe), "2");
    UtAssert_MemCmp(&SC_OperData.AtsTblAddr, &Expected_SC_OperData.AtsTblAddr, sizeof(Expected_SC_OperData.AtsTblAddr), "4");
    UtAssert_MemCmp(&SC_OperData.AppendTblAddr, &Expected_SC_OperData.AppendTblAddr, sizeof(Expected_SC_OperData.AppendTblAddr), "6");
    UtAssert_MemCmp(&SC_OperData.RtsTblAddr, &Expected_SC_OperData.RtsTblAddr, sizeof(Expected_SC_OperData.RtsTblAddr), "8");
    UtAssert_MemCmp(&SC_OperData.AtsInfoHandle, &Expected_SC_OperData.AtsInfoHandle, sizeof(Expected_SC_OperData.AtsInfoHandle), "AtsInfoHandle");
    UtAssert_MemCmp(&SC_OperData.AppendInfoHandle, &Expected_SC_OperData.AppendInfoHandle, sizeof(Expected_SC_OperData.AppendInfoHandle), "AppendInfoHandle");
    UtAssert_MemCmp(&SC_OperData.RtsInfoHandle, &Expected_SC_OperData.RtsInfoHandle, sizeof(Expected_SC_OperData.RtsInfoHandle), "13");
    UtAssert_MemCmp(&SC_OperData.RtsCtrlBlckHandle, &Expected_SC_OperData.RtsCtrlBlckHandle, sizeof(Expected_SC_OperData.RtsCtrlBlckHandle), "RtsCtrlBlckHandle");
    UtAssert_MemCmp(&SC_OperData.AtsCtrlBlckHandle, &Expected_SC_OperData.AtsCtrlBlckHandle, sizeof(Expected_SC_OperData.AtsCtrlBlckHandle), "AtsCtrlBlckHandle");
    UtAssert_MemCmp(&SC_OperData.AtsCmdStatusHandle, &Expected_SC_OperData.AtsCmdStatusHandle, sizeof(Expected_SC_OperData.AtsCmdStatusHandle), "AtsCmdStatusHandle");
    UtAssert_MemCmp(&SC_OperData.AtsDupTestArray, &Expected_SC_OperData.AtsDupTestArray, sizeof(Expected_SC_OperData.AtsDupTestArray), "21");
    UtAssert_MemCmp(&SC_OperData.NumCmdsSec, &Expected_SC_OperData.NumCmdsSec, sizeof(Expected_SC_OperData.NumCmdsSec), "22");
    UtAssert_MemCmp(&SC_OperData.HkPacket, &Expected_SC_OperData.HkPacket, sizeof(Expected_SC_OperData.HkPacket), "23");

    sprintf(Message, "SC Initialized. Version %d.%d.%d.%d", SC_MAJOR_VERSION, SC_MINOR_VERSION, SC_REVISION, SC_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(SC_INIT_INF_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_AppInit_Test_Nominal */

void SC_AppInit_Test_EVSRegisterError(void)
{
    /**
     **  Test case: "Event Services Register returned: 0x%08X\n"
     **/

    int32 ReturnValue;

    /* Set CFE_EVS_Register to return -1 in order to reach "CFE_ES_WriteToSysLog("Event Services Register returned: 0x%08X\n", Result)" */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SC_AppInit();
    
    /* Verify results */
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("Event Services Register returned: 0xFFFFFFFF\n"),
        "Event Services Register returned: 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end SC_AppInit_Test_EVSRegisterError */

void SC_AppInit_Test_SBCreatePipeError(void)
{
    /**
     **  Test case: "Software Bus Create Pipe returned: 0x%08X", Result
     **/

    int32 ReturnValue;

    /* Set CFE_SB_CreatePipe to return -1 in order to generate error message SC_INIT_SB_CREATE_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SC_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_INIT_SB_CREATE_ERR_EID,
            CFE_EVS_ERROR,
            "Software Bus Create Pipe returned: 0xFFFFFFFF"),
        "Software Bus Create Pipe returned: 0xFFFFFFFF");

    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_AppInit_Test_SBCreatePipeError */

void SC_AppInit_Test_SBSubscribeHKError(void)
{
    /**
     **  Test case: "Software Bus subscribe to housekeeping returned: 0x%08X", Result
     **/

    int32 ReturnValue;

    /* Set CFE_SB_Subscribe to return -1 on the first call in order to generate error message SC_INIT_SB_SUBSCRIBE_HK_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SC_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_INIT_SB_SUBSCRIBE_HK_ERR_EID,
            CFE_EVS_ERROR,
            "Software Bus subscribe to housekeeping returned: 0xFFFFFFFF"),
        "Software Bus subscribe to housekeeping returned: 0xFFFFFFFF");

    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_AppInit_Test_SBSubscribeHKError */

void SC_AppInit_Test_SubscribeTo1HzError(void)
{
    /**
     **  Test case: "Software Bus subscribe to 1 Hz cycle returned: 0x%08X", Result
     **/

    int32 ReturnValue;

    /* Set CFE_SB_Subscribe to return -1 on the 2nd call in order to generate error message SC_INIT_SB_SUBSCRIBE_1HZ_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 2);

    /* Execute the function being tested */
    ReturnValue = SC_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_INIT_SB_SUBSCRIBE_1HZ_ERR_EID,
            CFE_EVS_ERROR,
            "Software Bus subscribe to 1 Hz cycle returned: 0xFFFFFFFF"),
        "Software Bus subscribe to 1 Hz cycle returned: 0xFFFFFFFF");

    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_AppInit_Test_SubscribeTo1HzError */

void SC_AppInit_Test_SBSubscribeToCmdError(void)
{
    /**
     **  Test case: "Software Bus subscribe to command returned: 0x%08X", Result
     **/

    int32 ReturnValue;

    /* Set CFE_SB_Subscribe to return -1 on the 3rd call in order to generate error message SC_INIT_SB_SUBSCRIBE_CMD_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 3);

    /* Execute the function being tested */
    ReturnValue = SC_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_INIT_SB_SUBSCRIBE_CMD_ERR_EID,
            CFE_EVS_ERROR,
            "Software Bus subscribe to command returned: 0xFFFFFFFF"),
        "Software Bus subscribe to command returned: 0xFFFFFFFF");

    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_AppInit_Test_SBSubscribeToCmdError */

void SC_AppInit_Test_InitTablesError(void)
{
    /**
     **  Test case: "Software Bus subscribe to command returned: 0x%08X", Result
     **/

    int32 ReturnValue;

    /* Set CFE_TBL_Register to return -1 in order to reach return statement immediately after 
     * comment "Must be able to create and initialize tables" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Prevents error messages in call to SC_GetLoadTablePointers */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 8);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    ReturnValue = SC_AppInit();
    
    /* Verify results */
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_AppInit_Test_InitTablesError */

void SC_InitTables_Test_Nominal(void)
{
    int32 ReturnValue;

    /* Prevents error messages in call to SC_GetLoadTablePointers */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 8);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    ReturnValue = SC_InitTables();
    
    /* Verify results */
    UtAssert_True (ReturnValue == CFE_SUCCESS, "ReturnValue == CFE_SUCCESS");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_InitTables_Test_Nominal */

void SC_InitTables_Test_ErrorRegisterAllTables(void)
{
    int32 ReturnValue;

    /* Set CFE_TBL_Register to return -1 in order to cause SC_RegisterAllTables to return -1, in order to reach 
     * "return(Result)" after SC_RegisterAllTables */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Prevents error messages in call to SC_GetLoadTablePointers */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 8);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    ReturnValue = SC_InitTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_REGISTER_RTS_INFO_TABLE_ERR_EID,
            CFE_EVS_ERROR,
            "RTS info table register failed, returned: 0xFFFFFFFF"),
        "RTS info table register failed, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_InitTables_Test_ErrorRegisterAllTables */

void SC_InitTables_Test_ErrorGetDumpTablePointers(void)
{
    int32 ReturnValue;

    /* Set CFE_TBL_GetAddress to return -1 in order to cause SC_GetDumpTablePointers to return -1, in order to reach 
     * "return(Result)" after SC_GetDumpTablePointers */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SC_InitTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_GET_ADDRESS_RTS_INFO_ERR_EID,
            CFE_EVS_ERROR,
            "RTS Info table failed Getting Address, returned: 0xFFFFFFFF"),
        "RTS Info table failed Getting Address, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_InitTables_Test_ErrorGetDumpTablePointers */

void SC_InitTables_Test_ErrorGetLoadTablePointers(void)
{
    int32 ReturnValue;

    /* Causes SC_GetLoadTablePointers to return -1 in order to cause SC_GetLoadTablePointers to return -1, in order to reach 
     * "Return(Result)" after SC_GetLoadTablePointers */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, SC_NUMBER_OF_ATS + 6);

    /* Execute the function being tested */
    ReturnValue = SC_InitTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_GET_ADDRESS_ATS_ERR_EID,
            CFE_EVS_ERROR,
            "ATS table 1 failed Getting Address, returned: 0xFFFFFFFF"),
        "ATS table 1 failed Getting Address, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_InitTables_Test_ErrorGetLoadTablePointers */

void SC_RegisterAllTables_Test_Nominal(void)
{
    int32 ReturnValue;

    /* Execute the function being tested */
    ReturnValue = SC_RegisterAllTables();
    
    /* Verify results */
    UtAssert_True (ReturnValue == CFE_SUCCESS, "ReturnValue == CFE_SUCCESS");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SC_RegisterAllTables_Test_Nominal */

void SC_RegisterAllTables_Test_ErrorRegisterRTSInformation(void)
{
    int32 ReturnValue;

    /* Set CFE_TBL_Register to return -1 on the 1st call in order to generate error message SC_REGISTER_RTS_INFO_TABLE_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SC_RegisterAllTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_REGISTER_RTS_INFO_TABLE_ERR_EID,
            CFE_EVS_ERROR,
            "RTS info table register failed, returned: 0xFFFFFFFF"),
        "RTS info table register failed, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_RegisterAllTables_Test_ErrorRegisterRTSInformation */

void SC_RegisterAllTables_Test_ErrorRegisterRTPControl(void)
{
    int32 ReturnValue;

    /* Set CFE_TBL_Register to return -1 on the 2nd call in order to generate error message SC_REGISTER_RTS_CTRL_BLK_TABLE_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 2);

    /* Execute the function being tested */
    ReturnValue = SC_RegisterAllTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_REGISTER_RTS_CTRL_BLK_TABLE_ERR_EID,
            CFE_EVS_ERROR,
            "RTS control block table register failed, returned: 0xFFFFFFFF"),
        "RTS control block table register failed, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_RegisterAllTables_Test_ErrorRegisterRTPControl */

void SC_RegisterAllTables_Test_ErrorRegisterATSInformation(void)
{
    int32 ReturnValue;

    /* Set CFE_TBL_Register to return -1 on the 3rd call in order to generate error message SC_REGISTER_ATS_INFO_TABLE_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 3);

    /* Execute the function being tested */
    ReturnValue = SC_RegisterAllTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_REGISTER_ATS_INFO_TABLE_ERR_EID,
            CFE_EVS_ERROR,
            "ATS Info table register failed, returned: 0xFFFFFFFF"),
        "ATS Info table register failed, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_RegisterAllTables_Test_ErrorRegisterATSInformation */

void SC_RegisterAllTables_Test_ErrorRegisterAppendATSInformation(void)
{
    int32 ReturnValue;

    /* Set CFE_TBL_Register to return -1 on the 4th call in order to generate error message SC_REGISTER_APPEND_INFO_TABLE_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 4);

    /* Execute the function being tested */
    ReturnValue = SC_RegisterAllTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_REGISTER_APPEND_INFO_TABLE_ERR_EID,
            CFE_EVS_ERROR,
            "Append ATS Info table register failed, returned: 0xFFFFFFFF"),
        "Append ATS Info table register failed, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_RegisterAllTables_Test_ErrorRegisterAppendATSInformation */

void SC_RegisterAllTables_Test_ErrorRegisterATPControl(void)
{
    int32 ReturnValue;

    /* Set CFE_TBL_Register to return -1 on the 5th call in order to generate error message SC_REGISTER_ATS_CTRL_BLK_TABLE_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 5);

    /* Execute the function being tested */
    ReturnValue = SC_RegisterAllTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_REGISTER_ATS_CTRL_BLK_TABLE_ERR_EID,
            CFE_EVS_ERROR,
            "ATS control block table register failed, returned: 0xFFFFFFFF"),
        "ATS control block table register failed, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_RegisterAllTables_Test_ErrorRegisterATPControl */

void SC_RegisterAllTables_Test_ErrorRegisterATSCommandStatus(void)
{
    int32 ReturnValue;

    /* Set CFE_TBL_Register to return -1 on the 6th call in order to generate error message SC_REGISTER_ATS_CMD_STATUS_TABLE_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 6);

    /* Execute the function being tested */
    ReturnValue = SC_RegisterAllTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_REGISTER_ATS_CMD_STATUS_TABLE_ERR_EID,
            CFE_EVS_ERROR,
            "ATS command status table register failed for ATS 1, returned: 0xFFFFFFFF"),
        "ATS command status table register failed for ATS 1, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_RegisterAllTables_Test_ErrorRegisterATSCommandStatus */

void SC_RegisterAllTables_Test_ErrorRegisterLoadableRTS(void)
{
    int32 ReturnValue;

    /* Set CFE_TBL_Register to return -1 on the last call when registering loadable RTS tables in order to generate 
     * error message SC_REGISTER_RTS_TBL_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 6 + SC_NUMBER_OF_ATS);

    /* Execute the function being tested */
    ReturnValue = SC_RegisterAllTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_REGISTER_RTS_TBL_ERR_EID,
            CFE_EVS_ERROR,
            "RTS Table Registration Failed for RTS 1, returned: 0xFFFFFFFF"),
        "RTS Table Registration Failed for RTS 1, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_RegisterAllTables_Test_ErrorRegisterLoadableRTS */

void SC_RegisterAllTables_Test_ErrorRegisterLoadableATS(void)
{
    int32 ReturnValue;

    /* Set CFE_TBL_Register to return -1 on the last call when registering loadable ATS tables in order to generate 
     * error message SC_REGISTER_ATS_TBL_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 6 + SC_NUMBER_OF_ATS + SC_NUMBER_OF_RTS);

    /* Execute the function being tested */
    ReturnValue = SC_RegisterAllTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_REGISTER_ATS_TBL_ERR_EID,
            CFE_EVS_ERROR,
            "ATS Table Registration Failed for ATS 1, returned: 0xFFFFFFFF"),
        "ATS Table Registration Failed for ATS 1, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_RegisterAllTables_Test_ErrorRegisterLoadableATS */

void SC_RegisterAllTables_Test_ErrorRegisterLoadableAppendATS(void)
{
    int32 ReturnValue;

    /* Set CFE_TBL_Register to return -1 on the last call when registering loadable Append ATS tables in order to generate 
     * error message SC_REGISTER_APPEND_TBL_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 6 + 2*SC_NUMBER_OF_ATS + SC_NUMBER_OF_RTS);

    /* Execute the function being tested */
    ReturnValue = SC_RegisterAllTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_REGISTER_APPEND_TBL_ERR_EID,
            CFE_EVS_ERROR,
            "Append ATS Table Registration Failed, returned: 0xFFFFFFFF"),
        "Append ATS Table Registration Failed, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_RegisterAllTables_Test_ErrorRegisterLoadableAppendATS */

void SC_GetDumpTablePointers_Test_Nominal(void)
{
    int32 ReturnValue;

    /* Same return value as default, but bypasses default hook function to simplify test */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    ReturnValue = SC_GetDumpTablePointers();
    
    /* Verify results */
    UtAssert_True (ReturnValue == CFE_SUCCESS, "ReturnValue == CFE_SUCCESS");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SC_GetDumpTablePointers_Test_Nominal */

void SC_GetDumpTablePointers_Test_ErrorGetAddressRTSInformation(void)
{
    int32 ReturnValue;

    /* Need to set CFE_TBL_GetAddress to return -1 on 1st call (to generate error message SC_GET_ADDRESS_RTS_INFO_ERR_EID) and
     * CFE_SUCCESS on all other calls.  This could have been done using just a hook function and a global variable, but it was 
     * simpler to set a hook function that just returns CFE_SUCCESS and then also set a return code for the particular call 
     * number.  Because of the order of handling of return values and hook functions in the stub function, this results in the 
     * return code being used if it's the right call number, while otherwise using the hook function. */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CFE_TBL_GetAddressHookNominal);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SC_GetDumpTablePointers();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_GET_ADDRESS_RTS_INFO_ERR_EID,
            CFE_EVS_ERROR,
            "RTS Info table failed Getting Address, returned: 0xFFFFFFFF"),
        "RTS Info table failed Getting Address, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_GetDumpTablePointers_Test_ErrorGetAddressRTSInformation */

void SC_GetDumpTablePointers_Test_ErrorGetAddressRTPControl(void)
{
    int32 ReturnValue;

    /* Need to set CFE_TBL_GetAddress to return -1 on 2nd call (to generate error message SC_GET_ADDRESS_RTS_CTRL_BLCK_ERR_EID) and
     * CFE_SUCCESS on all other calls.  This could have been done using just a hook function and a global variable, but it was
     * simpler to set a hook function that just returns CFE_SUCCESS and then also set a return code for the particular call
     * number.  Because of the order of handling of return values and hook functions in the stub function, this results in the
     * return code being used if it's the right call number, while otherwise using the hook function. */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CFE_TBL_GetAddressHookNominal);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 2);

    /* Execute the function being tested */
    ReturnValue = SC_GetDumpTablePointers();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_GET_ADDRESS_RTS_CTRL_BLCK_ERR_EID,
            CFE_EVS_ERROR,
            "RTS Ctrl Blck table failed Getting Address, returned: 0xFFFFFFFF"),
        "RTS Ctrl Blck table failed Getting Address, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_GetDumpTablePointers_Test_ErrorGetAddressRTPControl */

void SC_GetDumpTablePointers_Test_ErrorGetAddressATSInformation(void)
{
    int32 ReturnValue;

    /* Need to set CFE_TBL_GetAddress to return -1 on 3rd call (to generate error message SC_GET_ADDRESS_ATS_INFO_ERR_EID) and
     * CFE_SUCCESS on all other calls.  This could have been done using just a hook function and a global variable, but it was
     * simpler to set a hook function that just returns CFE_SUCCESS and then also set a return code for the particular call
     * number.  Because of the order of handling of return values and hook functions in the stub function, this results in the
     * return code being used if it's the right call number, while otherwise using the hook function. */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CFE_TBL_GetAddressHookNominal);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 3);

    /* Execute the function being tested */
    ReturnValue = SC_GetDumpTablePointers();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_GET_ADDRESS_ATS_INFO_ERR_EID,
            CFE_EVS_ERROR,
            "ATS Info table failed Getting Address, returned: 0xFFFFFFFF"),
        "ATS Info table failed Getting Address, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_GetDumpTablePointers_Test_ErrorGetAddressATSInformation */

void SC_GetDumpTablePointers_Test_ErrorGetAddressAppendATSInformation(void)
{
    int32 ReturnValue;

    /* Need to set CFE_TBL_GetAddress to return -1 on 4th call (to generate error message SC_GET_ADDRESS_APPEND_INFO_ERR_EID) and
     * CFE_SUCCESS on all other calls.  This could have been done using just a hook function and a global variable, but it was
     * simpler to set a hook function that just returns CFE_SUCCESS and then also set a return code for the particular call
     * number.  Because of the order of handling of return values and hook functions in the stub function, this results in the
     * return code being used if it's the right call number, while otherwise using the hook function. */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CFE_TBL_GetAddressHookNominal);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 4);

    /* Execute the function being tested */
    ReturnValue = SC_GetDumpTablePointers();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_GET_ADDRESS_APPEND_INFO_ERR_EID,
            CFE_EVS_ERROR,
            "Append ATS Info table failed Getting Address, returned: 0xFFFFFFFF"),
        "Append ATS Info table failed Getting Address, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_GetDumpTablePointers_Test_ErrorGetAddressAppendATSInformation */

void SC_GetDumpTablePointers_Test_ErrorGetAddressATPControl(void)
{
    int32 ReturnValue;

    /* Need to set CFE_TBL_GetAddress to return -1 on 5th call (to generate error message SC_GET_ADDRESS_ATS_CTRL_BLCK_ERR_EID) and
     * CFE_SUCCESS on all other calls.  This could have been done using just a hook function and a global variable, but it was
     * simpler to set a hook function that just returns CFE_SUCCESS and then also set a return code for the particular call
     * number.  Because of the order of handling of return values and hook functions in the stub function, this results in the
     * return code being used if it's the right call number, while otherwise using the hook function. */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CFE_TBL_GetAddressHookNominal);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 5);

    /* Execute the function being tested */
    ReturnValue = SC_GetDumpTablePointers();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_GET_ADDRESS_ATS_CTRL_BLCK_ERR_EID,
            CFE_EVS_ERROR,
            "ATS Ctrl Blck table failed Getting Address, returned: 0xFFFFFFFF"),
        "ATS Ctrl Blck table failed Getting Address, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_GetDumpTablePointers_Test_ErrorGetAddressATPControl */

void SC_GetDumpTablePointers_Test_ErrorGetAddressATSCommandStatus(void)
{
    int32 ReturnValue;

    /* Need to set CFE_TBL_GetAddress to return -1 on 6th call (to generate error message SC_GET_ADDRESS_ATS_CMD_STAT_ERR_EID) and
     * CFE_SUCCESS on all other calls.  This could have been done using just a hook function and a global variable, but it was
     * simpler to set a hook function that just returns CFE_SUCCESS and then also set a return code for the particular call
     * number.  Because of the order of handling of return values and hook functions in the stub function, this results in the
     * return code being used if it's the right call number, while otherwise using the hook function. */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CFE_TBL_GetAddressHookNominal);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 6);

    /* Execute the function being tested */
    ReturnValue = SC_GetDumpTablePointers();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_GET_ADDRESS_ATS_CMD_STAT_ERR_EID,
            CFE_EVS_ERROR,
            "ATS Cmd Status table for ATS 1 failed Getting Address, returned: 0xFFFFFFFF"),
        "ATS Cmd Status table for ATS 1 failed Getting Address, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_GetDumpTablePointers_Test_ErrorGetAddressATSCommandStatus */

void SC_GetLoadTablePointers_Test_Nominal(void)
{
    int32 ReturnValue;

    /* Prevents all error messages */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_ERR_NEVER_LOADED, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETADDRESS_INDEX);

    /* Execute the function being tested */
    ReturnValue = SC_GetLoadTablePointers();
    
    /* Verify results */
    UtAssert_True (ReturnValue == CFE_SUCCESS, "ReturnValue == CFE_SUCCESS");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SC_GetLoadTablePointers_Test_Nominal */

void SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableATS(void)
{
    int32 ReturnValue;

    /* Need to set CFE_TBL_GetAddress to return -1 on 1st call (to generate error message SC_GET_ADDRESS_ATS_ERR_EID) and
     * CFE_TBL_ERR_NEVER_LOADED on all other calls.  This could have been done using just a hook function and a global variable,
     * but it was simpler to set a hook function that just returns CFE_SUCCESS and then also set a return code for the particular
     * call number.  Because of the order of handling of return values and hook functions in the stub function, this results in
     * the return code being used if it's the right call number, while otherwise using the hook function. */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CFE_TBL_GetAddressHookNominal2);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SC_GetLoadTablePointers();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_GET_ADDRESS_ATS_ERR_EID,
            CFE_EVS_ERROR,
            "ATS table 1 failed Getting Address, returned: 0xFFFFFFFF"),
        "ATS table 1 failed Getting Address, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableATS */

void SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableATSAppend(void)
{
    int32 ReturnValue;

    /* Need to set CFE_TBL_GetAddress to return -1 on call (SC_NUMBER_OF_ATS + 1) (to generate error message SC_GET_ADDRESS_APPEND_ERR_EID)
     * and CFE_TBL_ERR_NEVER_LOADED on all other calls.  This could have been done using just a hook function and a global variable,
     * but it was simpler to set a hook function that just returns CFE_SUCCESS and then also set a return code for the particular
     * call number.  Because of the order of handling of return values and hook functions in the stub function, this results in
     * the return code being used if it's the right call number, while otherwise using the hook function. */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CFE_TBL_GetAddressHookNominal2);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, SC_NUMBER_OF_ATS + 1);

    /* Execute the function being tested */
    ReturnValue = SC_GetLoadTablePointers();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_GET_ADDRESS_APPEND_ERR_EID,
            CFE_EVS_ERROR,
            "Append ATS table failed Getting Address, returned: 0xFFFFFFFF"),
        "Append ATS table failed Getting Address, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableATSAppend */

void SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableRTS(void)
{
    int32 ReturnValue;

    /* Need to set CFE_TBL_GetAddress to return -1 on call (SC_NUMBER_OF_ATS + 2) (to generate error message SC_GET_ADDRESS_RTS_ERR_EID)
     * and CFE_TBL_ERR_NEVER_LOADED on all other calls.  This could have been done using just a hook function and a global variable,
     * but it was simpler to set a hook function that just returns CFE_SUCCESS and then also set a return code for the particular
     * call number.  Because of the order of handling of return values and hook functions in the stub function, this results in
     * the return code being used if it's the right call number, while otherwise using the hook function. */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &CFE_TBL_GetAddressHookNominal2);
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, SC_NUMBER_OF_ATS + 2);

    /* Execute the function being tested */
    ReturnValue = SC_GetLoadTablePointers();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_GET_ADDRESS_RTS_ERR_EID,
            CFE_EVS_ERROR,
            "RTS table 1 failed Getting Address, returned: 0xFFFFFFFF"),
        "RTS table 1 failed Getting Address, returned: 0xFFFFFFFF");
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableRTS */

void SC_LoadDefaultTables_Test(void)
{

    /* Set OS_open to return 1, in order to enter if-block "if (FileDesc >= 0)" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPEN_INDEX, 1, 1);

    /* Same return value as default, but bypasses default hook function to simplify test */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_LOAD_INDEX, CFE_SUCCESS, 1);

    /* Execute the function being tested */
    SC_LoadDefaultTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_RTS_LOAD_COUNT_INFO_EID,
            CFE_EVS_INFORMATION,
            "RTS table file load count = 1"),
        "RTS table file load count = 1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_LoadDefaultTables_Test */

void SC_App_Test_AddTestCases(void)
{
    UtTest_Add(SC_AppMain_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_AppMain_Test_Nominal");
    UtTest_Add(SC_AppMain_Test_RegisterAppError, SC_Test_Setup, SC_Test_TearDown, "SC_AppMain_Test_RegisterAppError");
    UtTest_Add(SC_AppMain_Test_AppInitError, SC_Test_Setup, SC_Test_TearDown, "SC_AppMain_Test_AppInitError");
    UtTest_Add(SC_AppMain_Test_RcvMsgError, SC_Test_Setup, SC_Test_TearDown, "SC_AppMain_Test_RcvMsgError");
    UtTest_Add(SC_AppInit_Test_NominalPowerOnReset, SC_Test_Setup, SC_Test_TearDown, "SC_AppInit_Test_NominalPowerOnReset");
    UtTest_Add(SC_AppInit_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_AppInit_Test_Nominal");
    UtTest_Add(SC_AppInit_Test_EVSRegisterError, SC_Test_Setup, SC_Test_TearDown, "SC_AppInit_Test_EVSRegisterError");
    UtTest_Add(SC_AppInit_Test_SBCreatePipeError, SC_Test_Setup, SC_Test_TearDown, "SC_AppInit_Test_SBCreatePipeError");
    UtTest_Add(SC_AppInit_Test_SBSubscribeHKError, SC_Test_Setup, SC_Test_TearDown, "SC_AppInit_Test_SBSubscribeHKError");
    UtTest_Add(SC_AppInit_Test_SBSubscribeToCmdError, SC_Test_Setup, SC_Test_TearDown, "SC_AppInit_Test_SBSubscribeToCmdError");
    UtTest_Add(SC_AppInit_Test_SubscribeTo1HzError, SC_Test_Setup, SC_Test_TearDown, "SC_AppInit_Test_SubscribeTo1HzError");
    UtTest_Add(SC_AppInit_Test_InitTablesError, SC_Test_Setup, SC_Test_TearDown, "SC_AppInit_Test_InitTablesError");
    UtTest_Add(SC_InitTables_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_InitTables_Test_Nominal");
    UtTest_Add(SC_InitTables_Test_ErrorRegisterAllTables, SC_Test_Setup, SC_Test_TearDown, "SC_InitTables_Test_ErrorRegisterAllTables");
    UtTest_Add(SC_InitTables_Test_ErrorGetDumpTablePointers, SC_Test_Setup, SC_Test_TearDown, "SC_InitTables_Test_ErrorGetDumpTablePointers");
    UtTest_Add(SC_InitTables_Test_ErrorGetLoadTablePointers, SC_Test_Setup, SC_Test_TearDown, "SC_InitTables_Test_ErrorGetLoadTablePointers");
    UtTest_Add(SC_RegisterAllTables_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_RegisterAllTables_Test_Nominal");
    UtTest_Add(SC_RegisterAllTables_Test_ErrorRegisterRTSInformation, SC_Test_Setup, SC_Test_TearDown, "SC_RegisterAllTables_Test_ErrorRegisterRTSInformation");
    UtTest_Add(SC_RegisterAllTables_Test_ErrorRegisterRTPControl, SC_Test_Setup, SC_Test_TearDown, "SC_RegisterAllTables_Test_ErrorRegisterRTPControl");
    UtTest_Add(SC_RegisterAllTables_Test_ErrorRegisterATSInformation, SC_Test_Setup, SC_Test_TearDown, "SC_RegisterAllTables_Test_ErrorRegisterATSInformation");
    UtTest_Add(SC_RegisterAllTables_Test_ErrorRegisterAppendATSInformation, SC_Test_Setup, SC_Test_TearDown, "SC_RegisterAllTables_Test_ErrorRegisterAppendATSInformation");
    UtTest_Add(SC_RegisterAllTables_Test_ErrorRegisterATPControl, SC_Test_Setup, SC_Test_TearDown, "SC_RegisterAllTables_Test_ErrorRegisterATPControl");
    UtTest_Add(SC_RegisterAllTables_Test_ErrorRegisterATSCommandStatus, SC_Test_Setup, SC_Test_TearDown, "SC_RegisterAllTables_Test_ErrorRegisterATSCommandStatus");
    UtTest_Add(SC_RegisterAllTables_Test_ErrorRegisterLoadableRTS, SC_Test_Setup, SC_Test_TearDown, "SC_RegisterAllTables_Test_ErrorRegisterLoadableRTS");
    UtTest_Add(SC_RegisterAllTables_Test_ErrorRegisterLoadableATS, SC_Test_Setup, SC_Test_TearDown, "SC_RegisterAllTables_Test_ErrorRegisterLoadableATS");
    UtTest_Add(SC_RegisterAllTables_Test_ErrorRegisterLoadableAppendATS, SC_Test_Setup, SC_Test_TearDown, "SC_RegisterAllTables_Test_ErrorRegisterLoadableAppendATS");
    UtTest_Add(SC_GetDumpTablePointers_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_GetDumpTablePointers_Test_Nominal");
    UtTest_Add(SC_GetDumpTablePointers_Test_ErrorGetAddressRTSInformation, SC_Test_Setup, SC_Test_TearDown, "SC_GetDumpTablePointers_Test_ErrorGetAddressRTSInformation");
    UtTest_Add(SC_GetDumpTablePointers_Test_ErrorGetAddressRTPControl, SC_Test_Setup, SC_Test_TearDown, "SC_GetDumpTablePointers_Test_ErrorGetAddressRTPControl");
    UtTest_Add(SC_GetDumpTablePointers_Test_ErrorGetAddressATSInformation, SC_Test_Setup, SC_Test_TearDown, "SC_GetDumpTablePointers_Test_ErrorGetAddressATSInformation");
    UtTest_Add(SC_GetDumpTablePointers_Test_ErrorGetAddressAppendATSInformation, SC_Test_Setup, SC_Test_TearDown, "SC_GetDumpTablePointers_Test_ErrorGetAddressAppendATSInformation");
    UtTest_Add(SC_GetDumpTablePointers_Test_ErrorGetAddressATPControl, SC_Test_Setup, SC_Test_TearDown, "SC_GetDumpTablePointers_Test_ErrorGetAddressATPControl");
    UtTest_Add(SC_GetDumpTablePointers_Test_ErrorGetAddressATSCommandStatus, SC_Test_Setup, SC_Test_TearDown, "SC_GetDumpTablePointers_Test_ErrorGetAddressATSCommandStatus");
    UtTest_Add(SC_GetLoadTablePointers_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_GetLoadTablePointers_Test_Nominal");
    UtTest_Add(SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableATS, SC_Test_Setup, SC_Test_TearDown, "SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableATS");
    UtTest_Add(SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableATSAppend, SC_Test_Setup, SC_Test_TearDown, "SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableATSAppend");
    UtTest_Add(SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableRTS, SC_Test_Setup, SC_Test_TearDown, "SC_GetLoadTablePointers_Test_ErrorGetAddressLoadableRTS");
    UtTest_Add(SC_LoadDefaultTables_Test, SC_Test_Setup, SC_Test_TearDown, "SC_LoadDefaultTables_Test");
} /* end SC_App_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
