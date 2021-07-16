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
   
      def initialize(app)
         @app = app
      end

      def valid
      
         #
         # 1. Dump default table, transfer and veriy values equal osk_defs defaults
         # 2. Load dumped default table to verify 
         #         

         cmd_str = "DUMP_TBL with ID #{OskCDemo::MSGLOG_TBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{OskCDemo::INI_TBL_DUMP_FILE}"
         test_passed = AppFuncTest.send_cmd(@app, cmd_str, expect_event: false)

      end
      
      def invalid
         
         #
         # TODO
         #

      end
   end # Class MsgLogTblTest
   
end # Module OSK_C_DEMO
end # Module AppFuncTest
