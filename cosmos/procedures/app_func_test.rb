###############################################################################
# App Functional Test Module
#
# Notes:
#   1. Provides common test methods for use in app functional tests. Functional
#      test inherit from Cosmos::Test so including this module is similar to 
#      multiple inheritance or Java interfaces.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'osk_global'
require 'fsw_const'
require 'fsw_config_param'

module AppFuncTest

   attr_accessor :app
   
   def app_func_test_init (target_str,msg_id)
      @app = FswApp.new(target_str, target_str, Osk::TLM_STR_HK_PKT, msg_id)
      return @app
   end

   def test_noop_cmd
      status_bar("noop command")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      @app.send_cmd("NOOP")
   end

   def test_reset_cmd
      status_bar("reset command")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      # Send a couple of noops to increment the command counters
      @app.send_cmd("NOOP")
      @app.send_cmd("NOOP")
      @app.send_cmd("RESET_CTRS")
   end

end # Module AppFuncTest

