###############################################################################
# ISIM Instrument Power Test Module
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
   
   class PwrTest
   
      
      def initialize(app)

         @app = app
      
      end

      def valid
         
         #
         # These tests verify the valid usage of the instrument power commands. Some other 
         # functions like set/clear fault are exercised only to the intent to verify the
         # power behavior. Other tests like the science test will completely verify these
         # functions.         
         #
         # 1. Power on sequence
         # 2. Power reset
         # 3. Power off
         #
         
         # 1. Power on
         
         if not ( (tlm("#{@app.target_hk_str} INSTR_PWR_STATE") == "OFF") and
                  (tlm("#{@app.target_hk_str} SCI_STATE")       == "OFF")    and
                  (tlm("#{@app.target_hk_str} FAULT_PRESENT")   == "FALSE"))
         
            raise "ISIM Power Test started in non-default state" 
            
         end
         
         cmd_str = "POWER_ON_INSTR"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Science instrument powered on")
         end
         
         wait("ISIM HK_TLM_PKT PWR_INIT_CYCLE_CNT > 0",     10)
         wait("ISIM HK_TLM_PKT INSTR_PWR_STATE == 'INIT'",  10)
         wait("ISIM HK_TLM_PKT PWR_RESET_CYCLE_CNT == 0",    0)
         wait("ISIM HK_TLM_PKT INSTR_PWR_STATE == 'READY'", 25)
         wait("ISIM HK_TLM_PKT SCI_STATE == 'OFF'",          0)
         wait("ISIM HK_TLM_PKT FAULT_PRESENT == 'FALSE'",    0)

         # 2. Power reset

         cmd_str = "SET_FAULT"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Science instrument fault set to TRUE")
         end
         wait("ISIM HK_TLM_PKT FAULT_PRESENT == 'TRUE'", 10) 

         cmd_str = "POWER_RESET_INSTR"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Science instrument reset initiated")
         end
         
         wait("ISIM HK_TLM_PKT PWR_RESET_CYCLE_CNT > 0",    10)
         wait("ISIM HK_TLM_PKT INSTR_PWR_STATE == 'RESET'", 10)
         wait("ISIM HK_TLM_PKT PWR_INIT_CYCLE_CNT == 0",     0)
         wait("ISIM HK_TLM_PKT INSTR_PWR_STATE == 'READY'", 25)
         wait("ISIM HK_TLM_PKT SCI_STATE == 'OFF'",          0)
         wait("ISIM HK_TLM_PKT FAULT_PRESENT == 'FALSE'",    0)

         # 2. Power off

         cmd_str = "POWER_OFF_INSTR"
         test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
            raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "Instrument powered off from previous state READY")
         end
         wait("ISIM HK_TLM_PKT INSTR_PWR_STATE == 'OFF'", 10)

      end # valid
      
      def invalid
      
         #
         # TODO
         #

      end # invalid
   end # Class PwrTest
   
end # Module ISIM
end # Module AppFuncTest
