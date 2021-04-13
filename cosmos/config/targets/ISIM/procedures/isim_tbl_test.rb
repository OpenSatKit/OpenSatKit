###############################################################################
# ISIM Table Test Module
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
module ISIM
   
   class TblTest
   
      DEFAULT_TBL_FILE = "/cf/isim_tbl.json"   
      DUMP_TBL_FILE    = "/cf/isim_tbl~.json"
      
      def initialize(app, test_files)
         @app = app
         
         @test_replace_tbl          = File.join(Osk::FLT_TEST_DIR,test_files[:replace])
         @test_update_sci_instr_tbl = File.join(Osk::FLT_TEST_DIR,test_files[:update_sci_instr])
         @test_update_sci_file_tbl  = File.join(Osk::FLT_TEST_DIR,test_files[:update_sci_file])
      
      end

      def valid
         
         #
         # These tests verify the table functionality. Other tests verify the table parameters
         # are propoerly used. Default table values must be restored when the test exists.
         #
         # 1. Dump and verify default table
         # 2. Load the default dump table to verify dump format is valid         
         # 3. Load/dump the entire test table
         # 4. Update the sci-instr JSON object
         # 5. Update the sci-file JSON object
         #
         
         # 1. Dump and verify default table
         cmd_str = "DUMP_TBL with ID 0, FILENAME #{DUMP_TBL_FILE}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Successfully dumped table 0 to file #{DUMP_TBL_FILE}")
         end
         
         wait("ISIM HK_TLM_PKT LAST_TBL_ACTION == 'Fsw:Const::OSK_TBLMGR_ACTION_DUMP'",  8)
         wait("ISIM HK_TLM_PKT LAST_TBL_STATUS == 'Fsw:Const::OSK_TBLMGR_STATUS_VALID'", 8)

         #TODO - Verify last table action state for all tests         
         #TODO - Verify default contents
         
         # 2. Load the default dump table to verify dump format is valid         
         cmd_str = "LOAD_TBL with ID 0, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{DUMP_TBL_FILE}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Successfully replaced table 0 using file #{DUMP_TBL_FILE}")
         end
         
         # 3. Load/dump the entire test table
         cmd_str = "LOAD_TBL with ID 0, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{@test_replace_tbl}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Successfully replaced table 0 using file #{@test_replace_tbl}")
         end

         cmd_str = "DUMP_TBL with ID 0, FILENAME #{DUMP_TBL_FILE}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Successfully dumped table 0 to file #{DUMP_TBL_FILE}")
         end

         #TODO - Verify contents

         # 4. Update the sci-instr JSON object
         cmd_str = "LOAD_TBL with ID 0, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_UPDATE}, FILENAME #{@test_update_sci_instr_tbl}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Successfully updated table 0 using file #{@test_update_sci_instr_tbl}")
         end

         cmd_str = "DUMP_TBL with ID 0, FILENAME #{DUMP_TBL_FILE}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Successfully dumped table 0 to file #{DUMP_TBL_FILE}")
         end

         #TODO - Verify contents

         # 5. Update the sci-file JSON object
         cmd_str = "LOAD_TBL with ID 0, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_UPDATE}, FILENAME #{@test_update_sci_file_tbl}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Successfully updated table 0 using file #{@test_update_sci_file_tbl}")
         end

         cmd_str = "DUMP_TBL with ID 0, FILENAME #{DUMP_TBL_FILE}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Successfully dumped table 0 to file #{DUMP_TBL_FILE}")
         end

         #TODO - Verify default contents

         # Restore defaults
         
         cmd_str = "LOAD_TBL with ID 0, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{DEFAULT_TBL_FILE}"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Successfully replaced table 0 using file #{DEFAULT_TBL_FILE}")
         end

      end # valid
      
      def invalid
      
         #
         # TODO
         #

      end # invalid
   end # Class TblTest
   
end # Module ISIM
end # Module AppFuncTest
