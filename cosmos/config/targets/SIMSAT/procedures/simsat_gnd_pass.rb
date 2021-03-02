###############################################################################
# SimpleSat (SimSat) Mission Sim Pass
#
# Perform operational functions during a simulated ground contact.  This script
# is designed to make you think about what types of functions need to be
# performed. It is not trying to be prescriptive in how to perform them. Each
# mission has its own unique ops concept and functional requirements. These
# will drive strategies data collection priorities, level of automation, 
# assesment vs corrective action during a contact (health & safety), etc. 
#
# Notes:
#   1. This script is part of the cFS systems engineering training module
#   2. OSK uses TFTP and not CFDP for file transfers which impacts how
#      files are transferred and managed onboard.
#
# Global Script Variables:
#   simsat_ops_enable - Boolean indicating whether ops example is active
#   simsat_ops_status - Text string displayed on ops example screen
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_ops'

#
# Wait for contact to start
#

#TODO - Simulate a contact

#
# Assess health of spacecraft
#
# 1. Transfer event log file to the ground. Analyze after quick automated assessment.
# 2. Perform automated quick check and alert operator of any non-nominal status
#

simsat_ops_status = "Ground Pass: Downlink event message log"
Osk::Ops::send_flt_bin_file_cmd("CFE_EVS", "WRITE_LOG_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_EVS_LOG)

if (tlm("CFE_EVS HK_TLM_PKT LOG_FULL_FLAG") == 1) 
   wait # TODO - close and downlink DS event log
end


#
# Do quick real-time tlm check and alert user of issues 
#

simsat_ops_status = "Ground Pass: Real-time telemetry quick health assessment"

message_box("Instrument not in READY state",false)  unless (tlm("ISIM HK_TLM_PKT INSTR_PWR_STATE") == "READY")
message_box("Instrument fault present",false)       unless (tlm("ISIM HK_TLM_PKT FAULT_PRESENT") == "FALSE")

message_box("Housekeeping missed auxiliary packets. This unexpectedly occurs and needs to be debugged. This is not part of the example sim.",false) unless (tlm("HK HK_TLM_PKT MISSING_DATA_CNT") == 0)

message_box("An error occurred with a stored command relatve time sequence",false) unless (tlm("SC HK_TLM_PKT RTS_ACTIVE_ERR_CTR") == 0)

simsat_ops_status = "Review event log file displayed in Table Manager"
status_bar("Review event log file displayed in Table Manager")
wait  # <Go> to continue

# Establish known state to simplify error checks on next pass

Osk::flight.cfe_evs.send_cmd("CLEAR_LOG")
Osk::flight.send_cmd("HK", "#{Osk::CMD_STR_RESET}")
Osk::flight.send_cmd("SC", "#{Osk::CMD_STR_RESET}")


#
# Manage Onboard Files
#

simsat_recorder_playback("TFTP")
 
#
# Prepare for ops while out of contact
#
# - Optionally load next ATS
#

