/*=======================================================================================
** File Name:  bm_app.c
**
** Title:  Function Definitions for BM Application
**
** $Author:    Ian Wilmoth (iwilmoth)
** $Revision: 1.1 $
** $Date:      2016-06-16
**
** Purpose:  This source file contains all necessary function definitions to run the BM
**           application.
**
** Functions Defined:
**    BM_InitEvent - Initialize event filters
**	  BM_InitPipe - Initialize permanent pipes
**	  BM_InitData - Initialize message and other data
**	  BM_InitApp - Initialize app
**	  BM_CleanupCallback - Cleanup callback for BM
**	  BM_RcvMsg - Message handler
**	  BM_ProcessNewData - Process input data
**	  BM_CreateChildTask - Create benchmark child task
**	  BM_SetParams - Update pending benchmark parameters
**	  BM_ProcessNewCmds - Process commands
**	  BM_UpdateHousekeeping - Update housekeeping packet
**	  BM_ReportHousekeeping - Send housekeeping packet
**	  BM_SendOutData - Send output data
**	  BM_VerifyCmdLength - Check that a command has the proper length
**	  BM_AppMain - BM entry point
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
**   2016-06-16 | Ian Wilmoth (iwilmoth) | Build #: Code Started
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
BM_AppData_t  g_BM_AppData;

/*
** Local Variables
*/

/*
** Local Function Definitions
*/
    
/*=====================================================================================
** Name: BM_InitEvent
**
** Purpose: To initialize and register event table for BM application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_EVS_Register
**    CFE_ES_WriteToSysLog
**
** Called By:
**    BM_InitApp
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_BM_AppData.EventTbl
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app is in the process of being initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_InitEvent()
{
    int32  iStatus=CFE_SUCCESS;

    /* Create the event table */
    memset((void*)g_BM_AppData.EventTbl, 0x00, sizeof(g_BM_AppData.EventTbl));

    g_BM_AppData.EventTbl[0].EventID = BM_RESERVED_EID;
    g_BM_AppData.EventTbl[1].EventID = BM_INF_EID;
    g_BM_AppData.EventTbl[2].EventID = BM_INIT_INF_EID;
    g_BM_AppData.EventTbl[3].EventID = BM_ILOAD_INF_EID;
    g_BM_AppData.EventTbl[4].EventID = BM_CDS_INF_EID;
    g_BM_AppData.EventTbl[5].EventID = BM_CMD_INF_EID;

    g_BM_AppData.EventTbl[ 6].EventID = BM_ERR_EID;
    g_BM_AppData.EventTbl[ 7].EventID = BM_INIT_ERR_EID;
    g_BM_AppData.EventTbl[ 8].EventID = BM_ILOAD_ERR_EID;
    g_BM_AppData.EventTbl[ 9].EventID = BM_CDS_ERR_EID;
    g_BM_AppData.EventTbl[10].EventID = BM_CMD_ERR_EID;
    g_BM_AppData.EventTbl[11].EventID = BM_PIPE_ERR_EID;
    g_BM_AppData.EventTbl[12].EventID = BM_MSGID_ERR_EID;
    g_BM_AppData.EventTbl[13].EventID = BM_MSGLEN_ERR_EID;

    /* Register the table with CFE */
    iStatus = CFE_EVS_Register(g_BM_AppData.EventTbl,
                               BM_EVT_CNT, CFE_EVS_BINARY_FILTER);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("BM - Failed to register with EVS (0x%08X)\n", iStatus);
    }

    return (iStatus);
}
    
/*=====================================================================================
** Name: BM_InitPipe
**
** Purpose: To initialize all message pipes and subscribe to messages for BM application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_SB_CreatePipe
**    CFE_SB_Subscribe
**    CFE_ES_WriteToSysLog
**
** Called By:
**    BM_InitApp
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_BM_AppData.usSchPipeDepth
**    g_BM_AppData.cSchPipeName
**    g_BM_AppData.SchPipeId
**    g_BM_AppData.usCmdPipeDepth
**    g_BM_AppData.cCmdPipeName
**    g_BM_AppData.CmdPipeId
**    g_BM_AppData.usTlmPipeDepth
**    g_BM_AppData.cTlmPipeName
**    g_BM_AppData.TlmPipeId
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app is in the process of being initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_InitPipe()
{
    int32  iStatus=CFE_SUCCESS;

    /* Init schedule pipe */
    g_BM_AppData.usSchPipeDepth = BM_SCH_PIPE_DEPTH;
    memset((void*)g_BM_AppData.cSchPipeName, '\0', sizeof(g_BM_AppData.cSchPipeName));
    strncpy(g_BM_AppData.cSchPipeName, "BM_SCH_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to Wakeup messages */
    iStatus = CFE_SB_CreatePipe(&g_BM_AppData.SchPipeId,
                                 g_BM_AppData.usSchPipeDepth,
                                 g_BM_AppData.cSchPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        iStatus = CFE_SB_SubscribeEx(BM_WAKEUP_MID, g_BM_AppData.SchPipeId, CFE_SB_Default_Qos, 1);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("BM - Sch Pipe failed to subscribe to BM_WAKEUP_MID. (0x%08X)\n", iStatus);
            goto BM_InitPipe_Exit_Tag;
        }

        iStatus = CFE_SB_Subscribe(BM_CREATE_CHILD_TASK_MID, g_BM_AppData.SchPipeId);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("BM - Sch Pipe failed to subscribe to BM_CREATE_CHILD_TASK_MID. (0x%08X)\n", iStatus);
            goto BM_InitPipe_Exit_Tag;
        }
        
    }
    else
    {
        CFE_ES_WriteToSysLog("BM - Failed to create SCH pipe (0x%08X)\n", iStatus);
        goto BM_InitPipe_Exit_Tag;
    }

    /* Init command pipe */
    g_BM_AppData.usCmdPipeDepth = BM_CMD_PIPE_DEPTH ;
    memset((void*)g_BM_AppData.cCmdPipeName, '\0', sizeof(g_BM_AppData.cCmdPipeName));
    strncpy(g_BM_AppData.cCmdPipeName, "BM_CMD_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to command messages */
    iStatus = CFE_SB_CreatePipe(&g_BM_AppData.CmdPipeId,
                                 g_BM_AppData.usCmdPipeDepth,
                                 g_BM_AppData.cCmdPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        /* Subscribe to command messages */
        iStatus = CFE_SB_Subscribe(BM_CMD_MID, g_BM_AppData.CmdPipeId);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("BM - CMD Pipe failed to subscribe to BM_CMD_MID. (0x%08X)\n", iStatus);
            goto BM_InitPipe_Exit_Tag;
        }

        iStatus = CFE_SB_Subscribe(BM_SEND_HK_MID, g_BM_AppData.CmdPipeId);

        if (iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("BM - CMD Pipe failed to subscribe to BM_SEND_HK_MID. (0x%08X)\n", iStatus);
            goto BM_InitPipe_Exit_Tag;
        }
        
    }
    else
    {
        CFE_ES_WriteToSysLog("BM - Failed to create CMD pipe (0x%08X)\n", iStatus);
        goto BM_InitPipe_Exit_Tag;
    }

    /* Init telemetry pipe */
    g_BM_AppData.usTlmPipeDepth = BM_TLM_PIPE_DEPTH;
    memset((void*)g_BM_AppData.cTlmPipeName, '\0', sizeof(g_BM_AppData.cTlmPipeName));
    strncpy(g_BM_AppData.cTlmPipeName, "BM_TLM_PIPE", OS_MAX_API_NAME-1);

    /* Subscribe to telemetry messages on the telemetry pipe */
    iStatus = CFE_SB_CreatePipe(&g_BM_AppData.TlmPipeId,
                                 g_BM_AppData.usTlmPipeDepth,
                                 g_BM_AppData.cTlmPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        /* TODO:  Add CFE_SB_Subscribe() calls for other apps' output data here.
        **
        ** Examples:
        **     CFE_SB_Subscribe(GNCEXEC_OUT_DATA_MID, g_BM_AppData.TlmPipeId);
        */
    }
    else
    {
        CFE_ES_WriteToSysLog("BM - Failed to create TLM pipe (0x%08X)\n", iStatus);
        goto BM_InitPipe_Exit_Tag;
	}	

BM_InitPipe_Exit_Tag:
    return (iStatus);
}
    
/*=====================================================================================
** Name: BM_InitData
**
** Purpose: To initialize global variables used by BM application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_SB_InitMsg
**
** Called By:
**    BM_InitApp
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_BM_AppData.InData
**    g_BM_AppData.OutData
**    g_BM_AppData.HkTlm
**	  g_BM_AppData.TstData
**	  g_BM_AppData.ChldData
**	  g_BM_AppData.ChildTaskData
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app is in the process of being initialized.
**
** Algorithm:
**	  None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_InitData()
{
    int32  iStatus=CFE_SUCCESS;

    /* Init input data */
    memset((void*)&g_BM_AppData.InData, 0x00, sizeof(g_BM_AppData.InData));

    /* Init output data */
    memset((void*)&g_BM_AppData.OutData, 0x00, sizeof(g_BM_AppData.OutData));
    CFE_SB_InitMsg(&g_BM_AppData.OutData,
                   BM_OUT_DATA_MID, sizeof(g_BM_AppData.OutData), TRUE);

    /* Init housekeeping packet */
    memset((void*)&g_BM_AppData.HkTlm, 0x00, sizeof(g_BM_AppData.HkTlm));
    CFE_SB_InitMsg(&g_BM_AppData.HkTlm,
                   BM_HK_TLM_MID, sizeof(g_BM_AppData.HkTlm), TRUE);
	
	/* Init test packet */
    memset((void*)&g_BM_AppData.TstData, 0x00, sizeof(g_BM_AppData.TstData));
    CFE_SB_InitMsg(&g_BM_AppData.TstData,
                   BM_TST_MID, sizeof(g_BM_AppData.TstData), TRUE);
	
	/* Init child packet */
    memset((void*)&g_BM_AppData.ChldData, 0x00, sizeof(g_BM_AppData.ChldData));
    CFE_SB_InitMsg(&g_BM_AppData.ChldData,
                   BM_CHLD_MID, sizeof(g_BM_AppData.ChldData), TRUE);

    /* Init child task packet */
    memset((void*)&g_BM_AppData.ChildTaskData, 0x00, sizeof(g_BM_AppData.ChildTaskData));
    CFE_SB_InitMsg(&g_BM_AppData.ChildTaskData,
                   BM_CREATE_CHILD_TASK_MID, sizeof(g_BM_AppData.ChildTaskData), TRUE);

    return (iStatus);
}

/*=====================================================================================
** Name: BM_InitApp
**
** Purpose: To initialize all data local to and used by BM application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_ES_RegisterApp
**    CFE_ES_WriteToSysLog
**    CFE_EVS_SendEvent
**    OS_TaskInstallDeleteHandler
**    BM_InitEvent
**    BM_InitPipe
**    BM_InitData
**
** Called By:
**    BM_AppMain
**
** Global Inputs/Reads:
**	  None
**
** Global Outputs/Writes:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app is in the process of being initialized.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 BM_InitApp()
{
    int32  iStatus=CFE_SUCCESS;

    g_BM_AppData.uiRunStatus = CFE_ES_APP_RUN;

    iStatus = CFE_ES_RegisterApp();
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("BM - Failed to register the app (0x%08X)\n", iStatus);
        goto BM_InitApp_Exit_Tag;
    }

    if ((BM_InitEvent() != CFE_SUCCESS) || 
        (BM_InitPipe() != CFE_SUCCESS) || 
        (BM_InitData() != CFE_SUCCESS) ||
        (BM_InitDefaultParameters() != CFE_SUCCESS))
    {
        iStatus = -1;
        goto BM_InitApp_Exit_Tag;
    }

    /* Install the cleanup callback */
    OS_TaskInstallDeleteHandler((void*)&BM_CleanupCallback);

BM_InitApp_Exit_Tag:
    if (iStatus == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(BM_INIT_INF_EID, CFE_EVS_INFORMATION,
                          "BM - Application initialized");
    }
    else
    {
        CFE_ES_WriteToSysLog("BM - Application failed to initialize\n");
    }

    return (iStatus);
}
    
/*=====================================================================================
** Name: BM_CleanupCallback
**
** Purpose: To handle any neccesary cleanup prior to application exit
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
**    None
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
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
void BM_CleanupCallback()
{
    /* TODO:  Add code to cleanup memory and other cleanup here */
}
    
/*=====================================================================================
** Name: BM_RcvMsg
**
** Purpose: To receive and process messages for BM application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization 
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    CFE_ES_PerfLogEntry
**    CFE_ES_PerfLogExit
**    BM_ProcessNewCmds
**    BM_ProcessNewData
**    BM_SendOutData
**	  BM_CreateChildTask
**
** Called By:
**    BM_AppMain
**
** Global Inputs/Reads:
**    g_BM_AppData.SchPipeId
**
** Global Outputs/Writes:
**    g_BM_AppData.uiRunStatus
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
int32 BM_RcvMsg(int32 iBlocking)
{
    int32           iStatus=CFE_SUCCESS;
    CFE_SB_Msg_t*   MsgPtr=NULL;
    CFE_SB_MsgId_t  MsgId;

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(BM_MAIN_TASK_PERF_ID);

    /* Wait for WakeUp messages from scheduler */
    iStatus = CFE_SB_RcvMsg(&MsgPtr, g_BM_AppData.SchPipeId, iBlocking);

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(BM_MAIN_TASK_PERF_ID);

    if (iStatus == CFE_SUCCESS)
    {
        MsgId = CFE_SB_GetMsgId(MsgPtr);
        switch (MsgId)
	{
            case BM_WAKEUP_MID:
                BM_ProcessNewCmds();
                BM_ProcessNewData();

                /* The last thing to do at the end of this Wakeup cycle should be to
                   automatically publish new output. */
                BM_SendOutData();
                break;

            case BM_CREATE_CHILD_TASK_MID:
                BM_CreateChildTask(MsgPtr);
                break;

            default:
                CFE_EVS_SendEvent(BM_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "BM - Recvd invalid SCH msgId (0x%08X)", MsgId);
        }
    }
    else if (iStatus == CFE_SB_NO_MESSAGE)
    {
        /* If there's no incoming message, you can do something here, or nothing */
    }
    else
    {
        /* This is an example of exiting on an error.
        ** Note that a SB read error is not always going to result in an app quitting.
        */
        CFE_EVS_SendEvent(BM_PIPE_ERR_EID, CFE_EVS_ERROR,
			  "BM: SB pipe read error (0x%08X), app will exit", iStatus);
        g_BM_AppData.uiRunStatus= CFE_ES_APP_ERROR;
    }

    return (iStatus);
}
    
/*=====================================================================================
** Name: BM_ProcessNewData
**
** Purpose: To process incoming data subscribed by BM application
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**
** Called By:
**    BM_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
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
void BM_ProcessNewData()
{
    int iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   TlmMsgPtr=NULL;
    CFE_SB_MsgId_t  TlmMsgId;

    /* Process telemetry messages till the pipe is empty */
    while (1)
    {
        iStatus = CFE_SB_RcvMsg(&TlmMsgPtr, g_BM_AppData.TlmPipeId, CFE_SB_POLL);
        if (iStatus == CFE_SUCCESS)
        {
            TlmMsgId = CFE_SB_GetMsgId(TlmMsgPtr);
            switch (TlmMsgId)
            {
                /* TODO:  Add code to process all subscribed data here 
                **
                ** Example:
                **     case NAV_OUT_DATA_MID:
                **         BM_ProcessNavData(TlmMsgPtr);
                **         break;
                */

                default:
                    CFE_EVS_SendEvent(BM_MSGID_ERR_EID, CFE_EVS_ERROR,
                                      "BM - Recvd invalid TLM msgId (0x%08X)", TlmMsgId);
                    break;
            }
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else
        {
            CFE_EVS_SendEvent(BM_PIPE_ERR_EID, CFE_EVS_ERROR,
                  "BM: CMD pipe read error (0x%08X)", iStatus);
            g_BM_AppData.uiRunStatus = CFE_ES_APP_ERROR;
            break;
        }
    }
}

/*=====================================================================================
** Name: BM_CreateChildTask
**
** Purpose: To create a child task for benchmarking purposes
**
** Arguments:
**    CFE_SB_Msg_t* MsgPtr - contains the information needed to create the task
**
** Returns:
**    None
**
** Routines Called:
**    CFE_ES_CreateChildTask
**	  CFE_EVS_SendEvent
**
** Called By:
**    BM_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_BM_AppData.currentNewTaskId
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
void BM_CreateChildTask(CFE_SB_Msg_t* MsgPtr)
{
	BM_ChildTaskData_t* taskData = (BM_ChildTaskData_t*) MsgPtr;
	char bmTaskName[50];
	char bmTaskNumStr[10];

	memset((void*)bmTaskName, '\0', sizeof(bmTaskName));
   	strncpy(bmTaskName, "bm_child_task", OS_MAX_API_NAME - 1);
   	sprintf(bmTaskNumStr, "%d", taskData->suffixNumber);
   	strncat(bmTaskName, bmTaskNumStr, sizeof(bmTaskName) - strlen(bmTaskName) - 1);
	if (CFE_ES_CreateChildTask(&g_BM_AppData.currentNewTaskId, bmTaskName, taskData->childMain, 0, taskData->stackSize, taskData->priority, 0) != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(BM_CMD_INF_EID, CFE_EVS_INFORMATION, "BM - Could not create child task #%d for benchmarks!", taskData->suffixNumber);

	}
}

/*=====================================================================================
** Name: BM_ProcessNewCmds
**
** Purpose: To process incoming command messages for BM application
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    BM_ProcessNewAppCmds
**    BM_ReportHousekeeping
**
** Called By:
**    BM_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
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
void BM_ProcessNewCmds()
{
    int iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   CmdMsgPtr=NULL;
    CFE_SB_MsgId_t  CmdMsgId;

    /* Process command messages till the pipe is empty */
    while (1)
    {
        iStatus = CFE_SB_RcvMsg(&CmdMsgPtr, g_BM_AppData.CmdPipeId, CFE_SB_POLL);
        if(iStatus == CFE_SUCCESS)
        {
            CmdMsgId = CFE_SB_GetMsgId(CmdMsgPtr);
            switch (CmdMsgId)
            {
                case BM_CMD_MID:
                    BM_ProcessNewAppCmds(CmdMsgPtr);
                    break;

                case BM_SEND_HK_MID:
                    BM_ReportHousekeeping();
                    break;

                /* TODO:  Add code to process other subscribed commands here
                **
                ** Example:
                **     case CFE_TIME_DATA_CMD_MID:
                **         BM_ProcessTimeDataCmd(CmdMsgPtr);
                **         break;
                */

                default:
                    CFE_EVS_SendEvent(BM_MSGID_ERR_EID, CFE_EVS_ERROR,
                                      "BM - Recvd invalid CMD msgId (0x%08X)", CmdMsgId);
                    break;
            }
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else
        {
            CFE_EVS_SendEvent(BM_PIPE_ERR_EID, CFE_EVS_ERROR,
                  "BM: CMD pipe read error (0x%08X)", iStatus);
            g_BM_AppData.uiRunStatus = CFE_ES_APP_ERROR;
            break;
        }
    }
}

/*=====================================================================================
** Name: BM_SetParams
**
** Purpose: To update the pending benchmark parameters
**
** Arguments:
**    CFE_SB_Msg_t* MsgPtr - the message received
**
** Returns:
**    None
**
** Routines Called:
**    None
**
** Called By:
**    BM_ProcessNewAppCmds
**
** Global Inputs/Reads:
**    g_BM_AppData.paramMsgPtr
**
** Global Outputs/Writes:
**	  g_BM_AppData.paramMsgPtr
**    g_BM_AppData.NewParams
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  GSFC, Ian Wilmoth (iwilmoth)
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void BM_SetParams(CFE_SB_Msg_t* MsgPtr)
{
	g_BM_AppData.paramMsgPtr = MsgPtr;
	BM_CmdData_t* cmdData = (BM_CmdData_t*) g_BM_AppData.paramMsgPtr;
	memcpy(&g_BM_AppData.NewParams, &cmdData->params, sizeof(BM_Params_t));
}

/*=====================================================================================
** Name: BM_ProcessNewAppCmds
**
** Purpose: To process command messages targeting BM application
**
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr - new command message pointer
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_GetCmdCode
**    CFE_EVS_SendEvent
**	  CFE_ES_WriteToSysLog
**	  BM_ReportHousekeeping
**	  BM_SetParams
**	  CFE_ES_CreateChildTask
**
** Called By:
**    BM_ProcessNewCmds
**
** Global Inputs/Reads:
**    g_BM_AppData.HkTlm.BM_RunningBenchmarks
**
** Global Outputs/Writes:
**    g_BM_AppData.HkTlm.usCmdCnt
**    g_BM_AppData.HkTlm.usCmdErrCnt
**	  g_BM_AppData.HkTlm.BM_RunningBenchmarks
**	  g_BM_AppData.currentNewTaskId
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
void BM_ProcessNewAppCmds(CFE_SB_Msg_t* MsgPtr)
{
    uint32  uiCmdCode=0;

    if (MsgPtr != NULL)
    {
        uiCmdCode = CFE_SB_GetCmdCode(MsgPtr);
        switch (uiCmdCode)
        {
            case BM_NOOP_CC:
                g_BM_AppData.HkTlm.usCmdCnt++;
                CFE_EVS_SendEvent(BM_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "BM - Recvd NOOP cmd (%d)", uiCmdCode);
                break;

            case BM_RESET_CC:
                g_BM_AppData.HkTlm.usCmdCnt = 0;
                g_BM_AppData.HkTlm.usCmdErrCnt = 0;
                CFE_EVS_SendEvent(BM_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "BM - Recvd RESET cmd (%d)", uiCmdCode);
                break;

			case BM_RUNALL_CC:
				g_BM_AppData.HkTlm.usCmdCnt++;
				CFE_EVS_SendEvent(BM_CMD_INF_EID, CFE_EVS_INFORMATION,
                                  "BM - Recvd RUNALL cmd (%d)", uiCmdCode);
				if (!g_BM_AppData.HkTlm.BM_RunningBenchmarks)
				{
					g_BM_AppData.HkTlm.BM_RunningBenchmarks = 1;
					BM_ReportHousekeeping();
					
					if (CFE_ES_CreateChildTask(&g_BM_AppData.currentNewTaskId, "bm_benchmarks", BM_RunAllBenchmarks, 0, 16384, 255, 0) != CFE_SUCCESS)
					{
						CFE_ES_WriteToSysLog("BM - Could not create child task for benchmarks. (%d)\n", uiCmdCode);
					}
				}
				break;
			
            case BM_SETPARAMS_CC:
            	g_BM_AppData.HkTlm.usCmdCnt++;
            	CFE_EVS_SendEvent(BM_CMD_INF_EID, CFE_EVS_INFORMATION,
                                 "BM - Recvd SETPARAMS cmd (%d)", uiCmdCode);
            	BM_SetParams(MsgPtr);
            	break;

            default:
                g_BM_AppData.HkTlm.usCmdErrCnt++;
                CFE_EVS_SendEvent(BM_MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "BM - Recvd invalid cmdId (%d)", uiCmdCode);
                break;
        }
    }
}

/*=====================================================================================
** Name: BM_UpdateHousekeeping
**
** Purpose: To update housekeeping message
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
**    BM_ReportHouseKeeping
**
** Global Inputs/Reads:
**    g_BM_AppData.NewParams
**	  g_BM_AppData.Results
**
** Global Outputs/Writes:
**    g_BM_AppData.HkTlm.params
**	  g_BM_AppData.HkTlm.results
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  GSFC, Ian Wilmoth (iwilmoth)
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void BM_UpdateHousekeeping()
{
	memcpy(&g_BM_AppData.HkTlm.params, &g_BM_AppData.NewParams, sizeof(BM_Params_t));
	memcpy(&g_BM_AppData.HkTlm.results, &g_BM_AppData.Results, sizeof(BM_Results_t));
}

/*=====================================================================================
** Name: BM_ReportHousekeeping
**
** Purpose: To send housekeeping message
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    BM_UpdateHousekeeping
**	  CFE_SB_TimeStampMsg
**	  CFE_SB_SendMsg
**
** Called By:
**    BM_ProcessNewCmds
**	  BM_ProcessNewAppCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that the app has been initialized.
**
** Algorithm:
**    None
**
** Author(s):  GSFC, Ian Wilmoth (iwilmoth)
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void BM_ReportHousekeeping()
{
	BM_UpdateHousekeeping();

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_BM_AppData.HkTlm);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&g_BM_AppData.HkTlm);
}
    
/*=====================================================================================
** Name: BM_SendOutData
**
** Purpose: To publish 1-Wakeup cycle output data
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**	  CFE_SB_TimeStampMsg
**    CFE_SB_SendMsg
**
** Called By:
**    BM_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
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
void BM_SendOutData()
{
    /* TODO:  Add code to update output data, if needed, here.  */

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_BM_AppData.OutData);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&g_BM_AppData.OutData);
}
    
/*=====================================================================================
** Name: BM_VerifyCmdLength
**
** Purpose: To verify command length for a particular command message
**
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr      - command message pointer
**    uint16         usExpLength - expected command length
**
** Returns:
**    boolean bResult - Result of verification
**
** Routines Called:
**    CFE_SB_GetTotalMsgLength
**	  CFE_SB_GetMsgId
**	  CFE_SB_GetCmdCode
**	  CFE_EVS_SendEvent
**
** Called By:
**    BM_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_BM_AppData.HkTlm.usCmdErrCnt
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
boolean BM_VerifyCmdLength(CFE_SB_Msg_t* MsgPtr,
                           uint16 usExpectedLen)
{
    boolean bResult=FALSE;
    uint16  usMsgLen=0;

    if (MsgPtr != NULL)
    {
        usMsgLen = CFE_SB_GetTotalMsgLength(MsgPtr);

        if (usExpectedLen != usMsgLen)
        {
            CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(MsgPtr);
            uint16 usCmdCode = CFE_SB_GetCmdCode(MsgPtr);

            CFE_EVS_SendEvent(BM_MSGLEN_ERR_EID, CFE_EVS_ERROR,
                              "BM - Rcvd invalid msgLen: msgId=0x%08X, cmdCode=%d, "
                              "msgLen=%d, expectedLen=%d",
                              MsgId, usCmdCode, usMsgLen, usExpectedLen);
            g_BM_AppData.HkTlm.usCmdErrCnt++;
        }
    }

    return (bResult);
}
    
/*=====================================================================================
** Name: BM_AppMain
**
** Purpose: To define BM application's entry point and main process loop
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_ES_RegisterApp
**    CFE_ES_RunLoop
**    CFE_ES_PerfLogEntry
**    CFE_ES_PerfLogExit
**    CFE_ES_ExitApp
**    CFE_ES_WaitForStartupSync
**    BM_InitApp
**    BM_RcvMsg
**
** Called By:
**    None
**
** Global Inputs/Reads:
**    g_BM_AppData.uiRunStatus
**
** Global Outputs/Writes:
**    g_BM_AppData.uiRunStatus
**
** Limitations, Assumptions, External Events, and Notes:
**    It is assumed that this is the first function called.
**
** Algorithm:
**    None
**
** Author(s):  Ian Wilmoth (iwilmoth) 
**
** History:  Date Written  2016-06-16
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void BM_AppMain()
{
    /* Register the application with Executive Services */
    CFE_ES_RegisterApp();

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(BM_MAIN_TASK_PERF_ID);

    /* Perform application initializations */
    if (BM_InitApp() != CFE_SUCCESS)
    {
        g_BM_AppData.uiRunStatus = CFE_ES_APP_ERROR;
    } else {
        /* Do not perform performance monitoring on startup sync */
        CFE_ES_PerfLogExit(BM_MAIN_TASK_PERF_ID);
        CFE_ES_WaitForStartupSync(BM_TIMEOUT_MSEC);
        CFE_ES_PerfLogEntry(BM_MAIN_TASK_PERF_ID);
    }

    /* Application main loop */
    while (CFE_ES_RunLoop(&g_BM_AppData.uiRunStatus) == TRUE)
    {
        BM_RcvMsg(CFE_SB_PEND_FOREVER);
    }

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(BM_MAIN_TASK_PERF_ID);

    /* Exit the application */
    CFE_ES_ExitApp(g_BM_AppData.uiRunStatus);
} 
    
/*=======================================================================================
** End of file bm_app.c
**=====================================================================================*/
    