###############################################################################
# cFE Software Bus (SB) tutorial top-level script
# 
# Notes:
#   1. This tutorial is part of the cFE Service training module and implements
#      the SB exercises.
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

Osk::System.check_n_start_cfs('cfsat')


###################################
## SB01 - Review table registry ##
###################################

wait #SB01 - Click <Go> to table registry log to a file and display it in Table Manager

Osk::Ops::send_flt_bin_file_cmd("CFE_SB", "WRITE_PIPE_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_SB_PIPE)
puts "SB01 - Review pipe information file displayed in Table Manager"

wait #SB01 - Click <Go> to continue to next section

#############
## SB02 -  ##
#############

wait #SB02 - Click <Go> to request the SB statistics packet

spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'CFE_SB STATS_TLM_PKT'")
wait 2
Osk::flight.cfe_sb.send_cmd("SEND_STATS")

puts "SB02 - Review the SB statistics packet"
wait #SB02 - Click <Go> to continue to exit

#############
## Cleanup ##
#############

clear("CFS_KIT CFE_SCREEN")
