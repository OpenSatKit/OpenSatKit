/*******************************************************************************
** File: @template@_app.c
**
** Purpose:
**   This file contains the source code for the @Template@ App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "@template@_app.h"
#include "@template@_app_perfids.h"
#include "@template@_app_msgids.h"
#include "@template@_app_msg.h"
#include "@template@_app_events.h"
#include "@template@_app_version.h"

/*
** global data
*/

@template@_hk_tlm_t    @TEMPLATE@_HkTelemetryPkt;
CFE_SB_PipeId_t    @TEMPLATE@_CommandPipe;
CFE_SB_MsgPtr_t    @TEMPLATE@MsgPtr;

static CFE_EVS_BinFilter_t  @TEMPLATE@_EventFilters[] =
       {  /* Event ID    mask */
          {@TEMPLATE@_STARTUP_INF_EID,       0x0000},
          {@TEMPLATE@_COMMAND_ERR_EID,       0x0000},
          {@TEMPLATE@_COMMANDNOP_INF_EID,    0x0000},
          {@TEMPLATE@_COMMANDRST_INF_EID,    0x0000},
       };

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* @TEMPLATE@_AppMain() -- Application entry point and main process loop          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void @TEMPLATE@_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(@TEMPLATE@_APP_PERF_ID);

    @TEMPLATE@_AppInit();

    /*
    ** @TEMPLATE@ Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(@TEMPLATE@_APP_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&@TEMPLATE@MsgPtr, @TEMPLATE@_CommandPipe, 500);
        
        CFE_ES_PerfLogEntry(@TEMPLATE@_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            @TEMPLATE@_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of @TEMPLATE@_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* @TEMPLATE@_AppInit() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void @TEMPLATE@_AppInit(void)
{
    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp() ;

    /*
    ** Register the events
    */ 
    CFE_EVS_Register(@TEMPLATE@_EventFilters,
                     sizeof(@TEMPLATE@_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    /*
    ** Create the Software Bus command pipe and subscribe to housekeeping
    **  messages
    */
    CFE_SB_CreatePipe(&@TEMPLATE@_CommandPipe, @TEMPLATE@_PIPE_DEPTH,"@TEMPLATE@_CMD_PIPE");
    CFE_SB_Subscribe(@TEMPLATE@_CMD_MID, @TEMPLATE@_CommandPipe);
    CFE_SB_Subscribe(@TEMPLATE@_SEND_HK_MID, @TEMPLATE@_CommandPipe);

    @TEMPLATE@_ResetCounters();

    CFE_SB_InitMsg(&@TEMPLATE@_HkTelemetryPkt,
                   @TEMPLATE@_HK_TLM_MID,
                   @TEMPLATE@_HK_TLM_LNGTH, TRUE);

    CFE_EVS_SendEvent (@TEMPLATE@_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "@TEMPLATE@ App Initialized. Version %d.%d.%d.%d",
                @TEMPLATE@_APP_MAJOR_VERSION,
                @TEMPLATE@_APP_MINOR_VERSION, 
                @TEMPLATE@_APP_REVISION, 
                @TEMPLATE@_APP_MISSION_REV);
				
} /* End of @TEMPLATE@_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  @TEMPLATE@_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the @TEMPLATE@    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void @TEMPLATE@_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(@TEMPLATE@MsgPtr);

    switch (MsgId)
    {
        case @TEMPLATE@_CMD_MID:
            @TEMPLATE@_ProcessGroundCommand();
            break;

        case @TEMPLATE@_SEND_HK_MID:
            @TEMPLATE@_ReportHousekeeping();
            break;

        default:
            @TEMPLATE@_HkTelemetryPkt.@template@_command_error_count++;
            CFE_EVS_SendEvent(@TEMPLATE@_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"@TEMPLATE@: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End @TEMPLATE@_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* @TEMPLATE@_ProcessGroundCommand() -- @TEMPLATE@ ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

void @TEMPLATE@_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(@TEMPLATE@MsgPtr);

    /* Process "known" @TEMPLATE@ app ground commands */
    switch (CommandCode)
    {
        case @TEMPLATE@_APP_NOOP_CC:
            @TEMPLATE@_HkTelemetryPkt.@template@_command_count++;
            CFE_EVS_SendEvent(@TEMPLATE@_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"@TEMPLATE@: NOOP command");
            break;

        case @TEMPLATE@_APP_RESET_COUNTERS_CC:
            @TEMPLATE@_ResetCounters();
            break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of @TEMPLATE@_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  @TEMPLATE@_ReportHousekeeping                                              */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void @TEMPLATE@_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &@TEMPLATE@_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &@TEMPLATE@_HkTelemetryPkt);
    return;

} /* End of @TEMPLATE@_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  @TEMPLATE@_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void @TEMPLATE@_ResetCounters(void)
{
    /* Status of commands processed by the @TEMPLATE@ App */
    @TEMPLATE@_HkTelemetryPkt.@template@_command_count       = 0;
    @TEMPLATE@_HkTelemetryPkt.@template@_command_error_count = 0;

    CFE_EVS_SendEvent(@TEMPLATE@_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"@TEMPLATE@: RESET command");
    return;

} /* End of @TEMPLATE@_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* @TEMPLATE@_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
boolean @TEMPLATE@_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(@TEMPLATE@_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        @TEMPLATE@_HkTelemetryPkt.@template@_command_error_count++;
    }

    return(result);

} /* End of @TEMPLATE@_VerifyCmdLength() */

