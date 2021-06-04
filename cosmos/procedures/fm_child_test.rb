###############################################################################
# File Manager Child Task Test
# 
# Notes:
#   1. There have been reports that the FM main task can get out of synch with
#      the child task.  This test is designed to run forever until an error
#      occurs.
#   2. Look into reporting vs actual synch problem: child tlm copied, rollover
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require 'ccsds'
require 'osk_global'
require 'osk_system'
require 'osk_ops'

###########
## Setup ##
###########

DIR_LIST_FILE = "#{Osk::FLT_SRV_DIR}/#{Osk::TMP_BIN_FILE}"
test_loop = 0

###############
## Test Loop ##
###############

while (test_loop < 10000)

   puts "Test Loop: #{test_loop}\n"
   
   seq_cnt = tlm("FM #{Osk::TLM_STR_HK_PKT} #{Ccsds::PRI_HDR_SEQUENCE}")

   Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_SRV_DIR}, DIR_LIST_OFFSET 0")   
   Osk::flight.send_cmd("FM","WRITE_DIR_TO_FILE with DIRECTORY #{Osk::FLT_SRV_DIR}, FILENAME #{DIR_LIST_FILE}")
 
   wait("FM #{Osk::TLM_STR_HK_PKT} #{Ccsds::PRI_HDR_SEQUENCE} > #{seq_cnt}", 10) # Timeout will accomodate rollover

   app_cmd_valid_cnt   = tlm("FM #{Osk::TLM_STR_HK_PKT} #{Osk::TLM_STR_CMD_VLD}")
   child_cmd_valid_cnt = tlm("FM #{Osk::TLM_STR_HK_PKT} CHILD_CMD_CTR")
   
   if (app_cmd_valid_cnt != child_cmd_valid_cnt)
      prompt ("Command Counter Synch Error: App=#{app_cmd_valid_cnt} Child=#{child_cmd_valid_cnt} ");
   end
   
   test_loop += 1
   
end # Test Loop

 
#############
## Cleanup ##
#############

