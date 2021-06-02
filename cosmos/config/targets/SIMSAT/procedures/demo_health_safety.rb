################################################################################
# SimSat Health & Safety App Group Demo 
#
# Notes:
#   1. Developed for the YouTube Health & Safety App Group training video. This
#      demo uses the default SimSat configuration.
#      
# Demos:
#   1. HS Mon F42 execution and issue event when F42 fails to increment its counter
#   2. HS Mon F42 app critical event & respond by restarting F42
#   3. CS verifying HS's AppMon table, corrupt the table, and see CS response
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
require 'fsw_config_param'

#
# This demo intentionally corrupts a table being checksummed by CS so the 
# location of the table needs to be determined.
# These are the HS_AppData offsets to access the pointers to the first 
# data entry for each table. These offsets are valid for HS 2.3.0 using
# the compiler settings delivered with OSK.
#

HS_APP_DATA_SYM = "HS_AppData"

HS_XCT_TBL_OFFSET = 536   # HS_XCTEntry_t   *XCTablePtr;  Execution Counters Table
HS_AMT_TBL_OFFSET = 540   # HS_AMTEntry_t   *AMTablePtr;  Apps Monitor Table
HS_EMT_TBL_OFFSET = 544   # HS_EMTEntry_t   *EMTablePtr;  Events Monitor Table
HS_MAT_TBL_OFFSET = 548   # HS_MATEntry_t   *MATablePtr;  Message Actions Table

HS_AMT_TBL_ENTRY_0 = "CFE_ES" # First app name in default tables that use apps names 
                              # "CFE_ES" = ASCII: 0x43 0x46 0x45 0x5F 0x45 0x53
DEMO_TBL_CORRUPT_CHAR = "%"   # "%" = ASCII 0x25
DEMO_CORRUPT_DATA = "0x2525"

I42_SCH_TBL_EXE1_SLOT     = 0
I42_SCH_TBL_EXE1_ACTIVITY = 0
I42_SCH_TBL_EXE2_SLOT     = 2
I42_SCH_TBL_EXE2_ACTIVITY = 0

F42_SCH_TBL_HK_SLOT       = 3
F42_SCH_TBL_HK_ACTIVITY   = 9


######################
## Global Variables ##
######################


###########
## Setup ##
###########

# Configure system 

Osk::System.check_n_start_cfs('simsat')

# Configure apps - Done on a per step basis

#TODO Verify CS and HS in proper configuration


# Open displays -  Done on a per step basis

display("HS HS_MGMT_SCREEN",1500,50)

###########################################################################################
## Demo 1 - HS Mon F42 execution and issue event when F42 fails to increment its counter ##
###########################################################################################

status_bar("Pause before starting demo")
wait  # <Go> to continue

# Step 1.1 - Dump HS's App Mon Table source code

Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::OSK_CFS_DIR}/apps/hs/fsw/tables/hs_amt.c'")
status_bar("Observe the entry for F42 not executing. The action is to send an event message")
wait  # <Go> to continue

# Step 1.2 - Prevent F42's Execution & Observe HS App Mon Response

Osk::flight.send_cmd("KIT_SCH", "CFG_SCH_TBL_ENTRY with SLOT #{I42_SCH_TBL_EXE1_SLOT}, ACTIVITY #{I42_SCH_TBL_EXE1_ACTIVITY}, ENABLED 0")
wait (1)
Osk::flight.send_cmd("KIT_SCH", "CFG_SCH_TBL_ENTRY with SLOT #{I42_SCH_TBL_EXE2_SLOT}, ACTIVITY #{I42_SCH_TBL_EXE2_ACTIVITY}, ENABLED 0")
wait (1)
Osk::flight.send_cmd("KIT_SCH", "CFG_SCH_TBL_ENTRY with SLOT #{F42_SCH_TBL_HK_SLOT},   ACTIVITY #{F42_SCH_TBL_HK_ACTIVITY},   ENABLED 0")
wait (1)

status_bar("F42 execution stopped by disabling scheduler table entries. HS sends an event message after F42 doesn't execute for 8 cycles")
wait  # <Go> to continue

Osk::flight.send_cmd("KIT_SCH", "CFG_SCH_TBL_ENTRY with SLOT #{I42_SCH_TBL_EXE1_SLOT}, ACTIVITY #{I42_SCH_TBL_EXE1_ACTIVITY}, ENABLED 1")
wait (1)
Osk::flight.send_cmd("KIT_SCH", "CFG_SCH_TBL_ENTRY with SLOT #{I42_SCH_TBL_EXE2_SLOT}, ACTIVITY #{I42_SCH_TBL_EXE2_ACTIVITY}, ENABLED 1")
wait (1)
Osk::flight.send_cmd("KIT_SCH", "CFG_SCH_TBL_ENTRY with SLOT #{F42_SCH_TBL_HK_SLOT},   ACTIVITY #{F42_SCH_TBL_HK_ACTIVITY},   ENABLED 1")
wait (1)

status_bar("Demo 1 complete")
wait  # <Go> to continue


########################################################################
## Demo 2 - HS Mon F42 app critical event & respond by restarting F42 ##
########################################################################

# Step 2.1 - Dump HS's Event Mon Table source code

Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::OSK_CFS_DIR}/apps/hs/fsw/tables/hs_emt.c'")
status_bar("Observe F42 entry with the event ID for F42 terminating. The action is to restart the application")
wait  # <Go> to continue


# Step 2.2 - Send F42 control mode command with special mode value that will generate the event being monitored by HS
Osk::flight.send_cmd("F42", "SET_CTRL_MODE with NEW_MODE 99")

status_bar("F42 terminated by command. HS restarted F42 in response to its termination event message. Note cFS/Linux limitation.")
wait  # <Go> to continue

# App restart doesn't work on Linux so reload F42
Osk::flight.send_cmd("CFE_ES", "START_APP with APP_NAME 'F42', APP_ENTRY_POINT 'F42_AppMain', APP_FILENAME '/cf/f42.so', STACK_SIZE 16384, EXCEPTION_ACTION 0, PRIORITY 0")
wait (2)

status_bar("Demo 2 complete")
wait  # <Go> to continue


#####################################################################################
## Demo 3 - CS verifying HS's AppMon table, corrupt the table, and see CS response ##
#####################################################################################


# Step 3.1 - Dump CS's Tables Table source code

Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::OSK_CFS_DIR}/apps/cs/fsw/tables/cs_tablestbl.c'")

status_bar("Observe that each HS table is being checksummed")
wait  # <Go> to continue


# Step 3.2 - Setup prior to corrupting HS AppMon table 

clear("HS HS_MGMT_SCREEN")
display("CS CS_MGMT_SCREEN",1000,150)

status_bar("Select CS screen's 'Table Tbl' tab and note current status")
wait  # <Go> to continue


# Step 3.3 - Corrupt HS AppMon table 

# Get address of HS AMT. Peek event contains a string formatted as follows: "Peek Command: Addr = 0xF5CBA39C Size = 32 bits Data = 0x5673A228"
Osk::flight.send_cmd("MM","PEEK_MEM with DATA_SIZE  32, MEM_TYPE 1, ADDR_OFFSET #{HS_AMT_TBL_OFFSET}, ADDR_SYMBOL_NAME #{HS_APP_DATA_SYM}")
wait (1)

evt_msg = tlm("CFE_EVS EVENT_MSG_PKT MESSAGE")
amt_tbl_data_addr = evt_msg[evt_msg.rindex("0x"),10]

Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 16, MEM_TYPE 1, DATA #{DEMO_CORRUPT_DATA}, ADDR_OFFSET #{amt_tbl_data_addr}, ADDR_SYMBOL_NAME ''")

status_bar("Wait for HS.AppMon_Tbl checksum failure. This could take severals seconds")
wait  # <Go> to continue

# Step 3.4 - Confirm currupted table 

# Dump App Monitor table and look at the corrupted table values
Osk::Ops::send_cfe_dump_tbl_cmd("HS.#{FswConfigParam::HS_APP_MON_TBL_NAME}", Osk::TBL_MGR_DEF_HS_AMT_TBL)

status_bar("Wait for HS.AppMon_Tbl checksum failure. This could take severals seconds")
wait  # <Go> to continue

# Step 3.5 - Re-baseline table checksum. This would only be done if a tableis intentionally changed 

# Computing a new baseline checksum prevents the checksum from failing.
#Osk::flight.send_cmd("CS","DIS_TABLES_TBL_ENTRY with TBL_NAME HS.#{FswConfigParam::HS_APP_MON_TBL_NAME}")
#wait
Osk::flight.send_cmd("CS","COMPUTE_TBL_BASELINE with TBL_NAME HS.#{FswConfigParam::HS_APP_MON_TBL_NAME}")

status_bar("Note new baseline checksum is what was reported in the previous failure message")
wait  # <Go> to continue


#############
## Cleanup ##
#############

clear("CS CS_MGMT_SCREEN")   

