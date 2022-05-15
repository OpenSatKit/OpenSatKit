################################################################################
# cFE Table Service Load/Dump Table Demo 
#
# Notes:
#   1. TMD_INSTRUCT_x variables are used to put text on the demo screen's
#      text box. The TMD_INFO_x variables are used for extra detailed
#      information that is displayed on a separate screen.
#   2. Debug events are enabled for the apps used during the demo.
#
# Demo Steps:
#  1 - Display table registry for Memory Dwell(MD) app table
#  2 - Configure MD table and poke values in locations being monitored
#  3 - Write table to file, transfer file to ground, and display the tabke
#      in COSMOS Table Manager
#  4 - Illustrate how MD table is monitoring memory locations
#  5 - Modify dwell table and load it to show new behavior
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'osk_ops'
require 'fsw_config_param'

######################
## Global Variables ##
######################

TMD_TBL_FILE = "md_tbl1.dat"
TMD_TBL_PATH_FILE = "#{Osk::FLT_SRV_DIR}/#{TMD_TBL_FILE}"

TMD_TBL_NAME = "MD.DWELL_TABLE1"  # cFS table name
TMD_TBL_DEF_NAME = "md_tbl.txt"   # COSMOS table definition file

TMD_TMP_FLT_PATH_FILENAME = "#{Osk::FLT_SRV_DIR}/#{Osk::TMP_TBL_FILE}"
TMD_TMP_GND_PATH_FILENAME = "#{Osk::GND_SRV_TBL_DIR}/#{Osk::TMP_TBL_FILE}"


###########
## Setup ##
###########

display("CFE_TBL TABLE_MGMT_SCREEN",1500,50)    

cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME CFE_TBL, BITMASK 0x01") # Enable debug events

wait # Setup notes: 
# This demo shows basic table management features using Memory Dwell app's (MD)
# 'dwell' table. The Trivial File Transport Protocol (TFTP) app transfers table
# files between COSMOS and the cFS.
#
# The Memory Dwell(MD) application creates a telemetry packet containing the contents of memory
# locations that are specified in its 'dwell' table. MD is typically used for in-orbit FSW
# maintenance or for telemetry changes that occur late in a spacecraft's development lifecycle
# when it's undesirable to change a telemetry packet definition.
#
# This demo configures MD to monitor memory starting at symbol #{FswConfigParam::MM_SYMBOL} plus
# offset #{FswConfigParam::MM_OFFSET_W0}. These are known locations that can be safely altered
# during the demo.

  
#####################################
## Step 1 - Display table registry ##
#####################################

wait # Step 1
# The table management screen was opened. This screen is a combination of information from the
# cfE's Table Service (TBL) Housekeeping telemetry packet and TBL Service's table registry packet
# that is sent in response to a SEND_REGISTRY command
#
# Send TBL SEND_REGISTRY cmd with TABLENAME #{TMD_TBL_NAME}
               
Osk::flight.cfe_tbl.send_cmd("SEND_REGISTRY with TABLE_NAME #{TMD_TBL_NAME}")
  
wait # Step notes:
# Review the MD table registry entry values


###########################################
## Step 2 - Configure Memory Dwell Table ##
###########################################

display("MD DWELL_PKT_SCREEN",50,50)

wait # Step 2
# Configure MD table and poke values in locations being monitored
# 
# Memory Dwell's telemetry screen was opened. The commands sent using in this
# step will 'jam' MD's table with locations to monitor. 'Jamming' is a cFS term
# used to describe loading individual table values using a command without
# loading the entire table from a file using table service's load table. You
# will see the 'jammed' memory dwell location values when MD's table is dumped
# and displayed.
#
# This demo uses the Memory Manager(MM) app's 'Poke' command to write command-
# supplied values into a specified memory locations. MM provides a ground
# interface for directly reading/writing memory locations. It is typically used
# for testing and on-orbit maintenance. 

# Jam MD's table to monitor locations at symbol #{FswConfigParam::MM_SYMBOL}+#{FswConfigParam::MM_OFFSET_W0}
# Send Memory Manager cmds to write 1,2,3,4,... into monitored locations

Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 1, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W0}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 2, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W1}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 3, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W2}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 4, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W3}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MD","SET_SIGNATURE with TABLE_ID 1, PAD_16 0, SIGNATURE 'Table  Demo'")
wait 1   

Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x01020304, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W0}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x05060708, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W1}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0X090A0B0C, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W2}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x0D0E0F10, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W3}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MD","START_DWELL with TABLE_MASK 0x0001")


wait # Step notes:
# Note the event messages in the cFS terminal window and that the MD dwell telemetry
# page starts updating


################################################
## Step 3 - Dump, Transfer, and Display Table ##
################################################

wait # Step 3
# 'Dump' is a historical cFS term that was used when a table's content 
# was transferred directly to the telemetry downlink. This term was adopted
# before onboard files systems were used. The term is still used today
# although what takes place are discrete steps that include writing a
# table to a file and then transferring the file to the ground.
                     
# Use default temporary binary table file names in flight and ground default table server directories
Osk::Ops::send_cfe_dump_tbl_cmd(TMD_TBL_NAME, Osk::TBL_MGR_DEF_MD_TBL)
      
#if (Osk::flight.cfe_tbl.send_cmd("DUMP_TBL with ACTIVE_TBL_FLAG 1, 
#                                 TABLE_NAME #{TMD_TBL_NAME}, 
#                                 FILENAME #{TMD_TMP_FLT_PATH_FILENAME}"))
#
#   if Osk::Ops::get_flt_file(TMD_TMP_FLT_PATH_FILENAME, TMD_TMP_GND_PATH_FILENAME)
#      
#      Cosmos.run_process("ruby tools/TableManager") # Display table using the Table Manager tool 
#   
#   end
#
#end

wait # Step notes:
# TBD - Describe MD table registry 


########################################
## Step 4 - Show MD Table is Working  ##
########################################

wait # Step 4
# Demonstrate that MD monitoring the memory locations that were jammed into its
# table. This steps sends a memory 'poke' command to change the value at the 
# moemory location being monitored. The poke value starts at #{TMD_INITIAL_POKE_VAL}
# and increments each time the poke command is sent.

(32..37).each do |poke_val|

   Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 8, MEM_TYPE 1, 
                        DATA #{poke_val},
                        ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W0},
                        ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")

   wait 3
   
end


wait # Step notes:
# Note the MD event messages and verify MD page values updated as expected


################################
## Step 5 - Load new MD Table ##
################################

wait # Step 5
# Modify dwell table and load it to show new behavior
# In the Table Manager tool display change ENTRY0_ADDR_OFFSET from #{FswConfigParam::MM_OFFSET_W0} to
# #{FswConfigParam::MM_OFFSET_W3} and hit <enter> to ensure the change sticks. From the tool's menu
# select File->Save to save changes then hit <Go> to execute execute this step

if (Osk::system.file_transfer.put(TMD_TMP_GND_PATH_FILENAME, TMD_TMP_FLT_PATH_FILENAME))

   if (Osk::flight.cfe_tbl.send_cmd("LOAD_TBL with FILENAME #{TMD_TMP_FLT_PATH_FILENAME}"))

      if (Osk::flight.cfe_tbl.send_cmd("VALIDATE_TBL with ACTIVE_TBL_FLAG 0, TABLE_NAME #{TMD_TBL_NAME}"))
         
         wait 2
         if (Osk::flight.cfe_tbl.send_cmd("ACTIVATE_TBL with TABLE_NAME #{TMD_TBL_NAME}"))
         
         end
      end # End if validate table
   end # End if load table 
end # End if transfer table file to flight

wait # Step notes:
# TBD - Describe MD table registry 


#############
## Cleanup ##
#############

cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME CFE_TBL, BITMASK 0x01") # Disable debug events

clear("CFE_TBL TABLE_MGMT_SCREEN")
clear("MD DWELL_PKT_SCREEN")  
 

