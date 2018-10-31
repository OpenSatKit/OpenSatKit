 /*************************************************************************
 ** File:
 **   $Id: hk_utils_test.c 1.2 2016/10/07 14:33:10EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file hk_utils.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: hk_utils_test.c  $
 **   Revision 1.2 2016/10/07 14:33:10EDT czogby 
 **   Code Walkthrough Updates
 **   Revision 1.1 2016/06/24 14:43:51EDT czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/hk/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "hk_utils_test.h"
#include "hk_utils.h"
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

HK_Send_Out_Msg_t  HK_UTILS_TEST_ProcessNewCopyTable_CmdPacket;
int32 HK_UTILS_TEST_CFE_ES_GetPoolBufHook1 (uint32 **BufPtr, CFE_ES_MemHandle_t HandlePtr, uint32 Size)
{
    *BufPtr = (uint32 *)(&HK_UTILS_TEST_ProcessNewCopyTable_CmdPacket);

    return CFE_SUCCESS;
}

hk_copy_table_entry_t    HK_UTILS_TEST_CheckStatusOfTables_CopyTable[HK_COPY_TABLE_ENTRIES];
int32 HK_UTILS_TEST_CFE_TBL_GetAddressHook( void **TblPtr, CFE_TBL_Handle_t TblHandle )
{
    *TblPtr = (void *)(&HK_UTILS_TEST_CheckStatusOfTables_CopyTable);

    return CFE_SUCCESS;
}

void HK_ProcessIncomingHkData_Test_Nominal(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    CFE_SB_InitMsg (&CmdPacket, HK_SEND_COMBINED_PKT_MID, sizeof(HK_Send_Out_Msg_t), TRUE);

    HK_AppData.CopyTablePtr->InputMid = HK_SEND_COMBINED_PKT_MID;
    HK_AppData.CopyTablePtr->InputOffset = 1;
    HK_AppData.CopyTablePtr->NumBytes = 1;

    /* Execute the function being tested */
    HK_ProcessIncomingHkData((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (memcmp(HK_AppData.RuntimeTablePtr->OutputPktAddr, &CmdPacket, sizeof(HK_Send_Out_Msg_t)), "memcmp(RtTblEntry->OutputPktAddr, CmdPacket, sizeof(HK_Send_Out_Msg_t))");
    UtAssert_True (HK_AppData.RuntimeTablePtr->DataPresent == HK_DATA_PRESENT, "HK_AppData.RuntimeTablePtr->DataPresent == HK_DATA_PRESENT");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");
} /* end HK_ProcessIncomingHkData_Test_Nominal */

void HK_ProcessIncomingHkData_Test_MessageError(void)
{
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    CFE_SB_InitMsg (&CmdPacket, HK_SEND_COMBINED_PKT_MID, sizeof(HK_Send_Out_Msg_t), TRUE);

    HK_AppData.CopyTablePtr->InputMid = HK_SEND_COMBINED_PKT_MID;

    /* Execute the function being tested */
    HK_ProcessIncomingHkData((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_ACCESSING_PAST_PACKET_END_EID, CFE_EVS_ERROR, "HK table definition exceeds packet length. MID:0x189C, Length:10, Count:1"),
        "HK table definition exceeds packet length. MID:0x189C, Length:10, Count:1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
} /* end HK_ProcessIncomingHkData_Test_MessageError */

void HK_ValidateHkCopyTable_Test(void)
{
    hk_copy_table_entry_t    CopyTable;
    hk_runtime_tbl_entry_t   RuntimeTable;
    
    HK_AppData.CopyTablePtr = &CopyTable;
    HK_AppData.RuntimeTablePtr = &RuntimeTable;

    /* Execute the function being tested */
    HK_ValidateHkCopyTable(HK_AppData.CopyTablePtr);
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");
} /* end HK_ValidateHkCopyTable_Test */

void HK_ProcessNewCopyTable_Test_Nominal(void)
{
    uint16  i;

    hk_copy_table_entry_t    CopyTable[HK_COPY_TABLE_ENTRIES];
    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    for (i = 0; i < HK_COPY_TABLE_ENTRIES; i++)
    {
        (& HK_AppData.CopyTablePtr[i])->InputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputOffset = 1;
        (& HK_AppData.CopyTablePtr[i])->NumBytes = 1;
        (& HK_AppData.RuntimeTablePtr[i])->InputMidSubscribed = HK_INPUTMID_NOT_SUBSCRIBED;
    }

    /* Satisfies condition "Result >= CFE_SUCCESS", and prevents segmentation fault */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &HK_UTILS_TEST_CFE_ES_GetPoolBufHook1);

    /* Execute the function being tested */
    HK_ProcessNewCopyTable(HK_AppData.CopyTablePtr, HK_AppData.RuntimeTablePtr);
    
    /* Verify results */
    UtAssert_True
        ((& HK_AppData.RuntimeTablePtr[0])->OutputPktAddr == (CFE_SB_MsgPtr_t)(&HK_UTILS_TEST_ProcessNewCopyTable_CmdPacket),
        "(& HK_AppData.RuntimeTablePtr[0])->OutputPktAddr == (CFE_SB_MsgPtr_t)(&HK_UTILS_TEST_ProcessNewCopyTable_CmdPacket)");

    UtAssert_True ((& HK_AppData.RuntimeTablePtr[0])->InputMidSubscribed == HK_INPUTMID_SUBSCRIBED, "(& HK_AppData.RuntimeTablePtr[0])->InputMidSubscribed == HK_INPUTMID_SUBSCRIBED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");
} /* end HK_ProcessNewCopyTable_Test_Nominal */

void HK_ProcessNewCopyTable_Test_Errors(void)
{
    uint16  i;

    hk_copy_table_entry_t    CopyTable[HK_COPY_TABLE_ENTRIES];
    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    for (i = 0; i < HK_COPY_TABLE_ENTRIES; i++)
    {
        (& HK_AppData.CopyTablePtr[i])->InputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputOffset = 1;
        (& HK_AppData.CopyTablePtr[i])->NumBytes = 1;
    }

    /* Set to generate error message HK_MEM_POOL_MALLOC_FAILED_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_GETPOOLBUF_INDEX, -1, 1);
    Ut_CFE_ES_ContinueReturnCodeAfterCountZero(UT_CFE_ES_GETPOOLBUF_INDEX);

    /* Set to generate error message HK_CANT_SUBSCRIBE_TO_SB_PKT_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_SUBSCRIBE_INDEX);

    /* Execute the function being tested */
    HK_ProcessNewCopyTable(HK_AppData.CopyTablePtr, HK_AppData.RuntimeTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_MEM_POOL_MALLOC_FAILED_EID, CFE_EVS_ERROR, "HK Processing New Table: ES_GetPoolBuf for size 2 returned 0xFFFFFFFF"),
        "HK Processing New Table: ES_GetPoolBuf for size 2 returned 0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_CANT_SUBSCRIBE_TO_SB_PKT_EID, CFE_EVS_ERROR, "HK Processing New Table:SB_Subscribe for Mid 0x189C returned 0xFFFFFFFF"),
        "HK Processing New Table:SB_Subscribe for Mid 0x189C returned 0xFFFFFFFF");
    /* The two messages above each occur HK_COPY_TABLE_ENTRIES times */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2*HK_COPY_TABLE_ENTRIES, "Ut_CFE_EVS_GetEventQueueDepth() == 2*HK_COPY_TABLE_ENTRIES");
} /* end HK_ProcessNewCopyTable_Test_Errors */

void HK_TearDownOldCopyTable_Test_Nominal(void)
{
    uint16  i;

    hk_copy_table_entry_t    CopyTable[HK_COPY_TABLE_ENTRIES];
    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    for (i = 0; i < HK_COPY_TABLE_ENTRIES; i++)
    {
        (& HK_AppData.RuntimeTablePtr[i])->OutputPktAddr = (CFE_SB_MsgPtr_t)(1);
        (& HK_AppData.CopyTablePtr[i])->InputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.RuntimeTablePtr[i])->InputMidSubscribed = HK_INPUTMID_SUBSCRIBED;
    }

    /* Execute the function being tested */
    HK_TearDownOldCopyTable(HK_AppData.CopyTablePtr, HK_AppData.RuntimeTablePtr);
    
    /* Verify results */
    UtAssert_True ((& HK_AppData.RuntimeTablePtr[0])->OutputPktAddr == (CFE_SB_MsgPtr_t)(0), "(& HK_AppData.RuntimeTablePtr[0])->OutputPktAddr == (CFE_SB_MsgPtr_t)(0)");
    UtAssert_True ((& HK_AppData.RuntimeTablePtr[0])->InputMidSubscribed == HK_INPUTMID_NOT_SUBSCRIBED, "(& HK_AppData.RuntimeTablePtr[0])->InputMidSubscribed == HK_INPUTMID_NOT_SUBSCRIBED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");
} /* end HK_TearDownOldCopyTable_Test_Nominal */

void HK_TearDownOldCopyTable_Test_MemPoolError(void)
{
    uint16  i;

    hk_copy_table_entry_t    CopyTable[HK_COPY_TABLE_ENTRIES];
    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    for (i = 0; i < HK_COPY_TABLE_ENTRIES; i++)
    {
        (& HK_AppData.RuntimeTablePtr[i])->OutputPktAddr = (CFE_SB_MsgPtr_t)(1);
        (& HK_AppData.CopyTablePtr[i])->InputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.RuntimeTablePtr[i])->InputMidSubscribed = HK_INPUTMID_SUBSCRIBED;
    }

    /* Set to generate error message HK_MEM_POOL_FREE_FAILED_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_PUTPOOLBUF_INDEX, -1, 1);
    Ut_CFE_ES_ContinueReturnCodeAfterCountZero(UT_CFE_ES_PUTPOOLBUF_INDEX);

    /* Execute the function being tested */
    HK_TearDownOldCopyTable(HK_AppData.CopyTablePtr, HK_AppData.RuntimeTablePtr);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_MEM_POOL_FREE_FAILED_EID, CFE_EVS_ERROR, "HK TearDown: ES_putPoolBuf Err pkt:0x00000001 ret 0xFFFFFFFF, hdl 0x00000000"),
        "HK TearDown: ES_putPoolBuf Err pkt:0x00000001 ret 0xFFFFFFFF, hdl 0x00000000");

    UtAssert_True ((& HK_AppData.RuntimeTablePtr[0])->InputMidSubscribed == HK_INPUTMID_NOT_SUBSCRIBED, "(& HK_AppData.RuntimeTablePtr[0])->InputMidSubscribed == HK_INPUTMID_NOT_SUBSCRIBED");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == HK_COPY_TABLE_ENTRIES, "Ut_CFE_EVS_GetEventQueueDepth() == HK_COPY_TABLE_ENTRIES");
} /* end HK_TearDownOldCopyTable_Test_MemPoolError */

void HK_SendCombinedHkPacket_Test_MissingData(void)
{
    uint16  i;
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable[HK_COPY_TABLE_ENTRIES];
    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    CFE_SB_InitMsg (&CmdPacket, HK_SEND_COMBINED_PKT_MID, sizeof(HK_Send_Out_Msg_t), TRUE);

    for (i = 0; i < HK_COPY_TABLE_ENTRIES; i++)
    {
        (& HK_AppData.RuntimeTablePtr[i])->OutputPktAddr = (CFE_SB_MsgPtr_t)(&CmdPacket);
        (& HK_AppData.CopyTablePtr[i])->InputMid = (CFE_SB_MsgId_t)(HK_SEND_COMBINED_PKT_MID);
    }

    /* Set to generate error message HK_OUTPKT_MISSING_DATA_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, (CFE_SB_MsgId_t)(HK_SEND_COMBINED_PKT_MID), 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETMSGID_INDEX);

    /* Execute the function being tested */
    HK_SendCombinedHkPacket((CFE_SB_MsgId_t)(HK_SEND_COMBINED_PKT_MID));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_OUTPKT_MISSING_DATA_EID, CFE_EVS_DEBUG, "Combined Packet 0x189C missing data from Input Pkt 0x189C"),
        "Combined Packet 0x189C missing data from Input Pkt 0x189C");

    UtAssert_True (HK_AppData.MissingDataCtr == 1, "HK_AppData.MissingDataCtr == 1");
    UtAssert_True (HK_AppData.CombinedPacketsSent == 1, "HK_AppData.CombinedPacketsSent == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
} /* end HK_SendCombinedHkPacket_Test_MissingData */

void HK_SendCombinedHkPacket_Test_PacketFoundFalse(void)
{
    uint16  i;
    HK_Send_Out_Msg_t   CmdPacket;

    hk_copy_table_entry_t    CopyTable[HK_COPY_TABLE_ENTRIES];
    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    CFE_SB_InitMsg (&CmdPacket, HK_SEND_COMBINED_PKT_MID, sizeof(HK_Send_Out_Msg_t), TRUE);

    for (i = 0; i < HK_COPY_TABLE_ENTRIES; i++)
    {
        (& HK_AppData.RuntimeTablePtr[i])->OutputPktAddr = (CFE_SB_MsgPtr_t)(&CmdPacket);
        (& HK_AppData.CopyTablePtr[i])->InputMid = (CFE_SB_MsgId_t)(HK_SEND_COMBINED_PKT_MID);
    }

    /* Set to fail condition "ThisEntrysOutMid == WhichMidToSend", in order to  generate error message HK_UNKNOWN_COMBINED_PACKET_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, (CFE_SB_MsgId_t)(99), 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETMSGID_INDEX);

    /* Execute the function being tested */
    HK_SendCombinedHkPacket((CFE_SB_MsgId_t)(HK_SEND_COMBINED_PKT_MID));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_UNKNOWN_COMBINED_PACKET_EID, CFE_EVS_INFORMATION, "Combined HK Packet 0x189C is not found in current HK Copy Table"),
        "Combined HK Packet 0x189C is not found in current HK Copy Table");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
} /* end HK_SendCombinedHkPacket_Test_PacketFoundFalse */

void HK_CheckStatusOfTables_Test_ValidationPending(void)
{
    uint16  i;

    hk_copy_table_entry_t    CopyTable[HK_COPY_TABLE_ENTRIES];
    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    for (i = 0; i < HK_COPY_TABLE_ENTRIES; i++)
    {
        (& HK_AppData.CopyTablePtr[i])->InputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputOffset = 1;
        (& HK_AppData.CopyTablePtr[i])->NumBytes = 1;
        (& HK_AppData.RuntimeTablePtr[i])->InputMidSubscribed = HK_INPUTMID_NOT_SUBSCRIBED;
    }

    /* Prevents segmentation fault in subfunction HK_ProcessNewCopyTable */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &HK_UTILS_TEST_CFE_ES_GetPoolBufHook1);

    /* Set to satisfy condition "Status == CFE_TBL_INFO_VALIDATION_PENDING" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETSTATUS_INDEX, CFE_TBL_INFO_VALIDATION_PENDING, 1);

    /* Execute the function being tested */
    HK_CheckStatusOfTables();
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");
} /* end HK_CheckStatusOfTables_Test_ValidationPending */

void HK_CheckStatusOfTables_Test_UpdatePending(void)
{
    uint16  i;

    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &HK_UTILS_TEST_CheckStatusOfTables_CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    for (i = 0; i < HK_COPY_TABLE_ENTRIES; i++)
    {
        (& HK_AppData.CopyTablePtr[i])->InputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputOffset = 1;
        (& HK_AppData.CopyTablePtr[i])->NumBytes = 1;
        (& HK_AppData.RuntimeTablePtr[i])->InputMidSubscribed = HK_INPUTMID_NOT_SUBSCRIBED;
    }

    /* Prevents segmentation fault in subfunction HK_ProcessNewCopyTable */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &HK_UTILS_TEST_CFE_ES_GetPoolBufHook1);

    /* Set to satisfy condition "Status == CFE_TBL_INFO_UPDATE_PENDING" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETSTATUS_INDEX, CFE_TBL_INFO_UPDATE_PENDING, 1);

    /* Prevents segmentation fault */
    Ut_CFE_TBL_SetFunctionHook(UT_CFE_TBL_GETADDRESS_INDEX, &HK_UTILS_TEST_CFE_TBL_GetAddressHook);

    /* Execute the function being tested */
    HK_CheckStatusOfTables();
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");
} /* end HK_CheckStatusOfTables_Test_UpdatePending */

void HK_CheckStatusOfTables_Test_DumpPending(void)
{
    uint16  i;

    hk_copy_table_entry_t    CopyTable[HK_COPY_TABLE_ENTRIES];
    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    for (i = 0; i < HK_COPY_TABLE_ENTRIES; i++)
    {
        (& HK_AppData.CopyTablePtr[i])->InputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputOffset = 1;
        (& HK_AppData.CopyTablePtr[i])->NumBytes = 1;
        (& HK_AppData.RuntimeTablePtr[i])->InputMidSubscribed = HK_INPUTMID_NOT_SUBSCRIBED;
    }

    /* Prevents segmentation fault in subfunction HK_ProcessNewCopyTable */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &HK_UTILS_TEST_CFE_ES_GetPoolBufHook1);

    /* Set to satisfy condition "Status == CFE_TBL_INFO_DUMP_PENDING" */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETSTATUS_INDEX, CFE_TBL_INFO_DUMP_PENDING, 2);

    /* Execute the function being tested */
    HK_CheckStatusOfTables();
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");
} /* end HK_CheckStatusOfTables_Test_DumpPending */

void HK_CheckStatusOfTables_Test_Errors(void)
{
    uint16  i;

    hk_copy_table_entry_t    CopyTable[HK_COPY_TABLE_ENTRIES];
    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    for (i = 0; i < HK_COPY_TABLE_ENTRIES; i++)
    {
        (& HK_AppData.CopyTablePtr[i])->InputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputMid = HK_SEND_COMBINED_PKT_MID;
        (& HK_AppData.CopyTablePtr[i])->OutputOffset = 1;
        (& HK_AppData.CopyTablePtr[i])->NumBytes = 1;
        (& HK_AppData.RuntimeTablePtr[i])->InputMidSubscribed = HK_INPUTMID_NOT_SUBSCRIBED;
    }

    /* Prevents segmentation fault in subfunction HK_ProcessNewCopyTable */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETPOOLBUF_INDEX, &HK_UTILS_TEST_CFE_ES_GetPoolBufHook1);

    /* Set to generate error messages HK_UNEXPECTED_GETSTAT_RET_EID and HK_UNEXPECTED_GETSTAT2_RET_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_GETSTATUS_INDEX, -1, 1);
    Ut_CFE_TBL_ContinueReturnCodeAfterCountZero(UT_CFE_TBL_GETSTATUS_INDEX);

    /* Execute the function being tested */
    HK_CheckStatusOfTables();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_UNEXPECTED_GETSTAT_RET_EID, CFE_EVS_ERROR, "Unexpected CFE_TBL_GetStatus return (0xFFFFFFFF) for Copy Table"),
        "Unexpected CFE_TBL_GetStatus return (0xFFFFFFFF) for Copy Table");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(HK_UNEXPECTED_GETSTAT2_RET_EID, CFE_EVS_ERROR, "Unexpected CFE_TBL_GetStatus return (0xFFFFFFFF) for Runtime Table"),
        "Unexpected CFE_TBL_GetStatus return (0xFFFFFFFF) for Runtime Table");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
} /* end HK_CheckStatusOfTables_Test_Errors */

void HK_CheckForMissingData_Test(void)
{
    int32 Result;

    CFE_SB_MsgId_t  MissingInputMid;
    
    hk_copy_table_entry_t    CopyTable[HK_COPY_TABLE_ENTRIES];
    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    (& HK_AppData.RuntimeTablePtr[0])->OutputPktAddr = (CFE_SB_MsgPtr_t)(1);
    (& HK_AppData.CopyTablePtr[0])->InputMid = HK_SEND_COMBINED_PKT_MID;
    (& HK_AppData.CopyTablePtr[0])->OutputMid = HK_SEND_COMBINED_PKT_MID;
    (& HK_AppData.RuntimeTablePtr[0])->DataPresent = HK_DATA_NOT_PRESENT;

    /* Execute the function being tested */
    Result = HK_CheckForMissingData(HK_SEND_COMBINED_PKT_MID, &MissingInputMid);
    
    /* Verify results */
    UtAssert_True (Result == HK_MISSING_DATA_DETECTED, "Result == HK_MISSING_DATA_DETECTED");
    UtAssert_True (MissingInputMid == HK_SEND_COMBINED_PKT_MID, "MissingInputMid == HK_SEND_COMBINED_PKT_MID");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");
} /* end HK_CheckForMissingData_Test */

void HK_SetFlagsToNotPresent_Test(void)
{
    hk_copy_table_entry_t    CopyTable[HK_COPY_TABLE_ENTRIES];
    hk_runtime_tbl_entry_t   RuntimeTable[HK_COPY_TABLE_ENTRIES];
    
    HK_AppData.CopyTablePtr = &CopyTable[0];
    HK_AppData.RuntimeTablePtr = &RuntimeTable[0];

    (& HK_AppData.RuntimeTablePtr[0])->OutputPktAddr = (CFE_SB_MsgPtr_t)(1);
    (& HK_AppData.CopyTablePtr[0])->OutputMid = HK_SEND_COMBINED_PKT_MID;
    (& HK_AppData.RuntimeTablePtr[0])->DataPresent = HK_DATA_PRESENT;

    /* Execute the function being tested */
    HK_SetFlagsToNotPresent(HK_SEND_COMBINED_PKT_MID);
    
    /* Verify results */
    UtAssert_True ((& HK_AppData.RuntimeTablePtr[0])->DataPresent == HK_DATA_NOT_PRESENT, "(& HK_AppData.RuntimeTablePtr[0])->DataPresent == HK_DATA_NOT_PRESENT");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");
} /* end HK_SetFlagsToNotPresent_Test */

/* No test needed for HK_ReportHK - all lines of code are run and verified by other tests. */

void HK_Utils_Test_AddTestCases(void)
{
    UtTest_Add(HK_ProcessIncomingHkData_Test_Nominal, HK_Test_Setup, HK_Test_TearDown, "HK_ProcessIncomingHkData_Test_Nominal");
    UtTest_Add(HK_ProcessIncomingHkData_Test_MessageError, HK_Test_Setup, HK_Test_TearDown, "HK_ProcessIncomingHkData_Test_MessageError");

    UtTest_Add(HK_ValidateHkCopyTable_Test, HK_Test_Setup, HK_Test_TearDown, "HK_ValidateHkCopyTable_Test");

    UtTest_Add(HK_ProcessNewCopyTable_Test_Nominal, HK_Test_Setup, HK_Test_TearDown, "HK_ProcessNewCopyTable_Test_Nominal");
    UtTest_Add(HK_ProcessNewCopyTable_Test_Errors, HK_Test_Setup, HK_Test_TearDown, "HK_ProcessNewCopyTable_Test_Errors");

    UtTest_Add(HK_TearDownOldCopyTable_Test_Nominal, HK_Test_Setup, HK_Test_TearDown, "HK_TearDownOldCopyTable_Test_Nominal");
    UtTest_Add(HK_TearDownOldCopyTable_Test_MemPoolError, HK_Test_Setup, HK_Test_TearDown, "HK_TearDownOldCopyTable_Test_MemPoolError");

    UtTest_Add(HK_SendCombinedHkPacket_Test_MissingData, HK_Test_Setup, HK_Test_TearDown, "HK_SendCombinedHkPacket_Test_MissingData");
    UtTest_Add(HK_SendCombinedHkPacket_Test_PacketFoundFalse, HK_Test_Setup, HK_Test_TearDown, "HK_SendCombinedHkPacket_Test_PacketFoundFalse");

    UtTest_Add(HK_CheckStatusOfTables_Test_ValidationPending, HK_Test_Setup, HK_Test_TearDown, "HK_CheckStatusOfTables_Test_ValidationPending");
    UtTest_Add(HK_CheckStatusOfTables_Test_UpdatePending, HK_Test_Setup, HK_Test_TearDown, "HK_CheckStatusOfTables_Test_UpdatePending");
    UtTest_Add(HK_CheckStatusOfTables_Test_DumpPending, HK_Test_Setup, HK_Test_TearDown, "HK_CheckStatusOfTables_Test_DumpPending");
    UtTest_Add(HK_CheckStatusOfTables_Test_Errors, HK_Test_Setup, HK_Test_TearDown, "HK_CheckStatusOfTables_Test_Errors");

    UtTest_Add(HK_CheckForMissingData_Test, HK_Test_Setup, HK_Test_TearDown, "HK_CheckForMissingData_Test");

    UtTest_Add(HK_SetFlagsToNotPresent_Test, HK_Test_Setup, HK_Test_TearDown, "HK_SetFlagsToNotPresent_Test");
} /* end HK_Utils_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
