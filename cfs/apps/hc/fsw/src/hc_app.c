/*
** $Id: $
**
** Title: Core Flight System (CFS) Heater Control (HC) Application
**
** Purpose: The Heater Control (HC) Application provides onboard heater control services.
**          When the Heater Control application receives a housekeeping request
**          (scheduled within the scheduler application), HC reports its housekeeping
**          status values via telemetry messaging.
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** $Log: $ Changed HC_ReceivePkt function such that average temperatures for each heater
**	   are calculated on receipt of a Thermal Data packet from HSIM and stored in
**	   GlobalData for access by other functions and are stored as floats
**	   Changed HC_ProcessPkt and HC_AppInit to reflect new name of WAKEUP_MID
**	   cgesterling (22.Nov.2016)
**	   
*/

#include "hc_app.h"
#include "cfe.h"
#include "hc_msgdefs.h"
#include "hc_msgids.h"
#include "hc_cmds.h"
#include "hc_cmd_utils.h"
#include "hc_custom.h"
#include "hc_events.h"
#include "hc_perfids.h"
#include "hc_platform_cfg.h"
#include "hc_tbl.h"
#include "hc_version.h"
#include "hc_verify.h"
#include "sim_msgids.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC application global data                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

HC_GlobalData_t  HC_GlobalData;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC application -- entry point and main loop processor           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void HC_AppMain(void)
{
    uint32 RunStatus = CFE_ES_APP_RUN;
    CFE_SB_MsgPtr_t MsgPtr;
    int32 Result;

    /* Register application */
    Result = CFE_ES_RegisterApp();

    /* Performance Log (start time counter) */
    CFE_ES_PerfLogEntry(HC_APPMAIN_PERF_ID);

    /*
    ** Perform application specific initialization...
    */
    if (Result == CFE_SUCCESS)
    {
        Result = HC_AppInit();
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
        CFE_ES_PerfLogExit(HC_APPMAIN_PERF_ID);

        /* Wait for the next Software Bus message */
        Result = CFE_SB_RcvMsg(&MsgPtr, HC_GlobalData.CmdPipe, CFE_SB_PEND_FOREVER);

        /* Performance Log (start time counter) */
        CFE_ES_PerfLogEntry(HC_APPMAIN_PERF_ID);

        if (Result == CFE_SUCCESS)
        {
            /* Process Software Bus message */
            HC_ProcessPkt(MsgPtr);
        }
        else
        {
            /* Process Software Bus error */
            CFE_EVS_SendEvent(HC_SB_RECEIVE_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: SB receive: result = 0x%08X", Result);

            /* Set request to terminate main loop */
            RunStatus = CFE_ES_APP_ERROR;
        }
    }

    /*
    ** Send an event describing the reason for the termination...
    */
    CFE_EVS_SendEvent(HC_EXIT_ERR_EID, CFE_EVS_ERROR,
       "Application terminating: result = 0x%08X", Result);

    /*
    ** In case cFE Event Services is not working...
    */
    CFE_ES_WriteToSysLog("HC application terminating: result = 0x%08X\n", Result);

    /*
    ** Performance Log (stop time counter)...
    */
    CFE_ES_PerfLogExit(HC_APPMAIN_PERF_ID);

    /*
    ** Let cFE kill the task...
    */
    CFE_ES_ExitApp(RunStatus);

}
/* End HC_AppMain */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC application -- startup initialization processor              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 HC_AppInit(void)
{
    char *ErrText = "Initialization error:";
    int32 Result = CFE_SUCCESS;

    /* Initialize global data  */
    CFE_PSP_MemSet(&HC_GlobalData, 0, sizeof(HC_GlobalData_t));

    /* Initialize thermal telemetry stale flag */
    HC_GlobalData.HousekeepingPkt.TlmStale = TRUE;

    /* Register for event services */
    Result = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(HC_STARTUP_EVENTS_ERR_EID, CFE_EVS_ERROR,
           "%s register for event services: result = 0x%08X", ErrText, Result);
    }
    else
    {
        /* Create Software Bus message pipe */
        Result = CFE_SB_CreatePipe(&HC_GlobalData.CmdPipe,
                                    HC_APP_PIPE_DEPTH, HC_APP_PIPE_NAME);
        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(HC_STARTUP_CREAT_PIPE_ERR_EID, CFE_EVS_ERROR,
               "%s create SB input pipe: result = 0x%08X", ErrText, Result);
        }
        else
        {
            /* Subscribe to Housekeeping request commands */
            Result = CFE_SB_Subscribe(HC_SEND_HK_MID, HC_GlobalData.CmdPipe);

            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(HC_STARTUP_SUBSCRIB_HK_ERR_EID, CFE_EVS_ERROR,
                   "%s subscribe to HK request: result = 0x%08X", ErrText, Result);
            }
            
            /* Subscribe to HC data from SIM commands */
            Result = CFE_SB_Subscribe(SIM_HC_DATA_MID, HC_GlobalData.CmdPipe);

            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(HC_STARTUP_SUBSCRIB_SIM_ERR_EID, CFE_EVS_ERROR,
                   "%s subscribe to SIM data: result = 0x%08X", ErrText, Result);
            }

            /* Subscribe to 10 Hz wakeup from scheduler */
            Result = CFE_SB_Subscribe(HC_01HZ_WAKEUP_MID, HC_GlobalData.CmdPipe);

            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(HC_STARTUP_SUBSCRIB_WAKEUP_ERR_EID, CFE_EVS_ERROR,
                   "%s subscribe to SCH wakeup: result = 0x%08X", ErrText, Result);
            }
        }
    }

    /* Keep indentation from getting too deep */
    if (Result == CFE_SUCCESS)
    {
        /* Subscribe to HC ground command packets */
        Result = CFE_SB_Subscribe(HC_CMD_MID, HC_GlobalData.CmdPipe);

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(HC_STARTUP_SUBSCRIB_GCMD_ERR_EID, CFE_EVS_ERROR,
               "%s subscribe to HC commands: result = 0x%08X", ErrText, Result);
        }
        else
        {
            /* Initialize HC tables */
            Result = HC_TableInit();

	/* Initialize housekeeping telemetry message */
        CFE_SB_InitMsg(&HC_GlobalData.HousekeepingPkt, HC_HK_TLM_MID,
                       sizeof(HC_HousekeepingPkt_t), TRUE);

            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(HC_STARTUP_TABLE_INIT_ERR_EID, CFE_EVS_ERROR,
                   "%s register free space table: result = 0x%08X", ErrText, Result);
            }
            else
            {
               /* Application startup event message */
               CFE_EVS_SendEvent(HC_STARTUP_EID, CFE_EVS_INFORMATION,
                   "Initialization complete: version %d.%d.%d.%d",
                    HC_MAJOR_VERSION, HC_MINOR_VERSION, HC_REVISION, HC_MISSION_REV);
            }
        }
    }

    return(Result);

}
/* End of HC_AppInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC application -- input packet processor                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void HC_ProcessPkt(CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_SB_MsgId_t MessageID;

    MessageID = CFE_SB_GetMsgId(MessagePtr);

    switch(MessageID)
    {
        /* Housekeeping request */
        case HC_SEND_HK_MID:
            HC_ReportHK(MessagePtr);
            break;

        /* HC ground commands */
        case HC_CMD_MID:
            HC_ProcessCmd(MessagePtr);
            break;
         
        /* HC data from simulator */
        case SIM_HC_DATA_MID:
            HC_ReceivePkt(MessagePtr);
            break;

        /* Wakeup from scheduler */
        case HC_01HZ_WAKEUP_MID:
            HC_PerformHC(MessagePtr);
            break;

        default:
            CFE_EVS_SendEvent(HC_MID_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: invalid message ID: mid = 0x%04X", MessageID);
            break;

    }

    return;

}
/* End of HC_ProcessPkt */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC application -- command packet processor                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void HC_ProcessCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    boolean Result = TRUE;
    uint16 CommandCode = CFE_SB_GetCmdCode(MessagePtr);

    /* Invoke specific command handler */
    switch (CommandCode)
    {
        /* No-op command */
        case HC_NOOP_CC:
            Result = HC_NoopCmd(MessagePtr);
            break;
        /* Reset counters command */
        case HC_RESET_CC:
            Result = HC_ResetCountersCmd(MessagePtr);
            break;
        /* Set status command */
        case HC_SET_STATUS_CC:
            Result = HC_SetStatusCmd(MessagePtr);
            break;

        default:
            Result = FALSE;
            CFE_EVS_SendEvent(HC_CC_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: invalid command code: cc = %d", CommandCode);
            break;
    }

    if (Result == TRUE)
    {
        /* Increment command success counter */
        if (CommandCode != HC_RESET_CC)
        {
            HC_GlobalData.HousekeepingPkt.CommandCounter++;
        }
    }
    else
    {
        /* Increment command error counter */
        HC_GlobalData.HousekeepingPkt.CommandErrCounter++;
    }

    return;

}
 /* End of HC_ProcessCmd */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC application -- housekeeping request packet processor         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void HC_ReportHK(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "HK Request";
    boolean Result;
    uint8 i;
    /* Pointer to the table data */
    HC_TypeTable_t *TablePtr = HC_GlobalData.TypeTablePtr;

    /* Verify command packet length */
    Result = HC_IsValidCmdPktLength(MessagePtr, sizeof(HC_HousekeepingCmd_t),
                                    HC_HK_REQ_ERR_EID, CmdText);

    /* Report HC housekeeping telemetry data */
    if (Result == TRUE)
    {
        /* Release table pointers */
        HC_ReleaseTablePointers();

        /* Allow cFE chance to dump, update, etc. */
        HC_AcquireTablePointers();

        /* Update the HC Status array in housekeeping data */
        for (i = 0; i < HC_NUM_CTRL_TYPES; i++)
        {
            HC_GlobalData.HousekeepingPkt.Status[i] = TablePtr->Types[i].Enabled;
        }

        /* Timestamp and send housekeeping telemetry packet */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &HC_GlobalData.HousekeepingPkt);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &HC_GlobalData.HousekeepingPkt);
    }

    return;

}

 /* End of HC_ReportHK */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC application -- HC Data packet processor                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void HC_ReceivePkt(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "Receive Data Pkt";
    HC_HeaterControlPkt_t *DataPtr = (HC_HeaterControlPkt_t *) MessagePtr;
    boolean Result;
    uint8 i;
    uint8 j;
    uint8 k;
    float AverageTemp;
    uint8 Thermistors;

    HC_ComponentTable_t *ComTablePtr = HC_GlobalData.ComponentTablePtr;

    /* Verify command packet length */
    Result = HC_IsValidCmdPktLength(MessagePtr, sizeof(HC_HeaterControlPkt_t),
                                    HC_RCV_PKT_ERR_EID, CmdText);

    if (Result == TRUE)
    {
        /* Increment number of thermal packets received */
        HC_GlobalData.HousekeepingPkt.ThermalPktCount++;

        /* Set the value of thermal telemetry stale flag */
        HC_GlobalData.HousekeepingPkt.TlmStale = DataPtr->ErrorFlag; 

        /* Set the temperature values */
        for(i = 0; i < HC_MAX_COMPONENTS; i++)
        { 
	    for(j = 0; j < HC_MAX_NUM_HEATERS; j++)
	    {
	        for(k = 0; k < HC_MAX_NUM_THERM; k++)
		{
		    HC_GlobalData.ThermalDataPkt.Temps[i][j][k] = DataPtr->Temps[i][j][k];
		}
	    }
        }

	/* Calculate the average temperature for each heater from new temperature values */
	for (i = 0; i < HC_COM_TABLE_ENTRY_COUNT; i++)
	{
            for (j = 0; j < ComTablePtr->Components[i].NumHeaters; j++)
	    {
		/* Reset to 0 for each heater and read number of thermistors for this heater */
		AverageTemp = 0.0;
		Thermistors = ComTablePtr->Components[i].Heaters[j];

		for (k = 0; k < Thermistors; k++)
		{
		    /* Add temp of each thermistor to the current sum to calculate the average */
		    AverageTemp = AverageTemp + HC_GlobalData.ThermalDataPkt.Temps[i][j][k];
		}

		/* Divide by the number of thermistors to get the average and store this in GlobalData */
		AverageTemp = AverageTemp / (float) Thermistors;
		HC_GlobalData.HousekeepingPkt.AvgTemps[i][j] = AverageTemp;
	    }
	}

        /* Send event to signal command completion */
        CFE_EVS_SendEvent(HC_RCV_PKT_CMD_EID, CFE_EVS_DEBUG,
                         "%s command: Heater Control Packet Received", CmdText);
    }

    return;

}
 /* End of HC_ReceivePkt */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC application -- perfom heater control function                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void HC_PerformHC(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "Perform Heater Control";
    boolean Result;
    uint8 i;
    /* Pointer to the table data */
    HC_TypeTable_t *TablePtr = HC_GlobalData.TypeTablePtr;

    /* Verify command packet length */
    Result = HC_IsValidCmdPktLength(MessagePtr, sizeof(HC_PerformHC_t),
                                    HC_WAKEUP_ERR_EID, CmdText);

    /* Perform heater control */
    if (Result == TRUE)
    {
        /* Check if data is good */
        if (HC_GlobalData.HousekeepingPkt.TlmStale == FALSE)
        {
            /* Go through each heater control type */
            for (i = 0; i < HC_NUM_CTRL_TYPES; i++)
            {
                /* Determine if the status is enabled */
                if (TablePtr->Types[i].Enabled == HC_TYPE_ENABLED)
                {
                    /* Call the mission defined function for that type */
                    Result = HC_CustomFunction(TablePtr->Types[i].Index);
                }
            }
            /* Increment heater control cycle counter */
            HC_GlobalData.HousekeepingPkt.CycleCounter++;
        }
    }
    return;
}

 /* End of HC_PerformHC */

/************************/
/*  End of File Comment */
/************************/

