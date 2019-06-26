 /*************************************************************************
 ** File:
 **   $Id: sc_rtsrq_test.c 1.3 2016/09/09 19:27:07EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file sc_atsrq.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sc_rtsrq_test.c  $
 **   Revision 1.3 2016/09/09 19:27:07EDT czogby 
 **   Update based on code inspection log
 **   Revision 1.2 2016/02/04 16:03:12EST czogby 
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

void SC_StartRtsCmd_Test_Nominal(void)
{
    SC_RtsCmd_t   CmdPacket;
    SC_RtsEntryHeader_t*   Entry;
    uint8   RtsIndex = 0;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 0;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = 1;

    SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = FALSE;
    SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus = SC_LOADED;

    /* Set message size in order to satisfy if-statement after comment "Make sure the command is big enough, but not too big" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    SC_StartRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_EXECUTING, "SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_EXECUTING");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].CmdCtr == 0, "SC_OperData.RtsInfoTblAddr[RtsIndex].CmdCtr == 0");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr == 0, "SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr == 0");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandPtr == 0, "SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandPtr == 0");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].UseCtr == 1, "SC_OperData.RtsInfoTblAddr[RtsIndex].UseCtr == 1");

    UtAssert_True(SC_OperData.RtsCtrlBlckAddr->NumRtsActive == 1, "SC_OperData.RtsCtrlBlckAddr->NumRtsActive == 1");
    UtAssert_True(SC_AppData.RtsActiveCtr == 1, "SC_AppData.RtsActiveCtr == 1");
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_RTS_START_INF_EID, CFE_EVS_INFORMATION, "RTS Number 001 Started"),
         "RTS Number 001 Started");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartRtsCmd_Test_Nominal */

void SC_StartRtsCmd_Test_StartRtsNoEvents(void)
{
    SC_RtsCmd_t   CmdPacket;
    SC_RtsEntryHeader_t*   Entry;
    uint8   RtsIndex;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = SC_NUMBER_OF_RTS; /* 5 */

    RtsIndex = CmdPacket.RtsId - 1; /* 4 */

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 0;

    SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = FALSE;
    SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus = SC_LOADED;

    /* Set message size in order to satisfy if-statement after comment "Make sure the command is big enough, but not too big" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);
    Ut_CFE_SB_ContinueReturnCodeAfterCountZero(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX);

    /* Execute the function being tested */
    SC_StartRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_EXECUTING, "SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_EXECUTING");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].CmdCtr == 0, "SC_OperData.RtsInfoTblAddr[RtsIndex].CmdCtr == 0");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr == 0, "SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr == 0");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandPtr == 0, "SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandPtr == 0");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].UseCtr == 1, "SC_OperData.RtsInfoTblAddr[RtsIndex].UseCtr == 1");

    UtAssert_True(SC_OperData.RtsCtrlBlckAddr->NumRtsActive == 1, "SC_OperData.RtsCtrlBlckAddr->NumRtsActive == 1");
    UtAssert_True(SC_AppData.RtsActiveCtr == 1, "SC_AppData.RtsActiveCtr == 1");
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STARTRTS_CMD_DBG_EID, CFE_EVS_DEBUG, "Start RTS #5 command"),
         "Start RTS #5 command");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartRtsCmd_Test_StartRtsNoEvents */

void SC_StartRtsCmd_Test_InvalidCommandLength(void)
{
    SC_RtsCmd_t   CmdPacket;
    SC_RtsEntryHeader_t*   Entry;
    uint8   RtsIndex = 0;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = 1;

    SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = FALSE;
    SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus = SC_LOADED;

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&Entry;
    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 0;

    /* Set message size in order to satisfy if-statement after comment "Make sure the command is big enough, but not too big" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);

    /* Execute the function being tested */
    SC_StartRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STARTRTS_CMD_INVLD_LEN_ERR_EID, CFE_EVS_ERROR, "Start RTS 001 Rejected: Invld Len Field for 1st Cmd in Sequence. Invld Cmd Length = 49159"),
         "Start RTS 001 Rejected: Invld Len Field for 1st Cmd in Sequence. Invld Cmd Length = 49159");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartRtsCmd_Test_InvalidCommandLength */

void SC_StartRtsCmd_Test_RtsNotLoadedOrInUse(void)
{
    SC_RtsCmd_t   CmdPacket;
    SC_RtsEntryHeader_t*   Entry;
    uint8   RtsIndex = 0;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 0;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = 1;

    SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = FALSE;
    SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus = SC_IDLE;

    /* Set message size in order to satisfy if-statement after comment "Make sure the command is big enough, but not too big" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);

    /* Execute the function being tested */
    SC_StartRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr == 1");
    UtAssert_True(SC_AppData.RtsActiveErrCtr == 1, "SC_OperData.RtsInfoTblAddr[RtsIndex].RtsActiveErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STARTRTS_CMD_NOT_LDED_ERR_EID, CFE_EVS_ERROR, "Start RTS 001 Rejected: RTS Not Loaded or In Use, Status: 2"),
         "Start RTS 001 Rejected: RTS Not Loaded or In Use, Status: 2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartRtsCmd_Test_RtsNotLoadedOrInUse */

void SC_StartRtsCmd_Test_RtsDisabled(void)
{
    SC_RtsCmd_t   CmdPacket;
    SC_RtsEntryHeader_t*   Entry;
    uint8   RtsIndex = 0;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 0;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = 1;

    SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = TRUE;

    /* Set message size in order to satisfy if-statement after comment "Make sure the command is big enough, but not too big" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);

    /* Execute the function being tested */
    SC_StartRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr == 1");
    UtAssert_True(SC_AppData.RtsActiveErrCtr == 1, "SC_OperData.RtsInfoTblAddr[RtsIndex].RtsActiveErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STARTRTS_CMD_DISABLED_ERR_EID, CFE_EVS_ERROR, "Start RTS 001 Rejected: RTS Disabled"),
         "Start RTS 001 Rejected: RTS Disabled");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartRtsCmd_Test_RtsDisabled */

void SC_StartRtsCmd_Test_InvalidRtsId(void)
{
    SC_RtsCmd_t   CmdPacket;
    SC_RtsEntryHeader_t*   Entry;
    uint8   RtsIndex = SC_NUMBER_OF_RTS*2 - 1; /* RtsId - 1 */
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 0;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = SC_NUMBER_OF_RTS*2;

    /* Set message size in order to satisfy if-statement after comment "Make sure the command is big enough, but not too big" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);

    /* Execute the function being tested */
    SC_StartRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr == 1");
    UtAssert_True(SC_AppData.RtsActiveErrCtr == 1, "SC_OperData.RtsInfoTblAddr[RtsIndex].RtsActiveErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STARTRTS_CMD_INVALID_ERR_EID, CFE_EVS_ERROR, "Start RTS 010 Rejected: Invalid RTS ID"),
         "Start RTS 010 Rejected: Invalid RTS ID");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartRtsCmd_Test_InvalidRtsId */

void SC_StartRtsGrpCmd_Test_Nominal(void)
{
    SC_RtsGrpCmd_t   CmdPacket;
    SC_RtsEntryHeader_t*   Entry;
    uint8   RtsIndex = 0;
    uint16 RtsTable[SC_RTS_BUFF_SIZE];

    SC_InitTables();

    SC_OperData.RtsTblAddr[RtsIndex] = (uint16*)&RtsTable[0];

    Entry = (SC_RtsEntryHeader_t *) &SC_OperData.RtsTblAddr[RtsIndex][0];
    Entry->TimeTag = 1;

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsGrpCmd_t), TRUE);
    CmdPacket.FirstRtsId = 1;
    CmdPacket.LastRtsId = 1;

    SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = FALSE;
    SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus = SC_LOADED;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsGrpCmd_t), 1);

    /* Execute the function being tested */
    SC_StartRtsGrpCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_EXECUTING, "SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_EXECUTING");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].CmdCtr == 0, "SC_OperData.RtsInfoTblAddr[RtsIndex].CmdCtr == 0");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr == 0, "SC_OperData.RtsInfoTblAddr[RtsIndex].CmdErrCtr == 0");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandPtr == 0, "SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandPtr == 0");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].UseCtr == 1, "SC_OperData.RtsInfoTblAddr[RtsIndex].UseCtr == 1");

    UtAssert_True(SC_OperData.RtsCtrlBlckAddr->NumRtsActive == 1, "SC_OperData.RtsCtrlBlckAddr->NumRtsActive == 1");
    UtAssert_True(SC_AppData.RtsActiveCtr == 1, "SC_AppData.RtsActiveCtr == 1");
    UtAssert_True(SC_AppData.CmdCtr == 2, "SC_AppData.CmdCtr == 2");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STARTRTSGRP_CMD_INF_EID, CFE_EVS_INFORMATION, "Start RTS group: FirstID=1, LastID=1, Modified=1"),
         "Start RTS group: FirstID=1, LastID=1, Modified=1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartRtsGrpCmd_Test_Nominal */

void SC_StartRtsGrpCmd_Test_StartRtsGroupError(void)
{
    SC_RtsGrpCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsGrpCmd_t), TRUE);
    CmdPacket.FirstRtsId = SC_NUMBER_OF_RTS*2;
    CmdPacket.LastRtsId = SC_NUMBER_OF_RTS*2;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsGrpCmd_t), 1);

    /* Execute the function being tested */
    SC_StartRtsGrpCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STARTRTSGRP_CMD_ERR_EID, CFE_EVS_ERROR, "Start RTS group error: FirstID=10, LastID=10"),
         "Start RTS group error: FirstID=10, LastID=10");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StartRtsGrpCmd_Test_StartRtsGroupError */

void SC_StopRtsCmd_Test_Nominal(void)
{
    SC_RtsCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = 1;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);

    /* Execute the function being tested */
    SC_StopRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STOPRTS_CMD_INF_EID, CFE_EVS_INFORMATION, "RTS 001 Aborted"),
         "RTS 001 Aborted");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StopRtsCmd_Test_Nominal */

void SC_StopRtsCmd_Test_InvalidRts(void)
{
    SC_RtsCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = SC_NUMBER_OF_RTS*2;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);

    /* Execute the function being tested */
    SC_StopRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STOPRTS_CMD_ERR_EID, CFE_EVS_ERROR, "Stop RTS 010 rejected: Invalid RTS ID"),
         "Stop RTS 010 rejected: Invalid RTS ID");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StopRtsCmd_Test_InvalidRts */

void SC_StopRtsGrpCmd_Test_Nominal(void)
{
    SC_RtsGrpCmd_t   CmdPacket;
    uint8   RtsIndex = 0;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsGrpCmd_t), TRUE);
    CmdPacket.FirstRtsId = 1;
    CmdPacket.LastRtsId = 1;

    SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus = SC_EXECUTING;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsGrpCmd_t), 1);

    /* Execute the function being tested */
    SC_StopRtsGrpCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STOPRTSGRP_CMD_INF_EID, CFE_EVS_INFORMATION,
                               "Stop RTS group: FirstID=1, LastID=1, Modified=1"),
         "Stop RTS group: FirstID=1, LastID=1, Modified=1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StopRtsGrpCmd_Test_Nominal */

void SC_StopRtsGrpCmd_Test_Error(void)
{
    SC_RtsGrpCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsGrpCmd_t), TRUE);
    CmdPacket.FirstRtsId = SC_NUMBER_OF_RTS*2;
    CmdPacket.LastRtsId = SC_NUMBER_OF_RTS*2;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsGrpCmd_t), 1);

    /* Execute the function being tested */
    SC_StopRtsGrpCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_STOPRTSGRP_CMD_ERR_EID, CFE_EVS_ERROR,
                               "Stop RTS group error: FirstID=10, LastID=10"),
         "Stop RTS group error: FirstID=10, LastID=10");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_StopRtsGrpCmd_Test_Error */

void SC_DisableRtsCmd_Test_Nominal(void)
{
    SC_RtsCmd_t   CmdPacket;
    uint8   RtsIndex = 0;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = 1;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);

    /* Execute the function being tested */
    SC_DisableRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == TRUE, "SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == TRUE");
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_DISABLE_RTS_DEB_EID, CFE_EVS_DEBUG, "Disabled RTS 001"),
         "Disabled RTS 001");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_DisableRtsCmd_Test_Nominal */

void SC_DisableRtsCmd_Test_InvalidRtsID(void)
{
    SC_RtsCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = SC_NUMBER_OF_RTS*2;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);

    /* Execute the function being tested */
    SC_DisableRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_DISRTS_CMD_ERR_EID, CFE_EVS_ERROR, "Disable RTS 010 Rejected: Invalid RTS ID"),
         "Disable RTS 010 Rejected: Invalid RTS ID");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_DisableRtsCmd_Test_InvalidRtsID */

void SC_DisableRtsGrpCmd_Test_Nominal(void)
{
    SC_RtsGrpCmd_t   CmdPacket;
    uint8   RtsIndex = 0; /* RtsId - 1 */

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsGrpCmd_t), TRUE);
    CmdPacket.FirstRtsId = 1;
    CmdPacket.LastRtsId = 1;

    SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = FALSE;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsGrpCmd_t), 1);

    /* Execute the function being tested */
    SC_DisableRtsGrpCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == TRUE, "SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == TRUE");
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_DISRTSGRP_CMD_INF_EID, CFE_EVS_INFORMATION,
                              "Disable RTS group: FirstID=1, LastID=1"),
         "Disable RTS group error: FirstID=1, LastID=1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_DisableRtsGrpCmd_Test_Nominal */

void SC_DisableRtsGrpCmd_Test_Error(void)
{
    SC_RtsGrpCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsGrpCmd_t), TRUE);
    CmdPacket.FirstRtsId = SC_NUMBER_OF_RTS*2;
    CmdPacket.LastRtsId = SC_NUMBER_OF_RTS*2;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsGrpCmd_t), 1);

    /* Execute the function being tested */
    SC_DisableRtsGrpCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_DISRTSGRP_CMD_ERR_EID, CFE_EVS_ERROR,
                              "Disable RTS group error: FirstID=10, LastID=10"),
         "Disable RTS group error: FirstID=10, LastID=10");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_DisableRtsGrpCmd_Test_Error */

void SC_EnableRtsCmd_Test_Nominal(void)
{
    SC_RtsCmd_t   CmdPacket;
    uint8   RtsIndex = 0; /* RtsId - 1 */

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = 1;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);

    /* Execute the function being tested */
    SC_EnableRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == FALSE, "SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == FALSE");
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ENABLE_RTS_DEB_EID, CFE_EVS_DEBUG, "Enabled RTS 001"),
         "Enabled RTS 001");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_EnableRtsCmd_Test_Nominal */

void SC_EnableRtsCmd_Test_InvalidRtsID(void)
{
    SC_RtsCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsCmd_t), TRUE);
    CmdPacket.RtsId = SC_NUMBER_OF_RTS*2;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsCmd_t), 1);

    /* Execute the function being tested */
    SC_EnableRtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ENARTS_CMD_ERR_EID, CFE_EVS_ERROR, "Enable RTS 010 Rejected: Invalid RTS ID"),
         "Enable RTS 010 Rejected: Invalid RTS ID");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_EnableRtsCmd_Test_InvalidRtsID */

void SC_EnableRtsGrpCmd_Test_Nominal(void)
{
    SC_RtsGrpCmd_t   CmdPacket;
    uint8   RtsIndex = 0; /* RtsId - 1 */

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsGrpCmd_t), TRUE);
    CmdPacket.FirstRtsId = 1;
    CmdPacket.LastRtsId = 1;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsGrpCmd_t), 1);

    SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag = TRUE;

    /* Execute the function being tested */
    SC_EnableRtsGrpCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == FALSE, "SC_OperData.RtsInfoTblAddr[RtsIndex].DisabledFlag == FALSE");
    UtAssert_True(SC_AppData.CmdCtr == 1, "SC_AppData.CmdCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ENARTSGRP_CMD_INF_EID, CFE_EVS_INFORMATION, "Enable RTS group: FirstID=1, LastID=1, Modified=1"),
         "Enable RTS group: FirstID=1, LastID=1, Modified=1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_EnableRtsGrpCmd_Test_Nominal */

void SC_EnableRtsGrpCmd_Test_Error(void)
{
    SC_RtsGrpCmd_t   CmdPacket;

    SC_InitTables();

    CFE_SB_InitMsg (&CmdPacket, SC_CMD_MID, sizeof(SC_RtsGrpCmd_t), TRUE);
    CmdPacket.FirstRtsId = SC_NUMBER_OF_RTS*2;
    CmdPacket.LastRtsId = SC_NUMBER_OF_RTS*2;

    /* Set message size so SC_VerifyCmdLength will return TRUE, to satisfy first if-statement */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(SC_RtsGrpCmd_t), 1);

    /* Execute the function being tested */
    SC_EnableRtsGrpCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SC_AppData.CmdErrCtr == 1, "SC_AppData.CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SC_ENARTSGRP_CMD_ERR_EID, CFE_EVS_ERROR, "Enable RTS group error: FirstID=10, LastID=10"),
         "Enable RTS group error: FirstID=10, LastID=10");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");

} /* end SC_EnableRtsGrpCmd_Test_Error */

void SC_KillRts_Test(void)
{
    uint8   RtsIndex = 0;

    SC_InitTables();

    SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus = SC_EXECUTING;
    SC_OperData.RtsCtrlBlckAddr->NumRtsActive = 1;

    /* Execute the function being tested */
    SC_KillRts(RtsIndex);
    
    /* Verify results */
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_LOADED, "SC_OperData.RtsInfoTblAddr[RtsIndex].RtsStatus == SC_LOADED");
    UtAssert_True(SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandTime == SC_MAX_TIME, "SC_OperData.RtsInfoTblAddr[RtsIndex].NextCommandTime == SC_MAX_TIME");
    UtAssert_True(SC_OperData.RtsCtrlBlckAddr->NumRtsActive == 0, "SC_OperData.RtsCtrlBlckAddr->NumRtsActive == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SC_KillRts_Test */

/* No test needed for SC_AutoStartRts - all lines run and tested in other tests. */

void SC_Rtsrq_Test_AddTestCases(void)
{
    UtTest_Add(SC_StartRtsCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_StartRtsCmd_Test_Nominal");
    UtTest_Add(SC_StartRtsCmd_Test_StartRtsNoEvents, SC_Test_Setup, SC_Test_TearDown, "SC_StartRtsCmd_Test_StartRtsNoEvents");
    UtTest_Add(SC_StartRtsCmd_Test_InvalidCommandLength, SC_Test_Setup, SC_Test_TearDown, "SC_StartRtsCmd_Test_InvalidCommandLength");
    UtTest_Add(SC_StartRtsCmd_Test_RtsNotLoadedOrInUse, SC_Test_Setup, SC_Test_TearDown, "SC_StartRtsCmd_Test_RtsNotLoadedOrInUse");
    UtTest_Add(SC_StartRtsCmd_Test_RtsDisabled, SC_Test_Setup, SC_Test_TearDown, "SC_StartRtsCmd_Test_RtsDisabled");
    UtTest_Add(SC_StartRtsCmd_Test_InvalidRtsId, SC_Test_Setup, SC_Test_TearDown, "SC_StartRtsCmd_Test_InvalidRtsId");
    UtTest_Add(SC_StartRtsGrpCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_StartRtsGrpCmd_Test_Nominal");
    UtTest_Add(SC_StartRtsGrpCmd_Test_StartRtsGroupError, SC_Test_Setup, SC_Test_TearDown, "SC_StartRtsGrpCmd_Test_StartRtsGroupError");
    UtTest_Add(SC_StopRtsCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_StopRtsCmd_Test_Nominal");
    UtTest_Add(SC_StopRtsCmd_Test_InvalidRts, SC_Test_Setup, SC_Test_TearDown, "SC_StopRtsCmd_Test_InvalidRts");
    UtTest_Add(SC_StopRtsGrpCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_StopRtsGrpCmd_Test_Nominal");
    UtTest_Add(SC_StopRtsGrpCmd_Test_Error, SC_Test_Setup, SC_Test_TearDown, "SC_StopRtsGrpCmd_Test_Error");
    UtTest_Add(SC_DisableRtsCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_DisableRtsCmd_Test_Nominal");
    UtTest_Add(SC_DisableRtsCmd_Test_InvalidRtsID, SC_Test_Setup, SC_Test_TearDown, "SC_DisableRtsCmd_Test_InvalidRtsID");
    UtTest_Add(SC_DisableRtsGrpCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_DisableRtsGrpCmd_Test_Nominal");
    UtTest_Add(SC_DisableRtsGrpCmd_Test_Error, SC_Test_Setup, SC_Test_TearDown, "SC_DisableRtsGrpCmd_Test_Error");
    UtTest_Add(SC_EnableRtsCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_EnableRtsCmd_Test_Nominal");
    UtTest_Add(SC_EnableRtsCmd_Test_InvalidRtsID, SC_Test_Setup, SC_Test_TearDown, "SC_EnableRtsCmd_Test_InvalidRtsID");
    UtTest_Add(SC_EnableRtsGrpCmd_Test_Nominal, SC_Test_Setup, SC_Test_TearDown, "SC_EnableRtsGrpCmd_Test_Nominal");
    UtTest_Add(SC_EnableRtsGrpCmd_Test_Error, SC_Test_Setup, SC_Test_TearDown, "SC_EnableRtsGrpCmd_Test_Error");
    UtTest_Add(SC_KillRts_Test, SC_Test_Setup, SC_Test_TearDown, "SC_KillRts_Test");
} /* end SC_Rtsrq_Test_AddTestCases */


/************************/
/*  End of File Comment */
/************************/
