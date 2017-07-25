/*=======================================================================================
** File Name:  bm_app.h
**
** Title:  Header File for BM Application
**
** $Author:    Ian Wilmoth
** $Revision: 1.1 $
** $Date:      2016-06-16
**
** Purpose:  To define BM's internal macros, data types, global variables and
**           function prototypes
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-06-16 | Ian Wilmoth | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _BM_APP_H_
#define _BM_APP_H_

/*
** Pragmas
*/

/*
** Include Files
*/
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "bm_platform_cfg.h"
#include "bm_mission_cfg.h"
#include "bm_private_ids.h"
#include "bm_private_types.h"
#include "bm_perfids.h"
#include "bm_msgids.h"
#include "bm_msg.h"
#include "dhry.h"
#include "whet.h"


/*
** Local Defines
*/

/*#WRITEPARAMS*/
#define BM_SB_BENCHMARKENABLE			1
#define BM_SB_ZEROCOPYENABLE			0
#define BM_SB_NUMTHREADS			1
#define BM_SB_REPETITIONS			10
#define BM_SB_NUMTESTMSGS			1000
#define BM_SB_MESSAGEDATASIZE			4
#define BM_SB_MSGTIMEOUT			100000
#define BM_FS_BENCHMARKENABLE			1
#define BM_FS_REPETITIONS			1
#define BM_FS_SEQUENTIAL_SIZE			100000000
#define BM_CPU_BENCHMARKENABLE			1
#define BM_CPU_DHRYSTONEREPETITIONS			100000
#define BM_CPU_WHETSTONEREPETITIONS			200
/*#END*/
#define BM_MEM_REPETITIONS          100
#define BM_MEM_SEQUENTIAL_SIZE      100000000
#define BM_FS_RANDOM_SIZE           100000
#define BM_FS_RANDOM_NUMFILES       1000
#define BM_FS_SEPARATOR             "/"
#define BM_FS_ROOT                  "cf"
#define BM_FS_DIRNAME               "benchmark_files"
#define BM_FS_FILENAME_BASE         "benchfile"
#define BM_TIMEOUT_MSEC             1000
#define BM_SUCCESS                  0
#define BM_FAILURE                  1

/*
** Local Structure Declarations
*/
typedef struct
{
    /* CFE Event table */
    CFE_EVS_BinFilter_t  EventTbl[BM_EVT_CNT];

    /* CFE scheduling pipe */
    CFE_SB_PipeId_t  SchPipeId; 
    uint16           usSchPipeDepth;
    char             cSchPipeName[OS_MAX_API_NAME];

    /* CFE command pipe */
    CFE_SB_PipeId_t  CmdPipeId;
    uint16           usCmdPipeDepth;
    char             cCmdPipeName[OS_MAX_API_NAME];
    
    /* CFE telemetry pipe */
    CFE_SB_PipeId_t  TlmPipeId;
    uint16           usTlmPipeDepth;
    char             cTlmPipeName[OS_MAX_API_NAME];

    /* test pipe */
    CFE_SB_PipeId_t  TstPipeId;
    uint16           usTstPipeDepth;
    char             cTstPipeName[OS_MAX_API_NAME];

    /* child pipe */
    CFE_SB_PipeId_t  ChldPipeId;
    uint16           usChldPipeDepth;
    char             cChldPipeName[OS_MAX_API_NAME];
    
    /* Task-related */
    uint32  uiRunStatus;
    
    /* Input data - from I/O devices or subscribed from other apps' output data.
       Data structure should be defined in bm/fsw/src/bm_private_types.h */
    BM_InData_t   InData;

    /* Output data - to be published at the end of a Wakeup cycle.
       Data structure should be defined in bm/fsw/src/bm_private_types.h */
    BM_OutData_t  OutData;

    /* Housekeeping telemetry - for downlink only.
       Data structure should be defined in bm/fsw/src/bm_msg.h */
    BM_HkTlm_t  HkTlm;

    /* TODO:  Add declarations for additional private data here */
    
    BM_TstData_t    TstData;
    
    BM_ChldData_t   ChldData;

    BM_ChildTaskData_t   ChildTaskData;

    BM_Params_t    NewParams;

    BM_Params_t    CurrentParams;

    BM_Results_t   Results;

    uint32 currentNewTaskId;

    CFE_SB_MsgId_t currentNewTaskSendMid;

    CFE_SB_MsgId_t currentNewTaskRecvMid;

    CFE_SB_Msg_t* paramMsgPtr;

} BM_AppData_t;

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
int32  BM_InitApp(void);
int32  BM_InitEvent(void);
int32  BM_InitData(void);
int32  BM_InitPipe(void);
int32  BM_InitDefaultParameters(void);

void  BM_AppMain(void);

void  BM_CleanupCallback(void);

int32  BM_RcvMsg(int32 iBlocking);

void  BM_ProcessNewData(void);
void  BM_ProcessNewCmds(void);
void  BM_ProcessNewAppCmds(CFE_SB_Msg_t*);
void  BM_SetParams(CFE_SB_Msg_t* MsgPtr);
void  BM_CreateChildTask(CFE_SB_Msg_t* MsgPtr);
void  BM_UpdateHousekeeping(void);
void  BM_ReportHousekeeping(void);
void  BM_SendOutData(void);

boolean  BM_VerifyCmdLength(CFE_SB_Msg_t*, uint16);

#endif /* _BM_APP_H_ */

/*=======================================================================================
** End of file bm_app.h
**=====================================================================================*/
    