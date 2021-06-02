################################################################################
# SimSat File Manager Playback Demo 
#
# Notes:
#   1. Developed for the YouTube File Manager training video. This demo
#      illustrates how file manager could be used to manage the transfer
#      of files from an onboard recorder directory to the ground.  This is 
#      a manual approach illustrate some FM features. A more realistic 
#      approach would be to use an app like CFDP. 
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

require 'simsat_recorder_mgmt'

######################
## Global Variables ##
######################


FMD_DEMO_DIR  = "adir"
FMD_DEMO_FILE = "afile.txt"

FMD_FLT_DEMO_DIR      = File.join(Osk::FLT_DEMO_DIR,FMD_DEMO_DIR)
FMD_FLT_DEMO_FILE     = File.join(FMD_FLT_DEMO_DIR,FMD_DEMO_FILE)
FMD_FLT_DIR_LIST_FILE = File.join(FMD_FLT_DEMO_DIR,Osk::TMP_BIN_FILE)

FMD_GND_DEMO_DIR      = Osk::cfg_target_dir("FM","demo_files")
FMD_GND_DEMO_FILE     = File.join(FMD_GND_DEMO_DIR,FMD_DEMO_FILE)
FMD_GND_DIR_LIST_FILE = File.join(Osk::GND_SRV_DIR,Osk::TMP_BIN_FILE)

###########
## Setup ##
###########

# Configure system 

Osk::System.check_n_start_cfs('simsat')

# Configure apps 

cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME FM, BITMASK 0x01") # Enable debug events

cmd("ISIM","POWER_ON_INSTR") # Start ISIM to provide an example of an app with open files

# Open displays

display("CFS_KIT FILE_MGMT_SCREEN",1500,50)      
wait (1)
Cosmos.run_process("ruby tools/PacketViewer -p \"FM FILE_INFO_PKT\"")

status_bar("Confirm setup; A single PacketViewer window can be used to view all FM packets")
wait  # <Go> to continue

##############################################################
## Step 1 - Display flight server and create demo directory ##
##############################################################

# 1.1 - Send flight server directory listing in a packet

Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_SRV_DIR}, DIR_LIST_OFFSET 0")
wait  # Step 1.1 - Directory is listed in FILE_MGMT_SCREEN, note child task counter increments

# 1.2 Create OSK demo directory (if needed) and then display its contents

Osk::Ops.create_flt_dir(Osk::FLT_DEMO_DIR)
wait (1)
Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_DEMO_DIR}, DIR_LIST_OFFSET 0")
wait (1)
Osk::flight.send_cmd("FM","SEND_FILE_INFO with FILENAME #{Osk::FLT_DEMO_DIR}, CRC #{Fsw::Const::FM_IGNORE_CRC}")

wait  # Step 1.2 - Demo directory has been created if needed and should be empty

###########################################################
## Step 2 - Create new directory in OSK's demo directory ##
###########################################################

Osk::flight.send_cmd("FM","CREATE_DIR with DIRECTORY #{FMD_FLT_DEMO_DIR}")
wait (1)
Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{FMD_FLT_DEMO_DIR}, DIR_LIST_OFFSET 0")

wait  # Step 2 - Demo directory has been created and should be empty

#####################################################
## Step 3 - Transfer a file into the new directory ##
#####################################################

Osk::Ops::put_flt_file(FMD_GND_DEMO_FILE,FMD_FLT_DEMO_FILE)
wait (1)
Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{FMD_FLT_DEMO_DIR}, DIR_LIST_OFFSET 0")

wait  # Step 3 - Demo directory contains the demo file

#########################################################################################
## Step 4 - Send directory listing to file, download & display in COSMOS Table Manager ##
#########################################################################################

Osk::flight.send_cmd("FM","WRITE_DIR_TO_FILE with DIRECTORY #{Osk::FLT_SRV_DIR}, FILENAME #{FMD_FLT_DIR_LIST_FILE}, SIZE_TIME_MODE 1")
wait (2)
Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{FMD_FLT_DEMO_DIR}, DIR_LIST_OFFSET 0")

if (Osk::Ops.get_flt_file(FMD_FLT_DIR_LIST_FILE,FMD_GND_DIR_LIST_FILE))
   Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, Osk::TMP_BIN_FILE, Osk::TBL_MGR_DEF_FM_DIR)
else
   raise "FM Demo - File transfer from flight to ground failed" 
end
 
wait  # Step 4 - Demo directory contains the demo file

####################################
## Step 5 - Delete Demo directory ##
####################################

# Attempt to delete non-empty directory and get an error
Osk::flight.send_cmd("FM","DELETE_DIR with DIRECTORY #{FMD_FLT_DEMO_DIR}")

wait # Step 5.1 - Expect error event. Note valid command counter and invalid child command counter

Osk::flight.send_cmd("FM","DELETE_FILE with FILENAME #{FMD_FLT_DEMO_FILE}")
wait (1)
Osk::flight.send_cmd("FM","DELETE_FILE with FILENAME #{FMD_FLT_DIR_LIST_FILE}")
wait (1)
Osk::flight.send_cmd("FM","DELETE_DIR with DIRECTORY #{FMD_FLT_DEMO_DIR}")
wait (1)
Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_DEMO_DIR}, DIR_LIST_OFFSET 0")
wait (1)
Osk::flight.send_cmd("FM","SEND_FILE_INFO with FILENAME #{Osk::FLT_DEMO_DIR}, CRC #{Fsw::Const::FM_IGNORE_CRC}")

wait # Step 5.2 - File and directory deleted

####################################
## Step 6 - Send Open File Packet ##
####################################

# Enable ISIM science data which opens a science data file
Osk::flight.send_cmd("ISIM","START_SCI_DATA")
wait 2

Osk::flight.send_cmd("FM","SEND_OPEN_FILES")

wait  # Step 6 - ISIM's open science data fiel is listed

#############
## Cleanup ##
#############

cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME FM, BITMASK 0x01")   # Disable debug events
wait 1
cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME TFTP, BITMASK 0x01") # Disable debug events
wait 1
cmd("ISIM","POWER_OFF_INSTR")
   
clear("CFS_KIT FILE_MGMT_SCREEN")
