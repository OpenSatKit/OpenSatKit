 /*************************************************************************
 ** File:
 **   $Id: fm_cmd_utils_test.c 1.3.1.2 2017/01/23 21:53:06EST sstrege Exp  $
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
 **   This file contains unit test cases for the functions contained in the file fm_cmd_utils.c.
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

#include "fm_cmd_utils_test.h"
#include "fm_app.h"
#include "fm_msg.h"
#include "fm_msgdefs.h"
#include "fm_msgids.h"
#include "fm_events.h"
#include "fm_cmd_utils.h"
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

int32 UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile(const char *path, os_fstat_t  *filestats)
{
    filestats->st_mode = S_IFREG;

    return CFE_SUCCESS;
} /* end UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile */

int32 UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsDirectory(const char *path, os_fstat_t  *filestats)
{
    filestats->st_mode = S_IFDIR;

    filestats->st_mtime = 1;
    filestats->st_size = 2;

    return CFE_SUCCESS;
} /* end UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsDirectory */

int32 UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_FDGetInfoHook(int32 filedes, OS_FDTableEntry *fd_prop)
{
    fd_prop->IsValid = TRUE;

    strncpy (fd_prop->Path, "name", OS_MAX_PATH_LEN);

    return 0;
} /* end UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_FDGetInfoHook */

int32 UT_FM_CMD_UTILS_TEST_CFE_ES_GetTaskInfoHook(CFE_ES_TaskInfo_t *TaskInfo, uint32 TaskId)
{
    strncpy ((char *)TaskInfo->AppName, "appname", OS_MAX_PATH_LEN);

    return 0;
} /* end UT_FM_CMD_UTILS_TEST_CFE_ES_GetTaskInfoHook */

void FM_IsValidCmdPktLength_Test_InvalidPacketLength(void)
{
    boolean Result;
    CFE_SB_MsgPtr_t CmdPacket;
    uint16 ExpectedLength = 1;
    uint32 EventID = 1;

    CFE_SB_InitMsg (&CmdPacket, FM_CMD_MID, 0, TRUE);

    /* Set to generate error message "invalid command packet length" */
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 0, 1);

    /* Execute the function being tested */
    Result = FM_IsValidCmdPktLength(CmdPacket, ExpectedLength, EventID, "Command");
    
    /* Verify results */

    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(EventID, CFE_EVS_ERROR, "Command error: invalid command packet length: expected = 1, actual = 0"),
        "Command error: invalid command packet length: expected = 1, actual = 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_IsValidCmdPktLength_Test_InvalidPacketLength */

void FM_VerifyOverwrite_Test(void)
{
    boolean Result;
    uint16 Overwrite = 99;
    uint32 EventID = 1;

    /* Execute the function being tested */
    Result = FM_VerifyOverwrite(Overwrite, EventID, "Command");
    
    /* Verify results */

    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(EventID, CFE_EVS_ERROR, "Command error: invalid overwrite = 99"),
        "Command error: invalid overwrite = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyOverwrite_Test */

void FM_GetOpenFilesData_Test(void)
{
    uint32 Result;
    FM_OpenFilesEntry_t OpenFilesData[OS_MAX_NUM_OPEN_FILES];

    /* Satisfies condition "FDTableEntry.IsValid == TRUE", and causes LogicalName to be set to "name" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_FDGetInfoHook); 

    /* Sets AppName to "appname" */
    Ut_CFE_ES_SetFunctionHook(UT_CFE_ES_GETTASKINFO_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_ES_GetTaskInfoHook);

    /* Execute the function being tested */
    Result = FM_GetOpenFilesData(OpenFilesData);
    
    /* Verify results */
    UtAssert_True (Result == OS_MAX_NUM_OPEN_FILES, "Result == OS_MAX_NUM_OPEN_FILES");

    UtAssert_True
        (strncmp(OpenFilesData[0].LogicalName, "name", OS_MAX_PATH_LEN) == 0,
        "strncmp(OpenFilesData[0].LogicalName, 'name', OS_MAX_PATH_LEN) == 0");

    UtAssert_True
        (strncmp(OpenFilesData[0].AppName, "appname", OS_MAX_API_NAME) == 0,
        "strncmp(OpenFilesData[0].AppName, 'appname', OS_MAX_API_NAME) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_GetOpenFilesData_Test */

void FM_GetFilenameState_Test_IsDirectory(void)
{
    uint32 Result;

    /* Satisfies condition "if (S_ISDIR(FileStatus.st_mode))" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsDirectory);

    /* Execute the function being tested */
    Result = FM_GetFilenameState("filename", OS_MAX_PATH_LEN, TRUE);
    
    /* Verify results */
    UtAssert_True (Result == FM_NAME_IS_DIRECTORY, "Result == FM_NAME_IS_DIRECTORY");

    UtAssert_True (FM_GlobalData.FileStatTime == 1, "FM_GlobalData.FileStatTime == 1");
    UtAssert_True (FM_GlobalData.FileStatSize == 2, "FM_GlobalData.FileStatSize == 2");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_GetFilenameState_Test_IsDirectory */

void FM_GetFilenameState_Test_NotInUse(void)
{
    uint32 Result;

    /* Set to fail condition "OS_stat(Filename, &FileStatus) == OS_SUCCESS" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_GetFilenameState("filename", OS_MAX_PATH_LEN, TRUE);
    
    /* Verify results */
    UtAssert_True (Result == FM_NAME_IS_NOT_IN_USE, "Result == FM_NAME_IS_NOT_IN_USE");

    UtAssert_True (FM_GlobalData.FileStatTime == 0, "FM_GlobalData.FileStatTime == 0");
    UtAssert_True (FM_GlobalData.FileStatSize == 0, "FM_GlobalData.FileStatSize == 0");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_GetFilenameState_Test_NotInUse */

void FM_VerifyNameValid_Test_InvalidName(void)
{
    uint32 Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "***filename", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    Result = FM_VerifyNameValid(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FM_NAME_IS_INVALID, "Result == FM_NAME_IS_INVALID");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: invalid name: name = ***filename"),
        "Command error: invalid name: name = ***filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyNameValid_Test_InvalidName */

void FM_VerifyFileClosed_Test_InvalidFilename(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "***filename", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    Result = FM_VerifyFileClosed(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: filename is invalid: name = ***filename"),
        "Command error: filename is invalid: name = ***filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileClosed_Test_InvalidFilename */

void FM_VerifyFileClosed_Test_FileDoesNotExist(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_NOT_IN_USE" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_VerifyFileClosed(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: file does not exist: name = filename"),
        "Command error: file does not exist: name = filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileClosed_Test_FileDoesNotExist */

void FM_VerifyFileClosed_Test_FileAlreadyOpen(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Needed to satisfy condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Needed to satisfy condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_FDGetInfoHook);

    /* Execute the function being tested */
    Result = FM_VerifyFileClosed(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: file is already open: name = name"),
        "Command error: file is already open: name = name");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileClosed_Test_FileAlreadyOpen */

void FM_VerifyFileClosed_Test_FileClosed(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Needed to satisfy condition "FilenameState == FM_NAME_IS_FILE_CLOSED" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Execute the function being tested */
    Result = FM_VerifyFileClosed(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_VerifyFileClosed_Test_FileClosed */

void FM_VerifyFileClosed_Test_IsDirectory(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Needed to satisfy condition "FilenameState == FM_NAME_IS_DIRECTORY" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsDirectory);

    /* Execute the function being tested */
    Result = FM_VerifyFileClosed(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: filename is a directory: name = name"),
        "Command error: filename is a directory: name = name");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileClosed_Test_IsDirectory */

void FM_VerifyFileExists_Test_InvalidFilename(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "***filename", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    Result = FM_VerifyFileExists(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: filename is invalid: name = ***filename"),
        "Command error: filename is invalid: name = ***filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileExists_Test_InvalidFilename */

void FM_VerifyFileExists_Test_FileDoesNotExist(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_NOT_IN_USE" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_VerifyFileExists(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: file does not exist: name = filename"),
        "Command error: file does not exist: name = filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileExists_Test_FileDoesNotExist */

void FM_VerifyFileExists_Test_FileOpen(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Satisfies condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Satisfies condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_FDGetInfoHook);

    /* Execute the function being tested */
    Result = FM_VerifyFileExists(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_VerifyFileExists_Test_FileOpen */

void FM_VerifyFileExists_Test_FileClosed(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Satisfies condition "FilenameState == FM_NAME_IS_FILE_CLOSED" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Execute the function being tested */
    Result = FM_VerifyFileExists(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_VerifyFileExists_Test_FileClosed */

void FM_VerifyFileExists_Test_IsDirectory(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Satisfies condition "FilenameState == FM_NAME_IS_DIRECTORY" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsDirectory);

    /* Execute the function being tested */
    Result = FM_VerifyFileExists(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: filename is a directory: name = name"),
        "Command error: filename is a directory: name = name");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileExists_Test_IsDirectory */

void FM_VerifyFileNoExist_Test_InvalidFilename(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "***filename", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    Result = FM_VerifyFileNoExist(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: filename is invalid: name = ***filename"),
        "Command error: filename is invalid: name = ***filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileNoExist_Test_InvalidFilename */

void FM_VerifyFileNoExist_Test_NotInUse(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename", OS_MAX_PATH_LEN);

    /* Satisfies condition "FilenameState == FM_NAME_IS_NOT_IN_USE" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_VerifyFileNoExist(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_VerifyFileNoExist_Test_NotInUse */

void FM_VerifyFileNoExist_Test_FileOpen(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename", OS_MAX_PATH_LEN);

    /* Satisfies condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Satisfies condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_FDGetInfoHook);

    /* Execute the function being tested */
    Result = FM_VerifyFileNoExist(Filename, OS_MAX_PATH_LEN, 1, "Command");

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: file already exists: name = filename"),
        "Command error: file already exists: name = filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileNoExist_Test_FileOpen */

void FM_VerifyFileNoExist_Test_FileClosed(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename", OS_MAX_PATH_LEN);

    /* Satisfies condition "FilenameState == FM_NAME_IS_FILE_CLOSED" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Execute the function being tested */
    Result = FM_VerifyFileNoExist(Filename, OS_MAX_PATH_LEN, 1, "Command");

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: file already exists: name = filename"),
        "Command error: file already exists: name = filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileNoExist_Test_FileClosed */

void FM_VerifyFileNoExist_Test_IsDirectory(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename", OS_MAX_PATH_LEN);

    /* Satisfies condition "FilenameState == FM_NAME_IS_DIRECTORY" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsDirectory);

    /* Execute the function being tested */
    Result = FM_VerifyFileNoExist(Filename, OS_MAX_PATH_LEN, 1, "Command");

    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: filename is a directory: name = filename"),
        "Command error: filename is a directory: name = filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileNoExist_Test_IsDirectory */

void FM_VerifyFileNotOpen_Test_InvalidFilename(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "***filename", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    Result = FM_VerifyFileNotOpen(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: filename is invalid: name = ***filename"),
        "Command error: filename is invalid: name = ***filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileNotOpen_Test_InvalidFilename */

void FM_VerifyFileNotOpen_Test_NotInUse(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_NOT_IN_USE" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_VerifyFileNotOpen(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_VerifyFileNotOpen_Test_NotInUse */

void FM_VerifyFileNotOpen_Test_FileOpen(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Satisfies condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Satisfies condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_FDGetInfoHook);

    /* Execute the function being tested */
    Result = FM_VerifyFileNotOpen(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: file exists as an open file: name = name"),
        "Command error: file exists as an open file: name = name");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileNotOpen_Test_FileOpen */

void FM_VerifyFileNotOpen_Test_FileClosed(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Satisfies condition "FilenameState == FM_NAME_IS_FILE_CLOSED" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Execute the function being tested */
    Result = FM_VerifyFileNotOpen(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_VerifyFileNotOpen_Test_FileClosed */

void FM_VerifyFileNotOpen_Test_IsDirectory(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename", OS_MAX_PATH_LEN);

    /* Satisfies condition "FilenameState == FM_NAME_IS_DIRECTORY" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsDirectory);

    /* Execute the function being tested */
    Result = FM_VerifyFileNotOpen(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: filename is a directory: name = filename"),
        "Command error: filename is a directory: name = filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyFileNotOpen_Test_IsDirectory */

void FM_VerifyDirExists_Test_InvalidName(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "***filename", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    Result = FM_VerifyDirExists(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: directory name is invalid: name = ***filename"),
        "Command error: directory name is invalid: name = ***filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyDirExists_Test_InvalidName */

void FM_VerifyDirExists_Test_NotInUse(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_NOT_IN_USE" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_VerifyDirExists(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: directory does not exist: name = filename"),
        "Command error: directory does not exist: name = filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyDirExists_Test_NotInUse */

void FM_VerifyDirExists_Test_FileOpen(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_FDGetInfoHook);

    /* Execute the function being tested */
    Result = FM_VerifyDirExists(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: directory name exists as a file: name name"),
        "Command error: directory name exists as a file: name name");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyDirExists_Test_FileOpen */

void FM_VerifyDirExists_Test_FileClosed(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_FILE_CLOSED" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Execute the function being tested */
    Result = FM_VerifyDirExists(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: directory name exists as a file: name name"),
        "Command error: directory name exists as a file: name name");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyDirExists_Test_FileClosed */

void FM_VerifyDirExists_Test_IsDirectory(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_DIRECTORY" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsDirectory);

    /* Execute the function being tested */
    Result = FM_VerifyDirExists(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_VerifyDirExists_Test_IsDirectory */

void FM_VerifyDirNoExist_Test_InvalidName(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "***filename", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    Result = FM_VerifyDirNoExist(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: directory name is invalid: name = ***filename"),
        "Command error: directory name is invalid: name = ***filename");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyDirNoExist_Test_InvalidName */

void FM_VerifyDirNoExist_Test_NotInUse(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_NOT_IN_USE" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_VerifyDirNoExist(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_VerifyDirNoExist_Test_NotInUse */

void FM_VerifyDirNoExist_Test_FileOpen(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_FDGetInfoHook);

    /* Execute the function being tested */
    Result = FM_VerifyDirNoExist(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: directory name exists as a file: name = name"),
        "Command error: directory name exists as a file: name = name");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyDirNoExist_Test_FileOpen */

void FM_VerifyDirNoExist_Test_FileClosed(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_FILE_CLOSED" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Execute the function being tested */
    Result = FM_VerifyDirNoExist(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: directory name exists as a file: name = name"),
        "Command error: directory name exists as a file: name = name");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyDirNoExist_Test_FileClosed */

void FM_VerifyDirNoExist_Test_DirectoryExists(void)
{
    boolean Result;
    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "name", OS_MAX_PATH_LEN);

    /* Satisfies condition "FilenameState == FM_NAME_IS_DIRECTORY" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CMD_UTILS_TEST_CFE_OSFILEAPI_StatHookIsDirectory);

    /* Execute the function being tested */
    Result = FM_VerifyDirNoExist(Filename, OS_MAX_PATH_LEN, 1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: directory already exists: name = name"),
        "Command error: directory already exists: name = name");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyDirNoExist_Test_DirectoryExists */

void FM_VerifyChildTask_Test_ChildDisabled(void)
{
    boolean Result;

    FM_GlobalData.ChildSemaphore = FM_CHILD_SEM_INVALID;

    /* Execute the function being tested */
    Result = FM_VerifyChildTask(1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: child task is disabled"),
        "Command error: child task is disabled");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyChildTask_Test_ChildDisabled */

void FM_VerifyChildTask_Test_ChildQueueFull(void)
{
    boolean Result;

    FM_GlobalData.ChildQueueCount = FM_CHILD_QUEUE_DEPTH;

    /* Execute the function being tested */
    Result = FM_VerifyChildTask(1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: child task queue is full"),
        "Command error: child task queue is full");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyChildTask_Test_ChildQueueFull */

void FM_VerifyChildTask_Test_ChildInterfaceBrokenLocalQueueCount(void)
{
    boolean Result;

    FM_GlobalData.ChildQueueCount = FM_CHILD_QUEUE_DEPTH + 1;
    FM_GlobalData.ChildWriteIndex = 1;

    /* Execute the function being tested */
    Result = FM_VerifyChildTask(1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: child task interface is broken: count = 4, index = 1"),
        "Command error: child task interface is broken: count = 4, index = 1");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyChildTask_Test_ChildInterfaceBrokenLocalQueueCount */

void FM_VerifyChildTask_Test_ChildInterfaceBrokenChildWriteIndex(void)
{
    boolean Result;

    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildWriteIndex = FM_CHILD_QUEUE_DEPTH;

    /* Execute the function being tested */
    Result = FM_VerifyChildTask(1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(1, CFE_EVS_ERROR, "Command error: child task interface is broken: count = 1, index = 3"),
        "Command error: child task interface is broken: count = 1, index = 3");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_VerifyChildTask_Test_ChildInterfaceBrokenChildWriteIndex */

void FM_VerifyChildTask_Test_Nominal(void)
{
    boolean Result;

    FM_GlobalData.ChildQueueCount = FM_CHILD_QUEUE_DEPTH - 1;
    FM_GlobalData.ChildWriteIndex = 1;

    /* Execute the function being tested */
    Result = FM_VerifyChildTask(1, "Command");
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_VerifyChildTask_Test_Nominal */

void FM_InvokeChildTask_Test_Nominal(void)
{
    /* Execute the function being tested */
    FM_InvokeChildTask();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildWriteIndex == 1, "FM_GlobalData.ChildWriteIndex == 1");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 1, "FM_GlobalData.ChildQueueCount == 1");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_InvokeChildTask_Test_Nominal */

void FM_InvokeChildTask_Test_WriteIndexEqualsQueueDepth(void)
{
    FM_GlobalData.ChildWriteIndex = FM_CHILD_QUEUE_DEPTH;

    /* Execute the function being tested */
    FM_InvokeChildTask();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildWriteIndex == 0, "FM_GlobalData.ChildWriteIndex == 0");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 1, "FM_GlobalData.ChildQueueCount == 1");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_InvokeChildTask_Test_WriteIndexEqualsQueueDepth */

void FM_AppendPathSep_Test(void)
{
    char Directory[OS_MAX_PATH_LEN];
    uint32 BufferSize = 11;

    strncpy (Directory, "directory", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    FM_AppendPathSep(Directory, BufferSize);
    
    /* Verify results */
    UtAssert_True
        (strncmp(Directory, "directory/", OS_MAX_PATH_LEN) == 0,
        "strncmp(Directory, 'directory/', OS_MAX_PATH_LEN) == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_AppendPathSep_Test */

void FM_Cmd_Utils_Test_AddTestCases(void)
{
    UtTest_Add(FM_IsValidCmdPktLength_Test_InvalidPacketLength, FM_Test_Setup, FM_Test_TearDown, "FM_IsValidCmdPktLength_Test_InvalidPacketLength");
    UtTest_Add(FM_VerifyOverwrite_Test, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyOverwrite_Test");
    UtTest_Add(FM_GetOpenFilesData_Test, FM_Test_Setup, FM_Test_TearDown, "FM_GetOpenFilesData_Test");
    UtTest_Add(FM_GetFilenameState_Test_IsDirectory, FM_Test_Setup, FM_Test_TearDown, "FM_GetFilenameState_Test_IsDirectory");
    UtTest_Add(FM_GetFilenameState_Test_NotInUse, FM_Test_Setup, FM_Test_TearDown, "FM_GetFilenameState_Test_NotInUse");
    UtTest_Add(FM_VerifyNameValid_Test_InvalidName, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyNameValid_Test_InvalidName");
    UtTest_Add(FM_VerifyFileClosed_Test_InvalidFilename, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileClosed_Test_InvalidFilename");
    UtTest_Add(FM_VerifyFileClosed_Test_FileDoesNotExist, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileClosed_Test_FileDoesNotExist");
    UtTest_Add(FM_VerifyFileClosed_Test_FileAlreadyOpen, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileClosed_Test_FileAlreadyOpen");
    UtTest_Add(FM_VerifyFileClosed_Test_FileClosed, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileClosed_Test_FileClosed");
    UtTest_Add(FM_VerifyFileClosed_Test_IsDirectory, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileClosed_Test_IsDirectory");
    UtTest_Add(FM_VerifyFileExists_Test_InvalidFilename, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileExists_Test_InvalidFilename");
    UtTest_Add(FM_VerifyFileExists_Test_FileDoesNotExist, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileExists_Test_FileDoesNotExist");
    UtTest_Add(FM_VerifyFileExists_Test_FileOpen, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileExists_Test_FileOpen");
    UtTest_Add(FM_VerifyFileExists_Test_FileClosed, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileExists_Test_FileClosed");
    UtTest_Add(FM_VerifyFileExists_Test_IsDirectory, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileExists_Test_IsDirectory");
    UtTest_Add(FM_VerifyFileNoExist_Test_InvalidFilename, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileNoExist_Test_InvalidFilename");
    UtTest_Add(FM_VerifyFileNoExist_Test_NotInUse, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileNoExist_Test_NotInUse");
    UtTest_Add(FM_VerifyFileNoExist_Test_FileOpen, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileNoExist_Test_FileOpen");
    UtTest_Add(FM_VerifyFileNoExist_Test_FileClosed, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileNoExist_Test_FileClosed");
    UtTest_Add(FM_VerifyFileNoExist_Test_IsDirectory, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileNoExist_Test_IsDirectory");
    UtTest_Add(FM_VerifyFileNotOpen_Test_InvalidFilename, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileNotOpen_Test_InvalidFilename");
    UtTest_Add(FM_VerifyFileNotOpen_Test_NotInUse, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileNotOpen_Test_NotInUse");
    UtTest_Add(FM_VerifyFileNotOpen_Test_FileOpen, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileNotOpen_Test_FileOpen");
    UtTest_Add(FM_VerifyFileNotOpen_Test_FileClosed, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileNotOpen_Test_FileClosed");
    UtTest_Add(FM_VerifyFileNotOpen_Test_IsDirectory, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyFileNotOpen_Test_IsDirectory");
    UtTest_Add(FM_VerifyDirExists_Test_InvalidName, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyDirExists_Test_InvalidName");
    UtTest_Add(FM_VerifyDirExists_Test_NotInUse, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyDirExists_Test_NotInUse");
    UtTest_Add(FM_VerifyDirExists_Test_FileOpen, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyDirExists_Test_FileOpen");
    UtTest_Add(FM_VerifyDirExists_Test_FileClosed, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyDirExists_Test_FileClosed");
    UtTest_Add(FM_VerifyDirExists_Test_IsDirectory, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyDirExists_Test_IsDirectory");
    UtTest_Add(FM_VerifyDirNoExist_Test_InvalidName, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyDirNoExist_Test_InvalidName");
    UtTest_Add(FM_VerifyDirNoExist_Test_NotInUse, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyDirNoExist_Test_NotInUse");
    UtTest_Add(FM_VerifyDirNoExist_Test_FileOpen, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyDirNoExist_Test_FileOpen");
    UtTest_Add(FM_VerifyDirNoExist_Test_FileClosed, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyDirNoExist_Test_FileClosed");
    UtTest_Add(FM_VerifyDirNoExist_Test_DirectoryExists, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyDirNoExist_Test_DirectoryExists");
    UtTest_Add(FM_VerifyChildTask_Test_ChildDisabled, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyChildTask_Test_ChildDisabled");
    UtTest_Add(FM_VerifyChildTask_Test_ChildQueueFull, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyChildTask_Test_ChildQueueFull");
    UtTest_Add(FM_VerifyChildTask_Test_ChildInterfaceBrokenLocalQueueCount, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyChildTask_Test_ChildInterfaceBrokenLocalQueueCount");
    UtTest_Add(FM_VerifyChildTask_Test_ChildInterfaceBrokenChildWriteIndex, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyChildTask_Test_ChildInterfaceBrokenChildWriteIndex");
    UtTest_Add(FM_VerifyChildTask_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_VerifyChildTask_Test_Nominal");
    UtTest_Add(FM_InvokeChildTask_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_InvokeChildTask_Test_Nominal");
    UtTest_Add(FM_InvokeChildTask_Test_WriteIndexEqualsQueueDepth, FM_Test_Setup, FM_Test_TearDown, "FM_InvokeChildTask_Test_WriteIndexEqualsQueueDepth");
    UtTest_Add(FM_AppendPathSep_Test, FM_Test_Setup, FM_Test_TearDown, "FM_AppendPathSep_Test");
} /* end FM_Cmd_Utils_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
