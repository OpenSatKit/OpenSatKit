###############################################################################
# cFE Table Service tutorial top-level script
# 
# Notes:
#   1. This tutorial is part of the cFE Service training module and implements
#      the TBL exercises.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_ops'

###########
## Setup ##
###########

# MM_AppData+256 is a safe area in MM's global data structure that can be used
# as a working buffer during the demo

MM_SYMBOL = "MM_AppData"
MM_OFFSET_W0 = 256
MM_OFFSET_W1 = 260
MM_OFFSET_W2 = 264
MM_OFFSET_W3 = 268

MD_TBL1_FILE = "md_tbl1.dat"
MD_TBL_PATH_FILE = "#{Osk::FLT_SRV_DIR}/#{MD_TBL1_FILE}"

Osk::System.check_n_start_cfs('cfsat')


###################################
## TBL01 - Review table registry ##
###################################

wait #TBL01 - Click <Go> to table registry log to a file and display it in Table Manager

Osk::Ops::send_flt_bin_file_cmd("CFE_TBL", "WRITE_REG_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_TBL_REG)
puts "TBL01 - Review table registry file displayed in Table Manager"

wait #TBL01 - Click <Go> to continue to next section

#######################################################
## TBL02 - Introduce Memory Dwell (MD) app and table ##
#######################################################

wait #TBL02 - Click <Go> to jam MD table #1

Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 1, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MM_OFFSET_W0}, ADDR_SYMBOL_NAME #{MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 2, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MM_OFFSET_W1}, ADDR_SYMBOL_NAME #{MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 3, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MM_OFFSET_W2}, ADDR_SYMBOL_NAME #{MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 4, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MM_OFFSET_W3}, ADDR_SYMBOL_NAME #{MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MD","SET_SIGNATURE with TABLE_ID 1, PAD_16 0, SIGNATURE 'cFE Table Exercise'")
wait 1

Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x01020304, ADDR_OFFSET #{MM_OFFSET_W0}, ADDR_SYMBOL_NAME #{MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x05060708, ADDR_OFFSET #{MM_OFFSET_W1}, ADDR_SYMBOL_NAME #{MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0X090A0B0C, ADDR_OFFSET #{MM_OFFSET_W2}, ADDR_SYMBOL_NAME #{MM_SYMBOL}")
wait 1
Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x0D0E0F10, ADDR_OFFSET #{MM_OFFSET_W3}, ADDR_SYMBOL_NAME #{MM_SYMBOL}")

display("MD DWELL_PKT_SCREEN",50,50)  
Osk::flight.send_cmd("MD","START_DWELL with TABLE_MASK 0x0001")
            
puts "TBL02 - Review MD functionality"
wait #TBL02 - Click <Go> to continue to next section

############################################
## TBL03 - Perform MD table loads & dumps ##
############################################

wait #TBL03 - Click <Go> to attempt to dump inactive dwell table
display("CFS_KIT TABLE_MGMT_SCREEN",1500,50)  

Osk::flight.cfe_tbl.send_cmd("DUMP_TBL with ACTIVE_TBL_FLAG 0, TABLE_NAME MD.DWELL_TABLE1, FILENAME #{Osk::TMP_FLT_BIN_PATH_FILE}")

wait #TBL03 - Click <Go> to dump active dwell table

cmd_str = "DUMP_TBL with ACTIVE_TBL_FLAG 1, TABLE_NAME MD.DWELL_TABLE1, "
Osk::Ops::send_flt_bin_file_cmd("CFE_TBL", cmd_str, "Definition file for #{Osk::TBL_MGR_DEF_MD_TBL}", flt_path_filename: MD_TBL_PATH_FILE, gnd_rel_path: Osk::REL_SRV_TBL_DIR)

wait #TBL03 - Click <Go> to load inactive dwell table

# No need to transfer file since it was dumped in the previous step
Osk::flight.cfe_tbl.send_cmd("LOAD_TBL with FILENAME #{MD_TBL_PATH_FILE}")


wait #TBL03 - Click <Go> to attempt to activate table
Osk::flight.cfe_tbl.send_cmd("ACTIVATE_TBL with TABLE_NAME MD.DWELL_TABLE1")

wait #TBL03 - Click <Go> to validate and activate table
Osk::flight.cfe_tbl.send_cmd("VALIDATE_TBL with ACTIVE_TBL_FLAG 0, TABLE_NAME MD.DWELL_TABLE1")
wait 2
Osk::flight.cfe_tbl.send_cmd("ACTIVATE_TBL with TABLE_NAME MD.DWELL_TABLE1")

wait #TBL03 - Click <Go> to continue to next section

#############
## Cleanup ##
#############

clear("CFS_KIT CFE_SCREEN")
clear("MD DWELL_PKT_SCREEN")
clear("CFS_KIT TABLE_MGMT_SCREEN")
