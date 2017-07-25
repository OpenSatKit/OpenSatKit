/*=======================================================================================
** File Name:  bm_benchmarks.c
**
** Title:  Function Definitions for BM Application
**
** $Author:    Ian Wilmoth (iwilmoth)
** $Revision: 1.1 $
** $Date:      2016-06-15
**
** Purpose:  This source file contains all benchmarks that can be run on the cFE.
**
** Functions Defined:
**    BM_CheckTimes - Compare a time to worst and best times, set if necessary
**	  BM_GetTimeInSeconds - Convert two cFE times to time in seconds
**	  BM_SendTaskCreateMsg - Request a new benchmark task
**	  BM_CopyRunParams - Copies pending benchmark parameters to current parameters
**	  BM_Check_OS_creat - Return value handler for OS_creat
**	  BM_Check_OS_open - Return value handler for OS_open
**	  BM_Check_OS_mkdir - Return value handler for OS_mkdir
**	  BM_Check_OS_lseek - Return value handler for OS_lseek
**	  BM_Check_OS_read - Return value handler for OS_read
**	  BM_Check_OS_write - Return value handler for OS_write
**	  BM_Check_CFE_SB_CreatePipe - Return value handler for CFE_SB_CreatePipe
**	  BM_Check_CFE_SB_Subscribe - Return value handler for CFE_SB_Subscribe
**	  BM_Check_CFE_SB_SendMsg - Return value handler for CFE_SB_SendMsg
**	  BM_Check_CFE_SB_RcvMsg - Return value handler for CFE_SB_RcvMsg
**	  BM_Check_CFE_SB_ZeroCopySend - Return value handler for CFE_SB_ZeroCopySend
**	  BM_RunAllBenchmarks - Run all enabled benchmarks
**	  BM_StorageSequentialReadWrite - Run sequential storage benchmark
**	  BM_SoftwareBusMessagesPerSecond - Run SB benchmark
**	  BM_SoftwareBusMessagesPerSecondChildMain - Task entry point for SB benchmark
**	  BM_CPUDhrystone - Run CPU Dhrystone benchmark
**	  BM_CPUWhetstone - Run CPU Whetstone benchmark
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to all functions in the file.
**    2. List the external source(s) and event(s) that can cause the funcs in this
**       file to execute.
**    3. List known limitations that apply to the funcs in this file.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-06-15 | Ian Wilmoth (iwilmoth) | Build #: Code Started
**
**=====================================================================================*/

/*
** - Determine if OSAL library memory manipulation functions are needed instead of string.h, unistd.h functions.
*/

/*
** Pragmas
*/

/*
** Include Files
*/
#include <string.h>

#include "cfe.h"

#include "bm_platform_cfg.h"
#include "bm_mission_cfg.h"
#include "bm_app.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/
extern BM_AppData_t  g_BM_AppData;

/*
** Local Variables
*/

/*
** Local Function Definitions
*/

/*=====================================================================================
** Name: BM_CheckTimes
**
** Purpose: To compare a time to the current best and worst time
**
** Arguments:
**    float lastTime - time to compare
**	  float *bestTime - pointer to the best time
**	  float *worstTime - pointer to the worst time
**
** Returns:
**    None
**
** Routines Called:
**    None
**
** Called By:
**    BM_StorageSequentialReadWrite
**	  BM_SoftwareBusMessagesPerSecond
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the best time / worst time variables were initially
**	  negative numbers.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void BM_CheckTimes(float lastTime, float *bestTime, float *worstTime)
{
	if (*bestTime < 0.0)
	{
		*bestTime = lastTime;
		*worstTime = lastTime;
	}
		
	if (lastTime < *bestTime)
	{
		*bestTime = lastTime;
	}
		
	if (lastTime > *worstTime)
	{
		*worstTime = lastTime;
	}
}

/*=====================================================================================
** Name: BM_GetTimeInSeconds
**
** Purpose: To take the difference of two cFE time structures and return it in seconds
**
** Arguments:
**    CFE_TIME_SysTime_t startTime - the time the benchmark started
**	  CFE_TIME_SysTime_t endTime - the time the benchmark ended
**
** Returns:
**    float result - the time difference in seconds
**
** Routines Called:
**    CFE_TIME_CFE2FSSeconds
**	  CFE_TIME_Sub2MicroSecs
**
** Called By:
**    BM_StorageSequentialReadWrite
**	  BM_SoftwareBusMessagesPerSecond
**    BM_CPUDhrystone
**	  BM_CPUWhetstone
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    None
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
float BM_GetTimeInSeconds(CFE_TIME_SysTime_t startTime, CFE_TIME_SysTime_t endTime)
{
	int32 startSeconds = CFE_TIME_CFE2FSSeconds(startTime.Seconds);
	int32 startMicroseconds = CFE_TIME_Sub2MicroSecs(startTime.Subseconds);
	int32 endSeconds = CFE_TIME_CFE2FSSeconds(endTime.Seconds);
	int32 endMicroseconds = CFE_TIME_Sub2MicroSecs(endTime.Subseconds);
	float result;
	
	if (startSeconds == endSeconds)
	{
		result = ((float) (endMicroseconds - startMicroseconds)) / 1000000.0;
	}
	else
	{
		if (startMicroseconds > endMicroseconds)
		{
			result = ((float) (endSeconds - startSeconds - 1)) + (((float) (endMicroseconds - startMicroseconds + 1000000)) / 1000000.0);
		}
		else
		{
			result = ((float) (endSeconds - startSeconds)) + (((float) (endMicroseconds - startMicroseconds)) / 1000000.0);
		}
	}
	
	return result;
}

/*=====================================================================================
** Name: BM_SendTaskCreateMsg
**
** Purpose: To send a child task creation request message
**
** Arguments:
**    CFE_ES_ChildTaskMainFuncPtr_t childMain - pointer to the entry point of the task
**	  uint32 stackSize - size of the task's stack
**	  uint32 priority - priority of the task
**	  int32 suffixNumber - number to be appended to the task's name
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_SendMsg
**
** Called By:
**    BM_SoftwareBusMessagesPerSecond
**
** Global Inputs/Reads:
**    g_BM_AppData.ChildTaskData
**
** Global Outputs/Writes:
**	  g_BM_AppData.ChildTaskData.childMain
**	  g_BM_AppData.ChildTaskData.stackSize
**	  g_BM_AppData.ChildTaskData.priority
**	  g_BM_AppData.ChildTaskData.suffixNumber
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void BM_SendTaskCreateMsg(CFE_ES_ChildTaskMainFuncPtr_t childMain, uint32 stackSize, uint32 priority, int32 suffixNumber)
{
	g_BM_AppData.ChildTaskData.childMain = childMain;
	g_BM_AppData.ChildTaskData.stackSize = stackSize;
	g_BM_AppData.ChildTaskData.priority = priority;
	g_BM_AppData.ChildTaskData.suffixNumber = suffixNumber;
	CFE_SB_SendMsg((CFE_SB_Msg_t*) &g_BM_AppData.ChildTaskData);
}

/*=====================================================================================
** Name: BM_CopyRunParams
**
** Purpose: To update the benchmark run parameters
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    None
**
** Called By:
**    BM_SoftwareBusMessagesPerSecond
**
** Global Inputs/Reads:
**    g_BM_AppData.NewParams
**
** Global Outputs/Writes:
**	  g_BM_AppData.CurrentParams
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void BM_CopyRunParams()
{
    memcpy(&g_BM_AppData.CurrentParams, &g_BM_AppData.NewParams, sizeof(BM_Params_t));
}

/*=====================================================================================
** Name: BM_Check_OS_creat
**
** Purpose: Error handler for OS_creat, generates descriptive log messages
**
** Arguments:
**    int32 functionStatus - the return value to examine
**	  int32 lineNumber - line number to be placed in system log messages
**	  char* name - the name to be placed in system log messages
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**
** Called By:
**    BM_StorageSequentialReadWrite
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**	  None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_Check_OS_creat(int32 functionStatus, int32 lineNumber, char *name)
{
	int32 status = BM_SUCCESS;

	switch (functionStatus)
	{	
		case OS_FS_ERR_INVALID_POINTER:
			CFE_ES_WriteToSysLog("BM - Failed to create %s file, invalid path. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERR_PATH_TOO_LONG:
			CFE_ES_WriteToSysLog("BM - Failed to create %s file, file name too long. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERR_NAME_TOO_LONG:
			CFE_ES_WriteToSysLog("BM - Failed to create %s file, file name too long. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERROR:
			CFE_ES_WriteToSysLog("BM - Failed to create %s file. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERR_NO_FREE_FDS:
			CFE_ES_WriteToSysLog("BM - Failed to create %s file, no free file descriptors. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;
	}

	return status;
}

/*=====================================================================================
** Name: BM_Check_OS_open
**
** Purpose: Error handler for OS_open, generates descriptive log messages
**
** Arguments:
**    int32 functionStatus - the return value to examine
**	  int32 lineNumber - line number to be placed in system log messages
**	  char* name - the name to be placed in system log messages
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**
** Called By:
**    BM_StorageSequentialReadWrite
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**	  None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_Check_OS_open(int32 functionStatus, int32 lineNumber, char *name)
{
	int32 status = BM_SUCCESS;

	switch (functionStatus)
	{	
		case OS_FS_ERR_INVALID_POINTER:
			CFE_ES_WriteToSysLog("BM - Failed to open %s file, invalid path. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERR_PATH_TOO_LONG:
			CFE_ES_WriteToSysLog("BM - Failed to open %s file, file path too long. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERR_NAME_TOO_LONG:
			CFE_ES_WriteToSysLog("BM - Failed to open %s file, file name too long. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERROR:
			CFE_ES_WriteToSysLog("BM - Failed to open %s file. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERR_NO_FREE_FDS:
			CFE_ES_WriteToSysLog("BM - Failed to open %s file, no free file descriptors. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;
	}

	return status;
}

/*=====================================================================================
** Name: BM_Check_OS_mkdir
**
** Purpose: Error handler for OS_mkdir, generates descriptive log messages
**
** Arguments:
**    int32 functionStatus - the return value to examine
**	  int32 lineNumber - line number to be placed in system log messages
**	  char* name - the name to be placed in system log messages
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**
** Called By:
**    BM_StorageSequentialReadWrite
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**	  None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_Check_OS_mkdir(int32 functionStatus, int32 lineNumber, char *name)
{
	int32 status = BM_SUCCESS;
	
	switch (functionStatus)
	{	
		case OS_FS_ERR_INVALID_POINTER:
			CFE_ES_WriteToSysLog("BM - Failed to create %s directory, invalid path. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERR_PATH_TOO_LONG:
			CFE_ES_WriteToSysLog("BM - Failed to create %s directory, path too long. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERROR:
			CFE_ES_WriteToSysLog("BM - Failed to create %s directory. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;
	}

	return status;
}

/*=====================================================================================
** Name: BM_Check_OS_lseek
**
** Purpose: Error handler for OS_lseek, generates descriptive log messages
**
** Arguments:
**    int32 functionStatus - the return value to examine
**	  int32 lineNumber - line number to be placed in system log messages
**	  char* name - the name to be placed in system log messages
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**
** Called By:
**    BM_StorageSequentialReadWrite
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**	  None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_Check_OS_lseek(int32 functionStatus, int32 lineNumber, char *name)
{
	int32 status = BM_SUCCESS;

	if (functionStatus == OS_FS_ERROR)
	{
		CFE_ES_WriteToSysLog("BM - Failed to seek on %s file. (L%d)\n", name, lineNumber);
		status = BM_FAILURE;
	}

	return status;
}

/*=====================================================================================
** Name: BM_Check_OS_read
**
** Purpose: Error handler for OS_read, generates descriptive log messages
**
** Arguments:
**    int32 functionStatus - the return value to examine
**	  int32 lineNumber - line number to be placed in system log messages
**	  char* name - the name to be placed in system log messages
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**
** Called By:
**    BM_StorageSequentialReadWrite
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**	  None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_Check_OS_read(int32 functionStatus, int32 lineNumber, char *name)
{
	int32 status = BM_SUCCESS;

	switch (functionStatus)
	{
		case OS_FS_ERR_INVALID_POINTER:
			CFE_ES_WriteToSysLog("BM - Failed to read %s file, invalid buffer. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERROR:
			CFE_ES_WriteToSysLog("BM - Failed to read %s file. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;
	}

	return status;
}

/*=====================================================================================
** Name: BM_Check_OS_write
**
** Purpose: Error handler for OS_write, generates descriptive log messages
**
** Arguments:
**    int32 functionStatus - the return value to examine
**	  int32 lineNumber - line number to be placed in system log messages
**	  char* name - the name to be placed in system log messages
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**
** Called By:
**    BM_StorageSequentialReadWrite
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**	  None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_Check_OS_write(int32 functionStatus, int32 lineNumber, char *name)
{
	int32 status = BM_SUCCESS;

	switch (functionStatus)
	{
		case OS_FS_ERR_INVALID_POINTER:
			CFE_ES_WriteToSysLog("BM - Failed to write to %s file, invalid buffer. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;

		case OS_FS_ERROR:
			CFE_ES_WriteToSysLog("BM - Failed to write to %s file. (L%d)\n", name, lineNumber);
			status = BM_FAILURE;
	}

	return status;
}

/*=====================================================================================
** Name: BM_Check_CFE_SB_CreatePipe
**
** Purpose: Error handler for CFE_SB_CreatePipe, generates descriptive log messages
**
** Arguments:
**    int32 functionStatus - the return value to examine
**	  int32 lineNumber - line number to be placed in system log messages
**	  char* name - the name to be placed in system log messages
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**
** Called By:
**	  BM_SoftwareBusMessagesPerSecond
**    BM_SoftwareBusMessagesPerSecondChildMain
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**	  None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_Check_CFE_SB_CreatePipe(int32 functionStatus, int32 lineNumber, char *name)
{
	int32 status = BM_SUCCESS;

    switch (functionStatus)
    {
    	case CFE_SB_BAD_ARGUMENT:
       		CFE_ES_WriteToSysLog("BM - Failed to create %s pipe, bad argument. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

    	case CFE_SB_MAX_PIPES_MET:
    		CFE_ES_WriteToSysLog("BM - Failed to create %s pipe, pipe limit reached. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

    	case CFE_SB_PIPE_CR_ERR:
    		CFE_ES_WriteToSysLog("BM - Failed to create %s pipe. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;
    }

    return status;
}

/*=====================================================================================
** Name: BM_Check_CFE_SB_Subscribe
**
** Purpose: Error handler for CFE_SB_Subscribe, generates descriptive log messages
**
** Arguments:
**    int32 functionStatus - the return value to examine
**	  int32 lineNumber - line number to be placed in system log messages
**	  char* name - the name to be placed in system log messages
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**
** Called By:
**	  BM_SoftwareBusMessagesPerSecond
**    BM_SoftwareBusMessagesPerSecondChildMain
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**	  None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_Check_CFE_SB_Subscribe(int32 functionStatus, int32 lineNumber, char *name)
{
	int32 status = BM_SUCCESS;

	switch (functionStatus)
    {
    	case CFE_SB_MAX_MSGS_MET:
			CFE_ES_WriteToSysLog("BM - Failed to subscribe to %s messages, out of message IDs. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

    	case CFE_SB_MAX_DESTS_MET:
			CFE_ES_WriteToSysLog("BM - Failed to subscribe to %s messages, max destinations met. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

    	case CFE_SB_BAD_ARGUMENT:
			CFE_ES_WriteToSysLog("BM - Failed to subscribe to %s messages, bad argument. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

    	case CFE_SB_BUF_ALOC_ERR:
  			CFE_ES_WriteToSysLog("BM - Failed to subscribe to %s messages, buffer allocation error. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;
    }

    return status;
}

/*=====================================================================================
** Name: BM_Check_CFE_SB_SendMsg
**
** Purpose: Error handler for CFE_SB_SendMsg, generates descriptive log messages
**
** Arguments:
**    int32 functionStatus - the return value to examine
**	  int32 lineNumber - line number to be placed in system log messages
**	  char* name - the name to be placed in system log messages
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**
** Called By:
**	  BM_SoftwareBusMessagesPerSecond
**    BM_SoftwareBusMessagesPerSecondChildMain
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**	  None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_Check_CFE_SB_SendMsg(int32 functionStatus, int32 lineNumber, char *name)
{
	int32 status = BM_SUCCESS;
	switch (functionStatus)
	{
		case CFE_SB_BAD_ARGUMENT:
			CFE_ES_WriteToSysLog("BM - Failed to send %s message, bad argument. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

		case CFE_SB_MSG_TOO_BIG:
			CFE_ES_WriteToSysLog("BM - Failed to send %s message, message too big. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

		case CFE_SB_BUF_ALOC_ERR:
			CFE_ES_WriteToSysLog("BM - Failed to send %s message, buffer allocation error. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;
		}

	return status;
}

/*=====================================================================================
** Name: BM_Check_CFE_SB_RcvMsg
**
** Purpose: Error handler for CFE_SB_RcvMsg, generates descriptive log messages
**
** Arguments:
**    int32 functionStatus - the return value to examine
**	  int32 lineNumber - line number to be placed in system log messages
**	  char* name - the name to be placed in system log messages
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**
** Called By:
**	  BM_SoftwareBusMessagesPerSecond
**    BM_SoftwareBusMessagesPerSecondChildMain
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**	  None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_Check_CFE_SB_RcvMsg(int32 functionStatus, int32 lineNumber, char *name)
{
	int32 status = BM_SUCCESS;

	switch(functionStatus)
	{
		case CFE_SB_BAD_ARGUMENT:
			CFE_ES_WriteToSysLog("BM - Failed to receive %s message, bad argument. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

		case CFE_SB_TIME_OUT:
			CFE_ES_WriteToSysLog("BM - Failed to receive %s message, timeout. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

		case CFE_SB_PIPE_RD_ERR:
			CFE_ES_WriteToSysLog("BM - Failed to receive %s message, pipe read error. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;
	}

	return status;
}

/*=====================================================================================
** Name: BM_Check_CFE_SB_ZeroCopySend
**
** Purpose: Error handler for CFE_SB_ZeroCopySend, generates descriptive log messages
**
** Arguments:
**    int32 functionStatus - the return value to examine
**	  int32 lineNumber - line number to be placed in system log messages
**	  char* name - the name to be placed in system log messages
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**
** Called By:
**    BM_SoftwareBusMessagesPerSecondChildMain
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**	  None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_Check_CFE_SB_ZeroCopySend(int32 functionStatus, int32 lineNumber, char *name)
{
	int32 status = BM_SUCCESS;
	switch (functionStatus)
	{
		case CFE_SB_BAD_ARGUMENT:
			CFE_ES_WriteToSysLog("BM - Failed to send %s message with Zero Copy, bad argument. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

		case CFE_SB_MSG_TOO_BIG:
			CFE_ES_WriteToSysLog("BM - Failed to send %s message with Zero Copy, message too big. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

		case CFE_SB_BUF_ALOC_ERR:
			CFE_ES_WriteToSysLog("BM - Failed to send %s message with Zero Copy, buffer allocation error. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;

		case CFE_SB_BUFFER_INVALID:
			CFE_ES_WriteToSysLog("BM - Failed to send %s message with Zero Copy, buffer invalid. (0x%08X) (L%d)\n", name, functionStatus, lineNumber);
			status = BM_FAILURE;
		}

	return status;
}

/*=====================================================================================
** Name: BM_RunAllBenchmarks
**
** Purpose: Update benchmark run parameters and run all benchmarks that are enabled,
**	  entry point for the benchmark runner task
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_ES_RegisterChildTask
**	  BM_CopyRunParams
**	  CFE_ES_WriteToSysLog
**	  BM_StorageSequentialReadWrite
**	  BM_SoftwareBusMessagesPerSecond
**	  BM_CPUDhrystone
**	  BM_CPUWhetstone
**	  CFE_ES_ExitChildTask
**
** Called By:
**    BM_ProcessNewAppCmds
**
** Global Inputs/Reads:
**    g_BM_AppData.CurrentParams.BM_FS_BenchmarkEnable
**	  g_BM_AppData.CurrentParams.BM_SB_BenchmarkEnable
**	  g_BM_AppData.CurrentParams.BM_CPU_BenchmarkEnable
**
** Global Outputs/Writes:
**	  g_BM_AppData.HkTlm.BM_RunningBenchmarks
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void BM_RunAllBenchmarks()
{
	int32 status;

	CFE_ES_RegisterChildTask();

	BM_CopyRunParams();
	
	if (g_BM_AppData.CurrentParams.BM_FS_BenchmarkEnable)
	{
		status = BM_StorageSequentialReadWrite();
		
		if (status != BM_SUCCESS)
		{
			CFE_ES_WriteToSysLog("BM - There was a problem with BM_StorageSequentialReadWrite.\n");
		}
	}

	if (g_BM_AppData.CurrentParams.BM_SB_BenchmarkEnable)
	{
		status = BM_SoftwareBusMessagesPerSecond();
		
		if (status != BM_SUCCESS)
		{	
			CFE_ES_WriteToSysLog("BM - There was a problem with BM_SoftwareBusMessagesPerSecond.\n");
		}	
	}

	if (g_BM_AppData.CurrentParams.BM_CPU_BenchmarkEnable)
	{
		BM_CPUDhrystone();

		BM_CPUWhetstone();
	}

	g_BM_AppData.HkTlm.BM_RunningBenchmarks = 0;
	
	CFE_ES_ExitChildTask();
}

/*=====================================================================================
** Name: BM_StorageSequentialReadWrite
**
** Purpose: Run sequential storage benchmark
**
** Arguments:
**    None
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**	  CFE_TIME_GetTime
**	  OS_rmdir
**	  OS_mkdir
**	  OS_creat
**	  OS_lseek
**	  OS_write
**	  OS_read
**	  OS_close
**	  OS_open
**	  OS_remove
**	  BM_Check_OS_mkdir
**	  BM_Check_OS_creat
**	  BM_Check_OS_open
**	  BM_Check_OS_lseek
**	  BM_Check_OS_write
**	  BM_Check_OS_read
**	  BM_GetTimeInSeconds
**	  BM_CheckTimes
**
** Called By:
**    BM_RunAllBenchmarks
**
** Global Inputs/Reads:
**    g_BM_AppData.CurrentParams.BM_FS_Repetitions
**	  g_BM_AppData.CurrentParams.BM_FS_Sequential_Size
**
** Global Outputs/Writes:
**	  g_BM_AppData.Results.BM_FS_BestSequentialReadTime
**	  g_BM_AppData.Results.BM_FS_WorstSequentialReadTime
**	  g_BM_AppData.Results.BM_FS_AverageSequentialReadTime
**	  g_BM_AppData.Results.BM_FS_BestSequentialWriteTime
**	  g_BM_AppData.Results.BM_FS_WorstSequentialWriteTime
**	  g_BM_AppData.Results.BM_FS_AverageSequentialWriteTime
**	  g_BM_AppData.Results.BM_FS_SequentialReadAverageMegabytesPerSecond
**	  g_BM_AppData.Results.BM_FS_SequentialWriteAverageMegabytesPerSecond
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_StorageSequentialReadWrite()
{
	int32 status = BM_SUCCESS;
	int32 i;
	int32 j;
	int32 bmFileDescriptor;
	char bmDirPath[OS_MAX_PATH_LEN + 1];
	char bmFilePath[OS_MAX_PATH_LEN + 1];
	char *testData;
	CFE_TIME_SysTime_t startTime;
	CFE_TIME_SysTime_t endTime;
	uint32 dataScramble;
	float elapsedReadTime = 0.0;
	float lastReadTime = 0.0;
	float bestReadTime = -1.0;
	float worstReadTime = -1.0;
	float elapsedWriteTime = 0.0;
	float lastWriteTime = 0.0;
	float bestWriteTime = -1.0;
	float worstWriteTime = -1.0;
	float readAverageMegabytesPerSecond = 0.0;
	float writeAverageMegabytesPerSecond = 0.0;
	
	testData = (char*) malloc(sizeof(*testData) * g_BM_AppData.CurrentParams.BM_FS_Sequential_Size);
	
	if (testData == NULL)
	{
		CFE_ES_WriteToSysLog("Test memory allocation failed. (L%d)\n", __LINE__);
		status = BM_FAILURE;
		goto BM_StorageSequentialReadWrite_NoDealloc_Exit_Tag;
	}

	if ((strlen(BM_FS_SEPARATOR) * 3) + strlen(BM_FS_ROOT) + strlen(BM_FS_DIRNAME) + strlen(BM_FS_FILENAME_BASE) > OS_MAX_PATH_LEN)
	{
		CFE_ES_WriteToSysLog("Input file path too long. (L%d)\n", __LINE__);
		status = BM_FAILURE;
		goto BM_StorageSequentialReadWrite_Exit_Tag;
	}
	
	bmDirPath[0] = '\0';
	strcat(bmDirPath, BM_FS_SEPARATOR);
	strcat(bmDirPath, BM_FS_ROOT);
	strcat(bmDirPath, BM_FS_SEPARATOR);
	strcat(bmDirPath, BM_FS_DIRNAME);
	
	OS_rmdir(bmDirPath); /* Just in case something happens and the directory is still there. */
	
	status = BM_Check_OS_mkdir(OS_mkdir(bmDirPath, 0), __LINE__, "test");

	if (status != BM_SUCCESS)
	{
		goto BM_StorageSequentialReadWrite_Exit_Tag;
	}

	strcpy(bmFilePath, bmDirPath);
	strcat(bmFilePath, BM_FS_SEPARATOR);
	strcat(bmFilePath, BM_FS_FILENAME_BASE);
	
	bmFileDescriptor = OS_creat(bmFilePath, OS_READ_WRITE);
	
	status = BM_Check_OS_creat(bmFileDescriptor, __LINE__, "test");

	if (status != BM_SUCCESS)
	{
		goto BM_StorageSequentialReadWrite_Exit_Tag;
	}

	if (OS_close(bmFileDescriptor) != OS_FS_SUCCESS)
	{
		CFE_ES_WriteToSysLog("Failed to close test file. (L%d)\n", __LINE__);
		status = BM_FAILURE;
		goto BM_StorageSequentialReadWrite_Exit_Tag;
	}
	
	for (i = 0; i < g_BM_AppData.CurrentParams.BM_FS_Repetitions; i++)
	{

		dataScramble = CFE_TIME_GetTime().Subseconds;
	
		for (j = 0; j < g_BM_AppData.CurrentParams.BM_FS_Sequential_Size; j++)
		{
			testData[j] = (char) (j * dataScramble);
		}

		bmFileDescriptor = OS_open(bmFilePath, OS_READ_WRITE, 0);

		status = BM_Check_OS_open(bmFileDescriptor, __LINE__, "test");

		if (status != BM_SUCCESS)
		{
			goto BM_StorageSequentialReadWrite_Exit_Tag;
		}

		status = BM_Check_OS_lseek(OS_lseek(bmFileDescriptor, 0, OS_SEEK_SET), __LINE__, "test");

		if (status != BM_SUCCESS)
		{
			goto BM_StorageSequentialReadWrite_Exit_Tag;
		}

		startTime = CFE_TIME_GetTime(); /* Begin time measurement. */

		status = OS_write(bmFileDescriptor, testData, g_BM_AppData.CurrentParams.BM_FS_Sequential_Size);

		if (status != sizeof(*testData) * g_BM_AppData.CurrentParams.BM_FS_Sequential_Size)
		{
			CFE_ES_WriteToSysLog("BM - Failed to write to test file. (L%d)\n", __LINE__);
			status = BM_FAILURE;
			goto BM_StorageSequentialReadWrite_Exit_Tag;
		}

		status = BM_Check_OS_write(status, __LINE__, "test");
		
		if (status != BM_SUCCESS)
		{
			goto BM_StorageSequentialReadWrite_Exit_Tag;
		}
	
		endTime = CFE_TIME_GetTime(); /* End time measurement. */
		lastWriteTime = BM_GetTimeInSeconds(startTime, endTime);
		
		BM_CheckTimes(lastWriteTime, &bestWriteTime, &worstWriteTime);
		elapsedWriteTime += lastWriteTime;
		
		if (OS_close(bmFileDescriptor) != OS_FS_SUCCESS)
		{
			CFE_ES_WriteToSysLog("Failed to close test file. (L%d)\n", __LINE__);
			status = BM_FAILURE;
			goto BM_StorageSequentialReadWrite_Exit_Tag;
		}

		bmFileDescriptor = OS_open(bmFilePath, OS_READ_WRITE, 0);

		status = BM_Check_OS_open(bmFileDescriptor, __LINE__, "test");

		if (status != BM_SUCCESS)
		{
			goto BM_StorageSequentialReadWrite_Exit_Tag;
		}

		status = BM_Check_OS_lseek(OS_lseek(bmFileDescriptor, 0, OS_SEEK_SET), __LINE__, "test");

		if (status != BM_SUCCESS)
		{
			goto BM_StorageSequentialReadWrite_Exit_Tag;
		}
	
		startTime = CFE_TIME_GetTime(); /* Begin time measurement. */

		status = OS_read(bmFileDescriptor, testData, g_BM_AppData.CurrentParams.BM_FS_Sequential_Size);

		if (status != sizeof(*testData) * g_BM_AppData.CurrentParams.BM_FS_Sequential_Size)
		{
			CFE_ES_WriteToSysLog("BM - Failed to read test file. (L%d)\n", __LINE__);
			status = BM_FAILURE;
			goto BM_StorageSequentialReadWrite_Exit_Tag;
		}

		status = BM_Check_OS_read(status, __LINE__, "test");

		if (status != BM_SUCCESS)
		{
			goto BM_StorageSequentialReadWrite_Exit_Tag;
		}
	
		endTime = CFE_TIME_GetTime(); /* End time measurement. */
		lastReadTime = BM_GetTimeInSeconds(startTime, endTime);
		
		BM_CheckTimes(lastReadTime, &bestReadTime, &worstReadTime);
		elapsedReadTime += lastReadTime;

		if (OS_close(bmFileDescriptor) != OS_FS_SUCCESS)
		{
			CFE_ES_WriteToSysLog("Failed to close test file. (L%d)\n", __LINE__);
			status = BM_FAILURE;
			goto BM_StorageSequentialReadWrite_Exit_Tag;
		}
	}
	
	elapsedReadTime /= ((float) (g_BM_AppData.CurrentParams.BM_FS_Repetitions));
	elapsedWriteTime /= ((float) (g_BM_AppData.CurrentParams.BM_FS_Repetitions));
	readAverageMegabytesPerSecond = (((float) g_BM_AppData.CurrentParams.BM_FS_Sequential_Size) / elapsedReadTime) / 1000000.0;
	writeAverageMegabytesPerSecond = (((float) g_BM_AppData.CurrentParams.BM_FS_Sequential_Size) / elapsedWriteTime) / 1000000.0;

	g_BM_AppData.Results.BM_FS_BestSequentialReadTime = bestReadTime;
	g_BM_AppData.Results.BM_FS_WorstSequentialReadTime = worstReadTime;
	g_BM_AppData.Results.BM_FS_AverageSequentialReadTime = elapsedReadTime;
	g_BM_AppData.Results.BM_FS_BestSequentialWriteTime = bestWriteTime;
	g_BM_AppData.Results.BM_FS_WorstSequentialWriteTime = worstWriteTime;
	g_BM_AppData.Results.BM_FS_AverageSequentialWriteTime = elapsedWriteTime;
	g_BM_AppData.Results.BM_FS_SequentialReadAverageMegabytesPerSecond = readAverageMegabytesPerSecond;
	g_BM_AppData.Results.BM_FS_SequentialWriteAverageMegabytesPerSecond = writeAverageMegabytesPerSecond;

BM_StorageSequentialReadWrite_Exit_Tag:	

	free(testData);

BM_StorageSequentialReadWrite_NoDealloc_Exit_Tag:	

	OS_remove(bmFilePath);
	OS_rmdir(bmDirPath);
	
	return status;
}

/*=====================================================================================
** Name: BM_SoftwareBusMessagesPerSecond
**
** Purpose: Run Software Bus benchmark
**
** Arguments:
**    None
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**	  CFE_TIME_GetTime
**	  CFE_SB_CreatePipe
**	  CFE_SB_SubscribeEx
**	  CFE_SB_RcvMsg
**	  CFE_SB_SendMsg
**	  CFE_SB_DeletePipe
**	  BM_Check_CFE_SB_CreatePipe
**	  BM_Check_CFE_SB_Subscribe
**	  BM_Check_CFE_SB_RcvMsg
**	  BM_Check_CFE_SB_SendMsg
**	  BM_GetTimeInSeconds
**	  BM_CheckTimes
**	  BM_SendTaskCreateMsg
**
** Called By:
**    BM_RunAllBenchmarks
**
** Global Inputs/Reads:
**	  g_BM_AppData.CurrentParams.BM_SB_NumThreads
**	  g_BM_AppData.CurrentParams.BM_SB_Repetitions
**	  g_BM_AppData.CurrentParams.BM_SB_NumTestMsgs
**	  g_BM_AppData.CurrentParams.BM_SB_MsgTimeout
**
** Global Outputs/Writes:
**	  g_BM_AppData.Results.BM_SB_BestTime
**	  g_BM_AppData.Results.BM_SB_WorstTime
**	  g_BM_AppData.Results.BM_SB_AverageTime
**	  g_BM_AppData.Results.BM_SB_MessagesPerSecond
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_SoftwareBusMessagesPerSecond()
{
	int32 status = BM_SUCCESS;
	int32 iStatus;
	CFE_TIME_SysTime_t startTime;
	CFE_TIME_SysTime_t endTime;
	float elapsedTime = 0.0;
	float lastTime = 0.0;
	float bestTime = -1.0;
	float worstTime = -1.0;
	float messagesPerSecond;
	int32 i;
	int32 j;
	CFE_SB_Msg_t*   MsgPtr = NULL;

		/* Init test pipe */
    g_BM_AppData.usTstPipeDepth = g_BM_AppData.CurrentParams.BM_SB_NumThreads * 2;
    memset((void*)g_BM_AppData.cTstPipeName, '\0', sizeof(g_BM_AppData.cTstPipeName));
    strncpy(g_BM_AppData.cTstPipeName, "BM_TST_PIPE", OS_MAX_API_NAME-1);

    iStatus = CFE_SB_CreatePipe(&g_BM_AppData.TstPipeId,
                                 g_BM_AppData.usTstPipeDepth,
                                 g_BM_AppData.cTstPipeName);
    
    status = BM_Check_CFE_SB_CreatePipe(iStatus, __LINE__, "TST");

    if (status != BM_SUCCESS)
    {
    	goto BM_SoftwareBusMessagesPerSecond_Exit_Tag;
    }
    
    iStatus = CFE_SB_SubscribeEx(BM_CHLD_MID, g_BM_AppData.TstPipeId, CFE_SB_Default_Qos, (g_BM_AppData.CurrentParams.BM_SB_NumThreads * 2));
    
    status = BM_Check_CFE_SB_Subscribe(iStatus, __LINE__, "CHLD");

	if (status != BM_SUCCESS)
	{
		goto BM_SoftwareBusMessagesPerSecond_Exit_Tag;
	}

	for (i = 0; i < (g_BM_AppData.CurrentParams.BM_SB_NumThreads * 2); i += 2) 
	{
		g_BM_AppData.currentNewTaskSendMid = BM_CHLD_MID + i + 1;
		g_BM_AppData.currentNewTaskRecvMid = BM_CHLD_MID + i + 2;

   		BM_SendTaskCreateMsg(BM_SoftwareBusMessagesPerSecondChildMain, 16384, 255, i);

		iStatus = CFE_SB_RcvMsg(&MsgPtr, g_BM_AppData.TstPipeId, g_BM_AppData.CurrentParams.BM_SB_MsgTimeout);

		status = BM_Check_CFE_SB_RcvMsg(iStatus, __LINE__, "child");

		if (status != BM_SUCCESS)
		{
			goto BM_SoftwareBusMessagesPerSecond_Exit_Tag;
		}

		g_BM_AppData.currentNewTaskSendMid = BM_CHLD_MID + i + 2;
		g_BM_AppData.currentNewTaskRecvMid = BM_CHLD_MID + i + 1;

		BM_SendTaskCreateMsg(BM_SoftwareBusMessagesPerSecondChildMain, 16384, 255, i + 1);

		iStatus = CFE_SB_RcvMsg(&MsgPtr, g_BM_AppData.TstPipeId, g_BM_AppData.CurrentParams.BM_SB_MsgTimeout);

		status = BM_Check_CFE_SB_RcvMsg(iStatus, __LINE__, "child");

		if (status != BM_SUCCESS)
		{
			goto BM_SoftwareBusMessagesPerSecond_Exit_Tag;
		}
		
	}
	
	for (i = 0; i < g_BM_AppData.CurrentParams.BM_SB_Repetitions; i++)
	{
		iStatus = CFE_SB_SendMsg((CFE_SB_Msg_t*) &g_BM_AppData.TstData);

		status = BM_Check_CFE_SB_SendMsg(iStatus, __LINE__, "test");

		if (status != BM_SUCCESS)
		{
			goto BM_SoftwareBusMessagesPerSecond_Exit_Tag;
		}

		startTime = CFE_TIME_GetTime();	/* Begin time measurement. */
			
		for (j = 0; j < (g_BM_AppData.CurrentParams.BM_SB_NumThreads * 2); j++)
		{
			iStatus = CFE_SB_RcvMsg(&MsgPtr, g_BM_AppData.TstPipeId, g_BM_AppData.CurrentParams.BM_SB_MsgTimeout);

			status = BM_Check_CFE_SB_RcvMsg(iStatus, __LINE__, "child");

			if (status != BM_SUCCESS)
			{
				goto BM_SoftwareBusMessagesPerSecond_Exit_Tag;
			}
		}

		endTime = CFE_TIME_GetTime();	/* End time measurement. */
		lastTime = BM_GetTimeInSeconds(startTime, endTime);
		
		BM_CheckTimes(lastTime, &bestTime, &worstTime);
		elapsedTime += lastTime;
	}
	
	elapsedTime /= ((float) (g_BM_AppData.CurrentParams.BM_SB_Repetitions));
	messagesPerSecond = (g_BM_AppData.CurrentParams.BM_SB_NumTestMsgs * g_BM_AppData.CurrentParams.BM_SB_NumThreads * 2.0) / elapsedTime;
	
	g_BM_AppData.Results.BM_SB_BestTime = bestTime;
	g_BM_AppData.Results.BM_SB_WorstTime = worstTime;
	g_BM_AppData.Results.BM_SB_AverageTime = elapsedTime;
	g_BM_AppData.Results.BM_SB_MessagesPerSecond = messagesPerSecond;
	
BM_SoftwareBusMessagesPerSecond_Exit_Tag:

	if (CFE_SB_DeletePipe(g_BM_AppData.TstPipeId) == CFE_SB_BAD_ARGUMENT)
	{
		CFE_ES_WriteToSysLog("BM - Failed to delete TST pipe, bad argument. (0x%08X) (L%d)\n", iStatus, __LINE__);
		status = BM_FAILURE;
	}

	return status;
}

/*=====================================================================================
** Name: BM_SoftwareBusMessagesPerSecondChildMain
**
** Purpose: Software Bus benchmark child thread entry point
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_ES_WriteToSysLog
**	  CFE_SB_InitMsg
**	  CFE_SB_CreatePipe
**	  CFE_SB_SubscribeEx
**	  CFE_SB_RcvMsg
**	  CFE_SB_SendMsg
**	  CFE_SB_DeletePipe
**	  CFE_SB_ZeroCopyGetPtr
**	  CFE_SB_ZeroCopySend
**	  CFE_ES_RegisterChildTask
**	  CFE_ES_ExitChildTask
**	  BM_Check_CFE_SB_CreatePipe
**	  BM_Check_CFE_SB_Subscribe
**	  BM_Check_CFE_SB_RcvMsg
**	  BM_Check_CFE_SB_SendMsg
**	  BM_Check_CFE_SB_ZeroCopySend
**	  BM_SendTaskCreateMsg
**
** Called By:
**    BM_SoftwareBusMessagesPerSecond
**
** Global Inputs/Reads:
**	  g_BM_AppData.currentNewTaskId
**	  g_BM_AppData.CurrentParams.BM_SB_ZeroCopyEnable
**	  g_BM_AppData.CurrentParams.BM_SB_NumThreads
**	  g_BM_AppData.CurrentParams.BM_SB_Repetitions
**	  g_BM_AppData.CurrentParams.BM_SB_NumTestMsgs
**	  g_BM_AppData.CurrentParams.BM_SB_MessageDataSize
**	  g_BM_AppData.CurrentParams.BM_SB_MsgTimeout
**
** Global Outputs/Writes:
**	  g_BM_AppData.Results.BM_SB_BestTime
**	  g_BM_AppData.Results.BM_SB_WorstTime
**	  g_BM_AppData.Results.BM_SB_AverageTime
**	  g_BM_AppData.Results.BM_SB_MessagesPerSecond
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void BM_SoftwareBusMessagesPerSecondChildMain()
{
	int32 i;
	int32 j;
	int32 iStatus;
	size_t localBenchMessageSize;
	CFE_SB_Msg_t*   MsgPtr = NULL;
	uint16 pipeDepth = g_BM_AppData.CurrentParams.BM_SB_NumThreads * 2;
	CFE_SB_PipeId_t pipeId;
	CFE_SB_MsgId_t sendMid;
	CFE_SB_MsgId_t recvMid;
	CFE_SB_ZeroCopyHandle_t zeroCopyHandle;
	CFE_SB_Msg_t* zeroCopyPtr;
	/*BM_ChldData_t localChldData;*/
	char* localBenchMessageData;
	char idStr[50];
	char nameStr[OS_MAX_API_NAME];
	CFE_ES_RegisterChildTask();

	sendMid = g_BM_AppData.currentNewTaskSendMid;
	recvMid = g_BM_AppData.currentNewTaskRecvMid;
	localBenchMessageSize = sizeof(*localBenchMessageData) * ((sizeof(uint8) * CFE_SB_CMD_HDR_SIZE) + g_BM_AppData.CurrentParams.BM_SB_MessageDataSize);

		/* Init child packet */
    /*memset((void*)&localChldData, 0x00, sizeof(localChldData));
    CFE_SB_InitMsg(&localChldData, sendMid, sizeof(localChldData), TRUE);*/

    localBenchMessageData = (char*) malloc(localBenchMessageSize);

    if (localBenchMessageData == NULL)
	{
		CFE_ES_WriteToSysLog("Test message memory allocation failed. (L%d)\n", __LINE__);
		goto BM_SoftwareBusMessagesPerSecondChildMain_NoDealloc_Exit_Tag;
	}

	memset((void*)localBenchMessageData, 0x00, localBenchMessageSize);
    CFE_SB_InitMsg(localBenchMessageData, sendMid, localBenchMessageSize, TRUE);

	sprintf(idStr, "%d", g_BM_AppData.currentNewTaskId);
    
    memset((void*)nameStr, '\0', sizeof(nameStr));
    strncpy(nameStr, "BM_CHLD_PIPE", OS_MAX_API_NAME - 1);
    strncat(nameStr, idStr, OS_MAX_API_NAME - strlen(nameStr) - 1);

    iStatus = CFE_SB_CreatePipe(&pipeId, pipeDepth, nameStr);

    if (BM_Check_CFE_SB_CreatePipe(iStatus, __LINE__, "CHLD") != BM_SUCCESS)
    {
    	goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
    }
    	
    iStatus = CFE_SB_Subscribe(BM_TST_MID, pipeId);

    if (BM_Check_CFE_SB_Subscribe(iStatus, __LINE__, "receive pipe") != BM_SUCCESS)
    {
    	goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
    }

    iStatus = CFE_SB_Subscribe(recvMid, pipeId);

    if (BM_Check_CFE_SB_Subscribe(iStatus, __LINE__, "receive pipe") != BM_SUCCESS)
    {
    	goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
    }

	iStatus = CFE_SB_SendMsg((CFE_SB_Msg_t*) &g_BM_AppData.ChldData);

	if (BM_Check_CFE_SB_SendMsg(iStatus, __LINE__, "CHLD") != BM_SUCCESS)
	{
    	goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
	}

	for (i = 0; i < g_BM_AppData.CurrentParams.BM_SB_Repetitions; i++)
	{
		iStatus = CFE_SB_RcvMsg(&MsgPtr, pipeId, g_BM_AppData.CurrentParams.BM_SB_MsgTimeout);

		if (BM_Check_CFE_SB_RcvMsg(iStatus, __LINE__, "TST") != BM_SUCCESS)
		{
			goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
		}

		for (j = 0; j < g_BM_AppData.CurrentParams.BM_SB_NumTestMsgs; j++)
		{
			if (sendMid < recvMid)
			{
				iStatus = CFE_SB_RcvMsg(&MsgPtr, pipeId, g_BM_AppData.CurrentParams.BM_SB_MsgTimeout);

				if (BM_Check_CFE_SB_RcvMsg(iStatus, __LINE__, "TST") != BM_SUCCESS)
				{
					goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
				}

				if (g_BM_AppData.CurrentParams.BM_SB_ZeroCopyEnable)
				{
					zeroCopyPtr = CFE_SB_ZeroCopyGetPtr(localBenchMessageSize, &zeroCopyHandle);
					memcpy(zeroCopyPtr, localBenchMessageData, localBenchMessageSize);
					iStatus = CFE_SB_ZeroCopySend(zeroCopyPtr, zeroCopyHandle);

					if (BM_Check_CFE_SB_ZeroCopySend(iStatus, __LINE__, "CHLD") != BM_SUCCESS)
					{
    					goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
					}
				}
				else
				{
					iStatus = CFE_SB_SendMsg((CFE_SB_Msg_t*) localBenchMessageData);

					if (BM_Check_CFE_SB_SendMsg(iStatus, __LINE__, "CHLD") != BM_SUCCESS)
					{
    					goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
					}
				}
			}
			else
			{
				if (g_BM_AppData.CurrentParams.BM_SB_ZeroCopyEnable)
				{
					zeroCopyPtr = CFE_SB_ZeroCopyGetPtr(localBenchMessageSize, &zeroCopyHandle);
					memcpy(zeroCopyPtr, localBenchMessageData, localBenchMessageSize);
					iStatus = CFE_SB_ZeroCopySend(zeroCopyPtr, zeroCopyHandle);

					if (BM_Check_CFE_SB_ZeroCopySend(iStatus, __LINE__, "CHLD") != BM_SUCCESS)
					{
    					goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
					}
				}
				else
				{
					iStatus = CFE_SB_SendMsg((CFE_SB_Msg_t*) localBenchMessageData);

					if (BM_Check_CFE_SB_SendMsg(iStatus, __LINE__, "CHLD") != BM_SUCCESS)
					{
    					goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
					}
				}

				iStatus = CFE_SB_RcvMsg(&MsgPtr, pipeId, g_BM_AppData.CurrentParams.BM_SB_MsgTimeout);

				if (BM_Check_CFE_SB_RcvMsg(iStatus, __LINE__, "TST") != BM_SUCCESS)
				{
					goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
				}
			}
			
		}

		iStatus = CFE_SB_SendMsg((CFE_SB_Msg_t*) &g_BM_AppData.ChldData);

		if (BM_Check_CFE_SB_SendMsg(iStatus, __LINE__, "CHLD") != BM_SUCCESS)
		{
    		goto BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag;
		}
	}

BM_SoftwareBusMessagesPerSecondChildMain_Exit_Tag:

	free(localBenchMessageData);

BM_SoftwareBusMessagesPerSecondChildMain_NoDealloc_Exit_Tag:

	CFE_SB_DeletePipe(pipeId);
	CFE_ES_ExitChildTask();
}

/*=====================================================================================
** Name: BM_CPUDhrystone
**
** Purpose: Run CPU Dhrystone benchmark
**
** Arguments:
**    None
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**	  CFE_TIME_GetTime
**	  BM_GetTimeInSeconds
**	  runDhrystone
**
** Called By:
**    BM_RunAllBenchmarks
**
** Global Inputs/Reads:
**	  g_BM_AppData.CurrentParams.BM_CPU_DhrystoneRepetitions
**
** Global Outputs/Writes:
**	  g_BM_AppData.Results.BM_CPU_DhrystonesPerSecond
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void BM_CPUDhrystone()
{
	CFE_TIME_SysTime_t startTime;
	CFE_TIME_SysTime_t endTime;
	float elapsedTime = 0.0;

	startTime = CFE_TIME_GetTime(); /* Begin time measurement. */

	runDhrystone(g_BM_AppData.CurrentParams.BM_CPU_DhrystoneRepetitions);

	endTime = CFE_TIME_GetTime(); /* End time measurement. */

	elapsedTime = BM_GetTimeInSeconds(startTime, endTime);

	g_BM_AppData.Results.BM_CPU_DhrystonesPerSecond = ((float) g_BM_AppData.CurrentParams.BM_CPU_DhrystoneRepetitions) / elapsedTime;
}

/*=====================================================================================
** Name: BM_CPUWhetstone
**
** Purpose: Run CPU Whetstone benchmark
**
** Arguments:
**    None
**
** Returns:
**    int32 status - either BM_SUCCESS or BM_FAILURE
**
** Routines Called:
**	  CFE_TIME_GetTime
**	  BM_GetTimeInSeconds
**	  runDhrystone
**
** Called By:
**    BM_RunAllBenchmarks
**
** Global Inputs/Reads:
**	  g_BM_AppData.CurrentParams.BM_CPU_DhrystoneRepetitions
**
** Global Outputs/Writes:
**	  g_BM_AppData.Results.BM_CPU_DhrystonesPerSecond
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void BM_CPUWhetstone()
{
	CFE_TIME_SysTime_t startTime;
	CFE_TIME_SysTime_t endTime;
	float elapsedTime = 0.0;

	startTime = CFE_TIME_GetTime(); /* Begin time measurement. */

	/* runWhetstone(g_BM_AppData.CurrentParams.BM_CPU_WhetstoneRepetitions); */

	endTime = CFE_TIME_GetTime(); /* End time measurement. */

	elapsedTime = BM_GetTimeInSeconds(startTime, endTime);

	g_BM_AppData.Results.BM_CPU_WhetstonesPerSecond = ((float) g_BM_AppData.CurrentParams.BM_CPU_WhetstoneRepetitions) / elapsedTime;
}

/*=======================================================================================
** End of file bm_benchmarks.c
**=====================================================================================*/
