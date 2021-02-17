###############################################################################
# Simple Sat (SimSat) Ops Example Script
#
# This script configures SimSat to illustrate the cFS and OSK apps functioning
# as a system to meet mission operational objectives. It is intentionally kept
# very simple. The descriptions in this script are kept to a high level and
# more details can be found in the tutorials in each simsat functional app
# grouped screen.
#
# Notes:
#   1. This can be run with or without the 42 simulator running.
#   2. The iSim app uses more event messages than a typical flight app. The
#      events are used for illustrative purposes.  
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

require 'simsat_const'

#
# Configure global variables and utility functions
#

# Ensure SimSat directories exist
seq_count = tlm("FM FILE_INFO_PKT CCSDS_SEQUENCE")
Osk::flight.send_cmd("FM","SEND_FILE_INFO with FILENAME #{SimSat::FLT_SRV_DIR}")
wait("FM FILE_INFO_PKT CCSDS_SEQUENCE != #{seq_count}", 5)
if (tlm("FM FILE_INFO_PKT SIZE") == 0)
   Osk::flight.send_cmd("FM","CREATE_DIR with DIRECTORY #{SimSat::FLT_SRV_DIR}")
   wait 1
   Osk::flight.send_cmd("FM","CREATE_DIR with DIRECTORY #{SimSat::FLT_REC_DIR}")
else
   seq_count = tlm("FM FILE_INFO_PKT CCSDS_SEQUENCE")
   Osk::flight.send_cmd("FM","SEND_FILE_INFO with FILENAME #{SimSat::FLT_REC_DIR}")
   wait("FM FILE_INFO_PKT CCSDS_SEQUENCE != #{seq_count}", 5)
   if (tlm("FM FILE_INFO_PKT SIZE") == 0)
      Osk::flight.send_cmd("FM","CREATE_DIR with DIRECTORY #{SimSat::FLT_REC_DIR}")
   end
end

$simsat_ops_enable = true

$SIMSAT_SCH_TBL_GND_FILENAME = Osk::cfg_target_dir_file("SIMSAT","tables",SimSat::SCH_TBL_FILENAME) 
$SIMSAT_SCH_TBL_FLT_FILENAME = File.join(Osk::FLT_SRV_DIR,SimSat::SCH_TBL_FILENAME)


simsat_ops_status = "Preparing Ops Example. Read scenario comments in Script Runner."
load_utility('simsat_ops_example_utils.rb')
load_utility('simsat_recorder_mgmt.rb')
load_utility('simsat_isim_mgmt.rb')  #Provide methods for managing the instrument. E.g. power instrument on/off

#
# Create ops screen dynamically so script variables can be displayed
#

clear("SIMSAT OPS_SCREEN")  # Doesn't work for script generated screen
simsat_create_ops_screen


#
# Ops Example Scenario
#
# 1. Establish the following app configuration
#       Runtime Environment Apps
#          KIT_CI  - No configuration required
#          KIT_SCH - Load SimSat scheduler table
#          KIT_TO  - OSK default packet table with uploads as needed
#       Data/File Management
#          FM - No configuration required
#          HK - OSK Default: Combo#1 cFE cmd counters, Combo#2 F42 & ISIM states  
#          DS - OSK Default:  
#       Autonomy
#          LC - Enable entire app to be in 'active' mode. Enable Action Point #2 that responds to ISIM faults.
#          SC - Enable RTS #6 that powers of ISIM
#       Attitude Determination and Control
#          I42 - No configuration required
#          F42 - No configuration required
#       Health & Safety
#          CS and HS are running but not used for the demo
#       Maintenance
#          MD and MM are running but not used for the demo
#
# 2. Configure the science instrument (i.e ISIM) for science
#       Power on the instrument and enable science data collection
#
# 3. Perform science ops (i.e. collect data and store to file) with downlink to illustrate FSW behavior
#
# 4. Simulate a ground pass (Manually controlled from ground (future release will use absolute time command sequence)
#
# 5. Continue science ops without downlink
#
# 6. Inject a fault
#
# 7. Simulate a ground pass after a fault
#

status_bar("Review scenario")
wait  # <Go> to continue


#######################################
## 1. Establish the app configuration
#######################################

simsat_ops_status = "Configuring apps"
start("simsat_app_config.rb")

###############################################################
## 2. Configure the science instrument (i.e ISIM) for science
###############################################################

simsat_ops_status = "Powering on instrument"
simsat_isim_pwr_on

####################################################################
## 3. Perform automated science ops (i.e. collect data and store
##    to file). Have downlink so can observe activity but no uplink
##    so files can't be transferred
####################################################################

#
# Note  there's background "cheating" going on by a ruby thread
# that was created when the example started.  The thread is sending
# commands to File Manager(FM) and Data Storage(DS) to send telemetry
# packets that are sent only in response to commands. This could be
# automated in a stored command if needed by a mission whene there's
# no uplink. 
#
# What to observe:
# 1. The ISIM app generates a new science file every minute. ISIM
#    simulates collecting science data at 1Hz and a new filee is 
#    started after 60 scans.
# 2. Data Storage creates new event and science auxiliary files
#    every 5 minutes.
#
# Wait at least 5 minutes so every type of file is generated. It goes
# by quick if you look into details of what's going on! 
#
#

simsat_ops_status = "5 Minutes of automated science and DS activities"

# Cycle through a few apps to create different noop events
app_list  = ["KIT_CI", "KIT_SCH", "KIT_TO", "ISIM", "I42", "F42"]
app_cycle = app_list.cycle

20.times do |n|

   puts "Ops loop #{n}"
   Osk::flight.send_cmd(app_cycle.next,"NOOP") 
   status_bar("Waiting in ops loop #{n}. Click <Go> shorten wait time")
   wait 15 # You can click Script Runner's <Go> button if you don't want to wait

end


################################################################
## 4. Simulate a ground pass (Manually controlled from ground 
##    Future release will use absolute time command sequence)
################################################################

# 1. Perform automated realtime health assesment 
# 2. Close, dump and display event file
# 3. Manage onboard files: FM dir, TFTP transfer, and FM delete file
#    This will be replaced by CFDP
simsat_ops_status = "Ground pass"
start("simsat_gnd_pass.rb")

simsat_ops_status = "Completed a ground pass. No errors and successful recorder mgmt. Review flt/gnd simsat dir"
status_bar("Completed a ground pass. Review flight/ground simsat directories")
wait  # <Go> to continue

################################################################
## 5. Continue science ops without downlink then inject a fault
##    and observe autonmous LC & SC response
################################################################

simsat_ops_status = "Simulating an instrument fault. LC/SC will respond."
simsat_isim_set_fault

status_bar("Observe response to ISIM fault")
wait  # <Go> to continue

###############################
## 7. Simulate a ground pass
###############################

# TODO
# 1. Perform automated realtime health assesment. Recognize
#    instument is off and take action to diagnose the situation 
start("simsat_gnd_pass.rb")

simsat_ops_status = "Completed a ground pass with errors. Review current flight software state"
status_bar("Completed a ground pass with errors; review current flight software state")
wait  # <Go> to continue

###############################
## 8. Clean up
###############################

simsat_ops_status = "Example ops script complete"
simsat_ops_example_teardown
