/*******************************************************************************
** File: what_app.c
**
** Purpose:
**   This file contains the source code for the What App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "what_app.h"
#include "what_app_perfids.h"
#include "what_app_msgids.h"
#include "what_app_msg.h"
#include "what_app_events.h"
#include "what_app_version.h"

/*
** global data
*/

what_hk_tlm_t    WHAT_HkTelemetryPkt;
CFE_SB_PipeId_t    WHAT_CommandPipe;
CFE_SB_MsgPtr_t    WHATMsgPtr;

static CFE_EVS_BinFilter_t  WHAT_EventFilters[] =
       {  /* Event ID    mask */
          {WHAT_STARTUP_INF_EID,       0x0000},
          {WHAT_COMMAND_ERR_EID,       0x0000},
          {WHAT_COMMANDNOP_INF_EID,    0x0000},
          {WHAT_COMMANDRST_INF_EID,    0x0000},
       };

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* WHAT_AppMain() -- Application entry point and main process loop          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void WHAT_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(WHAT_APP_PERF_ID);

    WHAT_AppInit();

    /*
    ** WHAT Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(WHAT_APP_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&WHATMsgPtr, WHAT_CommandPipe, 500);
        
        CFE_ES_PerfLogEntry(WHAT_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            WHAT_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of WHAT_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* WHAT_AppInit() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void WHAT_AppInit(void)
{
    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp() ;

    /*
    ** Register the events
    */ 
    CFE_EVS_Register(WHAT_EventFilters,
                     sizeof(WHAT_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    /*
    ** Create the Software Bus command pipe and subscribe to housekeeping
    **  messages
    */
    CFE_SB_CreatePipe(&WHAT_CommandPipe, WHAT_PIPE_DEPTH,"WHAT_CMD_PIPE");
    CFE_SB_Subscribe(WHAT_APP_CMD_MID, WHAT_CommandPipe);
    CFE_SB_Subscribe(WHAT_APP_SEND_HK_MID, WHAT_CommandPipe);

    WHAT_ResetCounters();

    CFE_SB_InitMsg(&WHAT_HkTelemetryPkt,
                   WHAT_APP_HK_TLM_MID,
                   WHAT_APP_HK_TLM_LNGTH, TRUE);

    CFE_EVS_SendEvent (WHAT_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "WHAT App Initialized. Version %d.%d.%d.%d",
                WHAT_APP_MAJOR_VERSION,
                WHAT_APP_MINOR_VERSION, 
                WHAT_APP_REVISION, 
                WHAT_APP_MISSION_REV);
				
} /* End of WHAT_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  WHAT_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the WHAT    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void WHAT_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(WHATMsgPtr);

    switch (MsgId)
    {
        case WHAT_APP_CMD_MID:
            WHAT_ProcessGroundCommand();
            break;

        case WHAT_APP_SEND_HK_MID:
            WHAT_ReportHousekeeping();
            break;

        default:
            WHAT_HkTelemetryPkt.what_command_error_count++;
            CFE_EVS_SendEvent(WHAT_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"WHAT: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End WHAT_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* WHAT_ProcessGroundCommand() -- WHAT ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

void WHAT_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(WHATMsgPtr);

    /* Process "known" WHAT app ground commands */
    switch (CommandCode)
    {
        case WHAT_APP_NOOP_CC:
            WHAT_HkTelemetryPkt.what_command_count++;
            CFE_EVS_SendEvent(WHAT_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"WHAT: NOOP command");
            break;

        case WHAT_APP_RESET_COUNTERS_CC:
            WHAT_ResetCounters();
            break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of WHAT_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  WHAT_ReportHousekeeping                                              */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void WHAT_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &WHAT_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &WHAT_HkTelemetryPkt);
    return;

} /* End of WHAT_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  WHAT_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void WHAT_ResetCounters(void)
{
    /* Status of commands processed by the WHAT App */
    WHAT_HkTelemetryPkt.what_command_count       = 0;
    WHAT_HkTelemetryPkt.what_command_error_count = 0;

    CFE_EVS_SendEvent(WHAT_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"WHAT: RESET command");
    return;

} /* End of WHAT_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* WHAT_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
boolean WHAT_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(WHAT_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        WHAT_HkTelemetryPkt.what_command_error_count++;
    }

    return(result);

} /* End of WHAT_VerifyCmdLength() */

