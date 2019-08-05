/*
** $Id: $
**
** Title: CFS Heater Control (HC) Application Command and Telemetry
**        Packet Definitions Header File
**
** Purpose: Specification for the CFS HC command and telemetry message
**          macro definitions.
**
** Author: Lindsay A. Walton, Summer 2015 Intern for Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: $
*/

#ifndef _hc_msgdefs_h_
#define _hc_msgdefs_h_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC command packet command codes                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \hccmd No Operation
**
**  \par Description
**       This command performs no operation other than to generate an
**       informational event that also contains software version data.
**       The command is most often used as a general aliveness test by
**       demonstrating that the application can receive commands and
**       generate telemetry.
**
**  \hccmdmnemonic \HC_Noop
**
**  \par Command Packet Structure
**       #HC_NoopCmd_t
**
**  \par Command Success Verification
**       - Informational event #HC_NOOP_CMD_EID will be sent
**       - Command success counter /HC_CMDPC will increment
**
**  \par Command Error Conditions
**       - Invalid command packet length
**
**  \par Command Failure Verification
**       - Command error counter /HC_CMDEC will increment
**       - Error event #HC_NOOP_PKT_ERR_EID will be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
*/
#define HC_NOOP_CC              0


/** \hccmd Reset Counters
**
**  \par Description
**       This command resets the following housekeeping telemetry:
**       - Command success counter /HC_CMDPC
**       - Command error counter /HC_CMDEC
**       - Command warning counter /HC_WarnCtr
**
**  \hccmdmnemonic \HC_ResetCtrs
**
**  \par Command Packet Structure
**       #HC_ResetCmd_t
**
**  \par Command Success Verification
**       - Command counters will be set to zero (see description)
**       - Debug event #HC_RESET_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**
**  \par Command Failure Verification
**       - Command error counter /HC_CMDEC will increment
**       - Error event #HC_RESET_PKT_ERR_EID will be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
**
*/
#define HC_RESET_CC                  1


/** \hccmd Set Status
**
**  \par Description
**       This command sets the Heater Control Status of a specified
**       heater control type to a specified status (either enabled or
**       disabled).
**
**  \hccmdmnemonic \HC_SetStatus
**
**  \par Command Packet Structure
**       #HC_SetStatusCmd_t
**
**  \par Command Success Verification
**       - Command counters will be set to zero (see description)
**       - Debug event #HC_SET_STATUS_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**       - Invalid heater control type
**       - Invalid status
**
**  \par Command Failure Verification
**       - Command error counter /HC_CMDEC will increment
**       - Error event #HC_SET_STATUS_PKT_ERR_EID will be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
**
*/
#define HC_SET_STATUS_CC             2


#endif /* _hc_msgdefs_h_ */

/************************/
/*  End of File Comment */
/************************/
