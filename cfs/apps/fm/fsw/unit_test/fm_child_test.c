 /*************************************************************************
 ** File:
 **   $Id: fm_child_test.c 1.10.1.4 2017/01/24 23:53:32EST mdeschu Exp  $
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
 **   This file contains unit test cases for the functions contained in the file fm_child.c.
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

#include "fm_child_test.h"
#include "fm_app.h"
#include "fm_child.h"
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

int32 UT_FM_CHILD_TEST_CFE_OSFILEAPI_FDGetInfoHook(int32 filedes, OS_FDTableEntry *fd_prop)
{
    fd_prop->IsValid = TRUE;

    strncpy (fd_prop->Path, "source/filename", OS_MAX_PATH_LEN);

    return 0;
} /* end UT_FM_CHILD_TEST_CFE_OSFILEAPI_FDGetInfoHook */

int32 UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHookRunCount;
int32 UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHook(const char *path, os_fstat_t  *filestats)
{
    UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHookRunCount += 1;

    if (UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHookRunCount == 1)
        return sizeof(FM_DirListEntry_t);
    else if (UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHookRunCount == 2)
        return sizeof(FM_DirListFileStats_t);
    else
        return 0;
} /* end UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHook */

int32 UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHook2(const char *path, os_fstat_t  *filestats)
{
    UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHookRunCount += 1;

    if (UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHookRunCount == 1)
        return sizeof(FM_DirListEntry_t);
    else
        return 0;
} /* end UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHook2 */

uint8 UT_FM_CFE_OSFILEAPI_ReadHookCPUHoggingRunCount;
int32 UT_FM_CFE_OSFILEAPI_ReadHookCPUHogging(int32  filedes, void *buffer, uint32 nbytes)
{
    UT_FM_CFE_OSFILEAPI_ReadHookCPUHoggingRunCount += 1;

    if (UT_FM_CFE_OSFILEAPI_ReadHookCPUHoggingRunCount == FM_CHILD_FILE_LOOP_COUNT + 1)
        return 0;
    else
        return 99;
} /* end UT_FM_CFE_OSFILEAPI_ReadHookCPUHogging */


int32 UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookIsDirectory(const char *path, os_fstat_t  *filestats)
{
    filestats->st_mode = S_IFDIR;

    return CFE_SUCCESS;
} /* end UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookIsDirectory */

int32 UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookIsFile(const char *path, os_fstat_t  *filestats)
{
    filestats->st_mode = S_IFREG;

    return CFE_SUCCESS;
} /* end UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookIsFile */

int32 UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookSetSize(const char *path, os_fstat_t  *filestats)
{
    filestats->st_size = 5;
    filestats->st_mode = 0777;

    return CFE_SUCCESS;
} /* end UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookSetSize */

os_dirent_t UT_FM_GLOBAL_DirEntry;
uint8 UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount;
os_dirent_t *UT_FM_CFE_OSFILEAPI_ReadDirHook1(os_dirp_t directory)
{
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount += 1;

    if (UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount == 1)
        strncpy (UT_FM_GLOBAL_DirEntry.d_name, "filename", 256);
    else
        return NULL;

    return &UT_FM_GLOBAL_DirEntry;
} /* end UT_FM_CFE_OSFILEAPI_ReadDirHook1 */

os_dirent_t *UT_FM_CFE_OSFILEAPI_ReadDirHook2(os_dirp_t directory)
{
    uint32  i;

    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount += 1;

    if (UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount == 1)
    {
        for (i = 0; i < OS_MAX_PATH_LEN - 1; i++)
        {
            UT_FM_GLOBAL_DirEntry.d_name[i] = 'a';
        }

        UT_FM_GLOBAL_DirEntry.d_name[OS_MAX_PATH_LEN - 1] = '\0';
    }
    else
        return NULL;

    return &UT_FM_GLOBAL_DirEntry;
} /* end UT_FM_CFE_OSFILEAPI_ReadDirHook2 */

os_dirent_t *UT_FM_CFE_OSFILEAPI_ReadDirHook3(os_dirp_t directory)
{
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount += 1;

    if (UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount == 1)
        strncpy (UT_FM_GLOBAL_DirEntry.d_name, "", 256);
    else
        return NULL;

    return &UT_FM_GLOBAL_DirEntry;
} /* end UT_FM_CFE_OSFILEAPI_ReadDirHook3 */

void FM_ChildInit_Test_CreateChildSemaphoreFailed(void)
{
    int32 Result;

    /* Set to generate error message FM_CHILD_INIT_ERR_EID */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_COUNTSEMCREATE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_ChildInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CHILD_INIT_ERR_EID, CFE_EVS_ERROR, "Child Task initialization error: create semaphore failed: result = -1"),
        "Child Task initialization error: create semaphore failed: result = -1");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_AppMain_Test_CreateChildSemaphoreFailed */

void FM_ChildInit_Test_CreateQueueSemaphoreFailed(void)
{
    int32 Result;

    /* Set to generate error message FM_CHILD_INIT_ERR_EID */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_MUTSEMCREATE_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_ChildInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CHILD_INIT_ERR_EID, CFE_EVS_ERROR, "Child Task initialization error: create queue count semaphore failed: result = -1"),
        "Child Task initialization error: create queue count semaphore failed: result = -1");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_AppMain_Test_CreateQueueSemaphoreFailed */

void FM_ChildInit_Test_CreateTaskFailed(void)
{
    int32 Result;

    /* Set to generate error message FM_CHILD_INIT_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_ChildInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CHILD_INIT_ERR_EID, CFE_EVS_ERROR, "Child Task initialization error: create task failed: result = -1"),
        "Child Task initialization error: create task failed: result = -1");

    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_AppMain_Test_CreateTaskFailed */

void FM_ChildTask_Test_Nominal(void)
{
    /* Execute the function being tested */
    FM_ChildTask();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CHILD_INIT_EID, CFE_EVS_INFORMATION, "Child Task initialization complete"),
        "Child Task initialization complete");

    /* In this test we don't care about the 2nd message (error message) being generated by the call to FM_ChildLoop */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildTask_Test_Nominal */

void FM_ChildTask_Test_RegisterChildFailed(void)
{
    /* Set to generate error message FM_CHILD_INIT_ERR_EID */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERCHILDTASK_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildTask();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CHILD_INIT_ERR_EID, CFE_EVS_ERROR, "Child Task initialization error: register child failed: result = -1"),
        "Child Task initialization error: register child failed: result = -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildTask_Test_RegisterChildFailed */

void FM_ChildLoop_Test_Nominal(void)
{
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildReadIndex = 1;

    /* Execute the function being tested */
    FM_ChildLoop();
    
    /* Verify results */

    /* In this test we don't care about the error messages being generated by the second run of the while-loop */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildLoop_Test_Nominal */

void FM_ChildLoop_Test_SemaphoreTakeFailed(void)
{
    /* Set to generate error message FM_CHILD_TERM_ERR_EID */
    Ut_OSAPI_SetReturnCode(UT_OSAPI_COUNTSEMTAKE_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildLoop();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CHILD_TERM_ERR_EID, CFE_EVS_ERROR,
                              "Child Task termination error:  semaphore take failed: result = -1"),
        "Child Task termination error:  semaphore take failed: result = -1");

    /* In this test we don't care about the error messages being generated by the second run of the while-loop */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildLoop_Test_SemaphoreTakeFailed */

void FM_ChildLoop_Test_EmptyQueue(void)
{
    FM_GlobalData.ChildQueueCount = 0;

    /* Execute the function being tested */
    FM_ChildLoop();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CHILD_TERM_ERR_EID, CFE_EVS_ERROR,
                                  "Child Task termination error:  empty queue"),
        "Child Task termination error:  empty queue");

    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");

    /* In this test we don't care about the error messages being generated by the second run of the while-loop */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildLoop_Test_EmptyQueue */

void FM_ChildLoop_Test_InvalidQueueIndex(void)
{
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildReadIndex = FM_CHILD_QUEUE_DEPTH;

    /* Execute the function being tested */
    FM_ChildLoop();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CHILD_TERM_ERR_EID, CFE_EVS_ERROR,
                                  "Child Task termination error:  invalid queue index: index = 3"),
        "Child Task termination error:  invalid queue index: index = 3");

    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");

    /* In this test we don't care about the error messages being generated by the second run of the while-loop */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildLoop_Test_InvalidQueueIndex */

void FM_ChildProcess_Test_CopyAndTooLargeReadIndex(void)
{
    FM_GlobalData.ChildReadIndex = FM_CHILD_QUEUE_DEPTH;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_COPY_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 0, "FM_GlobalData.ChildReadIndex == 0");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_CopyAndTooLargeReadIndex */

void FM_ChildProcess_Test_Move(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_MOVE_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_Move */

void FM_ChildProcess_Test_Rename(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_RENAME_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_Rename */

void FM_ChildProcess_Test_Delete(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_DELETE_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_Delete */

void FM_ChildProcess_Test_DeleteAll(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_DELETE_ALL_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_DeleteAll */

void FM_ChildProcess_Test_Decompress(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_DECOMPRESS_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_Decompress */

void FM_ChildProcess_Test_Concat(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_CONCAT_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_Concat */

void FM_ChildProcess_Test_CreateDir(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_CREATE_DIR_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_CreateDir */

void FM_ChildProcess_Test_DeleteDir(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_DELETE_DIR_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_DeleteDir */

void FM_ChildProcess_Test_GetFileInfo(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_GET_FILE_INFO_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_GetFileInfo */

void FM_ChildProcess_Test_GetDirFile(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_GET_DIR_FILE_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_GetDirFile */

void FM_ChildProcess_Test_GetDirPkt(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_GET_DIR_PKT_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_GetDirPkt */

void FM_ChildProcess_Test_DeleteInt(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_DELETE_INT_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

} /* end FM_ChildProcess_Test_DeleteInt */


void FM_ChildProcess_Test_SetPermissions(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = FM_SET_FILE_PERM_CC;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");

} /* end FM_ChildProcess_Test_SetPermissions */

void FM_ChildProcess_Test_InvalidCommandCode(void)
{
    FM_GlobalData.ChildReadIndex = 1;
    FM_GlobalData.ChildQueueCount = 1;
    FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex].CommandCode = 99;

    /* Execute the function being tested */
    FM_ChildProcess();
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildReadIndex == 2, "FM_GlobalData.ChildReadIndex == 2");
    UtAssert_True (FM_GlobalData.ChildQueueCount == 0, "FM_GlobalData.ChildQueueCount == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CHILD_EXE_ERR_EID, CFE_EVS_ERROR, "Child Task execution error: invalid command code: cc = 99"),
        "Child Task execution error: invalid command code: cc = 99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildProcess_Test_InvalidCommandCode */

void FM_ChildCopyCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = FM_COPY_CC;
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    FM_ChildCopyCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == FM_COPY_CC, "FM_GlobalData.ChildPreviousCC == FM_COPY_CC");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_COPY_CMD_EID, CFE_EVS_DEBUG, "Copy File command: src = source1, tgt = target"),
        "Copy File command: src = source1, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildCopyCmd_Test_Nominal */

void FM_ChildCopyCmd_Test_CopyFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = FM_COPY_CC;
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_COPY_OS_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CP_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildCopyCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == FM_COPY_CC, "FM_GlobalData.ChildPreviousCC == FM_COPY_CC");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_COPY_OS_ERR_EID, CFE_EVS_ERROR, "Copy File error: OS_cp failed: result = -1, src = source1, tgt = target"),
        "Copy File error: OS_cp failed: result = -1, src = source1, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildCopyCmd_Test_CopyFailed */

void FM_ChildMoveCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    FM_ChildMoveCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_MOVE_CMD_EID, CFE_EVS_DEBUG, "Move File command: src = source1, tgt = target"),
        "Move File command: src = source1, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildMoveCmd_Test_Nominal */

void FM_ChildMoveCmd_Test_MoveFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_MOVE_OS_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_MV_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildMoveCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_MOVE_OS_ERR_EID, CFE_EVS_ERROR, "Move File error: OS_mv failed: result = -1, src = source1, tgt = target"),
        "Move File error: OS_mv failed: result = -1, src = source1, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildMoveCmd_Test_MoveFailed */

void FM_ChildRenameCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    FM_ChildRenameCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_RENAME_CMD_EID, CFE_EVS_DEBUG, "Rename File command: src = source1, tgt = target"),
        "Rename File command: src = source1, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildRenameCmd_Test_Nominal */

void FM_ChildRenameCmd_Test_RenameFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_RENAME_OS_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_RENAME_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildRenameCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_RENAME_OS_ERR_EID, CFE_EVS_ERROR, "Rename File error: OS_rename failed: result = -1, src = source1, tgt = target"),
        "Rename File error: OS_rename failed: result = -1, src = source1, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildRenameCmd_Test_RenameFailed */

void FM_ChildDeleteCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    FM_ChildDeleteCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_CMD_EID, CFE_EVS_DEBUG, "Delete File command: file = source1"),
        "Delete File command: file = source1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDeleteCmd_Test_Nominal */

void FM_ChildDeleteCmd_Test_RemoveFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_DELETE_OS_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_REMOVE_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildDeleteCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_OS_ERR_EID, CFE_EVS_ERROR, "Delete File error: OS_remove failed: result = -1, file = source1"),
        "Delete File error: OS_remove failed: result = -1, file = source1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDeleteCmd_Test_RemoveFailed */

void FM_ChildDeleteAllCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source/", OS_MAX_PATH_LEN);

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 100, 1);

    /* Causes while-loop to run exactly one time, and sets DirEntry->d_name so nominal case is reached */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);

    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Execute the function being tested */
    FM_ChildDeleteAllCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_CMD_EID, CFE_EVS_DEBUG, "Delete All Files command: deleted 1 files: dir = source"),
        "Delete All Files command: deleted 1 files: dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDeleteAllCmd_Test_Nominal */

void FM_ChildDeleteAllCmd_Test_opendirFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source/", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_DELETE_ALL_OS_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 0, 1);

    /* Execute the function being tested */
    FM_ChildDeleteAllCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_OS_ERR_EID, CFE_EVS_ERROR, "Delete All Files error: OS_opendir failed: dir = source"),
        "Delete All Files error: OS_opendir failed: dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDeleteAllCmd_Test_opendirFailed */

void FM_ChildDeleteAllCmd_Test_FilesNotDeletedFilenameTooLarge(void)
{
    FM_ChildQueueEntry_t *CmdArgs;
    uint32  i;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);

    for (i = 0; i < OS_MAX_PATH_LEN - 1; i++)
    {
        CmdArgs->Source2[i] = 'a';
    }

    CmdArgs->Source2[OS_MAX_PATH_LEN - 1] = '\0';

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 100, 1);

    /* Causes while-loop to run exactly one time, and sets DirEntry->d_name to satisfy strcmp condition */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Execute the function being tested */
    FM_ChildDeleteAllCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 1, "FM_GlobalData.ChildCmdWarnCounter == 1");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_CMD_EID, CFE_EVS_DEBUG, "Delete All Files command: deleted 0 files: dir = source"),
        "Delete All Files command: deleted 0 files: dir = source");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_WARNING_EID, CFE_EVS_INFORMATION, "Delete All Files command: one or more files could not be deleted. Files may be open : dir = source"),
        "Delete All Files command: one or more files could not be deleted. Files may be open : dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildDeleteAllCmd_Test_FilesNotDeletedFilenameTooLarge */

void FM_ChildDeleteAllCmd_Test_FilesNotDeletedNameIsInvalid(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "", OS_MAX_PATH_LEN);

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 100, 1);

    /* Set to get to condition "FilenameState == FM_NAME_IS_INVALID", which evaluates to true due to Filename */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook3);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Execute the function being tested */
    FM_ChildDeleteAllCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 1, "FM_GlobalData.ChildCmdWarnCounter == 1");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_CMD_EID, CFE_EVS_DEBUG, "Delete All Files command: deleted 0 files: dir = source"),
        "Delete All Files command: deleted 0 files: dir = source");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_WARNING_EID, CFE_EVS_INFORMATION, "Delete All Files command: one or more files could not be deleted. Files may be open : dir = source"),
        "Delete All Files command: one or more files could not be deleted. Files may be open : dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildDeleteAllCmd_Test_FilesNotDeletedNameIsInvalid */

void FM_ChildDeleteAllCmd_Test_FilesNotDeletedNameIsNotInUse(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source", OS_MAX_PATH_LEN);

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 100, 1);

    /* Causes while-loop to run exactly one time, and sets DirEntry->d_name to satisfy strcmp condition */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Set to make FM_GetFilenameState return FM_NAME_IS_NOT_IN_USE, to satisfy condition "FilenameState == FM_NAME_IS_NOT_IN_USE" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildDeleteAllCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 1, "FM_GlobalData.ChildCmdWarnCounter == 1");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_CMD_EID, CFE_EVS_DEBUG, "Delete All Files command: deleted 0 files: dir = source"),
        "Delete All Files command: deleted 0 files: dir = source");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_WARNING_EID, CFE_EVS_INFORMATION, "Delete All Files command: one or more files could not be deleted. Files may be open : dir = source"),
        "Delete All Files command: one or more files could not be deleted. Files may be open : dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildDeleteAllCmd_Test_FilesNotDeletedNameIsNotInUse */

void FM_ChildDeleteAllCmd_Test_FilesNotDeletedFileOpen(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source/", OS_MAX_PATH_LEN);

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 100, 1);

    /* Causes while-loop to run exactly one time, and sets DirEntry->d_name to satisfy strcmp condition */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookIsFile);

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_FILE_OPEN" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_FDGETINFO_INDEX, &UT_FM_CHILD_TEST_CFE_OSFILEAPI_FDGetInfoHook);

    /* Execute the function being tested */
    FM_ChildDeleteAllCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 1, "FM_GlobalData.ChildCmdWarnCounter == 1");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_CMD_EID, CFE_EVS_DEBUG, "Delete All Files command: deleted 0 files: dir = source"),
        "Delete All Files command: deleted 0 files: dir = source");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_WARNING_EID, CFE_EVS_INFORMATION, "Delete All Files command: one or more files could not be deleted. Files may be open : dir = source"),
        "Delete All Files command: one or more files could not be deleted. Files may be open : dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildDeleteAllCmd_Test_FilesNotDeletedFileOpen */

void FM_ChildDeleteAllCmd_Test_FilesNotDeletedFileClosed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source/", OS_MAX_PATH_LEN);

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 100, 1);

    /* Causes while-loop to run exactly one time, and sets DirEntry->d_name to satisfy strcmp condition */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Set to fail condition "(OS_Status = OS_remove(Filename)) == OS_SUCCESS" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_REMOVE_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildDeleteAllCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 1, "FM_GlobalData.ChildCmdWarnCounter == 1");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_CMD_EID, CFE_EVS_DEBUG, "Delete All Files command: deleted 0 files: dir = source"),
        "Delete All Files command: deleted 0 files: dir = source");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_WARNING_EID, CFE_EVS_INFORMATION, "Delete All Files command: one or more files could not be deleted. Files may be open : dir = source"),
        "Delete All Files command: one or more files could not be deleted. Files may be open : dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildDeleteAllCmd_Test_FilesNotDeletedFileClosed */

void FM_ChildDeleteAllCmd_Test_DirectorySkipped(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source/", OS_MAX_PATH_LEN);

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 100, 1);

    /* Causes while-loop to run exactly one time, and sets DirEntry->d_name to satisfy strcmp condition */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Set to satisfy condition "FilenameState == FM_NAME_IS_DIRECTORY" */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookIsDirectory);

    /* Execute the function being tested */
    FM_ChildDeleteAllCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_CMD_EID, CFE_EVS_DEBUG, "Delete All Files command: deleted 0 files: dir = source"),
        "Delete All Files command: deleted 0 files: dir = source");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_ALL_WARNING_EID, CFE_EVS_INFORMATION, "Delete All Files command: one or more directories skipped : dir = source"),
        "Delete All Files command: one or more directories skipped : dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildDeleteAllCmd_Test_DirectorySkipped */

void FM_ChildDecompressCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    FM_ChildDecompressCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DECOM_CMD_EID, CFE_EVS_DEBUG, "Decompress File command: src = source, tgt = target"),
        "Decompress File command: src = source, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDecompressCmd_Test_Nominal */

void FM_ChildDecompressCmd_Test_DecompressFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_DECOM_CFE_ERR_EID */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_DECOMPRESS_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildDecompressCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DECOM_CFE_ERR_EID, CFE_EVS_ERROR, "Decompress File error: CFE_FS_Decompress failed: result = -1, src = source, tgt = target"),
        "Decompress File error: CFE_FS_Decompress failed: result = -1, src = source, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDecompressCmd_Test_DecompressFailed */

void FM_ChildConcatCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source2", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    FM_ChildConcatCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CONCAT_CMD_EID, CFE_EVS_DEBUG, "Concat Files command: src1 = source1, src2 = source2, tgt = target"),
        "Concat Files command: src1 = source1, src2 = source2, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildConcatCmd_Test_Nominal */

void FM_ChildConcatCmd_Test_CopyFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source2", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_CONCAT_OS_ERR_EID for source file 1 */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CP_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildConcatCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CONCAT_OS_ERR_EID, CFE_EVS_ERROR, "Concat Files error: OS_cp failed: result = -1, src = source1, tgt = target"),
        "Concat Files error: OS_cp failed: result = -1, src = source1, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildConcatCmd_Test_CopyFailed */

void FM_ChildConcatCmd_Test_OpenFailed1(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source2", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_CONCAT_OS_ERR_EID for source file 2 */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPEN_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildConcatCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CONCAT_OS_ERR_EID, CFE_EVS_ERROR, "Concat Files error: OS_open failed: result = -1, src2 = source2"),
        "Concat Files error: OS_open failed: result = -1, src2 = source2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildConcatCmd_Test_OpenFailed1 */

void FM_ChildConcatCmd_Test_OpenFailed2(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source2", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_CONCAT_OS_ERR_EID for target file */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPEN_INDEX, -1, 2);

    /* Execute the function being tested */
    FM_ChildConcatCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CONCAT_OS_ERR_EID, CFE_EVS_ERROR, "Concat Files error: OS_open failed: result = -1, tgt = target"),
        "Concat Files error: OS_open failed: result = -1, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildConcatCmd_Test_OpenFailed2 */

void FM_ChildConcatCmd_Test_ReadFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source2", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_CONCAT_OS_ERR_EID for OS_read */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildConcatCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CONCAT_OS_ERR_EID, CFE_EVS_ERROR, "Concat Files error: OS_read failed: result = -1, file = source2"),
        "Concat Files error: OS_read failed: result = -1, file = source2");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildConcatCmd_Test_ReadFailed */

void FM_ChildConcatCmd_Test_WriteFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source2", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Set to fail conditions "BytesRead == 0" and "BytesRead < 0", in order to reach else case */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, 1, 1);

    /* Set to generate error message FM_CONCAT_OS_ERR_EID for OS_write */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildConcatCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CONCAT_OS_ERR_EID, CFE_EVS_ERROR, "Concat Files error: OS_write failed: result = -1, expected = 1"),
        "Concat Files error: OS_write failed: result = -1, expected = 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildConcatCmd_Test_WriteFailed */

void FM_ChildConcatCmd_Test_CPUHogging(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source2", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Needed to reach CPU hogging prevention code (starts with comment "Avoid CPU hogging") */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_FM_CFE_OSFILEAPI_ReadHookCPUHogging);
    UT_FM_CFE_OSFILEAPI_ReadHookCPUHoggingRunCount = 0;
    
    /* Needed to reach CPU hogging prevention code (starts with comment "Avoid CPU hogging") */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, 99, 1);
    Ut_OSFILEAPI_ContinueReturnCodeAfterCountZero(UT_OSFILEAPI_WRITE_INDEX);

    /* Execute the function being tested */
    FM_ChildConcatCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1"); /* Command succeeds on final loop */
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    /* Command succeeds on final loop */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CONCAT_CMD_EID, CFE_EVS_DEBUG, "Concat Files command: src1 = source1, src2 = source2, tgt = target"),
        "Concat Files command: src1 = source1, src2 = source2, tgt = target");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildConcatCmd_Test_CPUHogging */

void FM_ChildFileInfoCmd_Test_FinishedReading(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    CmdArgs->FileInfoSize = 0;
    CmdArgs->FileInfoTime = 0;
    CmdArgs->FileInfoCRC = CFE_ES_CRC_8;
    CmdArgs->FileInfoState = FM_NAME_IS_FILE_CLOSED;

    /* Execute the function being tested */
    FM_ChildFileInfoCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True (FM_GlobalData.FileInfoPkt.CRC_Computed == TRUE, "FM_GlobalData.FileInfoPkt.CRC_Computed == TRUE");
    UtAssert_True (FM_GlobalData.FileInfoPkt.CRC == 0, "FM_GlobalData.FileInfoPkt.CRC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_FILE_INFO_CMD_EID, CFE_EVS_DEBUG, "Get File Info command: file = source1"),
        "Get File Info command: file = source1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildFileInfoCmd_Test_FinishedReading */

void FM_ChildFileInfoCmd_Test_ContinueCRC(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    CmdArgs->FileInfoSize = 0;
    CmdArgs->FileInfoTime = 0;
    CmdArgs->FileInfoCRC = CFE_ES_CRC_8;
    CmdArgs->FileInfoState = FM_NAME_IS_FILE_CLOSED;

    /* Set to fail conditions "BytesRead == 0" and "BytesRead < 0" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, 99, 1);

    /* Set to return a value for FM_GlobalData.FileInfoPkt.CRC that will be verified in a UtAssert_True statement */
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CALCULATECRC_INDEX, 99, 1);

    /* Execute the function being tested */
    FM_ChildFileInfoCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True (FM_GlobalData.FileInfoPkt.CRC_Computed == TRUE, "FM_GlobalData.FileInfoPkt.CRC_Computed == TRUE");
    UtAssert_True (FM_GlobalData.FileInfoPkt.CRC == 99, "FM_GlobalData.FileInfoPkt.CRC == 99");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_FILE_INFO_CMD_EID, CFE_EVS_DEBUG, "Get File Info command: file = source1"),
        "Get File Info command: file = source1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildFileInfoCmd_Test_ContinueCRC */

void FM_ChildFileInfoCmd_Test_CPUHogging(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    CmdArgs->FileInfoSize = 0;
    CmdArgs->FileInfoTime = 0;
    CmdArgs->FileInfoCRC = CFE_ES_CRC_8;
    CmdArgs->FileInfoState = FM_NAME_IS_FILE_CLOSED;

    /* Needed to reach CPU hogging prevention code (starts with comment "Avoid CPU hogging") */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_FM_CFE_OSFILEAPI_ReadHookCPUHogging);
    UT_FM_CFE_OSFILEAPI_ReadHookCPUHoggingRunCount = 0;

    /* Execute the function being tested */
    FM_ChildFileInfoCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildFileInfoCmd_Test_CPUHogging */

void FM_ChildFileInfoCmd_Test_InvalidFileState(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    CmdArgs->FileInfoSize = 0;
    CmdArgs->FileInfoTime = 0;
    CmdArgs->FileInfoCRC = 99;
    CmdArgs->FileInfoState = 99;

    /* Execute the function being tested */
    FM_ChildFileInfoCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 1, "FM_GlobalData.ChildCmdWarnCounter == 1");
    UtAssert_True (CmdArgs->FileInfoCRC == FM_IGNORE_CRC, "CmdArgs->FileInfoCRC == FM_IGNORE_CRC");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_FILE_INFO_WARNING_EID, CFE_EVS_INFORMATION, "Get File Info warning: unable to compute CRC: invalid file state = 99, file = source1"),
        "Get File Info warning: unable to compute CRC: invalid file state = 99, file = source1");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildFileInfoCmd_Test_InvalidFileState */

void FM_ChildFileInfoCmd_Test_InvalidCRCType(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    CmdArgs->FileInfoSize = 0;
    CmdArgs->FileInfoTime = 0;
    CmdArgs->FileInfoCRC = 99;
    CmdArgs->FileInfoState = FM_NAME_IS_FILE_CLOSED;

    /* Execute the function being tested */
    FM_ChildFileInfoCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 1, "FM_GlobalData.ChildCmdWarnCounter == 1");
    UtAssert_True (CmdArgs->FileInfoCRC == FM_IGNORE_CRC, "CmdArgs->FileInfoCRC == FM_IGNORE_CRC");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_FILE_INFO_WARNING_EID, CFE_EVS_INFORMATION, "Get File Info warning: unable to compute CRC: invalid CRC type = 99, file = source1"),
        "Get File Info warning: unable to compute CRC: invalid CRC type = 99, file = source1");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildFileInfoCmd_Test_InvalidCRCType */

void FM_ChildFileInfoCmd_Test_OpenFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    CmdArgs->FileInfoSize = 0;
    CmdArgs->FileInfoTime = 0;
    CmdArgs->FileInfoCRC = CFE_ES_CRC_8;
    CmdArgs->FileInfoState = FM_NAME_IS_FILE_CLOSED;

    /* Set to generate error message FM_GET_FILE_INFO_WARNING_EID for call to OS_open */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPEN_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildFileInfoCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 1, "FM_GlobalData.ChildCmdWarnCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_FILE_INFO_CMD_EID, CFE_EVS_DEBUG, "Get File Info command: file = source1"),
        "Get File Info command: file = source1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_FILE_INFO_WARNING_EID, CFE_EVS_ERROR, "Get File Info warning: unable to compute CRC: OS_open result = -1, file = source1"),
        "Get File Info warning: unable to compute CRC: OS_open result = -1, file = source1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildFileInfoCmd_Test_OpenFailed */

void FM_ChildFileInfoCmd_Test_ReadFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    CmdArgs->FileInfoSize = 0;
    CmdArgs->FileInfoTime = 0;
    CmdArgs->FileInfoCRC = CFE_ES_CRC_8;
    CmdArgs->FileInfoState = FM_NAME_IS_FILE_CLOSED;

    /* Set to generate error message FM_GET_FILE_INFO_WARNING_EID for call to OS_read */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildFileInfoCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True (FM_GlobalData.FileInfoPkt.CRC == 0, "FM_GlobalData.FileInfoPkt.CRC == 0");
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 1, "FM_GlobalData.ChildCmdWarnCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_FILE_INFO_CMD_EID, CFE_EVS_DEBUG, "Get File Info command: file = source1"),
        "Get File Info command: file = source1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_FILE_INFO_WARNING_EID, CFE_EVS_INFORMATION, "Get File Info warning: unable to compute CRC: OS_read result = -1, file = source1"),
        "Get File Info warning: unable to compute CRC: OS_read result = -1, file = source1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildFileInfoCmd_Test_ReadFailed */

void FM_ChildCreateDirCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);

    /* Execute the function being tested */
     FM_ChildCreateDirCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CREATE_DIR_CMD_EID, CFE_EVS_DEBUG, "Create Directory command: src = source"),
        "Create Directory command: src = source1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildCreateDirCmd_Test_Nominal */

void FM_ChildCreateDirCmd_Test_mkdirFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_CREATE_DIR_OS_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_MKDIR_INDEX, -1, 1);

    /* Execute the function being tested */
     FM_ChildCreateDirCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_CREATE_DIR_OS_ERR_EID, CFE_EVS_ERROR, "Create Directory error: OS_mkdir failed: result = -1, dir = source"),
        "Create Directory error: OS_mkdir failed: result = -1, dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildCreateDirCmd_Test_mkdirFailed */

void FM_ChildDeleteDirCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_DELETE_DIR_CMD_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 1, 1);

    /* Execute the function being tested */
     FM_ChildDeleteDirCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_DIR_CMD_EID, CFE_EVS_DEBUG, "Delete Directory command: src = source"),
        "Delete Directory command: src = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDeleteDirCmd_Test_Nominal */

void FM_ChildDeleteDirCmd_Test_OpenDirFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_DELETE_DIR_OS_ERR_EID for call to OS_opendir */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 0, 1);

    /* Execute the function being tested */
     FM_ChildDeleteDirCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_DIR_OS_ERR_EID, CFE_EVS_ERROR, "Delete Directory error: OS_opendir failed: dir = source"),
        "Delete Directory error: OS_opendir failed: dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDeleteDirCmd_Test_DirectoryNotEmpty */

void FM_ChildDeleteDirCmd_Test_DirectoryNotEmpty(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 1, 1);

    /* Causes error message FM_DELETE_DIR_EMPTY_ERR_EID */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Execute the function being tested */
     FM_ChildDeleteDirCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_DIR_EMPTY_ERR_EID, CFE_EVS_ERROR, "Delete Directory error: directory is not empty: dir = source"),
        "Delete Directory error: directory is not empty: dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDeleteDirCmd_Test_DirectoryNotEmpty */

void FM_ChildDeleteDirCmd_Test_rmdirFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source", OS_MAX_PATH_LEN);

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 1, 1);

    /* Set to generate error message FM_DELETE_DIR_OS_ERR_EID for call to OS_rmdir */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_RMDIR_INDEX, -1, 1);

    /* Execute the function being tested */
     FM_ChildDeleteDirCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_DELETE_DIR_OS_ERR_EID, CFE_EVS_ERROR, "Delete Directory error: OS_rmdir failed: result = -1, dir = source"),
        "Delete Directory error: directory is not empty: dir = source");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDeleteDirCmd_Test_rmdirFailed */

void FM_ChildDirListFileCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source2", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 1, 1);

    /* Needed to make subfunction FM_ChildDirListFileInit return TRUE */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, OS_SUCCESS, 1);

    /* Needed to make subfunction FM_ChildDirListFileInit return TRUE */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Needed to make subfunction FM_ChildDirListFileInit return TRUE */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(FM_DirListFileStats_t), 1);

    /* Execute the function being tested */
     FM_ChildDirListFileCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDirListFileCmd_Test_Nominal */

void FM_ChildDirListFileCmd_Test_opendirFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source2", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Target, "target", OS_MAX_PATH_LEN);

    /* Set to generate error message FM_GET_DIR_FILE_OS_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 0, 1);

    /* Execute the function being tested */
     FM_ChildDirListFileCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR, "Directory List to File error: OS_opendir failed: dir = source1"),
        "Directory List to File error: OS_opendir failed: dir = source1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDirListFileCmd_Test_opendirFailed */

void FM_ChildDirListPktCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source2", OS_MAX_PATH_LEN);
    CmdArgs->DirListOffset = 0;
    CmdArgs->GetSizeTimeMode = TRUE;

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 1, 1);

    FM_GlobalData.DirListPkt.TotalFiles = FM_GlobalData.DirListPkt.FirstFile + 1;

    /* Sets DirEntry->d_name to pass strcmp conditions that check its value */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Execute the function being tested */
     FM_ChildDirListPktCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True (strncmp(FM_GlobalData.DirListPkt.DirName, "source1", OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.DirListPkt.DirName, 'source1', OS_MAX_PATH_LEN) == 0");
    UtAssert_True (FM_GlobalData.DirListPkt.FirstFile == 0, "FM_GlobalData.DirListPkt.FirstFile == 0");
    UtAssert_True (FM_GlobalData.DirListPkt.TotalFiles == 1, "FM_GlobalData.DirListPkt.TotalFiles == 1");
    UtAssert_True
        (strncmp(FM_GlobalData.DirListPkt.FileList[0].EntryName, UT_FM_GLOBAL_DirEntry.d_name, OS_MAX_PATH_LEN) == 0,
        "strncmp(FM_GlobalData.DirListPkt.FileList[0].EntryName, UT_FM_GLOBAL_DirEntry.d_name, OS_MAX_PATH_LEN) == 0");

    UtAssert_True (FM_GlobalData.DirListPkt.PacketFiles == 1, "FM_GlobalData.DirListPkt.PacketFiles == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_PKT_CMD_EID, CFE_EVS_DEBUG, "Directory List to Packet command: offset = 0, dir = source1"),
        "Directory List to Packet command: offset = 0, dir = source1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDirListPktCmd_Test_Nominal */

void FM_ChildDirListPktCmd_Test_opendirFailed(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    strncpy(CmdArgs->Source2, "source2", OS_MAX_PATH_LEN);
    CmdArgs->DirListOffset = 0;

    /* Set to generate error message FM_GET_DIR_PKT_OS_ERR_EID */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 0, 1);

    /* Execute the function being tested */
     FM_ChildDirListPktCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_PKT_OS_ERR_EID, CFE_EVS_ERROR, "Directory List to Packet error: OS_opendir failed: dir = source1"),
        "Directory List to Packet error: OS_opendir failed: dir = source1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDirListPktCmd_Test_opendirFailed */

void FM_ChildDirListPktCmd_Test_DirPlusEntryTooLong(void)
{
    FM_ChildQueueEntry_t *CmdArgs;
    uint32  i;
    char EventMessage1[CFE_EVS_MAX_MESSAGE_LENGTH];

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = 1; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "source1", OS_MAX_PATH_LEN);
    CmdArgs->DirListOffset = 0;

    for (i = 0; i < OS_MAX_PATH_LEN - 1; i++)
    {
        CmdArgs->Source2[i] = 'a';
    }

    CmdArgs->Source2[OS_MAX_PATH_LEN - 1] = '\0';

    /* Set to fail condition "DirPtr == NULL" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPENDIR_INDEX, 1, 1);

    FM_GlobalData.DirListPkt.TotalFiles = FM_GlobalData.DirListPkt.FirstFile + 1;

    /* Sets DirEntry->d_name to pass strcmp conditions that check its value */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Execute the function being tested */
     FM_ChildDirListPktCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 1, "FM_GlobalData.ChildCmdWarnCounter == 1");
    UtAssert_True (FM_GlobalData.ChildPreviousCC == 1, "FM_GlobalData.ChildPreviousCC == 1");
    UtAssert_True (FM_GlobalData.ChildCurrentCC == 0, "FM_GlobalData.ChildCurrentCC == 0");

    UtAssert_True (strncmp(FM_GlobalData.DirListPkt.DirName, "source1", OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.DirListPkt.DirName, 'source1', OS_MAX_PATH_LEN) == 0");
    UtAssert_True (FM_GlobalData.DirListPkt.FirstFile == 0, "FM_GlobalData.DirListPkt.FirstFile == 0");
    UtAssert_True (FM_GlobalData.DirListPkt.TotalFiles == 1, "FM_GlobalData.DirListPkt.TotalFiles == 1");

    strncpy(EventMessage1, "Directory List to Packet warning: dir + entry is too long: dir = ", CFE_EVS_MAX_MESSAGE_LENGTH);
    strncat(EventMessage1, CmdArgs->Source2, CFE_EVS_MAX_MESSAGE_LENGTH - 66);

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_PKT_WARNING_EID, CFE_EVS_INFORMATION, EventMessage1),
        EventMessage1);

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_PKT_CMD_EID, CFE_EVS_DEBUG, "Directory List to Packet command: offset = 0, dir = source1"),
        "Directory List to Packet command: offset = 0, dir = source1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildDirListPktCmd_Test_DirPlusEntryTooLong */

void FM_ChildDirListFileInit_Test_Nominal(void)
{
    boolean Result;
    int32 FileHandle = 0;

    /* Set to satisfy condition "FileHandle >= OS_SUCCESS" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 1, 1);

    /* Set to satisfy condition "BytesWritten == sizeof(CFE_FS_Header_t)" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Set to satisfy condition "BytesWritten == sizeof(FM_DirListFileStats_t)" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, sizeof(FM_DirListFileStats_t), 1);

    /* Execute the function being tested */
    Result = FM_ChildDirListFileInit(&FileHandle, "directory", "filename");
    
    /* Verify results */
    UtAssert_True (Result == TRUE, "Result == TRUE");
    UtAssert_True (strncmp(FM_GlobalData.DirListFileStats.DirName, "directory", OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.DirListFileStats.DirName, 'directory', OS_MAX_PATH_LEN) == 0");
    UtAssert_True (FileHandle == 1, "FileHandle == 1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_ChildDirListFileInit_Test_Nominal */

void FM_ChildDirListFileInit_Test_WriteBlankStatsFailed(void)
{
    boolean Result;
    int32 FileHandle = 0;
    char EventMessage[CFE_EVS_MAX_MESSAGE_LENGTH];

    /* Set to satisfy condition "FileHandle >= OS_SUCCESS" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 1, 1);

    /* Set to satisfy condition "BytesWritten == sizeof(CFE_FS_Header_t)" */
    Ut_CFE_FS_SetReturnCode(UT_CFE_FS_WRITEHDR_INDEX, sizeof(CFE_FS_Header_t), 1);

    /* Set to generate error message FM_GET_DIR_FILE_OS_ERR_EID for call to OS_write */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, 0, 1);

    /* Execute the function being tested */
    Result = FM_ChildDirListFileInit(&FileHandle, "directory", "filename");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (strncmp(FM_GlobalData.DirListFileStats.DirName, "directory", OS_MAX_PATH_LEN) == 0, "strncmp(FM_GlobalData.DirListFileStats.DirName, 'directory', OS_MAX_PATH_LEN) == 0");
    UtAssert_True (FileHandle == 0, "FileHandle == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");

    sprintf(EventMessage, "Directory List to File error: OS_write blank stats failed: result = 0, expected = %d", sizeof(FM_DirListFileStats_t));
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR, EventMessage),
        EventMessage);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDirListFileInit_Test_WriteBlankStatsFailed */

void FM_ChildDirListFileInit_Test_WriteHeaderFailed(void)
{
    boolean Result;
    int32 FileHandle = 0;

    /* Set to satisfy condition "FileHandle >= OS_SUCCESS" */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, 1, 1);

    /* Execute the function being tested */
    Result = FM_ChildDirListFileInit(&FileHandle, "directory", "filename");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (FileHandle == 0, "FileHandle == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR, "Directory List to File error: CFE_FS_WriteHeader failed: result = 0, expected = 64"),
        "Directory List to File error: CFE_FS_WriteHeader failed: result = 0, expected = 64");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDirListFileInit_Test_WriteHeaderFailed */

void FM_ChildDirListFileInit_Test_creatFailed(void)
{
    boolean Result;
    int32 FileHandle = 0;

    /* Set to generate error message FM_GET_DIR_FILE_OS_ERR_EID for call to OS_creat */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_ChildDirListFileInit(&FileHandle, "directory", "filename");
    
    /* Verify results */
    UtAssert_True (Result == FALSE, "Result == FALSE");
    UtAssert_True (FileHandle == 0, "FileHandle == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR, "Directory List to File error: OS_creat failed: result = -1, file = filename"),
        "Directory List to File error: OS_creat failed: result = -1, file = filename");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDirListFileInit_Test_creatFailed */

void FM_ChildDirListFileLoop_Test_Nominal(void)
{
    int32 FileHandle = 0;
    os_dirp_t DirPtr = 0;
    uint8 getSizeTimeMode = TRUE;

    /* Sets DirEntry->d_name to pass strcmp conditions that check its value */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Prevents errors in both calls to OS_write */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_WRITE_INDEX, &UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHook);
    UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHookRunCount = 0;

    /* Execute the function being tested */
    FM_ChildDirListFileLoop(DirPtr, FileHandle, "directory", "directory/", "filename", getSizeTimeMode);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.DirListFileStats.DirEntries == 1, "FM_GlobalData.DirListFileStats.DirEntries == 1");
    UtAssert_True (FM_GlobalData.DirListFileStats.FileEntries == 1, "FM_GlobalData.DirListFileStats.FileEntries == 1");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_FILE_CMD_EID, CFE_EVS_DEBUG, "Directory List to File command: wrote 1 of 1 names: dir = directory, filename = filename"),
        "Directory List to File command: wrote 1 of 1 names: dir = directory, filename = filename");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDirListFileLoop_Test_Nominal */


void FM_ChildDirListFileLoop_Test_ExcludeFileTimeSizeMode(void)
{
    int32 FileHandle = 0;
    os_dirp_t DirPtr = 0;
    uint8 getSizeTimeMode = FALSE;

    /* Sets DirEntry->d_name to pass strcmp conditions that check its value */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Prevents errors in both calls to OS_write */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_WRITE_INDEX, &UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHook);
    UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHookRunCount = 0;

    /* Execute the function being tested */
    FM_ChildDirListFileLoop(DirPtr, FileHandle, "directory", "directory/", "filename", getSizeTimeMode);
    
    /* Verify results */
    
    /* There is no way to assert this as the variable in which size time and mode are store is
       local to the FM_ChildDirListFileLoop */
    UtAssert_True (FM_GlobalData.DirListFileStats.DirEntries == 1, "FM_GlobalData.DirListFileStats.DirEntries == 1");
    UtAssert_True (FM_GlobalData.DirListFileStats.FileEntries == 1, "FM_GlobalData.DirListFileStats.FileEntries == 1");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_FILE_CMD_EID, CFE_EVS_DEBUG, "Directory List to File command: wrote 1 of 1 names: dir = directory, filename = filename"),
        "Directory List to File command: wrote 1 of 1 names: dir = directory, filename = filename");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDirListFileLoop_Test_ExcludeFileTimeSizeMode */

void FM_ChildDirListFileLoop_Test_WriteEntryFailed(void)
{
    int32 FileHandle = 0;
    os_dirp_t DirPtr = 0;
    char EventMessage[CFE_EVS_MAX_MESSAGE_LENGTH];
    uint8 getSizeTimeMode = TRUE;

    /* Sets DirEntry->d_name to pass strcmp conditions that check its value */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;


    /* Set to generate 1st instance of error message FM_GET_DIR_FILE_OS_ERR_EID ("OS_write entry failed") */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, 0, 1);

    /* Execute the function being tested */
    FM_ChildDirListFileLoop(DirPtr, FileHandle, "directory", "directory/", "filename", getSizeTimeMode);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");

    sprintf(EventMessage, "Directory List to File error: OS_write entry failed: result = 0, expected = %d", sizeof(FM_DirListEntry_t));
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR, EventMessage),
        EventMessage);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDirListFileLoop_Test_WriteEntryFailed */

void FM_ChildDirListFileLoop_Test_DirectoryAndNameTooLong(void)
{
    int32 FileHandle = 0;
    os_dirp_t DirPtr = 0;
    uint8 getSizeTimeMode = TRUE;

    uint32  i;
    char EntryText[CFE_EVS_MAX_MESSAGE_LENGTH];
    char EventMessage1[CFE_EVS_MAX_MESSAGE_LENGTH];

    /* Causes event message FM_GET_DIR_FILE_WARNING_EID to be generated */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook2);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Execute the function being tested */
    FM_ChildDirListFileLoop(DirPtr, FileHandle, "directory", "directory/", "filename", getSizeTimeMode);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.ChildCmdWarnCounter == 1, "FM_GlobalData.ChildCmdWarnCounter == 1");

    for (i = 0; i < OS_MAX_PATH_LEN - 1; i++)
    {
        EntryText[i] = 'a';
    }

    EntryText[OS_MAX_PATH_LEN - 1] = '\0';

    strncpy(EventMessage1, "Directory List to File error: combined directory and entry name too long: dir = directory, entry = ", CFE_EVS_MAX_MESSAGE_LENGTH);
    strncat(EventMessage1, EntryText, CFE_EVS_MAX_MESSAGE_LENGTH - 100);

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_FILE_WARNING_EID, CFE_EVS_INFORMATION, EventMessage1),
        EventMessage1);

    UtAssert_True (FM_GlobalData.DirListFileStats.DirEntries == 1, "FM_GlobalData.DirListFileStats.DirEntries == 1");
    UtAssert_True (FM_GlobalData.DirListFileStats.FileEntries == 0, "FM_GlobalData.DirListFileStats.FileEntries == 0");
    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_FILE_CMD_EID, CFE_EVS_DEBUG, "Directory List to File command: wrote 0 of 1 names: dir = directory, filename = filename"),
        "Directory List to File command: wrote 0 of 1 names: dir = directory, filename = filename");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end FM_ChildDirListFileLoop_Test_DirectoryAndNameTooLong */

void FM_ChildDirListFileLoop_Test_WriteUpdateStatsFailed(void)
{
    int32 FileHandle = 0;
    os_dirp_t DirPtr = 0;
    uint8 getSizeTimeMode = TRUE;
    char EventMessage[CFE_EVS_MAX_MESSAGE_LENGTH];

    /* Sets DirEntry->d_name to pass strcmp conditions that check its value */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READDIR_INDEX, &UT_FM_CFE_OSFILEAPI_ReadDirHook1);
    UT_FM_CFE_OSFILEAPI_ReadDirHookRunCount = 0;

    /* Generate 2nd instance of error message FM_GET_DIR_FILE_OS_ERR_EID ("OS_write update stats failed") */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_WRITE_INDEX, &UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHook2);
    UT_FM_CHILD_TEST_CFE_OSFILEAPI_WriteHookRunCount = 0;

    /* Execute the function being tested */
    FM_ChildDirListFileLoop(DirPtr, FileHandle, "directory", "directory/", "filename", getSizeTimeMode);
    
    /* Verify results */
    UtAssert_True (FM_GlobalData.DirListFileStats.DirEntries == 1, "FM_GlobalData.DirListFileStats.DirEntries == 1");
    UtAssert_True (FM_GlobalData.DirListFileStats.FileEntries == 1, "FM_GlobalData.DirListFileStats.FileEntries == 1");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");

    sprintf(EventMessage, "Directory List to File error: OS_write update stats failed: result = 0, expected = %d", sizeof(FM_DirListFileStats_t));
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR, EventMessage),
        EventMessage);

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildDirListFileLoop_Test_WriteUpdateStatsFailed */

void FM_ChildSizeAndTime_Test_Nominal(void)
{
    int32  Result   = 0;
    uint32 FileSize = 1;
    uint32 FileTime = 1;
    uint32 FileMode = 0555;

    /* Sets FileStatus->st_size = 5, which FileSize is then set to.  FileSize is verified in a UtAssert statement below. */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookSetSize);

    /* Set to return a specific value, which is checked in a UtAssert statement below */
    Ut_CFE_TIME_SetReturnCode(UT_CFE_TIME_FS2CFESECONDS_INDEX, 10, 1);

    /* Execute the function being tested */
    Result = FM_ChildSizeTimeMode("filename", &FileSize, &FileTime, &FileMode);
    
    /* Verify results */
    UtAssert_True (FileTime == 10, "FileTime == 10");
    UtAssert_True (FileSize == 5, "FileSize == 5");
    UtAssert_True (FileMode == 0777, "FileMode == 0777");
    UtAssert_True (Result == OS_SUCCESS, "Result == OS_SUCCESS");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_ChildSizeAndTime_Test_Nominal */

void FM_ChildSizeAndTime_Test_statError(void)
{
    int32  Result   = 0;
    uint32 FileSize = 1;
    uint32 FileTime = 1;
    uint32 FileMode = 0;

    /* Set to pass condition "Result != OS_SUCCESS", which is an error case */
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_STAT_INDEX, -1, 1);

    /* Execute the function being tested */
    Result = FM_ChildSizeTimeMode("filename", &FileSize, &FileTime, &FileMode);
    
    /* Verify results */
    UtAssert_True (FileTime == 0, "FileTime == 0");
    UtAssert_True (FileSize == 0, "FileSize == 0");
    UtAssert_True (FileMode == 0, "FileMode == 0");
    UtAssert_True (Result == -1, "Result == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_ChildSizeAndTime_Test_statError */


void FM_ChildSetPermissionsCmd_Test_Nominal(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = FM_SET_FILE_PERM_CC; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "/source1", OS_MAX_PATH_LEN);
    CmdArgs->Mode = 511; /* (0777) */

    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CHMOD_INDEX, OS_SUCCESS, 1);

    /* Execute the function being tested */
    FM_ChildSetPermissionsCmd(CmdArgs);
    
    /* Verify results */

    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_SET_PERM_CMD_EID, CFE_EVS_DEBUG, "Set Permissions command: file = /source1, access = 511"),  
        "Set Permissions command: file = /source1, access = 511");

    UtAssert_True (FM_GlobalData.ChildCmdCounter == 1, "FM_GlobalData.ChildCmdCounter == 1");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 0, "FM_GlobalData.ChildCmdErrCounter == 0");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildSetPermissionsCmd_Test_Nominal */

void FM_ChildSetPermissionsCmd_Test_OS_Error(void)
{
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    CmdArgs->CommandCode = FM_SET_FILE_PERM_CC; /* Value doesn't really matter in this test */
    strncpy(CmdArgs->Source1, "/source1", OS_MAX_PATH_LEN);
    CmdArgs->Mode = 511;  /* (0777) */

    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CHMOD_INDEX, -1, 1);

    /* Execute the function being tested */
    FM_ChildSetPermissionsCmd(CmdArgs);
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(FM_SET_PERM_OS_ERR_EID, CFE_EVS_ERROR, "Set Permissions command: OS_chmod error, RC=0xFFFFFFFF, file = /source1, access = 511"),  
        "﻿Set Permissions command: OS_chmod error, RC=0xFFFFFFFF, file = /source1, access = 511");

    UtAssert_True (FM_GlobalData.ChildCmdCounter == 0, "FM_GlobalData.ChildCmdCounter == 0");
    UtAssert_True (FM_GlobalData.ChildCmdErrCounter == 1, "FM_GlobalData.ChildCmdErrCounter == 1");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end FM_ChildSetPermissionsCmd_Test_OS_Error */


void FM_ChildSleepStat_Test_GetStatsAndDoNotSleep(void)
{
    char Filename[OS_MAX_PATH_LEN];
    FM_DirListEntry_t DirListData;
    int32 FilesTillSleep = 10;
    boolean getSizeTimeMode = TRUE;

    strncpy(Filename, "/source1", OS_MAX_PATH_LEN);
    
    /* Sets FileStatus->st_size = 5, which FileSize is then set to.  FileSize is verified in a UtAssert statement below. */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookSetSize);

    /* Set to return a specific value, which is checked in a UtAssert statement below */
    Ut_CFE_TIME_SetReturnCode(UT_CFE_TIME_FS2CFESECONDS_INDEX, 10, 1);


    /* Execute the function being tested */
    FM_ChildSleepStat(Filename, (FM_DirListEntry_t *)&DirListData, &FilesTillSleep, getSizeTimeMode);
    
    /* Verify results */

    UtAssert_True (FilesTillSleep == 9, "FilesTillSleep == 9");
    
    UtAssert_True (DirListData.EntrySize == 5,  "DirListData.EntrySize == 5");
    UtAssert_True (DirListData.ModifyTime == 10, "DirListData.ModifyTime == 10");
    UtAssert_True (DirListData.Mode == 511,       "DirListData.Mode == 511"); /* 0777 mode */
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_ChildSleepStat_Test_GetStats */


void FM_ChildSleepStat_Test_GetStatsAndSleep(void)
{
    char Filename[OS_MAX_PATH_LEN];
    FM_DirListEntry_t DirListData;
    int32 FilesTillSleep = 0;
    boolean getSizeTimeMode = TRUE;

    strncpy(Filename, "/source1", OS_MAX_PATH_LEN);
    
    /* Sets FileStatus->st_size = 5, which FileSize is then set to.  FileSize is verified in a UtAssert statement below. */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookSetSize);

    /* Set to return a specific value, which is checked in a UtAssert statement below */
    Ut_CFE_TIME_SetReturnCode(UT_CFE_TIME_FS2CFESECONDS_INDEX, 10, 1);


    /* Execute the function being tested */
    FM_ChildSleepStat(Filename, (FM_DirListEntry_t *)&DirListData, &FilesTillSleep, getSizeTimeMode);
    
    /* Verify results */

    UtAssert_True (FilesTillSleep == FM_CHILD_STAT_SLEEP_FILECOUNT - 1, "FilesTillSleep == FM_CHILD_STAT_SLEEP_FILECOUNT - 1");
    
    UtAssert_True (DirListData.EntrySize == 5,  "DirListData.EntrySize == 5");
    UtAssert_True (DirListData.ModifyTime == 10, "DirListData.ModifyTime == 10");
    UtAssert_True (DirListData.Mode == 511,       "DirListData.Mode == 511"); /* 0777 mode */
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_ChildSleepStat_Test_GetStatsAndSleep */

void FM_ChildSleepStat_Test_DoNotGetStats(void)
{
    char Filename[OS_MAX_PATH_LEN];
    FM_DirListEntry_t DirListData;
    int32 FilesTillSleep = 1;
    boolean getSizeTimeMode = FALSE;

    DirListData.EntrySize = 1;
    DirListData.ModifyTime = 1;
    DirListData.Mode = 1;
    
    strncpy(Filename, "/source1", OS_MAX_PATH_LEN);
    
    /* Sets FileStatus->st_size = 5, which FileSize is then set to.  FileSize is verified in a UtAssert statement below. */
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_STAT_INDEX, &UT_FM_CHILD_TEST_CFE_OSFILEAPI_StatHookSetSize);

    /* Set to return a specific value, which is checked in a UtAssert statement below */
    Ut_CFE_TIME_SetReturnCode(UT_CFE_TIME_FS2CFESECONDS_INDEX, 10, 1);


    /* Execute the function being tested */
    FM_ChildSleepStat(Filename, (FM_DirListEntry_t *)&DirListData, &FilesTillSleep, getSizeTimeMode);
    
    /* Verify results */

    UtAssert_True (FilesTillSleep == 1, "FilesTillSleep == 1");
    
    UtAssert_True (DirListData.EntrySize == 0,  "DirListData.EntrySize == 0");
    UtAssert_True (DirListData.ModifyTime == 0, "DirListData.ModifyTime == 0");
    UtAssert_True (DirListData.Mode == 0,       "DirListData.Mode == 0");
    
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end FM_ChildSleepStat_Test_DoNotGetStats */

/* FM_ChildInit_Test_Nominal not needed - already covered and tested in other tests */

void FM_Child_Test_AddTestCases(void)
{
    UtTest_Add(FM_ChildInit_Test_CreateChildSemaphoreFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildInit_Test_CreateChildSemaphoreFailed");
    UtTest_Add(FM_ChildInit_Test_CreateQueueSemaphoreFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildInit_Test_CreateQueueSemaphoreFailed");
    UtTest_Add(FM_ChildInit_Test_CreateTaskFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildInit_Test_CreateTaskFailed");
    UtTest_Add(FM_ChildTask_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildTask_Test_Nominal");
    UtTest_Add(FM_ChildTask_Test_RegisterChildFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildTask_Test_RegisterChildFailed");
    UtTest_Add(FM_ChildLoop_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildLoop_Test_Nominal");
    UtTest_Add(FM_ChildLoop_Test_SemaphoreTakeFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildLoop_Test_SemaphoreTakeFailed");
    UtTest_Add(FM_ChildLoop_Test_EmptyQueue, FM_Test_Setup, FM_Test_TearDown, "FM_ChildLoop_Test_EmptyQueue");
    UtTest_Add(FM_ChildLoop_Test_InvalidQueueIndex, FM_Test_Setup, FM_Test_TearDown, "FM_ChildLoop_Test_InvalidQueueIndex");
    UtTest_Add(FM_ChildProcess_Test_CopyAndTooLargeReadIndex, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_CopyAndTooLargeReadIndex");
    UtTest_Add(FM_ChildProcess_Test_Move, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_Move");
    UtTest_Add(FM_ChildProcess_Test_Rename, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_Rename");
    UtTest_Add(FM_ChildProcess_Test_Delete, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_Delete");
    UtTest_Add(FM_ChildProcess_Test_DeleteAll, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_DeleteAll");
    UtTest_Add(FM_ChildProcess_Test_Decompress, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_Decompress");
    UtTest_Add(FM_ChildProcess_Test_Concat, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_Concat");
    UtTest_Add(FM_ChildProcess_Test_CreateDir, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_CreateDir");
    UtTest_Add(FM_ChildProcess_Test_DeleteDir, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_DeleteDir");
    UtTest_Add(FM_ChildProcess_Test_GetFileInfo, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_GetFileInfo");
    UtTest_Add(FM_ChildProcess_Test_GetDirFile, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_GetDirFile");
    UtTest_Add(FM_ChildProcess_Test_GetDirPkt, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_GetDirPkt");
    UtTest_Add(FM_ChildProcess_Test_DeleteInt, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_DeleteInt");
    UtTest_Add(FM_ChildProcess_Test_SetPermissions, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_SetPermissions");
    UtTest_Add(FM_ChildProcess_Test_InvalidCommandCode, FM_Test_Setup, FM_Test_TearDown, "FM_ChildProcess_Test_InvalidCommandCode");
    UtTest_Add(FM_ChildCopyCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildCopyCmd_Test_Nominal");
    UtTest_Add(FM_ChildCopyCmd_Test_CopyFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildCopyCmd_Test_CopyFailed");
    UtTest_Add(FM_ChildMoveCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildMoveCmd_Test_Nominal");
    UtTest_Add(FM_ChildMoveCmd_Test_MoveFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildMoveCmd_Test_MoveFailed");
    UtTest_Add(FM_ChildRenameCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildRenameCmd_Test_Nominal");
    UtTest_Add(FM_ChildRenameCmd_Test_RenameFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildRenameCmd_Test_RenameFailed");
    UtTest_Add(FM_ChildDeleteCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteCmd_Test_Nominal");
    UtTest_Add(FM_ChildDeleteCmd_Test_RemoveFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteCmd_Test_RemoveFailed");
    UtTest_Add(FM_ChildDeleteAllCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteAllCmd_Test_Nominal");
    UtTest_Add(FM_ChildDeleteAllCmd_Test_opendirFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteAllCmd_Test_opendirFailed");
    UtTest_Add(FM_ChildDeleteAllCmd_Test_FilesNotDeletedFilenameTooLarge, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteAllCmd_Test_FilesNotDeletedFilenameTooLarge");
    UtTest_Add(FM_ChildDeleteAllCmd_Test_FilesNotDeletedNameIsInvalid, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteAllCmd_Test_FilesNotDeletedNameIsInvalid");
    UtTest_Add(FM_ChildDeleteAllCmd_Test_FilesNotDeletedNameIsNotInUse, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteAllCmd_Test_FilesNotDeletedNameIsNotInUse");
    UtTest_Add(FM_ChildDeleteAllCmd_Test_FilesNotDeletedFileOpen, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteAllCmd_Test_FilesNotDeletedFileOpen");
    UtTest_Add(FM_ChildDeleteAllCmd_Test_FilesNotDeletedFileClosed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteAllCmd_Test_FilesNotDeletedFileClosed");
    UtTest_Add(FM_ChildDeleteAllCmd_Test_DirectorySkipped, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteAllCmd_Test_DirectorySkipped");
    UtTest_Add(FM_ChildDecompressCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDecompressCmd_Test_Nominal");
    UtTest_Add(FM_ChildDecompressCmd_Test_DecompressFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDecompressCmd_Test_DecompressFailed");
    UtTest_Add(FM_ChildConcatCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildConcatCmd_Test_Nominal");
    UtTest_Add(FM_ChildConcatCmd_Test_CopyFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildConcatCmd_Test_CopyFailed");
    UtTest_Add(FM_ChildConcatCmd_Test_OpenFailed1, FM_Test_Setup, FM_Test_TearDown, "FM_ChildConcatCmd_Test_OpenFailed1");
    UtTest_Add(FM_ChildConcatCmd_Test_OpenFailed2, FM_Test_Setup, FM_Test_TearDown, "FM_ChildConcatCmd_Test_OpenFailed2");
    UtTest_Add(FM_ChildConcatCmd_Test_ReadFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildConcatCmd_Test_ReadFailed");
    UtTest_Add(FM_ChildConcatCmd_Test_WriteFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildConcatCmd_Test_WriteFailed");
    UtTest_Add(FM_ChildConcatCmd_Test_CPUHogging, FM_Test_Setup, FM_Test_TearDown, "FM_ChildConcatCmd_Test_CPUHogging");
    UtTest_Add(FM_ChildFileInfoCmd_Test_FinishedReading, FM_Test_Setup, FM_Test_TearDown, "FM_ChildFileInfoCmd_Test_FinishedReading");
    UtTest_Add(FM_ChildFileInfoCmd_Test_ContinueCRC, FM_Test_Setup, FM_Test_TearDown, "FM_ChildFileInfoCmd_Test_ContinueCRC");
    UtTest_Add(FM_ChildFileInfoCmd_Test_CPUHogging, FM_Test_Setup, FM_Test_TearDown, "FM_ChildFileInfoCmd_Test_CPUHogging");
    UtTest_Add(FM_ChildFileInfoCmd_Test_InvalidFileState, FM_Test_Setup, FM_Test_TearDown, "FM_ChildFileInfoCmd_Test_InvalidFileState");
    UtTest_Add(FM_ChildFileInfoCmd_Test_InvalidCRCType, FM_Test_Setup, FM_Test_TearDown, "FM_ChildFileInfoCmd_Test_InvalidCRCType");
    UtTest_Add(FM_ChildFileInfoCmd_Test_OpenFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildFileInfoCmd_Test_OpenFailed");
    UtTest_Add(FM_ChildFileInfoCmd_Test_ReadFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildFileInfoCmd_Test_ReadFailed");
    UtTest_Add(FM_ChildCreateDirCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildCreateDirCmd_Test_Nominal");
    UtTest_Add(FM_ChildCreateDirCmd_Test_mkdirFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildCreateDirCmd_Test_mkdirFailed");
    UtTest_Add(FM_ChildDeleteDirCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteDirCmd_Test_Nominal");
    UtTest_Add(FM_ChildDeleteDirCmd_Test_OpenDirFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteDirCmd_Test_OpenDirFailed");
    UtTest_Add(FM_ChildDeleteDirCmd_Test_DirectoryNotEmpty, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteDirCmd_Test_DirectoryNotEmpty");
    UtTest_Add(FM_ChildDeleteDirCmd_Test_rmdirFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDeleteDirCmd_Test_rmdirFailed");
    UtTest_Add(FM_ChildDirListFileCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListFileCmd_Test_Nominal");
    UtTest_Add(FM_ChildDirListFileCmd_Test_opendirFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListFileCmd_Test_opendirFailed");
    UtTest_Add(FM_ChildDirListPktCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListPktCmd_Test_Nominal");
    UtTest_Add(FM_ChildDirListPktCmd_Test_opendirFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListPktCmd_Test_opendirFailed");
    UtTest_Add(FM_ChildDirListPktCmd_Test_DirPlusEntryTooLong, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListPktCmd_Test_DirPlusEntryTooLong");
    UtTest_Add(FM_ChildDirListFileInit_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListFileInit_Test_Nominal");
    UtTest_Add(FM_ChildDirListFileInit_Test_WriteBlankStatsFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListFileInit_Test_WriteBlankStatsFailed");
    UtTest_Add(FM_ChildDirListFileInit_Test_WriteHeaderFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListFileInit_Test_WriteHeaderFailed");
    UtTest_Add(FM_ChildDirListFileInit_Test_creatFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListFileInit_Test_creatFailed");
    UtTest_Add(FM_ChildDirListFileLoop_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListFileLoop_Test_Nominal");
    UtTest_Add(FM_ChildDirListFileLoop_Test_WriteEntryFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListFileLoop_Test_WriteEntryFailed");
    UtTest_Add(FM_ChildDirListFileLoop_Test_DirectoryAndNameTooLong, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListFileLoop_Test_DirectoryAndNameTooLong");
    UtTest_Add(FM_ChildDirListFileLoop_Test_WriteUpdateStatsFailed, FM_Test_Setup, FM_Test_TearDown, "FM_ChildDirListFileLoop_Test_WriteUpdateStatsFailed");
    UtTest_Add(FM_ChildSizeAndTime_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildSizeAndTime_Test_Nominal");
    UtTest_Add(FM_ChildSizeAndTime_Test_statError, FM_Test_Setup, FM_Test_TearDown, "FM_ChildSizeAndTime_Test_statError");
    
    
    UtTest_Add(FM_ChildSetPermissionsCmd_Test_Nominal, FM_Test_Setup, FM_Test_TearDown, "FM_ChildSetPermissionsCmd_Test_Nominal");
    UtTest_Add(FM_ChildSetPermissionsCmd_Test_OS_Error, FM_Test_Setup, FM_Test_TearDown, "FM_ChildSetPermissionsCmd_Test_OS_Error");
    
    UtTest_Add(FM_ChildSleepStat_Test_GetStatsAndDoNotSleep, FM_Test_Setup, FM_Test_TearDown, "FM_ChildSleepStat_Test_GetStatsAndDoNotSleep");
    UtTest_Add(FM_ChildSleepStat_Test_GetStatsAndSleep, FM_Test_Setup, FM_Test_TearDown, "FM_ChildSleepStat_Test_GetStatsAndSleep");
    UtTest_Add(FM_ChildSleepStat_Test_DoNotGetStats, FM_Test_Setup, FM_Test_TearDown, "FM_ChildSleepStat_Test_DoNotGetStats");
} /* end FM_Child_Test_AddTestCases */

/************************/
/*  End of File Comment */
/************************/
