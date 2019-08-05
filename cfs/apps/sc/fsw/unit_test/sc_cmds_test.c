 /*************************************************************************
 ** File:
 **   $Id: sc_cmds_test.c 1.3 2016/09/09 19:25:36EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file sc_cmds.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_cmds_test.c  $
 **   Revision 1.3 2016/09/09 19:25:36EDT czogby 
 **   Update based on code inspection log
 **   Revision 1.2 2016/02/04 16:02:47EST czogby 
 **   Add End-of-file and end-of-function comments
 **   Revision 1.1 2015/08/18 14:02:58EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/sc/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "sc_cmds.h"
#include "sc_cmds_test.h"
#include "sc_events.h"
#include "sc_msgids.h"
#include "sc_test_utils.h"
#include "sc_utils.h"
#include "sc_version.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "cfe_tbl_msg.h"
#include <time.h>

/* STRINGIFY(x) is a preprocessor macro to convert preprocessor constants into strings */
#define STR(x) #x
#define STRINGIFY(x) STR(x)

extern SC_OperData_t SC_OperData;

/*
 * Function Definitions
 */

CFE_TIME_Compare_t Ut_CFE_TIME_CompareHookAlessthanB(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
{
    return CFE_TIME_A_LT_B;
} /* end CFE_TIME_Compare_t Ut_CFE_TIME_CompareHookAlessthanB */

void SC_ProcessAtpCmd_Test_SwitchCmd(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[0] = (uint16*)&AtsTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[0][0];
    Entry->CmdNumber = 1;
    
    SC_AppData.NextProcNumber               = SC_ATP;
    SC_OperData.AtsCtrlBlckAddr->AtpState   = SC_EXECUTING;

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->CmdNumber = 0;
    
    SC_OperData.AtsCmdStatusTblAddr[0][0]  = SC_LOADED;
    SC_AppData.AtsCmdIndexBuffer[0][0]     = 0;

    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* Set return value for CFE_TIME_Compare to make SC_CompareAbsTime return FALSE, to satisfy first if-statement of 
     * SC_ProcessAtpCmd, and for all other calls to CFE_TIME_Compare called from subfunctions reached by this test */
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &Ut_CFE_TIME_CompareHookAlessthanB);

    /* Set to return TRUE in order to satisfy the if-statement from which the function is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);

    /* Set these two functions to return these values in order to statisfy the if-statement from which they are both called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, SC_CMD_MID, 1);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, SC_SWITCH_ATS_CC, 1);
    
    /* Same return value as default, but bypasses default hook function to simplify test */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Execute the function being tested */
    SC_ProcessAtpCmd();
    
    /* Verify results */
    UtAssert_True(SC_AppData.AtsCmdCtr == 1, "SC_AppData.AtsCmdCtr == 1");
    UtAssert_True(SC_AppData.AtsCmdErrCtr == 0, "SC_AppData.AtsCmdErrCtr == 0");
    UtAssert_True(SC_OperData.NumCmdsSec == 1, "SC_OperData.NumCmdsSec == 1");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_EXECUTED, "SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_EXECUTED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_ProcessAtpCmd_Test_SwitchCmd */

void SC_ProcessAtpCmd_Test_NonSwitchCmd(void)
{    
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[0] = (uint16*)&AtsTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[0][0];
    Entry->CmdNumber = 1;
    
    SC_AppData.NextCmdTime[SC_ATP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_ATP;
    SC_OperData.AtsCtrlBlckAddr->AtpState   = SC_EXECUTING;

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->CmdNumber = 0;
    
    SC_OperData.AtsCmdStatusTblAddr[0][0]  = SC_LOADED;
    SC_AppData.AtsCmdIndexBuffer[0][0]     = 0;

    /* Set to return TRUE in order to satisfy the if-statement from which the function is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);

    /* Set to return this value in order to statisfy the if-statement from which it is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, SC_CMD_MID, 1);

    /* Set to return this value in order fail the if-statement from which it is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, SC_NOOP_CC, 1);

    /* Execute the function being tested */
    SC_ProcessAtpCmd();
    
    /* Verify results */
    UtAssert_True(SC_AppData.AtsCmdCtr == 1, "SC_AppData.AtsCmdCtr == 1");
    UtAssert_True(SC_AppData.AtsCmdErrCtr == 0, "SC_AppData.AtsCmdErrCtr == 0");
    UtAssert_True(SC_OperData.NumCmdsSec == 1, "SC_OperData.NumCmdsSec == 1");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_EXECUTED, "SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_EXECUTED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ProcessAtpCmd_Test_NonSwitchCmd */

void SC_ProcessAtpCmd_Test_InlineSwitchError(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[0] = (uint16*)&AtsTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[0][0];
    Entry->CmdNumber = 1;
    
    SC_AppData.NextProcNumber               = SC_ATP;
    SC_OperData.AtsCtrlBlckAddr->AtpState   = SC_EXECUTING;

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->CmdNumber = 0;
    
    SC_OperData.AtsCmdStatusTblAddr[0][0]  = SC_LOADED;
    SC_AppData.AtsCmdIndexBuffer[0][0]     = 0;

    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 0;
    
    /* Set return value for CFE_TIME_Compare to make SC_CompareAbsTime return FALSE, to satisfy first if-statement of 
     * SC_ProcessAtpCmd, and for all other calls to CFE_TIME_Compare called from subfunctions reached by this test */
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &Ut_CFE_TIME_CompareHookAlessthanB);

    /* Set to return TRUE in order to satisfy the if-statement from which the function is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);

    /* Set these two functions to return these values in order to statisfy the if-statement from which they are both called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, SC_CMD_MID, 1);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, SC_SWITCH_ATS_CC, 1);

    /* Same return value as default, but bypasses default hook function to simplify test */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    /* Execute the function being tested */
    SC_ProcessAtpCmd();
    
    /* Verify results */
    UtAssert_True(SC_AppData.AtsCmdCtr == 0, "SC_AppData.AtsCmdCtr == 0");
    UtAssert_True(SC_AppData.AtsCmdErrCtr == 1, "SC_AppData.AtsCmdErrCtr == 1");
    UtAssert_True(SC_OperData.NumCmdsSec == 1, "SC_OperData.NumCmdsSec == 1");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_FAILED_DISTRIB, "SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_FAILED_DISTRIB");
    UtAssert_True(SC_AppData.LastAtsErrSeq == 1, "SC_AppData.LastAtsErrSeq == 1");
    UtAssert_True(SC_AppData.LastAtsErrCmd == 1, "SC_AppData.LastAtsErrCmd == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_ProcessAtpCmd_Test_InlineSwitchError */

void SC_ProcessAtpCmd_Test_SBErrorAtsA(void)
{    
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[0] = (uint16*)&AtsTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[0][0];
    Entry->CmdNumber = 1;
    
    SC_AppData.NextCmdTime[SC_ATP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_ATP;
    SC_OperData.AtsCtrlBlckAddr->AtpState   = SC_EXECUTING;

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = SC_ATSA;
    SC_OperData.AtsCtrlBlckAddr->CmdNumber = 0;
    
    SC_OperData.AtsCmdStatusTblAddr[0][0]  = SC_LOADED;
    SC_AppData.AtsCmdIndexBuffer[0][0]     = 0;

    /* Set to return TRUE in order to satisfy the if-statement from which the function is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);

    /* Set to return this value in order to statisfy the if-statement from which it is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, SC_CMD_MID, 1);

    /* Set to return this value in order fail the if-statement from which it is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, SC_NOOP_CC, 1);

    /* Set to return -1 in order to generate error message SC_ATS_DIST_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SENDMSG_INDEX, -1, 1);

    /* Execute the function being tested */
    SC_ProcessAtpCmd();
    
    /* Verify results */
    UtAssert_True(SC_AppData.AtsCmdCtr == 0, "SC_AppData.AtsCmdCtr == 0");
    UtAssert_True(SC_AppData.AtsCmdErrCtr == 1, "SC_AppData.AtsCmdErrCtr == 1");
    UtAssert_True(SC_OperData.NumCmdsSec == 1, "SC_OperData.NumCmdsSec == 1");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_FAILED_DISTRIB, "SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_FAILED_DISTRIB");
    UtAssert_True(SC_AppData.LastAtsErrSeq == SC_ATSA, "SC_AppData.LastAtsErrSeq == SC_ATSA");
    UtAssert_True(SC_AppData.LastAtsErrCmd == 1, "SC_AppData.LastAtsErrCmd == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_DIST_ERR_EID, CFE_EVS_ERROR, "ATS Command Distribution Failed, Cmd Number: 1, SB returned: 0xFFFFFFFF"),
        "ATS Command Distribution Failed, Cmd Number: 1, SB returned: 0xFFFFFFFF");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_ABT_ERR_EID, CFE_EVS_ERROR, "ATS A Aborted"),
        "ATS A Aborted");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_ProcessAtpCmd_Test_SBErrorAtsA */

void SC_ProcessAtpCmd_Test_SBErrorAtsB(void)
{    
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[1] = (uint16*)&AtsTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[1][0];
    Entry->CmdNumber = 1;
    
    SC_AppData.NextCmdTime[SC_ATP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_ATP;
    SC_OperData.AtsCtrlBlckAddr->AtpState   = SC_EXECUTING;

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = SC_ATSB;
    SC_OperData.AtsCtrlBlckAddr->CmdNumber = 0;
    
    SC_OperData.AtsCmdStatusTblAddr[1][0]  = SC_LOADED;
    SC_AppData.AtsCmdIndexBuffer[1][0]     = 0;

    /* Set to return TRUE in order to satisfy the if-statement from which the function is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);

    /* Set to return this value in order to statisfy the if-statement from which it is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, SC_CMD_MID, 1);

    /* Set to return this value in order fail the if-statement from which it is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETCMDCODE_INDEX, SC_NOOP_CC, 1);

    /* Set to return -1 in order to generate error message SC_ATS_DIST_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SENDMSG_INDEX, -1, 1);

    /* Execute the function being tested */
    SC_ProcessAtpCmd();
    
    /* Verify results */
    UtAssert_True(SC_AppData.AtsCmdCtr == 0, "SC_AppData.AtsCmdCtr == 0");
    UtAssert_True(SC_AppData.AtsCmdErrCtr == 1, "SC_AppData.AtsCmdErrCtr == 1");
    UtAssert_True(SC_OperData.NumCmdsSec == 1, "SC_OperData.NumCmdsSec == 1");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[1][0] == SC_FAILED_DISTRIB, "SC_OperData.AtsCmdStatusTblAddr[1][0] == SC_FAILED_DISTRIB");
    UtAssert_True(SC_AppData.LastAtsErrSeq == SC_ATSB, "SC_AppData.LastAtsErrSeq == SC_ATSB");
    UtAssert_True(SC_AppData.LastAtsErrCmd == 1, "SC_AppData.LastAtsErrCmd == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_DIST_ERR_EID, CFE_EVS_ERROR, "ATS Command Distribution Failed, Cmd Number: 1, SB returned: 0xFFFFFFFF"),
        "ATS Command Distribution Failed, Cmd Number: 1, SB returned: 0xFFFFFFFF");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_ABT_ERR_EID, CFE_EVS_ERROR, "ATS B Aborted"),
        "ATS B Aborted");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_ProcessAtpCmd_Test_SBErrorAtsB */

void SC_ProcessAtpCmd_Test_ChecksumFailedAtsA(void)
{    
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[0] = (uint16*)&AtsTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[0][0];
    Entry->CmdNumber = 1;
    
    SC_AppData.NextCmdTime[SC_ATP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_ATP;
    SC_OperData.AtsCtrlBlckAddr->AtpState   = SC_EXECUTING;

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = SC_ATSA;
    SC_OperData.AtsCtrlBlckAddr->CmdNumber = 0;
    
    SC_OperData.AtsCmdStatusTblAddr[0][0]  = SC_LOADED;
    SC_AppData.AtsCmdIndexBuffer[0][0]     = 0;
    
    SC_AppData.ContinueAtsOnFailureFlag = FALSE;

    /* Set to return FALSE in order to generate error message SC_ATS_CHKSUM_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, FALSE, 1);

    /* Execute the function being tested */
    SC_ProcessAtpCmd();
    
    /* Verify results */
    UtAssert_True(SC_AppData.AtsCmdCtr == 0, "SC_AppData.AtsCmdCtr == 0");
    UtAssert_True(SC_AppData.AtsCmdErrCtr == 1, "SC_AppData.AtsCmdErrCtr == 1");
    UtAssert_True(SC_AppData.LastAtsErrSeq == SC_ATSA, "SC_AppData.LastAtsErrSeq == SC_ATSA");
    UtAssert_True(SC_AppData.LastAtsErrCmd == 1, "SC_AppData.LastAtsErrCmd == 1");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_FAILED_CHECKSUM, "SC_OperData.AtsCmdStatusTblAddr[1][0] == SC_FAILED_CHECKSUM");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_CHKSUM_ERR_EID, CFE_EVS_ERROR, "ATS Command Failed Checksum: Command #1 Skipped"),
        "ATS Command Failed Checksum: Command #1 Skipped");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_ABT_ERR_EID, CFE_EVS_ERROR, "ATS A Aborted"),
        "ATS A Aborted");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_ProcessAtpCmd_Test_ChecksumFailedAtsA */

void SC_ProcessAtpCmd_Test_ChecksumFailedAtsB(void)
{    
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[1] = (uint16*)&AtsTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[1][0];
    Entry->CmdNumber = 1;
    
    SC_AppData.NextCmdTime[SC_ATP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_ATP;
    SC_OperData.AtsCtrlBlckAddr->AtpState   = SC_EXECUTING;

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = SC_ATSB;
    SC_OperData.AtsCtrlBlckAddr->CmdNumber = 0;
    
    SC_OperData.AtsCmdStatusTblAddr[1][0]  = SC_LOADED;
    SC_AppData.AtsCmdIndexBuffer[1][0]     = 0;
    
    SC_AppData.ContinueAtsOnFailureFlag = FALSE;

    /* Set to return FALSE in order to generate error message SC_ATS_CHKSUM_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, FALSE, 1);

    /* Execute the function being tested */
    SC_ProcessAtpCmd();
    
    /* Verify results */
    UtAssert_True(SC_AppData.AtsCmdCtr == 0, "SC_AppData.AtsCmdCtr == 0");
    UtAssert_True(SC_AppData.AtsCmdErrCtr == 1, "SC_AppData.AtsCmdErrCtr == 1");
    UtAssert_True(SC_AppData.LastAtsErrSeq == SC_ATSB, "SC_AppData.LastAtsErrSeq == SC_ATSB");
    UtAssert_True(SC_AppData.LastAtsErrCmd == 1, "SC_AppData.LastAtsErrCmd == 1");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[1][0] == SC_FAILED_CHECKSUM, "SC_OperData.AtsCmdStatusTblAddr[1][0] == SC_FAILED_CHECKSUM");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_CHKSUM_ERR_EID, CFE_EVS_ERROR, "ATS Command Failed Checksum: Command #1 Skipped"),
        "ATS Command Failed Checksum: Command #1 Skipped");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_ABT_ERR_EID, CFE_EVS_ERROR, "ATS B Aborted"),
        "ATS B Aborted");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_ProcessAtpCmd_Test_ChecksumFailedAtsB */

void SC_ProcessAtpCmd_Test_CmdNumberMismatchAtsA(void)
{    
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[0] = (uint16*)&AtsTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[0][0];
    Entry->CmdNumber = 3;
    
    SC_AppData.NextCmdTime[SC_ATP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_ATP;
    SC_OperData.AtsCtrlBlckAddr->AtpState   = SC_EXECUTING;

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = SC_ATSA;
    SC_OperData.AtsCtrlBlckAddr->CmdNumber = 0;
    
    SC_OperData.AtsCmdStatusTblAddr[0][0]  = SC_LOADED;
    SC_AppData.AtsCmdIndexBuffer[0][0]     = 0;

    /* Execute the function being tested */
    SC_ProcessAtpCmd();
    
    /* Verify results */
    UtAssert_True(SC_AppData.AtsCmdCtr == 0, "SC_AppData.AtsCmdCtr == 0");
    UtAssert_True(SC_AppData.AtsCmdErrCtr == 1, "SC_AppData.AtsCmdErrCtr == 1");
    UtAssert_True(SC_AppData.LastAtsErrSeq == SC_ATSA, "SC_AppData.LastAtsErrSeq == SC_ATSA");
    UtAssert_True(SC_AppData.LastAtsErrCmd == 1, "SC_AppData.LastAtsErrCmd == 1");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_SKIPPED, "SC_OperData.AtsCmdStatusTblAddr[0][0] == SC_SKIPPED");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_MSMTCH_ERR_EID, CFE_EVS_ERROR, "ATS Command Number Mismatch: Command Skipped, expected: 1 received: 3"),
        "ATS Command Number Mismatch: Command Skipped, expected: 1 received: 3");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_ABT_ERR_EID, CFE_EVS_ERROR, "ATS A Aborted"),
        "ATS A Aborted");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_ProcessAtpCmd_Test_CmdNumberMismatchAtsA */

void SC_ProcessAtpCmd_Test_CmdNumberMismatchAtsB(void)
{    
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[1] = (uint16*)&AtsTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[1][0];
    Entry->CmdNumber = 3;
    
    SC_AppData.NextCmdTime[SC_ATP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_ATP;
    SC_OperData.AtsCtrlBlckAddr->AtpState   = SC_EXECUTING;

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = SC_ATSB;
    SC_OperData.AtsCtrlBlckAddr->CmdNumber = 0;
    
    SC_OperData.AtsCmdStatusTblAddr[1][0]  = SC_LOADED;
    SC_AppData.AtsCmdIndexBuffer[1][0]     = 0;

    /* Execute the function being tested */
    SC_ProcessAtpCmd();
    
    /* Verify results */
    UtAssert_True(SC_AppData.AtsCmdCtr == 0, "SC_AppData.AtsCmdCtr == 0");
    UtAssert_True(SC_AppData.AtsCmdErrCtr == 1, "SC_AppData.AtsCmdErrCtr == 1");
    UtAssert_True(SC_AppData.LastAtsErrSeq == SC_ATSB, "SC_AppData.LastAtsErrSeq == SC_ATSB");
    UtAssert_True(SC_AppData.LastAtsErrCmd == 1, "SC_AppData.LastAtsErrCmd == 1");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[1][0] == SC_SKIPPED, "SC_OperData.AtsCmdStatusTblAddr[1][0] == SC_SKIPPED");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_MSMTCH_ERR_EID, CFE_EVS_ERROR, "ATS Command Number Mismatch: Command Skipped, expected: 1 received: 3"),
        "ATS Command Number Mismatch: Command Skipped, expected: 1 received: 3");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_ABT_ERR_EID, CFE_EVS_ERROR, "ATS B Aborted"),
        "ATS B Aborted");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_ProcessAtpCmd_Test_CmdNumberMismatchAtsB */

void SC_ProcessAtpCmd_Test_CmdNotLoaded(void)
{    
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[0] = (uint16*)&AtsTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[0][0];
    Entry->CmdNumber = 1;
    
    SC_AppData.NextCmdTime[SC_ATP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_ATP;
    SC_OperData.AtsCtrlBlckAddr->AtpState   = SC_EXECUTING;

    SC_OperData.AtsCtrlBlckAddr->AtsNumber = SC_ATSA;
    SC_OperData.AtsCtrlBlckAddr->CmdNumber = 0;
    
    SC_AppData.AtsCmdIndexBuffer[0][0] = 0;

    /* Execute the function being tested */
    SC_ProcessAtpCmd();
    
    /* Verify results */
    UtAssert_True(SC_AppData.AtsCmdCtr == 0, "SC_AppData.AtsCmdCtr == 0");
    UtAssert_True(SC_AppData.AtsCmdErrCtr == 1, "SC_AppData.AtsCmdErrCtr == 1");
    UtAssert_True(SC_AppData.LastAtsErrSeq == SC_ATSA, "SC_AppData.LastAtsErrSeq == SC_ATSA");
    UtAssert_True(SC_AppData.LastAtsErrCmd == 1, "SC_AppData.LastAtsErrCmd == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_SKP_ERR_EID, CFE_EVS_ERROR, "Invalid ATS Command Status: Command Skipped, Status: 0"),
        "Invalid ATS Command Status: Command Skipped, Status: 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_ProcessAtpCmd_Test_CmdNotLoaded */

void SC_ProcessRtpCommand_Test_Nominal(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[0] = (uint16*)&RtsTable[0];

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[0][0];
    
    SC_AppData.NextCmdTime[SC_RTP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_RTP;
    SC_OperData.RtsCtrlBlckAddr->RtsNumber  = 1;
    SC_OperData.RtsInfoTblAddr[SC_OperData.RtsCtrlBlckAddr->RtsNumber - 1].RtsStatus = SC_EXECUTING;

    CFE_SB_InitMsg (&Entry->CmdHeader, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* Set to return TRUE in order to satisfy the if-statement from which the function is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);

    /* Set returned packet size to prevent error messages */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE / 2, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    SC_ProcessRtpCommand();
    
    /* Verify results */
    UtAssert_True(SC_AppData.RtsCmdCtr == 1, "SC_AppData.RtsCmdCtr == 1");
    UtAssert_True(SC_AppData.RtsCmdErrCtr == 0, "SC_AppData.RtsCmdErrCtr == 0");
    UtAssert_True(SC_OperData.NumCmdsSec == 1, "SC_OperData.NumCmdsSec == 1");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[0].CmdCtr == 1, "SC_OperData.RtsInfoTblAddr[0].CmdCtr == 1");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[0].CmdErrCtr == 0, "SC_OperData.RtsInfoTblAddr[0].CmdErrCtr == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_ProcessRtpCommand_Test_Nominal */

void SC_ProcessRtpCommand_Test_BadSoftwareBusReturn(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[0] = (uint16*)&RtsTable[0];

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[0][0];
    
    SC_AppData.NextCmdTime[SC_RTP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_RTP;
    SC_OperData.RtsCtrlBlckAddr->RtsNumber  = 1;
    SC_OperData.RtsInfoTblAddr[SC_OperData.RtsCtrlBlckAddr->RtsNumber - 1].RtsStatus = SC_EXECUTING;

    CFE_SB_InitMsg (&Entry->CmdHeader, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* Set to return TRUE in order to satisfy the if-statement from which the function is called */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SENDMSG_INDEX, -1, 1);

    /* Set returned packet size to prevent error messages */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE / 2, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    SC_ProcessRtpCommand();
    
    /* Verify results */
    UtAssert_True(SC_AppData.RtsCmdCtr == 0, "SC_AppData.RtsCmdCtr == 0");
    UtAssert_True(SC_AppData.RtsCmdErrCtr == 1, "SC_AppData.RtsCmdErrCtr == 1");
    UtAssert_True(SC_OperData.NumCmdsSec == 1, "SC_OperData.NumCmdsSec == 1");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[0].CmdCtr == 0, "SC_OperData.RtsInfoTblAddr[0].CmdCtr == 0");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[0].CmdErrCtr == 1, "SC_OperData.RtsInfoTblAddr[0].CmdErrCtr == 1");
    UtAssert_True(SC_AppData.LastRtsErrSeq == 1, "SC_AppData.LastRtsErrSeq == 1");
    UtAssert_True(SC_AppData.LastRtsErrCmd == 0, "SC_AppData.LastRtsErrCmd == 0");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_DIST_ERR_EID, CFE_EVS_ERROR, "RTS 001 Command Distribution Failed: RTS Stopped. SB returned 0xFFFFFFFF"),
         "RTS 001 Command Distribution Failed: RTS Stopped. SB returned 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ProcessRtpCommand_Test_BadSoftwareBusReturn */

void SC_ProcessRtpCommand_Test_BadChecksum(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[0] = (uint16*)&RtsTable[0];

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[0][0];
    
    SC_AppData.NextCmdTime[SC_RTP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_RTP;
    SC_OperData.RtsCtrlBlckAddr->RtsNumber  = 1;
    SC_OperData.RtsInfoTblAddr[SC_OperData.RtsCtrlBlckAddr->RtsNumber - 1].RtsStatus = SC_EXECUTING;

    CFE_SB_InitMsg (&Entry->CmdHeader, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* Set to return FALSE in order to generate error message SC_RTS_CHKSUM_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, FALSE, 1);

    /* Set returned packet size to prevent error messages */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE / 2, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    SC_ProcessRtpCommand();
    
    /* Verify results */
    UtAssert_True(SC_AppData.RtsCmdCtr == 0, "SC_AppData.RtsCmdCtr == 0");
    UtAssert_True(SC_AppData.RtsCmdErrCtr == 1, "SC_AppData.RtsCmdErrCtr == 1");
    UtAssert_True(SC_OperData.NumCmdsSec == 1, "SC_OperData.NumCmdsSec == 1");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[0].CmdCtr == 0, "SC_OperData.RtsInfoTblAddr[0].CmdCtr == 0");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[0].CmdErrCtr == 1, "SC_OperData.RtsInfoTblAddr[0].CmdErrCtr == 1");
    UtAssert_True(SC_AppData.LastRtsErrSeq == 1, "SC_AppData.LastRtsErrSeq == 1");
    UtAssert_True(SC_AppData.LastRtsErrCmd == 0, "SC_AppData.LastRtsErrCmd == 0");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_CHKSUM_ERR_EID, CFE_EVS_ERROR, "RTS 001 Command Failed Checksum: RTS Stopped"),
         "RTS 001 Command Failed Checksum: RTS Stopped");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ProcessRtpCommand_Test_BadChecksum */

void SC_SendHkPacket_Test(void)
{
    uint8 i;
    
    SC_InitTables();
    
    SC_AppData.CmdErrCtr = 1;
    SC_AppData.CmdCtr = 2;
    SC_AppData.RtsActiveErrCtr = 3;
    SC_AppData.RtsActiveCtr = 4;
    SC_AppData.AtsCmdCtr = 5;
    SC_AppData.AtsCmdErrCtr = 6;
    SC_AppData.RtsCmdCtr = 7;
    SC_AppData.RtsCmdErrCtr = 8;
    SC_AppData.LastAtsErrSeq = 9;
    SC_AppData.LastAtsErrCmd = 10;
    SC_AppData.LastRtsErrSeq = 11;
    SC_AppData.LastRtsErrCmd = 12;
    SC_AppData.AppendCmdArg = 13;
    SC_AppData.AppendEntryCount = 14;
    SC_AppData.AppendWordCount = 15;
    SC_AppData.AppendLoadCount = 16;
    SC_OperData.AtsInfoTblAddr->AtsSize = 0;
    SC_OperData.AtsCtrlBlckAddr -> AtsNumber = 17;
    SC_OperData.AtsCtrlBlckAddr -> AtpState = 18;
    SC_OperData.AtsCtrlBlckAddr -> CmdNumber = 19;
    SC_OperData.AtsCtrlBlckAddr -> SwitchPendFlag = 0;
    SC_AppData.NextCmdTime[0] = 0;
    SC_AppData.NextCmdTime[1] = 0;
    SC_OperData.RtsCtrlBlckAddr -> NumRtsActive = 20;
    SC_OperData.RtsCtrlBlckAddr -> RtsNumber = 21;
    SC_AppData.ContinueAtsOnFailureFlag = 1;
    
    for (i = 0; i < SC_NUMBER_OF_RTS ; i++)
    {
        SC_OperData.RtsInfoTblAddr[i].DisabledFlag = TRUE;
        SC_OperData.RtsInfoTblAddr[i].RtsStatus = SC_EXECUTING;
    }
    
    /* Execute the function being tested */
    SC_SendHkPacket();
    
    /* Verify results */
    UtAssert_True (SC_OperData.HkPacket.CmdErrCtr == 1, "SC_OperData.HkPacket.CmdErrCtr == 1");
    UtAssert_True (SC_OperData.HkPacket.CmdCtr == 2, "SC_OperData.HkPacket.CmdCtr == 2");    
    UtAssert_True (SC_OperData.HkPacket.RtsActiveErrCtr == 3, "SC_OperData.HkPacket.RtsActiveErrCtr == 3");
    UtAssert_True (SC_OperData.HkPacket.RtsActiveCtr == 4, "SC_OperData.HkPacket.RtsActiveCtr == 4");
    UtAssert_True (SC_OperData.HkPacket.AtsCmdCtr == 5, "SCSC_OperData.HkPacket.AtsCmdCtr == 5");
    UtAssert_True (SC_OperData.HkPacket.AtsCmdErrCtr == 6, "SC_OperData.HkPacket.AtsCmdErrCtr == 6");
    UtAssert_True (SC_OperData.HkPacket.RtsCmdCtr == 7, "SC_OperData.HkPacket.RtsCmdCtr == 7");
    UtAssert_True (SC_OperData.HkPacket.RtsCmdErrCtr == 8, "SC_OperData.HkPacket.RtsCmdErrCtr == 8");
    UtAssert_True (SC_OperData.HkPacket.LastAtsErrSeq == 9, "SC_OperData.HkPacket.LastAtsErrSeq == 9");
    UtAssert_True (SC_OperData.HkPacket.LastAtsErrCmd == 10, "SC_OperData.HkPacket.LastAtsErrCmd == 10");
    UtAssert_True (SC_OperData.HkPacket.LastRtsErrSeq == 11, "SC_OperData.HkPacket.LastRtsErrSeq == 11");
    UtAssert_True (SC_OperData.HkPacket.LastRtsErrCmd == 12, "SC_OperData.HkPacket.LastRtsErrCmd == 12");
    UtAssert_True (SC_OperData.HkPacket.AppendCmdArg == 13, "SC_OperData.HkPacket.AppendCmdArg == 13");
    UtAssert_True (SC_OperData.HkPacket.AppendEntryCount == 14, "SC_OperData.HkPacket.AppendEntryCount == 14");
    UtAssert_True (SC_OperData.HkPacket.AppendByteCount == 30, "SC_OperData.HkPacket.AppendByteCount == 30");
    UtAssert_True (SC_OperData.HkPacket.AppendLoadCount == 16, "SC_OperData.HkPacket.AppendLoadCount == 16");
    UtAssert_True (SC_OperData.HkPacket.AtpFreeBytes[0] == (SC_ATS_BUFF_SIZE * SC_BYTES_IN_WORD), "SC_OperData.HkPacket.AtpFreeBytes[0] == (SC_ATS_BUFF_SIZE * SC_BYTES_IN_WORD)");
    UtAssert_True (SC_OperData.HkPacket.AtpFreeBytes[1] == (SC_ATS_BUFF_SIZE * SC_BYTES_IN_WORD), "SC_OperData.HkPacket.AtpFreeBytes[1] == (SC_ATS_BUFF_SIZE * SC_BYTES_IN_WORD)");
    UtAssert_True (SC_OperData.HkPacket.AtsNumber == 17, "SC_OperData.HkPacket.AtsNumber == 17");
    UtAssert_True (SC_OperData.HkPacket.AtpState == 18, "SC_OperData.HkPacket.AtpState == 18");
    UtAssert_True (SC_OperData.HkPacket.AtpCmdNumber == 20, "SC_OperData.HkPacket.AtpCmdNumber == 20");
    UtAssert_True (SC_OperData.HkPacket.SwitchPendFlag == 0, "SC_OperData.HkPacket.SwitchPendFlag == 0");
    UtAssert_True (SC_OperData.HkPacket.NextAtsTime == 0, "SC_OperData.HkPacket.NextAtsTime == 0");
    UtAssert_True (SC_OperData.HkPacket.NumRtsActive == 20, "SC_OperData.HkPacket.NumRtsActive == 20");
    UtAssert_True (SC_OperData.HkPacket.RtsNumber == 21, "SC_OperData.HkPacket.RtsNumber == 21");
    UtAssert_True (SC_OperData.HkPacket.NextRtsTime == 0, "SC_OperData.HkPacket.NextRtsTime == 0");
    UtAssert_True (SC_OperData.HkPacket.ContinueAtsOnFailureFlag == 1, "SC_OperData.HkPacket.ContinueAtsOnFailureFlag == 1");
    
    /* Check first element */
    UtAssert_True (SC_OperData.HkPacket.RtsExecutingStatus[0] == 31, "SC_OperData.HkPacket.RtsExecutingStatus[0] == 31");
    UtAssert_True (SC_OperData.HkPacket.RtsDisabledStatus[0] == 31, "SC_OperData.HkPacket.RtsDisabledStatus[0] == 31");

    /* Check middle element.  Note: (SC_NUMBER_OF_RTS+15)/16 is taken directly from the source code */
    UtAssert_True (SC_OperData.HkPacket.RtsExecutingStatus[((SC_NUMBER_OF_RTS+15)/16) / 2] == 31, "SC_OperData.HkPacket.RtsExecutingStatus[((SC_NUMBER_OF_RTS+15)/16) / 2] == 31");
    UtAssert_True (SC_OperData.HkPacket.RtsDisabledStatus[((SC_NUMBER_OF_RTS+15)/16) / 2] == 31, "SC_OperData.HkPacket.RtsDisabledStatus[((SC_NUMBER_OF_RTS+15)/16) / 2] == 31");

    /* Check last element.  Note: (SC_NUMBER_OF_RTS+15)/16 is taken directly from the source code */
    UtAssert_True (SC_OperData.HkPacket.RtsExecutingStatus[((SC_NUMBER_OF_RTS+15)/16) - 1] == 31, "SC_OperData.HkPacket.RtsExecutingStatus[((SC_NUMBER_OF_RTS+15)/16) - 1] == 31");
    UtAssert_True (SC_OperData.HkPacket.RtsDisabledStatus[((SC_NUMBER_OF_RTS+15)/16) - 1] == 31, "SC_OperData.HkPacket.RtsDisabledStatus[((SC_NUMBER_OF_RTS+15)/16) - 1] == 31");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_SendHkPacket_Test */

void SC_ProcessRequest_Test_CmdMID(void)
{
    /**
     **  Test case: SC_CMD_MID
     **/

    SC_NoArgsCmd_t   CmdPacket;
    char             Message[CFE_EVS_MAX_MESSAGE_LENGTH];

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_NOOP_CC);

    /* Execute the function being tested */
    SC_ProcessRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");
    UtAssert_True(SC_AppData.CmdErrCtr == 0, "SC_AppData.CmdErrCtr == 0");

    sprintf(Message, "No-op command. Version %d.%d.%d.%d", SC_MAJOR_VERSION, SC_MINOR_VERSION, SC_REVISION, SC_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(SC_NOOP_INF_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_ProcessRequest_Test_CmdMID */

void SC_ProcessRequest_Test_HkMID(void)
{
    /**
     **  Test case: SC_SEND_HK_MID
     **/
    
    SC_InitTables();

    SC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SC_SEND_HK_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    SC_ProcessRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_ProcessRequest_Test_HkMID */

void SC_ProcessRequest_Test_1HzWakeupNONE(void)
{
    /**
     **  Test case: SC_1HZ_WAKEUP_MID with SC_AppData.NextProcNumber == SC_NONE
     **/
    
    SC_InitTables();

    SC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SC_1HZ_WAKEUP_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag = TRUE;
    SC_AppData.NextProcNumber = SC_NONE;
    SC_AppData.NextCmdTime[SC_ATP] = 0;
    SC_AppData.CurrentTime = 0;

    /* Execute the function being tested */
    SC_ProcessRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (SC_OperData.NumCmdsSec == 0, "SC_OperData.NumCmdsSec == 0");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_ProcessRequest_Test_1HzWakeupNONE */

void SC_ProcessRequest_Test_1HzWakeupAtpNotExecutionTime(void)
{
    /**
     **  Test case: SC_1HZ_WAKEUP_MID with a pending ATP command that should not execute yet
     **/
    
    SC_InitTables();

    SC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SC_1HZ_WAKEUP_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_OperData.AtsCtrlBlckAddr->SwitchPendFlag = TRUE;
    SC_AppData.NextProcNumber = SC_ATP;
    SC_AppData.NextCmdTime[SC_ATP] = 1000;

    /* Execute the function being tested */
    SC_ProcessRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (SC_OperData.NumCmdsSec == 0, "SC_OperData.NumCmdsSec == 0");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ProcessRequest_Test_1HzWakeupAtpNotExecutionTime */

void SC_ProcessRequest_Test_1HzWakeupRtpExecutionTime(void)
{
    /**
     **  Test case: SC_1HZ_WAKEUP_MID with a pending RTP command that needs to execute immediately
     **/
    
    SC_AtsEntryHeader_t*   Entry;
    SC_NoArgsCmd_t   CmdPacket;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[0] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[0][0];
    Entry->CmdNumber = 1;

    CFE_SB_InitMsg (&CmdPacket, SC_1HZ_WAKEUP_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_AppData.NextProcNumber = SC_RTP;
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING; /* Causes switch to ATP */
    SC_AppData.NextCmdTime[SC_ATP] = 0;
    SC_OperData.NumCmdsSec = 3;
    
    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;
    SC_OperData.AtsCtrlBlckAddr->CmdNumber = 1;
    
    SC_OperData.AtsCmdStatusTblAddr[0][0]  = SC_LOADED;
    SC_AppData.AtsCmdIndexBuffer[0][0]     = 0;

    /* Execute the function being tested */
    SC_ProcessRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 4, "Ut_CFE_EVS_GetEventQueueDepth() == 4");

} /* end SC_ProcessRequest_Test_1HzWakeupRtpExecutionTime */

void SC_ProcessRequest_Test_1HzWakeupRtpExecutionTimeTooManyCmds(void)
{
    /**
     **  Test case: SC_1HZ_WAKEUP_MID with a pending RTP command that needs to execute immediately, but too many commands are being sent at once
     **/

    SC_InitTables();

    SC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SC_1HZ_WAKEUP_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_AppData.NextProcNumber = SC_RTP;
    SC_AppData.NextCmdTime[SC_RTP] = 0;
    SC_AppData.NextCmdTime[SC_ATP] = 0;
    SC_OperData.NumCmdsSec = 1000;
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;

    /* Execute the function being tested */
    SC_ProcessRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (SC_OperData.NumCmdsSec == 0, "SC_OperData.NumCmdsSec == 0");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_ProcessRequest_Test_1HzWakeupRtpExecutionTimeTooManyCmds */

void SC_ProcessRequest_Test_MIDError(void)
{
    /**
     **  Test case: SC_MID_ERR_EID
     **/

    SC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, 255, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_NOOP_CC);

    /* Execute the function being tested */
    SC_ProcessRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "CmdErrCtr == 1");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_MID_ERR_EID, CFE_EVS_ERROR, "Invalid command pipe message ID: 0x000000FF"),
        "Invalid command pipe message ID: 0x000000FF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_ProcessRequest_Test_MIDError */

void SC_ProcessCommand_Test_NoOp(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t     CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_NOOP_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");
    UtAssert_True(SC_AppData.CmdErrCtr == 0, "SC_AppData.CmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SC_NOOP_INF_EID,
            CFE_EVS_INFORMATION,
            "No-op command. Version " STRINGIFY (SC_MAJOR_VERSION) "." STRINGIFY (SC_MINOR_VERSION) "." STRINGIFY (SC_REVISION) "." STRINGIFY (SC_MISSION_REV)),
        "No-op command. Version " STRINGIFY (SC_MAJOR_VERSION) "." STRINGIFY (SC_MINOR_VERSION) "." STRINGIFY (SC_REVISION) "." STRINGIFY (SC_MISSION_REV));

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_ProcessCommand_Test_NoOp */

void SC_ProcessCommand_Test_ResetCounters(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_RESET_COUNTERS_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SC_AppData.CmdErrCtr == 0, "CmdErrCtr == 0");
    UtAssert_True(SC_AppData.AtsCmdCtr == 0, "AtsCmdCtr == 0");
    UtAssert_True(SC_AppData.AtsCmdErrCtr == 0, "AtsCmdErrCtr == 0");
    UtAssert_True(SC_AppData.RtsCmdCtr == 0, "RtsCmdCtr == 0");
    UtAssert_True(SC_AppData.RtsCmdErrCtr == 0, "RtsCmdErrCtr == 0");
    UtAssert_True(SC_AppData.RtsActiveCtr == 0, "RtsActiveCtr == 0");
    UtAssert_True(SC_AppData.RtsActiveErrCtr == 0, "RtsActiveErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RESET_DEB_EID, CFE_EVS_DEBUG, "Reset counters command"),
        "Reset counters command");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SC_ProcessCommand_Test_ResetCounters */

void SC_ProcessCommand_Test_StartAts(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_StartAtsCmd_t   CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_START_ATS_CC);
    CmdPacket.AtsId = 1;

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_StartAts */

void SC_ProcessCommand_Test_StopAts(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_STOP_ATS_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_StopAts */

void SC_ProcessCommand_Test_StartRts(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_START_RTS_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_StartRts */

void SC_ProcessCommand_Test_StopRts(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_STOP_RTS_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_StopRts */

void SC_ProcessCommand_Test_DisableRts(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_DISABLE_RTS_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_DisableRts */

void SC_ProcessCommand_Test_EnableRts(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_ENABLE_RTS_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_EnableRts */

void SC_ProcessCommand_Test_SwitchAts(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_SWITCH_ATS_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_SwitchAts */

void SC_ProcessCommand_Test_JumpAts(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_JUMP_ATS_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_JumpAts */

void SC_ProcessCommand_Test_ContinueAtsOnFailure(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_CONTINUE_ATS_ON_FAILURE_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_ContinueAtsOnFailure */

void SC_ProcessCommand_Test_AppendAts(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_APPEND_ATS_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_AppendAts */

void SC_ProcessCommand_Test_TableManageAtsTableNominal(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[0] = (uint16*)&AtsTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[0][0];
    Entry->CmdNumber = 0;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_ATS_0;

    /* Set to reach "SC_LoadAts(ArrayIndex)" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_TableManageAtsTableNominal */

void SC_ProcessCommand_Test_TableManageAtsTableGetAddressError(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_ATS_0;

    /* Set to generate error message SC_TABLE_MANAGE_ATS_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
     UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_TABLE_MANAGE_ATS_ERR_EID, CFE_EVS_ERROR, "ATS table manage process error: ATS = 1, Result = 0xFFFFFFFF"),
        "ATS table manage process error: ATS = 1, Result = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ProcessCommand_Test_TableManageAtsTableGetAddressError */

void SC_ProcessCommand_Test_TableManageAppendTableNominal(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsAppendTable[SC_APPEND_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AppendTblAddr = (uint16*)&AtsAppendTable[0];

    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AppendTblAddr;
    Entry->CmdNumber = 0;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_APPEND;

    /* Set to reach "SC_UpdateAppend()" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_TableManageAppendTableNominal */

void SC_ProcessCommand_Test_TableManageAppendTableGetAddressError(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_APPEND;

    /* Set to generate error message SC_TABLE_MANAGE_APPEND_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
     UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_TABLE_MANAGE_APPEND_ERR_EID, CFE_EVS_ERROR, "ATS Append table manage process error: Result = 0xFFFFFFFF"),
        "ATS Append table manage process error: Result = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ProcessCommand_Test_TableManageAppendTableGetAddressError */

void SC_ProcessCommand_Test_TableManageRtsTableNominal(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    SC_RtsEntryHeader_t*   Entry;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[0] = (uint16*)&RtsTable[0];

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[0][0];

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_RTS_0;

    /* Set to reach "SC_LoadRts(ArrayIndex)" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, CFE_TBL_INFO_UPDATED, 1);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_TableManageRtsTableNominal */

void SC_ProcessCommand_Test_TableManageRtsTableGetAddressError(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_RTS_0;

    /* Set to generate error message SC_TABLE_MANAGE_RTS_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETADDRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
     UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_TABLE_MANAGE_RTS_ERR_EID, CFE_EVS_ERROR, "RTS table manage process error: RTS = 1, Result = 0xFFFFFFFF"),
        "RTS table manage process error: RTS = 1, Result = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ProcessCommand_Test_TableManageRtsTableGetAddressError */

void SC_ProcessCommand_Test_TableManageRtsInfo(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_RTS_INFO;

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_TableManageRtsInfo */

void SC_ProcessCommand_Test_TableManageRtpCtrl(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_RTP_CTRL;

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_TableManageRtpCtrl */

void SC_ProcessCommand_Test_TableManageAtsInfo(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_ATS_INFO;

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_TableManageAtsInfo */

void SC_ProcessCommand_Test_TableManageAppendInfo(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_APP_INFO;

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_TableManageAppendInfo */

void SC_ProcessCommand_Test_TableManageAtpCtrl(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_ATP_CTRL;

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_TableManageAtpCtrl */

void SC_ProcessCommand_Test_TableManageAtsCmdStatus(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = SC_TBL_ID_ATS_CMD_0;

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_TableManageAtsCmdStatus */

void SC_ProcessCommand_Test_TableManageInvalidTableID(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    CFE_TBL_NotifyCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(CFE_TBL_NotifyCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_MANAGE_TABLE_CC);
    CmdPacket.Payload.Parameter = 999;

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
     UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_TABLE_MANAGE_ID_ERR_EID, CFE_EVS_ERROR, "Table manage command packet error: table ID = 999"),
        "Table manage command packet error: table ID = 999");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ProcessCommand_Test_TableManageInvalidTableID */

void SC_ProcessCommand_Test_StartRtsGrp(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_START_RTSGRP_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_StartRtsGrp */

void SC_ProcessCommand_Test_StopRtsGrp(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_STOP_RTSGRP_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_StopRtsGrp */

void SC_ProcessCommand_Test_DisableRtsGrp(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_DISABLE_RTSGRP_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_DisableRtsGrp */

void SC_ProcessCommand_Test_EnableRtsGrp(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_NoArgsCmd_t    CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_ENABLE_RTSGRP_CC);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* This function is already verified to work correctly in another file, so no verifications here. */

} /* end SC_ProcessCommand_Test_EnableRtsGrp */

void SC_ProcessCommand_Test_InvalidCmdError(void)
{
    /**
     **  Note: This test does not follow the standard test guideline to only test what's directly in the function-under-test.
     **  Since the code for reaching each branch in SC_ProcessCommand is so trivial and non-verifiable, it was decided to
     **  combine the tests for each command with the tests for reaching the command from SC_ProcessCommand.
     **/

    SC_StartAtsCmd_t   CmdPacket;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 99);

    /* Execute the function being tested */
    SC_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "CmdErrCtr == 1");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_INVLD_CMD_ERR_EID, CFE_EVS_ERROR, "Invalid Command Code: MID =  0x18A9 CC =  99"),
        "Invalid Command Code: MID =  0x18A9 CC =  99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ProcessCommand_Test_InvalidCmdError */

/* SC_VerifyCmdLength_Test_Nominal not needed - nominal case already tested in other functions. */
void SC_VerifyCmdLength_Test_LenError(void)
{
    /**
     **  Test case: SC_LEN_ERR_EID
     **/

    SC_NoArgsCmd_t CmdPacket;
    boolean Result;
    
    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SC_NOOP_CC);

    /* Execute the function being tested */
    Result = SC_VerifyCmdLength((CFE_SB_MsgPtr_t)(&CmdPacket), 999);
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");
    UtAssert_True(SC_AppData.CmdCtr == 0, "SC_AppData.CmdCtr == 0");
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid msg length: ID = 0x18A9, CC = 0, Len = 8, Expected = 999"),
        "Invalid msg length: ID = 0x18A9, CC = 0, Len = 8, Expected = 999");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_VerifyCmdLength_Test_LenError */

void SC_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(SC_ProcessAtpCmd_Test_SwitchCmd, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessAtpCmd_Test_SwitchCmd");
    UtTest_Add(SC_ProcessAtpCmd_Test_NonSwitchCmd, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessAtpCmd_Test_NonSwitchCmd");

    UtTest_Add(SC_ProcessAtpCmd_Test_InlineSwitchError, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessAtpCmd_Test_InlineSwitchError");
    UtTest_Add(SC_ProcessAtpCmd_Test_SBErrorAtsA, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessAtpCmd_Test_SBErrorAtsA");
    UtTest_Add(SC_ProcessAtpCmd_Test_SBErrorAtsB, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessAtpCmd_Test_SBErrorAtsB");
    UtTest_Add(SC_ProcessAtpCmd_Test_ChecksumFailedAtsA, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessAtpCmd_Test_ChecksumFailedAtsA");
    UtTest_Add(SC_ProcessAtpCmd_Test_ChecksumFailedAtsB, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessAtpCmd_Test_ChecksumFailedAtsB");
    UtTest_Add(SC_ProcessAtpCmd_Test_CmdNumberMismatchAtsA, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessAtpCmd_Test_CmdNumberMismatchAtsA");
    UtTest_Add(SC_ProcessAtpCmd_Test_CmdNumberMismatchAtsB, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessAtpCmd_Test_CmdNumberMismatchAtsB");
    UtTest_Add(SC_ProcessAtpCmd_Test_CmdNotLoaded, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessAtpCmd_Test_CmdNotLoaded");
    
    UtTest_Add(SC_ProcessRtpCommand_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessRtpCommand_Test_Nominal");
    UtTest_Add(SC_ProcessRtpCommand_Test_BadSoftwareBusReturn, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessRtpCommand_Test_BadSoftwareBusReturn");
    UtTest_Add(SC_ProcessRtpCommand_Test_BadChecksum, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessRtpCommand_Test_BadChecksum");
    
    UtTest_Add(SC_SendHkPacket_Test, SC_Test_Setup, SC_Test_TearDown, "SC_SendHkPacket_Test");
    UtTest_Add(SC_ProcessRequest_Test_CmdMID, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessRequest_Test_CmdMID");
    UtTest_Add(SC_ProcessRequest_Test_HkMID, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessRequest_Test_HkMID");
    UtTest_Add(SC_ProcessRequest_Test_1HzWakeupNONE, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessRequest_Test_1HzWakeupNONE");
    UtTest_Add(SC_ProcessRequest_Test_1HzWakeupAtpNotExecutionTime, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessRequest_Test_1HzWakeupAtpNotExecutionTime");
    UtTest_Add(SC_ProcessRequest_Test_1HzWakeupRtpExecutionTime, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessRequest_Test_1HzWakeupRtpExecutionTime");
    UtTest_Add(SC_ProcessRequest_Test_1HzWakeupRtpExecutionTimeTooManyCmds, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessRequest_Test_1HzWakeupRtpExecutionTimeTooManyCmds");
    UtTest_Add(SC_ProcessRequest_Test_MIDError, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessRequest_Test_MIDError");
    UtTest_Add(SC_ProcessCommand_Test_NoOp, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_NoOp");
    UtTest_Add(SC_ProcessCommand_Test_ResetCounters, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_ResetCounters");
    UtTest_Add(SC_ProcessCommand_Test_StartAts, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_StartAts");
    UtTest_Add(SC_ProcessCommand_Test_StopAts, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_StopAts");
    
    UtTest_Add(SC_ProcessCommand_Test_StartRts, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_StartRts");
    UtTest_Add(SC_ProcessCommand_Test_StopRts, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_StopRts");
    UtTest_Add(SC_ProcessCommand_Test_DisableRts, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_DisableRts");
    UtTest_Add(SC_ProcessCommand_Test_EnableRts, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_EnableRts");
    UtTest_Add(SC_ProcessCommand_Test_SwitchAts, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_SwitchAts");
    UtTest_Add(SC_ProcessCommand_Test_JumpAts, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_JumpAts");
    UtTest_Add(SC_ProcessCommand_Test_ContinueAtsOnFailure, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_ContinueAtsOnFailure");
    UtTest_Add(SC_ProcessCommand_Test_AppendAts, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_AppendAts");

    UtTest_Add(SC_ProcessCommand_Test_TableManageAtsTableNominal, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageAtsTableNominal");
    UtTest_Add(SC_ProcessCommand_Test_TableManageAtsTableGetAddressError, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageAtsTableGetAddressError");
    UtTest_Add(SC_ProcessCommand_Test_TableManageAppendTableNominal, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageAppendTableNominal");
    UtTest_Add(SC_ProcessCommand_Test_TableManageAppendTableGetAddressError, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageAppendTableGetAddressError");
    UtTest_Add(SC_ProcessCommand_Test_TableManageRtsTableNominal, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageRtsTableNominal");
    UtTest_Add(SC_ProcessCommand_Test_TableManageRtsTableGetAddressError, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageRtsTableGetAddressError");
    UtTest_Add(SC_ProcessCommand_Test_TableManageRtsInfo, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageRtsInfo");
    UtTest_Add(SC_ProcessCommand_Test_TableManageRtpCtrl, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageRtpCtrl");
    UtTest_Add(SC_ProcessCommand_Test_TableManageAtsInfo, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageAtsInfo");
    UtTest_Add(SC_ProcessCommand_Test_TableManageAppendInfo, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageAppendInfo");
    UtTest_Add(SC_ProcessCommand_Test_TableManageAtpCtrl, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageAtpCtrl");
    UtTest_Add(SC_ProcessCommand_Test_TableManageAtsCmdStatus, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageAtsCmdStatus");
    UtTest_Add(SC_ProcessCommand_Test_TableManageInvalidTableID, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_TableManageInvalidTableID");

    UtTest_Add(SC_ProcessCommand_Test_StartRtsGrp, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_StartRtsGrp");
    UtTest_Add(SC_ProcessCommand_Test_StopRtsGrp, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_StopRtsGrp");
    UtTest_Add(SC_ProcessCommand_Test_DisableRtsGrp, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_DisableRtsGrp");
    UtTest_Add(SC_ProcessCommand_Test_EnableRtsGrp, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_EnableRtsGrp");
    UtTest_Add(SC_ProcessCommand_Test_InvalidCmdError, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessCommand_Test_InvalidCmdError");
    UtTest_Add(SC_VerifyCmdLength_Test_LenError, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyCmdLength_Test_LenError");
} /* end SC_Cmds_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
