###############################################################################
# ISIM Instrument Science Test Module
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
   
   class SciTest
   
      
      def initialize(app)

         @app = app
      
      end

      def valid
         
         #
         # These tests verify the valid usage of the instrument science commands. Some other 
         # functions like power on/off are exercised only to the intent to verify the
         # sceince behavior. Other tests like the power test ill completely verify these
         # functions.
         #
         # 1. Enable science
         # 2. Verify science data generation, files names, and file content
         # 3. Disable science
         #
         
         # 1. Enable science
         
         if not ( (tlm("#{@app.target_hk_str} INSTR_PWR_STATE") == "OFF")   and
                  (tlm("#{@app.target_hk_str} SCI_STATE")       == "OFF")   and
                  (tlm("#{@app.target_hk_str} FAULT_PRESENT")   == "FALSE"))
         
            raise "ISIM Sceince Test started in non-default state" 
            
         end
         
         cmd_str = "POWER_ON_INSTR"
         wait("ISIM HK_TLM_PKT INSTR_PWR_STATE == 'READY'", 25)


         # 2. Verify science data generation, files names, and file content

         cmd_str = "START_SCI_DATA"
         # Two events immediately issued so need event queue utility
         # "Science instrument data started"
         # "New science file created: /cf/simsat/rec/isim_000.txt"
         #test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
         #   raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "New science file created: /cf/simsat/rec/isim_000.txt")
         #end


         # 3. Disable science

         cmd_str = "STOP_SCI_DATA"
         # Two events immediately issued so need event queue utility
         # "Science instrument data stopped"
         # "Closed science file /cf/simsat/rec/isim_000.txt"
         #test_passed = AppFuncTest.send_cmd(@app,cmd_str) do |app_name, event_type, event_msg|
         #   raise "ISIM ERROR: Failed event message verification for '#{event_msg}'" unless AppFuncTest.valid_event_msg("ISIM", EVS_INFORMATION, "New science file created: /cf/simsat/rec/isim_000.txt")
         #end

         cmd_str = "ISIM POWER_OFF_INSTR"
         wait("ISIM HK_TLM_PKT INSTR_PWR_STATE == 'OFF'", 10)

      end # valid
      
      def invalid
      
         #
         # TODO
         #

      end # invalid
   end # Class SciTest
   
end # Module ISIM
end # Module AppFuncTest
