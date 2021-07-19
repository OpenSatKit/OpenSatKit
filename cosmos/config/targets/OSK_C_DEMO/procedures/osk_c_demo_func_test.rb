###############################################################################
# OSK C Demo Application Functional Test
#
# Notes:
#   1. Loaded by OSK test suite
#   2. Includes AppFuncTest for common test utilities
#   3. Serves as an example functional test but it is not formal in the sense 
#      that the test cases do not trace to functional requirements. The
#      functional tests for FileMgr (a refactor of NASA/Goddard's File 
#      Manager(FM) app provides an example of requirements-based test.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'app_func_test'
require 'osk_c_demo_app_test'
require 'osk_c_demo_msglog_test'
require 'osk_c_demo_msglogtbl_test'


class OskCDemoFuncTest < Cosmos::Test
 
 
   include AppFuncTest
   
   def initialize
      
      super()    
      @app = app_func_test_init("OSK_C_DEMO")

      @app_test       = AppFuncTest::OSK_C_DEMO::AppTest.new(@app)
      @msglog_test    = AppFuncTest::OSK_C_DEMO::MsgLogTest.new(@app)
      @msglogtbl_test = AppFuncTest::OSK_C_DEMO::MsgLogTblTest.new(@app)

   end # initialize()
   

   def setup

      status_bar("setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
            
   end # setup()


   def teardown
   
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
   
   end # teardown()


   def test_app
      
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Valid file test cases"      
      @app_test.valid
      @app_test.invalid
   
   end # test_app()

   
   def test_msglog
   
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Valid directory test cases"      
      @msglog_test.valid
      @msglog_test.invalid

   end # test_msglog()


   def test_msglogtbl
   
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Valid directory test cases"      
      @msglogtbl_test.valid
      @msglogtbl_test.invalid

   end # test_msglogtbl()
   
end # Class OskCDemoFuncTest
