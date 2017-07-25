/*
** $Id: $
**
** Title: CFS Simulator (SIM) Application Event ID Header File
**
** Purpose: Specification for the CFS Simulator Event Identifers.
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

#ifndef _sim_events_h_
#define _sim_events_h_


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SIM event message ID's                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \brief <tt> 'Initialization Complete' </tt>
**  \event <tt> 'Initialization complete: version \%d.\%d.\%d.\%d' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message is issued after the Simulator application has
**  successfully completed startup initialization.
**
**  The version numbers indicate the application major version number,
**  minor version number, revision number and mission revision number.
*/
#define SIM_STARTUP_EID                      1


/** \brief <tt> 'Initialization Error: Register for Event Services' </tt>
**  \event <tt> 'Initialization error: register for event services: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Simulator application has
**  failed in its attempt to register for event services during startup
**  initialization.
**
**  This is a fatal error that will cause the Simulator application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_EVS_Register.
*/
#define SIM_STARTUP_EVENTS_ERR_EID           2


/** \brief <tt> 'Initialization Error: Create SB Input Pipe' </tt>
**  \event <tt> 'Initialization error: create SB input pipe: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Simulator application has
**  failed in its attempt to create a Software Bus input pipe during startup
**  initialization.
**
**  This is a fatal error that will cause the Simulator application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_CreatePipe.
*/
#define SIM_STARTUP_CREAT_PIPE_ERR_EID       3


/** \brief <tt> 'Initialization Error: Subscribe to HK Request' </tt>
**  \event <tt> 'Initialization error: subscribe to HK request: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Simulator application has
**  failed in its attempt to subscribe to the HK telemetry request command
**  during startup initialization.
**
**  This is a fatal error that will cause the Simulator application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_Subscribe.
*/
#define SIM_STARTUP_SUBSCRIB_HK_ERR_EID      4


/** \brief <tt> 'Initialization Error: Subscribe to SIM Commands' </tt>
**  \event <tt> 'Initialization error: subscribe to SIM commands: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Simulator application has
**  failed in its attempt to subscribe to the SIM ground command packet
**  during startup initialization.
**
**  This is a fatal error that will cause the Simulator application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_Subscribe.
*/
#define SIM_STARTUP_SUBSCRIB_GCMD_ERR_EID    5


/** \brief <tt> 'Main Loop Error: Software Bus Receive' </tt>
**  \event <tt> 'Main Loop Error: SB receive: result = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the Simulator application has
**  failed in its attempt to read from its Software Bus input pipe while
**  processing the software main loop sequence.
**
**  This is a fatal error that will cause the Simulator application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_RcvMsg.
*/
#define SIM_SB_RECEIVE_ERR_EID               7


/** \brief <tt> 'Application Terminating' </tt>
**  \event <tt> 'Application terminating: result = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the Simulator application is about
**  to terminate.
**
**  A non-zero result value in the event text is the error code from a
**  fatal error that has occurred.  Fatal errors all have descriptive
**  events.
**
**  If the result value in the event text is zero, then it is likely that
**  the CFE has terminated the SIM application, presumably by command.
*/
#define SIM_EXIT_ERR_EID                     8


/** \brief <tt> 'Main Loop Error: Invalid Message ID' </tt>
**  \event <tt> 'Main loop error: invalid message ID: mid = 0x\%04X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the Simulator application has
**  received an unexpected Software Bus packet. There is no obvious
**  explanation of why or how SIM could receive such a packet.
**
**  The number in the message text is the unexpected MessageID.
*/
#define SIM_MID_ERR_EID                      9


/** \brief <tt> 'Main Loop Error: Invalid Command Code' </tt>
**  \event <tt> 'Main loop error: invalid command code: cc = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the Simulator application has
**  received a command packet with an unexpected command code value.
**
**  Mal-formed command packets are generally prevented by the ground
**  system.  Therefore, the source for the problem command is likely
**  to be one of the on-board tables that contain commands.
**
**  The number in the message text is the unexpected command code.
*/
#define SIM_CC_ERR_EID                       10


/** \brief <tt> 'HK Request Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'HK Request error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a housekeeping
**  request command packet with an invalid length.
*/
#define SIM_HK_REQ_ERR_EID                   11


/** \brief <tt> 'No-op Command Success' </tt>
**  \event <tt> 'No-op command: version \%d.\%d.\%d.\%d' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /SIM_Noop command.
**
**  The version data includes the application major version, minor version,
**  revision and mission revision numbers.
*/
#define SIM_NOOP_CMD_EID                     12


/** \brief <tt> 'No-op Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'No-op error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /SIM_Noop
**  command packet with an invalid length.
*/
#define SIM_NOOP_PKT_ERR_EID                 13


/** \brief <tt> 'Reset Counters Command Success' </tt>
**  \event <tt> 'Reset Counters command' </tt>
**
**  \par Type: DEBUG
**
**  This event is type debug because the command resets housekeeping
**  telemetry counters that also signal the completion of the command.
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /SIM_ResetCtrs command.
*/
#define SIM_RESET_CMD_EID                    14


/** \brief <tt> 'Reset Counters Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Reset Counters error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /SIM_ResetCtrs
**  command packet with an invalid length.
*/
#define SIM_RESET_PKT_ERR_EID                15


/** \brief <tt> 'Send Data Packet Command Success' </tt>
**  \event <tt> 'Send Data Packet command: version \%d.\%d.\%d.\%d' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /SIM_SendDataPkt command.
**
**  The version data includes the application major version, minor version,
**  revision and mission revision numbers.
*/
#define SIM_SEND_DATA_PKT_CMD_EID            16


/** \brief <tt> 'Send Data Packet Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Send Data Packet error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /SIM_SendDataPKt
**  command packet with an invalid length.
*/
#define SIM_DATA_PKT_ERR_EID                 17


/** \brief <tt> 'Initialization Error: Subscribe to HC Commands' </tt>
**  \event <tt> 'Initialization error: subscribe to HC commands: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Simulator application has
**  failed in its attempt to subscribe to the Heater Control commands
**  during startup initialization.
**
**  This is a fatal error that will cause the Simulator application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_Subscribe.
*/
#define SIM_STARTUP_SUBSCRIB_HC_ERR_EID       18


/** \brief <tt> 'Receive Command Success' </tt>
**  \event <tt> 'Receive command success' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /SIM_ReceiveCmd command.
**
*/
#define SIM_RCV_CMD_EID                      19


/** \brief <tt> 'Receive Command Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Receive command error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /SIM_ReceiveCmd
**  command packet with an invalid length.
*/
#define SIM_RCV_CMD_ERR_EID                   20


/** \brief <tt> 'Initialization Error: Subscribe to SCH Request' </tt>
**  \event <tt> 'Initialization error: subscribe to SCH request: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Simulator application has
**  failed in its attempt to subscribe to the Scheduler request
**  during startup initialization.
**
**  This is a fatal error that will cause the Simulator application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_Subscribe.
*/
#define SIM_STARTUP_SUBSCRIB_SCH_ERR_EID       21

#define SIM_MISBEHAVE_CMD_EID                  22

#define SIM_MISBEHAVE_PKT_ERR_EID              23


#endif /* _sim_events_h_ */

/************************/
/*  End of File Comment */
/************************/
