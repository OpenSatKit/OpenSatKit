###############################################################################
# App Functional Test Module
#
# Notes:
#   1. Provides common test methods for use in app functional tests. Functional
#      test inherit from Cosmos::Test so including this module is similar to 
#      multiple inheritance or Java interfaces.
#   2. "expected_result" is a common return status that indicates the expected
#      test case scenario results ocurred so in an error case this would be 
#      error command counters incrementing and not the valid command counters. 
#   3. Utility functions do not raise exceptions. They pass status to the
#      calling function that manages exceptions.
#   4. Command counter verifications do not account for rollover. Apps do not
#      have a consistent counter size and that the FswApp class does not captured
#      this information. Test suites are typically run against a recent restart
#      so this should not be a problem. 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'osk_global'
require 'osk_targets'
require 'osk_system'
require 'fsw_const'
require 'fsw_config_param'

module AppFuncTest

   # Shorthand and keep type integer for event verification
   EVS_DEBUG       = Fsw::Const::CFE_EVS_DEBUG
   EVS_INFORMATION = Fsw::Const::CFE_EVS_INFORMATION
   EVS_ERROR       = Fsw::Const::CFE_EVS_ERROR
   EVS_CRITICAL    = Fsw::Const::CFE_EVS_CRITICAL

   FLT_TMP_TEST_FILE = File.join(Osk::FLT_TEST_DIR,"app_test_tmp.txt")

   @@test_dir_created = false
   
   attr_reader :app
   attr_reader :gnd_test_files_dir
   
   attr_reader :event_app
   attr_reader :event_type
   attr_reader :event_msg
   
   def app_func_test_init(target_str)

      # Configure the system
      FswApp.validate_cmd(false) # This module provides methods that verify valid/invalid command counters for expected errors
      
      @gnd_test_files_dir = Osk::cfg_target_dir(target_str,"test_files")
      Osk::Ops.create_flt_dir(Osk::FLT_TEST_DIR)
      @@test_dir_created = true
      
      @app = Osk::flight.app[target_str]
      return @app
   
   end # app_func_test_init()

   ##############################
   ## Tests Common to all Apps ##
   ##############################
   
   def test_noop_cmd
            
      status_bar("noop command")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      
      puts "test_noop_cmd: app version = #{@app.version}\n"
      test_passed = AppFuncTest.send_cmd(@app,"NOOP") do |app_name, event_type, event_msg|
         raise "JSON version #{@app.version} does not match NOOP event message version: #{event_msg}" unless AppFuncTest.valid_event_msg(@app.fsw_name, EVS_INFORMATION, @app.version)
      end
      raise "Failed NOOP command verification" unless test_passed
   
   end # test_noop_cmd()

   def test_reset_cmd
      
      status_bar("reset command")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      # Send a couple of noops to increment the command counters
      @app.send_cmd("NOOP")
      @app.send_cmd("NOOP")
      AppFuncTest.send_cmd(@app,"RESET_CTRS",expect_event: false)
   
   end # test_reset_cmd

   ##############################
   ## Instance Utility Methods ##
   ##############################

   def load_test_files(test_files)
   
      file_load_err = false
      
      test_files.each_value do |file|
         if (not Osk::system.file_transfer.put(File.join(@gnd_test_files_dir,file),File.join(Osk::FLT_TEST_DIR,file)))
            file_load_err = true  
         end 
      end
      
      return (not file_load_err) # Success if there weren't any load errors
      
   end # load_test_files()
   
   ###########################
   ## Class Utility Methods ##
   ###########################

   #
   # Verify command counters after event message to minimize chance a second
   # event gets captured
   #
   def self.send_cmd(app, cmd_str, expect_event: true, error_case: false, hk_timeout: 8)
      
      expected_result = false
      received_event  = false
      
      target_hk_str = "#{app.target} #{app.hk_pkt}"
      cmd_valid_cnt = tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_VLD}")
      cmd_error_cnt = tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_ERR}")
      cmd_seq_cnt   = tlm("#{target_hk_str} #{Ccsds::PRI_HDR_SEQUENCE}")

      event_seq_cnt = tlm("CFE_EVS EVENT_MSG_PKT CCSDS_SEQUENCE")
      app.send_cmd(cmd_str)
      if (expect_event)
         wait("CFE_EVS EVENT_MSG_PKT CCSDS_SEQUENCE != #{event_seq_cnt}", 3) 
         if (event_seq_cnt != tlm("CFE_EVS EVENT_MSG_PKT CCSDS_SEQUENCE"))
            
            @event_app  = tlm("CFE_EVS EVENT_MSG_PKT APP_NAME")
            @event_type = tlm("CFE_EVS EVENT_MSG_PKT EVENT_TYPE")
            @event_msg  = tlm("CFE_EVS EVENT_MSG_PKT MESSAGE")
            yield(@app_name, @event_type, @event_msg)  ##if block_given?
            received_event = true
         
         end
      end
           
      wait("#{target_hk_str} #{Ccsds::PRI_HDR_SEQUENCE} != #{cmd_seq_cnt}", hk_timeout)
      # Leave expected_result set to false if counters can't be verified
      if (cmd_seq_cnt != tlm("#{target_hk_str} #{Ccsds::PRI_HDR_SEQUENCE}"))
      
         if (error_case)
            
            expected_result = 
               (cmd_valid_cnt == tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_VLD}")) and
               (cmd_error_cnt == tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_ERR}") + 1)
               
         else

            expected_result = 
               (cmd_error_cnt == tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_ERR}")) and
               (cmd_valid_cnt == tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_VLD}") + 1)
         
         end # If error_case
      end # If cmd sequence count changed
      
      return expected_result
   
   end # send_cmd()


   def self.valid_event_msg(app_name, event_type, msg_substr)
      
      return ((@event_app == app_name) and (@event_type == event_type) and (@event_msg.include? msg_substr))
      
   end # verify_event_msg()
   
   
end # Module AppFuncTest

