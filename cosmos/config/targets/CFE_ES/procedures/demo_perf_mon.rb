################################################################################
# Performance Monitor Demo 
#
# Notes:
#   1. Developed for the YouTube Performance Monitor training video and 
#      originally based the CFS_KIT PerfMon demo 
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

PMD_DAT_FILE = "perf_mon_demo.dat"
PMD_FLT_DAT_FILE = "#{Osk::FLT_SRV_DIR}/#{PMD_DAT_FILE}"
PMD_GND_DAT_FILE = "#{Osk::GND_SRV_DIR}/#{PMD_DAT_FILE}"

PMD_DIR_LIST_FILE = "perf_mon_dir.dat"
PMD_DIR_LIST_FLT_FILE = "#{Osk::FLT_SRV_DIR}/#{PMD_DIR_LIST_FILE}"

###########
## Setup ##
###########

display("CFS_KIT PERF_MON_SCREEN",1500,50)    

cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME FM, BITMASK 0x01") # Enable debug events
wait 2
cmd("KIT_SCH CFG_SCH_TBL_ENTRY with SLOT 2, ACTIVITY 10, ENABLED 0") # Disable FM HK request
wait 2
cmd("KIT_SCH CFG_SCH_TBL_ENTRY with SLOT 3, ACTIVITY 13, ENABLED 0") # Disable MD HK request
  
#########################################
## Step 1 - Configure the filter masks ##
#########################################

wait  # Step 1
 
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
      
##########################################
## Step 2 - Configure the trigger masks ##
##########################################

wait  # Step 2

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


###############################
## Step 3 - Collect the data ##
###############################

wait  # Step 3


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

wait  # Step 4

if (Osk::Ops.get_flt_file(PMD_FLT_DAT_FILE,PMD_GND_DAT_FILE))
   spawn("java -jar #{Osk::PERF_MON_DIR}/CPM.jar")
else
   raise "Performance Monitor Demo - File transfer from flight to ground failed" 
end    
 

#############
## Cleanup ##
#############

wait  # Cleanup

cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME FM, BITMASK 0x01") # Disable debug events
wait 2
cmd("KIT_SCH CFG_SCH_TBL_ENTRY with SLOT 2, ACTIVITY 10, ENABLED 1") # Enable FM HK request
wait 2
cmd("KIT_SCH CFG_SCH_TBL_ENTRY with SLOT 3, ACTIVITY 13, ENABLED 1") # Enable MD HK request

clear("CFS_KIT PERF_MON_SCREEN")
