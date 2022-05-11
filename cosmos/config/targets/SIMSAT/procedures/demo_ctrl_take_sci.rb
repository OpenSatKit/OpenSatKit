################################################################################
# Demo F42's controller's 'take science' flag  
#
# Notes:
#   1. This demo intentionally has a limited scope in order to minimize
#      complexity and to make it suitable for presentations 
# 
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'fsw_const'
require 'fsw_config_param'

require 'simsat_const'
load_utility('simsat_payload_mgmt') # Don't use 'require' to show lines since this is a demo

###########
## Setup ##
###########

display("SIMSAT SIM_42_SCREEN")
display("PL_MGR PL_MGR_SCREEN")

restart = message_box("Would you like to restart the cFS if it's running?", Osk::MSG_BUTTON_YES, Osk::MSG_BUTTON_NO, false)

if ((restart == Osk::MSG_BUTTON_YES) or (not Osk::System.cfs_running?))
   Osk::System.stop_n_start_cfs('simsat')
end

wait 3                      # Give cFS chance to start

Osk::System.start_42(true)  # True displays I42/F42 sim screen



wait_check("F42 CONTROL_PKT TAKE_SCI == 'TRUE'", 300)

simsat_payload_power_on  # Configure PL_SIM power on and PL_MGR to science mode

status_bar("Observe science collection. <Go> will power off the payload")
wait  # <Go> to continue

#############
## Cleanup ##
#############

simsat_payload_power_off

status_bar("Observe PL_SIM powered off state. <Go> will terminate the demo")
wait  # <Go> to continue

clear("PL_MGR PL_MGR_SCREEN")
clear("SIMSAT SIM_42_SCREEN")
