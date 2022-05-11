###############################################################################
# PL_MGR Configuration Test Module
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
module PL_MGR
   
   class ConfigTest
   
      APP_INI_FILE = "/cf/pl_mgr_ini.json"   
      
      def initialize(app, test_files)
         @app = app
               
      end

      def valid
         
         #
         # These tests verify PL_MGR's configuration parameters. These parameters are initialized
         # by the JSON ini file and can be changed during runtime by the config command.
         #
         # 1. Dump and verify ini table
         # 2. Set and use each configuration parameter
         #      "SCI_FILE_PATH_BASE": "/cf/pl_sci_",
         #      "SCI_FILE_EXTENSION": ".txt",
         #      "SCI_FILE_IMAGE_CNT": 6
         
         # 1. Dump and verify app ini table
         #cmd_str = "DUMP_TBL with ID 0, FILENAME #{DUMP_TBL_FILE}"
         #test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
         #   raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Successfully dumped table 0 to file #{DUMP_TBL_FILE}")
         #end
         
      end # valid
      
      
      def invalid
      
         #
         # TODO
         #

      end # invalid
   end # Class ConfigTest
   
end # Module PL_MGR
end # Module AppFuncTest
