 /*************************************************************************
 ** File:
 **   $Id: mm_app_test.c 1.5.1.1 2016/11/17 16:35:29EST sstrege Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file mm_app.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: mm_app_test.c  $
 **   Revision 1.5.1.1 2016/11/17 16:35:29EST sstrege 
 **   
 **   Revision 1.6 2016/11/17 12:51:15EST mdeschu 
 **   Remove references to temporary header file mm_test_exports.h
 **   Revision 1.5 2016/10/24 18:34:56EDT czogby 
 **   Code Walkthough Updates
 **   Revision 1.4 2016/05/04 19:06:43EDT czogby 
 **   Update MM to Compile With New UT-Assert PSP Files
 **   Revision 1.3 2016/03/04 20:15:48EST czogby 
 **   Use Constants for Version Number Checks
 **   Revision 1.2 2016/03/04 20:00:58EST czogby 
 **   Need Temporary Header File to Import Some MM Functions Into Unit Tests
 **   Revision 1.1 2016/02/03 17:11:36EST czogby 
 **   Initial revision
 **   Member added to project /CFS-APPs-PROJECT/mm/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "mm_app_test.h"
#include "mm_app.h"
#include "mm_msg.h"
#include "mm_msgdefs.h"
#include "mm_msgids.h"
#include "mm_events.h"
#include "mm_version.h"
#include "mm_test_utils.h"
#include "ut_osapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_psp_eeprom_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Function Definitions
 */

void MM_APP_TEST_CFE_ES_ExitAppHook(uint32 ExitStatus)
{
    MM_AppData.CmdCounter++;
}

void MM_AppMain_Test_Nominal(void)
{
    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to prevent call to CFE_SB_RcvMsg from returning an error */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, MM_SEND_HK_MID, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(MM_NoArgsCmd_t), 1);

    /* Used to verify completion of MM_AppMain by incrementing MM_AppData.CmdCounter */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_EXITAPP_INDEX, &MM_APP_TEST_CFE_ES_ExitAppHook);

    /* Execute the function being tested */
    MM_AppMain();
    
    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppMain_Test_Nominal */

void MM_AppMain_Test_AppInitError(void)
{
    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to prevent call to CFE_SB_RcvMsg from returning an error */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, MM_SEND_HK_MID, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(MM_NoArgsCmd_t), 1);

    /* Set to satisfy condition "Status != CFE_SUCCESS" */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_AppMain();
    
    /* Verify results */
    UtAssert_True (MM_AppData.RunStatus == CFE_ES_APP_ERROR, "MM_AppData.RunStatus == CFE_ES_APP_ERROR");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_AppMain_Test_AppInitError */

void MM_AppMain_Test_SBError(void)
{
    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to generate error message MM_PIPE_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, MM_SEND_HK_MID, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(MM_NoArgsCmd_t), 1);

    /* Used to verify completion of MM_AppMain by incrementing MM_AppData.CmdCounter. */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_EXITAPP_INDEX, &MM_APP_TEST_CFE_ES_ExitAppHook);

    /* Execute the function being tested */
    MM_AppMain();
    
    /* Verify results */
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_PIPE_ERR_EID, CFE_EVS_ERROR, "SB Pipe Read Error, App will exit. RC = 0xFFFFFFFF"),
        "SB Pipe Read Error, App will exit. RC = 0xFFFFFFFF");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end MM_AppMain_Test_SBError */

void MM_AppInit_Test_Nominal(void)
{
    int32  Result;
    char   Message[125];

    /* Initialize all elements to 1, in order verify that elements initialized to 0 by MM_AppInit are actually initialized */
    CFE_PSP_MemSet(&MM_AppData, 1, sizeof(MM_AppData_t));

    /* Execute the function being tested */
    Result = MM_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (MM_AppData.RunStatus == CFE_ES_APP_RUN, "MM_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 0, "MM_AppData.ErrCounter == 0");
    UtAssert_True (strncmp(MM_AppData.PipeName, "MM_CMD_PIPE", 16) == 0, "strncmp(MM_AppData.PipeName, 'MM_CMD_PIPE', 16) == 0");
    UtAssert_True (MM_AppData.PipeDepth == MM_CMD_PIPE_DEPTH, "MM_AppData.PipeDepth == MM_CMD_PIPE_DEPTH");
    UtAssert_True (MM_AppData.LimitHK == MM_HK_LIMIT, "MM_AppData.LimitHK == MM_HK_LIMIT");
    UtAssert_True (MM_AppData.LimitCmd == MM_CMD_LIMIT, "MM_AppData.LimitCmd == MM_CMD_LIMIT");

    sprintf(Message, "MM Initialized. Version %d.%d.%d.%d", MM_MAJOR_VERSION, MM_MINOR_VERSION, MM_REVISION, MM_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(MM_INIT_INF_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppInit_Test_Nominal */

void MM_AppInit_Test_EVSRegisterError(void)
{
    int32 Result;

    /* Initialize all elements to 1, in order verify that elements initialized to 0 by MM_AppInit are actually initialized */
    CFE_PSP_MemSet(&MM_AppData, 1, sizeof(MM_AppData_t));

    /* Set to generate system log message "MM App: Error Registering For Event Services " */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (MM_AppData.RunStatus == CFE_ES_APP_RUN, "MM_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 0, "MM_AppData.ErrCounter == 0");
    UtAssert_True (strncmp(MM_AppData.PipeName, "MM_CMD_PIPE", 16) == 0, "strncmp(MM_AppData.PipeName, 'MM_CMD_PIPE', 16) == 0");
    UtAssert_True (MM_AppData.PipeDepth == MM_CMD_PIPE_DEPTH, "MM_AppData.PipeDepth == MM_CMD_PIPE_DEPTH");
    UtAssert_True (MM_AppData.LimitHK == MM_HK_LIMIT, "MM_AppData.LimitHK == MM_HK_LIMIT");
    UtAssert_True (MM_AppData.LimitCmd == MM_CMD_LIMIT, "MM_AppData.LimitCmd == MM_CMD_LIMIT");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MM App: Error Registering For Event Services, RC = 0xFFFFFFFF\n"),
        "MM App: Error Registering For Event Services, RC = 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end MM_AppInit_Test_EVSRegisterError */

void MM_AppInit_Test_SBCreatePipeError(void)
{
    int32 Result;

    /* Initialize all elements to 1, in order verify that elements initialized to 0 by MM_AppInit are actually initialized */
    CFE_PSP_MemSet(&MM_AppData, 1, sizeof(MM_AppData_t));

    /* Set to generate system log message "MM App: Error Creating SB Pipe" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (MM_AppData.RunStatus == CFE_ES_APP_RUN, "MM_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 0, "MM_AppData.ErrCounter == 0");
    UtAssert_True (strncmp(MM_AppData.PipeName, "MM_CMD_PIPE", 16) == 0, "strncmp(MM_AppData.PipeName, 'MM_CMD_PIPE', 16) == 0");
    UtAssert_True (MM_AppData.PipeDepth == MM_CMD_PIPE_DEPTH, "MM_AppData.PipeDepth == MM_CMD_PIPE_DEPTH");
    UtAssert_True (MM_AppData.LimitHK == MM_HK_LIMIT, "MM_AppData.LimitHK == MM_HK_LIMIT");
    UtAssert_True (MM_AppData.LimitCmd == MM_CMD_LIMIT, "MM_AppData.LimitCmd == MM_CMD_LIMIT");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MM App: Error Creating SB Pipe, RC = 0xFFFFFFFF\n"),
        "MM App: Error Creating SB Pipe, RC = 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end MM_AppInit_Test_SBCreatePipeError */

void MM_AppInit_Test_SBSubscribeHKError(void)
{
    int32 Result;

    /* Initialize all elements to 1, in order verify that elements initialized to 0 by MM_AppInit are actually initialized */
    CFE_PSP_MemSet(&MM_AppData, 1, sizeof(MM_AppData_t));

    /* Set to generate system log message "MM App: Error Subscribing to HK Request" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = MM_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (MM_AppData.RunStatus == CFE_ES_APP_RUN, "MM_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 0, "MM_AppData.ErrCounter == 0");
    UtAssert_True (strncmp(MM_AppData.PipeName, "MM_CMD_PIPE", 16) == 0, "strncmp(MM_AppData.PipeName, 'MM_CMD_PIPE', 16) == 0");
    UtAssert_True (MM_AppData.PipeDepth == MM_CMD_PIPE_DEPTH, "MM_AppData.PipeDepth == MM_CMD_PIPE_DEPTH");
    UtAssert_True (MM_AppData.LimitHK == MM_HK_LIMIT, "MM_AppData.LimitHK == MM_HK_LIMIT");
    UtAssert_True (MM_AppData.LimitCmd == MM_CMD_LIMIT, "MM_AppData.LimitCmd == MM_CMD_LIMIT");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MM App: Error Subscribing to HK Request, RC = 0xFFFFFFFF\n"),
        "MM App: Error Subscribing to HK Request, RC = 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end MM_AppInit_Test_SBSubscribeHKError */

void MM_AppInit_Test_SBSubscribeMMError(void)
{
    int32 Result;

    /* Initialize all elements to 1, in order verify that elements initialized to 0 by MM_AppInit are actually initialized */
    CFE_PSP_MemSet(&MM_AppData, 1, sizeof(MM_AppData_t));

    /* Set to generate system log message "MM App: Error Subscribing to MM Command" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = MM_AppInit();
    
    /* Verify results */
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (MM_AppData.RunStatus == CFE_ES_APP_RUN, "MM_AppData.RunStatus == CFE_ES_APP_RUN");
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 0, "MM_AppData.ErrCounter == 0");
    UtAssert_True (strncmp(MM_AppData.PipeName, "MM_CMD_PIPE", 16) == 0, "strncmp(MM_AppData.PipeName, 'MM_CMD_PIPE', 16) == 0");
    UtAssert_True (MM_AppData.PipeDepth == MM_CMD_PIPE_DEPTH, "MM_AppData.PipeDepth == MM_CMD_PIPE_DEPTH");
    UtAssert_True (MM_AppData.LimitHK == MM_HK_LIMIT, "MM_AppData.LimitHK == MM_HK_LIMIT");
    UtAssert_True (MM_AppData.LimitCmd == MM_CMD_LIMIT, "MM_AppData.LimitCmd == MM_CMD_LIMIT");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("MM App: Error Subscribing to MM Command, RC = 0xFFFFFFFF\n"),
        "MM App: Error Subscribing to MM Command, RC = 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end MM_AppInit_Test_SBSubscribeMMError */

void MM_AppPipe_Test_Noop(void)
{
    MM_NoArgsCmd_t   CmdPacket;
    char             Message[125];

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_NOOP_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    sprintf(Message, "No-op command. Version %d.%d.%d.%d", MM_MAJOR_VERSION, MM_MINOR_VERSION, MM_REVISION, MM_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(MM_NOOP_INF_EID, CFE_EVS_INFORMATION, Message), Message);
    /* Note: this event message is generated in subfunction MM_NoopCmd.  It is checked here to verify that the subfunction has been reached. */

    UtAssert_True (MM_AppData.LastAction == MM_NOOP, "MM_AppData.LastAction == MM_NOOP");
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
} /* end MM_AppPipe_Test_Noop */

void MM_AppPipe_Test_Reset(void)
{
    MM_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_RESET_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_RESET_DBG_EID, CFE_EVS_DEBUG, "Reset counters command received"),
        "Reset counters command received");
    /* Note: this event message is generated in subfunction MM_ResetCmd.  It is checked here to verify that the subfunction has been reached. */

    UtAssert_True (MM_AppData.LastAction == MM_RESET, "MM_AppData.LastAction == MM_RESET");
    UtAssert_True (MM_AppData.CmdCounter == 0, "MM_AppData.CmdCounter == 0");
    UtAssert_True (MM_AppData.ErrCounter == 0, "MM_AppData.ErrCounter == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_Reset */

void MM_AppPipe_Test_Peek(void)
{
    MM_PeekCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PeekCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_PEEK_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    /* Generates 1 event message in subfunction MM_PeekCmd.  Event message count = 1 verifies that the subfunction has been reached. */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_Peek */

void MM_AppPipe_Test_Poke(void)
{
    MM_PokeCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_PokeCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_POKE_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    /* Generates 1 event message in subfunction MM_PokeCmd.  Event message count = 1 verifies that the subfunction has been reached. */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_Poke */

void MM_AppPipe_Test_LoadMemWID(void)
{
    MM_LoadMemWIDCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemWIDCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_WID_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    /* Generates 1 event message in subfunction MM_LoadMemWIDCmd.  Event message count = 1 verifies that the subfunction has been reached. */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_LoadMemWID */

void MM_AppPipe_Test_LoadMemFromFile(void)
{
    MM_LoadMemFromFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LoadMemFromFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOAD_MEM_FROM_FILE_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    /* Generates 1 event message in subfunction MM_LoadMemFromFileCmd.  Event message count = 1 verifies that the subfunction has been reached. */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_LoadMemFromFile */

void MM_AppPipe_Test_DumpMemToFile(void)
{
    MM_DumpMemToFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpMemToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_MEM_TO_FILE_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    /* Generates 1 event message in subfunction MM_DumpMemToFileCmd.  Event message count = 1 verifies that the subfunction has been reached. */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_DumpMemToFile */

void MM_AppPipe_Test_DumpInEvent(void)
{
    MM_DumpInEventCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_DumpInEventCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DUMP_IN_EVENT_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    /* Generates 1 event message in subfunction MM_DumpInEventCmd.  Event message count = 1 verifies that the subfunction has been reached. */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_DumpInEvent */

void MM_AppPipe_Test_FillMem(void)
{
    MM_FillMemCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_FillMemCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_FILL_MEM_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    /* Generates 1 event message in subfunction MM_FillMemCmd.  Event message count = 1 verifies that the subfunction has been reached. */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_FillMem */

void MM_AppPipe_Test_LookupSymbol(void)
{
    MM_LookupSymCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LookupSymCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOOKUP_SYM_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    /* Generates 1 event message in subfunction MM_LookupSymbolCmd.  Event message count = 1 verifies that the subfunction has been reached. */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_LookupSymbol */

void MM_AppPipe_Test_SymTblToFile(void)
{
    MM_SymTblToFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_SymTblToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_SYMTBL_TO_FILE_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    /* Generates 1 event message in subfunction MM_SymTblToFileCmd.  Event message count = 1 verifies that the subfunction has been reached. */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_SymTblToFile */

void MM_AppPipe_Test_EnableEepromWrite(void)
{
    MM_EepromWriteEnaCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_EepromWriteEnaCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_ENABLE_EEPROM_WRITE_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    /* Generates 1 event message in subfunction MM_EepromWriteEnaCmd.  Event message count = 1 verifies that the subfunction has been reached. */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_EnableEepromWrite */

void MM_AppPipe_Test_DisableEepromWrite(void)
{
    MM_EepromWriteDisCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_EepromWriteDisCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DISABLE_EEPROM_WRITE_CC);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    /* Generates 1 event message in subfunction MM_EepromWriteDisCmd.  Event message count = 1 verifies that the subfunction has been reached. */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_DisableEepromWrite */

void MM_AppPipe_Test_InvalidCommandCode(void)
{
    MM_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 99);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_CC1_ERR_EID, CFE_EVS_ERROR, "Invalid ground command code: ID = 0x1888, CC = 99"),
        "Invalid ground command code: ID = 0x1888, CC = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_InvalidCommandCode */

void MM_AppPipe_Test_InvalidCommandPipeMessageID(void)
{
    MM_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, 15, sizeof(MM_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    MM_AppPipe((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_MID_ERR_EID, CFE_EVS_ERROR, "Invalid command pipe message ID: 0xF"),
        "Invalid command pipe message ID: 0xF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_AppPipe_Test_InvalidCommandPipeMessageID */

void MM_HousekeepingCmd_Test(void)
{
    MM_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_HK_TLM_MID, sizeof(MM_NoArgsCmd_t), TRUE);

    MM_AppData.CmdCounter = 1;
    MM_AppData.ErrCounter = 2;
    MM_AppData.LastAction = 3;
    MM_AppData.MemType = 4;
    MM_AppData.Address = 5;
    MM_AppData.DataValue = 6;
    MM_AppData.BytesProcessed = 7;

    strncpy(MM_AppData.FileName, "name", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    MM_HousekeepingCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.HkPacket.CmdCounter == 1, "MM_AppData.HkPacket.CmdCounter == 1");
    UtAssert_True (MM_AppData.HkPacket.ErrCounter == 2, "MM_AppData.HkPacket.ErrCounter == 2");
    UtAssert_True (MM_AppData.HkPacket.LastAction == 3, "MM_AppData.HkPacket.LastAction == 3");
    UtAssert_True (MM_AppData.HkPacket.MemType == 4, "MM_AppData.HkPacket.MemType == 4");
    UtAssert_True (MM_AppData.HkPacket.Address == 5, "MM_AppData.HkPacket.Address == 5");
    UtAssert_True (MM_AppData.HkPacket.DataValue == 6, "MM_AppData.HkPacket.DataValue == 6");
    UtAssert_True (MM_AppData.HkPacket.BytesProcessed == 7, "MM_AppData.HkPacket.BytesProcessed == 7");
    
    UtAssert_True
        (strncmp(MM_AppData.HkPacket.FileName, MM_AppData.FileName, OS_MAX_PATH_LEN) == 0,
        "strncmp(MM_AppData.HkPacket.FileName, MM_AppData.FileName, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end MM_HousekeepingCmd_Test */

void MM_LookupSymbolCmd_Test_Nominal(void)
{
    MM_LookupSymCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LookupSymCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOOKUP_SYM_CC);

    strncpy(CmdPacket.SymName, "name", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    MM_LookupSymbolCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.LastAction == MM_SYM_LOOKUP, "MM_AppData.LastAction == MM_SYM_LOOKUP");
    UtAssert_True (MM_AppData.Address == 0, "MM_AppData.Address == 0");
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYM_LOOKUP_INF_EID, CFE_EVS_INFORMATION, "Symbol Lookup Command: Name = 'name' Addr = 0x00000000"),
        "Symbol Lookup Command: Name = 'name' Addr = 0x00000000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LookupSymbolCmd_Test_Nominal */

void MM_LookupSymbolCmd_Test_SymbolNameNull(void)
{
    MM_LookupSymCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LookupSymCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOOKUP_SYM_CC);

    strncpy(CmdPacket.SymName, "", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    MM_LookupSymbolCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMNAME_NUL_ERR_EID, CFE_EVS_ERROR, "NUL (empty) string specified as symbol name"),
        "NUL (empty) string specified as symbol name");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LookupSymbolCmd_Test_SymbolNameNull */

void MM_LookupSymbolCmd_Test_SymbolLookupError(void)
{
    MM_LookupSymCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_LookupSymCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_LOOKUP_SYM_CC);

    strncpy(CmdPacket.SymName, "name", OS_MAX_PATH_LEN);

    /* Set to generate error message MM_SYMNAME_ERR_EID */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLLOOKUP_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_LookupSymbolCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMNAME_ERR_EID, CFE_EVS_ERROR, "Symbolic address can't be resolved: Name = 'name'"),
        "Symbolic address can't be resolved: Name = 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_LookupSymbolCmd_Test_SymbolLookupError */

void MM_SymTblToFileCmd_Test_Nominal(void)
{
    MM_SymTblToFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_SymTblToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_SYMTBL_TO_FILE_CC);

    strncpy(CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "OS_Status == OS_SUCCESS" */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLTABLEDUMP_INDEX, CFE_SUCCESS, 1);

    /* Execute the function being tested */
    MM_SymTblToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.LastAction == MM_SYMTBL_SAVE, "MM_AppData.LastAction == MM_SYMTBL_SAVE");
    UtAssert_True (strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0, "strncmp(MM_AppData.FileName, CmdPacket.FileName, OS_MAX_PATH_LEN) == 0");
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMTBL_TO_FILE_INF_EID, CFE_EVS_INFORMATION, "Symbol Table Dump to File Started: Name = 'name'"),
        "Symbol Table Dump to File Started: Name = 'name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_SymTblToFileCmd_Test_Nominal */

void MM_SymTblToFileCmd_Test_SymbolFilenameNull(void)
{
    MM_SymTblToFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_SymTblToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_SYMTBL_TO_FILE_CC);

    strncpy(CmdPacket.FileName, "", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    MM_SymTblToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMFILENAME_NUL_ERR_EID, CFE_EVS_ERROR, "NUL (empty) string specified as symbol dump file name"),
        "NUL (empty) string specified as symbol dump file name");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_SymTblToFileCmd_Test_SymbolFilenameNull */

void MM_SymTblToFileCmd_Test_SymbolTableDumpError(void)
{
    MM_SymTblToFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_SymTblToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_SYMTBL_TO_FILE_CC);

    strncpy(CmdPacket.FileName, "name", OS_MAX_PATH_LEN);

    /* Set to generate error message MM_SYMTBL_TO_FILE_FAIL_ERR_EID */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_SYMBOLTABLEDUMP_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_SymTblToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMTBL_TO_FILE_FAIL_ERR_EID, CFE_EVS_ERROR, "Error dumping symbol table, OS_Status= 0xFFFFFFFF, File='name'"),
        "Error dumping symbol table, OS_Status= 0xFFFFFFFF, File='name'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_SymTblToFileCmd_Test_SymbolTableDumpError */

void MM_SymTblToFileCmd_Test_IllegalCharacters(void)
{
    MM_SymTblToFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_SymTblToFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_SYMTBL_TO_FILE_CC);

    strncpy(CmdPacket.FileName, "***name***", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    MM_SymTblToFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_SYMTBL_TO_FILE_INVALID_ERR_EID, CFE_EVS_ERROR, "Illegal characters in target filename, File='***name***'"),
        "Illegal characters in target filename, File='***name***'");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_SymTblToFileCmd_Test_IllegalCharacters */

void MM_EepromWriteEnaCmd_Test_Nominal(void)
{
    MM_EepromWriteEnaCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_EepromWriteEnaCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_ENABLE_EEPROM_WRITE_CC);

    CmdPacket.Bank = 1;

    /* Execute the function being tested */
    MM_EepromWriteEnaCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.LastAction == MM_EEPROMWRITE_ENA, "MM_AppData.LastAction == MM_EEPROMWRITE_ENA");
    UtAssert_True (MM_AppData.MemType == MM_EEPROM, "MM_AppData.MemType == MM_EEPROM");
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_EEPROM_WRITE_ENA_INF_EID, CFE_EVS_INFORMATION, "EEPROM bank 1 write enabled, cFE_Status= 0x0"),
        "EEPROM bank 1 write enabled, cFE_Status= 0x0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_EepromWriteEnaCmd_Test_Nominal */

void MM_EepromWriteEnaCmd_Test_Error(void)
{
    MM_EepromWriteEnaCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_EepromWriteEnaCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_ENABLE_EEPROM_WRITE_CC);

    CmdPacket.Bank = 1;

    /* Set to generate error message MM_EEPROM_WRITE_ENA_ERR_EID */
    Ut_CFE_PSP_EEPROM_SetReturnCode(UT_CFE_PSP_EEPROM_WRITEENABLE_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_EepromWriteEnaCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_EEPROM_WRITE_ENA_ERR_EID, CFE_EVS_ERROR, "Error requesting EEPROM bank 1 write enable, cFE_Status= 0xF"),
        "Error requesting EEPROM bank 1 write enable, cFE_Status= 0xF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_EepromWriteEnaCmd_Test_Error */

void MM_EepromWriteDisCmd_Test_Nominal(void)
{
    MM_EepromWriteDisCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_EepromWriteDisCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DISABLE_EEPROM_WRITE_CC);

    CmdPacket.Bank = 1;

    /* Execute the function being tested */
    MM_EepromWriteDisCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.LastAction == MM_EEPROMWRITE_DIS, "MM_AppData.LastAction == MM_EEPROMWRITE_DIS");
    UtAssert_True (MM_AppData.MemType == MM_EEPROM, "MM_AppData.MemType == MM_EEPROM");
    UtAssert_True (MM_AppData.CmdCounter == 1, "MM_AppData.CmdCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_EEPROM_WRITE_DIS_INF_EID, CFE_EVS_INFORMATION, "EEPROM bank 1 write disabled, cFE_Status= 0x0"),
        "EEPROM bank 1 write disabled, cFE_Status= 0x0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_EepromWriteDisCmd_Test_Nominal */

void MM_EepromWriteDisCmd_Test_Error(void)
{
    MM_EepromWriteDisCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, MM_CMD_MID, sizeof(MM_EepromWriteDisCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, MM_DISABLE_EEPROM_WRITE_CC);

    CmdPacket.Bank = 1;

    /* Set to generate error message MM_EEPROM_WRITE_DIS_ERR_EID */
    Ut_CFE_PSP_EEPROM_SetReturnCode(UT_CFE_PSP_EEPROM_WRITEDISABLE_INDEX, -1, 1);

    /* Execute the function being tested */
    MM_EepromWriteDisCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True (MM_AppData.ErrCounter == 1, "MM_AppData.ErrCounter == 1");
    
    UtAssert_True
        (Ut_CFE_EVS_EventSent(MM_EEPROM_WRITE_DIS_ERR_EID, CFE_EVS_ERROR, "Error requesting EEPROM bank 1 write disable, cFE_Status= 0xF"),
        "Error requesting EEPROM bank 1 write edisable, cFE_Status= 0xF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end MM_EepromWriteDisCmd_Test_Error */

void MM_App_Test_AddTestCases(void)
{
    UtTest_Add(MM_AppMain_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_AppMain_Test_Nominal");
    UtTest_Add(MM_AppMain_Test_AppInitError, MM_Test_Setup, MM_Test_TearDown, "MM_AppMain_Test_AppInitError");
    UtTest_Add(MM_AppMain_Test_SBError, MM_Test_Setup, MM_Test_TearDown, "MM_AppMain_Test_SBError");
    UtTest_Add(MM_AppInit_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_AppInit_Test_Nominal");
    UtTest_Add(MM_AppInit_Test_EVSRegisterError, MM_Test_Setup, MM_Test_TearDown, "MM_AppInit_Test_EVSRegisterError");
    UtTest_Add(MM_AppInit_Test_SBCreatePipeError, MM_Test_Setup, MM_Test_TearDown, "MM_AppInit_Test_SBCreatePipeError");
    UtTest_Add(MM_AppInit_Test_SBSubscribeHKError, MM_Test_Setup, MM_Test_TearDown, "MM_AppInit_Test_SBSubscribeHKError");
    UtTest_Add(MM_AppInit_Test_SBSubscribeMMError, MM_Test_Setup, MM_Test_TearDown, "MM_AppInit_Test_SBSubscribeMMError");
    UtTest_Add(MM_AppPipe_Test_Noop, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_Noop");
    UtTest_Add(MM_AppPipe_Test_Reset, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_Reset");
    UtTest_Add(MM_AppPipe_Test_Peek, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_Peek");
    UtTest_Add(MM_AppPipe_Test_Poke, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_Poke");
    UtTest_Add(MM_AppPipe_Test_LoadMemWID, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_LoadMemWID");
    UtTest_Add(MM_AppPipe_Test_LoadMemFromFile, MM_Test_Setup, MM_Test_TearDown, "LoadMemFromFile");
    UtTest_Add(MM_AppPipe_Test_DumpMemToFile, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_DumpMemToFile");
    UtTest_Add(MM_AppPipe_Test_DumpInEvent, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_DumpInEvent");
    UtTest_Add(MM_AppPipe_Test_FillMem, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_FillMem");
    UtTest_Add(MM_AppPipe_Test_LookupSymbol, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_LookupSymbol");
    UtTest_Add(MM_AppPipe_Test_SymTblToFile, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_SymTblToFile");
    UtTest_Add(MM_AppPipe_Test_EnableEepromWrite, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_EnableEepromWrite");
    UtTest_Add(MM_AppPipe_Test_DisableEepromWrite, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_DisableEepromWrite");
    UtTest_Add(MM_AppPipe_Test_InvalidCommandCode, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_InvalidCommandCode");
    UtTest_Add(MM_AppPipe_Test_InvalidCommandPipeMessageID, MM_Test_Setup, MM_Test_TearDown, "MM_AppPipe_Test_InvalidCommandPipeMessageID");
    UtTest_Add(MM_HousekeepingCmd_Test, MM_Test_Setup, MM_Test_TearDown, "MM_HousekeepingCmd_Test");
    UtTest_Add(MM_LookupSymbolCmd_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_LookupSymbolCmd_Test_Nominal");
    UtTest_Add(MM_LookupSymbolCmd_Test_SymbolNameNull, MM_Test_Setup, MM_Test_TearDown, "MM_LookupSymbolCmd_Test_SymbolNameNull");
    UtTest_Add(MM_LookupSymbolCmd_Test_SymbolLookupError, MM_Test_Setup, MM_Test_TearDown, "MM_LookupSymbolCmd_Test_SymbolLookupError");
    UtTest_Add(MM_SymTblToFileCmd_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_SymTblToFileCmd_Test_Nominal");
    UtTest_Add(MM_SymTblToFileCmd_Test_SymbolFilenameNull, MM_Test_Setup, MM_Test_TearDown, "MM_SymTblToFileCmd_Test_SymbolFilenameNull");
    UtTest_Add(MM_SymTblToFileCmd_Test_SymbolTableDumpError, MM_Test_Setup, MM_Test_TearDown, "MM_SymTblToFileCmd_Test_SymbolTableDumpError");
    UtTest_Add(MM_SymTblToFileCmd_Test_IllegalCharacters, MM_Test_Setup, MM_Test_TearDown, "MM_SymTblToFileCmd_Test_IllegalCharacters");
    UtTest_Add(MM_EepromWriteEnaCmd_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_EepromWriteEnaCmd_Test_Nominal");
    UtTest_Add(MM_EepromWriteEnaCmd_Test_Error, MM_Test_Setup, MM_Test_TearDown, "MM_EepromWriteEnaCmd_Test_Error");
    UtTest_Add(MM_EepromWriteDisCmd_Test_Nominal, MM_Test_Setup, MM_Test_TearDown, "MM_EepromWriteDisCmd_Test_Nominal");
    UtTest_Add(MM_EepromWriteDisCmd_Test_Error, MM_Test_Setup, MM_Test_TearDown, "MM_EepromWriteDisCmd_Test_Error");
} /* end MM_App_Test_AddTestCases */


/************************/
/*  End of File Comment */
/************************/
