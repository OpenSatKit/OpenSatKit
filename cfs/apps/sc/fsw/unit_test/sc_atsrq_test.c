 /*************************************************************************
 ** File:
 **   $Id: sc_atsrq_test.c 1.3 2016/09/09 19:25:38EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file sc_atsrq.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_atsrq_test.c  $
 **   Revision 1.3 2016/09/09 19:25:38EDT czogby 
 **   Update based on code inspection log
 **   Revision 1.2 2016/02/04 16:02:48EST czogby 
 **   Add End-of-file and end-of-function comments
 **   Revision 1.1 2015/08/18 14:02:57EDT czogby 
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
#include "sc_state_test.h"
#include "sc_test_utils.h"
#include "ut_osapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Function Definitions
 */

CFE_TIME_Compare_t UT_SC_StartAtsRq_CompareHookAgreaterthanB(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
{
    return CFE_TIME_A_GT_B;
} /* end CFE_TIME_Compare_t UT_SC_StartAtsRq_CompareHookAgreaterthanB */

uint8 UT_SC_StartAtsRq_CompareHookRunCount;
CFE_TIME_Compare_t UT_SC_StartAtsRq_CompareHook3(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
{
    if (UT_SC_StartAtsRq_CompareHookRunCount == 0)
    {
        UT_SC_StartAtsRq_CompareHookRunCount++;
        return CFE_TIME_A_GT_B;
    }
    else
    {
        return CFE_TIME_A_LT_B;
    }
} /* CFE_TIME_Compare_t UT_SC_StartAtsRq_CompareHook3 */

void SC_StartAtsCmd_Test_Nominal(void)
{
    SC_StartAtsCmd_t   CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_START_ATS_CC);
    CmdPacket.AtsId = 1;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 1;

    /* Execute the function being tested */
    SC_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->AtpState == SC_EXECUTING, "SC_OperData.AtsCtrlBlckAddr->AtpState == SC_EXECUTING");
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STARTATS_CMD_INF_EID, CFE_EVS_INFORMATION, "ATS A Execution Started"),
         "ATS A Execution Started");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_StartAtsCmd_Test_Nominal */

void SC_StartAtsCmd_Test_CouldNotStart(void)
{
    SC_StartAtsCmd_t   CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_START_ATS_CC);
    CmdPacket.AtsId = 1;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 1;

    /* Set to cause SC_BeginAts to return FALSE, in order to reach block starting with "could not start the ats" */
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &UT_SC_StartAtsRq_CompareHookAgreaterthanB);

    /* Same return value as default, but bypasses default hook function to simplify test */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);
    Ut_CFE_PSP_MEMUTILS_ContinueReturnCodeAfterCountZero(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX);

    /* Execute the function being tested */
    SC_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_SKP_ALL_ERR_EID, CFE_EVS_ERROR, "All ATS commands were skipped, ATS stopped"),
         "All ATS commands were skipped, ATS stopped");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartAtsCmd_Test_CouldNotStart */

void SC_StartAtsCmd_Test_NoCommands(void)
{
    SC_StartAtsCmd_t   CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_START_ATS_CC);
    CmdPacket.AtsId = 1;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 0;

    /* Execute the function being tested */
    SC_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STARTATS_CMD_NOT_LDED_ERR_EID, CFE_EVS_ERROR, "Start ATS Rejected: ATS A Not Loaded"),
         "Start ATS Rejected: ATS A Not Loaded");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartAtsCmd_Test_NoCommands */

void SC_StartAtsCmd_Test_InUse(void)
{
    SC_StartAtsCmd_t   CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_START_ATS_CC);
    CmdPacket.AtsId = 1;
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;

    /* Execute the function being tested */
    SC_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STARTATS_CMD_NOT_IDLE_ERR_EID, CFE_EVS_ERROR, "Start ATS Rejected: ATP is not Idle"),
         "Start ATS Rejected: ATP is not Idle");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartAtsCmd_Test_InUse */

void SC_StartAtsCmd_Test_InvalidAtsId(void)
{
    SC_StartAtsCmd_t   CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_START_ATS_CC);
    CmdPacket.AtsId = 99;
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;

    /* Execute the function being tested */
    SC_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STARTATS_CMD_INVLD_ID_ERR_EID, CFE_EVS_ERROR, "Start ATS 99 Rejected: Invalid ATS ID"),
         "Start ATS 99 Rejected: Invalid ATS ID");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartAtsCmd_Test_InvalidAtsId */

void SC_StopAtsCmd_Test_NominalA(void)
{
    SC_StartAtsCmd_t   CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_START_ATS_CC);
    SC_OperData.AtsCtrlBlckAddr->AtsNumber = SC_ATSA;

    /* Execute the function being tested */
    SC_StopAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STOPATS_CMD_INF_EID, CFE_EVS_INFORMATION, "ATS A stopped"),
         "ATS A stopped");

    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StopAtsCmd_Test_NominalA */

void SC_StopAtsCmd_Test_NominalB(void)
{
    SC_StartAtsCmd_t   CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_START_ATS_CC);
    SC_OperData.AtsCtrlBlckAddr->AtsNumber = SC_ATSB;

    /* Execute the function being tested */
    SC_StopAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STOPATS_CMD_INF_EID, CFE_EVS_INFORMATION, "ATS B stopped"),
         "ATS B stopped");

    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StopAtsCmd_Test_NominalB */

void SC_StopAtsCmd_Test_NoRunningAts(void)
{
    SC_StartAtsCmd_t   CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_START_ATS_CC);
    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 99;

    /* Execute the function being tested */
    SC_StopAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STOPATS_NO_ATS_INF_EID, CFE_EVS_INFORMATION, "There is no ATS running to stop"),
         "There is no ATS running to stop");

    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StopAtsCmd_Test_NoRunningAts */

void SC_BeginAts_Test_Nominal(void)
{
    boolean Result;
    uint16 AtsIndex = 0;
    uint16 TimeOffset = 0;
    
    SC_InitTables();

    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 1;

    /* Execute the function being tested */
    Result = SC_BeginAts(AtsIndex, TimeOffset);
    
    /* Verify results */
    UtAssert_True(Result == TRUE, "Result == TRUE");
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->AtsNumber == 1, "SC_OperData.AtsCtrlBlckAddr->AtsNumber == 1");
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->CmdNumber == SC_AppData.AtsTimeIndexBuffer[AtsIndex][0], "SC_OperData.AtsCtrlBlckAddr->CmdNumber == SC_AppData.AtsTimeIndexBuffer[AtsIndex][0]");
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->TimeIndexPtr == 0, "SC_OperData.AtsCtrlBlckAddr->TimeIndexPtr == 0");
    UtAssert_True(SC_AppData.NextCmdTime[SC_ATP] == 0, "SC_AppData.NextCmdTime[SC_ATP] == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_ERR_SKP_DBG_EID, CFE_EVS_DEBUG, "ATS started, skipped 0 commands"),
         "ATS started, skipped 0 commands");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_BeginAts_Test_Nominal */

void SC_BeginAts_Test_AllCommandsSkipped(void)
{
    boolean Result;
    uint16 AtsIndex = 0;
    uint16 TimeOffset = 0;
    
    SC_InitTables();

    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 1;

    /* Set to cause all commnds to be skipped, to generate error message SC_ATS_SKP_ALL_ERR_EID */
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &UT_SC_StartAtsRq_CompareHookAgreaterthanB);

    /* Execute the function being tested */
    Result = SC_BeginAts(AtsIndex, TimeOffset);
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_SKIPPED, "SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_SKIPPED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_SKP_ALL_ERR_EID, CFE_EVS_ERROR, "All ATS commands were skipped, ATS stopped"),
         "All ATS commands were skipped, ATS stopped");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_BeginAts_Test_AllCommandsSkipped */

void SC_KillAts_Test(void)
{
    SC_InitTables();

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->AtpState = 99;

    /* Execute the function being tested */
    SC_KillAts();
    
    /* Verify results */
    UtAssert_True(SC_OperData.AtsInfoTblAddr[0].AtsUseCtr == 1, "SC_OperData.AtsInfoTblAddr[0].AtsUseCtr == 1");
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->AtpState == SC_IDLE, "SC_OperData.AtsCtrlBlckAddr->AtpState == SC_IDLE");
    UtAssert_True(SC_AppData.NextCmdTime[SC_ATP] == SC_MAX_TIME, "SC_AppData.NextCmdTime[SC_ATP] == SC_MAX_TIME");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_KillAts_Test */

void SC_GroundSwitchCmd_Test_Nominal(void)
{
    SC_InitTables();

    SC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SC_1HZ_WAKEUP_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* Execute the function being tested */
    SC_GroundSwitchCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_SWITCH_ATS_CMD_INF_EID, CFE_EVS_INFORMATION, "Switch ATS is Pending"),
         "Switch ATS is Pending");

    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == TRUE, "SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == TRUE");
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_GroundSwitchCmd_Test_Nominal */

void SC_GroundSwitchCmd_Test_DestinationAtsNotLoaded(void)
{
    SC_InitTables();

    SC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SC_1HZ_WAKEUP_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 0;

    /* Execute the function being tested */
    SC_GroundSwitchCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_SWITCH_ATS_CMD_NOT_LDED_ERR_EID, CFE_EVS_ERROR, "Switch ATS Failure: Destination ATS Not Loaded"),
         "Switch ATS Failure: Destination ATS Not Loaded");

    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE, "SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE");
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_GroundSwitchCmd_Test_DestinationAtsNotLoaded */

void SC_GroundSwitchCmd_Test_AtpIdle(void)
{
    SC_InitTables();

    SC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SC_1HZ_WAKEUP_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->AtpState = 99;

    /* Execute the function being tested */
    SC_GroundSwitchCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_SWITCH_ATS_CMD_IDLE_ERR_EID, CFE_EVS_ERROR, "Switch ATS Rejected: ATP is idle"),
         "Switch ATS Rejected: ATP is idle");

    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE, "SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE");
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_GroundSwitchCmd_Test_AtpIdle */

void SC_ServiceSwitchPend_Test_Nominal(void)
{
    SC_InitTables();

    /* Set to satisfy first if-statement, while not affecting later calls to CFE_TIME_Compare */
    UT_SC_StartAtsRq_CompareHookRunCount = 0;
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &UT_SC_StartAtsRq_CompareHook3);

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 1;
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* Execute the function being tested */
    SC_ServiceSwitchPend();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_SERVICE_SWTCH_INF_EID, CFE_EVS_INFORMATION , "ATS Switched from A to B"),
         "ATS Switched from A to B");

    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->AtpState == SC_EXECUTING, "SC_OperData.AtsCtrlBlckAddr->AtpState == SC_EXECUTING");
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE, "SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_ServiceSwitchPend_Test_Nominal */

void SC_ServiceSwitchPend_Test_AtsEmpty(void)
{
    SC_InitTables();

    /* Set to satisfy first if-statement, while not affecting later calls to CFE_TIME_Compare */
    UT_SC_StartAtsRq_CompareHookRunCount = 0;
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &UT_SC_StartAtsRq_CompareHook3);

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 0;
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 0;

    /* Execute the function being tested */
    SC_ServiceSwitchPend();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_SERVICE_SWITCH_ATS_CMD_LDED_ERR_EID, CFE_EVS_ERROR, "Switch ATS Failure: Destination ATS is empty"),
         "Switch ATS Failure: Destination ATS is empty");

    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE, "SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /*  end SC_ServiceSwitchPend_Test_AtsEmpty */

void SC_ServiceSwitchPend_Test_AtpIdle(void)
{
    SC_InitTables();

    /* Set to satisfy first if-statement, while not affecting later calls to CFE_TIME_Compare */
    UT_SC_StartAtsRq_CompareHookRunCount = 0;
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &UT_SC_StartAtsRq_CompareHook3);

    SC_OperData.AtsCtrlBlckAddr->AtpState = 99;

    /* Execute the function being tested */
    SC_ServiceSwitchPend();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_SERVICE_SWITCH_IDLE_ERR_EID, CFE_EVS_ERROR , "Switch ATS Rejected: ATP is idle"),
         "Switch ATS Rejected: ATP is idle");

    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE, "SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ServiceSwitchPend_Test_AtpIdle */

void SC_InlineSwitch_Test_Nominal(void)
{
    boolean Result;

    SC_InitTables();

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 1;
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* Execute the function being tested */
    Result = SC_InlineSwitch();
    
    /* Verify results */
    UtAssert_True(Result == TRUE, "Result == TRUE");
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->AtpState == SC_STARTING, "SC_OperData.AtsCtrlBlckAddr->AtpState == SC_STARTING");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_INLINE_SWTCH_INF_EID, CFE_EVS_INFORMATION, "ATS Switched from A to B"),
         "ATS Switched from A to B");

    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE, "SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_InlineSwitch_Test_Nominal */

void SC_InlineSwitch_Test_AllCommandsSkipped(void)
{
    boolean Result;

    SC_InitTables();

    /* Set to cause all commnds to be skipped, to reach block starting with comment "all of the commands in the new ats were skipped" */
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &UT_SC_StartAtsRq_CompareHookAgreaterthanB);

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 1;
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* Execute the function being tested */
    Result = SC_InlineSwitch();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE, "SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_InlineSwitch_Test_AllCommandsSkipped */

void SC_InlineSwitch_Test_DestinationAtsNotLoaded(void)
{
    boolean Result;

    SC_InitTables();

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 1;
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 0;

    /* Execute the function being tested */
    Result = SC_InlineSwitch();
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_INLINE_SWTCH_NOT_LDED_ERR_EID, CFE_EVS_ERROR, "Switch ATS Failure: Destination ATS Not Loaded"),
         "Switch ATS Failure: Destination ATS Not Loaded");

    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE, "SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_InlineSwitch_Test_DestinationAtsNotLoaded */

void SC_JumpAtsCmd_Test_SkipOneCmd(void)
{
    SC_JumpAtsCmd_t   CmdPacket;
    uint8 AtsIndex = 0;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_JumpAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_JUMP_ATS_CC);

    /* Set to satisfy first if-statement, while not affecting later calls to CFE_TIME_Compare */
    UT_SC_StartAtsRq_CompareHookRunCount = 0;
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &UT_SC_StartAtsRq_CompareHook3);

    SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] = SC_LOADED;
    SC_OperData.AtsCmdStatusTblAddr[AtsIndex][1] = SC_LOADED;
    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 2;

    /* Execute the function being tested */
    SC_JumpAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] == SC_SKIPPED, "SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] == SC_SKIPPED");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[AtsIndex][1] == SC_LOADED, "SC_OperData.AtsCmdStatusTblAddr[AtsIndex][1] == SC_LOADED");
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->CmdNumber == SC_AppData.AtsTimeIndexBuffer[AtsIndex][0], "SC_OperData.AtsCtrlBlckAddr->CmdNumber == SC_AppData.AtsTimeIndexBuffer[AtsIndex][0]");
    UtAssert_True(SC_OperData.AtsCtrlBlckAddr->TimeIndexPtr == 1, "SC_OperData.AtsCtrlBlckAddr->TimeIndexPtr == 1");
    UtAssert_True(SC_AppData.NextCmdTime[0] == 0, "SC_AppData.NextCmdTime[0] == 0");
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_JUMP_ATS_INF_EID, CFE_EVS_INFORMATION, "Next ATS command time in the ATP was set to 1980-001-00:00:00.00000"),
         "Next ATS command time in the ATP was set to 1980-001-00:00:00.00000");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_JUMP_ATS_SKIPPED_DBG_EID, CFE_EVS_DEBUG, "Jump Cmd: Skipped 1 ATS commands"),
         "Jump Cmd: Skipped 1 ATS commands");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_JumpAtsCmd_Test_SkipOneCmd */

void SC_JumpAtsCmd_Test_AllCommandsSkipped(void)
{
    SC_JumpAtsCmd_t   CmdPacket;
    uint8 AtsIndex = 0;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_JumpAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_JUMP_ATS_CC);

    /* Set to satisfy first if-statement, while not affecting later calls to CFE_TIME_Compare */
    UT_SC_StartAtsRq_CompareHookRunCount = 0;
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &UT_SC_StartAtsRq_CompareHook3);

    SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] = SC_LOADED;
    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;
    SC_OperData.AtsInfoTblAddr[0].NumberOfCommands = 1;

    /* Execute the function being tested */
    SC_JumpAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_JUMPATS_CMD_STOPPED_ERR_EID, CFE_EVS_ERROR, "Jump Cmd: All ATS commands were skipped, ATS stopped"),
         "Jump Cmd: All ATS commands were skipped, ATS stopped");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_JumpAtsCmd_Test_AllCommandsSkipped */

void SC_JumpAtsCmd_Test_NoRunningAts(void)
{
    SC_JumpAtsCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_JumpAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_JUMP_ATS_CC);

    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_IDLE;

    /* Execute the function being tested */
    SC_JumpAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_JUMPATS_CMD_NOT_ACT_ERR_EID, CFE_EVS_ERROR, "ATS Jump Failed: No active ATS"),
         "ATS Jump Failed: No active ATS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_JumpAtsCmd_Test_NoRunningAts */

void ContinueAtsOnFailureCmd_Test_Nominal(void)
{
    SC_SetContinueAtsOnFailureCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_SetContinueAtsOnFailureCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_CONTINUE_ATS_ON_FAILURE_CC);

    CmdPacket.ContinueState = TRUE;

    /* Execute the function being tested */
    SC_ContinueAtsOnFailureCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.ContinueAtsOnFailureFlag == TRUE, "SC_AppData.ContinueAtsOnFailureFlag == TRUE");
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_CONT_CMD_DEB_EID, CFE_EVS_DEBUG, "Continue-ATS-On-Failure command, State: 1"),
         "Continue-ATS-On-Failure command, State: 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end ContinueAtsOnFailureCmd_Test_Nominal */

void ContinueAtsOnFailureCmd_Test_InvalidState(void)
{
    SC_SetContinueAtsOnFailureCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_SetContinueAtsOnFailureCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_CONTINUE_ATS_ON_FAILURE_CC);

    CmdPacket.ContinueState = 99;

    /* Execute the function being tested */
    SC_ContinueAtsOnFailureCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_CONT_CMD_ERR_EID, CFE_EVS_ERROR, "Continue ATS On Failure command  failed, invalid state: 99"),
         "Continue ATS On Failure command  failed, invalid state: 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end ContinueAtsOnFailureCmd_Test_InvalidState */

void SC_AppendAtsCmd_Test_Nominal(void)
{
    SC_AppendAtsCmd_t   CmdPacket;
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 1;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_AppendAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_APPEND_ATS_CC);

    CmdPacket.AtsId = 1;
    SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands = 1;
    SC_AppData.AppendEntryCount = 1;

    /* Execute the function being tested */
    SC_AppendAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.AppendCmdArg == 1, "SC_AppData.AppendCmdArg == 1");
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_APPEND_CMD_INF_EID, CFE_EVS_INFORMATION,
                             "Append ATS A command: 1 ATS entries appended"),
         "Append ATS A command: 1 ATS entries appended");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_AppendAtsCmd_Test_Nominal */

void SC_AppendAtsCmd_Test_InvalidAtsId(void)
{
    SC_AppendAtsCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_AppendAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_APPEND_ATS_CC);

    CmdPacket.AtsId = 99;
    SC_AppData.AppendEntryCount = 1;

    /* Execute the function being tested */
    SC_AppendAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_APPEND_CMD_ARG_ERR_EID, CFE_EVS_ERROR,
                             "Append ATS error: invalid ATS ID = 99"),
         "Append ATS error: invalid ATS ID = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_AppendAtsCmd_Test_InvalidAtsId */

void SC_AppendAtsCmd_Test_AtsTableEmpty(void)
{
    SC_AppendAtsCmd_t   CmdPacket;
    uint8 AtsIndex = 0;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_AppendAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_APPEND_ATS_CC);

    CmdPacket.AtsId = 1;
    SC_AppData.AppendEntryCount = 1;
    SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands = 0;

    /* Execute the function being tested */
    SC_AppendAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_APPEND_CMD_TGT_ERR_EID, CFE_EVS_ERROR,
                             "Append ATS A error: ATS table is empty"),
         "Append ATS A error: ATS table is empty");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_AppendAtsCmd_Test_AtsTableEmpty */

void SC_AppendAtsCmd_Test_AppendTableEmpty(void)
{
    SC_AppendAtsCmd_t   CmdPacket;
    uint8 AtsIndex = 0;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_AppendAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_APPEND_ATS_CC);

    CmdPacket.AtsId = 1;
    SC_AppData.AppendEntryCount = 0;
    SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands = 1;

    /* Execute the function being tested */
    SC_AppendAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_APPEND_CMD_SRC_ERR_EID, CFE_EVS_ERROR,
                             "Append ATS A error: Append table is empty"),
         "Append ATS A error: Append table is empty");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_AppendAtsCmd_Test_AppendTableEmpty */

void SC_AppendAtsCmd_Test_NoRoomForAppendInAts(void)
{
    SC_AppendAtsCmd_t   CmdPacket;
    uint8 AtsIndex = 0;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_AppendAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_APPEND_ATS_CC);

    CmdPacket.AtsId = 1;
    SC_AppData.AppendEntryCount = 1;
    SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands = 1;
    SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize = SC_ATS_BUFF_SIZE;
    SC_AppData.AppendWordCount = SC_ATS_BUFF_SIZE;

    /* Execute the function being tested */
    SC_AppendAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_APPEND_CMD_FIT_ERR_EID, CFE_EVS_ERROR,
                             "Append ATS A error: ATS size = 8000, Append size = 8000, ATS buffer = 8000"),
         "Append ATS A error: ATS size = 8000, Append size = 8000, ATS buffer = 8000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_AppendAtsCmd_Test_NoRoomForAppendInAts */

void SC_Atsrq_Test_AddTestCases(void)
{
    UtTest_Add(SC_StartAtsCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_StartAtsCmd_Test_Nominal");
    UtTest_Add(SC_StartAtsCmd_Test_CouldNotStart, SC_Test_Setup, SC_Test_TearDown, "SC_StartAtsCmd_Test_CouldNotStart");
    UtTest_Add(SC_StartAtsCmd_Test_NoCommands, SC_Test_Setup, SC_Test_TearDown, "SC_StartAtsCmd_Test_NoCommands");
    UtTest_Add(SC_StartAtsCmd_Test_InUse, SC_Test_Setup, SC_Test_TearDown, "SC_StartAtsCmd_Test_InUse");
    UtTest_Add(SC_StartAtsCmd_Test_InvalidAtsId, SC_Test_Setup, SC_Test_TearDown, "SC_StartAtsCmd_Test_InvalidAtsId");
    UtTest_Add(SC_StopAtsCmd_Test_NominalA, SC_Test_Setup, SC_Test_TearDown, "SC_StopAtsCmd_Test_NominalA");
    UtTest_Add(SC_StopAtsCmd_Test_NominalB, SC_Test_Setup, SC_Test_TearDown, "SC_StopAtsCmd_Test_NominalB");
    UtTest_Add(SC_StopAtsCmd_Test_NoRunningAts, SC_Test_Setup, SC_Test_TearDown, "SC_StopAtsCmd_Test_NoRunningAts");
    UtTest_Add(SC_BeginAts_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_BeginAts_Test_Nominal");
    UtTest_Add(SC_BeginAts_Test_AllCommandsSkipped, SC_Test_Setup, SC_Test_TearDown, "SC_BeginAts_Test_AllCommandsSkipped");
    UtTest_Add(SC_KillAts_Test, SC_Test_Setup, SC_Test_TearDown, "SC_KillAts_Test");
    UtTest_Add(SC_GroundSwitchCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_GroundSwitchCmd_Test_Nominal");
    UtTest_Add(SC_GroundSwitchCmd_Test_DestinationAtsNotLoaded, SC_Test_Setup, SC_Test_TearDown, "SC_GroundSwitchCmd_Test_DestinationAtsNotLoaded");
    UtTest_Add(SC_GroundSwitchCmd_Test_AtpIdle, SC_Test_Setup, SC_Test_TearDown, "SC_GroundSwitchCmd_Test_AtpIdle");
    UtTest_Add(SC_ServiceSwitchPend_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_ServiceSwitchPend_Test_Nominal");
    UtTest_Add(SC_ServiceSwitchPend_Test_AtsEmpty, SC_Test_Setup, SC_Test_TearDown, "SC_ServiceSwitchPend_Test_AtsEmpty");
    UtTest_Add(SC_ServiceSwitchPend_Test_AtpIdle, SC_Test_Setup, SC_Test_TearDown, "SC_ServiceSwitchPend_Test_AtpIdle");
    UtTest_Add(SC_InlineSwitch_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_InlineSwitch_Test_Nominal");
    UtTest_Add(SC_InlineSwitch_Test_AllCommandsSkipped, SC_Test_Setup, SC_Test_TearDown, "SC_InlineSwitch_Test_AllCommandsSkipped");
    UtTest_Add(SC_InlineSwitch_Test_DestinationAtsNotLoaded, SC_Test_Setup, SC_Test_TearDown, "SC_InlineSwitch_Test_DestinationAtsNotLoaded");
    UtTest_Add(SC_JumpAtsCmd_Test_SkipOneCmd, SC_Test_Setup, SC_Test_TearDown, "SC_JumpAtsCmd_Test_SkipOneCmd");
    UtTest_Add(SC_JumpAtsCmd_Test_AllCommandsSkipped, SC_Test_Setup, SC_Test_TearDown, "SC_JumpAtsCmd_Test_AllCommandsSkipped");
    UtTest_Add(SC_JumpAtsCmd_Test_NoRunningAts, SC_Test_Setup, SC_Test_TearDown, "SC_JumpAtsCmd_Test_NoRunningAts");
    UtTest_Add(ContinueAtsOnFailureCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "ContinueAtsOnFailureCmd_Test_Nominal");
    UtTest_Add(ContinueAtsOnFailureCmd_Test_InvalidState, SC_Test_Setup, SC_Test_TearDown, "ContinueAtsOnFailureCmd_Test_InvalidState");
    UtTest_Add(SC_AppendAtsCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_AppendAtsCmd_Test_Nominal");
    UtTest_Add(SC_AppendAtsCmd_Test_InvalidAtsId, SC_Test_Setup, SC_Test_TearDown, "SC_AppendAtsCmd_Test_InvalidAtsId");
    UtTest_Add(SC_AppendAtsCmd_Test_AtsTableEmpty, SC_Test_Setup, SC_Test_TearDown, "SC_AppendAtsCmd_Test_AtsTableEmpty");
    UtTest_Add(SC_AppendAtsCmd_Test_AppendTableEmpty, SC_Test_Setup, SC_Test_TearDown, "SC_AppendAtsCmd_Test_AppendTableEmpty");
    UtTest_Add(SC_AppendAtsCmd_Test_NoRoomForAppendInAts, SC_Test_Setup, SC_Test_TearDown, "SC_AppendAtsCmd_Test_NoRoomForAppendInAts");
} /* end SC_Atsrq_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
