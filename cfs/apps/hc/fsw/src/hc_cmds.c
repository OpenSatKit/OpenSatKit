/*
** $Id: $
**
** Title: Heater Control (HC) Application Ground Commands
**
** Purpose: Provides functions for the execution of the HC ground commands
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** $Log: $
*/

#include "cfe.h"
#include "hc_app.h"
#include "hc_cmds.h"
#include "hc_cmd_utils.h"
#include "hc_events.h"
#include "hc_msg.h"
#include "hc_msgdefs.h"
#include "hc_msgids.h"
#include "hc_platform_cfg.h"
#include "hc_version.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC -- access to global data structure (defined in hc_app.h)     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern HC_GlobalData_t  HC_GlobalData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC command handler -- NOOP                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean HC_NoopCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "No-op";
    boolean CommandResult;

    /* Verify message length */
    CommandResult = HC_IsValidCmdPktLength(MessagePtr, sizeof(HC_NoopCmd_t),
                                           HC_NOOP_PKT_ERR_EID, CmdText);

    /* Send command completion event (info) */
    if (CommandResult == TRUE)
    {
        CFE_EVS_SendEvent(HC_NOOP_CMD_EID, CFE_EVS_INFORMATION,
                         "%s command: HC version %d.%d.%d.%d", CmdText,
                          HC_MAJOR_VERSION, HC_MINOR_VERSION, HC_REVISION, HC_MISSION_REV);
    }

    return(CommandResult);

} /* End of HC_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC command handler -- Reset Counters                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean HC_ResetCountersCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "Reset Counters";
    boolean CommandResult;

    /* Verify message length */
    CommandResult = HC_IsValidCmdPktLength(MessagePtr, sizeof(HC_ResetCmd_t),
                                           HC_RESET_PKT_ERR_EID, CmdText);

    /* Reset command counters */
    if (CommandResult == TRUE)
    {
        HC_GlobalData.HousekeepingPkt.CommandCounter = 0;
        HC_GlobalData.HousekeepingPkt.CommandErrCounter = 0;

        /* Send command completion event (debug) */
       CFE_EVS_SendEvent(HC_RESET_CMD_EID, CFE_EVS_DEBUG, "%s command", CmdText);
    }

    return(CommandResult);

} /* End of HC_ResetCountersCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC command handler -- Set HC Status                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean HC_SetStatusCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "Set Status";
    HC_SetStatusCmd_t *CmdPtr = (HC_SetStatusCmd_t *) MessagePtr;
    boolean CommandResult;
    /* Pointer to the table data */
    HC_TypeTable_t *TablePtr = HC_GlobalData.TypeTablePtr;
   
    /* Get data from SB Message */
    uint8 Type = CmdPtr->HCType;
    uint8 NewStatus = CmdPtr->HCStatus;

    /* Verify message length */
    CommandResult = HC_IsValidCmdPktLength(MessagePtr, sizeof(HC_SetStatusCmd_t),
                                           HC_SET_STATUS_PKT_ERR_EID, CmdText);

    if (CommandResult == TRUE)
    {
        /* Verify heater control type */
        if ((Type < HC_NUM_CTRL_TYPES) && (Type >= 0))
        { 
            /* Verify new heater control status */
            if (NewStatus == HC_TYPE_ENABLED)
            {
                /* Set the status of the heater control type */
                TablePtr->Types[Type].Enabled = NewStatus;

                /* Send command completion event (info) */
                CFE_EVS_SendEvent(HC_SET_STATUS_ENABLED_EID , CFE_EVS_INFORMATION, 
                    "%s command success: heater control type %d enabled.", CmdText, Type);
            }
            else if (NewStatus == HC_TYPE_DISABLED)
            {
                /* Set the status of the heater control type */
                TablePtr->Types[Type].Enabled = NewStatus;

                /* Send command completion event (info) */
                CFE_EVS_SendEvent(HC_SET_STATUS_DISABLED_EID , CFE_EVS_INFORMATION, 
                    "%s command success: heater control type %d disabled.", CmdText, Type);
            }
            else
            {
                /* Invalid heater control status */
                CFE_EVS_SendEvent(HC_SET_STATUS_ERR_EID, CFE_EVS_ERROR,
                         "Set Status error: invalid heater control status: = %d\n",
                          NewStatus);

                /* Set command result to false due to error */
                CommandResult = FALSE;
            }
        }
        else
        {
            /* Invalid heater control type */
            CFE_EVS_SendEvent(HC_SET_STATUS_ERR_EID, CFE_EVS_ERROR,
                         "Set Status error: invalid heater control type: = %d\n",
                          Type);

            /* Set command result to false due to error */
            CommandResult = FALSE;
        }
    }

    return(CommandResult);

} /* End of HC_SetStatusCmd() */


/************************/
/*  End of File Comment */
/************************/

