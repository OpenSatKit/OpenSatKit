###############################################################################
# Payload Manager Science Test Module
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
   
   class SciTest
   
      
      def initialize(app)

         @app = app
      
      end

      def valid
         
         #
         # These tests verify the valid usage of the instrument science
         # commands. 
         #
         # These test require the Payload Simulator (PL_SIM) but they are not
         # designed to test PL_SIM. Functions like PL_SIM power on/off are
         # exercised only to the intent to verify the science file functioanlity.
         #
         # 1. Enable science
         # 2. Verify science data generation, files names, and file content
         # 3. Disable science
         #
         
         # 1. Enable science
         
         if not ( (tlm("#{@app.target_hk_str} PAYLOAD_POWER") == "OFF")   and
                  (tlm("#{@app.target_hk_str} SCI_FILE_OPEN") == "FALSE") and
                  (tlm("PL_SIM HK_TLM_PKT PAYLOAD_POWER")     == "OFF")   and
                  (tlm("PL_SIM HK_TLM_PKT DETECTOR_FAULT")    == "FALSE"))
         
            raise "PL_MGR Science Test started in non-default state" 
            
         end
         
         cmd("PL_SIM POWER_ON")
         wait("PL_SIM HK_TLM_PKT PAYLOAD_POWER == 'READY'", 25)


         # 2. Verify science data generation, files names, and file content

         cmd_str = "START_SCI_DATA"
         # Two events immediately issued so need event queue utility
         # "Start science data collection accepted"
         # "New science file created: /cf/simsat/pl_sim_001.txt"
         #test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
         #   raise "PL_SIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("PL_SIM", EVS_INFORMATION, "New science file created: /cf/simsat/rec/isim_000.txt")
         #end


         # 3. Disable science

         cmd_str = "STOP_SCI_DATA"
         # Two events immediately issued so need event queue utility
         # "Closed science file /cf/pl_sci_001.txt"
         # "Sucessfully stopped science"
         #test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
         #   raise "PL_SIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("PL_SIM", EVS_INFORMATION, "New science file created: /cf/simsat/rec/isim_000.txt")
         #end

         cmd_str = "PL_SIM POWER_OFF"
         wait("PL_SIM HK_TLM_PKT PAYLOAD_POWER == 'OFF'", 10)

      end # valid
      
      def invalid
      
         #
         # TODO
         #

      end # invalid
   end # Class SciTest
   
end # Module PL_SIM
end # Module AppFuncTest
