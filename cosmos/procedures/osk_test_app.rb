###############################################################################
# OSK Test App Module
#
# Notes:
#   1. Provides OSK specific application test functionality that extends the
#      COSMOS test class. Test that inherit from Cosmos::Test also include
#      this module so it's similar to multiple inheritance. 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################


module OskTestApp

   attr_accessor :app
   
   def osk_test_app_init (target_str,msg_id)
      @app = FswApp.new(target_str, target_str, Osk::TLM_STR_HK_PKT, msg_id)
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

end # Module OskTestApp

