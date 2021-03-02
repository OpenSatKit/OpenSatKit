###############################################################################
# File Manager (FM) Application Functional Test
#
# Notes:
#   1. Loaded by cFS suite so it can assume common utilities have been 
#      preloaded.
#   2. This is not a direct rehost of the NASA FM test procedures. Those
#      procedures were consulted but a new test structure was created to
#      optimize the tests using the OSK environment.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'app_func_test'
require 'fm_file_copy_test'


class FmFuncTest < Cosmos::Test
 
   SRC_TEST_FILES = {src1: "fm_test_src1.txt", src2: "fm_test_src2.txt", src_gz: "fm_test_src.gz"}
   
   include AppFuncTest
   
   def initialize
      
      super()    
      @app = app_func_test_init("FM")

      @file_copy_test = AppFuncTest::FM::FileCopyTest.new(@app, SRC_TEST_FILES)

   end # initialize()
   

   def setup

      status_bar("setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      
      if (not load_test_files(SRC_TEST_FILES))
         raise "FM ERROR: Failed to load test files"
      end
      
   end # setup()


   def teardown
   
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
   
   end # teardown()


   def test_file
      
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Pass test cases"      
      @file_copy_test.valid
      @file_copy_test.invalid     
   
   end # test_file()

   
   def test_dir
   
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Pass test cases"      

   end # test_dir()


   def test_file_sys_tbl
   
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Pass test cases"      

   end # test_file_sys_tbl()
   
end # Class FmFuncTest
