/*******************************************************************************
** File: test1_app.c
**
** Purpose:
**   This file contains the source code for the Test1 App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "test1_app.h"
#include "test1_app_perfids.h"
#include "test1_app_msgids.h"
#include "test1_app_msg.h"
#include "test1_app_events.h"
#include "test1_app_version.h"

/*
** global data
*/

test1_hk_tlm_t    TEST1_HkTelemetryPkt;
CFE_SB_PipeId_t    TEST1_CommandPipe;
CFE_SB_MsgPtr_t    TEST1MsgPtr;

static CFE_EVS_BinFilter_t  TEST1_EventFilters[] =
       {  /* Event ID    mask */
          {TEST1_STARTUP_INF_EID,       0x0000},
          {TEST1_COMMAND_ERR_EID,       0x0000},
          {TEST1_COMMANDNOP_INF_EID,    0x0000},
          {TEST1_COMMANDRST_INF_EID,    0x0000},
       };

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* TEST1_AppMain() -- Application entry point and main process loop          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void TEST1_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(TEST1_APP_PERF_ID);

    TEST1_AppInit();

    /*
    ** TEST1 Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(TEST1_APP_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&TEST1MsgPtr, TEST1_CommandPipe, 500);
        
        CFE_ES_PerfLogEntry(TEST1_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            TEST1_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of TEST1_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* TEST1_AppInit() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void TEST1_AppInit(void)
{
    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp() ;

    /*
    ** Register the events
    */ 
    CFE_EVS_Register(TEST1_EventFilters,
                     sizeof(TEST1_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    /*
    ** Create the Software Bus command pipe and subscribe to housekeeping
    **  messages
    */
    CFE_SB_CreatePipe(&TEST1_CommandPipe, TEST1_PIPE_DEPTH,"TEST1_CMD_PIPE");
    CFE_SB_Subscribe(TEST1_APP_CMD_MID, TEST1_CommandPipe);
    CFE_SB_Subscribe(TEST1_APP_SEND_HK_MID, TEST1_CommandPipe);

    TEST1_ResetCounters();

    CFE_SB_InitMsg(&TEST1_HkTelemetryPkt,
                   TEST1_APP_HK_TLM_MID,
                   TEST1_APP_HK_TLM_LNGTH, TRUE);

    CFE_EVS_SendEvent (TEST1_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "TEST1 App Initialized. Version %d.%d.%d.%d",
                TEST1_APP_MAJOR_VERSION,
                TEST1_APP_MINOR_VERSION, 
                TEST1_APP_REVISION, 
                TEST1_APP_MISSION_REV);
				
} /* End of TEST1_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TEST1_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the TEST1    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TEST1_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(TEST1MsgPtr);

    switch (MsgId)
    {
        case TEST1_APP_CMD_MID:
            TEST1_ProcessGroundCommand();
            break;

        case TEST1_APP_SEND_HK_MID:
            TEST1_ReportHousekeeping();
            break;

        default:
            TEST1_HkTelemetryPkt.test1_command_error_count++;
            CFE_EVS_SendEvent(TEST1_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"TEST1: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End TEST1_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TEST1_ProcessGroundCommand() -- TEST1 ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

void TEST1_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(TEST1MsgPtr);

    /* Process "known" TEST1 app ground commands */
    switch (CommandCode)
    {
        case TEST1_APP_NOOP_CC:
            TEST1_HkTelemetryPkt.test1_command_count++;
            CFE_EVS_SendEvent(TEST1_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"TEST1: NOOP command");
            break;

        case TEST1_APP_RESET_COUNTERS_CC:
            TEST1_ResetCounters();
            break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of TEST1_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TEST1_ReportHousekeeping                                              */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TEST1_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &TEST1_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &TEST1_HkTelemetryPkt);
    return;

} /* End of TEST1_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TEST1_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TEST1_ResetCounters(void)
{
    /* Status of commands processed by the TEST1 App */
    TEST1_HkTelemetryPkt.test1_command_count       = 0;
    TEST1_HkTelemetryPkt.test1_command_error_count = 0;

    CFE_EVS_SendEvent(TEST1_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"TEST1: RESET command");
    return;

} /* End of TEST1_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TEST1_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
boolean TEST1_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{     
    boolean result = TRUE;

    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(msg);
        uint16         CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(TEST1_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        TEST1_HkTelemetryPkt.test1_command_error_count++;
    }

    return(result);

} /* End of TEST1_VerifyCmdLength() */

