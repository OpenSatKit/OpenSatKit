################################################################################
# File Manager Feature Demo 
#
# Notes:
#   1. Developed for the YouTube File Manager training video and 
#      originally based the CFS_KIT File Manager demo 
#   2. Debug events are enabled for the apps used during the demo.
#      COSMOS cmd() is used instead of OSK App.send_cmd() because speed is
#      preferred over command verification
#   3. Starts SimSat target if cFS is not running because the demo depends
#      on ISIM
#
# Demo Steps:
#   1. Display directories and create OSK directroy if needed
#      - Note child task counter increments
#   2. Create new directory in OSK's demo directory
#      - Display new directory listing
#      - Display file info on the new directory
#   3. Transfer a file into the new directory
#      - Display new directory listing
#      - Display file info on the new directory
#   4. Send FSW server directory listing to a file
#      - Transfer dirctory listing file
#      - Display the directory listing using table manager
#   5. Delete demo file directory
#      - First attempt to delete a non-empty directory
#   6. List open Files
#      - Start ISIM science data  
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

status_bar("Step 1.1 - Directory '#{Osk::FLT_SRV_DIR}' is listed in FILE_MGMT_SCREEN, note child task counter increments")
wait  # <Go> to continue

# 1.2 - Send flight erroneous directory listing command

Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_SRV_DIR}__, DIR_LIST_OFFSET 0")

status_bar("Step 1.2 - Expect error event message; app cmd error counter increments; child cmd counters unchanged")
wait  # <Go> to continue

# 1.3 Create OSK demo directory (if needed) and then display its contents

Osk::Ops.create_flt_dir(Osk::FLT_DEMO_DIR)
wait (1)
Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_DEMO_DIR}, DIR_LIST_OFFSET 0")
wait (1)
Osk::flight.send_cmd("FM","SEND_FILE_INFO with FILENAME #{Osk::FLT_DEMO_DIR}, CRC #{Fsw::Const::FM_IGNORE_CRC}")

status_bar("Step 1.3 - OSK demo directory '#{Osk::FLT_DEMO_DIR}' created if needed and should be empty")
wait  # <Go> to continue

###########################################################
## Step 2 - Create new directory in OSK's demo directory ##
###########################################################

Osk::flight.send_cmd("FM","CREATE_DIR with DIRECTORY #{FMD_FLT_DEMO_DIR}")
wait (1)
Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{FMD_FLT_DEMO_DIR}, DIR_LIST_OFFSET 0")

status_bar("Step 2 - Created directory '#{FMD_FLT_DEMO_DIR}'")
wait  # <Go> to continue

#####################################################
## Step 3 - Transfer a file into the new directory ##
#####################################################

Osk::Ops::put_flt_file(FMD_GND_DEMO_FILE,FMD_FLT_DEMO_FILE)
wait (1)
Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{FMD_FLT_DEMO_DIR}, DIR_LIST_OFFSET 0")

status_bar("Step 3 - Transferred file '#{FMD_FLT_DEMO_FILE}'")
wait  # <Go> to continue

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
 
status_bar("Step 4 - Review '#[#{Osk::FLT_SRV_DIR}' directory listing in Table Manager")
wait  # <Go> to continue

####################################
## Step 5 - Delete Demo directory ##
####################################

# 5.1 - Attempt to delete non-empty directory and get an error
Osk::flight.send_cmd("FM","DELETE_DIR with DIRECTORY #{FMD_FLT_DEMO_DIR}")

status_bar("Step 5.1 - Expect error event; Note app valid command counter and invalid child command counter")
wait  # <Go> to continue

# 5.2 - Delete demo files and directory

Osk::flight.send_cmd("FM","DELETE_FILE with FILENAME #{FMD_FLT_DEMO_FILE}")
wait (1)
Osk::flight.send_cmd("FM","DELETE_FILE with FILENAME #{FMD_FLT_DIR_LIST_FILE}")
wait (1)
Osk::flight.send_cmd("FM","DELETE_DIR with DIRECTORY #{FMD_FLT_DEMO_DIR}")
wait (1)
Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_DEMO_DIR}, DIR_LIST_OFFSET 0")
wait (1)
Osk::flight.send_cmd("FM","SEND_FILE_INFO with FILENAME #{Osk::FLT_DEMO_DIR}, CRC #{Fsw::Const::FM_IGNORE_CRC}")

status_bar("Step 5.2 - Transferred file '#{FMD_FLT_DEMO_FILE}'")
wait  # <Go> to continue

####################################
## Step 6 - Send Open File Packet ##
####################################

# Enable ISIM science data which opens a science data file
Osk::flight.send_cmd("ISIM","START_SCI_DATA")
wait 2

Osk::flight.send_cmd("FM","SEND_OPEN_FILES")

status_bar("Step 6 - ISIM's open science data file is listed")
wait  # <Go> to continue

#############
## Cleanup ##
#############

cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME FM, BITMASK 0x01")   # Disable debug events
wait 1
cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME TFTP, BITMASK 0x01") # Disable debug events
wait 1
cmd("ISIM","POWER_OFF_INSTR")
   
clear("CFS_KIT FILE_MGMT_SCREEN")
