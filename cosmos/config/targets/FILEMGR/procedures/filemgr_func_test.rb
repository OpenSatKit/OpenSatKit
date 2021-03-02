###############################################################################
# File Manager (FileMgr) Application Functional Test
#
# Notes:
#   1. Loaded by OSK test suite
#   2. Includes AppFuncTest for common test utilities
#   3. FileMgr is a refactor NASA's File Manager (FM) app and implements the
#      same requirements
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'app_func_test'
require 'filemgr_file_copy_test'


class FileMgrFuncTest < Cosmos::Test
 
   SRC_TEST_FILES = {src1: "filemgr_test_src1.txt", src2: "filemgr_test_src2.txt", src_gz: "filemgr_test_src.gz"}
   
   include AppFuncTest
   
   def initialize
      
      super()    
      @app = app_func_test_init("FILEMGR")

      @file_copy_test = AppFuncTest::FILEMGR::FileCopyTest.new(@app, SRC_TEST_FILES)

   end # initialize()
   

   def setup

      status_bar("setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      
      if (not load_test_files(SRC_TEST_FILES))
         raise "FILEMGR ERROR: Failed to load test files"
      end
      
   end # setup()


   def teardown
   
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
   
   end # teardown()


   def test_file
      
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Valid file test cases"      
      @file_copy_test.valid
      @file_copy_test.invalid
   
   end # test_file()

   
   def test_dir
   
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Valid directory test cases"      

   end # test_dir()


   def test_file_sys_tbl
   
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - Valid file system test cases"      

   end # test_file_sys_tbl()
   
end # Class FileMgrFuncTest
