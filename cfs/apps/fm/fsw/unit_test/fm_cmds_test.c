 /*************************************************************************
 ** File:
 **   $Id: fm_cmds_test.c 1.6.1.2 2017/01/23 21:53:01EST sstrege Exp  $
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
 **   This file contains unit test cases for the functions contained in the file fm_cmds.c.
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

#include "fm_cmds_test.h"
#include "fm_app.h"
#include "fm_cmds.h"
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

int32 UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsDirectory(const char *path, os_fstat_t  *filestats)
{
    filestats->st_mode = S_IFDIR;

    return CFE_SUCCESS;
} /* end UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsDirectory */

int32 UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsFile(const char *path, os_fstat_t  *filestats)
{
    filestats->st_mode = S_IFREG;

    return CFE_SUCCESS;
} /* end UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsFile */

uint8 UT_FM_CFE_OSFILEAPI_StatStatHook3RunCount;
int32 UT_FM_CFE_OSFILEAPI_StatHook3(const char *path, os_fstat_t  *filestats)
{
    UT_FM_CFE_OSFILEAPI_StatStatHook3RunCount++;

    if (UT_FM_CFE_OSFILEAPI_StatStatHook3RunCount == 1)
        filestats->st_mode = S_IFDIR;
    else
        filestats->st_mode = S_IFREG;

    return CFE_SUCCESS;
} /* end UT_FM_CFE_OSFILEAPI_StatHook3 */

int32 UT_FM_CFE_OSFILEAPI_FDGetInfoHook (int32 filedes, OS_FDTableEntry *fd_prop)
{
    CFE_PSP_MemSet(fd_prop, 0, sizeof(OS_FDTableEntry));

    return CFE_SUCCESS;
} /* end UT_FM_CFE_OSFILEAPI_FDGetInfoHook */

void FM_NoopCmd_Test(void)
{
    boolean Result;
    FM_NoopCmd_t   CmdPacket;
    char           Message[125];

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_NoopCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_NOOP_CC);

    /* Execute the function being tested */
    Result = FM_NoopCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    sprintf(Message, "No-op command: FM version %d.%d.%d.%d", FM_MAJOR_VERSION, FM_MINOR_VERSION, FM_REVISION, FM_MISSION_REV);
    UtAssert_True (Ut_CFE_EVS_EventSent(FM_NOOP_CMD_EID, CFE_EVS_INFORMATION, Message), Message);

    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_NoopCmd_Test */

void FM_ResetCountersCmd_Test(void)
{
    boolean Result;
    FM_ResetCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_ResetCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_RESET_CC);

    /* Execute the function being tested */
    Result = FM_ResetCountersCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_RESET_CMD_EID, CFE_EVS_DEBUG, "Reset Counters command"),
        "Reset Counters command");

    UtAssert_True (FM_GlobalData.CommandCounter == 0, "FM_GlobalData.CommandCounter == 0");
    UtAssert_True (FM_GlobalData.CommandErrCounter == 0, "FM_GlobalData.CommandErrCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 0, "FM_GlobalData.ChildCmdCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 0, "FM_GlobalData.ChildCmdErrCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 0, "FM_GlobalData.ChildCmdWarnCounter == 0");

    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ResetCountersCmd_Test */

void FM_CopyFileCmd_Test_OverwriteZero(void)
{
    boolean Result;
    FM_CopyFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_CopyFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_COPY_CC);

    strncpy (CmdPacket.Source, "./testfile.txt", OS_MAX_PATH_LEN);
    strncpy (CmdPacket.Target, "./temp_dest.txt", OS_MAX_PATH_LEN);

    CmdPacket.Overwrite = 0;

    /* Set target file to not exist */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = FM_CopyFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_COPY_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_COPY_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_CopyFileCmd_Test_OverwriteZero */

void FM_CopyFileCmd_Test_OverwriteNonzero(void)
{
    boolean Result;
    FM_CopyFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_CopyFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_COPY_CC);

    strncpy (CmdPacket.Source, "./testfile.txt", OS_MAX_PATH_LEN);
    strncpy (CmdPacket.Target, "./temp_dest.txt", OS_MAX_PATH_LEN);

    CmdPacket.Overwrite = 1;

    /* Execute the function being tested */
    Result = FM_CopyFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_COPY_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_COPY_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_CopyFileCmd_Test_OverwriteNonzero */

void FM_MoveFileCmd_Test_OverwriteZero(void)
{
    boolean Result;
    FM_MoveFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_MoveFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_MOVE_CC);

    strncpy (CmdPacket.Source, "./testfile.txt", OS_MAX_PATH_LEN);
    strncpy (CmdPacket.Target, "./temp_dest.txt", OS_MAX_PATH_LEN);

    CmdPacket.Overwrite = 0;

    /* Set target file to not exist */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = FM_MoveFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_MOVE_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_MOVE_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_MoveFileCmd_Test_OverwriteZero */

void FM_MoveFileCmd_Test_OverwriteNonzero(void)
{
    boolean Result;
    FM_MoveFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_MoveFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_MOVE_CC);

    strncpy (CmdPacket.Source, "./testfile.txt", OS_MAX_PATH_LEN);
    strncpy (CmdPacket.Target, "./temp_dest.txt", OS_MAX_PATH_LEN);

    CmdPacket.Overwrite = 1;

    /* Execute the function being tested */
    Result = FM_MoveFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_MOVE_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_MOVE_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_MoveFileCmd_Test_OverwriteNonzero */

void FM_RenameFileCmd_Test(void)
{
    boolean Result;
    FM_RenameFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_RenameFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_RENAME_CC);

    strncpy (CmdPacket.Source, "./testfile.txt", OS_MAX_PATH_LEN);
    strncpy (CmdPacket.Target, "./temp_dest.txt", OS_MAX_PATH_LEN);

    /* Set target file to not exist */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = FM_RenameFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_RENAME_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_RENAME_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_RenameFileCmd_Test */

void FM_DeleteFileCmd_Test(void)
{
    boolean Result;
    FM_DeleteFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_DeleteFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_DELETE_CC);

    strncpy (CmdPacket.Filename, "filename.txt", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    Result = FM_DeleteFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_DELETE_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_DELETE_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Filename, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Filename, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_DeleteFileCmd_Test */

void FM_DeleteAllFilesCmd_Test(void)
{
    boolean Result;
    FM_DeleteAllCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_DeleteAllCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_DELETE_ALL_CC);

    strncpy (CmdPacket.Directory, "/directory/", OS_MAX_PATH_LEN);

    /* Satisfies second instanace of condition "CommandResult == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsDirectory);

    /* Execute the function being tested */
    Result = FM_DeleteAllFilesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_DELETE_ALL_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_DELETE_ALL_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source2, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source2, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_DeleteAllFilesCmd_Test */

void FM_DecompressFileCmd_Test(void)
{
    boolean Result;
    FM_DecompressCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_DecompressCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_DECOMPRESS_CC);

    strncpy (CmdPacket.Source, "./testfile.txt", OS_MAX_PATH_LEN);
    strncpy (CmdPacket.Target, "./temp_dest.txt", OS_MAX_PATH_LEN);

    /* Satisfies first instance of condition "CommandResult == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Set target file to not exist */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 2);

    /* Execute the function being tested */
    Result = FM_DecompressFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_DECOMPRESS_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_DECOMPRESS_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_DecompressFileCmd_Test */

void FM_ConcatFilesCmd_Test(void)
{
    boolean Result;
    FM_ConcatCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_ConcatCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_CONCAT_CC);

    strncpy (CmdPacket.Source1, "./testfile1.txt", OS_MAX_PATH_LEN);
    strncpy (CmdPacket.Source2, "./testfile2.txt", OS_MAX_PATH_LEN);
    strncpy (CmdPacket.Target, "./temp_dest.txt", OS_MAX_PATH_LEN);

    /* Set target file to not exist */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 3);
    
    /* Mark files as closed files on other calls to OS_stat */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Execute the function being tested */
    Result = FM_ConcatFilesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_CONCAT_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_CONCAT_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source1, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Source1, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source2, CmdPacket.Source2, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source2, CmdPacket.Source2, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Target, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_ConcatFilesCmd_Test */

void FM_GetFileInfoCmd_Test_Nominal(void)
{
    boolean Result;
    FM_GetFileInfoCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_FILE_INFO_CC);

    strncpy (CmdPacket.Filename, "./testfile.txt", OS_MAX_PATH_LEN);

    /* Satisfies second instance of condition "CommandResult == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Execute the function being tested */
    Result = FM_GetFileInfoCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_GET_FILE_INFO_CC, "FM_GlobalData.ChildQueue[0].CommandCode == FM_GET_FILE_INFO_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Filename, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Filename, OS_MAX_PATH_LEN) == 0");
    UtAssert_True (FM_GlobalData.ChildQueue[0].FileInfoState == FM_NAME_IS_FILE_CLOSED, "FM_GlobalData.ChildQueue[0].FileInfoState == FM_NAME_IS_FILE_CLOSED");
    UtAssert_True (FM_GlobalData.ChildQueue[0].FileInfoCRC == 0, "FM_GlobalData.ChildQueue[0].FileInfoCRC == 0");
    UtAssert_True (FM_GlobalData.ChildQueue[0].FileInfoSize == 0, "FM_GlobalData.ChildQueue[0].FileInfoSize == 0");
    UtAssert_True (FM_GlobalData.ChildQueue[0].FileInfoTime == 0, "FM_GlobalData.ChildQueue[0].FileInfoTime == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_GetFileInfoCmd_Test_Nominal */

void FM_GetFileInfoCmd_Test_InvalidFilename(void)
{
    boolean Result;
    FM_GetFileInfoCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_FILE_INFO_CC);

    strncpy (CmdPacket.Filename, "", OS_MAX_PATH_LEN);

    /* Satisfies second instance of condition "CommandResult == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Execute the function being tested */
    Result = FM_GetFileInfoCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_FILE_INFO_SRC_ERR_EID, CFE_EVS_ERROR, "Get File Info error: invalid name: name = "),
        "Get File Info error: invalid name: name = ");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_GetFileInfoCmd_Test_InvalidFilename */

void FM_GetOpenFilesCmd_Test(void)
{
    boolean Result;
    FM_GetOpenFilesCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetOpenFilesCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_OPEN_FILES_CC);

    /* Sets file descriptor table (fd_prop) to all zero, to prevent segmentation fault */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, &UT_FM_CFE_OSFILEAPI_FDGetInfoHook);

    /* Execute the function being tested */
    Result = FM_GetOpenFilesCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.OpenFilesPkt.NumOpenFiles == OS_MAX_NUM_OPEN_FILES, "FM_GlobalData.OpenFilesPkt.NumOpenFiles == OS_MAX_NUM_OPEN_FILES");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_OPEN_FILES_CMD_EID, CFE_EVS_DEBUG, "Get Open Files command"),
        "Get Open Files command");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_GetOpenFilesCmd_Test */

void FM_CreateDirectoryCmd_Test(void)
{
    boolean Result;
    FM_CreateDirCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_CreateDirCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_CREATE_DIR_CC);

    strncpy (CmdPacket.Directory, "/directory/", OS_MAX_PATH_LEN);

    /* Set target file to not exist */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_CreateDirectoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_CreateDirectoryCmd_Test */

void FM_DeleteDirectoryCmd_Test(void)
{
    boolean Result;
    FM_DeleteDirCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_DeleteDirCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_DELETE_DIR_CC);

    strncpy (CmdPacket.Directory, "/directory/", OS_MAX_PATH_LEN);

    /* Satisfies second instance of condition "CommandResult == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsDirectory);

    /* Execute the function being tested */
    Result = FM_DeleteDirectoryCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_DeleteDirectoryCmd_Test */

void FM_GetDirListFileCmd_Test_DefaultFilename(void)
{
    boolean Result;
    FM_GetDirFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetDirFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_DIR_FILE_CC);

    strncpy (CmdPacket.Directory, "/directory/", OS_MAX_PATH_LEN);
    strncpy (CmdPacket.Filename, "", OS_MAX_PATH_LEN);

    /* Satisfies second and third instances of condition "CommandResult == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CFE_OSFILEAPI_StatHook3); 
    UT_FM_CFE_OSFILEAPI_StatStatHook3RunCount = 0;

    /* Execute the function being tested */
    Result = FM_GetDirListFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_GET_DIR_FILE_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_GET_DIR_FILE_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source2, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source2, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Target, FM_DIR_LIST_FILE_DEFNAME, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Target, FM_DIR_LIST_FILE_DEFNAME, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_GetDirListFileCmd_Test_DefaultFilename */

void FM_GetDirListFileCmd_Test_SpecifiedFilename(void)
{
    boolean Result;
    FM_GetDirFileCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetDirFileCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_DIR_FILE_CC);

    strncpy (CmdPacket.Directory, "/directory/", OS_MAX_PATH_LEN);
    strncpy (CmdPacket.Filename, "filename.txt", OS_MAX_PATH_LEN);

    /* Satisfies second and third instances of condition "CommandResult == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CFE_OSFILEAPI_StatHook3);
    UT_FM_CFE_OSFILEAPI_StatStatHook3RunCount = 0;

    /* Execute the function being tested */
    Result = FM_GetDirListFileCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_GET_DIR_FILE_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_GET_DIR_FILE_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source2, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source2, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Filename, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Target, CmdPacket.Filename, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_GetDirListFileCmd_Test_SpecifiedFilename */

void FM_GetDirListPktCmd_Test(void)
{
    boolean Result;
    FM_GetDirPktCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetDirPktCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_DIR_PKT_CC);

    strncpy (CmdPacket.Directory, "/directory/", OS_MAX_PATH_LEN);

    /* Satisfies second instance of condition "CommandResult == TRUE" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsDirectory);

    /* Execute the function being tested */
    Result = FM_GetDirListPktCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (FM_GlobalData.ChildQueue[0].CommandCode == FM_GET_DIR_PKT_CC, "FM_GlobalData.ChildQueue[0].CommandCode = FM_GET_DIR_PKT_CC");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source1, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0");
    UtAssert_True
        (strncmp(FM_GlobalData.ChildQueue[0].Source2, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.ChildQueue[0].Source2, CmdPacket.Directory, OS_MAX_PATH_LEN) == 0");
    
    UtAssert_True (FM_GlobalData.ChildQueue[0].DirListOffset == CmdPacket.DirListOffset, "FM_GlobalData.ChildQueue[0].DirListOffset == CmdPacket.DirListOffset");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_GetDirListPktCmd_Test */

void FM_GetFreeSpaceCmd_Test_Nominal(void)
{
    boolean Result;
    FM_GetFreeSpaceCmd_t   CmdPacket;
    FM_FreeSpaceTable_t    FreeSpaceTable;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetFreeSpaceCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_FREE_SPACE_CC);

    FM_GlobalData.FreeSpaceTablePtr = &FreeSpaceTable;

    FM_GlobalData.FreeSpaceTablePtr->FileSys[0].State = FM_TABLE_ENTRY_ENABLED;

    strncpy(FM_GlobalData.FreeSpaceTablePtr->FileSys[0].Name, "Name", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    Result = FM_GetFreeSpaceCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True (strncmp (FM_GlobalData.FreeSpacePkt.FileSys[0].Name, "Name", OS_MAX_PATH_LEN) == 0, "strncmp (FM_GlobalData.FreeSpacePkt.FileSys[0].Name, 'Name', OS_MAX_PATH_LEN) == 0");
    UtAssert_True (FM_GlobalData.FreeSpacePkt.FileSys[0].FreeSpace_A == 0, "FM_GlobalData.FreeSpacePkt.FileSys[0].FreeSpace_A == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_FREE_SPACE_CMD_EID, CFE_EVS_DEBUG, "Get Free Space command"),
        "Get Free Space command");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_GetFreeSpaceCmd_Test_Nominal */

void FM_GetFreeSpaceCmd_Test_TableNotLoaded(void)
{
    boolean Result;
    FM_GetFreeSpaceCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_GetFreeSpaceCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_GET_FREE_SPACE_CC);

    /* Execute the function being tested */
    Result = FM_GetFreeSpaceCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_FREE_SPACE_TBL_ERR_EID, CFE_EVS_ERROR, "Get Free Space error: file system free space table is not loaded"),
        "Get Free Space error: file system free space table is not loaded");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_GetFreeSpaceCmd_Test_TableNotLoaded */

void FM_SetTableStateCmd_Test_Nominal(void)
{
    boolean Result;
    FM_SetTableStateCmd_t   CmdPacket;
    FM_FreeSpaceTable_t     FreeSpaceTable;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_SET_TABLE_STATE_CC);

    CmdPacket.TableEntryIndex = 0;
    CmdPacket.TableEntryState = FM_TABLE_ENTRY_ENABLED;

    FM_GlobalData.FreeSpaceTablePtr = &FreeSpaceTable;

    FM_GlobalData.FreeSpaceTablePtr->FileSys[CmdPacket.TableEntryIndex].State = FM_TABLE_ENTRY_DISABLED;

    /* Execute the function being tested */
    Result = FM_SetTableStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

    UtAssert_True
        (FM_GlobalData.FreeSpaceTablePtr->FileSys[CmdPacket.TableEntryIndex].State == CmdPacket.TableEntryState,
        "FM_GlobalData.FreeSpaceTablePtr->FileSys[CmdPacket.TableEntryIndex].State == CmdPacket.TableEntryState");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_SET_TABLE_STATE_CMD_EID, CFE_EVS_INFORMATION, "Set Table State command: index = 0, state = 1"),
        "Set Table State command: index = 0, state = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_SetTableStateCmd_Test_Nominal */

void FM_SetTableStateCmd_Test_TableNotLoaded(void)
{
    boolean Result;
    FM_SetTableStateCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_SET_TABLE_STATE_CC);

    /* Execute the function being tested */
    Result = FM_SetTableStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_SET_TABLE_STATE_TBL_ERR_EID, CFE_EVS_ERROR,
               "Set Table State error: file system free space table is not loaded"),
        "Set Table State error: file system free space table is not loaded");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_SetTableStateCmd_Test_TableNotLoaded */

void FM_SetTableStateCmd_Test_TableIndexOutOfRange(void)
{
    boolean Result;
    FM_SetTableStateCmd_t   CmdPacket;
    FM_FreeSpaceTable_t     FreeSpaceTable;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_SET_TABLE_STATE_CC);

    CmdPacket.TableEntryIndex = FM_TABLE_ENTRY_COUNT + 1;
    CmdPacket.TableEntryState = FM_TABLE_ENTRY_ENABLED;

    FM_GlobalData.FreeSpaceTablePtr = &FreeSpaceTable;

    FM_GlobalData.FreeSpaceTablePtr->FileSys[CmdPacket.TableEntryIndex].State = FM_TABLE_ENTRY_DISABLED;

    /* Execute the function being tested */
    Result = FM_SetTableStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_SET_TABLE_STATE_ARG_ERR_EID, CFE_EVS_ERROR, "Set Table State error: invalid command argument: index = 9"),  
        "Set Table State error: invalid command argument: index = 9");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_SetTableStateCmd_Test_TableIndexOutOfRange */

void FM_SetTableStateCmd_Test_InvalidState(void)
{
    boolean Result;
    FM_SetTableStateCmd_t   CmdPacket;
    FM_FreeSpaceTable_t     FreeSpaceTable;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_SET_TABLE_STATE_CC);

    CmdPacket.TableEntryIndex = 0;
    CmdPacket.TableEntryState = 99;

    FM_GlobalData.FreeSpaceTablePtr = &FreeSpaceTable;

    FM_GlobalData.FreeSpaceTablePtr->FileSys[CmdPacket.TableEntryIndex].State = FM_TABLE_ENTRY_DISABLED;

    /* Execute the function being tested */
    Result = FM_SetTableStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_SET_TABLE_STATE_ARG_ERR_EID, CFE_EVS_ERROR, "Set Table State error: invalid command argument: state = 99"),  
        "Set Table State error: invalid command argument: state = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_SetTableStateCmd_Test_InvalidState */

void FM_SetTableStateCmd_Test_UnusedTable(void)
{
    boolean Result;
    FM_SetTableStateCmd_t   CmdPacket;
    FM_FreeSpaceTable_t     FreeSpaceTable;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_SetTableStateCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_SET_TABLE_STATE_CC);

    CmdPacket.TableEntryIndex = 0;
    CmdPacket.TableEntryState = FM_TABLE_ENTRY_ENABLED;

    FM_GlobalData.FreeSpaceTablePtr = &FreeSpaceTable;

    FM_GlobalData.FreeSpaceTablePtr->FileSys[CmdPacket.TableEntryIndex].State = FM_TABLE_ENTRY_UNUSED;

    /* Execute the function being tested */
    Result = FM_SetTableStateCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_SET_TABLE_STATE_UNUSED_ERR_EID, CFE_EVS_ERROR, "Set Table State error: cannot modify unused table entry: index = 0"),  
        "Set Table State error: cannot modify unused table entry: index = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_SetTableStateCmd_Test_UnusedTable */


void FM_SetPermissionsCmd_Test_Nominal(void)
{
    boolean Result = FALSE;
    FM_SetPermCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_SetPermCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_SET_FILE_PERM_CC);

    strcpy( CmdPacket.FileName,"/tmp/name");
    CmdPacket.Mode = 0777;
//    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, OS_SUCCESS, 1);
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Execute the function being tested */
    Result = FM_SetPermissionsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");

} /* end FM_SetPermissionsCmd_Test_Nominal */


void FM_SetPermissionsCmd_Test_InvalidPathTest(void)
{
    boolean Result = TRUE;
    FM_SetPermCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, sizeof(FM_SetPermCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, FM_SET_FILE_PERM_CC);

    strcpy( CmdPacket.FileName,"");
    CmdPacket.Mode = 0777;
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMDS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Execute the function being tested */
    Result = FM_SetPermissionsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

} /* end FM_SetPermissionsCmd_Test_DirTest */

void FM_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(FM_NoopCmd_Test, FM_Test_Setup, FM_Test_TearDown, "FM_NoopCmd_Test");
    UtTest_Add(FM_ResetCountersCmd_Test, FM_Test_Setup, FM_Test_TearDown, "FM_ResetCountersCmd_Test");
    UtTest_Add(FM_CopyFileCmd_Test_OverwriteZero, FM_Test_Setup, FM_Test_TearDown, "FM_CopyFileCmd_Test_OverwriteZero");
    UtTest_Add(FM_CopyFileCmd_Test_OverwriteNonzero, FM_Test_Setup, FM_Test_TearDown, "FM_CopyFileCmd_Test_OverwriteNonzero");
    UtTest_Add(FM_MoveFileCmd_Test_OverwriteZero, FM_Test_Setup, FM_Test_TearDown, "FM_MoveFileCmd_Test_OverwriteZero");
    UtTest_Add(FM_MoveFileCmd_Test_OverwriteNonzero, FM_Test_Setup, FM_Test_TearDown, "FM_MoveFileCmd_Test_OverwriteNonzero");
    UtTest_Add(FM_RenameFileCmd_Test, FM_Test_Setup, FM_Test_TearDown, "FM_RenameFileCmd_Test");
    UtTest_Add(FM_DeleteFileCmd_Test, FM_Test_Setup, FM_Test_TearDown, "FM_DeleteFileCmd_Test");
    UtTest_Add(FM_DeleteAllFilesCmd_Test, FM_Test_Setup, FM_Test_TearDown, "FM_DeleteAllFilesCmd_Test");
    UtTest_Add(FM_DecompressFileCmd_Test, FM_Test_Setup, FM_Test_TearDown, "FM_DecompressFileCmd_Test");
    UtTest_Add(FM_ConcatFilesCmd_Test, FM_Test_Setup, FM_Test_TearDown, "FM_ConcatFilesCmd_Test");
    UtTest_Add(FM_GetFileInfoCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_GetFileInfoCmd_Test_Nominal");
    UtTest_Add(FM_GetFileInfoCmd_Test_InvalidFilename, FM_Test_Setup, FM_Test_TearDown, "FM_GetFileInfoCmd_Test_InvalidFilename");
    UtTest_Add(FM_GetOpenFilesCmd_Test, FM_Test_Setup, FM_Test_TearDown, "FM_GetOpenFilesCmd_Test");
    UtTest_Add(FM_CreateDirectoryCmd_Test, FM_Test_Setup, FM_Test_TearDown, "FM_CreateDirectoryCmd_Test");
    UtTest_Add(FM_DeleteDirectoryCmd_Test, FM_Test_Setup, FM_Test_TearDown, "FM_DeleteDirectoryCmd_Test");
    UtTest_Add(FM_GetDirListFileCmd_Test_DefaultFilename, FM_Test_Setup, FM_Test_TearDown, "FM_GetDirListFileCmd_Test_DefaultFilename");
    UtTest_Add(FM_GetDirListFileCmd_Test_SpecifiedFilename, FM_Test_Setup, FM_Test_TearDown, "FM_GetDirListFileCmd_Test_SpecifiedFilename");
    UtTest_Add(FM_GetDirListPktCmd_Test, FM_Test_Setup, FM_Test_TearDown, "FM_GetDirListPktCmd_Test");
    UtTest_Add(FM_GetFreeSpaceCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_GetFreeSpaceCmd_Test_Nominal");
    UtTest_Add(FM_GetFreeSpaceCmd_Test_TableNotLoaded, FM_Test_Setup, FM_Test_TearDown, "FM_GetFreeSpaceCmd_Test_TableNotLoaded");
    UtTest_Add(FM_SetTableStateCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_SetTableStateCmd_Test_Nominal");
    UtTest_Add(FM_SetTableStateCmd_Test_TableNotLoaded, FM_Test_Setup, FM_Test_TearDown, "FM_SetTableStateCmd_Test_TableNotLoaded");
    UtTest_Add(FM_SetTableStateCmd_Test_TableIndexOutOfRange, FM_Test_Setup, FM_Test_TearDown, "FM_SetTableStateCmd_Test_TableIndexOutOfRange");
    UtTest_Add(FM_SetTableStateCmd_Test_InvalidState, FM_Test_Setup, FM_Test_TearDown, "FM_SetTableStateCmd_Test_InvalidState");
    UtTest_Add(FM_SetTableStateCmd_Test_UnusedTable, FM_Test_Setup, FM_Test_TearDown, "FM_SetTableStateCmd_Test_UnusedTable");
    
    UtTest_Add(FM_SetPermissionsCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_SetPermissionsCmd_Test_Nominal");
    UtTest_Add(FM_SetPermissionsCmd_Test_InvalidPathTest, FM_Test_Setup, FM_Test_TearDown, "FM_SetPermissionsCmd_Test_InvalidPathTest");
} /* end FM_Cmds_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
