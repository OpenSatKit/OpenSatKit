###############################################################################
# Kit Command Ingest (KIT_CI) Application Test
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


class KitCiTest < Cosmos::Test

   def initialize
      super()
   end

   def setup
      status_bar("#{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      @kit_ci = FswApp.new("KIT_CI",  "KIT_CI",  Osk::TLM_STR_HK_PKT, Fsw::MsgId::KIT_CI_CMD_MID)
      wait(2)
   end

   def test_noop_cmd
      status_bar("noop command")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      @kit_ci.send_cmd("NOOP")
   end

   def test_reset_cmd
      status_bar("reset command")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      # Send a coupel of noops to increment the command counters
      @kit_ci.send_cmd("NOOP")
      @kit_ci.send_cmd("NOOP")
      @kit_ci.send_cmd("RESET_CTRS")
   end

   def teardown
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      wait(2)
   end

   def helper_method
   end

end # Class KitCiTest

