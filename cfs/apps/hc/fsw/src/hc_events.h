/*
** $Id: $
**
** Title: CFS Heater Control (HC) Application Event ID Header File
**
** Purpose: Specification for the CFS Heater Control Event Identifers.
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

#ifndef _hc_events_h_
#define _hc_events_h_


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* HC event message ID's                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \brief <tt> 'Initialization Complete' </tt>
**  \event <tt> 'Initialization complete: version \%d.\%d.\%d.\%d' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message is issued after the Heater Control application has
**  successfully completed startup initialization.
**
**  The version numbers indicate the application major version number,
**  minor version number, revision number and mission revision number.
*/
#define HC_STARTUP_EID                      1


/** \brief <tt> 'Initialization Error: Register for Event Services' </tt>
**  \event <tt> 'Initialization error: register for event services: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Heater Control application has
**  failed in its attempt to register for event services during startup
**  initialization.
**
**  This is a fatal error that will cause the Heater Control application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_EVS_Register.
*/
#define HC_STARTUP_EVENTS_ERR_EID           2


/** \brief <tt> 'Initialization Error: Create SB Input Pipe' </tt>
**  \event <tt> 'Initialization error: create SB input pipe: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Heater Control application has
**  failed in its attempt to create a Software Bus input pipe during startup
**  initialization.
**
**  This is a fatal error that will cause the Heater Control application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_CreatePipe.
*/
#define HC_STARTUP_CREAT_PIPE_ERR_EID       3


/** \brief <tt> 'Initialization Error: Subscribe to HK Request' </tt>
**  \event <tt> 'Initialization error: subscribe to HK request: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Heater Control application has
**  failed in its attempt to subscribe to the HK telemetry request command
**  during startup initialization.
**
**  This is a fatal error that will cause the Heater Control application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_Subscribe.
*/
#define HC_STARTUP_SUBSCRIB_HK_ERR_EID      4


/** \brief <tt> 'Initialization Error: Subscribe to HC Commands' </tt>
**  \event <tt> 'Initialization error: subscribe to HC commands: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Heater Control application has
**  failed in its attempt to subscribe to the HC ground command packet
**  during startup initialization.
**
**  This is a fatal error that will cause the Heater Control application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_Subscribe.
*/
#define HC_STARTUP_SUBSCRIB_GCMD_ERR_EID    5


/** \brief <tt> 'Initialization Error: Register Table' </tt>
**  \event <tt> 'Initialization error: register table: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Heater Control application has
**  failed in its attempt to register its tables during startup initialization.
**
**  This is a fatal error that will cause the Heater Control application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_TBL_Register.
*/
#define HC_STARTUP_TABLE_INIT_ERR_EID      6


/** \brief <tt> 'Main Loop Error: Software Bus Receive' </tt>
**  \event <tt> 'Main Loop Error: SB receive: result = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the Heater Control application has
**  failed in its attempt to read from its Software Bus input pipe while
**  processing the software main loop sequence.
**
**  This is a fatal error that will cause the Heater Control application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_RcvMsg.
*/
#define HC_SB_RECEIVE_ERR_EID               7


/** \brief <tt> 'Application Terminating' </tt>
**  \event <tt> 'Application terminating: result = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the Heater Control application is about
**  to terminate.
**
**  A non-zero result value in the event text is the error code from a
**  fatal error that has occurred.  Fatal errors all have descriptive
**  events.
**
**  If the result value in the event text is zero, then it is likely that
**  the CFE has terminated the HC application, presumably by command.
*/
#define HC_EXIT_ERR_EID                     8


/** \brief <tt> 'Main Loop Error: Invalid Message ID' </tt>
**  \event <tt> 'Main loop error: invalid message ID: mid = 0x\%04X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the Heater Control application has
**  received an unexpected Software Bus packet. There is no obvious
**  explanation of why or how HC could receive such a packet.
**
**  The number in the message text is the unexpected MessageID.
*/
#define HC_MID_ERR_EID                      9


/** \brief <tt> 'Main Loop Error: Invalid Command Code' </tt>
**  \event <tt> 'Main loop error: invalid command code: cc = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is issued when the Heater Control application has
**  received a command packet with an unexpected command code value.
**
**  Mal-formed command packets are generally prevented by the ground
**  system.  Therefore, the source for the problem command is likely
**  to be one of the on-board tables that contain commands.
**
**  The number in the message text is the unexpected command code.
*/
#define HC_CC_ERR_EID                       10


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
#define HC_HK_REQ_ERR_EID                   11


/** \brief <tt> 'No-op Command Success' </tt>
**  \event <tt> 'No-op command: version \%d.\%d.\%d.\%d' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /HC_Noop command.
**
**  The version data includes the application major version, minor version,
**  revision and mission revision numbers.
*/
#define HC_NOOP_CMD_EID                     12


/** \brief <tt> 'No-op Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'No-op error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /HC_Noop
**  command packet with an invalid length.
*/
#define HC_NOOP_PKT_ERR_EID                 13


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
**  /HC_ResetCtrs command.
*/
#define HC_RESET_CMD_EID                    14


/** \brief <tt> 'Reset Counters Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Reset Counters error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /HC_ResetCtrs
**  command packet with an invalid length.
*/
#define HC_RESET_PKT_ERR_EID                15


/** \brief <tt> 'Set Status Command Success' </tt>
**  \event <tt> 'Set Status command success: heater control type \%d enabled' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /HC_SetStatus command to enable a heater control type.
*/
#define HC_SET_STATUS_ENABLED_EID            16


/** \brief <tt> 'Set Status Command Success' </tt>
**  \event <tt> 'Set Status command success: heater control type \%d disabled' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /HC_SetStatus command to disable a heater control type.
*/
#define HC_SET_STATUS_DISABLED_EID            17


/** \brief <tt> 'Set Status Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Set Status error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /HC_SetStatus
**  command packet with an invalid length.
*/
#define HC_SET_STATUS_PKT_ERR_EID             18


/** \brief <tt> 'Set Status Error: Invalid Heater Control Status' </tt>
**  \event <tt> 'Set Status error: invalid heater control status: = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /HC_SetStatus
**  command packet with an invalid heater control status.
*/
#define HC_SET_STATUS_ERR_EID                 19 


/** \brief <tt> 'Receive Packet Command Success' </tt>
**  \event <tt> 'Receive packet command success' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /HC_ReceivePkt command.
**
*/
#define HC_RCV_PKT_CMD_EID                    20


/** \brief <tt> 'Receive Packet Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Receive packet error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /HC_ReceivePkt
**  command packet with an invalid length.
*/
#define HC_RCV_PKT_ERR_EID                    21


/** \brief <tt> 'Table Verify Error' </tt>
**  \event <tt> 'HC Type Table verify error: index = %d, temperature set point error' </tt>
**  \event <tt> 'HC Type Table verify error: index = %d, invalid HC type' </tt>
**  \event <tt> 'HC Type Table verify error: index = %d, invalid enable flag' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when a heater control type table fails the table
**  verification process.  Each HC type table entry has only 5 fields: the enable/disable
**  flag, the heater control type, the function index, the high temperature set
**  point and the low temperature set point. The enable/disable flag must be either
**  enabled or disabled. The heater control type must be a valid heater control type, 
**  as defined in the platform configurations. The function index cannot be validated. 
**  The high temperature set point must be greater than the low temperature set point. 
**
**  If the HC type table loaded at startup fails verification, the HC application will
**  not terminate. However, the HC application will not process commands that request
**  the HC type table data if a HC type table has not been successfully loaded. Thereafter,
**  if an attempt to load a new table fails verification, the HC application will continue
**  to use the previous table. 
**
*/
#define HC_TABLE_VERIFY_ERR_EID               22


/**
**  \brief <tt> 'Table Validation Results' </tt>
**
**  \event <tt> 'HC Type Table verify results: good entries = %d, bad = %d' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event describes the results of the Heater Control Type Table validation
**  function.  The cFE Table Services Manager will call this function autonomously
**  when the default table is loaded at startup and also whenever a table validate
**  command (that targets this table) is processed.
**
**  The event text will indicate the number of table entries that were verified 
**  without error (good) and the number of table entries that had one or more errors
**  (bad). Thus, the sum of good + bad results will equal the total number of table
**  entries.
**
*/
#define HC_TABLE_VERIFY_EID                   23


/** \brief <tt> 'Table Verify Error' </tt>
**  \event <tt> 'HC Components Table verify error: index = %d, invalid number of thermistors' </tt>
**  \event <tt> 'HC Components Table verify error: index = %d, invalid number of heaters' </tt>
**  \event <tt> 'HC Components Table verify error: index = %d, invalid HC type' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated when a heater control component table fails the table
**  verification process.  Each component table entry has only 5 fields: the heater control
**  type, the number of heaters, the number of thermistors, an array of heaters and an
**  array of thermistors. The heater control type must be a valid heater control type as
**  defined in the platform configurations. The number of heaters must be greater than 
**  zero and less than or equal to the maximum number of heaters. The number
**  of thermistors must be greater than or zero and less than or equal to the
**  maximum number of thermistors. Validation for the arrays of heaters and thermistors
**  has not been implemented yet. 
**
**  If the heater control component table loaded at startup fails verification, the HC
**  application will not terminate. However, the HC application will not process
**  commands that request the heater control component data if a heater control
**  component table has not been successfully loaded. Thereafter, if an attempt to
**  load a new table fails verification, the HC application will continue to use the
**  previous table. 
**
*/
#define HC_COM_TABLE_VERIFY_ERR_EID           24


/**
**  \brief <tt> 'Table Validation Results' </tt>
**
**  \event <tt> 'HC Component Table verify results: good entries = %d, bad = %d' </tt>
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event describes the results of the Heater Control Component Table validation
**  function.  The cFE Table Services Manager will call this function autonomously
**  when the default table is loaded at startup and also whenever a table validate
**  command (that targets this table) is processed.
**
**  The event text will indicate the number of table entries that were verified 
**  without error (good) and the number of table entries that had one or more errors
**  (bad). Thus, the sum of good + bad results will equal the total number of table
**  entries.
**
*/
#define HC_COM_TABLE_VERIFY_EID               25


/** \brief <tt> 'Custom Function Call Success' </tt>
**  \event <tt> 'Custom function call success' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  custom function call.
**
*/
#define HC_CFCALL_CMD_EID                     26


/** \brief <tt> 'Custom Function Call Error: Invalid Index' </tt>
**  \event <tt> 'Invalid HC_CustomFunction call: index = %d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of an invalid
**  function index.
*/
#define HC_CFCALL_ERR_EID                     27


/** \brief <tt> 'Initialization Error: Subscribe to SIM Data' </tt>
**  \event <tt> 'Initialization error: subscribe to SIM data: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Heater Control application has
**  failed in its attempt to subscribe to the data packets from the SIM app
**  during startup initialization.
**
**  This is a fatal error that will cause the Heater Control application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_Subscribe.
*/
#define HC_STARTUP_SUBSCRIB_SIM_ERR_EID       28


/** \brief <tt> 'Initialization Error: Subscribe to SCH wakeup Request' </tt>
**  \event <tt> 'Initialization error: subscribe to SCH wakeup request: result = 0x\%08X' </tt>
**
**  \par Type: Error
**
**  \par Cause
**
**  This event message is issued when the Heater Control application has
**  failed in its attempt to subscribe to the SCH wakeup request
**  during startup initialization.
**
**  This is a fatal error that will cause the Heater Control application
**  to terminate.
**
**  The result number in the message text is the error code returned
**  from the the call to the API function #CFE_SB_Subscribe.
*/
#define HC_STARTUP_SUBSCRIB_WAKEUP_ERR_EID     29


/** \brief <tt> 'Wakeup Request Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Wakeup Request error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a scheduler wakeup
**  command packet with an invalid length.
*/
#define HC_WAKEUP_ERR_EID                      30


/** \brief <tt> 'Send Heater Control Command Success' </tt>
**  \event <tt> 'Send heater control command: success' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause
**
**  This event message signals the successful completion of a
**  /HC_SendCtrl command.
**
*/
#define HC_SEND_CTRL_CMD_EID                   31


/** \brief <tt> 'Send Heater Control Error: Invalid Command Packet Length' </tt>
**  \event <tt> 'Send heater control error: invalid command packet length: expected = \%d, actual = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause
**
**  This event message is generated upon receipt of a /HC_SendCtrl
**  command packet with an invalid length.
*/
#define HC_SEND_CTRL_ERR_EID                   32


#endif /* _hc_events_h_ */

/************************/
/*  End of File Comment */
/************************/
