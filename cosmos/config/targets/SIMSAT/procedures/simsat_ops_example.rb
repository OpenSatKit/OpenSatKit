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
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_ops'

load_utility('simsat_instr_mgmt.rb')

#
# Ops Example Scenario
#
# 1. Establish the following app configuration
#       Runtime Environment Apps
#          KIT_CI  - No configuration required
#          KIT_SCH - Use default scheduler table
#          KIT_TO  - Use default subscription table
#       Data/File Management
#          FM - No configuration required
#          HK - Default configured to create 
#          DS - Default 
#       Autonomy
#          LC - Enable entire app to be in 'active' mode. Enable Action Point #2 that responds to ISIM faults.
#          SC - Enable RTS #6 that powers of ISIM
#       Attitude Determination and Control
#          I42 - No configuration required
#          F42 - No configuration required
#        
#          CS and HS are running but not used for the demo
#       Maintenance
#          MD and MM are running but not used for the demo
#
# 2. Configure the science instrument (i.e ISIM) for science
#       Power on the instrument and enable science data collection
#
# 3. Perform science ops (i.e. collect data and store to file) with downlink to illustrate FSW behavior
#
# 4. Simulate a ground pass (Manually controlled from ground Future release will use absolute time command sequence)
#
# 5. Continue science ops without downlink
#
# 6. Inject a fault
#
# 7. Simulate a ground pass
#

wait # Click <Go> when doen reading the scenario

#######################################
## 1. Establish the app configuration
#######################################

start("simsat_app_prep.rb")

###############################################################
## 2. Configure the science instrument (i.e ISIM) for science
###############################################################

simsat_isim_pwr_on

####################################################################
## 3. Perform automated science ops (i.e. collect data and store
##    to file). Have downlink so can observe activity but no uplink
##    so files can be transferred
####################################################################

# The science cycle counter counts from 0 to the number of science
# scans in a file (defaults to 60). Wait for at least 1 science file
# to be created.

5.times do
   #
   # Add an event to the event file
   # Refresh DS file info
   #
   
   Osk::flight.isim.send_cmd("NOOP") 
   Osk::flight.ds.send_cmd("SEND_FILE_INFO")
   wait 15

end


################################################################
## 4. Simulate a ground pass (Manually controlled from ground 
##    Future release will use absolute time command sequence)
################################################################

# TODO
# 1. Perform automated realtime health assesment 
# 2. Close, dump and display event file
# 3. Manage onboard files: FM dir, TFTP transfer, and FM delete file
#    Thsi will be replaced by CFDP
wait #

################################################################
## 5. Continue science ops without downlink then inject a fault
##    and observe autonmous LC & SC response
################################################################


simsat_isim_set_fault
wait # Observe automated response

###############################
## 7. Simulate a ground pass
###############################

# TODO
# 1. Perform automated realtime health assesment. Recognize
#    instument is off and take action to diagnose the situation 

