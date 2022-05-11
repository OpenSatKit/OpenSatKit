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

wait #EVS01 - Click <Go> to execute the steps in the comments below
# 1. Write event log to a file
# 2. Transfer log file to the ground
# 3. Display event log using Table Manager

Osk::Ops::send_flt_bin_file_cmd("CFE_EVS", "WRITE_LOG_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_EVS_LOG)
puts "EVS01 - Review event log file displayed in Table Manager"

wait #EVS01 - Click <Go> to execute the steps in the comments below
# 1. Clear event event log
# 2. Send noop commands to generate events. Events are displayed in the cFS terminal window
# 3. Write event log to a file
# 4. Transfer log file to the ground
# 5. Display event log using Table Manager

display("CFS_KIT CFE_SCREEN",50,50)

Osk::flight.cfe_evs.send_cmd("CLEAR_LOG")
wait 1
Osk::flight.cfe_es.send_cmd("NOOP")
wait 1
Osk::flight.cfe_evs.send_cmd("NOOP")
wait 1
Osk::flight.cfe_sb.send_cmd("NOOP")
wait 1
Osk::flight.cfe_tbl.send_cmd("NOOP")
wait 1
Osk::flight.cfe_time.send_cmd("NOOP")
wait 2
Osk::Ops::send_flt_bin_file_cmd("CFE_EVS", "WRITE_LOG_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_EVS_LOG)

wait #EVS01 - Click <Go> to continue to next section


####################################################################
## EVS02 - Review app event registry and current data 'info' file ##
####################################################################

wait #EVS02 - Click <Go> to execute the steps in the comments below
# 1. Write event registry/status to a file
# 2. Transfer event registry/status file to the ground
# 3. Display event registry/status file using Table Manager

Osk::Ops::send_flt_bin_file_cmd("CFE_EVS", "WRITE_APP_INFO_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_EVS_APP_INFO)
puts "EVS02 - Review app event info file displayed in Table Manager"


wait #EVS02 - Click <Go> to execute the steps in the comments below
# 1. Reset SB's event filter counter for no subscription message
# 2. Observe new no subscription messages are output and then stop when the limit of 4 is reached

Osk::flight.cfe_evs.send_cmd("RESET_FILTER_CTR with APP_NAME CFE_SB, EVENT_ID 14");

wait #EVS02 - Click <Go> to continue to next section

########################################
## EVS03 - Enable/Disable event types ##
########################################

wait #EVS03 - Click <Go> to execute the steps in the comments below
# 1. Use EVS to enable KIT_SCH debug events. This will cause a large volume of debug messages to be sent.

Osk::flight.cfe_evs.send_cmd("ENA_APP_EVENT_TYPE with APP_NAME KIT_SCH, BITMASK 1"); # Enable debug messages

wait #EVS03 - Click <Go> to disable KIT_SCH debug events
Osk::flight.cfe_evs.send_cmd("DIS_APP_EVENT_TYPE with APP_NAME KIT_SCH, BITMASK 1"); # Disable debug messages

wait #EVS3 - Click <Go> to exit the script


#############
## Cleanup ##
#############

clear("CFS_KIT CFE_SCREEN")


