################################################################################
# cFE Executive Service Performance Monitor Demo 
#
# Notes:
#   1. Developed for the YouTube Performance Monitor training video and 
#      originally based the CFS_KIT PerfMon screen-based demo 
#   2. The performance monitor feature has been equated to a Logic 
#      Analyzer (LA) so LA is in the command names and in the code.
#   3. Debug events are enabled for the apps used during the demo.
#
# Demo Steps:
#   1. Configure the filter masks
#   2. Configure the trigger masks
#   3. Collect the data
#   4. Transfer file from flight to ground & display in cFS Performance
#      Monitor (CPM)
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'osk_global'
require 'osk_system'
require 'osk_flight'

######################
## Global Variables ##
######################

FM_HK_SLOT     = 2
FM_HK_ACTIVITY = 10

MD_HK_SLOT     = 3
MD_HK_ACTIVITY = 13

PMD_DAT_FILE = "perf_mon_demo.dat"
PMD_FLT_DAT_FILE = "#{Osk::FLT_SRV_DIR}/#{PMD_DAT_FILE}"
PMD_GND_DAT_FILE = "#{Osk::GND_SRV_DIR}/#{PMD_DAT_FILE}"

PMD_DIR_LIST_FILE = "perf_mon_dir.dat"
PMD_DIR_LIST_FLT_FILE = "#{Osk::FLT_SRV_DIR}/#{PMD_DIR_LIST_FILE}"

###########
## Setup ##
###########

display("CFE_ES PERF_MON_SCREEN",1500,50)    

cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME FM, BITMASK 0x01") # Enable debug events
wait 2
cmd("KIT_SCH CFG_SCH_TBL_ENTRY with SLOT #{FM_HK_SLOT}, ACTIVITY #{FM_HK_ACTIVITY}, ENABLED 0") # Disable FM HK request
wait 2
cmd("KIT_SCH CFG_SCH_TBL_ENTRY with SLOT #{MD_HK_SLOT}, ACTIVITY #{MD_HK_ACTIVITY}, ENABLED 0") # Disable MD HK request

wait # 
# The Performance Monitor utility contains a flight and ground component that allows a user
# to trace and display the execution of the flight software. The performance monitor screen
# has been launched that will display the cFE ES performance monitor configuration/status as
# the demo steps are being performed.  
#
# The File Manager(FM) and Memory Dwell(MD) apps are used during the demo. Their 'send
# housekeeping (HK) requests' in the scheduler app were disabled during the demo setup. The
# demo traces the FM and MD execution and command responses and the HK requests responses
# clutter the FSW execution traces making it harder to follow the demo.  
#
# The cFE Executive Serve (ES) app controls the performance monitor data. The operational
# steps are
#  1. Send SET_LA_FILTER_MASK to configure which performance monitor traces will be logged
#  2. Send SET_LA_TRIG_MASK to configure what trigger(s) will start the trace log
#  3. Send the START_LA_DATA command to enabled the data collection that will be started when
#     a trigger occurs
#  4. Wait some amount of time for the data to be collected
#  5. Send the STOP_LA_DATA command. This writes the data to a file
#  6. Transfer the data to the ground
#  7. Analyse the data using the cFS Performance Monitor tool.
#
# ES configuration parameter CFE_ES_PERF_DATA_BUFFER_SIZE defines amount of memory used to
# capture execution markers. The tools perfutils-java directory contains the CPM user's guide.

#########################################
## Step 1 - Configure the filter masks ##
#########################################

wait  # Start step 1 
# Send CE ES commands to configure the following trace filter masks. The masks are arrays of
# 32-bit words with one bit used to enabled/disable a performance monitor ID trace. 
#
#  ID  Definition              Mask[] 
#  --  ----------------------- --------------
#  26  Memory Dwell App        [0] 0x04000000
#  39  File Manager App        [1] 0x00000080
#  44  File Manager Child Task [1] 0x00001000

Osk::flight.send_cmd("CFE_ES","SET_LA_FILTER_MASK with FILTER_MASK_NUM 0, FILTER_MASK 0x04000000")  
wait(1)
Osk::flight.send_cmd("CFE_ES","SET_LA_FILTER_MASK with FILTER_MASK_NUM 1, FILTER_MASK 0x00001080")  
wait(1)
Osk::flight.send_cmd("CFE_ES","SET_LA_FILTER_MASK with FILTER_MASK_NUM 2, FILTER_MASK 0x00000000")  
wait(1)
Osk::flight.send_cmd("CFE_ES","SET_LA_FILTER_MASK with FILTER_MASK_NUM 3, FILTER_MASK 0x00000000")  

wait # End step 1
# Confirm on the screen that the masks are configured properly. There is one bit for each
# performance marker (ID) and a '1' indicates the ID should be traced. The masks are arranged
# in an array and the bits are logically numbered 0..127 spanning the 32-bit array entries.
#
# ES configuration parameter CFE_ES_PERF_MAX_IDS defines the number of performance monitor IDs.
# It defaults to 128 which is why the demo page is configured as a 4 dimensional array with 32
# bits per entry.

##########################################
## Step 2 - Configure the trigger masks ##
##########################################

wait  # Start step 2 
# Send CE ES commands to configure one trigger which is MD's 1Hz execution. The masks are arrays of
# 32-bit words with one bit used to enabled/disable a performance monitor ID trigger. 
#
#  ID  Definition              Mask[] 
#  --  ----------------------- --------------
#  26  Memory Dwell App        [0] 0x04000000

Osk::flight.send_cmd("CFE_ES","SET_LA_TRIG_MASK with TRIG_MASK_NUM 0, TRIG_MASK 0x04000000")  
wait(1)
Osk::flight.send_cmd("CFE_ES","SET_LA_TRIG_MASK with TRIG_MASK_NUM 1, TRIG_MASK 0x00000000")  
wait(1)
Osk::flight.send_cmd("CFE_ES","SET_LA_TRIG_MASK with TRIG_MASK_NUM 2, TRIG_MASK 0x00000000")  
wait(1)
Osk::flight.send_cmd("CFE_ES","SET_LA_TRIG_MASK with TRIG_MASK_NUM 3, TRIG_MASK 0x00000000")  

wait # End step 2
# Confirm on the screen that the triggers are configured properly. The trigger mask are structured
# the same as the filter masks. A '1' indicates to use an ID as a trigger to start data capture.


###############################
## Step 3 - Collect the data ##
###############################

wait  # Start step 3
# Send CFE_ES START_LA_DATA cmd to enable the start of data collection. After ~16 seconds
# the CFE_ES STOP_LA_DATA cmd is sent and the data is written to a file. During the 16s 
# the following commands are issued and their execution will be traced:
#  - Wait 4s, Send FM Send Directory packet command
#  - Wait 4s, Send FM Write Directory to file command
#  - Wait 4s, Send FM Send Directory packet command
#  - Wait 4s, Send STOP_LA_DATA command

Osk::flight.send_cmd("CFE_ES","START_LA_DATA with TRIG_MODE 0")
wait 4
Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_SRV_DIR}, DIR_LIST_OFFSET 0")
wait 4
Osk::flight.send_cmd("FM","WRITE_DIR_TO_FILE with DIRECTORY #{Osk::FLT_SRV_DIR}, FILENAME #{PMD_DIR_LIST_FLT_FILE}")
wait 4
Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_SRV_DIR}, DIR_LIST_OFFSET 0")
wait 4

cmd_valid_cnt = tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT")
cmd_error_cnt = tlm("CFE_ES HK_TLM_PKT CMD_ERROR_COUNT")
seq_cnt       = tlm("CFE_ES HK_TLM_PKT CCSDS_SEQUENCE")

Osk::flight.send_cmd("CFE_ES","STOP_LA_DATA with DATA_FILENAME #{PMD_FLT_DAT_FILE}")  

# Perform some error checks before launching the cFS Performance Monitor tool 
wait("CFE_ES HK_TLM_PKT CMD_VALID_COUNT == #{cmd_valid_cnt}+1", 10)  # Delay until updated valid cmd count or timeout
if ( (tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT") != (cmd_valid_cnt + 1)) || 
     (tlm("CFE_ES HK_TLM_PKT CMD_ERROR_COUNT") !=  cmd_error_cnt))
   if (tlm("CFE_ES HK_TLM_PKT CCSDS_SEQUENCE") == seq_cnt)
      prompt ("No telemetry received to verify the error.\nVerify connection and telemetry output filter table.");
   else
      prompt ("Executive Service had an error processing the command.\nSee event message for details.")
   end
end 


###################################################################
## Step 4 - Transfer file from flight to ground & display in CPM ##
###################################################################

wait  # Start step 4
# Transfer data file to the ground and launch the cFS Performance Monitor(CPM) tool

if (Osk::Ops.get_flt_file(PMD_FLT_DAT_FILE,PMD_GND_DAT_FILE))
   spawn("java -jar #{Osk::PERF_MON_DIR}/CPM.jar")
else
   raise "Performance Monitor Demo - File transfer from flight to ground failed" 
end    

wait  # View data in the CPM 
# Perform the following steps to import and view the data in the CPM tool:
#   1. Select the Log Menu item in the top left and then the Read Log drop
#      down menu.
#   2. In the file dialogue box navigate to #{Osk::GND_SRV_DIR}
#      directory
#   3. Select #{PMD_DAT_FILE} and click Read.
#   4. You'll see a couple of status dialogue boxes stating the number of IDs
#      (should be 3) and that some inconsistencies exist. This is okay and
#      it may or may not be true depending on timing.
#
# The total time is roughly 16 seconds and the commanding events will occur
# before seconds 4, 8, and 12 because the COSMOS wait commands are not
# exactly 4 seconds.
#
# There are four rows in the display:
#   CPU Activity - This is a summation of all the traced activity.
#     oxoooooo1A - MD App - Wakes up at 1Hz.
#     oxoooooo27 - FM App - Wakes up when command received
#     oxoooooo2c - FM Child Task - Runs for each command. Notice write directory
#                                  to file takes longer than the send directory 
#                                  listing to a telemetry packet

#############
## Cleanup ##
#############


cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME FM, BITMASK 0x01") # Disable debug events
wait 2
cmd("KIT_SCH CFG_SCH_TBL_ENTRY with SLOT #{FM_HK_SLOT}, ACTIVITY #{FM_HK_ACTIVITY}, ENABLED 1") # Enable FM HK request
wait 2
cmd("KIT_SCH CFG_SCH_TBL_ENTRY with SLOT #{MD_HK_SLOT}, ACTIVITY #{MD_HK_ACTIVITY}, ENABLED 1") # Enable MD HK request

clear("CFE_ES PERF_MON_SCREEN")
