/*
** $Id: $
**
** Title: Simulator (SIM) Command Utility Functions
**
** Purpose: Provides simulator utility function definitions for
**          processing simulator commands
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** $Log: $
*/

#include "cfe.h"
#include "sim_msg.h"
#include "sim_cmd_utils.h"
#include "sim_perfids.h"
#include "sim_events.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM utility function -- verify command packet length            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean SIM_IsValidCmdPktLength(CFE_SB_MsgPtr_t CmdPacket, uint16 ExpectedLength,
                                uint32 EventID, char *CmdText)
{
    boolean FunctionResult = TRUE;
    uint16 ActualLength = CFE_SB_GetTotalMsgLength(CmdPacket);

    /* Verify command packet length */
    if (ActualLength != ExpectedLength)
    {
        FunctionResult = FALSE;

        CFE_EVS_SendEvent(EventID, CFE_EVS_ERROR,
                         "%s error: invalid command packet length: expected = %d, actual = %d",
                          CmdText, ExpectedLength, ActualLength);
    }

    return(FunctionResult);

} /* SIM_IsValidCmdPktLength */


/************************/
/*  End of File Comment */
/************************/
