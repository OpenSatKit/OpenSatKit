/*******************************************************************************
** File: tt_app.c
**
** Purpose:
**   This file contains the source code for the Tt App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "tt_app.h"
#include "tt_app_perfids.h"
#include "tt_app_msgids.h"
#include "tt_app_msg.h"
#include "tt_app_events.h"
#include "tt_app_version.h"

/*
** global data
*/

tt_hk_tlm_t    TT_HkTelemetryPkt;
CFE_SB_PipeId_t    TT_CommandPipe;
CFE_SB_MsgPtr_t    TTMsgPtr;

static CFE_EVS_BinFilter_t  TT_EventFilters[] =
       {  /* Event ID    mask */
          {TT_STARTUP_INF_EID,       0x0000},
          {TT_COMMAND_ERR_EID,       0x0000},
          {TT_COMMANDNOP_INF_EID,    0x0000},
          {TT_COMMANDRST_INF_EID,    0x0000},
       };

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* TT_AppMain() -- Application entry point and main process loop          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void TT_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(TT_APP_PERF_ID);

    TT_AppInit();

    /*
    ** TT Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(TT_APP_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&TTMsgPtr, TT_CommandPipe, 500);
        
        CFE_ES_PerfLogEntry(TT_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            TT_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of TT_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* TT_AppInit() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void TT_AppInit(void)
{
    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp() ;

    /*
    ** Register the events
    */ 
    CFE_EVS_Register(TT_EventFilters,
                     sizeof(TT_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    /*
    ** Create the Software Bus command pipe and subscribe to housekeeping
    **  messages
    */
    CFE_SB_CreatePipe(&TT_CommandPipe, TT_PIPE_DEPTH,"TT_CMD_PIPE");
    CFE_SB_Subscribe(TT_APP_CMD_MID, TT_CommandPipe);
    CFE_SB_Subscribe(TT_APP_SEND_HK_MID, TT_CommandPipe);

    TT_ResetCounters();

    CFE_SB_InitMsg(&TT_HkTelemetryPkt,
                   TT_APP_HK_TLM_MID,
                   TT_APP_HK_TLM_LNGTH, TRUE);

    CFE_EVS_SendEvent (TT_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "TT App Initialized. Version %d.%d.%d.%d",
                TT_APP_MAJOR_VERSION,
                TT_APP_MINOR_VERSION, 
                TT_APP_REVISION, 
                TT_APP_MISSION_REV);
				
} /* End of TT_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TT_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the TT    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TT_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(TTMsgPtr);

    switch (MsgId)
    {
        case TT_APP_CMD_MID:
            TT_ProcessGroundCommand();
            break;

        case TT_APP_SEND_HK_MID:
            TT_ReportHousekeeping();
            break;

        default:
            TT_HkTelemetryPkt.tt_command_error_count++;
            CFE_EVS_SendEvent(TT_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"TT: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End TT_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TT_ProcessGroundCommand() -- TT ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

void TT_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(TTMsgPtr);

    /* Process "known" TT app ground commands */
    switch (CommandCode)
    {
        case TT_APP_NOOP_CC:
            TT_HkTelemetryPkt.tt_command_count++;
            CFE_EVS_SendEvent(TT_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"TT: NOOP command");
            break;

        case TT_APP_RESET_COUNTERS_CC:
            TT_ResetCounters();
            break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of TT_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TT_ReportHousekeeping                                              */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TT_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &TT_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &TT_HkTelemetryPkt);
    return;

} /* End of TT_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TT_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TT_ResetCounters(void)
{
    /* Status of commands processed by the TT App */
    TT_HkTelemetryPkt.tt_command_count       = 0;
    TT_HkTelemetryPkt.tt_command_error_count = 0;

    CFE_EVS_SendEvent(TT_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"TT: RESET command");
    return;

} /* End of TT_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TT_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
boolean TT_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(TT_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        TT_HkTelemetryPkt.tt_command_error_count++;
    }

    return(result);

} /* End of TT_VerifyCmdLength() */

