/*
** $Id: $
**
** Title: Core Flight System (CFS) Simulator (SIM) Application
**
** Purpose: The Simulator (SIM) Application provides onboard heater control services.
**          When the Simulator application receives a housekeeping request
**          (scheduled within the scheduler application), SIM reports its housekeeping
**          status values via telemetry messaging.
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** $Log: $
*/
#include <stdlib.h>
#include <time.h>
#include "sim_app.h"
#include "cfe.h"
#include "sim_msgdefs.h"
#include "sim_msgids.h"
#include "sim_cmds.h"
#include "sim_cmd_utils.h"
#include "sim_events.h"
#include "sim_perfids.h"
#include "sim_platform_cfg.h"
#include "sim_version.h"
#include "sim_verify.h"
#include "hc_msgids.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM application global data                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

SIM_GlobalData_t  SIM_GlobalData;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM application -- entry point and main loop processor          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SIM_AppMain(void)
{
    uint32 RunStatus = CFE_ES_APP_RUN;
    CFE_SB_MsgPtr_t MsgPtr;
    int32 Result;
    srand(time(NULL));

    /* Register application */
    Result = CFE_ES_RegisterApp();

    /* Performance Log (start time counter) */
    CFE_ES_PerfLogEntry(SIM_APPMAIN_PERF_ID);

    /*
    ** Perform application specific initialization...
    */
    if (Result == CFE_SUCCESS)
    {
        Result = SIM_AppInit();
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
        CFE_ES_PerfLogExit(SIM_APPMAIN_PERF_ID);

        /* Wait for the next Software Bus message */
        Result = CFE_SB_RcvMsg(&MsgPtr, SIM_GlobalData.CmdPipe, CFE_SB_PEND_FOREVER); 

        /* Performance Log (start time counter) */
        CFE_ES_PerfLogEntry(SIM_APPMAIN_PERF_ID);

        if (Result == CFE_SUCCESS)
        {
            /* Process Software Bus message */
            SIM_ProcessPkt(MsgPtr);
        }
        else
        {
            /* Process Software Bus error */
            CFE_EVS_SendEvent(SIM_SB_RECEIVE_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: SB receive: result = 0x%08X", Result);

            /* Set request to terminate main loop */
            RunStatus = CFE_ES_APP_ERROR;
        }
    }

    /*
    ** Send an event describing the reason for the termination...
    */
    CFE_EVS_SendEvent(SIM_EXIT_ERR_EID, CFE_EVS_ERROR,
       "Application terminating: result = 0x%08X", Result);

    /*
    ** In case cFE Event Services is not working...
    */
    CFE_ES_WriteToSysLog("SIM application terminating: result = 0x%08X\n", Result); 

    /*
    ** Performance Log (stop time counter)...
    */
    CFE_ES_PerfLogExit(SIM_APPMAIN_PERF_ID);

    /*
    ** Let cFE kill the task...
    */
    CFE_ES_ExitApp(RunStatus);

}
/* End SIM_AppMain */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM application -- startup initialization processor             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 SIM_AppInit(void)
{
    char *ErrText = "Initialization error:";
    int32 Result = CFE_SUCCESS;

    /* Initialize global data  */
    CFE_PSP_MemSet(&SIM_GlobalData, 0, sizeof(SIM_GlobalData_t));

    /* Set the global data */
    SIM_SetData();

    /* Register for event services */
    Result = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SIM_STARTUP_EVENTS_ERR_EID, CFE_EVS_ERROR,
           "%s register for event services: result = 0x%08X", ErrText, Result);
    }
    else
    {
        /* Create Software Bus message pipe */
        Result = CFE_SB_CreatePipe(&SIM_GlobalData.CmdPipe,
                                    SIM_APP_PIPE_DEPTH, SIM_APP_PIPE_NAME);
        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(SIM_STARTUP_CREAT_PIPE_ERR_EID, CFE_EVS_ERROR,
               "%s create SB input pipe: result = 0x%08X", ErrText, Result);
        }
        else
        {
            /* Subscribe to Housekeeping request commands */
            Result = CFE_SB_Subscribe(SIM_SEND_HK_MID, SIM_GlobalData.CmdPipe);

            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(SIM_STARTUP_SUBSCRIB_HK_ERR_EID, CFE_EVS_ERROR,
                   "%s subscribe to HK request: result = 0x%08X", ErrText, Result);
            }

            /* Subscribe to scheduler request commands */
            Result = CFE_SB_Subscribe(SIM_SEND_DATA_MID, SIM_GlobalData.CmdPipe);

            if (Result != CFE_SUCCESS)
            { 
                CFE_EVS_SendEvent(SIM_STARTUP_SUBSCRIB_SCH_ERR_EID, CFE_EVS_ERROR,
                   "%s subscribe to SCH request: result = 0x%08X", ErrText, Result);
            }

        }

    }

    /* Keep indentation from getting too deep */
    if (Result == CFE_SUCCESS)
    {
        /* Subscribe to SIM ground command packets */
        Result = CFE_SB_SubscribeEx(SIM_CMD_MID, SIM_GlobalData.CmdPipe,CFE_SB_Default_Qos,6);

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(SIM_STARTUP_SUBSCRIB_GCMD_ERR_EID, CFE_EVS_ERROR,
               "%s subscribe to SIM commands: result = 0x%08X", ErrText, Result);
        }
        else
        {
             /* Application startup event message */
             CFE_EVS_SendEvent(SIM_STARTUP_EID, CFE_EVS_INFORMATION,
                   "Initialization complete: version %d.%d.%d.%d",
                    SIM_MAJOR_VERSION, SIM_MINOR_VERSION, SIM_REVISION, SIM_MISSION_REV);
        }
    }

    return(Result);

}
/* End of SIM_AppInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM application -- input packet processor                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SIM_ProcessPkt(CFE_SB_MsgPtr_t MessagePtr)
{
    CFE_SB_MsgId_t MessageID;

    MessageID = CFE_SB_GetMsgId(MessagePtr);

    switch(MessageID)
    {
        /* Housekeeping request */
        case SIM_SEND_HK_MID:
            SIM_ReportHK(MessagePtr);
            break;

        /* SIM ground commands */
        case SIM_CMD_MID:
            SIM_ProcessCmd(MessagePtr);
            break;
       
        /* Scheduler wakeup to send data */
        case SIM_SEND_DATA_MID:
            SIM_SendTlmData(MessagePtr);
            break;

        default:
            CFE_EVS_SendEvent(SIM_MID_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: invalid message ID: mid = 0x%04X", MessageID);
            break;

    }

    return;

}
/* End of SIM_ProcessPkt */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM application -- command packet processor                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SIM_ProcessCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    boolean Result = TRUE;
    uint16 CommandCode = CFE_SB_GetCmdCode(MessagePtr);

    /* Invoke specific command handler */
    switch (CommandCode)
    {
        case SIM_NOOP_CC:
            Result = SIM_NoopCmd(MessagePtr);
            break;

        case SIM_RESET_CC:
            Result = SIM_ResetCountersCmd(MessagePtr);
            break;

        case SIM_CTRL_HEATER_CC:
            Result = SIM_ReceiveCmd(MessagePtr);
            break;

	case SIM_HEATER_MISBEHAVE_CC:
	    Result = SIM_MisbehaveCmd(MessagePtr);
	    break;

        default:
            Result = FALSE;
            CFE_EVS_SendEvent(SIM_CC_ERR_EID, CFE_EVS_ERROR,
               "Main loop error: invalid command code: cc = %d", CommandCode);
            break;
    }

    if (Result == TRUE)
    {
        /* Increment command success counter */
        if (CommandCode != SIM_RESET_CC)
        {
            SIM_GlobalData.HousekeepingPkt.CommandCounter++;
        }
    }
    else
    {
        /* Increment command error counter */
        SIM_GlobalData.HousekeepingPkt.CommandErrCounter++;
    }

    return;

}
/* End of SIM_ProcessCmd */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM application -- housekeeping request packet processor        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SIM_ReportHK(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "HK Request";
    boolean Result;

    /* Verify command packet length */
    Result = SIM_IsValidCmdPktLength(MessagePtr, sizeof(SIM_HousekeepingCmd_t),
                                    SIM_HK_REQ_ERR_EID, CmdText);

    /* Report SIM housekeeping telemetry data */
    if (Result == TRUE)
    {

        /* Initialize housekeeping telemetry message */
        CFE_SB_InitMsg(&SIM_GlobalData.HousekeepingPkt, SIM_HK_TLM_MID,
                       sizeof(SIM_HousekeepingPkt_t), FALSE);

        /* Timestamp and send housekeeping telemetry packet */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &SIM_GlobalData.HousekeepingPkt);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &SIM_GlobalData.HousekeepingPkt);
    }

    return;

} /* End of SIM_ReportHK */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM application -- HC Command packet processor                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean SIM_ReceiveCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "Receive Cmd";
    SIM_CmdPkt_t *DataPtr = (SIM_CmdPkt_t *) MessagePtr;
    boolean Result;
    uint8 ComponentIndex;
    uint8 HeaterIndex;
    boolean TurnOnFlag;

    /* Verify command packet length */
    Result = SIM_IsValidCmdPktLength(MessagePtr, sizeof(SIM_CmdPkt_t),
                                    SIM_RCV_CMD_ERR_EID, CmdText);

    /* Send event */
    if (Result == TRUE)
    {
        /* Get data from packet */
	ComponentIndex = DataPtr->Component;
        HeaterIndex = DataPtr->Heater;
        TurnOnFlag = DataPtr->TurnOn;


	if(ComponentIndex < SIM_NUM_COMPONENTS && ComponentIndex >= 0)
	{
	    if(HeaterIndex < SIM_MAX_NUM_HEATERS && HeaterIndex >= 0)
	    {
		if(SIM_GlobalData.Misbehave == FALSE || (ComponentIndex != 0 || HeaterIndex != 1))
		{
		    /* Turn the heater off or on */
		    SIM_GlobalData.HousekeepingPkt.Heaters[ComponentIndex][HeaterIndex] = TurnOnFlag;

		    /* Send event to signal command completion */
		    CFE_EVS_SendEvent(SIM_RCV_CMD_EID, CFE_EVS_DEBUG,
 		        "%s command: HC Command Received", CmdText);
		}
	    }
	    else
	    {
		CFE_EVS_SendEvent(SIM_RCV_CMD_ERR_EID, CFE_EVS_ERROR,
                   "Receive command error: invalid heater index %d", HeaterIndex);
	    }
	}
	else
	{
	    CFE_EVS_SendEvent(SIM_RCV_CMD_ERR_EID, CFE_EVS_ERROR,
               "Receive command error: invalid component index %d", ComponentIndex);
	} 
    }

    return(Result);

}
 /* End of SIM_ReceiveCmd */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM application -- Send thermal telemetry packets               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SIM_SendTlmData(CFE_SB_MsgPtr_t MessagePtr)
{
    uint8 i;
    uint8 j;
    uint8 k;
    float randomnumber;
 
    char *CmdText = "Therm Data Request";
    boolean Result;

    randomnumber = -0.5 + (rand() % 101) / 100.0;

    /* Verify command packet length */
    Result = SIM_IsValidCmdPktLength(MessagePtr, sizeof(SIM_HousekeepingCmd_t),
                                    SIM_DATA_PKT_ERR_EID, CmdText);

    /* Report SIM housekeeping telemetry data */
    if (Result == TRUE)
    {
	/* Set flag marking telemetry as stale */
	SIM_GlobalData.TempData.StaleFlag = TRUE;

	if(SIM_GlobalData.Counter % 2 == 0)  // every two seconds second
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			if(SIM_GlobalData.HousekeepingPkt.Heaters[i][j] == TRUE)
		    	{
			    /* If this heater is turned on, increase the temperature
			       of all of its thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+=0.5;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
		    }
		}
	    }		    
	}

	if(SIM_GlobalData.Counter % 5 == 0)  // every two seconds second
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			if(SIM_GlobalData.HousekeepingPkt.Heaters[i][j] == FALSE)
		    	{
			    /* If this heater is turned on, increase the temperature
			       of all of its thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-=0.5;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
		    }
		}
	    }		    
	}

	if(SIM_GlobalData.Counter % 9 == 0)  // every two seconds second
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			if(SIM_GlobalData.HousekeepingPkt.Heaters[i][j] == FALSE)
		    	{
			    /* If this heater is turned on, increase the temperature
			       of all of its thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+=0.3;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
		    }
		}
	    }		    
	}

	if(SIM_GlobalData.Counter % 6 == 0)  // every three seconds
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			if(k == 1)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.55 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* If this heater is turned off, decrease the temperature
			       of all of its odd thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.55 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(k == 0)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.3 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.3 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
		    }
		}
	    }
	}

	if(SIM_GlobalData.Counter % 7 == 0)  // every three seconds
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			if(k == 1)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.55 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* If this heater is turned off, decrease the temperature
			       of all of its odd thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.55 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}

			if(k == 2 && j == 2)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.5 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.5 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(k == 0)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.3 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.3 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(j % 2 == 0)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.4 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Increase the temperature of all thermistors
			       on odd heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.4 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
		    }
		}
	    }		    
	}
	
	if(SIM_GlobalData.Counter % 8 == 0)  // every three seconds
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			if(k == 2 && j == 1)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.6 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.6 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(k == 2 && j == 2)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.5 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.5 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(k == 1)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.2 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.2 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(k == 0)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.3 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* If this heater is turned off, decrease the temperature
			       of all of its odd thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.3 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(j == 0 || j == 1)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.4 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Increase the temperature of all thermistors
			       on odd heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.4 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
		    }
		}
	    }		    
	}

	if(SIM_GlobalData.Counter % 9 == 0)  // every three seconds
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			if(k == 0)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.3 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* If this heater is turned off, decrease the temperature
			       of all of its odd thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.3 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(k == 2 && j == 1)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.6 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.6 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(k == 2 && j == 0)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.8 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.8 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(j == 0)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.33 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.33 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(j % 2 == 0)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.4 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Increase the temperature of all thermistors
			       on odd heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.4 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
		    }
		}
	    }		    
	}

	if(SIM_GlobalData.Counter % 12 == 0)  // every three seconds
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			if(j == 0)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.33 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.33 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(j == 1 && k == 0)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.75 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.75 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
		    }
		}
	    }
	}

	if(SIM_GlobalData.Counter % 13 == 0)  // every three seconds
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			if(k == 1)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.2 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.2 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(j == 0 || j == 1)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.4 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Increase the temperature of all thermistors
			       on odd heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.4 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(j == 1)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.25 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.25 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(i == 0)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.7 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
		    }
		}
	    }
	}

	if(SIM_GlobalData.Counter % 14 == 0)  // every three seconds
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			if(k == 2 && j == 0)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.8 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.8 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(j == 1 && k == 0)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.75 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.75 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			if(i == 0)
			{
			    /* If this heater is turned off, increase the temperature
			       of all of its even thermistors by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.7 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
		    }
		}
	    }
	}
	
	if(SIM_GlobalData.Counter % 27 == 0)  // every five seconds
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			/* Decrease the temperature of all thermistors
	  	 	   by one degree */
			SIM_GlobalData.TempData.Temps[i][j][k]+= 0.2;
			SIM_GlobalData.TempData.StaleFlag = FALSE;

			if(j == 1)
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]-= 0.25 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
			else
			{
			    /* Decrease the temperature of all thermistors
			       on even heaters by one degree */
			    SIM_GlobalData.TempData.Temps[i][j][k]+= 0.25 * randomnumber;
			    SIM_GlobalData.TempData.StaleFlag = FALSE;
			}
		    }
		}
	    }		    
	}

	if(SIM_GlobalData.Counter % 29 == 0)  // every five seconds
	{
	    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
	    {
		for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
		{
		    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
		    {
			/* Decrease the temperature of all thermistors
	  	 	   by one degree */
			SIM_GlobalData.TempData.Temps[i][j][k]-= 0.25;
			SIM_GlobalData.TempData.StaleFlag = FALSE;
		    }
		}
	    }		    
	}

	if(SIM_GlobalData.Counter % 31 == 0)  // every thirteen seconds
	{
	    /* Set flag marking telemetry as stale */
	    SIM_GlobalData.TempData.StaleFlag = TRUE;		    
	}

	if(SIM_GlobalData.Counter % 2 == 0)  // every two seconds
	{
	    /* Call the function to send the data */
	    SIM_SendDataPktCmd();
	}

	/* If counter has reached the max value an int8 can hold */
	if (SIM_GlobalData.Counter == 255)
	{
            /* Reset Counter to zero to loop it */
            SIM_GlobalData.Counter = 0;
	}
	else
	{
	    /* Increment Counter so that different loops happen next time */
	    SIM_GlobalData.Counter++;
	}
    }
    return;

} /* End SIM_SendTlmData */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM application -- Hard code the thermal telemetry              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SIM_SetData(void)
{
    uint8 i;	// Cycle through components
    uint8 j;	// Cycle through heaters
    uint8 k;	// Cycle through thermistors

    /* Initialize telemetry as stale */
    SIM_GlobalData.TempData.StaleFlag = TRUE;

    /* Initialize data counter at zero */
    SIM_GlobalData.Counter = 0;

    SIM_GlobalData.Misbehave = FALSE;

    for(i = 0; i < SIM_NUM_COMPONENTS; i++)
    {
	for(j = 0; j < SIM_MAX_NUM_HEATERS; j++)
	{
	    /* Initialize all heaters to be in an off state */
	    SIM_GlobalData.HousekeepingPkt.Heaters[i][j] = FALSE;

	    for(k = 0; k < SIM_MAX_NUM_THERMS; k++)
	    {
		/* Initialize temperature data on all thermistors */
		if(i == 0)	// battery component
		{
		    /* Initialize with temperatures that are within
		       acceptable range for battery type */
		    SIM_GlobalData.TempData.Temps[i][j][k] = 30.0;  
		}
		else		// propulsion component
		{
		    /* Initialize with temperatures that are within
		       acceptable range for propulsion type */
		    SIM_GlobalData.TempData.Temps[i][j][k] = 15.0;  
		}
	    }
	}
    }

    return;

} /* End SIM_SetData */


/************************/
/*  End of File Comment */
/************************/

