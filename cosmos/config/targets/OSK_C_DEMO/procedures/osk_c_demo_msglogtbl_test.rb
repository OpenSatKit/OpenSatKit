###############################################################################
# OSK C Demo Message Log Table Test Module
#
# Notes:
#   1. See osk_c_demo_func_test prologue.  
#
# Requirements Verified: Not Applicable
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
require 'osk_c_demo_const'
require 'app_func_test'

module AppFuncTest
module OSK_C_DEMO
   
   class MsgLogTblTest
   
      def initialize(app, test_files, gnd_test_files_dir)
         @app = app

         @flt_test_tbl_file = File.join(Osk::FLT_TEST_DIR,test_files[:tbl])
         @gnd_test_tbl_file = File.join(gnd_test_files_dir,test_files[:tbl])
      end

      def valid
        
         #
         # 1. Transfer and load test table file
         # 2. Dump test table, transfer and verify values equal original test file
         # 3. Load dumped test table to verify dump creates a valid table
         #

         test_passed = false

         if Osk::Ops::put_flt_file(@gnd_test_tbl_file,@flt_test_tbl_file)
 
            cmd_str = "LOAD_TBL with ID #{OskCDemo::MSGLOG_TBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{@flt_test_tbl_file}"
            if AppFuncTest.send_cmd(@app, cmd_str, expect_event: false)
         
               cmd_str = "DUMP_TBL with ID #{OskCDemo::MSGLOG_TBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{TMP_FLT_JSON_PATH_FILE}"
               if AppFuncTest.send_cmd(@app, cmd_str, expect_event: false)
      
                  if Osk::Ops::get_flt_file(TMP_FLT_JSON_PATH_FILE, TMP_GND_JSON_PATH_FILE)

                     test_tbl_file = File.read(@gnd_test_tbl_file)
                     test_tbl_hash = JSON.parse(test_tbl_file)
                     test_tbl_json_file_obj = test_tbl_hash["file"]

                     test_tbl_dmp_file = File.read(TMP_FLT_JSON_PATH_FILE)
                     test_tbl_dmp_hash = JSON.parse(test_tbl_dmp_file)
                     test_tbl_dmp_json_file_obj = test_tbl_dmp_hash["file"]
                     if ( test_tbl_hash["playbk-delay"].eql?            test_tbl_dmp_hash["playbk-delay"]            and
                          test_tbl_json_file_obj["path-base-name"].eql? test_tbl_dmp_json_file_obj["path-base-name"] and
                          test_tbl_json_file_obj["extension"].eql?      test_tbl_dmp_json_file_obj["extension"]      and
                          test_tbl_json_file_obj["entry-cnt"].eql?      test_tbl_dmp_json_file_obj["entry-cnt"])
                        
                        test_passed = true
                     
                     end
                     
                  end # get_flt_file
               end # send dump command
            end # send load command
      
            if (test_passed)
               
               test_passed = false
               cmd_str = "LOAD_TBL with ID #{OskCDemo::MSGLOG_TBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{TMP_FLT_JSON_PATH_FILE}"
               if AppFuncTest.send_cmd(@app, cmd_str, expect_event: false)
                  test_passed = true        
               end
               
            end
         
         end # put_flt_file
         
         return test_passed
      
      end # valid
            
      def invalid
         
         #
         # TODO
         #

      end # invalid
      
   end # Class MsgLogTblTest
   
end # Module OSK_C_DEMO
end # Module AppFuncTest
