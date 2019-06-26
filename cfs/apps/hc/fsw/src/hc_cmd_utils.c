/*
** $Id: $
**
** Title: Heater Control (HC) Command Utility Functions
**
** Purpose: Provides heater control utility function definitions for
**          processing heater control commands
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** $Log: $
*/

#include "cfe.h"
#include "hc_msg.h"
#include "hc_cmd_utils.h"
#include "hc_perfids.h"
#include "hc_events.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC utility function -- verify command packet length             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean HC_IsValidCmdPktLength(CFE_SB_MsgPtr_t CmdPacket, uint16 ExpectedLength,
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

} /* HC_IsValidCmdPktLength */

/************************/
/*  End of File Comment */
/************************/
