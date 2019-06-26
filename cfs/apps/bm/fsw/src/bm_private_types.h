/*=======================================================================================
** File Name:  bm_private_types.h
**
** Title:  Type Header File for BM Application
**
** $Author:    Ian Wilmoth
** $Revision: 1.1 $
** $Date:      2016-06-16
**
** Purpose:  This header file contains declarations and definitions of all BM's private
**           data structures and data types.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-06-16 | Ian Wilmoth | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _BM_PRIVATE_TYPES_H_
#define _BM_PRIVATE_TYPES_H_

/*
** Pragmas
*/

/*
** Include Files
*/
#include "cfe.h"

/*
** Local Defines
*/

/*
** Local Structure Declarations
*/

typedef struct
{
    uint8  ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
} BM_NoArgCmd_t;


typedef struct
{
    uint32  counter;

    /* TODO:  Add input data to this application here, such as raw data read from I/O
    **        devices or data subscribed from other apps' output data.
    */

} BM_InData_t;

typedef struct
{
    uint8   ucTlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint32  uiCounter;
} BM_OutData_t;

/* TODO:  Add more private structure definitions here, if necessary. */

typedef struct
{
    uint8   ucTstHeader[CFE_SB_CMD_HDR_SIZE];
    int32  testData;
} BM_TstData_t;

typedef struct
{
    uint8   ucChldHeader[CFE_SB_CMD_HDR_SIZE];
    int32   chldData;
} BM_ChldData_t;

typedef struct {
    /*#WRITEPARAMS*/
    uint32  BM_SB_BenchmarkEnable;
    uint32  BM_SB_ZeroCopyEnable;
    uint32  BM_SB_NumThreads;
    uint32  BM_SB_Repetitions;
    uint32  BM_SB_NumTestMsgs;
    uint32  BM_SB_MessageDataSize;
    uint32  BM_SB_MsgTimeout;
    uint32  BM_FS_BenchmarkEnable;
    uint32  BM_FS_Repetitions;
    uint32  BM_FS_Sequential_Size;
    uint32  BM_CPU_BenchmarkEnable;
    uint32  BM_CPU_DhrystoneRepetitions;
    uint32  BM_CPU_WhetstoneRepetitions;
    /*#END*/
} BM_Params_t;

typedef struct
{
    uint8   ucCmdHeader[CFE_SB_CMD_HDR_SIZE];
    BM_Params_t   params;
} BM_CmdData_t;

typedef struct
{
    uint8   ucChldTskHeader[CFE_SB_CMD_HDR_SIZE];
    CFE_ES_ChildTaskMainFuncPtr_t  childMain;
    uint32  stackSize;
    uint32  priority;
    int32   suffixNumber;
} BM_ChildTaskData_t;

typedef struct {
    /*#WRITERESULTS*/ 
    float  BM_SB_BestTime;
    float  BM_SB_WorstTime;
    float  BM_SB_AverageTime;
    float  BM_SB_MessagesPerSecond;
    float  BM_FS_BestSequentialReadTime;
    float  BM_FS_WorstSequentialReadTime;
    float  BM_FS_AverageSequentialReadTime;
    float  BM_FS_BestSequentialWriteTime;
    float  BM_FS_WorstSequentialWriteTime;
    float  BM_FS_AverageSequentialWriteTime;
    float  BM_FS_SequentialReadAverageMegabytesPerSecond;
    float  BM_FS_SequentialWriteAverageMegabytesPerSecond;
    float  BM_CPU_DhrystonesPerSecond;
    float  BM_CPU_WhetstonesPerSecond;
    /*#END*/
} BM_Results_t;

/*
** External Global Variables
*/

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/

void BM_CheckTimes(float lastTime, float *bestTime, float *worstTime);
float BM_GetTimeInSeconds(CFE_TIME_SysTime_t startTime, CFE_TIME_SysTime_t endTime);
void BM_SendTaskCreateMsg(CFE_ES_ChildTaskMainFuncPtr_t childMain, uint32 stackSize, uint32 priority, int32 suffixNumber);
void BM_RunAllBenchmarks(void);
int32 BM_StorageSequentialReadWrite(void);
int32 BM_StorageRandomFiles(void);
int32 BM_MemorySequentialSet(void);
int32 BM_MemorySequentialCopy(void);
int32 BM_SoftwareBusMessagesPerSecond(void);
void BM_SoftwareBusMessagesPerSecondChildMain(void);
void BM_CPUDhrystone(void);
void BM_CPUWhetstone(void);
void BM_CopyRunParams(void);
int32 BM_Check_OS_creat(int32 functionStatus, int32 lineNumber, char *name);
int32 BM_Check_OS_open(int32 functionStatus, int32 lineNumber, char *name);
int32 BM_Check_OS_mkdir(int32 functionStatus, int32 lineNumber, char *name);
int32 BM_Check_OS_lseek(int32 functionStatus, int32 lineNumber, char *name);
int32 BM_Check_OS_read(int32 functionStatus, int32 lineNumber, char *name);
int32 BM_Check_OS_write(int32 functionStatus, int32 lineNumber, char *name);
int32 BM_Check_CFE_SB_CreatePipe(int32 functionStatus, int32 lineNumber, char *name);
int32 BM_Check_CFE_SB_Subscribe(int32 functionStatus, int32 lineNumber, char *name);
int32 BM_Check_CFE_SB_SendMsg(int32 functionStatus, int32 lineNumber, char *name);
int32 BM_Check_CFE_SB_RcvMsg(int32 functionStatus, int32 lineNumber, char *name);
int32 BM_Check_CFE_SB_ZeroCopySend(int32 functionStatus, int32 lineNumber, char *name);

#endif /* _BM_PRIVATE_TYPES_H_ */

/*=======================================================================================
** End of file bm_private_types.h
**=====================================================================================*/
    