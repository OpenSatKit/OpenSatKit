###############################################################################
# cFE Time Service tutorial top-level script
# 
# Notes:
#   1. This tutorial is part of the cFE Service training module and implements
#      the TIME exercises.
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


################################################
## TIME01 - Review default time configuration ##
################################################

wait #TIME01 - Click <Go> to review default time configuration
display("CFS_KIT CFE_SCREEN",50,50)
puts "TIME01 - Note packet time = Mission Elapsed Time (MET) + Spacecraft Time Correlation Factor (STCF)"

wait #TIME01 - Click <Go> to clear MET
Osk::flight.cfe_time.send_cmd("SET_CLOCK_MET with SECONDS 0, MICROSECONDS 0")

wait #TIME01 - Click <Go> to set STCF to 1000
Osk::flight.cfe_time.send_cmd("SET_CLOCK_STCF with SECONDS 1000, MICROSECONDS 0")

wait #TIME01 - Click <Go> to continue to next section

##########################################
## TIME02 - Review diagnostic telemetry ##
##########################################

wait #TIME02 - Click <Go> to review diagnostic telemetry
  
spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'CFE_TIME DIAG_TLM_PKT'")

wait #TIME02 - Click <Go> to send diagnostic telemetry packet
Osk::flight.cfe_time.send_cmd("SEND_DIAG")

wait #TIME02 - Click <Go> to continue to next section

####################################################
## TIME03 - Plot incrementing 1Hz time adjustment ##
####################################################

wait #TIME03 - Click <Go> to plot incrementing 1Hz time adjustment

spawn("ruby #{Osk::COSMOS_TLM_GRAPHER} -s -i 'CFE_TIME HK_TLM_PKT STCF_SECONDS'")
Osk::flight.cfe_time.send_cmd("ADD_1HZ_STCF_ADJ with SECONDS 10, SUBSECONDS 0")

wait #TIME03 - Click <Go> to exit

#############
## Cleanup ##
#############

Osk::flight.cfe_time.send_cmd("ADD_1HZ_STCF_ADJ with SECONDS 0, SUBSECONDS 0")
clear("CFS_KIT CFE_SCREEN")

 
