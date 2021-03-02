###############################################################################
# App Functional Test Module
#
# Notes:
#   1. Provides common test methods for use in app functional tests. Functional
#      test inherit from Cosmos::Test so including this module is similar to 
#      multiple inheritance or Java interfaces.
#   2. "test_" methods are automatically added to the test suite. Other methods
#      are explicitly called as needed. 
#
# Requirements Verified:
#   1002: 
#   1003: 
#   1004: 
#   1005: 
#   1006: 
#   1007: 
#   1008: 
#   2002: 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'osk_global'
require 'osk_ops'
require 'fsw_const'
require 'fsw_config_param'
require 'app_func_test'

module AppFuncTest
module FM
   
   class FileCopyTest
   
      def initialize(app, test_files)
         @app = app
         @src1_file = File.join(Osk::FLT_TEST_DIR,test_files[:src1])
         @src2_file = File.join(Osk::FLT_TEST_DIR,test_files[:src2])
         @dest_file = AppFuncTest::FLT_TMP_TEST_FILE
      end

      def valid
         #
         # With debug on
         # 1. Overwrite off, same file system  
         # 2. Overwrite on, same file system 
         # 3. TODO - Different file system
         #
         cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME FM, BITMASK 0x01") # Enable debug events
         cmd("FM DELETE_FILE with FILENAME #{@dest_file}")                # Make sure target doesn't exist
         wait 2
       
         cmd_str = "COPY_FILE with OVERWRITE 0, SOURCE #{@src1_file}, TARGET #{@dest_file}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "FM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FM", EVS_DEBUG, "Copy File command: src = #{@src1_file}, tgt = #{@dest_file}")
         end

         cmd_str = "COPY_FILE with OVERWRITE 1, SOURCE #{@src2_file}, TARGET #{@dest_file}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "FM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FM", EVS_DEBUG, "Copy File command: src = #{@src2_file}, tgt = #{@dest_file}")
         end

         cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME FM, BITMASK 0x01") # Disable debug events

      end # valid()
      
      def invalid
         #
         # 1. Invalid overwrite value
         # 2. Source file invalid name
         # 3. Source file doesn't exist
         # 4. Target file invalid name
         # 5. Target file exists and overwrite flag is false
         #
         cmd_str = "COPY_FILE with OVERWRITE 2, SOURCE #{@src1_file}, TARGET #{@dest_file}"
         test_passed = AppFuncTest.send_cmd(@app, cmd_str, error_case: true) do |app_name, event_type, event_msg|
            raise "FM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FM", EVS_ERROR, "Copy File error: invalid overwrite = 2")
         end

         cmd_str = "COPY_FILE with OVERWRITE 0, SOURCE #{@src1_file+"|"}, TARGET #{@dest_file}"
         test_passed = AppFuncTest.send_cmd(@app, cmd_str, error_case: true) do |app_name, event_type, event_msg|
            raise "FM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FM", EVS_ERROR, "Copy File error: filename is invalid:")
         end

         cmd_str = "COPY_FILE with OVERWRITE 0, SOURCE #{@src1_file.sub("txt","aaa")}, TARGET #{@dest_file}"
         test_passed = AppFuncTest.send_cmd(@app, cmd_str, error_case: true) do |app_name, event_type, event_msg|
            raise "FM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FM", EVS_ERROR, "Copy File error: file does not exist:")
         end

         cmd_str = "COPY_FILE with OVERWRITE 0, SOURCE #{@src1_file}, TARGET #{@dest_file+"|"}"
         test_passed = AppFuncTest.send_cmd(@app, cmd_str, error_case: true) do |app_name, event_type, event_msg|
            raise "FM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FM", EVS_ERROR, "Copy File error: filename is invalid:")
         end

         cmd_str = "COPY_FILE with OVERWRITE 0, SOURCE #{@src1_file}, TARGET #{@src2_file}"
         test_passed = AppFuncTest.send_cmd(@app, cmd_str, error_case: true) do |app_name, event_type, event_msg|
            raise "FM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FM", EVS_ERROR, "Copy File error: file already exists:")
         end

      end # invalid()
   end # Class FileCopyTest
   
end # Module FM
end # Module AppFuncTest
