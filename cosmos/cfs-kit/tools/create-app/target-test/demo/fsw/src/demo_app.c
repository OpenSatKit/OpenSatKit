/*******************************************************************************
** File: demo_app.c
**
** Purpose:
**   This file contains the source code for the Demo App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "demo_app.h"
#include "demo_app_perfids.h"
#include "demo_app_msgids.h"
#include "demo_app_msg.h"
#include "demo_app_events.h"
#include "demo_app_version.h"

/*
** global data
*/

demo_hk_tlm_t    DEMO_HkTelemetryPkt;
CFE_SB_PipeId_t    DEMO_CommandPipe;
CFE_SB_MsgPtr_t    DEMOMsgPtr;

static CFE_EVS_BinFilter_t  DEMO_EventFilters[] =
       {  /* Event ID    mask */
          {DEMO_STARTUP_INF_EID,       0x0000},
          {DEMO_COMMAND_ERR_EID,       0x0000},
          {DEMO_COMMANDNOP_INF_EID,    0x0000},
          {DEMO_COMMANDRST_INF_EID,    0x0000},
       };

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* DEMO_AppMain() -- Application entry point and main process loop          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void DEMO_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(DEMO_APP_PERF_ID);

    DEMO_AppInit();

    /*
    ** DEMO Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(DEMO_APP_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&DEMOMsgPtr, DEMO_CommandPipe, 500);
        
        CFE_ES_PerfLogEntry(DEMO_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            DEMO_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of DEMO_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* DEMO_AppInit() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void DEMO_AppInit(void)
{
    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp() ;

    /*
    ** Register the events
    */ 
    CFE_EVS_Register(DEMO_EventFilters,
                     sizeof(DEMO_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    /*
    ** Create the Software Bus command pipe and subscribe to housekeeping
    **  messages
    */
    CFE_SB_CreatePipe(&DEMO_CommandPipe, DEMO_PIPE_DEPTH,"DEMO_CMD_PIPE");
    CFE_SB_Subscribe(DEMO_APP_CMD_MID, DEMO_CommandPipe);
    CFE_SB_Subscribe(DEMO_APP_SEND_HK_MID, DEMO_CommandPipe);

    DEMO_ResetCounters();

    CFE_SB_InitMsg(&DEMO_HkTelemetryPkt,
                   DEMO_APP_HK_TLM_MID,
                   DEMO_APP_HK_TLM_LNGTH, TRUE);

    CFE_EVS_SendEvent (DEMO_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "DEMO App Initialized. Version %d.%d.%d.%d",
                DEMO_APP_MAJOR_VERSION,
                DEMO_APP_MINOR_VERSION, 
                DEMO_APP_REVISION, 
                DEMO_APP_MISSION_REV);
				
} /* End of DEMO_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  DEMO_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the DEMO    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void DEMO_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(DEMOMsgPtr);

    switch (MsgId)
    {
        case DEMO_APP_CMD_MID:
            DEMO_ProcessGroundCommand();
            break;

        case DEMO_APP_SEND_HK_MID:
            DEMO_ReportHousekeeping();
            break;

        default:
            DEMO_HkTelemetryPkt.demo_command_error_count++;
            CFE_EVS_SendEvent(DEMO_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"DEMO: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End DEMO_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* DEMO_ProcessGroundCommand() -- DEMO ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

void DEMO_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(DEMOMsgPtr);

    /* Process "known" DEMO app ground commands */
    switch (CommandCode)
    {
        case DEMO_APP_NOOP_CC:
            DEMO_HkTelemetryPkt.demo_command_count++;
            CFE_EVS_SendEvent(DEMO_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"DEMO: NOOP command");
            break;

        case DEMO_APP_RESET_COUNTERS_CC:
            DEMO_ResetCounters();
            break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of DEMO_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  DEMO_ReportHousekeeping                                              */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void DEMO_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &DEMO_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &DEMO_HkTelemetryPkt);
    return;

} /* End of DEMO_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  DEMO_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void DEMO_ResetCounters(void)
{
    /* Status of commands processed by the DEMO App */
    DEMO_HkTelemetryPkt.demo_command_count       = 0;
    DEMO_HkTelemetryPkt.demo_command_error_count = 0;

    CFE_EVS_SendEvent(DEMO_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"DEMO: RESET command");
    return;

} /* End of DEMO_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* DEMO_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
boolean DEMO_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(DEMO_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        DEMO_HkTelemetryPkt.demo_command_error_count++;
    }

    return(result);

} /* End of DEMO_VerifyCmdLength() */

