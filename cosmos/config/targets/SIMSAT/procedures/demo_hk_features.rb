################################################################################
# SimSat Housekeeping Feature Demo 
#
# Notes:
#   1. Developed for the YouTube Housekeeping app training video. This demo
#      illustrates housekeeping app features using the SimSat configuration.
#      
# Demo Steps:
#   1. Create files in a recorder directory
#   2. Write recorder directory listing to a file
#   3. Transfer the directory file to the ground
#   4. Use the directory to manage the file transfer and onboard deletion
#      of each file  
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


######################
## Global Variables ##
######################



###########
## Setup ##
###########

# Configure system 

Osk::System.check_n_start_cfs('simsat')

# Configure apps - Done on a per step basis

# Open displays -  Done on a per step basis


############################################
## Step 1 - Display SimSat Configurations ##
############################################

# 1.1 - HK's copy table source file
Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::OSK_CFS_DIR}/apps/hk/fsw/tables/hk_cpy_tbl.c'")
status_bar("Observe two packets being defined: HK_COMBINED_PKT1_MID and HK_COMBINED_PKT2_MID")
wait  # <Go> to continue

# 1.2 - SimSat schedule table
Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::GND_TO_SIMSAT_SRV_DIR}/osk_sch_sch_tbl.json'")
status_bar("Search for 'HK Combined Pkt' entries and note frequency and message table entry IDs")
wait  # <Go> to continue

# 1.3 - SimSat message table
Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::GND_TO_SIMSAT_SRV_DIR}/osk_sch_msg_tbl.json'")
status_bar("Scroll to message table entry IDs noted in previous step or search for HK_COMBINED_PKT1_MID")
wait  # <Go> to continue


#####################################################
## Step 2 - Observe HK behavior without 42 running ##
#####################################################

display("SIMSAT DEMO_HK_SCREEN",1500,50)      

["CFE_ES", "CFE_EVS", "CFE_SB", "CFE_TBL", "CFE_TIME"].each do |app|
   Osk::flight.send_cmd(app,"NOOP") 
   wait 2
end

status_bar("Each command cFE app valid counter incremented. Combo packet counter increments. Missed data increments since 42 is not running")
wait  # <Go> to continue


##################################################
## Step 3 - Observe HK behavior with 42 running ##
##################################################

cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME HK, BITMASK 0x01") # Enable debug events to view missed packets
status_bar("Observe missed data events")
wait  # <Go> to continue

Osk::System.start_42

status_bar("Observe missed data events and counter increments stop")
wait  # <Go> to continue


#############
## Cleanup ##
#############

cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME HK, BITMASK 0x01")   # Disable debug events
   
clear("SIMSAT DEMO_HK_SCREEN")
