 /*************************************************************************
 ** File:
 **   $Id: sc_loads_test.c 1.4.1.1 2016/10/21 17:37:27EDT sstrege Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file sc_atsrq.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_loads_test.c  $
 **   Revision 1.4.1.1 2016/10/21 17:37:27EDT sstrege 
 **   Applying DCR 145914 change packages
 **   Revision 1.5 2016/10/21 15:19:45EDT mdeschu 
 **   Updated gcov results
 **   Revision 1.4 2016/10/15 16:16:57EDT mdeschu 
 **   Add unit test to for inserting a command into middle of the list.
 **   Remove dependency in SC_ProcessAppend_Test on SC_StartATSCmd_Test_CouldNotStart.
 **   SC_StartATSCmd_Test_CouldNotStart overrided continue after zero return code of PSP_MemCpy 
 **   that persists between tests
 **   Revision 1.3 2016/09/09 19:25:55EDT czogby 
 **   Update based on code inspection log
 **   Revision 1.2 2016/02/04 16:03:12EST czogby 
 **   Add End-of-file and end-of-function comments
 **   Revision 1.1 2015/08/18 14:02:59EDT czogby 
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

uint8  SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount;
uint16 SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook1(CFE_SB_MsgPtr_t MsgPtr)
{
    SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount += 1;

    if (SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount <= 2)
        return SC_PACKET_MAX_SIZE;
    else
        return SC_ATS_BUFF_SIZE * 2;
} /* end SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook1 */

uint16 SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook2(CFE_SB_MsgPtr_t MsgPtr)
{
    SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount += 1;
    
    if (SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount == 189)
        return 120;
    else
        return SC_PACKET_MAX_SIZE;
} /* end SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook2 */

uint16 SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook3(CFE_SB_MsgPtr_t MsgPtr)
{
    SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount += 1;

    if (SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount == 189)
        return 122;
    else
        return SC_PACKET_MAX_SIZE;
} /* end SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook3 */

uint16 SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook4(CFE_SB_MsgPtr_t MsgPtr)
{
    SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount += 1;

    if (SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount == 32)
        return 58;
    else
        return SC_PACKET_MAX_SIZE;
} /* end SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook4 */

CFE_TIME_Compare_t UT_SC_Insert_CompareHookAgreaterthanB(CFE_TIME_SysTime_t TimeA, CFE_TIME_SysTime_t TimeB)
{
    return CFE_TIME_A_GT_B;
} /* end CFE_TIME_Compare_t UT_SC_StartAtsRq_CompareHookAgreaterthanB */

void SC_LoadAts_Test_Nominal(void)
{
    SC_NoArgsCmd_t   CmdPacket;
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];
    uint8  AtsIndex = 0;

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 1;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] = SC_EMPTY;

    /* Set to satisfy the conditions of if-statement below comment "if the length of the command is valid", along
     * with the if-statement immediately after */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_NoArgsCmd_t), 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    SC_LoadAts(AtsIndex);
    
    /* Verify results */
    UtAssert_True(SC_AppData.AtsCmdIndexBuffer[AtsIndex][0] == 0, "SC_AppData.AtsCmdIndexBuffer[AtsIndex][0] == 0");
    UtAssert_True(SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] == SC_LOADED, "SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] == SC_LOADED");
    UtAssert_True(SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands == 1, "SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands == 1");
    UtAssert_True(SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize == 7, "SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize == 7");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_LoadAts_Test_Nominal */

void SC_LoadAts_Test_CmdRunOffEndOfBuffer(void)
{
    SC_NoArgsCmd_t   CmdPacket;
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];
    uint8  AtsIndex = 0;

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 1;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] = SC_EMPTY;

    /* Causes CFE_SB_GetTotalMsgLength to satisfy the conditions of if-statement below comment "if the length of the command is valid", but
     * NOT the if-statement immediately after */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, &SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook1);
    SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount = 0;

    /* Execute the function being tested */
    SC_LoadAts(AtsIndex);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_LoadAts_Test_CmdRunOffEndOfBuffer */

void SC_LoadAts_Test_CmdLengthInvalid(void)
{
    SC_NoArgsCmd_t   CmdPacket;
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];
    uint8  AtsIndex = 0;

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 1;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] = SC_EMPTY;

    /* Set to make the if-statement below comment "if the length of the command is valid" fail */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MIN_SIZE*2, 2);

    /* Execute the function being tested */
    SC_LoadAts(AtsIndex);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_LoadAts_Test_CmdLengthInvalid */

void SC_LoadAts_Test_CmdNumberInvalid(void)
{
    SC_NoArgsCmd_t   CmdPacket;
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];
    uint8  AtsIndex = 0;

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = SC_MAX_ATS_CMDS*2;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    SC_LoadAts(AtsIndex);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_LoadAts_Test_CmdNumberInvalid */

void SC_LoadAts_Test_EndOfLoadReached(void)
{
    SC_NoArgsCmd_t   CmdPacket;
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];
    uint8  AtsIndex = 0;

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 0;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    SC_LoadAts(AtsIndex);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_LoadAts_Test_EndOfLoadReached */

void SC_LoadAts_Test_AtsBufferTooSmall(void)
{
    SC_NoArgsCmd_t   CmdPacket;
    /* Runs through the nominal case one time, at the end of which the condition is set for this case */

    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];
    uint32   AtsIndex = 0;
    uint32   i;

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];

    for (i=0; i < 65; i++)
    {
        Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][(SC_PACKET_MAX_SIZE/2 + 3)*i];
        Entry->CmdNumber = i+1;
    }

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    /* Set to reach block of code starting with comment "even the smallest command will not fit in the buffer" */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, &SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook2);
    SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount = 0;

    /* Execute the function being tested */
    SC_LoadAts(AtsIndex);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_LoadAts_Test_AtsBufferTooSmall */

void SC_LoadAts_Test_LoadExactlyBufferLength(void)
{
    SC_NoArgsCmd_t   CmdPacket;
    /* Runs through the nominal case one time, at the end of which the condition is set for this case */

    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];
    uint32   AtsIndex = 0;
    uint32   i;

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];

    for (i=0; i < 65; i++)
    {
        Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][(SC_PACKET_MAX_SIZE/2 + 3)*i];
        Entry->CmdNumber = i+1;
    }

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_NoArgsCmd_t), TRUE);

    /* Set to reach block of code starting with comment "we encountered a load exactly as long as the buffer" */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, &SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook3);
    SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount = 0;

    /* Execute the function being tested */
    SC_LoadAts(AtsIndex);
    
    /* Verify results */
    UtAssert_True (SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize == SC_ATS_BUFF_SIZE, "SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize == SC_ATS_BUFF_SIZE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_LoadAts_Test_LoadExactlyBufferLength */

void SC_Insert_Test(void)
{
    uint8 AtsIndex    = 0;
    uint8 ListLength  = 1;
    uint8 NewCmdIndex = 0;

    SC_InitTables();

    /* Execute the function being tested */
    SC_Insert(AtsIndex, NewCmdIndex, ListLength);
    
    /* Verify results */
    UtAssert_True (SC_AppData.AtsTimeIndexBuffer[AtsIndex][1] == SC_AppData.AtsTimeIndexBuffer[AtsIndex][0], "SC_AppData.AtsTimeIndexBuffer[AtsIndex][1] == SC_AppData.AtsTimeIndexBuffer[AtsIndex][0]");
    UtAssert_True (SC_AppData.AtsTimeIndexBuffer[AtsIndex][1] == NewCmdIndex, "SC_AppData.AtsTimeIndexBuffer[AtsIndex][1] == NewCmdIndex");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_Insert_Test */


void SC_Insert_Test_MiddleOfList(void)
{
    uint8 AtsIndex    = 0;
    uint8 ListLength  = 1;
    uint8 NewCmdIndex = 0;

    /* Set to cause SC_BeginAts to return FALSE, in order to reach block starting with "could not start the ats" */
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_COMPARE_INDEX, &UT_SC_Insert_CompareHookAgreaterthanB);

    SC_InitTables();

    /* Execute the function being tested */
    SC_Insert(AtsIndex, NewCmdIndex, ListLength);
    
    /* Verify results */
    UtAssert_True (SC_AppData.AtsTimeIndexBuffer[AtsIndex][1] == SC_AppData.AtsTimeIndexBuffer[AtsIndex][0], "SC_AppData.AtsTimeIndexBuffer[AtsIndex][1] == SC_AppData.AtsTimeIndexBuffer[AtsIndex][0]");
    UtAssert_True (SC_AppData.AtsTimeIndexBuffer[AtsIndex][1] == NewCmdIndex, "SC_AppData.AtsTimeIndexBuffer[AtsIndex][1] == NewCmdIndex");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_Insert_Test_MiddleOfList */

void SC_ValidateAts_Test(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 0;

    /* Execute the function being tested */
    Result = SC_ValidateAts((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]));
    
    /* Verify results */
    UtAssert_True (Result == SC_ERROR, "Result == SC_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ValidateAts_Test */

void SC_ValidateAppend_Test(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 0;

    /* Execute the function being tested */
    Result = SC_ValidateAppend((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]));
    
    /* Verify results */
    UtAssert_True (Result == SC_ERROR, "Result == SC_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ValidateAppend_Test */

void SC_ValidateRts_Test(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint8 RtsIndex = 0;
    int16 Result;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];
    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 1;

    /* Execute the function being tested */
    Result = SC_ValidateRts((uint16 *)(SC_OperData.RtsTblAddr[RtsIndex]));
    
    /* Verify results */
    UtAssert_True (Result == SC_ERROR, "Result == SC_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ValidateRts_Test */

void SC_ParseRts_Test_EndOfFile(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint8 RtsIndex = 0;
    int16 Result;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];
    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 0;

    /* Set these to satisfy if-statement to reach line with comment "assumed end of file" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 0, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETMSGID_INDEX);

    /* Execute the function being tested */
    Result = SC_ParseRts(SC_OperData.RtsTblAddr[RtsIndex]);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_ParseRts_Test_EndOfFile */

void SC_ParseRts_Test_InvalidMsgId(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint8 RtsIndex = 0;
    int16 Result;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];
    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 1;

    /* Set to generate error message SC_RTS_INVLD_MID_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 0, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETMSGID_INDEX);

    /* Execute the function being tested */
    Result = SC_ParseRts(SC_OperData.RtsTblAddr[RtsIndex]);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_INVLD_MID_ERR_EID, CFE_EVS_ERROR, "RTS cmd loaded with invalid MID at 0"),
         "RTS cmd loaded with invalid MID at 0");

    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ParseRts_Test_InvalidMsgId */

void SC_ParseRts_Test_LengthErrorTooShort(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint8 RtsIndex = 0;
    int16 Result;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];
    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 1;

    /* Set to reach code block that starts with comment "check to see if the length field in the RTS is valid" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETMSGID_INDEX);

    /* Set to generate error message SC_RTS_LEN_ERR_EID as a result of length being too short */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 0, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = SC_ParseRts(SC_OperData.RtsTblAddr[RtsIndex]);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_LEN_ERR_EID, CFE_EVS_ERROR, "RTS cmd loaded with invalid length at 0, len: 0"),
         "RTS cmd loaded with invalid length at 0, len: 0");

    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ParseRts_Test_LengthErrorTooShort */

void SC_ParseRts_Test_LengthErrorTooLong(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint8 RtsIndex = 0;
    int16 Result;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];
    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 1;

    /* Set to reach code block that starts with comment "check to see if the length field in the RTS is valid" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETMSGID_INDEX);

    /* Set to generate error message SC_RTS_LEN_ERR_EID as a result of length being too long */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE + 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = SC_ParseRts(SC_OperData.RtsTblAddr[RtsIndex]);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_LEN_ERR_EID, CFE_EVS_ERROR, "RTS cmd loaded with invalid length at 0, len: 251"),
         "RTS cmd loaded with invalid length at 0, len: 251");

    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ParseRts_Test_LengthErrorTooLong */

void SC_ParseRts_Test_CmdRunsOffEndOfBuffer(void)
{
    SC_RtsEntryHeader_t*   Entry;
    uint8 RtsIndex = 0;
    int16 Result;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];
    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 1;

    /* Set to reach code block that starts with comment "check to see if the length field in the RTS is valid" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETMSGID_INDEX);

    /* Set to generate error message SC_RTS_LEN_BUFFER_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = SC_ParseRts(SC_OperData.RtsTblAddr[RtsIndex]);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_LEN_BUFFER_ERR_EID, CFE_EVS_ERROR, "RTS cmd at 126 runs off end of buffer"),
         "RTS cmd at 126 runs off end of buffer");

    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ParseRts_Test_CmdRunsOffEndOfBuffer */

void SC_ParseRts_Test_CmdLengthEqualsBufferLength(void)
{
    /* Also tests the case where CmdLength is less than the buffer length */

    SC_RtsEntryHeader_t*   Entry;
    uint8 RtsIndex = 0;
    int16 Result;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];
    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 1;

    /* Set to reach code block that starts with comment "check to see if the length field in the RTS is valid" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETMSGID_INDEX);

    /* Set to satisfy condition "((i + ((RtsCmdSize + 1) / SC_BYTES_IN_WORD)) == SC_RTS_BUFF_SIZE)" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 148, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = SC_ParseRts(SC_OperData.RtsTblAddr[RtsIndex]);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_ParseRts_Test_CmdLengthEqualsBufferLength */

void SC_ParseRts_Test_CmdDoesNotFitBufferEmpty(void)
{
    SC_RtsEntryHeader_t*   Entry1;
    SC_RtsEntryHeader_t*   Entry2;
    uint8 RtsIndex = 0;
    int16 Result;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];

    Entry1 = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry1->TimeTag = 1;

    Entry2 = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][146];
    Entry2->TimeTag = 0;

    /* Set to reach code block that starts with comment "check to see if the length field in the RTS is valid" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETMSGID_INDEX);

    /* Set to reach "Done = True" immediately after comment "command does not fit in the buffer" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 144, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = SC_ParseRts(SC_OperData.RtsTblAddr[RtsIndex]);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_ParseRts_Test_CmdDoesNotFitBufferEmpty */

void SC_ParseRts_Test_CmdDoesNotFitBufferNotEmpty(void)
{
    SC_RtsEntryHeader_t*   Entry1;
    SC_RtsEntryHeader_t*   Entry2;
    uint8 RtsIndex = 0;
    int16 Result;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];

    Entry1 = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry1->TimeTag = 1;

    Entry2 = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][146];
    Entry2->TimeTag = 1;

    /* Set to reach code block that starts with comment "check to see if the length field in the RTS is valid" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETMSGID_INDEX);

    /* Set to generate error message SC_RTS_LEN_TOO_LONG_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 144, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = SC_ParseRts(SC_OperData.RtsTblAddr[RtsIndex]);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_LEN_TOO_LONG_ERR_EID, CFE_EVS_ERROR, "RTS cmd loaded won't fit in buffer at 146"),
         "RTS cmd loaded won't fit in buffer at 146");

    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ParseRts_Test_CmdDoesNotFitBufferNotEmpty */

void SC_UpdateAppend_Test_Nominal(void)
{
    /* Also tests the case where CmdLength is less than the buffer length */

    SC_AtsEntryHeader_t*   Entry;
    uint8 EntryIndex = 0;
    uint16 AtsAppendTable[SC_APPEND_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AppendTblAddr = (uint16*)&AtsAppendTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AppendTblAddr[EntryIndex];
    Entry->CmdNumber = 1;

    /* Set to reach code block starting with comment "Compute buffer index for next Append ATS table entry" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 50, 1);

    /* Execute the function being tested */
    SC_UpdateAppend();
    
    /* Verify results */
    UtAssert_True (SC_AppData.AppendLoadCount == 1, "SC_AppData.AppendLoadCount == 1");
    UtAssert_True (SC_AppData.AppendEntryCount == 1, "SC_AppData.AppendEntryCount == 1");
    UtAssert_True (SC_AppData.AppendWordCount == 28, "SC_AppData.AppendWordCount == 28");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_UPDATE_APPEND_EID, CFE_EVS_INFORMATION, "Update Append ATS Table: load count = 1, command count = 1, byte count = 56"),
         "Update Append ATS Table: load count = 1, command count = 1, byte count = 56");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_UpdateAppend_Test_Nominal */

void SC_UpdateAppend_Test_CmdDoesNotFitBuffer(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsAppendTable[SC_APPEND_BUFF_SIZE];
    uint32 EntryIndex = 0;

    SC_InitTables();

    SC_OperData.AppendTblAddr = (uint16*)&AtsAppendTable[0];
    
    for (EntryIndex = 0; EntryIndex <= 35; EntryIndex++)
    {
        Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AppendTblAddr[EntryIndex*128];
        Entry->CmdNumber = EntryIndex+1;
    }

    /* Set to reach code block starting with comment "Entry command packet must fit within ATS append table buffer" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    SC_UpdateAppend();
    
    /* Verify results */
    UtAssert_True (SC_AppData.AppendLoadCount == 1, "SC_AppData.AppendLoadCount == 1");
    UtAssert_True (SC_AppData.AppendEntryCount == 31, "SC_AppData.AppendEntryCount == 31");
    UtAssert_True (SC_AppData.AppendWordCount == 3968, "SC_AppData.AppendWordCount == 3968");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_UPDATE_APPEND_EID, CFE_EVS_INFORMATION, "Update Append ATS Table: load count = 1, command count = 31, byte count = 7936"),
         "Update Append ATS Table: load count = 1, command count = 31, byte count = 7936");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_UpdateAppend_Test_CmdDoesNotFitBuffer */

void SC_UpdateAppend_Test_InvalidCmdLengthTooLow(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsAppendTable[SC_APPEND_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AppendTblAddr = (uint16*)&AtsAppendTable[0];
    
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AppendTblAddr[0];
    Entry->CmdNumber = 1;

    /* Set to satisfy condition "(CommandBytes < SC_PACKET_MIN_SIZE)" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MIN_SIZE - 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    SC_UpdateAppend();
    
    /* Verify results */
    UtAssert_True (SC_AppData.AppendLoadCount == 1, "SC_AppData.AppendLoadCount == 1");
    UtAssert_True (SC_AppData.AppendEntryCount == 0, "SC_AppData.AppendEntryCount == 0");
    UtAssert_True (SC_AppData.AppendWordCount == 0, "SC_AppData.AppendWordCount == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_UPDATE_APPEND_EID, CFE_EVS_INFORMATION, "Update Append ATS Table: load count = 1, command count = 0, byte count = 0"),
         "Update Append ATS Table: load count = 1, command count = 0, byte count = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_UpdateAppend_Test_InvalidCmdLengthTooLow */

void SC_UpdateAppend_Test_InvalidCmdLengthTooHigh(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsAppendTable[SC_APPEND_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AppendTblAddr = (uint16*)&AtsAppendTable[0];
    
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AppendTblAddr[0];
    Entry->CmdNumber = 1;

    /* Set to satisfy condition "(CommandBytes > SC_PACKET_MAX_SIZE)" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE*2, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    SC_UpdateAppend();
    
    /* Verify results */
    UtAssert_True (SC_AppData.AppendLoadCount == 1, "SC_AppData.AppendLoadCount == 1");
    UtAssert_True (SC_AppData.AppendEntryCount == 0, "SC_AppData.AppendEntryCount == 0");
    UtAssert_True (SC_AppData.AppendWordCount == 0, "SC_AppData.AppendWordCount == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_UPDATE_APPEND_EID, CFE_EVS_INFORMATION, "Update Append ATS Table: load count = 1, command count = 0, byte count = 0"),
         "Update Append ATS Table: load count = 1, command count = 0, byte count = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_UpdateAppend_Test_InvalidCmdLengthTooHigh */

void SC_UpdateAppend_Test_EndOfBuffer(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint16 AtsAppendTable[SC_APPEND_BUFF_SIZE];
    uint32 EntryIndex = 0;

    SC_InitTables();

    SC_OperData.AppendTblAddr = (uint16*)&AtsAppendTable[0];
    
    for (EntryIndex = 0; EntryIndex <= 35; EntryIndex++)
    {
        Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AppendTblAddr[EntryIndex*128];
        Entry->CmdNumber = EntryIndex+1;
    }

    /* Cause condition to be met: "(EntryIndex >= SC_APPEND_BUFF_SIZE)" */
    Ut_CFE_SB_SetFunctionHook(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, &SC_LOADS_TEST_CFE_SB_GetTotalMsgLengthHook4);
    SC_LOADS_TEST_GetTotalMsgLengthHook_RunCount = 0;

    /* Execute the function being tested */
    SC_UpdateAppend();
    
    /* Verify results */
    UtAssert_True (SC_AppData.AppendLoadCount == 1, "SC_AppData.AppendLoadCount == 1");
    UtAssert_True (SC_AppData.AppendEntryCount == 32, "SC_AppData.AppendEntryCount == 32");
    UtAssert_True (SC_AppData.AppendWordCount == 4000, "SC_AppData.AppendWordCount == 4000");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_UPDATE_APPEND_EID, CFE_EVS_INFORMATION, "Update Append ATS Table: load count = 1, command count = 32, byte count = 8000"),
         "Update Append ATS Table: load count = 1, command count = 32, byte count = 8000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_UpdateAppend_Test_EndOfBuffer */

void SC_UpdateAppend_Test_CmdNumberZero(void)
{
    /* Also tests the case where CmdLength is less than the buffer length */

    SC_AtsEntryHeader_t*   Entry;
    uint8 EntryIndex = 0;
    uint16 AtsAppendTable[SC_APPEND_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AppendTblAddr = (uint16*)&AtsAppendTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AppendTblAddr[EntryIndex];
    Entry->CmdNumber = 0;

    /* Cause condition to be met: "(Entry->CmdNumber == 0)" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 50, 1);

    /* Execute the function being tested */
    SC_UpdateAppend();
    
    /* Verify results */
    UtAssert_True (SC_AppData.AppendLoadCount == 1, "SC_AppData.AppendLoadCount == 1");
    UtAssert_True (SC_AppData.AppendEntryCount == 0, "SC_AppData.AppendEntryCount == 0");
    UtAssert_True (SC_AppData.AppendWordCount == 0, "SC_AppData.AppendWordCount == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_UPDATE_APPEND_EID, CFE_EVS_INFORMATION, "Update Append ATS Table: load count = 1, command count = 0, byte count = 0"),
         "Update Append ATS Table: load count = 1, command count = 0, byte count = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_UpdateAppend_Test_CmdNumberZero */

void SC_UpdateAppend_Test_CmdNumberTooHigh(void)
{
    /* Also tests the case where CmdLength is less than the buffer length */

    SC_AtsEntryHeader_t*   Entry;
    uint8 EntryIndex = 0;
    uint16 AtsAppendTable[SC_APPEND_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AppendTblAddr = (uint16*)&AtsAppendTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AppendTblAddr[EntryIndex];
    Entry->CmdNumber = SC_MAX_ATS_CMDS + 1;

    /* Cause condition to be met: "(Entry->CmdNumber > SC_MAX_ATS_CMDS)" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 50, 1);

    /* Execute the function being tested */
    SC_UpdateAppend();
    
    /* Verify results */
    UtAssert_True (SC_AppData.AppendLoadCount == 1, "SC_AppData.AppendLoadCount == 1");
    UtAssert_True (SC_AppData.AppendEntryCount == 0, "SC_AppData.AppendEntryCount == 0");
    UtAssert_True (SC_AppData.AppendWordCount == 0, "SC_AppData.AppendWordCount == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_UPDATE_APPEND_EID, CFE_EVS_INFORMATION, "Update Append ATS Table: load count = 1, command count = 0, byte count = 0"),
         "Update Append ATS Table: load count = 1, command count = 0, byte count = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_UpdateAppend_Test_CmdNumberTooHigh */

void SC_ProcessAppend_Test(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    /* Setting memcpy to return success, otherwise the test will cause a seg fault */
    Ut_CFE_PSP_MEMUTILS_SetReturnCode(UT_CFE_PSP_MEMUTILS_MEMCPY_INDEX, CFE_PSP_SUCCESS, 1);

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 1;

    SC_AppData.AppendWordCount = 1;
    SC_AppData.AppendEntryCount = 1;

    SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] = SC_EMPTY;
    SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING;
    SC_OperData.AtsCtrlBlckAddr->AtsNumber = 1;

    /* Execute the function being tested */
    SC_ProcessAppend(AtsIndex);
    
    /* Verify results */
    UtAssert_True (SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize == 1, "SC_OperData.AtsInfoTblAddr[AtsIndex].AtsSize == 1");
    UtAssert_True (SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands == 1, "SC_OperData.AtsInfoTblAddr[AtsIndex].NumberOfCommands == 1");
    UtAssert_True (SC_AppData.AtsCmdIndexBuffer[AtsIndex][0] == 0, "SC_AppData.AtsCmdIndexBuffer[AtsIndex][0] == 0");
    UtAssert_True (SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] == SC_LOADED, "SC_OperData.AtsCmdStatusTblAddr[AtsIndex][0] == SC_LOADED");
    UtAssert_True (SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING, "SC_OperData.AtsCtrlBlckAddr->AtpState = SC_EXECUTING");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_ProcessAppend_Test */

void SC_VerifyAtsTable_Test_Nominal(void)
{
    SC_AtsEntryHeader_t*   Entry1;
    SC_AtsEntryHeader_t*   Entry2;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];

    Entry1 = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry1->CmdNumber = 1;

    Entry2 = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][128];
    Entry2->CmdNumber = 0;

    /* Set to call to SC_VerifyAtsEntry will return SC_PACKET_MAX_SIZE, which will cause execution of
     * code block starting with comment "Result is size (in words) of this entry" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = SC_VerifyAtsTable((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]), SC_ATS_BUFF_SIZE);
    
    /* Verify results */

    /* Test element 1.  Note: element 0 is modified in call to SC_VerifyAtsEntry and so it does not need to be verified in this test */
    UtAssert_True (SC_OperData.AtsDupTestArray[1] == SC_DUP_TEST_UNUSED, "SC_OperData.AtsDupTestArray[1] == SC_DUP_TEST_UNUSED");

    /* Test middle element.  Note: element 0 is modified in call to SC_VerifyAtsEntry */
    UtAssert_True (SC_OperData.AtsDupTestArray[SC_MAX_ATS_CMDS / 2] == SC_DUP_TEST_UNUSED, "SC_OperData.AtsDupTestArray[SC_MAX_ATS_CMDS / 2] == SC_DUP_TEST_UNUSED");

    /* Test last element */
    UtAssert_True (SC_OperData.AtsDupTestArray[SC_MAX_ATS_CMDS - 1] == SC_DUP_TEST_UNUSED, "SC_OperData.AtsDupTestArray[SC_MAX_ATS_CMDS - 1] == SC_DUP_TEST_UNUSED");

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_VERIFY_ATS_EID, CFE_EVS_INFORMATION, "Verify ATS Table: command count = 1, byte count = 256"),
         "Verify ATS Table: command count = 1, byte count = 256");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_VerifyAtsTable_Test_Nominal */

void SC_VerifyAtsTable_Test_InvalidEntry(void)
{
    SC_AtsEntryHeader_t*   Entry1;
    SC_AtsEntryHeader_t*   Entry2;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];

    Entry1 = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry1->CmdNumber = 5000;

    Entry2 = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][128];
    Entry2->CmdNumber = 0;

    /* Execute the function being tested */
    Result = SC_VerifyAtsTable((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]), SC_ATS_BUFF_SIZE);
    
    /* Verify results */
    UtAssert_True (Result == SC_ERROR, "Result == SC_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_VerifyAtsTable_Test_InvalidEntry */

void SC_VerifyAtsTable_Test_EmptyTable(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 0;

    /* Execute the function being tested */
    Result = SC_VerifyAtsTable((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]), SC_ATS_BUFF_SIZE);
    
    /* Verify results */
    UtAssert_True (Result == SC_ERROR, "Result == SC_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_VERIFY_ATS_MPT_ERR_EID, CFE_EVS_ERROR, "Verify ATS Table error: table is empty"),
         "Verify ATS Table error: table is empty");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_VerifyAtsTable_Test_EmptyTable */

void SC_VerifyAtsEntry_Test_Nominal(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 1;

    SC_OperData.AtsDupTestArray[0] = SC_DUP_TEST_UNUSED;

    /* Set to reach code block starting with comment "Compute length (in words) for this ATS table entry" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = SC_VerifyAtsEntry((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]), AtsIndex, SC_ATS_BUFF_SIZE);
    
    /* Verify results */
    UtAssert_True (Result == SC_ATS_HDR_NOPKT_WORDS + (SC_PACKET_MAX_SIZE + 1) / 2, "Result == SC_ATS_HDR_NOPKT_WORDS + (SC_PACKET_MAX_SIZE + 1) / 2");
    UtAssert_True (SC_OperData.AtsDupTestArray[0] == 0, "SC_OperData.AtsDupTestArray[0] == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_VerifyAtsEntry_Test_Nominal */

void SC_VerifyAtsEntry_Test_EndOfBuffer(void)
{
    uint16 AtsIndex = 10000;
    int16 Result;

    SC_InitTables();

    /* Execute the function being tested */
    Result = SC_VerifyAtsEntry((uint16 *)(SC_OperData.AtsTblAddr[0]), AtsIndex, SC_ATS_BUFF_SIZE);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_VerifyAtsEntry_Test_EndOfBuffer */

void SC_VerifyAtsEntry_Test_InvalidCmdNumber(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 5000;

    /* Execute the function being tested */
    Result = SC_VerifyAtsEntry((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]), AtsIndex, SC_ATS_BUFF_SIZE);
    
    /* Verify results */
    UtAssert_True (Result == SC_ERROR, "Result == SC_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_VERIFY_ATS_NUM_ERR_EID, CFE_EVS_ERROR, "Verify ATS Table error: invalid command number: buf index = 0, cmd num = 5000"),
         "Verify ATS Table error: invalid command number: buf index = 0, cmd num = 5000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_VerifyAtsEntry_Test_InvalidCmdNumber */

void SC_VerifyAtsEntry_Test_BufferFull(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 1;

    /* Execute the function being tested */
    Result = SC_VerifyAtsEntry((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]), AtsIndex, 2);
    
    /* Verify results */
    UtAssert_True (Result == SC_ERROR, "Result == SC_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_VERIFY_ATS_END_ERR_EID, CFE_EVS_ERROR, "Verify ATS Table error: buffer full: buf index = 0, cmd num = 1, buf words = 2"),
         "Verify ATS Table error: buffer full: buf index = 0, cmd num = 1, buf words = 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_VerifyAtsEntry_Test_BufferFull */

void SC_VerifyAtsEntry_Test_InvalidCmdLengthTooLow(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 1;

    /* Set to generate error message SC_VERIFY_ATS_PKT_ERR_EID by satisfying condition "(CommandBytes < SC_PACKET_MIN_SIZE)" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MIN_SIZE - 1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = SC_VerifyAtsEntry((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]), AtsIndex, SC_ATS_BUFF_SIZE);
    
    /* Verify results */
    UtAssert_True (Result == SC_ERROR, "Result == SC_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_VERIFY_ATS_PKT_ERR_EID, CFE_EVS_ERROR, "Verify ATS Table error: invalid length: buf index = 0, cmd num = 1, pkt len = 7"),
         "Verify ATS Table error: invalid length: buf index = 0, cmd num = 1, pkt len = 7");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_VerifyAtsEntry_Test_InvalidCmdLengthTooLow */

void SC_VerifyAtsEntry_Test_InvalidCmdLengthTooHigh(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 1;

    /* Set to generate error message SC_VERIFY_ATS_PKT_ERR_EID by satisfying condition "(CommandBytes > SC_PACKET_MAX_SIZE)" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE*2, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = SC_VerifyAtsEntry((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]), AtsIndex, SC_ATS_BUFF_SIZE);
    
    /* Verify results */
    UtAssert_True (Result == SC_ERROR, "Result == SC_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_VERIFY_ATS_PKT_ERR_EID, CFE_EVS_ERROR, "Verify ATS Table error: invalid length: buf index = 0, cmd num = 1, pkt len = 500"),
         "Verify ATS Table error: invalid length: buf index = 0, cmd num = 1, pkt len = 500");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_VerifyAtsEntry_Test_InvalidCmdLengthTooHigh */

void SC_VerifyAtsEntry_Test_BufferOverflow(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 1;

    /* Set to generate error message SC_VERIFY_ATS_BUF_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    Result = SC_VerifyAtsEntry((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]), AtsIndex, 20);
    
    /* Verify results */
    UtAssert_True (Result == SC_ERROR, "Result == SC_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_VERIFY_ATS_BUF_ERR_EID, CFE_EVS_ERROR, "Verify ATS Table error: buffer overflow: buf index = 0, cmd num = 1, pkt len = 250"),
         "Verify ATS Table error: buffer overflow: buf index = 0, cmd num = 1, pkt len = 250");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_VerifyAtsEntry_Test_BufferOverflow */

void SC_VerifyAtsEntry_Test_DuplicateCmdNumber(void)
{
    SC_AtsEntryHeader_t*   Entry;
    uint8 AtsIndex = 0;
    int16 Result;
    uint16 AtsTable[SC_ATS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.AtsTblAddr[AtsIndex] = (uint16*)&AtsTable[0];
    Entry = (SC_AtsEntryHeader_t *) &SC_OperData.AtsTblAddr[AtsIndex][0];
    Entry->CmdNumber = 1;

    /* Set to generate error message SC_VERIFY_ATS_DUP_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, SC_PACKET_MAX_SIZE, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    SC_OperData.AtsDupTestArray[0] = 99;

    /* Execute the function being tested */
    Result = SC_VerifyAtsEntry((uint16 *)(SC_OperData.AtsTblAddr[AtsIndex]), AtsIndex, SC_ATS_BUFF_SIZE);
    
    /* Verify results */
    UtAssert_True (Result == SC_ERROR, "Result == SC_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_VERIFY_ATS_DUP_ERR_EID, CFE_EVS_ERROR, "Verify ATS Table error: dup cmd number: buf index = 0, cmd num = 1, dup index = 99"),
         "Verify ATS Table error: dup cmd number: buf index = 0, cmd num = 1, dup index = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_VerifyAtsEntry_Test_DuplicateCmdNumber */

/** 
 **  No tests needed for SC_BuildTimeIndexTable, SC_InitAtsTables, SC_LoadRts, SC_UpdateAppend_Test_EndOfValidCmdNumbers, 
 **  and SC_VerifyAtsEntry_Test_EndOfInUsePartOfBuffer - all lines run and tested in other tests.
*/

/* Lines 191 - 192 in SC_LoadAts are unreachable. */

void SC_Loads_Test_AddTestCases(void)
{
    UtTest_Add(SC_LoadAts_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_LoadAts_Test_Nominal");
    UtTest_Add(SC_LoadAts_Test_CmdRunOffEndOfBuffer, SC_Test_Setup, SC_Test_TearDown, "SC_LoadAts_Test_CmdRunOffEndOfBuffer");
    UtTest_Add(SC_LoadAts_Test_CmdLengthInvalid, SC_Test_Setup, SC_Test_TearDown, "SC_LoadAts_Test_CmdLengthInvalid");
    UtTest_Add(SC_LoadAts_Test_CmdNumberInvalid, SC_Test_Setup, SC_Test_TearDown, "SC_LoadAts_Test_CmdNumberInvalid");
    UtTest_Add(SC_LoadAts_Test_EndOfLoadReached, SC_Test_Setup, SC_Test_TearDown, "SC_LoadAts_Test_EndOfLoadReached");
    UtTest_Add(SC_LoadAts_Test_AtsBufferTooSmall, SC_Test_Setup, SC_Test_TearDown, "SC_LoadAts_Test_AtsBufferTooSmall");
    UtTest_Add(SC_LoadAts_Test_LoadExactlyBufferLength, SC_Test_Setup, SC_Test_TearDown, "SC_LoadAts_Test_LoadExactlyBufferLength");
    UtTest_Add(SC_Insert_Test, SC_Test_Setup, SC_Test_TearDown, "SC_Insert_Test");
    UtTest_Add(SC_Insert_Test_MiddleOfList, SC_Test_Setup, SC_Test_TearDown, "SC_Insert_Test_MiddleOfList");
    UtTest_Add(SC_ValidateAts_Test, SC_Test_Setup, SC_Test_TearDown, "SC_ValidateAts_Test");
    UtTest_Add(SC_ValidateAppend_Test, SC_Test_Setup, SC_Test_TearDown, "SC_ValidateAppend_Test");
    UtTest_Add(SC_ValidateRts_Test, SC_Test_Setup, SC_Test_TearDown, "SC_ValidateRts_Test");
    UtTest_Add(SC_ParseRts_Test_EndOfFile, SC_Test_Setup, SC_Test_TearDown, "SC_ParseRts_Test_EndOfFile");
    UtTest_Add(SC_ParseRts_Test_InvalidMsgId, SC_Test_Setup, SC_Test_TearDown, "SC_ParseRts_Test_InvalidMsgId");
    UtTest_Add(SC_ParseRts_Test_LengthErrorTooShort, SC_Test_Setup, SC_Test_TearDown, "SC_ParseRts_Test_LengthErrorTooShort");
    UtTest_Add(SC_ParseRts_Test_LengthErrorTooLong, SC_Test_Setup, SC_Test_TearDown, "SC_ParseRts_Test_LengthErrorTooLong");
    UtTest_Add(SC_ParseRts_Test_CmdRunsOffEndOfBuffer, SC_Test_Setup, SC_Test_TearDown, "SC_ParseRts_Test_CmdRunsOffEndOfBuffer");
    UtTest_Add(SC_ParseRts_Test_CmdLengthEqualsBufferLength, SC_Test_Setup, SC_Test_TearDown, "SC_ParseRts_Test_CmdLengthEqualsBufferLength");
    UtTest_Add(SC_ParseRts_Test_CmdDoesNotFitBufferEmpty, SC_Test_Setup, SC_Test_TearDown, "SC_ParseRts_Test_CmdDoesNotFitBufferEmpty");
    UtTest_Add(SC_ParseRts_Test_CmdDoesNotFitBufferNotEmpty, SC_Test_Setup, SC_Test_TearDown, "SC_ParseRts_Test_CmdDoesNotFitBufferNotEmpty");
    UtTest_Add(SC_UpdateAppend_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_UpdateAppend_Test_Nominal");
    UtTest_Add(SC_UpdateAppend_Test_CmdNumberZero, SC_Test_Setup, SC_Test_TearDown, "SC_UpdateAppend_Test_CmdNumberZero");
    UtTest_Add(SC_UpdateAppend_Test_CmdNumberTooHigh, SC_Test_Setup, SC_Test_TearDown, "SC_UpdateAppend_Test_CmdNumberTooHigh");
    UtTest_Add(SC_UpdateAppend_Test_CmdDoesNotFitBuffer, SC_Test_Setup, SC_Test_TearDown, "SC_UpdateAppend_Test_CmdDoesNotFitBuffer");
    UtTest_Add(SC_UpdateAppend_Test_EndOfBuffer, SC_Test_Setup, SC_Test_TearDown, "SC_UpdateAppend_Test_EndOfBuffer");
    UtTest_Add(SC_UpdateAppend_Test_InvalidCmdLengthTooLow, SC_Test_Setup, SC_Test_TearDown, "SC_UpdateAppend_Test_InvalidCmdLengthTooLow");
    UtTest_Add(SC_UpdateAppend_Test_InvalidCmdLengthTooHigh, SC_Test_Setup, SC_Test_TearDown, "SC_UpdateAppend_Test_InvalidCmdLengthTooHigh");
    UtTest_Add(SC_ProcessAppend_Test, SC_Test_Setup, SC_Test_TearDown, "SC_ProcessAppend_Test");
    UtTest_Add(SC_VerifyAtsTable_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyAtsTable_Test_Nominal");
    UtTest_Add(SC_VerifyAtsTable_Test_InvalidEntry, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyAtsTable_Test_InvalidEntry");
    UtTest_Add(SC_VerifyAtsTable_Test_EmptyTable, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyAtsTable_Test_EmptyTable");
    UtTest_Add(SC_VerifyAtsEntry_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyAtsEntry_Test_Nominal");
    UtTest_Add(SC_VerifyAtsEntry_Test_EndOfBuffer, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyAtsEntry_Test_EndOfBuffer");
    UtTest_Add(SC_VerifyAtsEntry_Test_InvalidCmdNumber, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyAtsEntry_Test_InvalidCmdNumber");
    UtTest_Add(SC_VerifyAtsEntry_Test_BufferFull, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyAtsEntry_Test_BufferFull");
    UtTest_Add(SC_VerifyAtsEntry_Test_InvalidCmdLengthTooLow, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyAtsEntry_Test_InvalidCmdLengthTooLow");
    UtTest_Add(SC_VerifyAtsEntry_Test_InvalidCmdLengthTooHigh, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyAtsEntry_Test_InvalidCmdLengthTooHigh");
    UtTest_Add(SC_VerifyAtsEntry_Test_BufferOverflow, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyAtsEntry_Test_BufferOverflow");
    UtTest_Add(SC_VerifyAtsEntry_Test_DuplicateCmdNumber, SC_Test_Setup, SC_Test_TearDown, "SC_VerifyAtsEntry_Test_DuplicateCmdNumber");
} /* end SC_Loads_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
