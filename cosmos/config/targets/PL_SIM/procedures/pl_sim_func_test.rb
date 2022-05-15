###############################################################################
# Payload Manager (PL_SIM) Application Functional Test
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
require 'pl_sim_pwr_test'


class PlSimFuncTest < Cosmos::Test

   include AppFuncTest
   
   def initialize
   
      super()
      @app = app_func_test_init("PL_SIM")
      
      @power_test = AppFuncTest::PL_SIM::PowerTest.new(@app)
      
   end

   def setup

      status_bar("setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      
      #TODO - Verify instrument state is off. If not stop/start the app
            
   end # setup()
   

   def teardown
   
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
   
   end # teardown()

   def test_instr_power
      
      status_bar("instrument power tests")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Instrument power table test cases"      
      @power_test.valid
      @power_test.invalid
   
   end # test_instr_power()

   
end # Class PlSimFuncTest

