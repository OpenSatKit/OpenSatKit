/*
** $Id: $
**
** Title: Simulator (SIM) Application Ground Commands
**
** Purpose: Provides functions for the execution of the SIM ground commands
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** $Log: $
*/

#include "cfe.h"
#include "sim_msg.h"
#include "sim_msgdefs.h"
#include "sim_msgids.h"
#include "sim_events.h"
#include "sim_app.h"
#include "sim_cmds.h"
#include "sim_cmd_utils.h"
#include "sim_platform_cfg.h"
#include "sim_version.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM -- access to global data structure (defined in sim_app.c)   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern SIM_GlobalData_t  SIM_GlobalData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM command handler -- NOOP                                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean SIM_NoopCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "No-op";
    boolean CommandResult;

    /* Verify message length */
    CommandResult = SIM_IsValidCmdPktLength(MessagePtr, sizeof(SIM_NoopCmd_t),
                                           SIM_NOOP_PKT_ERR_EID, CmdText);

    /* Send command completion event (info) */
   if (CommandResult == TRUE)
    {
        CFE_EVS_SendEvent(SIM_NOOP_CMD_EID, CFE_EVS_INFORMATION,
                         "%s command: SIM version %d.%d.%d.%d", CmdText,
                          SIM_MAJOR_VERSION, SIM_MINOR_VERSION, SIM_REVISION, SIM_MISSION_REV);
    }

    return(CommandResult);

} /* End of SIM_NoopCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM command handler -- Reset Counters                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean SIM_ResetCountersCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "Reset Counters";
    boolean CommandResult;

    /* Verify message length */
    CommandResult = SIM_IsValidCmdPktLength(MessagePtr, sizeof(SIM_ResetCmd_t),
                                           SIM_RESET_PKT_ERR_EID, CmdText);

    /* Reset command counters */
    if (CommandResult == TRUE)
    {
        SIM_GlobalData.HousekeepingPkt.CommandCounter = 0;
        SIM_GlobalData.HousekeepingPkt.CommandErrCounter = 0;

        /* Send command completion event (debug) */
       CFE_EVS_SendEvent(SIM_RESET_CMD_EID, CFE_EVS_DEBUG, "%s command", CmdText);
    }

    return(CommandResult);

} /* End of SIM_ResetCountersCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM command handler -- Send Data Packet                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SIM_SendDataPktCmd(void)
{
    char *CmdText = "Send Data Packet";

    /* Initialize thermal data telemetry message */
    CFE_SB_InitMsg(&SIM_GlobalData.TempData, SIM_HC_DATA_MID,
                   sizeof(SIM_HeaterControlPkt_t), FALSE);

    /* Timestamp and send thermal data telemetry packet */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &SIM_GlobalData.TempData);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &SIM_GlobalData.TempData);

    /* Send command completion event (debug) */
    CFE_EVS_SendEvent(SIM_SEND_DATA_PKT_CMD_EID, CFE_EVS_DEBUG, "%s command", CmdText);

    return;

} /* End of SIM_SendDataPktCmd() */


boolean SIM_MisbehaveCmd(CFE_SB_MsgPtr_t MessagePtr)
{
    char *CmdText = "Misbehave";
    boolean CommandResult;
    SIM_MisbehaveCmd_t *DataPtr = (SIM_MisbehaveCmd_t *) MessagePtr;

    /* Verify message length */
    CommandResult = SIM_IsValidCmdPktLength(MessagePtr, sizeof(SIM_MisbehaveCmd_t),
                                           SIM_MISBEHAVE_PKT_ERR_EID, CmdText);

    /* Reset command counters */
    if (CommandResult == TRUE)
    {
        SIM_GlobalData.Misbehave = DataPtr->Misbehave;
    }

    return(CommandResult);

}

/************************/
/*  End of File Comment */
/************************/

