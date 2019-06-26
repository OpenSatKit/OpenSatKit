/*
** $Id: $
**
** Title: CFS Simulator (SIM) Application Command and Telemetry
**        Packet Definitions Header File
**
** Purpose: Specification for the CFS SIM command and telemetry message
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

#ifndef _sim_msgdefs_h_
#define _sim_msgdefs_h_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM command packet command codes                                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \simcmd No Operation
**
**  \par Description
**       This command performs no operation other than to generate an
**       informational event that also contains software version data.
**       The command is most often used as a general aliveness test by
**       demonstrating that the application can receive commands and
**       generate telemetry.
**
**  \simcmdmnemonic \SIM_Noop
**
**  \par Command Packet Structure
**       #SIM_NoopCmd_t
**
**  \par Command Success Verification
**       - Informational event #SIM_NOOP_CMD_EID will be sent
**       - Command success counter /SIM_CMDPC will increment
**
**  \par Command Error Conditions
**       - Invalid command packet length
**
**  \par Command Failure Verification
**       - Command error counter /SIM_CMDEC will increment
**       - Error event #SIM_NOOP_PKT_ERR_EID will be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
*/
#define SIM_NOOP_CC              0


/** \simcmd Reset Counters
**
**  \par Description
**       This command resets the following housekeeping telemetry:
**       - Command success counter /SIM_CMDPC
**       - Command error counter /SIM_CMDEC
**       - Command warning counter /SIM_WarnCtr
**
**  \simcmdmnemonic \SIM_ResetCtrs
**
**  \par Command Packet Structure
**       #SIM_ResetCmd_t
**
**  \par Command Success Verification
**       - Command counters will be set to zero (see description)
**       - Debug event #SIM_RESET_CMD_EID will be sent
**
**  \par Command Error Conditions
**       - Invalid command packet length
**
**  \par Command Failure Verification
**       - Command error counter /SIM_CMDEC will increment
**       - Error event #SIM_RESET_PKT_ERR_EID will be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
**
*/
#define SIM_RESET_CC             1


/** \simcmd Send Data Packet
**
**  \par Description
**       This command sends a data packet to Heater Control.
**
**  \simcmdmnemonic \SIM_SendDataPkt
**
**  \par Command Packet Structure
**       #SIM_SendDataPktCmd_t
**
**  \par Command Success Verification
**       - Informational event #SIM_SEND_DATA_PKT_CMD_EID will be sent
**       - Command success counter /SIM_CMDPC will increment
**
**  \par Command Error Conditions
**       - Invalid command packet length
**
**  \par Command Failure Verification
**       - Command error counter /SIM_CMDEC will increment
**       - Error event #SIM_DATA_PKT_ERR_EID will be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
**
*/
#define SIM_HEATER_MISBEHAVE_CC              2


/** \simcmd Receive Heater Control Command
** 
**  \par Description
**       This command performs the proper heater control according to
**       the heater control command received. It will turn a command
**       specified heater on or off. 
**
**  \simcmdmnemonic \SIM_ReceiveCmd
**
**  \par Command Packet Structure
**       #SIM_CmdPkt_t
**
**  \par Command Success Verification
**       - Informational event #SIM_RCV_CMD_EID will be sent
**       - Command success counter /SIM_CMDPC will increment
**
**  \par Command Error Conditions
**       - Invalid command packet length
**
**  \par Command Failure Verification
**       - Command error counter /SIM_CMDEC will increment
**       - Error event #SIM_RCV_CMD_ERR_EID will be sent
**
**  \par Criticality
**       - There are no critical issues related to this command.
**
*/
#define SIM_CTRL_HEATER_CC                3


#endif /* _sim_msgdefs_h_ */

/************************/
/*  End of File Comment */
/************************/
