###############################################################################
# Kit Scheduler (KIT_SCH) Application Functional Test
#
# Notes:
#   1. Loaded by kit test suite so it can assume common utilities have been 
#      preloaded.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'app_func_test'

class KitSchFuncTest < Cosmos::Test
 
   include AppFuncTest
    
   def initialize
      super()
      @app = app_func_test_init("KIT_SCH")
   end

   def setup
      status_bar("setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      wait(2)
   end

   def teardown
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      wait(2)
   end

   def test_sch_tbl_entry_cmds
      # CFG_SCH_TBL_ENTRY
      # LOAD_SCH_TBL_ENTRY
      # SEND_SCH_TBL_ENTRY
      Osk::flight.send_cmd("KIT_SCH","#{Osk::CMD_STR_RESET}")
   end
   
   def test_msg_tbl_entry_cmds
      # LOAD_MSG_TBL_ENTRY
      # SEND_MSG_TBL_ENTRY
      Osk::flight.send_cmd("KIT_SCH","#{Osk::CMD_STR_RESET}")
   end

   def helper_method
   end
   
end # Class KitSchFuncTest
