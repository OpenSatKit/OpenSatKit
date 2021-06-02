################################################################################
# SimSat Maintenance Memory App Feature Demo 
#
# Notes:
#   1. Originally a rehost of the memory management screen-based demo. It may
#      diverge as needed. This illustrate basic memory management features using
#      the Memory Management(MM) app to peek/load and poke/dump memory and the
#      Memory Dwell(MD) app to autonomously monitor and telemeter memory
#      locations.
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

MMD_PAT_ABCD   = 0x0A0B0C0D
MMD_PAT_1234   = 0x01020304
MMD_PAT_0A0B   = 0x0A0B
MMD_PAT_A0     = 0xA0
MMD_PAT_A0_STR = "0xA0"

# MM_AppData+256 is a safe area in MM's global data structure that can be used
# as a working buffer during the demo

# Can get start address from table results dump

MMD_SYMBOL = "MM_AppData"
MMD_OFFSET_W0 = 256
MMD_OFFSET_W1 = 260
MMD_OFFSET_W2 = 264
MMD_OFFSET_W3 = 268

MMD_DMP_FILE = "mm_demo_dmp.dat"

MMD_FLT_PATH_FILENAME = "#{Osk::FLT_SRV_DIR}/#{MMD_DMP_FILE}"
MMD_GND_PATH_FILENAME = "#{Osk::GND_SRV_DIR}/#{MMD_DMP_FILE}"


###########
## Setup ##
###########

# Configure system 

Osk::System.check_n_start_cfs('simsat')

# Configure apps

cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME MM, BITMASK 0x01") # Enable debug events
wait (1)
cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME MD, BITMASK 0x01") # Enable debug events
wait (1)
# Ensure HS aliveness indicator is disabled because it displays dots in the cFS console that
# make it hard to read some messages
Osk::flight.send_cmd("HS", "DIS_ALIVENESS") 

# Open displays - Done on a per step basis

display("CFS_KIT MEMORY_MGMT_SCREEN",1500,50)    
display("MD DWELL_PKT_SCREEN",50,50)    

status_bar("Review screen content and demo steps below. ")
wait  # <Go> to continue
#
# 1 - Lookup symbol to be used as demo base address
# 2 - Jam & start memory dwell
# 3 - Poke, Peek, and dump in event 
# 4 - Dump memory to a file, transfer to ground, and display it
# 5 - Fill memory with pattern from command
# 6 = Load memory from dump file to restore values
#

############################################################
## Step 1 - Lookup symbol to be used as demo base address ##
############################################################

Osk::flight.send_cmd("MM","LOOKUP_SYMBOL with SYMBOL_NAME #{MMD_SYMBOL}")

status_bar("Note address of #{MMD_SYMBOL} is displayed in an event message and MD HK telemetry")
wait  # <Go> to continue

#####################################################
## Step 2 - Jam Memory Dwell Table and start dwell ##
#####################################################

Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 1, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
wait (1)
Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 2, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
wait (1)
Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 3, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W2}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
wait (1)
Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 4, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W3}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
wait (1)
Osk::flight.send_cmd("MD","SET_SIGNATURE with TABLE_ID 1, PAD_16 0, SIGNATURE 'Memory Management Demo'")

status_bar("Dwell table has not been activated so dwell packet is not being generated")
wait  # <Go> to continue

Osk::flight.send_cmd("MD","START_DWELL with TABLE_MASK 0x0001")

status_bar("Observe dwell packet sequence counter updating and other status fields populated. Data values probably zero if MM hasn't been used since the cFS was started.")
wait  # <Go> to continue

#############################################
## Step 3 - Poke, Peek, and dump in event  ##
#############################################

Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x01020304, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
wait (1)
Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x05060708, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
wait (1)
Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0X090A0B0C, ADDR_OFFSET #{MMD_OFFSET_W2}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
wait (1)
Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x0D0E0F10, ADDR_OFFSET #{MMD_OFFSET_W3}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")

status_bar("Observe 'poked' values in dwell packet")
wait  # <Go> to continue

Osk::flight.send_cmd("MM","PEEK_MEM with DATA_SIZE  8, MEM_TYPE 1, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
wait (1)
Osk::flight.send_cmd("MM","PEEK_MEM with DATA_SIZE 16, MEM_TYPE 1, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
wait (1)
Osk::flight.send_cmd("MM","PEEK_MEM with DATA_SIZE 32, MEM_TYPE 1, ADDR_OFFSET #{MMD_OFFSET_W2}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")

status_bar("Peek allows a different number of bytes to be specified in the event message")
wait  # <Go> to continue

Osk::flight.send_cmd("MM","DUMP_IN_EVENT with MEM_TYPE 1, NUM_BYTES 8, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")

status_bar("Note dump to event treats memory as an array of bytes. Peek and MD telemetry use different unsigned integer data widths")
wait  # <Go> to continue

########################################################################
## Step 4 - Dump memory to a file, transfer to ground, and display it ##
########################################################################


Osk::flight.send_cmd("MM","DUMP_MEM_TO_FILE with MEM_TYPE 1, NUM_BYTES 16, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}, FILENAME #{MMD_FLT_PATH_FILENAME}")
wait (2)
if (Osk::Ops.get_flt_file(MMD_FLT_PATH_FILENAME,MMD_GND_PATH_FILENAME))
   Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, MMD_DMP_FILE, Osk::TBL_MGR_DEF_MM_DMP)
else
   raise "Maintenance App Demo - File transfer from flight to ground failed" 
end

status_bar("Note dump to file treats memory as an array of bytes")
wait  # <Go> to continue

####################################################
## Step 5 - Fill memory with pattern from command ##
####################################################

Osk::flight.send_cmd("MM","FILL_MEM with MEM_TYPE 1, NUM_BYTES 8, FILL_PATTERN #{MMD_PAT_A0}, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")

status_bar("Middle two rows of the dwell packet contain the fill pattern")
wait  # <Go> to continue

###########################################################
## Step 6 = Load memory from dump file to restore values ##
###########################################################

Osk::flight.send_cmd("MM","LOAD_MEM_FROM_FILE with FILENAME #{MMD_FLT_PATH_FILENAME}")

status_bar("The middle two rows in the dwell packet show that memory was restored")
wait  # <Go> to continue

#############
## Cleanup ##
#############

cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME MM, BITMASK 0x01") # Disable debug events
wait (1)
cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME MD, BITMASK 0x01") # Disable debug events
clear("CFS_KIT MEMORY_MGMT_SCREEN")    
clear("MD DWELL_PKT_SCREEN")    
clear("CFS_KIT MEMORY_MGMT_DEMO_SCREEN")
clear("CFS_KIT MEMORY_MGMT_DEMO_INFO_SCREEN")
