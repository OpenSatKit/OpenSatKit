###############################################################################
# cFE Event Service tutorial top-level script
# 
# Notes:
#   1. This tutorial is part of the cFE Service training module and implements
#      the EVS exercises.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_ops'


###########
## Setup ##
###########

FLT_BIN_FILE = "#{Osk::FLT_SRV_DIR}/#{Osk::TMP_BIN_FILE}"
GND_BIN_FILE = "#{Osk::GND_SRV_DIR}/#{Osk::TMP_BIN_FILE}"

Osk::System.check_n_start_cfs('cfsat')


###################################
## EVS01 - Review event log file ##
###################################

wait #EVS01 - Click <Go> to write the event log to a file and display it in Table Manager

Osk::Ops::send_flt_bin_file_cmd("CFE_EVS", "WRITE_LOG_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_EVS_LOG)
puts "EVS01 - Review event log file displayed in Table Manager"

wait #EVS01 - Click <Go> to clear event event log, send events, dump & display log

display("CFS_KIT CFE_SCREEN",50,50)

Osk::flight.cfe_evs.send_cmd("CLEAR_LOG")
Osk::flight.cfe_es.send_cmd("NOOP")
Osk::flight.cfe_evs.send_cmd("NOOP")
Osk::flight.cfe_sb.send_cmd("NOOP")
Osk::flight.cfe_tbl.send_cmd("NOOP")
Osk::flight.cfe_time.send_cmd("NOOP")
Osk::Ops::send_flt_bin_file_cmd("CFE_EVS", "WRITE_LOG_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_EVS_LOG)
puts "EVS01 - Review event log file displayed in Table Manager"

wait #EVS01 - Click <Go> to continue to next section

####################################################################
## EVS02 - Review app event registry and current data 'info' file ##
####################################################################

wait #EVS02 - Click <Go> to write event registry/status to a file

Osk::Ops::send_flt_bin_file_cmd("CFE_EVS", "WRITE_APP_INFO_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_EVS_APP_INFO)
puts "EVS02 - Review app event info file displayed in Table Manager"


wait #EVS02 - Click <Go> to reset SB's event filter counter for no subscription message
Osk::flight.cfe_evs.send_cmd("RESET_FILTER_CTR with APP_NAME CFE_SB, EVENT_ID 14");

wait #EVS02 - Click <Go> to continue to next section

########################################
## EVS03 - Enable/Disable event types ##
########################################

wait #EVS03 - Click <Go> to enable EVS debug events
Osk::flight.cfe_evs.send_cmd("ENA_APP_EVENT_TYPE with APP_NAME KIT_SCH, BITMASK 1"); # Enable debug messages

wait #EVS03 - Click <Go> to disable EVS debug events
Osk::flight.cfe_evs.send_cmd("DIS_APP_EVENT_TYPE with APP_NAME KIT_SCH, BITMASK 1"); # Disable debug messages

wait #EVS3 - Click <Go> to continue to exit


#############
## Cleanup ##
#############

clear("CFS_KIT CFE_SCREEN")


