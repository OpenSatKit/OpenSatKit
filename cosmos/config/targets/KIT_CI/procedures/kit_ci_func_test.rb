###############################################################################
# Kit Command Ingest (KIT_CI) Application Functional Test
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
require 'kit_ci_msg_tunnel_test'

class KitCiFuncTest < Cosmos::Test

   include AppFuncTest
   
   def initialize
      super()
      @app = app_func_test_init("KIT_CI")
      @msg_tunnel_test = AppFuncTest::KIT_CI::MsgTunnelTest.new(@app)
   end

   def setup
      status_bar("#{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      wait(2)
   end

   def teardown
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      wait(2)
   end

   def test_uplink
      
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Uplink"      
      @file_copy_test.valid
      @file_copy_test.invalid
   
   end # test_file()


end # Class KitCiFuncTest

