 /*************************************************************************
 ** File:
 **   $Id: fm_app_test.c 1.6.1.2 2017/01/23 21:53:25EST sstrege Exp  $
 **
 **  Copyright (c) 2007-2014 United States Government as represented by the 
 **  Administrator of the National Aeronautics and Space Administration. 
 **  All Other Rights Reserved.  
 **
 **  This software was created at NASA's Goddard Space Flight Center.
 **  This software is governed by the NASA Open Source Agreement and may be 
 **  used, distributed and modified only pursuant to the terms of that 
 **  agreement.
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file fm_app.c
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 *************************************************************************/

/*
 * Includes
 */

#include "fm_app_test.h"
#include "fm_app.h"
#include "fm_msg.h"
#include "fm_msgdefs.h"
#include "fm_msgids.h"
#include "fm_events.h"
#include "fm_version.h"
#include "fm_test_utils.h"
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

void FM_AppMain_Test_Nominal(void)
{
    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to prevent call to CFE_SB_RcvMsg from returning an error */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(FM_NoopCmd_t), 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, FM_SEND_HK_MID, 1);

    /* Execute the function being tested */
    FM_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_EXIT_ERR_EID, CFE_EVS_ERROR, "Application terminating: result = 0x00000000"),
        "Application terminating: result = 0x00000000");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("FM application terminating: result = 0x00000000\n"),
        "FM application terminating: result = 0x00000000\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end FM_AppMain_Test_Nominal */

void FM_AppMain_Test_RegisterAppError(void)
{
    /* Set to satisfy condition "Result != CFE_SUCCESS", to generate an error */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERAPP_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_EXIT_ERR_EID, CFE_EVS_ERROR, "Application terminating: result = 0xFFFFFFFF"),
        "Application terminating: result = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("FM application terminating: result = 0xFFFFFFFF\n"),
        "FM application terminating: result = 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end FM_AppMain_Test_RegisterAppError */

void FM_AppMain_Test_AppInitError(void)
{
    /* Set to cause FM_AppInit to return -1, to generate an error */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_EXIT_ERR_EID, CFE_EVS_ERROR, "Application terminating: result = 0xFFFFFFFF"),
        "Application terminating: result = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");
    /* Generates 1 event message we don't care about in this test */

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("FM application terminating: result = 0xFFFFFFFF\n"),
        "FM application terminating: result = 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end FM_AppMain_Test_AppInitError */

void FM_AppMain_Test_SBError(void)
{
    /* Set to exit loop after first run */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Set to generate error message FM_SB_RECEIVE_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, sizeof(FM_NoopCmd_t), 1);

    /* Set to prevent segmentation fault */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, FM_SEND_HK_MID, 1);

    /* Execute the function being tested */
    FM_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_SB_RECEIVE_ERR_EID, CFE_EVS_ERROR, "Main loop error: SB receive: result = 0xFFFFFFFF"),
        "Main loop error: SB receive: result = 0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_EXIT_ERR_EID, CFE_EVS_ERROR, "Application terminating: result = 0xFFFFFFFF"),
        "Application terminating: result = 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 3, "Ut_CFE_EVS_GetEventQueueDepth() == 3");
    /* Generates 1 event message we don't care about in this test */

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("FM application terminating: result = 0xFFFFFFFF\n"),
        "FM application terminating: result = 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_ES_GetSysLogQueueDepth() == 1, "Ut_CFE_ES_GetSysLogQueueDepth() == 1");

} /* end FM_AppMain_Test_SBError */

void FM_AppInit_Test_Nominal(void)
{
    int32  Result;
    char   Message[125];

    CFE_PSP_MemSet(&FM_GlobalData, 1, sizeof(FM_GlobalData_t));

    /* Execute the function being tested */
    Result = FM_AppInit();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    /* Not verifying that FM_GlobalData.FreeSpaceTablePtr is set to 0 when FM_GlobalData is set to 0, because it's 
       modified by a subfunction, which we're not testing here */
    UtAssert_True (FM_GlobalData.FreeSpaceTableHandle == 0, "FM_GlobalData.FreeSpaceTableHandle == 0");
    UtAssert_True (FM_GlobalData.CmdPipe == 0, "FM_GlobalData.CmdPipe == 0");

    UtAssert_True (FM_GlobalData.ChildTaskID == 0, "FM_GlobalData.ChildTaskID == 0");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 0, "FM_GlobalData.ChildCmdCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 0, "FM_GlobalData.ChildCmdErrCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 0, "FM_GlobalData.ChildCmdWarnCounter == 0");
    UtAssert_True (FM_GlobalData.ChildWriteIndex == 0, "FM_GlobalData.ChildWriteIndex == 0");
    UtAssert_True (FM_GlobalData.ChildReadIndex == 0, "FM_GlobalData.ChildReadIndex == 0");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");
    UtAssert_True (FM_GlobalData.CommandCounter == 0, "FM_GlobalData.CommandCounter == 0");
    UtAssert_True (FM_GlobalData.CommandErrCounter == 0, "FM_GlobalData.CommandErrCounter == 0");
    UtAssert_True (FM_GlobalData.Spare8a == 0, "FM_GlobalData.Spare8a == 0");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 0, "FM_GlobalData.ChildPreviousCC == 0");
    UtAssert_True (FM_GlobalData.Spare8b == 0, "FM_GlobalData.Spare8b == 0");
    UtAssert_True (FM_GlobalData.FileStatTime == 0, "FM_GlobalData.FileStatTime == 0");
    UtAssert_True (FM_GlobalData.FileStatSize == 0, "FM_GlobalData.FileStatSize == 0");

    FM_DirListFileStats_t DirListFileStats;
    FM_DirListPkt_t DirListPkt;
    FM_FreeSpacePkt_t FreeSpacePkt;
    FM_FileInfoPkt_t FileInfoPkt;
    FM_OpenFilesPkt_t OpenFilesPkt;
    FM_HousekeepingPkt_t HousekeepingPkt;

    memset(&DirListFileStats, 0, sizeof(FM_DirListFileStats_t));
    memset(&DirListPkt, 0, sizeof(FM_DirListPkt_t));
    memset(&FreeSpacePkt, 0, sizeof(FM_FreeSpacePkt_t));
    memset(&FileInfoPkt, 0, sizeof(FM_FileInfoPkt_t));
    memset(&OpenFilesPkt, 0, sizeof(FM_OpenFilesPkt_t));
    memset(&HousekeepingPkt, 0, sizeof(FM_HousekeepingPkt_t));

    UtAssert_True (memcmp(&FM_GlobalData.DirListFileStats, &DirListFileStats, sizeof(FM_DirListFileStats_t)) == 0, "FM_GlobalData.DirListFileStats == 0");
    UtAssert_True (memcmp(&FM_GlobalData.DirListPkt, &DirListPkt, sizeof(FM_DirListPkt_t)) == 0, "FM_GlobalData.DirListPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FreeSpacePkt, &FreeSpacePkt, sizeof(FM_FreeSpacePkt_t)) == 0, "FM_GlobalData.FreeSpacePkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FileInfoPkt, &FileInfoPkt, sizeof(FM_FileInfoPkt_t)) == 0, "FM_GlobalData.FileInfoPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.OpenFilesPkt, &OpenFilesPkt, sizeof(FM_OpenFilesPkt_t)) == 0, "FM_GlobalData.OpenFilesPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.HousekeepingPkt, &HousekeepingPkt, sizeof(FM_HousekeepingPkt_t)) == 0, "FM_GlobalData.HousekeepingPkt == 0");
    /* Not verifying that FM_GlobalData.ChildBuffer is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */
    /* Not verifying that FM_GlobalData.ChildQueue is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */

    sprintf(Message, "Initialization complete: version %d.%d.%d.%d", FM_MAJOR_VERSION, FM_MINOR_VERSION, FM_REVISION, FM_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(FM_STARTUP_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_AppInit_Test_Nominal */

void FM_AppInit_Test_EVSRegisterError(void)
{
    int32 Result;

    CFE_PSP_MemSet(&FM_GlobalData, 1, sizeof(FM_GlobalData_t));

    /* Set to generate error message FM_STARTUP_EVENTS_ERR_EID */
    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_AppInit();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    /* Not verifying that FM_GlobalData.FreeSpaceTablePtr is set to 0 when FM_GlobalData is set to 0, because it's 
       modified by a subfunction, which we're not testing here */
    UtAssert_True (FM_GlobalData.FreeSpaceTableHandle == 0, "FM_GlobalData.FreeSpaceTableHandle == 0");
    UtAssert_True (FM_GlobalData.CmdPipe == 0, "FM_GlobalData.CmdPipe == 0");

    UtAssert_True (FM_GlobalData.ChildTaskID == 0, "FM_GlobalData.ChildTaskID == 0");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 0, "FM_GlobalData.ChildCmdCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 0, "FM_GlobalData.ChildCmdErrCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 0, "FM_GlobalData.ChildCmdWarnCounter == 0");
    UtAssert_True (FM_GlobalData.ChildWriteIndex == 0, "FM_GlobalData.ChildWriteIndex == 0");
    UtAssert_True (FM_GlobalData.ChildReadIndex == 0, "FM_GlobalData.ChildReadIndex == 0");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");
    UtAssert_True (FM_GlobalData.CommandCounter == 0, "FM_GlobalData.CommandCounter == 0");
    UtAssert_True (FM_GlobalData.CommandErrCounter == 0, "FM_GlobalData.CommandErrCounter == 0");
    UtAssert_True (FM_GlobalData.Spare8a == 0, "FM_GlobalData.Spare8a == 0");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 0, "FM_GlobalData.ChildPreviousCC == 0");
    UtAssert_True (FM_GlobalData.Spare8b == 0, "FM_GlobalData.Spare8b == 0");
    UtAssert_True (FM_GlobalData.FileStatTime == 0, "FM_GlobalData.FileStatTime == 0");
    UtAssert_True (FM_GlobalData.FileStatSize == 0, "FM_GlobalData.FileStatSize == 0");

    FM_DirListFileStats_t DirListFileStats;
    FM_DirListPkt_t DirListPkt;
    FM_FreeSpacePkt_t FreeSpacePkt;
    FM_FileInfoPkt_t FileInfoPkt;
    FM_OpenFilesPkt_t OpenFilesPkt;
    FM_HousekeepingPkt_t HousekeepingPkt;

    memset(&DirListFileStats, 0, sizeof(FM_DirListFileStats_t));
    memset(&DirListPkt, 0, sizeof(FM_DirListPkt_t));
    memset(&FreeSpacePkt, 0, sizeof(FM_FreeSpacePkt_t));
    memset(&FileInfoPkt, 0, sizeof(FM_FileInfoPkt_t));
    memset(&OpenFilesPkt, 0, sizeof(FM_OpenFilesPkt_t));
    memset(&HousekeepingPkt, 0, sizeof(FM_HousekeepingPkt_t));

    UtAssert_True (memcmp(&FM_GlobalData.DirListFileStats, &DirListFileStats, sizeof(FM_DirListFileStats_t)) == 0, "FM_GlobalData.DirListFileStats == 0");
    UtAssert_True (memcmp(&FM_GlobalData.DirListPkt, &DirListPkt, sizeof(FM_DirListPkt_t)) == 0, "FM_GlobalData.DirListPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FreeSpacePkt, &FreeSpacePkt, sizeof(FM_FreeSpacePkt_t)) == 0, "FM_GlobalData.FreeSpacePkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FileInfoPkt, &FileInfoPkt, sizeof(FM_FileInfoPkt_t)) == 0, "FM_GlobalData.FileInfoPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.OpenFilesPkt, &OpenFilesPkt, sizeof(FM_OpenFilesPkt_t)) == 0, "FM_GlobalData.OpenFilesPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.HousekeepingPkt, &HousekeepingPkt, sizeof(FM_HousekeepingPkt_t)) == 0, "FM_GlobalData.HousekeepingPkt == 0");
    /* Not verifying that FM_GlobalData.ChildBuffer is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */
    /* Not verifying that FM_GlobalData.ChildQueue is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_STARTUP_EVENTS_ERR_EID, CFE_EVS_ERROR, "Initialization error: register for event services: result = 0xFFFFFFFF"),
        "Initialization error: register for event services: result = 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_AppInit_Test_EVSRegisterError */

void FM_AppInit_Test_SBCreatePipeError(void)
{
    int32 Result;

    CFE_PSP_MemSet(&FM_GlobalData, 1, sizeof(FM_GlobalData_t));

    /* Set to generate error message FM_STARTUP_CREAT_PIPE_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_AppInit();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    /* Not verifying that FM_GlobalData.FreeSpaceTablePtr is set to 0 when FM_GlobalData is set to 0, because it's 
       modified by a subfunction, which we're not testing here */
    UtAssert_True (FM_GlobalData.FreeSpaceTableHandle == 0, "FM_GlobalData.FreeSpaceTableHandle == 0");
    UtAssert_True (FM_GlobalData.CmdPipe == 0, "FM_GlobalData.CmdPipe == 0");

    UtAssert_True (FM_GlobalData.ChildTaskID == 0, "FM_GlobalData.ChildTaskID == 0");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 0, "FM_GlobalData.ChildCmdCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 0, "FM_GlobalData.ChildCmdErrCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 0, "FM_GlobalData.ChildCmdWarnCounter == 0");
    UtAssert_True (FM_GlobalData.ChildWriteIndex == 0, "FM_GlobalData.ChildWriteIndex == 0");
    UtAssert_True (FM_GlobalData.ChildReadIndex == 0, "FM_GlobalData.ChildReadIndex == 0");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");
    UtAssert_True (FM_GlobalData.CommandCounter == 0, "FM_GlobalData.CommandCounter == 0");
    UtAssert_True (FM_GlobalData.CommandErrCounter == 0, "FM_GlobalData.CommandErrCounter == 0");
    UtAssert_True (FM_GlobalData.Spare8a == 0, "FM_GlobalData.Spare8a == 0");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 0, "FM_GlobalData.ChildPreviousCC == 0");
    UtAssert_True (FM_GlobalData.Spare8b == 0, "FM_GlobalData.Spare8b == 0");
    UtAssert_True (FM_GlobalData.FileStatTime == 0, "FM_GlobalData.FileStatTime == 0");
    UtAssert_True (FM_GlobalData.FileStatSize == 0, "FM_GlobalData.FileStatSize == 0");

    FM_DirListFileStats_t DirListFileStats;
    FM_DirListPkt_t DirListPkt;
    FM_FreeSpacePkt_t FreeSpacePkt;
    FM_FileInfoPkt_t FileInfoPkt;
    FM_OpenFilesPkt_t OpenFilesPkt;
    FM_HousekeepingPkt_t HousekeepingPkt;

    memset(&DirListFileStats, 0, sizeof(FM_DirListFileStats_t));
    memset(&DirListPkt, 0, sizeof(FM_DirListPkt_t));
    memset(&FreeSpacePkt, 0, sizeof(FM_FreeSpacePkt_t));
    memset(&FileInfoPkt, 0, sizeof(FM_FileInfoPkt_t));
    memset(&OpenFilesPkt, 0, sizeof(FM_OpenFilesPkt_t));
    memset(&HousekeepingPkt, 0, sizeof(FM_HousekeepingPkt_t));

    UtAssert_True (memcmp(&FM_GlobalData.DirListFileStats, &DirListFileStats, sizeof(FM_DirListFileStats_t)) == 0, "FM_GlobalData.DirListFileStats == 0");
    UtAssert_True (memcmp(&FM_GlobalData.DirListPkt, &DirListPkt, sizeof(FM_DirListPkt_t)) == 0, "FM_GlobalData.DirListPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FreeSpacePkt, &FreeSpacePkt, sizeof(FM_FreeSpacePkt_t)) == 0, "FM_GlobalData.FreeSpacePkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FileInfoPkt, &FileInfoPkt, sizeof(FM_FileInfoPkt_t)) == 0, "FM_GlobalData.FileInfoPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.OpenFilesPkt, &OpenFilesPkt, sizeof(FM_OpenFilesPkt_t)) == 0, "FM_GlobalData.OpenFilesPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.HousekeepingPkt, &HousekeepingPkt, sizeof(FM_HousekeepingPkt_t)) == 0, "FM_GlobalData.HousekeepingPkt == 0");
    /* Not verifying that FM_GlobalData.ChildBuffer is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */
    /* Not verifying that FM_GlobalData.ChildQueue is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_STARTUP_CREAT_PIPE_ERR_EID, CFE_EVS_ERROR, "Initialization error: create SB input pipe: result = 0xFFFFFFFF"),
        "Initialization error: create SB input pipe: result = 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_AppInit_Test_SBCreatePipeError */

void FM_AppInit_Test_SBSubscribeHKError(void)
{
    int32 Result;

    CFE_PSP_MemSet(&FM_GlobalData, 1, sizeof(FM_GlobalData_t));

    /* Set to generate error message FM_STARTUP_SUBSCRIB_HK_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_AppInit();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    /* Not verifying that FM_GlobalData.FreeSpaceTablePtr is set to 0 when FM_GlobalData is set to 0, because it's 
       modified by a subfunction, which we're not testing here */
    UtAssert_True (FM_GlobalData.FreeSpaceTableHandle == 0, "FM_GlobalData.FreeSpaceTableHandle == 0");
    UtAssert_True (FM_GlobalData.CmdPipe == 0, "FM_GlobalData.CmdPipe == 0");

    UtAssert_True (FM_GlobalData.ChildTaskID == 0, "FM_GlobalData.ChildTaskID == 0");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 0, "FM_GlobalData.ChildCmdCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 0, "FM_GlobalData.ChildCmdErrCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 0, "FM_GlobalData.ChildCmdWarnCounter == 0");
    UtAssert_True (FM_GlobalData.ChildWriteIndex == 0, "FM_GlobalData.ChildWriteIndex == 0");
    UtAssert_True (FM_GlobalData.ChildReadIndex == 0, "FM_GlobalData.ChildReadIndex == 0");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");
    UtAssert_True (FM_GlobalData.CommandCounter == 0, "FM_GlobalData.CommandCounter == 0");
    UtAssert_True (FM_GlobalData.CommandErrCounter == 0, "FM_GlobalData.CommandErrCounter == 0");
    UtAssert_True (FM_GlobalData.Spare8a == 0, "FM_GlobalData.Spare8a == 0");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 0, "FM_GlobalData.ChildPreviousCC == 0");
    UtAssert_True (FM_GlobalData.Spare8b == 0, "FM_GlobalData.Spare8b == 0");
    UtAssert_True (FM_GlobalData.FileStatTime == 0, "FM_GlobalData.FileStatTime == 0");
    UtAssert_True (FM_GlobalData.FileStatSize == 0, "FM_GlobalData.FileStatSize == 0");

    FM_DirListFileStats_t DirListFileStats;
    FM_DirListPkt_t DirListPkt;
    FM_FreeSpacePkt_t FreeSpacePkt;
    FM_FileInfoPkt_t FileInfoPkt;
    FM_OpenFilesPkt_t OpenFilesPkt;
    FM_HousekeepingPkt_t HousekeepingPkt;

    memset(&DirListFileStats, 0, sizeof(FM_DirListFileStats_t));
    memset(&DirListPkt, 0, sizeof(FM_DirListPkt_t));
    memset(&FreeSpacePkt, 0, sizeof(FM_FreeSpacePkt_t));
    memset(&FileInfoPkt, 0, sizeof(FM_FileInfoPkt_t));
    memset(&OpenFilesPkt, 0, sizeof(FM_OpenFilesPkt_t));
    memset(&HousekeepingPkt, 0, sizeof(FM_HousekeepingPkt_t));

    UtAssert_True (memcmp(&FM_GlobalData.DirListFileStats, &DirListFileStats, sizeof(FM_DirListFileStats_t)) == 0, "FM_GlobalData.DirListFileStats == 0");
    UtAssert_True (memcmp(&FM_GlobalData.DirListPkt, &DirListPkt, sizeof(FM_DirListPkt_t)) == 0, "FM_GlobalData.DirListPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FreeSpacePkt, &FreeSpacePkt, sizeof(FM_FreeSpacePkt_t)) == 0, "FM_GlobalData.FreeSpacePkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FileInfoPkt, &FileInfoPkt, sizeof(FM_FileInfoPkt_t)) == 0, "FM_GlobalData.FileInfoPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.OpenFilesPkt, &OpenFilesPkt, sizeof(FM_OpenFilesPkt_t)) == 0, "FM_GlobalData.OpenFilesPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.HousekeepingPkt, &HousekeepingPkt, sizeof(FM_HousekeepingPkt_t)) == 0, "FM_GlobalData.HousekeepingPkt == 0");
    /* Not verifying that FM_GlobalData.ChildBuffer is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */
    /* Not verifying that FM_GlobalData.ChildQueue is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_STARTUP_SUBSCRIB_HK_ERR_EID, CFE_EVS_ERROR, "Initialization error: subscribe to HK request: result = 0xFFFFFFFF"),
        "Initialization error: subscribe to HK request: result = 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_AppInit_Test_SBSubscribeHKError */

void FM_AppInit_Test_SBSubscribeFMError(void)
{
    int32 Result;

    CFE_PSP_MemSet(&FM_GlobalData, 1, sizeof(FM_GlobalData_t));

    /* Set to generate error message FM_STARTUP_SUBSCRIB_GCMD_ERR_EID */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = FM_AppInit();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    /* Not verifying that FM_GlobalData.FreeSpaceTablePtr is set to 0 when FM_GlobalData is set to 0, because it's 
       modified by a subfunction, which we're not testing here */
    UtAssert_True (FM_GlobalData.FreeSpaceTableHandle == 0, "FM_GlobalData.FreeSpaceTableHandle == 0");
    UtAssert_True (FM_GlobalData.CmdPipe == 0, "FM_GlobalData.CmdPipe == 0");

    UtAssert_True (FM_GlobalData.ChildTaskID == 0, "FM_GlobalData.ChildTaskID == 0");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 0, "FM_GlobalData.ChildCmdCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 0, "FM_GlobalData.ChildCmdErrCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 0, "FM_GlobalData.ChildCmdWarnCounter == 0");
    UtAssert_True (FM_GlobalData.ChildWriteIndex == 0, "FM_GlobalData.ChildWriteIndex == 0");
    UtAssert_True (FM_GlobalData.ChildReadIndex == 0, "FM_GlobalData.ChildReadIndex == 0");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");
    UtAssert_True (FM_GlobalData.CommandCounter == 0, "FM_GlobalData.CommandCounter == 0");
    UtAssert_True (FM_GlobalData.CommandErrCounter == 0, "FM_GlobalData.CommandErrCounter == 0");
    UtAssert_True (FM_GlobalData.Spare8a == 0, "FM_GlobalData.Spare8a == 0");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 0, "FM_GlobalData.ChildPreviousCC == 0");
    UtAssert_True (FM_GlobalData.Spare8b == 0, "FM_GlobalData.Spare8b == 0");
    UtAssert_True (FM_GlobalData.FileStatTime == 0, "FM_GlobalData.FileStatTime == 0");
    UtAssert_True (FM_GlobalData.FileStatSize == 0, "FM_GlobalData.FileStatSize == 0");

    FM_DirListFileStats_t DirListFileStats;
    FM_DirListPkt_t DirListPkt;
    FM_FreeSpacePkt_t FreeSpacePkt;
    FM_FileInfoPkt_t FileInfoPkt;
    FM_OpenFilesPkt_t OpenFilesPkt;
    FM_HousekeepingPkt_t HousekeepingPkt;

    memset(&DirListFileStats, 0, sizeof(FM_DirListFileStats_t));
    memset(&DirListPkt, 0, sizeof(FM_DirListPkt_t));
    memset(&FreeSpacePkt, 0, sizeof(FM_FreeSpacePkt_t));
    memset(&FileInfoPkt, 0, sizeof(FM_FileInfoPkt_t));
    memset(&OpenFilesPkt, 0, sizeof(FM_OpenFilesPkt_t));
    memset(&HousekeepingPkt, 0, sizeof(FM_HousekeepingPkt_t));

    UtAssert_True (memcmp(&FM_GlobalData.DirListFileStats, &DirListFileStats, sizeof(FM_DirListFileStats_t)) == 0, "FM_GlobalData.DirListFileStats == 0");
    UtAssert_True (memcmp(&FM_GlobalData.DirListPkt, &DirListPkt, sizeof(FM_DirListPkt_t)) == 0, "FM_GlobalData.DirListPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FreeSpacePkt, &FreeSpacePkt, sizeof(FM_FreeSpacePkt_t)) == 0, "FM_GlobalData.FreeSpacePkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FileInfoPkt, &FileInfoPkt, sizeof(FM_FileInfoPkt_t)) == 0, "FM_GlobalData.FileInfoPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.OpenFilesPkt, &OpenFilesPkt, sizeof(FM_OpenFilesPkt_t)) == 0, "FM_GlobalData.OpenFilesPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.HousekeepingPkt, &HousekeepingPkt, sizeof(FM_HousekeepingPkt_t)) == 0, "FM_GlobalData.HousekeepingPkt == 0");
    /* Not verifying that FM_GlobalData.ChildBuffer is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */
    /* Not verifying that FM_GlobalData.ChildQueue is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_STARTUP_SUBSCRIB_GCMD_ERR_EID, CFE_EVS_ERROR, "Initialization error: subscribe to FM commands: result = 0xFFFFFFFF"),
        "Initialization error: subscribe to FM commands: result = 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_AppInit_Test_SBSubscribeFMError */

void FM_AppInit_Test_TableInitError(void)
{
    int32 Result;

    CFE_PSP_MemSet(&FM_GlobalData, 1, sizeof(FM_GlobalData_t));

    /* Set to generate error message FM_STARTUP_TABLE_INIT_ERR_EID */
    Ut_CFE_TBL_SetReturnCode(UT_CFE_TBL_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_AppInit();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    UtAssert_True (FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildSemaphore == FM_CHILD_SEM_INVALID");
    UtAssert_True (FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID, "FM_GlobalData.ChildQueueCountSem == FM_CHILD_SEM_INVALID");

    /* Not verifying that FM_GlobalData.FreeSpaceTablePtr is set to 0 when FM_GlobalData is set to 0, because it's 
       modified by a subfunction, which we're not testing here */
    UtAssert_True (FM_GlobalData.FreeSpaceTableHandle == 0, "FM_GlobalData.FreeSpaceTableHandle == 0");
    UtAssert_True (FM_GlobalData.CmdPipe == 0, "FM_GlobalData.CmdPipe == 0");

    UtAssert_True (FM_GlobalData.ChildTaskID == 0, "FM_GlobalData.ChildTaskID == 0");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 0, "FM_GlobalData.ChildCmdCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 0, "FM_GlobalData.ChildCmdErrCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 0, "FM_GlobalData.ChildCmdWarnCounter == 0");
    UtAssert_True (FM_GlobalData.ChildWriteIndex == 0, "FM_GlobalData.ChildWriteIndex == 0");
    UtAssert_True (FM_GlobalData.ChildReadIndex == 0, "FM_GlobalData.ChildReadIndex == 0");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");
    UtAssert_True (FM_GlobalData.CommandCounter == 0, "FM_GlobalData.CommandCounter == 0");
    UtAssert_True (FM_GlobalData.CommandErrCounter == 0, "FM_GlobalData.CommandErrCounter == 0");
    UtAssert_True (FM_GlobalData.Spare8a == 0, "FM_GlobalData.Spare8a == 0");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 0, "FM_GlobalData.ChildPreviousCC == 0");
    UtAssert_True (FM_GlobalData.Spare8b == 0, "FM_GlobalData.Spare8b == 0");
    UtAssert_True (FM_GlobalData.FileStatTime == 0, "FM_GlobalData.FileStatTime == 0");
    UtAssert_True (FM_GlobalData.FileStatSize == 0, "FM_GlobalData.FileStatSize == 0");

    FM_DirListFileStats_t DirListFileStats;
    FM_DirListPkt_t DirListPkt;
    FM_FreeSpacePkt_t FreeSpacePkt;
    FM_FileInfoPkt_t FileInfoPkt;
    FM_OpenFilesPkt_t OpenFilesPkt;
    FM_HousekeepingPkt_t HousekeepingPkt;

    memset(&DirListFileStats, 0, sizeof(FM_DirListFileStats_t));
    memset(&DirListPkt, 0, sizeof(FM_DirListPkt_t));
    memset(&FreeSpacePkt, 0, sizeof(FM_FreeSpacePkt_t));
    memset(&FileInfoPkt, 0, sizeof(FM_FileInfoPkt_t));
    memset(&OpenFilesPkt, 0, sizeof(FM_OpenFilesPkt_t));
    memset(&HousekeepingPkt, 0, sizeof(FM_HousekeepingPkt_t));

    UtAssert_True (memcmp(&FM_GlobalData.DirListFileStats, &DirListFileStats, sizeof(FM_DirListFileStats_t)) == 0, "FM_GlobalData.DirListFileStats == 0");
    UtAssert_True (memcmp(&FM_GlobalData.DirListPkt, &DirListPkt, sizeof(FM_DirListPkt_t)) == 0, "FM_GlobalData.DirListPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FreeSpacePkt, &FreeSpacePkt, sizeof(FM_FreeSpacePkt_t)) == 0, "FM_GlobalData.FreeSpacePkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.FileInfoPkt, &FileInfoPkt, sizeof(FM_FileInfoPkt_t)) == 0, "FM_GlobalData.FileInfoPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.OpenFilesPkt, &OpenFilesPkt, sizeof(FM_OpenFilesPkt_t)) == 0, "FM_GlobalData.OpenFilesPkt == 0");
    UtAssert_True (memcmp(&FM_GlobalData.HousekeepingPkt, &HousekeepingPkt, sizeof(FM_HousekeepingPkt_t)) == 0, "FM_GlobalData.HousekeepingPkt == 0");
    /* Not verifying that FM_GlobalData.ChildBuffer is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */
    /* Not verifying that FM_GlobalData.ChildQueue is set to 0 when FM_GlobalData is set to 0, because it's modified by a subfunction, which we're not testing here */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_STARTUP_TABLE_INIT_ERR_EID, CFE_EVS_ERROR, "Initialization error: register free space table: result = 0xFFFFFFFF"),
        "Initialization error: register free space table: result = 0xFFFFFFFF");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_AppInit_Test_TableInitError */

void FM_ProcessPkt_Test_HK(void)
{
    FM_HousekeepingCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_SEND_HK_MID, sizeof(FM_HousekeepingCmd_t), TRUE);

    /* Execute the function being tested */
    FM_ProcessPkt((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Nothing to verify */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_ProcessPkt_Test_HK */

void FM_ProcessPkt_Test_Cmd(void)
{
    FM_NoopCmd_t   CmdPacket;
    char           Message[125];

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_NoopCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_NOOP_CC);

    /* Execute the function being tested */
    FM_ProcessPkt((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    sprintf(Message, "No-op command: FM version %d.%d.%d.%d", FM_MAJOR_VERSION, FM_MINOR_VERSION, FM_REVISION, FM_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(FM_NOOP_CMD_EID, CFE_EVS_INFORMATION, Message), Message);
    /* Note: The above event message occurs in subfunction FM_ProcessCmd (in its subfunction FM_NoopCmd).  We check for it here to verify that FM_ProcessCmd was reached. */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessPkt_Test_Cmd */

void FM_ProcessPkt_Test_Error(void)
{
    FM_NoopCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, 99, sizeof(FM_NoopCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_NOOP_CC);

    /* Execute the function being tested */
    FM_ProcessPkt((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_MID_ERR_EID, CFE_EVS_ERROR, "Main loop error: invalid message ID: mid = 0x0063"),
        "Main loop error: invalid message ID: mid = 0x0063");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessPkt_Test_Error */

void FM_ProcessCmd_Test_Noop(void)
{
    FM_NoopCmd_t   CmdPacket;
    char           Message[125];

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_NoopCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_NOOP_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    sprintf(Message, "No-op command: FM version %d.%d.%d.%d", FM_MAJOR_VERSION, FM_MINOR_VERSION, FM_REVISION, FM_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(FM_NOOP_CMD_EID, CFE_EVS_INFORMATION, Message), Message);
    /* Note: The above event message occurs in subfunction FM_NoopCmd.  We check for it here to verify that FM_NoopCmd was reached. */

    UtAssert_True (FM_GlobalData.CommandCounter == 1, "FM_GlobalData.CommandCounter == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_Noop */

void FM_ProcessCmd_Test_Reset(void)
{
    FM_ResetCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_ResetCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_RESET_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_RESET_CMD_EID, CFE_EVS_DEBUG, "Reset Counters command"),
        "Reset Counters command");
    /* Note: The above event message occurs in subfunction FM_ResetCountersCmd.  We check for it here to verify that FM_ResetCountersCmd was reached. */

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_Reset */

void FM_ProcessCmd_Test_Copy(void)
{
    FM_CopyFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_CopyFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_COPY_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_CopyFileCmd.  We check it here to verify that FM_CopyFileCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_Copy */

void FM_ProcessCmd_Test_Move(void)
{
    FM_MoveFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_MoveFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_MOVE_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_MoveFileCmd.  We check it here to verify that FM_MoveFileCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_Move */

void FM_ProcessCmd_Test_Rename(void)
{
    FM_RenameFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_RenameFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_RENAME_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_RenameFileCmd.  We check it here to verify that FM_RenameFileCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_Rename */

void FM_ProcessCmd_Test_Delete(void)
{
    FM_DeleteFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_DeleteFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_DELETE_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_DeleteFileCmd.  We check it here to verify that FM_DeleteFileCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_Delete */

void FM_ProcessCmd_Test_DeleteAll(void)
{
    FM_DeleteAllCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_DeleteAllCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_DELETE_ALL_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_DeleteAllFilesCmd.  We check it here to verify that FM_DeleteAllFilesCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_DeleteAll */

void FM_ProcessCmd_Test_Decompress(void)
{
    FM_DecompressCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_DecompressCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_DECOMPRESS_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_DecompressFileCmd.  We check it here to verify that FM_DecompressFileCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_Decompress */

void FM_ProcessCmd_Test_Concat(void)
{
    FM_ConcatCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_ConcatCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_CONCAT_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_ConcatFilesCmd.  We check it here to verify that FM_ConcatFilesCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_Concat */

void FM_ProcessCmd_Test_GetFileInfo(void)
{
    FM_GetFileInfoCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_FILE_INFO_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_GetFileInfoCmd.  We check it here to verify that FM_GetFileInfoCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_GetFileInfo */

void FM_ProcessCmd_Test_GetOpenFiles(void)
{
    FM_GetOpenFilesCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetOpenFilesCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_OPEN_FILES_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_OPEN_FILES_CMD_EID, CFE_EVS_DEBUG, "Get Open Files command"),
        "Get Open Files command");

    UtAssert_True (FM_GlobalData.CommandCounter == 1, "FM_GlobalData.CommandCounter == 1");
    /* Note: The above variable is modified in subfunction FM_GetOpenFilesCmd.  We check it here to verify that FM_GetOpenFilesCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_GetOpenFiles */

void FM_ProcessCmd_Test_CreateDir(void)
{
    FM_CreateDirCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_CreateDirCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_CREATE_DIR_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_CreateDirectoryCmd.  We check it here to verify that FM_CreateDirectoryCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_CreateDir */

void FM_ProcessCmd_Test_DeleteDir(void)
{
    FM_DeleteDirCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_DeleteDirCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_DELETE_DIR_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_DeleteDirectoryCmd.  We check it here to verify that FM_DeleteDirectoryCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_DeleteDir */

void FM_ProcessCmd_Test_GetDirFile(void)
{
    FM_GetDirFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetDirFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_DIR_FILE_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_GetDirListFileCmd.  We check it here to verify that FM_GetDirListFileCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_GetDirFile */

void FM_ProcessCmd_Test_GetDirPkt(void)
{
    FM_GetDirPktCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetDirPktCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_DIR_PKT_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_GetDirListPktCmd.  We check it here to verify that FM_GetDirListPktCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_GetDirPkt */

void FM_ProcessCmd_Test_GetFreeSpace(void)
{
    FM_GetFreeSpaceCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetFreeSpaceCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_FREE_SPACE_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_GetFreeSpaceCmd.  We check it here to verify that FM_GetFreeSpaceCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_GetFreeSpace */

void FM_ProcessCmd_Test_SetTableState(void)
{
    FM_SetTableStateCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_SET_TABLE_STATE_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_SetTableStateCmd.  We check it here to verify that FM_SetTableStateCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_SetTableState */


void FM_ProcessCmd_Test_SetPermissions(void)
{
    FM_SetPermCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_SetPermCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_SET_FILE_PERM_CC);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.CommandErrCounter == 1, "FM_GlobalData.CommandErrCounter == 1");
    /* Note: The above variable is modified in subfunction FM_SetPermissionsCmd.  We check it here to verify that FM_SetTableStateCmd was reached. */

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_SetTableState */

void FM_ProcessCmd_Test_DeleteInt(void)
{
    FM_DeleteFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_DeleteFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_DELETE_INT_CC);

    strncpy (CmdPacket.Filename, "filename.txt", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Nothing to verify */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_ProcessCmd_Test_DeleteInt */

void FM_ProcessCmd_Test_InvalidCommandCode(void)
{
    FM_DeleteFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_DeleteFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 99);

    /* Execute the function being tested */
    FM_ProcessCmd((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CC_ERR_EID, CFE_EVS_ERROR, "Main loop error: invalid command code: cc = 99"),
        "Main loop error: invalid command code: cc = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ProcessCmd_Test_InvalidCommandCode */

/* No test needed for FM_ReportHK - all lines of code are run and verified by other tests. */

void FM_App_Test_AddTestCases(void)
{
    UtTest_Add(FM_AppMain_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_AppMain_Test_Nominal");
    UtTest_Add(FM_AppMain_Test_RegisterAppError, FM_Test_Setup, FM_Test_TearDown, "FM_AppMain_Test_RegisterAppError");
    UtTest_Add(FM_AppMain_Test_AppInitError, FM_Test_Setup, FM_Test_TearDown, "FM_AppMain_Test_AppInitError");
    UtTest_Add(FM_AppMain_Test_SBError, FM_Test_Setup, FM_Test_TearDown, "FM_AppMain_Test_SBError");
    UtTest_Add(FM_AppInit_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_AppInit_Test_Nominal");
    UtTest_Add(FM_AppInit_Test_EVSRegisterError, FM_Test_Setup, FM_Test_TearDown, "FM_AppInit_Test_EVSRegisterError");
    UtTest_Add(FM_AppInit_Test_SBCreatePipeError, FM_Test_Setup, FM_Test_TearDown, "FM_AppInit_Test_SBCreatePipeError");
    UtTest_Add(FM_AppInit_Test_SBSubscribeHKError, FM_Test_Setup, FM_Test_TearDown, "FM_AppInit_Test_SBSubscribeHKError");
    UtTest_Add(FM_AppInit_Test_SBSubscribeFMError, FM_Test_Setup, FM_Test_TearDown, "FM_AppInit_Test_SBSubscribeFMError");
    UtTest_Add(FM_AppInit_Test_TableInitError, FM_Test_Setup, FM_Test_TearDown, "FM_AppInit_Test_TableInitError");
    UtTest_Add(FM_ProcessPkt_Test_HK, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessPkt_Test_HK");
    UtTest_Add(FM_ProcessPkt_Test_Cmd, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessPkt_Test_Cmd");
    UtTest_Add(FM_ProcessPkt_Test_Error, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessPkt_Test_Error");
    UtTest_Add(FM_ProcessCmd_Test_Noop, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_Noop");
    UtTest_Add(FM_ProcessCmd_Test_Reset, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_Reset");
    UtTest_Add(FM_ProcessCmd_Test_Copy, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_Copy");
    UtTest_Add(FM_ProcessCmd_Test_Move, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_Move");
    UtTest_Add(FM_ProcessCmd_Test_Rename, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_Rename");
    UtTest_Add(FM_ProcessCmd_Test_Delete, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_Delete");
    UtTest_Add(FM_ProcessCmd_Test_DeleteAll, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_DeleteAll");
    UtTest_Add(FM_ProcessCmd_Test_Decompress, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_Decompress");
    UtTest_Add(FM_ProcessCmd_Test_Concat, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_Concat");
    UtTest_Add(FM_ProcessCmd_Test_GetFileInfo, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_GetFileInfo");
    UtTest_Add(FM_ProcessCmd_Test_GetOpenFiles, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_GetOpenFiles");
    UtTest_Add(FM_ProcessCmd_Test_CreateDir, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_CreateDir");
    UtTest_Add(FM_ProcessCmd_Test_DeleteDir, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_DeleteDir");
    UtTest_Add(FM_ProcessCmd_Test_GetDirFile, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_GetDirFile");
    UtTest_Add(FM_ProcessCmd_Test_GetDirPkt, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_GetDirPkt");
    UtTest_Add(FM_ProcessCmd_Test_GetFreeSpace, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_GetFreeSpace");
    UtTest_Add(FM_ProcessCmd_Test_SetTableState, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_SetTableState");
    UtTest_Add(FM_ProcessCmd_Test_SetPermissions, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_SetPermissions");
    UtTest_Add(FM_ProcessCmd_Test_DeleteInt, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_DeleteInt");
    UtTest_Add(FM_ProcessCmd_Test_InvalidCommandCode, FM_Test_Setup, FM_Test_TearDown, "FM_ProcessCmd_Test_InvalidCommandCode");
} /* end FM_App_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
