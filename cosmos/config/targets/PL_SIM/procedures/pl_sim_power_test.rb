###############################################################################
# Payload Simulator Power Test Module
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
module PL_SIM
   
   class PowerTest
   
      
      def initialize(app)

         @app = app
      
      end

      def valid
         
         #
         # These tests verify the valid usage of the instrument power commands. Some other 
         # functions like set/clear fault are exercised only to the intent to verify the
         # power behavior.
         #
         # 1. Power on sequence
         # 2. Power reset
         # 3. Power off
         #
         
         # 1. Power on
         
         if not ( (tlm("#{@app.target_hk_str} PAYLOAD_POWER")  == "OFF") and
                  (tlm("#{@app.target_hk_str} DETECTOR_FAULT") == "FALSE"))
         
            raise "PL_SIM Power Test started in non-default state" 
            
         end
         
         cmd_str = "POWER_ON"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "PL_SIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("PL_SIM", EVS_INFORMATION, "PL_SIM power transitioned from OFF to INITIALIZING")
         end
         
         wait("PL_SIM HK_TLM_PKT POWER_INIT_CYCLE_CNT > 0",  10)
         wait("PL_SIM HK_TLM_PKT PAYLOAD_POWER == 'INIT'",   10)
         wait("PL_SIM HK_TLM_PKT POWER_RESET_CYCLE_CNT == 0", 0)
         wait("PL_SIM HK_TLM_PKT PAYLOAD_POWER == 'READY'",  25)
         wait("PL_SIM HK_TLM_PKT DETECTOR_FAULT == 'FALSE'",  0)

         # 2. Power reset

         cmd_str = "SET_FAULT"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "PL_SIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("PL_SIM", EVS_INFORMATION, "Payload fault set to TRUE")
         end
         wait("PL_SIM HK_TLM_PKT DETECTOR_FAULT == 'TRUE'", 10) 

         cmd_str = "POWER_RESET_INSTR"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "PL_SIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("PL_SIM", EVS_INFORMATION, "PL_SIM power transitioned from READY to RESETTING")
         end
         
         wait("PL_SIM HK_TLM_PKT POWER_RESET_CYCLE_CNT > 0", 10)
         wait("PL_SIM HK_TLM_PKT PAYLOAD_POWER == 'RESET'",  10)
         wait("PL_SIM HK_TLM_PKT POWER_INIT_CYCLE_CNT == 0",  0)
         wait("PL_SIM HK_TLM_PKT PAYLOAD_POWER == 'READY'",  25)
         wait("PL_SIM HK_TLM_PKT DETECTOR_FAULT == 'FALSE'",  0)

         # 2. Power off

         cmd_str = "POWER_OFF_INSTR"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "PL_SIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("PL_SIM", EVS_INFORMATION, "PL_SIM power transitioned from READY to OFF")
         end
         wait("PL_SIM HK_TLM_PKT PAYLOAD_POWER == 'OFF'", 10)

      end # valid
      
      def invalid
      
         #
         # TODO
         #

      end # invalid
   end # Class PowerTest
   
end # Module PL_SIM
end # Module AppFuncTest
