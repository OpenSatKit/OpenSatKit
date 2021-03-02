/************************************************************************
** File: md_events.h 
**
** NASA Docket No. GSC-18,450-1, identified as “Core Flight Software System (CFS)
** Memory Dwell Application Version 2.3.3” 
**
** Copyright © 2019 United States Government as represented by the Administrator of
** the National Aeronautics and Space Administration. All Rights Reserved. 
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
** http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License. 
*
** Purpose:
**  The CFS Memory Dwell (MD) Application event id header file
**
** Notes:
**
**
*************************************************************************/
#ifndef _md_events_h_
#define _md_events_h_

/*
** MD event message ID's...
*/
/** \brief <tt> 'MD Initialized.  Version %d.%d.%d.%d' </tt>
**  \event <tt> 'MD Initialized.  Version %d.%d.%d.%d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  Issued upon successful completion of task initialization.
**
**/
#define MD_INIT_INF_EID    1    

/** \brief <tt> 'SB Pipe Read Error, App will exit. Pipe Return Status = 0x%08X' </tt>
**  \event <tt> 'SB Pipe Read Error, App will exit. Pipe Return Status = 0x%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event is issued following error return from #CFE_SB_RcvMsg call.
**/
#define MD_PIPE_ERR_EID    2

/** \brief <tt> 'Recovered Dwell Table #%d is valid and has been copied to the MD App' </tt>
**  \event <tt> 'Recovered Dwell Table #%d is valid and has been copied to the MD App' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  Issued upon successful recovery of a Dwell Table.
**
**/
#define MD_RECOVERED_TBL_VALID_INF_EID 3

/** \brief <tt> 'MD App will reinitialize Dwell Table #%d because recovered table is not valid' </tt>
**  \event <tt> 'MD App will reinitialize Dwell Table #%d because recovered table is not valid' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  Issued when a Dwell Table is recovered and found to be invalid.
**/
#define MD_RECOVERED_TBL_NOT_VALID_ERR_EID 4

/** \brief <tt> 'Dwell Table(s) are too large to register: %d > %d bytes, %d > %d entries' </tt>
**  \event <tt> 'Dwell Table(s) are too large to register: %d > %d bytes, %d > %d entries' </tt> 
**
**  \par Type: CRITICAL
**
**  \par Cause:
**
**  Issued when a #CFE_TBL_ERR_INVALID_SIZE error message is received from #CFE_TBL_Register call.
**  Load structure can be reduced by reducing #MD_DWELL_TABLE_SIZE, number of entries per Dwell Table.
**/
#define MD_DWELL_TBL_TOO_LARGE_CRIT_EID 5

/** \brief <tt> 'CFE_TBL_Register error 0x%08X received for tbl#%d' </tt>
**  \event <tt> 'CFE_TBL_Register error 0x%08X received for tbl#%d' </tt> 
**
**  \par Type: CRITICAL
**
**  \par Cause:
**
**  Issued when an error message, other than #CFE_TBL_ERR_INVALID_SIZE, is received from #CFE_TBL_Register call.
**/
#define MD_TBL_REGISTER_CRIT_EID 6

/** \brief <tt> 'Dwell Tables Recovered: %d, Dwell Tables Initialized: %d' </tt>
**  \event <tt> 'Dwell Tables Recovered: %d, Dwell Tables Initialized: %d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  Issued at the end of Table Initialization, specifying how many tables were recovered and how many initialized.
**
**/
#define MD_TBL_INIT_INF_EID 7

/** \brief <tt> 'No-op command, Version %d.%d.%d.%d' </tt>
**  \event <tt> 'No-op command, Version %d.%d.%d.%d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  Issued upon receipt of a Memory Dwell no-op command.
**/
#define MD_NOOP_INF_EID	       10

/** \brief <tt> 'Reset Counters Cmd Received' </tt>
**  \event <tt> 'Reset Counters Cmd Received' </tt> 
**
**  \par Type: DEBUG
**
**  \par Cause:
**
**  Issued upon receipt of a Memory Dwell Reset Counters command.
**/
#define MD_RESET_CNTRS_DBG_EID 11


/** \brief <tt> 'Start Dwell Table command processed successfully for table mask 0x%04X' </tt>
**  \event <tt> 'Start Dwell Table command processed successfully for table mask 0x%04X' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  Issued upon receipt of a Memory Dwell Start command.
**  Upon receipt of this command, the specified tables are started for processing.
**/
#define MD_START_DWELL_INF_EID 12

/** \brief <tt> 'Stop Dwell Table command processed successfully for table mask 0x%04X' </tt>
**  \event <tt> 'Stop Dwell Table command processed successfully for table mask 0x%04X' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  Issued upon receipt of a Memory Dwell Start command.
**  Upon receipt of this command, the specified tables are stopped.
**/
#define MD_STOP_DWELL_INF_EID 13

/** \brief <tt> '%s command rejected because no tables were specified in table mask (0x%04X)' </tt>
**  \event <tt> '%s command rejected because no tables were specified in table mask (0x%04X)' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  None of the valid Table Ids (1..#MD_NUM_DWELL_TABLES) are contained in
**  the table mask argument for the Start Dwell or Stop Dwell command.
**/
#define MD_EMPTY_TBLMASK_ERR_EID 14


/** \brief <tt> 'Msg with Invalid message ID Rcvd -- ID = 0x%04X' </tt>
**  \event <tt> 'Msg with Invalid message ID Rcvd -- ID = 0x%04X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event is issued if the Memory Dwell task receives a message
**  with an unrecognized Message ID.
**/
#define MD_MID_ERR_EID 15

/** \brief <tt> 'Command Code %d not found in MD_CmdHandlerTbl structure' </tt>
**  \event <tt> 'Command Code %d not found in MD_CmdHandlerTbl structure' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event is issued when a command for the Memory Dwell task is 
**  received with a function code which is not listed in the internal 
**  MD_CmdHandlerTbl structure, which is used to associate an expected
**  length for the command.
**/
#define MD_CC_NOT_IN_TBL_ERR_EID 16

/** \brief <tt> 'Command Code %d not found in command processing loop' </tt>
**  \event <tt> 'Command Code %d not found in command processing loop' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event is issued when a command for the Memory Dwell task is 
**  received with a function code which is not included in the task's
**  command code processing loop.
**/
#define MD_CC_NOT_IN_LOOP_ERR_EID 17

/** \brief <tt> 'Received unexpected error 0x%08X from CFE_TBL_GetStatus for tbl #%d' </tt>
**  \event <tt> 'Received unexpected error 0x%08X from CFE_TBL_GetStatus for tbl #%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event is issued on receipt of an unexpected error message from
**  CFE_TBL_GetStatus.  Normal processing continues; no special action is taken.  
**/
#define MD_TBL_STATUS_ERR_EID 20

/** \brief <tt> 'Cmd Msg with Bad length Rcvd: ID = 0x%04X, CC = %d, Exp Len = %d, Len = %d' </tt>
**  \event <tt> 'Cmd Msg with Bad length Rcvd: ID = 0x%04X, CC = %d, Exp Len = %d, Len = %d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event is issued when the Memory Dwell task receives a command 
**  which has a length that is inconsistent with the expected length
**  for its command code.  
**/
#define MD_CMD_LEN_ERR_EID 21

/** \brief <tt> 'Msg with Bad length Rcvd: ID = 0x%04X, Exp Len = %d, Len = %d' </tt>
**  \event <tt> 'Msg with Bad length Rcvd: ID = 0x%04X, Exp Len = %d, Len = %d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event is issued when the Memory Dwell task receives a message 
**  which has a length that is inconsistent with the expected length
**  for its message id.  
**/
#define MD_MSG_LEN_ERR_EID 22

/** \brief <tt> 'Successful Jam to Dwell Tbl#%d Entry #%d' </tt>
**  \event <tt> 'Successful Jam to Dwell Tbl#%d Entry #%d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event is issued for a successful jam operation.
**/
#define MD_JAM_DWELL_INF_EID 30

/** \brief <tt> 'Successful Jam of a Null Dwell Entry to Dwell Tbl#%d Entry #%d' </tt>
**  \event <tt> 'Successful Jam of a Null Dwell Entry to Dwell Tbl#%d Entry #%d' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event is issued for a jam operation in which a null dwell entry is specified.
**  A null entry is specified when the input dwell length is zero.
**  All dwell fields (address, length, and delay) will be set to zero in this case.
**/
#define MD_JAM_NULL_DWELL_INF_EID 31

/** \brief <tt> 'Jam Cmd rejected due to invalid Tbl Id arg = %d (Expect 1.. %d)' </tt>
**  \event <tt> 'Jam Cmd rejected due to invalid Tbl Id arg = %d (Expect 1.. %d)' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event is issued when a Jam Dwell Command is received 
**  with an invalid value for the table id argument.  
**  Values in the range 1..#MD_NUM_DWELL_TABLES are expected.
**/
#define MD_INVALID_JAM_TABLE_ERR_EID 32


/** \brief <tt> 'Jam Cmd rejected due to invalid Entry Id arg = %d (Expect 1.. %d)' </tt>
**  \event <tt> 'Jam Cmd rejected due to invalid Entry Id arg = %d (Expect 1.. %d)' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event is issued when a Jam Dwell Command is received 
**  with an invalid value for the entry id argument.  
**  Values in the range 1..#MD_DWELL_TABLE_SIZE are expected.
**/
#define MD_INVALID_ENTRY_ARG_ERR_EID 33

/** \brief <tt> 'Jam Cmd rejected due to invalid Field Length arg = %d (Expect 0,1,2,or 4)' </tt>
**  \event <tt> 'Jam Cmd rejected due to invalid Field Length arg = %d (Expect 0,1,2,or 4)' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event is issued when a Jam Dwell Command is received 
**  with an invalid value for the field length argument.  
**/
#define MD_INVALID_LEN_ARG_ERR_EID 34

/** \brief <tt> 'Jam Cmd rejected because symbolic address '%s' couldn't be resolved' </tt>
**  \event <tt> 'Jam Cmd rejected because symbolic address '%s' couldn't be resolved' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event is issued when symbolic address passed in Jam command
**  couldn't be resolved by use of the on-board Symbol Table.
**/
#define MD_CANT_RESOLVE_JAM_ADDR_ERR_EID 35

/** \brief <tt> 'Jam Cmd rejected because address 0x%08X is not in a valid range' </tt>
**  \event <tt> 'Jam Cmd rejected because address 0x%08X is not in a valid range' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  The resolved address (numerical value for symbol + offset) and
**  field length specified by the Jam command were found to specify a dwell
**  be outside valid ranges.
**/
#define MD_INVALID_JAM_ADDR_ERR_EID 36


/** \brief <tt> 'Jam Cmd rejected because address 0x%08X is not 32-bit aligned' </tt>
**  \event <tt> 'Jam Cmd rejected because address 0x%08X is not 32-bit aligned' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  The Jam command specified a 4-byte read, and the resolved address 
**  (numerical value for symbol + offset) is not on a 4-byte boundary.
**/
#define MD_JAM_ADDR_NOT_32BIT_ERR_EID 37

/** \brief <tt> 'Jam Cmd rejected because address 0x%08X is not 16-bit aligned' </tt>
**  \event <tt> 'Jam Cmd rejected because address 0x%08X is not 16-bit aligned' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  The Jam command specified a 2-byte read, and the resolved address 
**  (numerical value for symbol + offset) is not on a 2-byte boundary.
**/
#define MD_JAM_ADDR_NOT_16BIT_ERR_EID 38


/** \brief <tt> 'Didn't update MD tbl #%d due to unexpected CFE_TBL_GetAddress return: 0x%08X' </tt>
**  \event <tt> 'Didn't update MD tbl #%d due to unexpected CFE_TBL_GetAddress return: 0x%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event is issued after the following sequence occurs:
**  1) #CFE_TBL_GetStatus returned #CFE_TBL_INFO_UPDATE_PENDING
**  2) #CFE_TBL_Update returned #CFE_SUCCESS, a call is made to
**  3) #CFE_TBL_GetAddress returned something other than #CFE_TBL_INFO_UPDATED.
**
**  When this happens, the newly loaded table contents are _not_ copied
**  to MD task structures.
**
**/
#define MD_NO_TBL_COPY_ERR_EID 39


/** \brief <tt> 'Dwell Table is enabled but no processing will occur for table being loaded (rate is zero)' </tt>
**  \event <tt> 'Dwell Table is enabled but no processing will occur for table being loaded (rate is zero)' </tt> 
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  1) The calculated rate, the total of delays for all active entries, 
**  equals zero, and
**  2) The table is enabled
**
**  If this load was initiated via ground command, the Table Id will be known
**  from the command.  If this load was recovered after a reset, a subsequent
**  event message will identify the Table Id.
**/
#define MD_ZERO_RATE_TBL_INF_EID 40

/** \brief <tt> 'Dwell Table rejected because address (sym='%s'/offset=0x%08X) in entry #%d couldn't be resolved' </tt>
**  \event <tt> 'Dwell Table rejected because address (sym='%s'/offset=0x%08X) in entry #%d couldn't be resolved' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  The specified symbol wasn't found in the system symbol table.
**  This could be either because there is no symbol table, or because
**  the symbol isn't present in an existing symbol table.
**
**  If this load was initiated via ground command, the Table Id will be known
**  from the command.  If this load was recovered after a reset, a subsequent
**  event message will identify the Table Id.
**/
#define MD_RESOLVE_ERR_EID 41

/** \brief <tt> 'Dwell Table rejected because address (sym='%s'/offset=0x%08X) in entry #%d was out of range' </tt>
**  \event <tt> 'Dwell Table rejected because address (sym='%s'/offset=0x%08X) in entry #%d was out of range' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  The specified address was not in allowable memory ranges.
**
**  If this load was initiated via ground command, the Table Id will be known
**  from the command.  If this load was recovered after a reset, a subsequent
**  event message will identify the Table Id.
**/
#define MD_RANGE_ERR_EID 42

/** \brief <tt> 'Dwell Table rejected because length (%d) in entry #%d was invalid' </tt>
**  \event <tt> 'Dwell Table rejected because length (%d) in entry #%d was invalid' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  The dwell table contains an invalid value for a dwell length.
**
**  If this load was initiated via ground command, the Table Id will be known
**  from the command.  If this load was recovered after a reset, a subsequent
**  event message will identify the Table Id.
**/
#define MD_TBL_HAS_LEN_ERR_EID 43

/** \brief <tt> 'Dwell Table rejected because value of enable flag (%d) is invalid' </tt>
**  \event <tt> 'Dwell Table rejected because value of enable flag (%d) is invalid' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  The dwell table's enable value was neither zero nor one.
**
**  If this load was initiated via ground command, the Table Id will be known
**  from the command.  If this load was recovered after a reset, a subsequent
**  event message will identify the Table Id.
**/
#define MD_TBL_ENA_FLAG_EID 44

/** \brief <tt> 'Dwell Table rejected because address (sym='%s'/offset=0x08X) in entry #%d is not properly aligned for a %d-byte dwell' </tt>
**  \event <tt> 'Dwell Table rejected because address (sym='%s'/offset=0x08X) in entry #%d is not properly aligned for a %d-byte dwell' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  Either a 4-byte dwell was specified and address is not 4-byte aligned, or
**  a 2-byte dwell was specified and address is not 2-byte aligned.
**  
**  If this load was initiated via ground command, the Table Id will be known
**  from the command.  If this load was recovered after a reset, a subsequent
**  event message will identify the Table Id.
**/
#define MD_TBL_ALIGN_ERR_EID 45

/** \brief <tt> 'Successfully set signature for Dwell Tbl#%d to '%s'' </tt>
**  \event <tt> 'Successfully set signature for Dwell Tbl#%d to '%s'' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  A 'Set Signature' command was received and processed nominally to
**  associate a signature with the specified dwell table.  All dwell packets
**  derived from that dwell table will include the specified signature string.
**
**/
#define MD_SET_SIGNATURE_INF_EID  46


/** \brief <tt> 'Set Signature cmd rejected due to invalid Tbl Id arg = %d (Expect 1.. %d)' </tt>
**  \event <tt> 'Set Signature cmd rejected due to invalid Tbl Id arg = %d (Expect 1.. %d)' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event is issued when a Set Signature Command is received 
**  with an invalid value for the table id argument.  
**  Values in the range 1..#MD_NUM_DWELL_TABLES are expected.
**/
#define MD_INVALID_SIGNATURE_TABLE_ERR_EID 47


/** \brief <tt> 'Set Signature cmd rejected due to invalid Signature length' </tt>
**  \event <tt> 'Set Signature cmd rejected due to invalid Signature length' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This error event is issued when a Set Signature Command is received 
**  with a string not containing null termination within the allowable length.
**/
#define MD_INVALID_SIGNATURE_LENGTH_ERR_EID 49


/** \brief <tt> 'Dwell Table rejected because Signature length was invalid' </tt>
**  \event <tt> 'Dwell Table rejected because Signature length was invalid' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  The dwell table contains an invalid Signature (not null terminated).
**
**  If this load was initiated via ground command, the Table Id will be known
**  from the command.  If this load was recovered after a reset, a subsequent
**  event message will identify the Table Id.
**/
#define MD_TBL_SIG_LEN_ERR_EID 50


/** \brief <tt> 'Dwell Table %d is enabled with a delay of zero so no processing will occur' </tt>
**  \event <tt> 'Dwell Table %d is enabled with a delay of zero so no processing will occur' </tt> 
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  1) The calculated rate, the total of delays for all active entries, 
**  current equals zero
**  2) The table is currently enabled
**
**  If the command either changes the delay values in the table (such that the total delay is 0)
**  while the table is enabled, or if the table is enabled while the total delay value is 0, this
**  event will be sent.
**/
#define MD_ZERO_RATE_CMD_INF_EID 51


/** \brief <tt> 'MD Dwell Tbl verify results: good = \%d, bad = \%d, unused = \%d' </tt>
**  \event <tt> 'MD Dwell Tbl verify results: good = \%d, bad = \%d, unused = \%d' </tt> 
**  
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is issued when a table validation has been 
**  completed for memory dwell table load 
**
**  The \c good field is number of entries that passed, the \c bad field
**  is number of entries that failed, the \c unused field is the 
**  number of entries that weren't checked because they were 
**  marked unused.
*/
#define MD_DWELL_TBL_INF_EID 52

/** \brief <tt> 'MD_UpdateTableEnabledField, TableIndex \%d: CFE_TBL_GetAddress Returned 0x\%08x' </tt>
**  \event <tt> 'MD_UpdateTableEnabledField, TableIndex \%d: CFE_TBL_GetAddress Returned 0x\%08x' </tt>
**
**  \par Type: ERROR
**  
**  \par Cause:
**
**  This event message is issued when the table address cannot be acquired in
**  the #MD_UpdateTableEnabledField function.  This event is issued when the
**  #CFE_TBL_GetAddress function returns a value other than CFE_SUCCESS or 
**  CFE_TBL_INFO_UPDATED.
**
**  The \c TableIndex field is the index of the dwell table for which the
**  failure occurred
**  The \c Returned field is the value returned from #CFE_Tbl_GetAddress
*/
#define MD_UPDATE_TBL_EN_ERR_EID  53

/** \brief <tt> 'MD_UpdateTableDwellEntry, TableIndex %d: CFE_TBL_GetAddress Returned 0x\%08x' </tt>
**  \event <tt> 'MD_UpdateTableDwellEntry, TableIndex %d: CFE_TBL_GetAddress Returned 0x\%08x' </tt>
**
**  \par Type: ERROR
**  
**  \par Cause:
**
**  This event message is issued when the table address cannot be acquired in
**  the #MD_UpdateTableDwellEntry function.  This event is issued when the
**  #CFE_TBL_GetAddress function returns a value other than CFE_SUCCESS or 
**  CFE_TBL_INFO_UPDATED.
**
**  The \c TableIndex field is the index of the dwell table for which the
**  failure occurred
**  The \c Returned field is the value returned from #CFE_Tbl_GetAddress
*/
#define MD_UPDATE_TBL_DWELL_ERR_EID  54


/** \brief <tt> 'MD_UpdateTableSignature, TableIndex %d: CFE_TBL_GetAddress Returned 0x\%08x' </tt>
**  \event <tt> 'MD_UpdateTableSignature, TableIndex %d: CFE_TBL_GetAddress Returned 0x\%08x' </tt>
**
**  \par Type: ERROR
**  
**  \par Cause:
**
**  This event message is issued when the table address cannot be acquired in
**  the #MD_UpdateTableSignature function.  This event is issued when the
**  #CFE_TBL_GetAddress function returns a value other than CFE_SUCCESS or 
**  CFE_TBL_INFO_UPDATED.
**
**  The \c TableIndex field is the index of the dwell table for which the
**  failure occurred
**  The \c Returned field is the value returned from #CFE_Tbl_GetAddress
*/
#define MD_UPDATE_TBL_SIG_ERR_EID  55

/** \brief <tt> 'Start Dwell Table for mask 0x\%04X failed for \%d of \%d tables' </tt>
**  \event <tt> 'Start Dwell Table for mask 0x\%04X failed for \%d of \%d tables' </tt>
**
**  \par Type: ERROR
**  
**  \par Cause:
**
**  This event message is issued when the table address cannot be acquired for
**  one of the tables being started with the #MD_ProcessStartCmd.  In addition
**  to this summary message, a #MD_UPDATE_TBL_EN_ERR_EID event is issed for each
**  failure.
**
**  The \c mask  field is the mask of tables to start specified in the command.
**  The \c integer fields "\%d or \%d" states the total number of tables that
**  could not be started.
*/
#define MD_START_DWELL_ERR_EID 56

/** \brief <tt> 'Stop Dwell Table for mask 0x\%04X failed for \%d of \%d tables' </tt>
**  \event <tt> 'Stop Dwell Table for mask 0x\%04X failed for \%d of \%d tables' </tt>
**
**  \par Type: ERROR
**  
**  \par Cause:
**
**  This event message is issued when the table address cannot be acquired for
**  one of the tables being started with the #MD_ProcessStopCmd.  In addition
**  to this summary message, a #MD_UPDATE_TBL_EN_ERR_EID event is issed for each
**  failure.
**
**  The \c mask  field is the mask of tables to start specified in the command.
**  The \c integer fields "\%d or \%d" states the total number of tables that
**  could not be started.
*/
#define MD_STOP_DWELL_ERR_EID 57

/** \brief <tt> 'Failed to set signature for Dwell Tbl#\%d. Update returned 0x\%08X' </tt>
**  \event <tt> 'Failed to set signature for Dwell Tbl#\%d. Update returned 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued when the table address cannot be acquired
**  for the table specified in the #MD_ProcessSignatureCmd. 
**
**  The \c Tbl field is the index of the dwell table for which the error occurred.
**  The \c returned field is the value returned from #MD_UpdateTableSignature.
*/
#define MD_SET_SIGNATURE_ERR_EID 58

/** \brief <tt> 'Failed Jam to Dwell Tbl#%d Entry #%d' </tt>
**  \event <tt> 'Failed Jam to Dwell Tbl#%d Entry #%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event is issued for a failed jam operation.
**
**  The \c Tbl field is the index of the table for which the failure occurred.
**  The \c Entry field is the index of the entry for which the failure occurred.
**/
#define MD_JAM_DWELL_ERR_EID 59

/** \brief <tt> 'Failed Jam of a Null Dwell Entry to Dwell Tbl#%d Entry #%d' </tt>
**  \event <tt> 'Failed Jam of a Null Dwell Entry to Dwell Tbl#%d Entry #%d' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event is issued for a failed jam operation in which a null dwell entry is 
**  specified.  A null entry is specified when the input dwell length is zero.
**  All dwell fields (address, length, and delay) will be set to zero in this case.
** 
**  The \c Tbl field is the index of the table for which the failure occurred.
**  The \c Entry field is the index of the entry for which the failure occurred.
**/
#define MD_JAM_NULL_DWELL_ERR_EID 60

/** \brief <tt> 'TableName could not be made. Err=0x\%08X, Idx=\%d" </tt>
**  \event <tt> 'TableName could not be made. Err=0x\%08X, Idx=\%d" </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued if the snprintf call used to create the 
**  TableName in #MD_InitTableServices function fails.
**
**  The \c Err field in the event text is the value returned from snprintf.
**  The \c Idx field in the event text is the TblIndex at which the error
**  occurred.
*/
#define MD_INIT_TBL_NAME_ERR_EID 61

/** \brief <tt> 'TblFileName could not be made. Err=0x\%08X, Idx=\%d" </tt>
**  \event <tt> 'TblFileName could not be made. Err=0x\%08X, Idx=\%d" </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is issued if the snprintf call used to create the 
**  TblFileName in #MD_InitTableServices function fails.
**
**  The \c Err field in the event text is the value returned from snprintf.
**  The \c Idx field in the event text is the TblIndex at which the error
**  occurred.
*/

#define MD_INIT_TBL_FILENAME_ERR_EID 62

/** \brief <tt> 'Dwell Table rejected because of null table pointer' </tt>
**  \event <tt> 'Dwell Table rejected because of null table pointer' </tt>
**
**  \par Type: ERROR
** 
**  \par Cause:
**
**  This event message is issued when the table pointer passed to
**  #MD_TableValidationFunc is null.
*/
#define MD_TBL_VAL_NULL_PTR_ERR_EID 55


/** \brief <tt> 'Dwell Table failed to read entry %d in table %d' </tt>
**  \event <tt> 'Dwell Table failed to read entry %d in table %d' </tt>
**
**  \par Type: ERROR
** 
**  \par Cause:
**
**  This event message is issued when the PSP returns an error when attempting
**  to read the memory address of an entry
*/
#define MD_DWELL_LOOP_GET_DWELL_DATA_ERR_EID 70


/** \brief <tt> 'Failed to create pipe.  RC = %d' </tt>
**  \event <tt> 'Failed to create pipe.  RC = %d' </tt>
**
**  \par Type: ERROR
** 
**  \par Cause:
**
**  This event message is issued when MD cannot create the software bus pipe.
**  The RC field is the return code from #CFE_SB_CreatePipe.
*/
#define MD_CREATE_PIPE_ERR_EID               71


/** \brief <tt> 'Failed to subscribe to HK requests.  RC = %d' </tt>
**  \event <tt> 'Failed to subscribe to HK requests.  RC = %d' </tt>
**
**  \par Type: ERROR
** 
**  \par Cause:
**
**  This event message is issued when MD cannot subscribe to housekeeping
**  requests.
**  The RC field is the return code from #CFE_SB_Subscribe
*/
#define MD_SUB_HK_ERR_EID                   72


/** \brief <tt> 'Failed to subscribe to commands.  RC = %d' </tt>
**  \event <tt> 'Failed to subscribe to commands.  RC = %d' </tt>
**
**  \par Type: ERROR
** 
**  \par Cause:
**
**  This event message is issued when MD cannot subscribe to commands.
**  The RC field is the return code from #CFE_SB_Subscribe
*/
#define MD_SUB_CMD_ERR_EID                  73


/** \brief <tt> 'Failed to subscribe to wakeup messages.  RC = %d' </tt>
**  \event <tt> 'Failed to subscribe to wakeup messages.  RC = %d' </tt>
**
**  \par Type: ERROR
** 
**  \par Cause:
**
**  This event message is issued when MD cannot subscribe to wakeup 
**  messages.
**  The RC field is the return code from #CFE_SB_Subscribe
*/
#define MD_SUB_WAKEUP_ERR_EID                74


#endif
/************************/
/*  End of File Comment */
/************************/
