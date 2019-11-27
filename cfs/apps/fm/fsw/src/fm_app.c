/*
** $Id: fm_app.c 1.5.1.2 2017/01/23 21:53:08EST sstrege Exp  $
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
** Title: Core Flight System (CFS) File Manager (FM) Application
**
** Purpose: The File Manager (FM) Application provides onboard file system
**          management services by processing commands for copying and moving
**          files, decompressing files, concatenating files, creating directories,
**          deleting files and directories, and providing file and directory status.
**          When the File Manager application receives a housekeeping request
**          (scheduled within the scheduler application), FM  reports it's housekeeping
**          status values via telemetry messaging.
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
*/

#include "cfe.h"
#include "fm_msg.h"
#include "fm_msgdefs.h"
#include "fm_msgids.h"
#include "fm_app.h"
#include "fm_tbl.h"
#include "fm_child.h"
#include "fm_cmds.h"
#include "fm_cmd_utils.h"
#include "fm_events.h"
#include "fm_perfids.h"
#include "fm_platform_cfg.h"
#include "fm_version.h"
#include "fm_verify.h"

#include <string.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application global data                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

FM_GlobalData_t  FM_GlobalData;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application -- entry point and main loop processor           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_AppMain(void)
{
    uint32 RunStatus = CFE_ES_APP_RUN;
    CFE_SB_MsgPtr_t MsgPtr;
    int32  Result;

    /* Register application */
    Result = CFE_ES_RegisterApp();

    /* Performance Log (start time counter) */
    CFE_ES_PerfLogEntry(FM_APPMAIN_PERF_ID);

    /*
    ** Perform application specific initialization...
    */
    if (Result == CFE_SUCCESS)
    {
        Result = FM_AppInit();
    }

    /*
    ** Check for start-up error...
    */
    if (Result != CFE_SUCCESS)
    {
        /*
        ** Set request to terminate main loop...
        */
        RunStatus = CFE_ES_APP_ERROR;
    }

    /*
    ** Main process loop...
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        /* Performance Log (stop time counter) */
        CFE_ES_PerfLogExit(FM_APPMAIN_PERF_ID);

        /* Wait for the next Software Bus message */
        Result = CFE_SB_RcvMsg(&MsgPtr, FM_GlobalData.CmdPipe, CFE_SB_PEND_FOREVER);

        /* Performance Log (start time counter) */
        CFE_ES_PerfLogEntry(FM_APPMAIN_PERF_ID);

        if (Result == CFE_SUCCESS)
        {
            /* Process Software Bus message */
            FM_ProcessPkt(MsgPtr);
        }
        else
        {
            /* Process Software Bus error */
            CFE_EVS_SendEvent(FM_SB_RECEIVE_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: SB receive: result = 0x%08X", (unsigned int)Result);

            /* Set request to terminate main loop */
            RunStatus = CFE_ES_APP_ERROR;
        }
    }

    /*
    ** Send an event describing the reason for the termination...
    */
    CFE_EVS_SendEvent(FM_EXIT_ERR_EID, CFE_EVS_ERROR,
       "Application terminating: result = 0x%08X", (unsigned int)Result);

    /*
    ** In case cFE Event Services is not working...
    */
    CFE_ES_WriteToSysLog("FM application terminating: result = 0x%08X\n", (unsigned int)Result);

    /*
    ** Performance Log (stop time counter)...
    */
    CFE_ES_PerfLogExit(FM_APPMAIN_PERF_ID);

    /*
    ** Let cFE kill the task (and any child tasks)...
    */
    CFE_ES_ExitApp(RunStatus);

} /* End FM_AppMain */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application -- startup initialization processor              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 FM_AppInit(void)
{
    char *ErrText = "Initialization error:";
    int32 Result;

    /* Initialize global data  */
    CFE_PSP_MemSet(&FM_GlobalData, 0, sizeof(FM_GlobalData_t));

    /* Initialize child task semaphores */
    FM_GlobalData.ChildSemaphore = FM_CHILD_SEM_INVALID;
    FM_GlobalData.ChildQueueCountSem = FM_CHILD_SEM_INVALID;

    /* Register for event services */
    Result = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(FM_STARTUP_EVENTS_ERR_EID, CFE_EVS_ERROR,
           "%s register for event services: result = 0x%08X", ErrText, (unsigned int)Result);
    }
    else
    {
        /* Create Software Bus message pipe */
        Result = CFE_SB_CreatePipe(&FM_GlobalData.CmdPipe,
                                    FM_APP_PIPE_DEPTH, FM_APP_PIPE_NAME);
        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(FM_STARTUP_CREAT_PIPE_ERR_EID, CFE_EVS_ERROR,
               "%s create SB input pipe: result = 0x%08X", ErrText, (unsigned int)Result);
        }
        else
        {
            /* Subscribe to Housekeeping request commands */
            Result = CFE_SB_Subscribe(FM_SEND_HK_MID, FM_GlobalData.CmdPipe);

            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(FM_STARTUP_SUBSCRIB_HK_ERR_EID, CFE_EVS_ERROR,
                   "%s subscribe to HK request: result = 0x%08X", ErrText, (unsigned int)Result);
            }
        }
    }

    /* Keep indentation from getting too deep */
    if (Result == CFE_SUCCESS)
    {
        /* Subscribe to FM ground command packets */
        Result = CFE_SB_Subscribe(FM_CMD_MID, FM_GlobalData.CmdPipe);

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(FM_STARTUP_SUBSCRIB_GCMD_ERR_EID, CFE_EVS_ERROR,
               "%s subscribe to FM commands: result = 0x%08X", ErrText, (unsigned int)Result);
        }
        else
        {
            /* Initialize FM tables */
            Result = FM_TableInit();

            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(FM_STARTUP_TABLE_INIT_ERR_EID, CFE_EVS_ERROR,
                   "%s register free space table: result = 0x%08X", ErrText, (unsigned int)Result);
            }
            else
            {
                /* Create low priority child task */
                FM_ChildInit();

                /* Application startup event message */
                CFE_EVS_SendEvent(FM_STARTUP_EID, CFE_EVS_INFORMATION,
                   "Initialization complete: version %d.%d.%d.%d",
                    FM_MAJOR_VERSION, FM_MINOR_VERSION, FM_REVISION, FM_MISSION_REV);
            }
        }
    }

    return(Result);

} /* End of FM_AppInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application -- input packet processor                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ProcessPkt(CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_SB_MsgId_t MessageID;

    MessageID = CFE_SB_GetMsgId(MessagePtr);

    switch(MessageID)
    {
        /* Housekeeping request */
        case FM_SEND_HK_MID:
            FM_ReportHK(MessagePtr);
            break;

        /* FM ground commands */
        case FM_CMD_MID:
            FM_ProcessCmd(MessagePtr);
            break;

        default:
            CFE_EVS_SendEvent(FM_MID_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: invalid message ID: mid = 0x%04X", MessageID);
            break;

    }

    return;

} /* End of FM_ProcessPkt */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application -- command packet processor                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ProcessCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    boolean Result = TRUE;
    uint16 CommandCode = CFE_SB_GetCmdCode(MessagePtr);

    /* Invoke specific command handler */
    switch (CommandCode)
    {
        case FM_NOOP_CC:
            Result = FM_NoopCmd(MessagePtr);
            break;

        case FM_RESET_CC:
            Result = FM_ResetCountersCmd(MessagePtr);
            break;

        case FM_COPY_CC:
            Result = FM_CopyFileCmd(MessagePtr);
            break;

        case FM_MOVE_CC:
            Result = FM_MoveFileCmd(MessagePtr);
            break;

        case FM_RENAME_CC:
            Result = FM_RenameFileCmd(MessagePtr);
            break;

        case FM_DELETE_CC:
            Result = FM_DeleteFileCmd(MessagePtr);
            break;

        case FM_DELETE_ALL_CC:
            Result = FM_DeleteAllFilesCmd(MessagePtr);
            break;

        case FM_DECOMPRESS_CC:
            Result = FM_DecompressFileCmd(MessagePtr);
            break;

        case FM_CONCAT_CC:
            Result = FM_ConcatFilesCmd(MessagePtr);
            break;

        case FM_GET_FILE_INFO_CC:
            Result = FM_GetFileInfoCmd(MessagePtr);
            break;

        case FM_GET_OPEN_FILES_CC:
            Result = FM_GetOpenFilesCmd(MessagePtr);
            break;

        case FM_CREATE_DIR_CC:
            Result = FM_CreateDirectoryCmd(MessagePtr);
            break;

        case FM_DELETE_DIR_CC:
            Result = FM_DeleteDirectoryCmd(MessagePtr);
            break;

        case FM_GET_DIR_FILE_CC:
            Result = FM_GetDirListFileCmd(MessagePtr);
            break;

        case FM_GET_DIR_PKT_CC:
            Result = FM_GetDirListPktCmd(MessagePtr);
            break;

        case FM_GET_FREE_SPACE_CC:
            Result = FM_GetFreeSpaceCmd(MessagePtr);
            break;

        case FM_SET_TABLE_STATE_CC:
            Result = FM_SetTableStateCmd(MessagePtr);
            break;

        case FM_DELETE_INT_CC:
            Result = FM_DeleteFileCmd(MessagePtr);
            break;
            
        case FM_SET_FILE_PERM_CC:
            Result = FM_SetPermissionsCmd(MessagePtr);
            break;

        default:
            Result = FALSE;
            CFE_EVS_SendEvent(FM_CC_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: invalid command code: cc = %d", CommandCode);
            break;
    }

    if (Result == TRUE)
    {
        /* Increment command success counter */
        if ((CommandCode != FM_RESET_CC) && (CommandCode != FM_DELETE_INT_CC))
        {
            FM_GlobalData.CommandCounter++;
        }
    }
    else
    {
        /* Increment command error counter */
        FM_GlobalData.CommandErrCounter++;
    }

    return;

} /* End of FM_ProcessCmd */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM application -- housekeeping request packet processor         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ReportHK(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "HK Request";
    boolean Result;

    /* Verify command packet length */
    Result = FM_IsValidCmdPktLength(MessagePtr, sizeof(FM_HousekeepingCmd_t),
                                    FM_HK_REQ_ERR_EID, CmdText);

    /* Report FM housekeeping telemetry data */
    if (Result == TRUE)
    {
        /* Release table pointers */
        FM_ReleaseTablePointers();

        /* Allow cFE chance to dump, update, etc. */
        FM_AcquireTablePointers();

        /* Initialize housekeeping telemetry message */
        CFE_SB_InitMsg(&FM_GlobalData.HousekeepingPkt, FM_HK_TLM_MID,
                       sizeof(FM_HousekeepingPkt_t), TRUE);

        /* Report application command counters */
        FM_GlobalData.HousekeepingPkt.CommandCounter = FM_GlobalData.CommandCounter;
        FM_GlobalData.HousekeepingPkt.CommandErrCounter = FM_GlobalData.CommandErrCounter;

        /* Report current number of open files */
        FM_GlobalData.HousekeepingPkt.NumOpenFiles = FM_GetOpenFilesData(NULL);

        /* Report child task command counters */
        FM_GlobalData.HousekeepingPkt.ChildCmdCounter = FM_GlobalData.ChildCmdCounter;
        FM_GlobalData.HousekeepingPkt.ChildCmdErrCounter = FM_GlobalData.ChildCmdErrCounter;
        FM_GlobalData.HousekeepingPkt.ChildCmdWarnCounter = FM_GlobalData.ChildCmdWarnCounter;

        /* Report number of commands in child task queue */
        FM_GlobalData.HousekeepingPkt.ChildQueueCount = FM_GlobalData.ChildQueueCount;

        /* Report current and previous commands executed by the child task */
        FM_GlobalData.HousekeepingPkt.ChildCurrentCC = FM_GlobalData.ChildCurrentCC;
        FM_GlobalData.HousekeepingPkt.ChildPreviousCC = FM_GlobalData.ChildPreviousCC;

        /* Timestamp and send housekeeping telemetry packet */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FM_GlobalData.HousekeepingPkt);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &FM_GlobalData.HousekeepingPkt);
    }

    return;

} /* End of FM_ReportHK */


/************************/
/*  End of File Comment */
/************************/

