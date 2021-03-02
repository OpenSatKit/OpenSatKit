###############################################################################
# FileMgr Copy Function Test Module
#
# Notes:
#   1.  
#
# Requirements Verified: TODO
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
module FILEMGR
   
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
         cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME FILEMGR, BITMASK 0x01") # Enable debug events
         cmd("FILEMGR DELETE_FILE with FILENAME #{@dest_file}")                # Make sure destination doesn't exist
         wait 2
       
         cmd_str = "COPY_FILE with OVERWRITE 0, SRC_FILE #{@src1_file}, DEST_FILE #{@dest_file}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "FILEMGR ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FILEMGR", EVS_DEBUG, "Copied file from #{@src1_file} to #{@dest_file}")
         end

         cmd_str = "COPY_FILE with OVERWRITE 1, SRC_FILE #{@src2_file}, DEST_FILE #{@dest_file}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "FILEMGR ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FILEMGR", EVS_DEBUG, "Copied file from #{@src2_file} to #{@dest_file}")
         end

         cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME FILEMGR, BITMASK 0x01") # Disable debug events

      end
      
      def invalid
         #
         # 1. Invalid overwrite value
         # 2. Source file invalid name
         # 3. Source file doesn't exist
         # 4. Destination file invalid name
         # 5. Destination file exists and overwrite flag is false
         #
         cmd_str = "COPY_FILE with OVERWRITE 2, SRC_FILE #{@src1_file}, DEST_FILE #{@dest_file}"
         test_passed = AppFuncTest.send_cmd(@app, cmd_str, error_case: true) do |app_name, event_type, event_msg|
            raise "FILEMGR ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FILEMGR", EVS_ERROR, "Invalid overwrite flag 2")
         end

         cmd_str = "COPY_FILE with OVERWRITE 0, SRC_FILE #{@src1_file+"|"}, DEST_FILE #{@dest_file}"
         test_passed = AppFuncTest.send_cmd(@app, cmd_str, error_case: true) do |app_name, event_type, event_msg|
            raise "FILEMGR ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FILEMGR", EVS_ERROR, "failed: Source file doesn't exist")
         end

         cmd_str = "COPY_FILE with OVERWRITE 0, SRC_FILE #{@src1_file.sub("txt","aaa")}, DEST_FILE #{@dest_file}"
         test_passed = AppFuncTest.send_cmd(@app, cmd_str, error_case: true) do |app_name, event_type, event_msg|
            raise "FILEMGR ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FILEMGR", EVS_ERROR, "failed: Source file doesn't exist")
         end

         cmd_str = "COPY_FILE with OVERWRITE 0, SRC_FILE #{@src1_file}, DEST_FILE #{@dest_file+"|"}"
         test_passed = AppFuncTest.send_cmd(@app, cmd_str, error_case: true) do |app_name, event_type, event_msg|
            ##raise "FILEMGR ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FILEMGR", EVS_ERROR, "Copy File error: filename is invalid:")
         end

         cmd_str = "COPY_FILE with OVERWRITE 0, SRC_FILE #{@src1_file}, DEST_FILE #{@src2_file}"
         test_passed = AppFuncTest.send_cmd(@app, cmd_str, error_case: true) do |app_name, event_type, event_msg|
            raise "FILEMGR ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("FILEMGR", EVS_ERROR, "failed: Target file exists")
         end

      end
   end # Class FileCopyTest
   
end # Module FILEMGR
end # Module AppFuncTest
