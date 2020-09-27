 /*************************************************************************
 ** File:
 **   $Id: lc_watch_test.c 1.5 2017/01/22 17:24:55EST sstrege Exp  $
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
 **   This file contains unit test cases for the functions contained in the file lc_watch.c
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

#include "lc_watch_test.h"
#include "lc_watch.h"
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

void LC_CreateHashTable_Test_UnsubscribeError(void)
{
    LC_OperData.MessageIDsCount           = 1;
    LC_OperData.MessageLinks[0].MessageID = 1;

    /* Set to generate error message LC_UNSUB_WP_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_UNSUBSCRIBE_INDEX, -1, 1);

    /* Execute the function being tested */
    LC_CreateHashTable();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_UNSUB_WP_ERR_EID, CFE_EVS_ERROR, "Error unsubscribing watchpoint: MID=0x0001, RC=0xFFFFFFFF"),
        "Error unsubscribing watchpoint: MID=0x0001, RC=0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CreateHashTable_Test_UnsubscribeError */

void LC_CreateHashTable_Test_NominalAllSameMID(void)
{
    int32               WatchPtTblIndex;

    LC_OperData.MessageIDsCount           = 1;
    LC_OperData.WatchpointCount           = 1;

    for (WatchPtTblIndex = 0; WatchPtTblIndex < LC_MAX_WATCHPOINTS; WatchPtTblIndex++)
    {
        LC_OperData.WDTPtr[WatchPtTblIndex].DataType = 99;
        LC_OperData.MessageLinks[WatchPtTblIndex].MessageID = 1;
    }

    /* Execute the function being tested */
    LC_CreateHashTable();
    
    /* Verify results */
    /* Nothing to verify for LC_OperData.WatchPtLinks, because it depends on LC_AddWatchpoint, which we're not testing here */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_CreateHashTable_Test_NominalAllSameMID */

void LC_AddWatchpoint_Test_HashTableAndWatchPtListNullPointersNominal(void)
{
    LC_WatchPtList_t   *Result;
    int32               HashTableIndex = 1;
    CFE_SB_MsgId_t      MessageID = 1;

    LC_OperData.HashTable[HashTableIndex]   = 0;
    LC_OperData.MessageLinks[0].WatchPtList = 0;

    /* Execute the function being tested */
    Result = (LC_WatchPtList_t  *)LC_AddWatchpoint(MessageID);
    
    /* Verify results */
    UtAssert_True (LC_OperData.HashTable[HashTableIndex] == &LC_OperData.MessageLinks[0], "LC_OperData.HashTable[HashTableIndex] == &LC_OperData.MessageLinks[0]");
    UtAssert_True (LC_OperData.MessageLinks[0].MessageID == 1, "LC_OperData.MessageLinks[0].MessageID == 1");
    UtAssert_True (LC_OperData.MessageLinks[0].WatchPtList == &LC_OperData.WatchPtLinks[0], "LC_OperData.MessageLinks[0].WatchPtList == &LC_OperData.WatchPtLinks[0]");
    UtAssert_True (Result == &LC_OperData.WatchPtLinks[0], "Result == &LC_OperData.WatchPtLinks[0]");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_AddWatchpoint_Test_HashTableAndWatchPtListNullPointersNominal */

void LC_AddWatchpoint_Test_HashTableAndWatchPtListNotNullPointerTwoMsgLinksMIDFound(void)
{
    LC_WatchPtList_t   *Result;
    int32               HashTableIndex = 0;
    CFE_SB_MsgId_t      MessageID = 5;

    LC_OperData.MessageLinks[0].MessageID = 5;
    LC_OperData.MessageLinks[1].MessageID = 0;

    LC_OperData.MessageLinks[0].WatchPtList->Next = &LC_OperData.WatchPtLinks[1];
    LC_OperData.HashTable[HashTableIndex]->Next   = &LC_OperData.MessageLinks[1];

    LC_OperData.MessageLinks[1].WatchPtList->Next = &LC_OperData.WatchPtLinks[2];

    /* Execute the function being tested */
    Result = (LC_WatchPtList_t  *)LC_AddWatchpoint(MessageID);
    
    /* Verify results */
    UtAssert_True (Result == &LC_OperData.WatchPtLinks[0], "Result == &LC_OperData.WatchPtLinks[0]");
    /* Not verifying line "WatchPtLink->Next = &LC_OperData.WatchPtLinks[LC_OperData.WatchpointCount++]" */
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_AddWatchpoint_Test_HashTableAndWatchPtListNotNullPointerTwoMsgLinksMIDFound */

void LC_AddWatchpoint_Test_NullPointersErrorSubscribingWatchpoint(void)
{
    LC_WatchPtList_t   *Result;
    int32               HashTableIndex = 1;
    CFE_SB_MsgId_t      MessageID = 1;

    LC_OperData.HashTable[HashTableIndex]   = 0;
    LC_OperData.MessageLinks[0].WatchPtList = 0;

    /* Set to generate error message LC_SUB_WP_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = (LC_WatchPtList_t  *)LC_AddWatchpoint(MessageID);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_SUB_WP_ERR_EID, CFE_EVS_ERROR, "Error subscribing watchpoint: MID=0x0001, RC=0xFFFFFFFF"),
        "Error subscribing watchpoint: MID=0x0001, RC=0xFFFFFFFF");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_AddWatchpoint_Test_NullPointersErrorSubscribingWatchpoint */

void LC_CheckMsgForWPs_Test_Nominal(void)
{
    LC_NoArgsCmd_t      CmdPacket;
    uint16              WatchIndex = 0;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_AppData.CurrentLCState = 99;
    LC_OperData.HashTable[LC_GetHashTableIndex(1)]->MessageID = 99;
    LC_OperData.HashTable[LC_GetHashTableIndex(1)]->Next = LC_OperData.HashTable[LC_GetHashTableIndex(2)];
    LC_OperData.HashTable[LC_GetHashTableIndex(1)]->Next->MessageID = 1;

    LC_OperData.HashTable[LC_GetHashTableIndex(1)]->Next->WatchPtList->WatchIndex = WatchIndex;
    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_BYTE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = -1;

    LC_OperData.HashTable[LC_GetHashTableIndex(1)]->Next->Next = LC_OperData.HashTable[LC_GetHashTableIndex(3)];
    LC_OperData.HashTable[LC_GetHashTableIndex(1)]->Next->Next->WatchPtList->WatchIndex = 99;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_CUSTOM;

    /* Execute the function being tested */
    LC_CheckMsgForWPs(1, (CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (LC_AppData.MonitoredMsgCount == 1, "LC_AppData.MonitoredMsgCount == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_CheckMsgForWPs_Test_Nominal */

void LC_CheckMsgForWPs_Test_UnreferencedMessageID(void)
{
    LC_NoArgsCmd_t      CmdPacket;

    LC_OperData.HashTable[LC_GetHashTableIndex(1)]  = (LC_MessageList_t *) NULL;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    LC_CheckMsgForWPs(1, (CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_MID_INF_EID, CFE_EVS_INFORMATION, "Msg with unreferenced message ID rcvd: ID = 0x0001"),
        "Msg with unreferenced message ID rcvd: ID = 0x0001");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_CheckMsgForWPs_Test_UnreferencedMessageID */

void LC_ProcessWP_Test_CustomFunctionWatchFalse(void)
{
    uint16              WatchIndex = 0;
    LC_NoArgsCmd_t      CmdPacket;
    CFE_TIME_SysTime_t  Timestamp;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_OperData.WDTPtr[WatchIndex].DataType            = LC_DATA_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID          = LC_OPER_CUSTOM;
    LC_OperData.WDTPtr[WatchIndex].CustomFuncArgument  = 0;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale  = 1;

    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, (CFE_SB_MsgPtr_t)(&CmdPacket), Timestamp);
    
    /* Verify results */
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0, "LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1, "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1");

    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE, "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1, "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");

    /* Note: this event message is generated in a subfunction, but is tested anyway to verify that the correct code path has been reached */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_CFCALL_ERR_EID, CFE_EVS_ERROR, "Unexpected LC_CustomFunction call: WP = 0"),
        "Unexpected LC_CustomFunction call: WP = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ProcessWP_Test_CustomFunctionWatchFalse */

void LC_ProcessWP_Test_OperatorCompareError(void)
{
    uint16              WatchIndex = 0;
    LC_NoArgsCmd_t      CmdPacket;
    CFE_TIME_SysTime_t  Timestamp;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_OperData.WDTPtr[WatchIndex].DataType    = LC_DATA_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID  = 99;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;
    
    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, (CFE_SB_MsgPtr_t)(&CmdPacket), Timestamp);
    
    /* Verify results */
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR, "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1, "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0, "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
    /* Generates 1 event message we don't care about in this test */

} /* end LC_ProcessWP_Test_OperatorCompareError */

void LC_ProcessWP_Test_OperatorCompareWatchTruePreviousStale(void)
{
    uint16              WatchIndex = 0;
    LC_NoArgsCmd_t      CmdPacket;
    CFE_TIME_SysTime_t  Timestamp;

    Timestamp.Seconds = 3;
    Timestamp.Subseconds = 5;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_OperData.WDTPtr[WatchIndex].DataType            = LC_DATA_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID          = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult         = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale  = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask             = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed8     = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;
    
    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, (CFE_SB_MsgPtr_t)(&CmdPacket), Timestamp);
    
    /* Verify results */
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_TRUE, "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_TRUE");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1, "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");

    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].CumulativeTrueCount == 1, "LC_OperData.WRTPtr[WatchIndex].CumulativeTrueCount == 1");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 1, "LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 1");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1, "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1");

    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].FalseToTrueCount == 1, "LC_OperData.WRTPtr[WatchIndex].FalseToTrueCount == 1");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Value == 0, "LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Value == 0");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Seconds == 3, "LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Seconds == 3");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Subseconds == 5, "LC_OperData.WRTPtr[WatchIndex].LastFalseToTrue.Timestamp.Subseconds == 5");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ProcessWP_Test_OperatorCompareWatchTruePreviousStale */

void LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousStale(void)
{
    uint16              WatchIndex = 0;
    LC_NoArgsCmd_t      CmdPacket;
    CFE_TIME_SysTime_t  Timestamp;

    Timestamp.Seconds = 3;
    Timestamp.Subseconds = 5;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_OperData.WDTPtr[WatchIndex].DataType            = LC_DATA_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID          = LC_OPER_GE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult         = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale  = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask             = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed8     = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;
    
    /* Execute the function being tested */
    LC_ProcessWP(WatchIndex, (CFE_SB_MsgPtr_t)(&CmdPacket), Timestamp);
    
    /* Verify results */
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE, "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_FALSE");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1, "LC_OperData.WRTPtr[WatchIndex].EvaluationCount == 1");

    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0, "LC_OperData.WRTPtr[WatchIndex].ConsecutiveTrueCount == 0");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1, "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 1");

    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Value == 0, "LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Value == 0");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Seconds == 3, "LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Seconds == 3");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Subseconds == 5, "LC_OperData.WRTPtr[WatchIndex].LastTrueToFalse.Timestamp.Subseconds == 5");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousStale */

void LC_OperatorCompare_Test_DataByte(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    uint32   ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType            = LC_DATA_BYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID          = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult         = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale  = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask             = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed8     = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;
    
    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_OperatorCompare_Test_DataByte */

void LC_OperatorCompare_Test_DataWordLE(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    uint32   ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType            = LC_DATA_WORD_LE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID          = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult         = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale  = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask             = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed16     = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;
    
    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_OperatorCompare_Test_DataWordLE */

void LC_OperatorCompare_Test_DataDWordLE(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    uint32   ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType            = LC_DATA_DWORD_LE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID          = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult         = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale  = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask             = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Signed32     = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;
    
    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_OperatorCompare_Test_DataDWordLE */

void LC_OperatorCompare_Test_DataUByte(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    uint32   ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType            = LC_DATA_UBYTE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID          = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult         = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale  = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask             = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Unsigned8     = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;
    
    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_OperatorCompare_Test_DataUByte */

void LC_OperatorCompare_Test_DataUWordLE(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    uint32   ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType            = LC_DATA_UWORD_LE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID          = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult         = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale  = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask             = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Unsigned16     = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;
    
    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_OperatorCompare_Test_DataUWordLE */

void LC_OperatorCompare_Test_DataUDWordLE(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    uint32   ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType            = LC_DATA_UDWORD_LE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID          = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult         = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale  = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask             = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Unsigned32     = 1;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;
    
    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_OperatorCompare_Test_DataUDWordLE */

void LC_OperatorCompare_Test_DataFloatLE(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    uint32   ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType            = LC_DATA_FLOAT_LE;
    LC_OperData.WDTPtr[WatchIndex].OperatorID          = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult         = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale  = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask             = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Float32 = 1.0;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;
    
    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_OperatorCompare_Test_DataFloatLE */

void LC_OperatorCompare_Test_DataTypeError(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    uint32   ProcessedWPData = 0;

    LC_OperData.WDTPtr[WatchIndex].DataType            = 99;
    LC_OperData.WDTPtr[WatchIndex].OperatorID          = LC_OPER_LE;
    LC_OperData.WRTPtr[WatchIndex].WatchResult         = LC_WATCH_STALE;
    LC_OperData.WDTPtr[WatchIndex].ResultAgeWhenStale  = 1;
    LC_OperData.WDTPtr[WatchIndex].BitMask             = 0;
    LC_OperData.WDTPtr[WatchIndex].ComparisonValue.Float32 = 1.0;
    LC_OperData.WRTPtr[WatchIndex].EvaluationCount = 0;
    
    /* Execute the function being tested */
    Result = LC_OperatorCompare(WatchIndex, ProcessedWPData);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_ERROR, "Result == LC_WATCH_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WP_DATATYPE_ERR_EID, CFE_EVS_ERROR, "WP has undefined data type: WP = 0, DataType = 99"),
        "WP has undefined data type: WP = 0, DataType = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_OperatorCompare_Test_DataTypeError */

void LC_SignedCompare_Test_LE(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 0;
    int32    CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LE;
    
    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SignedCompare_Test_LE */

void LC_SignedCompare_Test_LT(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 0;
    int32    CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LT;
    
    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SignedCompare_Test_LT */

void LC_SignedCompare_Test_EQ(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 1;
    int32    CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_EQ;
    
    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SignedCompare_Test_EQ */

void LC_SignedCompare_Test_NE(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 0;
    int32    CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_NE;
    
    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SignedCompare_Test_NE */

void LC_SignedCompare_Test_GT(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 1;
    int32    CompareValue = 0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GT;
    
    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SignedCompare_Test_GT */

void LC_SignedCompare_Test_GE(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 1;
    int32    CompareValue = 0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GE;
    
    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_SignedCompare_Test_GE */

void LC_SignedCompare_Test_InvalidOperatorID(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 1;
    int32    CompareValue = 0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = 99;
    
    /* Execute the function being tested */
    Result = LC_SignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_ERROR, "Result == LC_WATCH_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WP_OPERID_ERR_EID, CFE_EVS_ERROR, "WP has invalid operator ID: WP = 0, OperID = 99"),
        "WP has invalid operator ID: WP = 0, OperID = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_SignedCompare_Test_InvalidOperatorID */

void LC_UnsignedCompare_Test_LE(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 0;
    int32    CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LE;
    
    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_UnsignedCompare_Test_LE */

void LC_UnsignedCompare_Test_LT(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 0;
    int32    CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LT;
    
    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_UnsignedCompare_Test_LT */

void LC_UnsignedCompare_Test_EQ(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 1;
    int32    CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_EQ;
    
    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_UnsignedCompare_Test_EQ */

void LC_UnsignedCompare_Test_NE(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 0;
    int32    CompareValue = 1;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_NE;
    
    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_UnsignedCompare_Test_NE */

void LC_UnsignedCompare_Test_GT(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 1;
    int32    CompareValue = 0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GT;
    
    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_UnsignedCompare_Test_GT */

void LC_UnsignedCompare_Test_GE(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 1;
    int32    CompareValue = 0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GE;
    
    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_UnsignedCompare_Test_GE */

void LC_UnsignedCompare_Test_InvalidOperatorID(void)
{
    uint8    Result;
    uint16   WatchIndex = 0;
    int32    WPValue = 1;
    int32    CompareValue = 0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = 99;
    
    /* Execute the function being tested */
    Result = LC_UnsignedCompare(WatchIndex, WPValue, CompareValue);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_ERROR, "Result == LC_WATCH_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WP_OPERID_ERR_EID, CFE_EVS_ERROR, "WP has invalid operator ID: WP = 0, OperID = 99"),
        "WP has invalid operator ID: WP = 0, OperID = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_UnsignedCompare_Test_InvalidOperatorID */

void LC_FloatCompare_Test_LE(void)
{
    uint8            Result;
    uint16           WatchIndex = 0;
    LC_MultiType_t   WPMultiType;
    LC_MultiType_t   CompareMultiType;

    WPMultiType.Float32      = 0.0;
    CompareMultiType.Float32 = 1.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LE;
    
    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, WPMultiType, CompareMultiType);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_FloatCompare_Test_LE */

void LC_FloatCompare_Test_LT(void)
{
    uint8            Result;
    uint16           WatchIndex = 0;
    LC_MultiType_t   WPMultiType;
    LC_MultiType_t   CompareMultiType;

    WPMultiType.Float32      = 0.0;
    CompareMultiType.Float32 = 1.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_LT;
    
    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, WPMultiType, CompareMultiType);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_FloatCompare_Test_LT */

void LC_FloatCompare_Test_EQ(void)
{
    uint8            Result;
    uint16           WatchIndex = 0;
    LC_MultiType_t   WPMultiType;
    LC_MultiType_t   CompareMultiType;

    WPMultiType.Float32      = 1.0;
    CompareMultiType.Float32 = 1.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_EQ;
    
    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, WPMultiType, CompareMultiType);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_FloatCompare_Test_EQ */

void LC_FloatCompare_Test_NE(void)
{
    uint8            Result;
    uint16           WatchIndex = 0;
    LC_MultiType_t   WPMultiType;
    LC_MultiType_t   CompareMultiType;

    WPMultiType.Float32      = 1.0;
    CompareMultiType.Float32 = 0.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_NE;
    
    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, WPMultiType, CompareMultiType);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_FloatCompare_Test_NE */

void LC_FloatCompare_Test_GT(void)
{
    uint8            Result;
    uint16           WatchIndex = 0;
    LC_MultiType_t   WPMultiType;
    LC_MultiType_t   CompareMultiType;

    WPMultiType.Float32      = 1.0;
    CompareMultiType.Float32 = 0.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GT;
    
    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, WPMultiType, CompareMultiType);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_FloatCompare_Test_GT */

void LC_FloatCompare_Test_GE(void)
{
    uint8            Result;
    uint16           WatchIndex = 0;
    LC_MultiType_t   WPMultiType;
    LC_MultiType_t   CompareMultiType;

    WPMultiType.Float32      = 1.0;
    CompareMultiType.Float32 = 0.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = LC_OPER_GE;
    
    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, WPMultiType, CompareMultiType);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_TRUE, "Result == LC_WATCH_TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_FloatCompare_Test_GE */

void LC_FloatCompare_Test_InvalidOperatorID(void)
{
    uint8            Result;
    uint16           WatchIndex = 0;
    LC_MultiType_t   WPMultiType;
    LC_MultiType_t   CompareMultiType;

    WPMultiType.Float32      = 1.0;
    CompareMultiType.Float32 = 0.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = 99;
    
    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, WPMultiType, CompareMultiType);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_ERROR, "Result == LC_WATCH_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WP_OPERID_ERR_EID, CFE_EVS_ERROR, "WP has invalid operator ID: WP = 0, OperID = 99"),
        "WP has invalid operator ID: WP = 0, OperID = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_FloatCompare_Test_InvalidOperatorID */

void LC_FloatCompare_Test_NaN(void)
{
    uint8            Result;
    uint16           WatchIndex = 0;
    LC_MultiType_t   WPMultiType;
    LC_MultiType_t   CompareMultiType;

    WPMultiType.Float32      = 1.0;
    WPMultiType.Unsigned32   = 0x7F8FFFFF;
    CompareMultiType.Float32 = 0.0;

    LC_OperData.WDTPtr[WatchIndex].OperatorID = 99;
    
    /* Execute the function being tested */
    Result = LC_FloatCompare(WatchIndex, WPMultiType, CompareMultiType);
    
    /* Verify results */
    UtAssert_True (Result == LC_WATCH_ERROR, "Result == LC_WATCH_ERROR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WP_NAN_ERR_EID, CFE_EVS_ERROR, "WP data value is a float NAN: WP = 0, Value = 0x7F8FFFFF"),
        "WP data value is a float NAN: WP = 0, Value = 0x7F8FFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_FloatCompare_Test_NaN */

void LC_WPOffsetValid_Test_DataUByte(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    LC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_UBYTE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = 0;
    
    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, (CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_WPOffsetValid_Test_DataUByte */

void LC_WPOffsetValid_Test_UWordLE(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    LC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_UWORD_LE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = 0;
    
    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, (CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_WPOffsetValid_Test_UWordLE */

void LC_WPOffsetValid_Test_UDWordLE(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    LC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_UDWORD_LE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = 0;
    
    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, (CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_WPOffsetValid_Test_UDWordLE */

void LC_WPOffsetValid_Test_FloatLE(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    LC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_FLOAT_LE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = 0;
    
    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, (CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_WPOffsetValid_Test_FloatLE */

void LC_WPOffsetValid_Test_DataTypeError(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    LC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_OperData.WDTPtr[WatchIndex].DataType = 99;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = 0;
    
    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, (CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WP_DATATYPE_ERR_EID, CFE_EVS_ERROR, "WP has undefined data type: WP = 0, DataType = 99"),
        "WP has undefined data type: WP = 0, DataType = 99");

    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR, "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0, "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_WPOffsetValid_Test_DataTypeError */

void LC_WPOffsetValid_Test_OffsetError(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    LC_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg ((CFE_SB_MsgPtr_t)(&CmdPacket), 1, sizeof(LC_NoArgsCmd_t), TRUE);

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_UBYTE;
    LC_OperData.WDTPtr[WatchIndex].WatchpointOffset = sizeof(LC_NoArgsCmd_t);
    
    /* Execute the function being tested */
    Result = LC_WPOffsetValid(WatchIndex, (CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WP_OFFSET_ERR_EID, CFE_EVS_ERROR, "WP offset error: MID = 1, WP = 0, Offset = 8, DataSize = 1, MsgLen = 8"),
        "WP offset error: MID = 1, WP = 0, Offset = 8, DataSize = 1, MsgLen = 8");

    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR, "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0, "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_WPOffsetValid_Test_OffsetError */

void LC_GetSizedWPData_Test_DataByte(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    uint8            WPData[4];
    uint32           SizedData;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_BYTE;
    
    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (SizedData == 1, "SizedData == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_GetSizedWPData_Test_DataByte */

void LC_GetSizedWPData_Test_DataUByte(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    uint8            WPData[4];
    uint32           SizedData;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_UBYTE;
    
    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (SizedData == 1, "SizedData == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_GetSizedWPData_Test_DataUByte */

void LC_GetSizedWPData_Test_DataWordBELittleEndian(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    uint8            WPData[4];
    uint32           SizedData;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_WORD_BE;
    
    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (SizedData == 0x0102, "SizedData == 0x0102");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_GetSizedWPData_Test_DataWordBELittleEndian */

void LC_GetSizedWPData_Test_DataWordLELittleEndian(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    uint8            WPData[4];
    uint32           SizedData;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_WORD_LE;
    
    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (SizedData == 0x0201, "SizedData == 0x0201");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_GetSizedWPData_Test_DataWordLELittleEndian */

void LC_GetSizedWPData_Test_DataUWordBELittleEndian(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    uint8            WPData[4];
    uint32           SizedData;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_UWORD_BE;
    
    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (SizedData == 0x0102, "SizedData == 0x0102");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_GetSizedWPData_Test_DataUWordBELittleEndian */

void LC_GetSizedWPData_Test_DataUWordLELittleEndian(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    uint8            WPData[4];
    uint32           SizedData;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_UWORD_LE;
    
    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (SizedData == 0x0201, "SizedData == 0x0201");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_GetSizedWPData_Test_DataUWordLELittleEndian */

void LC_GetSizedWPData_Test_DataFloatBELittleEndian(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    uint8            WPData[4];
    uint32           SizedData;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_FLOAT_BE;
    
    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (SizedData == 0x01020304, "SizedData == 0x01020304");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_GetSizedWPData_Test_DataFloatBELittleEndian */

void LC_GetSizedWPData_Test_DataFloatLELittleEndian(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    uint8            WPData[4];
    uint32           SizedData;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = LC_DATA_FLOAT_LE;
    
    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (SizedData == 0x04030201, "SizedData == 0x04030201");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_GetSizedWPData_Test_DataFloatLELittleEndian */

void LC_GetSizedWPData_Test_DataTypeError(void)
{
    boolean          Result;
    uint16           WatchIndex = 0;
    uint8            WPData[4];
    uint32           SizedData;

    WPData[0] = 1;
    WPData[1] = 2;
    WPData[2] = 3;
    WPData[3] = 4;

    LC_OperData.WDTPtr[WatchIndex].DataType = 99;
    
    /* Execute the function being tested */
    Result = LC_GetSizedWPData(WatchIndex, WPData, &SizedData);
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR, "LC_OperData.WRTPtr[WatchIndex].WatchResult == LC_WATCH_ERROR");
    UtAssert_True (LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0, "LC_OperData.WRTPtr[WatchIndex].CountdownToStale == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WP_DATATYPE_ERR_EID, CFE_EVS_ERROR, "WP has undefined data type: WP = 0, DataType = 99"),
        "WP has undefined data type: WP = 0, DataType = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_GetSizedWPData_Test_DataTypeError */

void LC_ValidateWDT_Test_UnusedTableEntry(void)
{
    int32   Result;
    int32   TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++ )
    {
        LC_OperData.WDTPtr[TableIndex].DataType = LC_WATCH_NOT_USED;
    }
    
    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_INF_EID, CFE_EVS_INFORMATION, "WDT verify results: good = 0, bad = 0, unused = 176"),
        "WDT verify results: good = 0, bad = 0, unused = 176");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ValidateWDT_Test_UnusedTableEntry */

void LC_ValidateWDT_Test_InvalidDataType(void)
{
    int32   Result;
    int32   TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++ )
    {
        LC_OperData.WDTPtr[TableIndex].DataType = 99;
        LC_OperData.WDTPtr[TableIndex].OperatorID = 1;
        LC_OperData.WDTPtr[TableIndex].MessageID = 2;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 3;
    }
    
    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_WDTVAL_ERR_DATATYPE, "Result == LC_WDTVAL_ERR_DATATYPE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_ERR_EID, CFE_EVS_ERROR, "WDT verify err: WP = 0, Err = 1, DType = 99, Oper = 1, MID = 2"),
        "WDT verify err: WP = 0, Err = 1, DType = 99, Oper = 1, MID = 2");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_INF_EID, CFE_EVS_INFORMATION, "WDT verify results: good = 0, bad = 176, unused = 0"),
        "WDT verify results: good = 0, bad = 176, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end LC_ValidateWDT_Test_InvalidDataType */

void LC_ValidateWDT_Test_InvalidOperator(void)
{
    int32   Result;
    int32   TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++ )
    {
        LC_OperData.WDTPtr[TableIndex].DataType = LC_DATA_BYTE;
        LC_OperData.WDTPtr[TableIndex].OperatorID = 99;
        LC_OperData.WDTPtr[TableIndex].MessageID = 2;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 3;
    }
    
    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_WDTVAL_ERR_OPER, "Result == LC_WDTVAL_ERR_OPER");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_ERR_EID, CFE_EVS_ERROR, "WDT verify err: WP = 0, Err = 2, DType = 1, Oper = 99, MID = 2"),
        "WDT verify err: WP = 0, Err = 2, DType = 1, Oper = 99, MID = 2");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_INF_EID, CFE_EVS_INFORMATION, "WDT verify results: good = 0, bad = 176, unused = 0"),
        "WDT verify results: good = 0, bad = 176, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end LC_ValidateWDT_Test_InvalidOperator */

void LC_ValidateWDT_Test_BadMessageID(void)
{
    int32   Result;
    int32   TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++ )
    {
        LC_OperData.WDTPtr[TableIndex].DataType = LC_DATA_BYTE;
        LC_OperData.WDTPtr[TableIndex].OperatorID = LC_OPER_LT;
        LC_OperData.WDTPtr[TableIndex].MessageID = CFE_SB_HIGHEST_VALID_MSGID + 1;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 3;
    }
    
    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_WDTVAL_ERR_MID, "Result == LC_WDTVAL_ERR_MID");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_ERR_EID, CFE_EVS_ERROR, "WDT verify err: WP = 0, Err = 3, DType = 1, Oper = 1, MID = 8192"),
        "WDT verify err: WP = 0, Err = 3, DType = 1, Oper = 1, MID = 8192");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_INF_EID, CFE_EVS_INFORMATION, "WDT verify results: good = 0, bad = 176, unused = 0"),
        "WDT verify results: good = 0, bad = 176, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end LC_ValidateWDT_Test_BadMessageID */

void LC_ValidateWDT_Test_NaN(void)
{
    int32   Result;
    int32   TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++ )
    {
        LC_OperData.WDTPtr[TableIndex].DataType = LC_DATA_FLOAT_LE;
        LC_OperData.WDTPtr[TableIndex].OperatorID = LC_OPER_LT;
        LC_OperData.WDTPtr[TableIndex].MessageID = CFE_SB_HIGHEST_VALID_MSGID;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 0x7F8FFFFF;
    }
    
    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_WDTVAL_ERR_FPNAN, "Result == LC_WDTVAL_ERR_FPNAN");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_FPERR_EID, CFE_EVS_ERROR, "WDT verify float err: WP = 0, Err = 4, ComparisonValue = 0x7F8FFFFF"),
        "WDT verify float err: WP = 0, Err = 4, ComparisonValue = 0x7F8FFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_INF_EID, CFE_EVS_INFORMATION, "WDT verify results: good = 0, bad = 176, unused = 0"),
        "WDT verify results: good = 0, bad = 176, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end LC_ValidateWDT_Test_NaN */

void LC_ValidateWDT_Test_Inf(void)
{
    int32   Result;
    int32   TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++ )
    {
        LC_OperData.WDTPtr[TableIndex].DataType = LC_DATA_FLOAT_LE;
        LC_OperData.WDTPtr[TableIndex].OperatorID = LC_OPER_LT;
        LC_OperData.WDTPtr[TableIndex].MessageID = CFE_SB_HIGHEST_VALID_MSGID;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 0x7F800000;
    }
    
    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_WDTVAL_ERR_FPINF, "Result == LC_WDTVAL_ERR_FPINF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_FPERR_EID, CFE_EVS_ERROR, "WDT verify float err: WP = 0, Err = 5, ComparisonValue = 0x7F800000"),
        "WDT verify float err: WP = 0, Err = 5, ComparisonValue = 0x7F800000");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_INF_EID, CFE_EVS_INFORMATION, "WDT verify results: good = 0, bad = 176, unused = 0"),
        "WDT verify results: good = 0, bad = 176, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end LC_ValidateWDT_Test_Inf */

void LC_ValidateWDT_Test_FloatingPointPassed(void)
{
    int32   Result;
    int32   TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++ )
    {
        LC_OperData.WDTPtr[TableIndex].DataType = LC_DATA_FLOAT_LE;
        LC_OperData.WDTPtr[TableIndex].OperatorID = LC_OPER_LT;
        LC_OperData.WDTPtr[TableIndex].MessageID = CFE_SB_HIGHEST_VALID_MSGID;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 1;
    }
    
    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_WDTVAL_NO_ERR, "Result == LC_WDTVAL_NO_ERR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_INF_EID, CFE_EVS_INFORMATION, "WDT verify results: good = 176, bad = 0, unused = 0"),
        "WDT verify results: good = 176, bad = 0, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ValidateWDT_Test_FloatingPointPassed */

void LC_ValidateWDT_Test_NonFloatingPointPassed(void)
{
    int32   Result;
    int32   TableIndex;

    for (TableIndex = 0; TableIndex < LC_MAX_WATCHPOINTS; TableIndex++ )
    {
        LC_OperData.WDTPtr[TableIndex].DataType = LC_DATA_BYTE;
        LC_OperData.WDTPtr[TableIndex].OperatorID = LC_OPER_LT;
        LC_OperData.WDTPtr[TableIndex].MessageID = CFE_SB_HIGHEST_VALID_MSGID;
        LC_OperData.WDTPtr[TableIndex].ComparisonValue.Unsigned32 = 1;
    }
    
    /* Execute the function being tested */
    Result = LC_ValidateWDT(LC_OperData.WDTPtr);
    
    /* Verify results */
    UtAssert_True (Result == LC_WDTVAL_NO_ERR, "Result == LC_WDTVAL_NO_ERR");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(LC_WDTVAL_INF_EID, CFE_EVS_INFORMATION, "WDT verify results: good = 176, bad = 0, unused = 0"),
        "WDT verify results: good = 176, bad = 0, unused = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end LC_ValidateWDT_Test_NonFloatingPointPassed */

void LC_Uint32IsNAN_Test_True(void)
{
    boolean   Result;
    
    /* Execute the function being tested */
    Result = LC_Uint32IsNAN(0x7F8FFFFF);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_Uint32IsNAN_Test_True */

void LC_Uint32IsNAN_Test_False(void)
{
    boolean   Result;
    
    /* Execute the function being tested */
    Result = LC_Uint32IsNAN(0x10000000);
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_Uint32IsNAN_Test_False */

void LC_Uint32IsInfinite_Test_True(void)
{
    boolean   Result;
    
    /* Execute the function being tested */
    Result = LC_Uint32IsInfinite(0x7F800000);
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_Uint32IsInfinite_Test_True */

void LC_Uint32IsInfinite_Test_False(void)
{
    boolean   Result;
    
    /* Execute the function being tested */
    Result = LC_Uint32IsInfinite(0x10000000);
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end LC_Uint32IsInfinite_Test_False */

void LC_Watch_Test_AddTestCases(void)
{
    UtTest_Add(LC_CreateHashTable_Test_UnsubscribeError, LC_Test_Setup, LC_Test_TearDown, "LC_CreateHashTable_Test_UnsubscribeError");
    UtTest_Add(LC_CreateHashTable_Test_NominalAllSameMID, LC_Test_Setup, LC_Test_TearDown, "LC_CreateHashTable_Test_NominalAllSameMID");

    UtTest_Add(LC_AddWatchpoint_Test_HashTableAndWatchPtListNullPointersNominal, LC_Test_Setup, LC_Test_TearDown, "LC_AddWatchpoint_Test_HashTableAndWatchPtListNullPointersNominal");
    UtTest_Add(LC_AddWatchpoint_Test_HashTableAndWatchPtListNotNullPointerTwoMsgLinksMIDFound, LC_Test_Setup, LC_Test_TearDown, "LC_AddWatchpoint_Test_HashTableAndWatchPtListNotNullPointerTwoMsgLinksMIDFound");
    UtTest_Add(LC_AddWatchpoint_Test_NullPointersErrorSubscribingWatchpoint, LC_Test_Setup, LC_Test_TearDown, "LC_AddWatchpoint_Test_NullPointersErrorSubscribingWatchpoint");

    UtTest_Add(LC_CheckMsgForWPs_Test_Nominal, LC_Test_Setup, LC_Test_TearDown, "LC_CheckMsgForWPs_Test_Nominal");
    UtTest_Add(LC_CheckMsgForWPs_Test_UnreferencedMessageID, LC_Test_Setup, LC_Test_TearDown, "LC_CheckMsgForWPs_Test_UnreferencedMessageID");

    UtTest_Add(LC_ProcessWP_Test_CustomFunctionWatchFalse, LC_Test_Setup, LC_Test_TearDown, "LC_ProcessWP_Test_CustomFunctionWatchFalse");
    UtTest_Add(LC_ProcessWP_Test_OperatorCompareError, LC_Test_Setup, LC_Test_TearDown, "LC_ProcessWP_Test_OperatorCompareError");
    UtTest_Add(LC_ProcessWP_Test_OperatorCompareWatchTruePreviousStale, LC_Test_Setup, LC_Test_TearDown, "LC_ProcessWP_Test_OperatorCompareWatchTruePreviousStale");
    UtTest_Add(LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousStale, LC_Test_Setup, LC_Test_TearDown, "LC_ProcessWP_Test_OperatorCompareWatchFalsePreviousStale");

    /* Note: Only testing little-ending (LE) cases.  Not testing big-endian (BE) cases. */
    UtTest_Add(LC_OperatorCompare_Test_DataByte, LC_Test_Setup, LC_Test_TearDown, "LC_OperatorCompare_Test_DataByte");
    UtTest_Add(LC_OperatorCompare_Test_DataWordLE, LC_Test_Setup, LC_Test_TearDown, "LC_OperatorCompare_Test_DataWordLE");
    UtTest_Add(LC_OperatorCompare_Test_DataDWordLE, LC_Test_Setup, LC_Test_TearDown, "LC_OperatorCompare_Test_DataDWordLE");
    UtTest_Add(LC_OperatorCompare_Test_DataUByte, LC_Test_Setup, LC_Test_TearDown, "LC_OperatorCompare_Test_DataUByte");
    UtTest_Add(LC_OperatorCompare_Test_DataUWordLE, LC_Test_Setup, LC_Test_TearDown, "LC_OperatorCompare_Test_DataUWordLE");
    UtTest_Add(LC_OperatorCompare_Test_DataUDWordLE, LC_Test_Setup, LC_Test_TearDown, "LC_OperatorCompare_Test_DataUDWordLE");
    UtTest_Add(LC_OperatorCompare_Test_DataFloatLE, LC_Test_Setup, LC_Test_TearDown, "LC_OperatorCompare_Test_DataFloatLE");
    UtTest_Add(LC_OperatorCompare_Test_DataTypeError, LC_Test_Setup, LC_Test_TearDown, "LC_OperatorCompare_Test_DataTypeError");

    UtTest_Add(LC_SignedCompare_Test_LE, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_LE");
    UtTest_Add(LC_SignedCompare_Test_LT, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_LT");
    UtTest_Add(LC_SignedCompare_Test_EQ, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_EQ");
    UtTest_Add(LC_SignedCompare_Test_NE, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_NE");
    UtTest_Add(LC_SignedCompare_Test_GT, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_GT");
    UtTest_Add(LC_SignedCompare_Test_GE, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_GE");
    UtTest_Add(LC_SignedCompare_Test_InvalidOperatorID, LC_Test_Setup, LC_Test_TearDown, "LC_SignedCompare_Test_InvalidOperatorID");

    UtTest_Add(LC_UnsignedCompare_Test_LE, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_LE");
    UtTest_Add(LC_UnsignedCompare_Test_LT, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_LT");
    UtTest_Add(LC_UnsignedCompare_Test_EQ, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_EQ");
    UtTest_Add(LC_UnsignedCompare_Test_NE, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_NE");
    UtTest_Add(LC_UnsignedCompare_Test_GT, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_GT");
    UtTest_Add(LC_UnsignedCompare_Test_GE, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_GE");
    UtTest_Add(LC_UnsignedCompare_Test_InvalidOperatorID, LC_Test_Setup, LC_Test_TearDown, "LC_UnsignedCompare_Test_InvalidOperatorID");

    UtTest_Add(LC_FloatCompare_Test_LE, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_LE");
    UtTest_Add(LC_FloatCompare_Test_LT, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_LT");
    UtTest_Add(LC_FloatCompare_Test_EQ, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_EQ");
    UtTest_Add(LC_FloatCompare_Test_NE, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_NE");
    UtTest_Add(LC_FloatCompare_Test_GT, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_GT");
    UtTest_Add(LC_FloatCompare_Test_GE, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_GE");
    UtTest_Add(LC_FloatCompare_Test_InvalidOperatorID, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_InvalidOperatorID");
    UtTest_Add(LC_FloatCompare_Test_NaN, LC_Test_Setup, LC_Test_TearDown, "LC_FloatCompare_Test_NaN");

    UtTest_Add(LC_WPOffsetValid_Test_DataUByte, LC_Test_Setup, LC_Test_TearDown, "LC_WPOffsetValid_Test_DataUByte");
    UtTest_Add(LC_WPOffsetValid_Test_UWordLE, LC_Test_Setup, LC_Test_TearDown, "LC_WPOffsetValid_Test_UWordLE");
    UtTest_Add(LC_WPOffsetValid_Test_UDWordLE, LC_Test_Setup, LC_Test_TearDown, "LC_WPOffsetValid_Test_UDWordLE");
    UtTest_Add(LC_WPOffsetValid_Test_FloatLE, LC_Test_Setup, LC_Test_TearDown, "LC_WPOffsetValid_Test_FloatLE");
    UtTest_Add(LC_WPOffsetValid_Test_DataTypeError, LC_Test_Setup, LC_Test_TearDown, "LC_WPOffsetValid_Test_DataTypeError");
    UtTest_Add(LC_WPOffsetValid_Test_OffsetError, LC_Test_Setup, LC_Test_TearDown, "LC_WPOffsetValid_Test_OffsetError");

    UtTest_Add(LC_GetSizedWPData_Test_DataByte, LC_Test_Setup, LC_Test_TearDown, "LC_GetSizedWPData_Test_DataByte");
    UtTest_Add(LC_GetSizedWPData_Test_DataUByte, LC_Test_Setup, LC_Test_TearDown, "LC_GetSizedWPData_Test_DataUByte");
    UtTest_Add(LC_GetSizedWPData_Test_DataWordBELittleEndian, LC_Test_Setup, LC_Test_TearDown, "LC_GetSizedWPData_Test_DataWordBELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataWordLELittleEndian, LC_Test_Setup, LC_Test_TearDown, "LC_GetSizedWPData_Test_DataWordLELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataUWordBELittleEndian, LC_Test_Setup, LC_Test_TearDown, "LC_GetSizedWPData_Test_DataUWordBELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataUWordLELittleEndian, LC_Test_Setup, LC_Test_TearDown, "LC_GetSizedWPData_Test_DataUWordLELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataFloatBELittleEndian, LC_Test_Setup, LC_Test_TearDown, "LC_GetSizedWPData_Test_DataFloatBELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataFloatLELittleEndian, LC_Test_Setup, LC_Test_TearDown, "LC_GetSizedWPData_Test_DataFloatLELittleEndian");
    UtTest_Add(LC_GetSizedWPData_Test_DataTypeError, LC_Test_Setup, LC_Test_TearDown, "LC_GetSizedWPData_Test_DataTypeError");

    UtTest_Add(LC_ValidateWDT_Test_UnusedTableEntry, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_UnusedTableEntry");
    UtTest_Add(LC_ValidateWDT_Test_InvalidDataType, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_InvalidDataType");
    UtTest_Add(LC_ValidateWDT_Test_InvalidOperator, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_InvalidOperator");
    UtTest_Add(LC_ValidateWDT_Test_BadMessageID, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_BadMessageID");
    UtTest_Add(LC_ValidateWDT_Test_NaN, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_NaN");
    UtTest_Add(LC_ValidateWDT_Test_Inf, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_Inf");
    UtTest_Add(LC_ValidateWDT_Test_FloatingPointPassed, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_FloatingPointPassed");
    UtTest_Add(LC_ValidateWDT_Test_NonFloatingPointPassed, LC_Test_Setup, LC_Test_TearDown, "LC_ValidateWDT_Test_NonFloatingPointPassed");

    UtTest_Add(LC_Uint32IsNAN_Test_True, LC_Test_Setup, LC_Test_TearDown, "LC_Uint32IsNAN_Test_True");
    UtTest_Add(LC_Uint32IsNAN_Test_False, LC_Test_Setup, LC_Test_TearDown, "LC_Uint32IsNAN_Test_False");

    UtTest_Add(LC_Uint32IsInfinite_Test_True, LC_Test_Setup, LC_Test_TearDown, "LC_Uint32IsInfinite_Test_True");
    UtTest_Add(LC_Uint32IsInfinite_Test_False, LC_Test_Setup, LC_Test_TearDown, "LC_Uint32IsInfinite_Test_False");

} /* end LC_Watch_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
