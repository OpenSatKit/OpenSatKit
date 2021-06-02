###############################################################################
# Verify the cFE table Service application is fully integrated with OSK.
#
# Notes:
#   1. See app_intg_test.rb's prologue for a description of the purpose and
#      scope of an integration test.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require 'app_intg_test'

class HsimIntgTest < AppIntgTest

   def initialize (app)
      super(app)
   end
  
   def app_test
      return verify_running
   end
  
   #
   # Base class verify_noop isn't used because HSIM is constantly receiving commands
   # from HC. This verifies the error count doesn't increment and the valid counter
   # is more than 1 after the command is issued
   #
   # HSIM will eventually be replaced by an external (to cFS) simulator so 
   # this test will be removed. Just good enough for now. 
   #
   def verify_noop (noop_fc=0)
  
      result = false
	  
      cmd_valid_cnt = tlm("#{@app.target_hk_str} #{Osk::TLM_STR_CMD_VLD}")
      cmd_error_cnt = tlm("#{@app.target_hk_str} #{Osk::TLM_STR_CMD_ERR}")
      seq_cnt = tlm("#{@app.target_hk_str} #{Ccsds::PRI_HDR_SEQUENCE}")
      event_seq_cnt = tlm("CFE_EVS EVENT_MSG_PKT CCSDS_SEQUENCE")
    
      cmd("#{@app.target} #{Osk::CMD_STR_NOOP} with #{Ccsds::PRI_HDR_STREAMID} #{@app.cmd_mid}, #{Ccsds::PRI_HDR_SEQUENCE} 0, #{Ccsds::PRI_HDR_LENGTH} 1, #{Ccsds::SEC_HDR_CHECKSUM} 0, #{Ccsds::SEC_HDR_FUNCCODE} #{noop_fc}")
      wait("#{@app.target_hk_str} #{Osk::TLM_STR_CMD_VLD} == #{cmd_valid_cnt}+1", 10)  # Delay until updated cmd valid count or timeout. 
	
      #~~puts tlm("#{@target_hk_str} CMD_VALID_COUNT")
      if ( (tlm("#{@app.target_hk_str} #{Osk::TLM_STR_CMD_VLD}") == (cmd_valid_cnt + 1)) && 
           (tlm("#{@app.target_hk_str} #{Osk::TLM_STR_CMD_ERR}") ==  cmd_error_cnt))
      
         # TODO - Add utility to queue recent events so they can be queried 
         event_seq_cnt_delta = tlm("CFE_EVS EVENT_MSG_PKT CCSDS_SEQUENCE") - event_seq_cnt
         event_app  = tlm("CFE_EVS EVENT_MSG_PKT APP_NAME")
         event_type = tlm("CFE_EVS EVENT_MSG_PKT EVENT_TYPE")
         event_msg  = tlm("CFE_EVS EVENT_MSG_PKT MESSAGE")
         puts "event_seq_cnt_delta = #{event_seq_cnt_delta}\n"
         if (event_seq_cnt_delta == 1)
            if ((event_app == @app.target) and (event_type == Fsw::Const::CFE_EVS_INFORMATION) and (event_msg.include? @app.version))
               result = true
            end
         else
            puts "#{@app.target} NOOP test warning: Unable to verify event message. Event message sequence count changed by #{event_seq_cnt_delta}" 
            result = true
         end
   
      else
         if ( tlm("#{@app.target_hk_str} #{Osk::TLM_STR_CMD_VLD}") == seq_cnt)
           raise "#{@app.target} NOOP test failed: HK telemetry packet not received"
         end
         raise "#{@app.target} NOOP test failed: Command counters"
      end
	
      return result
    
   end # verify_noop()

   def verify_running (noop_fc=0)
  
      result = false
	  
      cmd_valid_cnt = tlm("#{@app.target_hk_str} #{Osk::TLM_STR_CMD_VLD}")
      cmd_error_cnt = tlm("#{@app.target_hk_str} #{Osk::TLM_STR_CMD_ERR}")
      seq_cnt = tlm("#{@app.target_hk_str} #{Ccsds::PRI_HDR_SEQUENCE}")
      event_seq_cnt = tlm("CFE_EVS EVENT_MSG_PKT CCSDS_SEQUENCE")
 
      cmd("#{@app.target} #{Osk::CMD_STR_NOOP} with #{Ccsds::PRI_HDR_STREAMID} #{@app.cmd_mid}, #{Ccsds::PRI_HDR_SEQUENCE} 0, #{Ccsds::PRI_HDR_LENGTH} 1, #{Ccsds::SEC_HDR_CHECKSUM} 0, #{Ccsds::SEC_HDR_FUNCCODE} #{noop_fc}")
      wait("#{@app.target_hk_str} #{Osk::TLM_STR_CMD_VLD} >= #{cmd_valid_cnt}+1", 10)  # Delay until updated cmd valid count or timeout. 
	  
      if ( (tlm("#{@app.target_hk_str} #{Osk::TLM_STR_CMD_VLD}") != cmd_valid_cnt) && 
           (tlm("#{@app.target_hk_str} #{Osk::TLM_STR_CMD_ERR}") == cmd_error_cnt))
        
         result = true
      else
         puts app.name + " verify_running test failed"
      end
	
    return result

   end # verify_running
  
end # Class HsimIntgTest