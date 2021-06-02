###############################################################################
# Instrument Simulator (ISIM) Application Functional Test
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
require 'isim_tbl_test'
require 'isim_pwr_test'
require 'isim_sci_test'

class IsimFuncTest < Cosmos::Test

   TBL_TEST_FILES = {replace: "isim_test_tbl.json", update_sci_instr: "isim_test_tbl_instr.json", update_sci_file: "isim_test_tbl_file.json"}

   include AppFuncTest
   
   def initialize
   
      super()
      @app = app_func_test_init("ISIM")
      
      @isim_tbl_test = AppFuncTest::ISIM::TblTest.new(@app, TBL_TEST_FILES)
      @isim_pwr_test = AppFuncTest::ISIM::PwrTest.new(@app)
      @isim_sci_test = AppFuncTest::ISIM::SciTest.new(@app)
      
   end

   def setup

      status_bar("setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      
      #TODO - Verify instrument state is off. If not stop/start the app
      
      if (not load_test_files(TBL_TEST_FILES))
         raise "ISIM ERROR: Failed to load test table files"
      end
      
   end # setup()
   

   def teardown
   
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
   
   end # teardown()

   def test_table
      
      status_bar("table tests")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Table test cases"
      @isim_tbl_test.valid
      @isim_tbl_test.invalid
   
   end # test_table()

   def test_instr_pwr
      
      status_bar("instrument power tests")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Instrument power table test cases"      
      @isim_pwr_test.valid
      @isim_pwr_test.invalid
   
   end # test_instr_pwr()

   def test_instr_sci
      
      status_bar("instrument science tests")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Instrument power table test cases"      
      @isim_sci_test.valid
      @isim_sci_test.invalid
   
   end # test_instr_sci()
   
end # Class IsimFuncTest

