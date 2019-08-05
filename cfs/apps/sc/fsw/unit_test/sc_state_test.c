 /*************************************************************************
 ** File:
 **   $Id: sc_state_test.c 1.3 2016/09/09 19:25:48EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file sc_state.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_state_test.c  $
 **   Revision 1.3 2016/09/09 19:25:48EDT czogby 
 **   Update based on code inspection log
 **   Revision 1.2 2016/02/04 16:03:11EST czogby 
 **   Add End-of-file and end-of-function comments
 **   Revision 1.1 2015/08/18 14:03:01EDT czogby 
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
uint8  SC_STATE_TEST_GetTotalMsgLengthHook_RunCount;
uint16 SC_STATE_TEST_CFE_SB_GetTotalMsgLengthHook(CFE_SB_MsgPtr_t MsgPtr)
{
    SC_STATE_TEST_GetTotalMsgLengthHook_RunCount += 1;

    if (SC_STATE_TEST_GetTotalMsgLengthHook_RunCount == 1)
        return SC_PACKET_MAX_SIZE;
    else
        return SC_PACKET_MAX_SIZE + 100;
} /* end SC_STATE_TEST_CFE_SB_GetTotalMsgLengthHook */

void SC_GetNextRtsTime_Test_Nominal(void)
{    
    SC_InitTables();

    SC_OperData.RtsInfoTblAddr[0].RtsStatus = SC_EXECUTING;
    SC_OperData.RtsInfoTblAddr[0].NextCommandTime = SC_MAX_TIME;

    /* Execute the function being tested */
    SC_GetNextRtsTime();

    /* Verify results */
    UtAssert_True(SC_OperData.RtsCtrlBlckAddr->RtsNumber == 1, "SC_OperData.RtsCtrlBlckAddr->RtsNumber == 1");
    UtAssert_True(SC_AppData.NextCmdTime[1] == SC_MAX_TIME, "SC_AppData.NextCmdTime[1] == SC_MAX_TIME");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_GetNextRtsTime_Test_Nominal */

void SC_GetNextRtsTime_Test_InvalidRtsNumber(void)
{
    uint8  i;

    SC_InitTables();

    for (i = 0; i < SC_NUMBER_OF_RTS; i++)
    {
        SC_OperData.RtsInfoTblAddr[i].RtsStatus = -1;
    }

    /* Execute the function being tested */
    SC_GetNextRtsTime();
    
    /* Verify results */
    UtAssert_True(SC_OperData.RtsCtrlBlckAddr->RtsNumber == SC_INVALID_RTS_NUMBER, "SC_OperData.RtsCtrlBlckAddr->RtsNumber == SC_INVALID_RTS_NUMBER");
    UtAssert_True(SC_AppData.NextCmdTime[SC_RTP] == SC_MAX_TIME, "SC_AppData.NextCmdTime[SC_RTP] == SC_MAX_TIME");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_GetNextRtsTime_Test_InvalidRtsNumber */

void SC_UpdateNextTime_TestAtp(void)
{
    SC_InitTables();

    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;

    /* Execute the function being tested */
    SC_UpdateNextTime();
    
    /* Verify results */
    UtAssert_True(SC_AppData.NextProcNumber == SC_ATP, "SC_AppData.NextProcNumber == SC_ATP");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_UpdateNextTime_TestAtp */

void SC_UpdateNextTime_TestRtp(void)
{
    SC_InitTables();

    SC_OperData.RtsCtrlBlckAddr->RtsNumber = 10;
    SC_AppData.NextCmdTime[SC_RTP] = 0;
    SC_AppData.NextCmdTime[SC_ATP] = 10;

    SC_OperData.RtsInfoTblAddr[0].RtsStatus = SC_EXECUTING;
    SC_OperData.RtsInfoTblAddr[0].NextCommandTime = 1;

    /* Execute the function being tested */
    SC_UpdateNextTime();
    
    /* Verify results */
    UtAssert_True(SC_AppData.NextProcNumber == SC_RTP, "SC_AppData.NextProcNumber == SC_RTP");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_UpdateNextTime_TestRtp */

void SC_GetNextRtsCommand_Test_GetNextCommand(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[0] = (uint16*)&RtsTable[0];
    
    SC_AppData.NextCmdTime[SC_RTP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_RTP;
    SC_OperData.RtsCtrlBlckAddr->RtsNumber  = 1;
    SC_OperData.RtsInfoTblAddr[SC_OperData.RtsCtrlBlckAddr->RtsNumber - 1].RtsStatus = SC_EXECUTING;
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[0][0];

    CFE_SB_InitMsg (&Entry->CmdHeader, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    /* Set so checksum will pass in SC_ProcessRtpCommand */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);

    /* Give the packet the minimum possible size, to ensure that (CmdOffset <  (SC_RTS_BUFF_SIZE - (sizeof(SC_RtsEntryHeader_t) / SC_BYTES_IN_WORD) is met */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MIN_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    /* NOTE: Calling SC_ProcessRtpCommand instead of SC_GetNextRtsCommand - SC_ProcessRtpCommand calls SC_GetNextRtsCommand, and it's much to test easier this way. */
    SC_ProcessRtpCommand();

    /* Verify results */
    UtAssert_True
        (SC_OperData.RtsInfoTblAddr[0].NextCommandPtr == (SC_PACKET_MIN_SIZE + SC_RTS_HEADER_SIZE + 1) / SC_BYTES_IN_WORD,
         "SC_OperData.RtsInfoTblAddr[0].NextCommandPtr == (SC_PACKET_MIN_SIZE + SC_RTS_HEADER_SIZE + 1) / SC_BYTES_IN_WORD");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_GetNextRtsCommand_Test_GetNextCommand */

void SC_GetNextRtsCommand_Test_RtsLengthError(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[0] = (uint16*)&RtsTable[0];
    
    SC_AppData.NextCmdTime[SC_RTP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_RTP;
    SC_OperData.RtsCtrlBlckAddr->RtsNumber  = 1;
    SC_OperData.RtsInfoTblAddr[SC_OperData.RtsCtrlBlckAddr->RtsNumber - 1].RtsStatus = SC_EXECUTING;
 
    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[0][0];

    CFE_SB_InitMsg (&Entry->CmdHeader, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    /* Set so checksum will pass in SC_ProcessRtpCommand */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);
    
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* Set to generate error message SC_RTS_LNGTH_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    SC_OperData.RtsInfoTblAddr[0].NextCommandPtr = (SC_RTS_BUFF_SIZE - (sizeof(SC_RtsEntryHeader_t) / SC_BYTES_IN_WORD)) - ((SC_PACKET_MAX_SIZE + SC_RTS_HEADER_SIZE + 1) / SC_BYTES_IN_WORD) - 1;

    /* Execute the function being tested */
    /* NOTE: Calling SC_ProcessRtpCommand instead of SC_GetNextRtsCommand - SC_ProcessRtpCommand calls SC_GetNextRtsCommand, and it's much easier to test this way. */
    SC_ProcessRtpCommand();

    /* Verify results */
    UtAssert_True (SC_AppData.RtsCmdErrCtr == 1, "SC_AppData.RtsCmdErrCtr == 1");
    UtAssert_True (SC_OperData.RtsInfoTblAddr[0].CmdErrCtr == 1, "SC_OperData.RtsInfoTblAddr[0].CmdErrCtr == 1");
    UtAssert_True (SC_AppData.LastRtsErrSeq == SC_OperData.RtsCtrlBlckAddr->RtsNumber, "SC_AppData.LastRtsErrSeq == SC_OperData.RtsCtrlBlckAddr->RtsNumber");

    UtAssert_True
        (SC_AppData.LastRtsErrCmd == SC_OperData.RtsInfoTblAddr[0].NextCommandPtr + ((SC_PACKET_MAX_SIZE + SC_RTS_HEADER_SIZE + 1) / SC_BYTES_IN_WORD),
        "SC_AppData.LastRtsErrCmd == SC_OperData.RtsInfoTblAddr[0].NextCommandPtr + ((SC_PACKET_MAX_SIZE + SC_RTS_HEADER_SIZE + 1) / SC_BYTES_IN_WORD)");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_LNGTH_ERR_EID, CFE_EVS_ERROR, "Cmd Runs passed end of table, RTS 001 Aborted"),
         "Cmd Runs passed end of table, RTS 001 Aborted");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_GetNextRtsCommand_Test_RtsLengthError */

void SC_GetNextRtsCommand_Test_CommandLengthError(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[0] = (uint16*)&RtsTable[0];
    
    SC_AppData.NextCmdTime[SC_RTP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_RTP;
    SC_OperData.RtsCtrlBlckAddr->RtsNumber  = 1;
    SC_OperData.RtsInfoTblAddr[SC_OperData.RtsCtrlBlckAddr->RtsNumber - 1].RtsStatus = SC_EXECUTING;

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[0][0];

    CFE_SB_InitMsg (&Entry->CmdHeader, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    /* Set to make checksum pass in SC_ProcessRtpCommand, so that SC_GetNextRtsCommand is reached */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SENDMSG_INDEX, CFE_SUCCESS, 1);
    
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* First call to CFE_SB_GetTotalMsgLength returns value to make subsequent if-statement pass,
     * next call returns value to generate error message SC_RTS_LNGTH_ERR_EID */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, &SC_STATE_TEST_CFE_SB_GetTotalMsgLengthHook);
    SC_STATE_TEST_GetTotalMsgLengthHook_RunCount = 0;

    SC_OperData.RtsInfoTblAddr[0].NextCommandPtr = (SC_RTS_BUFF_SIZE - (sizeof(SC_RtsEntryHeader_t) / SC_BYTES_IN_WORD)) - ((SC_PACKET_MAX_SIZE + SC_RTS_HEADER_SIZE + 1) / SC_BYTES_IN_WORD) - 1;

    /* Execute the function being tested */
    /* NOTE: Calling SC_ProcessRtpCommand instead of SC_GetNextRtsCommand - SC_ProcessRtpCommand calls SC_GetNextRtsCommand, and it's much easier to test this way. */
    SC_ProcessRtpCommand();

    /* Verify results */
    UtAssert_True (SC_AppData.RtsCmdErrCtr == 1, "SC_AppData.RtsCmdErrCtr == 1");
    UtAssert_True (SC_OperData.RtsInfoTblAddr[0].CmdErrCtr == 1, "SC_OperData.RtsInfoTblAddr[0].CmdErrCtr == 1");
    UtAssert_True (SC_AppData.LastRtsErrSeq == SC_OperData.RtsCtrlBlckAddr -> RtsNumber, "SC_AppData.LastRtsErrSeq == SC_OperData.RtsCtrlBlckAddr -> RtsNumber");

    UtAssert_True
        (SC_AppData.LastRtsErrCmd == SC_OperData.RtsInfoTblAddr[0].NextCommandPtr + ((SC_PACKET_MAX_SIZE + SC_RTS_HEADER_SIZE + 1) / SC_BYTES_IN_WORD),
        "SC_AppData.LastRtsErrCmd == SC_OperData.RtsInfoTblAddr[0].NextCommandPtr + ((SC_PACKET_MAX_SIZE + SC_RTS_HEADER_SIZE + 1) / SC_BYTES_IN_WORD)");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_CMD_LNGTH_ERR_EID, CFE_EVS_ERROR, "Invalid Length Field in RTS Command, RTS 001 Aborted. Length: 350, Max: 250"),
         "Invalid Length Field in RTS Command, RTS 001 Aborted. Length: 350, Max: 250");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_GetNextRtsCommand_Test_CommandLengthError */

void SC_GetNextRtsCommand_Test_ZeroCommandLength(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[0] = (uint16*)&RtsTable[0];
    
    SC_AppData.NextCmdTime[SC_RTP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_RTP;
    SC_OperData.RtsCtrlBlckAddr->RtsNumber  = 1;
    SC_OperData.RtsInfoTblAddr[SC_OperData.RtsCtrlBlckAddr->RtsNumber - 1].RtsStatus = SC_EXECUTING;

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[0][0];

    CFE_SB_InitMsg (&Entry->CmdHeader, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    /* Set to make checksum pass in SC_ProcessRtpCommand, so that SC_GetNextRtsCommand is reached */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SENDMSG_INDEX, CFE_SUCCESS, 1);
    
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* Set to generate error message SC_RTS_COMPL_INF_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MIN_SIZE - 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    SC_OperData.RtsInfoTblAddr[0].NextCommandPtr = (SC_RTS_BUFF_SIZE - (sizeof(SC_RtsEntryHeader_t) / SC_BYTES_IN_WORD)) - ((SC_PACKET_MAX_SIZE + SC_RTS_HEADER_SIZE + 1) / SC_BYTES_IN_WORD) - 1;

    /* Execute the function being tested */
    /* NOTE: Calling SC_ProcessRtpCommand instead of SC_GetNextRtsCommand - SC_ProcessRtpCommand calls SC_GetNextRtsCommand, and it's much easier to test this way. */
    SC_ProcessRtpCommand();

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_COMPL_INF_EID, CFE_EVS_INFORMATION, "RTS 001 Execution Completed"),
         "RTS 001 Execution Completed");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_GetNextRtsCommand_Test_ZeroCommandLength */

void SC_GetNextRtsCommand_Test_EndOfBuffer(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[0] = (uint16*)&RtsTable[0];
    
    SC_AppData.NextCmdTime[SC_RTP]          = 0;
    SC_AppData.CurrentTime                  = 1;
    SC_AppData.NextProcNumber               = SC_RTP;
    SC_OperData.RtsCtrlBlckAddr->RtsNumber  = 1;
    SC_OperData.RtsInfoTblAddr[SC_OperData.RtsCtrlBlckAddr->RtsNumber - 1].RtsStatus = SC_EXECUTING;
 
    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[0][0];

    CFE_SB_InitMsg (&Entry->CmdHeader, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    /* Set to make checksum pass in SC_ProcessRtpCommand, so that SC_GetNextRtsCommand is reached */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_VALIDATECHECKSUM_INDEX, TRUE, 1);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SENDMSG_INDEX, CFE_SUCCESS, 1);
    
    SC_OperData.AtsInfoTblAddr[1].NumberOfCommands = 1;

    /* Set to generate error message SC_RTS_COMPL_INF_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 2 * SC_RTS_BUFF_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    SC_OperData.RtsInfoTblAddr[0].NextCommandPtr = (SC_RTS_BUFF_SIZE - (sizeof(SC_RtsEntryHeader_t) / SC_BYTES_IN_WORD)) - ((SC_PACKET_MAX_SIZE + SC_RTS_HEADER_SIZE + 1) / SC_BYTES_IN_WORD) - 1;

    /* Execute the function being tested */
    /* NOTE: Calling SC_ProcessRtpCommand instead of SC_GetNextRtsCommand - SC_ProcessRtpCommand calls SC_GetNextRtsCommand, and it's much easier to test this way. */
    SC_ProcessRtpCommand();

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_COMPL_INF_EID, CFE_EVS_INFORMATION, "RTS 001 Execution Completed"),
         "RTS 001 Execution Completed");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_GetNextRtsCommand_Test_EndOfBuffer */

void SC_GetNextAtsCommand_Test_Starting(void)
{    
    SC_InitTables();
    
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_STARTING;

    /* Execute the function being tested */
    /* NOTE: Calling SC_ProcessRtpCommand instead of SC_GetNextRtsCommand - SC_ProcessRtpCommand calls SC_GetNextRtsCommand, and it's much easier to test this way. */
    SC_GetNextAtsCommand();

    /* Verify results */
    UtAssert_True (SC_OperData.AtsCtrlBlckAddr -> AtpState == SC_EXECUTING, "SC_OperData.AtsCtrlBlckAddr -> AtpState == SC_EXECUTING");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_GetNextAtsCommand_Test_Starting */

void SC_GetNextAtsCommand_Test_GetNextCommand(void)
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

    SC_OperData.AtsInfoTblAddr[SC_ATP].NumberOfCommands = 100;

    /* Execute the function being tested */
    SC_GetNextAtsCommand();
    
    /* Verify results */
    UtAssert_True (SC_OperData.AtsCtrlBlckAddr->TimeIndexPtr == 1, "SC_OperData.AtsCtrlBlckAddr->TimeIndexPtr == 1");
    UtAssert_True (SC_OperData.AtsCtrlBlckAddr->CmdNumber == SC_AppData.AtsTimeIndexBuffer[0][1], "SC_OperData.AtsCtrlBlckAddr->CmdNumber == SC_AppData.AtsTimeIndexBuffer[0][1];");
    UtAssert_True (SC_AppData.NextCmdTime[SC_ATP] == 0, "SC_AppData.NextCmdTime[SC_ATP] == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_GetNextAtsCommand_Test_GetNextCommand */

void SC_GetNextAtsCommand_Test_ExecutionCompleted(void)
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

    SC_OperData.AtsInfoTblAddr[SC_ATP].NumberOfCommands = 0;

    /* Execute the function being tested */
    SC_GetNextAtsCommand();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ATS_COMPL_INF_EID, CFE_EVS_INFORMATION, "ATS A Execution Completed"),
         "ATS A Execution Completed");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_GetNextAtsCommand_Test_ExecutionCompleted */

void SC_State_Test_AddTestCases(void)
{
    UtTest_Add(SC_GetNextRtsTime_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_GetNextRtsTime_Test_Nominal");
    UtTest_Add(SC_GetNextRtsTime_Test_InvalidRtsNumber, SC_Test_Setup, SC_Test_TearDown, "SC_GetNextRtsTime_Test_InvalidRtsNumber");
    UtTest_Add(SC_UpdateNextTime_TestAtp, SC_Test_Setup, SC_Test_TearDown, "SC_UpdateNextTime_TestAtp");
    UtTest_Add(SC_UpdateNextTime_TestRtp, SC_Test_Setup, SC_Test_TearDown, "SC_UpdateNextTime_TestRtp");

    UtTest_Add(SC_GetNextRtsCommand_Test_GetNextCommand, SC_Test_Setup, SC_Test_TearDown, "SC_GetNextRtsCommand_Test_GetNextCommand");
    UtTest_Add(SC_GetNextRtsCommand_Test_RtsLengthError, SC_Test_Setup, SC_Test_TearDown, "SC_GetNextRtsCommand_Test_RtsLengthError");
    UtTest_Add(SC_GetNextRtsCommand_Test_CommandLengthError, SC_Test_Setup, SC_Test_TearDown, "SC_GetNextRtsCommand_Test_CommandLengthError");
    UtTest_Add(SC_GetNextRtsCommand_Test_ZeroCommandLength, SC_Test_Setup, SC_Test_TearDown, "SC_GetNextRtsCommand_Test_ZeroCommandLength");
    UtTest_Add(SC_GetNextRtsCommand_Test_EndOfBuffer, SC_Test_Setup, SC_Test_TearDown, "SC_GetNextRtsCommand_Test_EndOfBuffer");

    UtTest_Add(SC_GetNextAtsCommand_Test_Starting, SC_Test_Setup, SC_Test_TearDown, "SC_GetNextAtsCommand_Test_Starting");
    UtTest_Add(SC_GetNextAtsCommand_Test_GetNextCommand, SC_Test_Setup, SC_Test_TearDown, "SC_GetNextAtsCommand_Test_GetNextCommand");
    UtTest_Add(SC_GetNextAtsCommand_Test_ExecutionCompleted, SC_Test_Setup, SC_Test_TearDown, "SC_GetNextAtsCommand_Test_ExecutionCompleted");
} /* end SC_State_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
