###############################################################################
# Simple Sat (SimSat) Mission Sim Prep
#
# This script configures the apps used for an example mission sim. Some of the
# configurations may redundant with OSK's default configuration but they done
# here to ensure the proper sim configuration. Tables required by the sim are
# part of OSK's default tables so they do not need to be loaded. 
#
# Notes:
#   1. This script is part of the cFS systems engineering training module
#   2. The iSim app uses more event messages than a typical flight app. The
#      events are used for illustrative purposes.  
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_ops'

#
# House Keeping Combo Packets
#

Osk::flight.kit_sch.send_cmd("CFG_SCH_ENTRY with SLOT 2, ACTIVITY 6, CONFIG 1") # Enable HK Combo Pkt #1 
wait 2

Osk::flight.kit_sch.send_cmd("CFG_SCH_ENTRY with SLOT 2, ACTIVITY 7, CONFIG 1") # Enable HK Combo Pkt #2
wait  # Review configuration
 
#
# Data Storage
#

Osk::flight.ds.send_cmd("SET_APP_STATE with APP_STATE 1") # Enable DS to start creating packet files
wait 2

Osk::flight.ds.send_cmd("SET_FILE_STATE with FILE_TBL_IDX 0, FILE_STATE 1") # Enable Event file
wait 2

Osk::flight.ds.send_cmd("SET_FILE_STATE with FILE_TBL_IDX 6, FILE_STATE 1") # Enable Science Auxiliary file
wait  # Review configuration

#
# Instrument Simulator (iSim)
#

Osk::flight.isim.send_cmd("POWER_ON_INSTR")

wait("ISIM HK_TLM_PKT INSTR_STATE == 'READY'", 20) 

Osk::flight.isim.send_cmd("START_SCI_DATA")
wait  # Review configuration

# 
# Stored Commmands
#

Osk::flight.sc.send_cmd("ENABLE_RTS with RTS_ID 6") # Enable ISIM power off RTS
wait  # Review configuration

#
# Limit Checker
#

Osk::flight.lc.send_cmd("SET_APP_STATE with NEW_STATE 1") # Set LC to active mode
wait 2

Osk::flight.lc.send_cmd("SET_AP_STATE with AP_ID 2, NEW_STATE 1") # Enable ISIM Fault AP
wait  # Review configuration
