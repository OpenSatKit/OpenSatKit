/*=======================================================================================
** File Name:  bm_init_params.c
**
** Title:  Function Definitions for BM Application
**
** $Author:    Ian Wilmoth
** $Revision: 1.1 $
** $Date:      2016-06-16
**
** Purpose:  This source file contains all necessary function definitions to run BM
**           application.
**
** Functions Defined:
**    Function X - Brief purpose of function X
**    Function Y - Brief purpose of function Y
**    Function Z - Brief purpose of function Z
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
**   2016-06-16 | Ian Wilmoth | Build #: Code Started
**
**=====================================================================================*/

/*
** Pragmas
*/

/*
** Include Files
*/
#include <string.h>

#include "cfe.h"

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

int32 BM_InitDefaultParameters()
{
	int32 iStatus = CFE_SUCCESS;
	/*#WRITEPARAMS*/
	g_BM_AppData.NewParams.BM_SB_BenchmarkEnable = BM_SB_BENCHMARKENABLE;
	g_BM_AppData.NewParams.BM_SB_ZeroCopyEnable = BM_SB_ZEROCOPYENABLE;
	g_BM_AppData.NewParams.BM_SB_NumThreads = BM_SB_NUMTHREADS;
	g_BM_AppData.NewParams.BM_SB_Repetitions = BM_SB_REPETITIONS;
	g_BM_AppData.NewParams.BM_SB_NumTestMsgs = BM_SB_NUMTESTMSGS;
	g_BM_AppData.NewParams.BM_SB_MessageDataSize = BM_SB_MESSAGEDATASIZE;
	g_BM_AppData.NewParams.BM_SB_MsgTimeout = BM_SB_MSGTIMEOUT;
	g_BM_AppData.NewParams.BM_FS_BenchmarkEnable = BM_FS_BENCHMARKENABLE;
	g_BM_AppData.NewParams.BM_FS_Repetitions = BM_FS_REPETITIONS;
	g_BM_AppData.NewParams.BM_FS_Sequential_Size = BM_FS_SEQUENTIAL_SIZE;
	g_BM_AppData.NewParams.BM_CPU_BenchmarkEnable = BM_CPU_BENCHMARKENABLE;
	g_BM_AppData.NewParams.BM_CPU_DhrystoneRepetitions = BM_CPU_DHRYSTONEREPETITIONS;
	g_BM_AppData.NewParams.BM_CPU_WhetstoneRepetitions = BM_CPU_WHETSTONEREPETITIONS;
	/*#END*/

	BM_CopyRunParams(); /* Only necessary to ensure defaults load before params are changed. */
	return iStatus;
}

/*=======================================================================================
** End of bm_init_params.c
**=====================================================================================*/
