/*
** $Id: $
**
** Title: Heater Control (HC) Application Custom Commands
**
** Purpose: Provides functions for each heater control type
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** $Log: $ Changed structure of HC_Battery function such that
**	   average temperatures are no longer calculated within this 
**	   function, but simply read from HC_GlobalData
**	   Added HC_Propulsion function so that propulsion 
**	   heater control is now supported
**	   Changed HC_SendCtrlCmd function to reflect changes in HC_SendCtrlCmd_t
**	   cgesterling (18.Nov.2016)
**
*/

#include "cfe.h"
#include "hc_app.h"
#include "hc_custom.h"
#include "hc_events.h"
#include "hc_msg.h"
#include "hc_platform_cfg.h"
#include "sim_msgids.h"
#include "sim_msgdefs.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC -- access to global data structure (defined in hc_app.c)     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern HC_GlobalData_t  HC_GlobalData;


/*************************************************************************
**
** Main custom function
**
*************************************************************************/
boolean  HC_CustomFunction(uint8 Index)
{
    boolean Result;

    /* Switch on the function index */
    switch (Index)
    {
        case HC_BATTERY_FUNC:
            Result = HC_Battery();
	    CFE_EVS_SendEvent(HC_CFCALL_CMD_EID, CFE_EVS_DEBUG,
                              "Calling HC_CustomFunction: index = %d",
                              Index);
            break;
        case HC_PROPULSION_FUNC:
            Result = HC_Propulsion();
	    CFE_EVS_SendEvent(HC_CFCALL_CMD_EID, CFE_EVS_DEBUG,
                              "Calling HC_CustomFunction: index = %d",
                              Index);
            break;
        default:
            CFE_EVS_SendEvent(HC_CFCALL_ERR_EID, CFE_EVS_ERROR,
                              "Invalid HC_CustomFunction call: index = %d",
                              Index);
            Result = FALSE;
            break;

    }
    return(Result);
}
/* End HC_CustomFunction */

/*************************************************************************
**
** Mission specific functions
**
*************************************************************************/

/* Battery function */
boolean HC_Battery(void)
{
    boolean Result = TRUE;
    uint8 i; /* Used to cycle through components */
    uint8 j; /* Used to cycle through heaters */

    /* Access to table data */
    HC_TypeTable_t *TypeTablePtr = HC_GlobalData.TypeTablePtr;
    HC_ComponentTable_t *ComTablePtr = HC_GlobalData.ComponentTablePtr;

    /* Determine heater control */
    for (i = 0; i < HC_COM_TABLE_ENTRY_COUNT; i++)
    {
	if (ComTablePtr->Components[i].Type == HC_BATTERY_TYPE)
	{
            for (j = 0; j < ComTablePtr->Components[i].NumHeaters; j++)
	    {
		/* If average temperature is lower than the low set point */
		if (HC_GlobalData.HousekeepingPkt.AvgTemps[i][j] < TypeTablePtr->Types[HC_BATTERY_TYPE].TempLow)
		{
		    CFE_EVS_SendEvent(HC_SEND_CTRL_CMD_EID, CFE_EVS_DEBUG,
                              "Sending Heater Control command: Turning heater %d on component %d ON",
                              j, i);

		    /* Send command to turn the heater on */
		    Result = HC_SendCtrlCmd(i, j, TRUE);
		} 
    
		/* If average temperature is higher than the high set point */
		else if (HC_GlobalData.HousekeepingPkt.AvgTemps[i][j] > TypeTablePtr->Types[HC_BATTERY_TYPE].TempHigh)
		{
		    CFE_EVS_SendEvent(HC_SEND_CTRL_CMD_EID, CFE_EVS_DEBUG,
                              "Sending Heater Control command: Turning heater %d on component %d OFF",
                              j, i);

		    /* Send command to turn the heater off */
		    Result = HC_SendCtrlCmd(i, j, FALSE);
		}

		else
		{
		    CFE_EVS_SendEvent(HC_CFCALL_CMD_EID, CFE_EVS_DEBUG,
                              "No heater control needed for heater %d on component %d",
                              j, i);
		}
            }
        }
    }
 
    return(Result);
} /* End HC_Battery */


/* Propulsion function */
boolean HC_Propulsion(void)
{
    boolean Result = TRUE;
    uint8 i; /* Used to cycle through components */
    uint8 j; /* Used to cycle through heaters */

    /* Access to table data */
    HC_TypeTable_t *TypeTablePtr = HC_GlobalData.TypeTablePtr;
    HC_ComponentTable_t *ComTablePtr = HC_GlobalData.ComponentTablePtr;

    /* Determine heater control */
    for (i = 0; i < HC_COM_TABLE_ENTRY_COUNT; i++)
    {
	if (ComTablePtr->Components[i].Type == HC_PROPULSION_TYPE)
	{
            for (j = 0; j < ComTablePtr->Components[i].NumHeaters; j++)
	    {
		/* If average temperature is lower than the low set point */
		if (HC_GlobalData.HousekeepingPkt.AvgTemps[i][j] < TypeTablePtr->Types[HC_PROPULSION_TYPE].TempLow)
		{
		    CFE_EVS_SendEvent(HC_SEND_CTRL_CMD_EID, CFE_EVS_DEBUG,
                              "Sending Heater Control command: Turning heater %d on component %d ON",
                              j, i);

		    /* Send command to turn the heater on */
		    Result = HC_SendCtrlCmd(i, j, TRUE);
		} 
    
		/* If average temperature is higher than the high set point */
		else if (HC_GlobalData.HousekeepingPkt.AvgTemps[i][j] > TypeTablePtr->Types[HC_PROPULSION_TYPE].TempHigh)
		{
		    CFE_EVS_SendEvent(HC_SEND_CTRL_CMD_EID, CFE_EVS_DEBUG,
                              "Sending Heater Control command: Turning heater %d on component %d OFF",
                              j, i);

		    /* Send command to turn the heater off */
		    Result = HC_SendCtrlCmd(i, j, FALSE);
		}

		else
		{
		    CFE_EVS_SendEvent(HC_CFCALL_CMD_EID, CFE_EVS_DEBUG,
                              "No heater control needed for heater %d on component %d",
                              j, i);
		}
            }
        }
    }
 
    return(Result);
} /* End HC_Propulsion */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Send Heater Control Command                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean HC_SendCtrlCmd(uint8 Component, uint8 Heater, boolean TurnOn)
{
    boolean CommandResult;
    HC_SendCtrlCmd_t HC_SendCtrlCmd;
    
    /* Initialize heater control command message */
    CFE_SB_InitMsg(&HC_SendCtrlCmd, SIM_CMD_MID,
                   sizeof(HC_SendCtrlCmd_t), TRUE);

    /* Initialize data in the message */
    HC_SendCtrlCmd.ComponentIndex = Component;
    HC_SendCtrlCmd.HeaterIndex = Heater;
    HC_SendCtrlCmd.TurnOnFlag = TurnOn;

    /* Set the message ID */
    CFE_SB_SetMsgId((CFE_SB_Msg_t *) &HC_SendCtrlCmd, SIM_CMD_MID);

    /* Set the command code */
    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &HC_SendCtrlCmd, SIM_CTRL_HEATER_CC);

    /* Send heater control command packet */
    CommandResult = CFE_SB_SendMsg((CFE_SB_Msg_t *) &HC_SendCtrlCmd);

    return(CommandResult);

} /* End of HC_SendCtrlCmd() */

/************************/
/*  End of File Comment */
/************************/
