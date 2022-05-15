###############################################################################
# Payload Manager (PL_MGR) Application Functional Test
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
require 'pl_mgr_cfg_test'
require 'pl_mgr_sci_test'

class PlMgrFuncTest < Cosmos::Test

   include AppFuncTest
   
   def initialize
   
      super()
      @app = app_func_test_init("PL_MGR")
      
      @science_test = AppFuncTest::PL_MGR::SciTest.new(@app)
      
   end

   def setup

      status_bar("setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      
      #TODO - Verify payload state is off. If not stop/start the app
            
   end # setup()
   

   def teardown
   
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
   
   end # teardown()

   def test_sci_files
      
      status_bar("Payload science file tests")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Instrument power table test cases"      
      @science_test.valid
      @science_test.invalid
   
   end # test_sci_files
   
end # Class PlMgrFuncTest

