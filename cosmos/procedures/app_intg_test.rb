###############################################################################
# Defines a base class for an application integration test
#
# Notes:
#   1. This procedure verifies anything relevant with making the app functional
#      within the OSK build and runtime environments. Some app functionality is
#      exercised to achieved these goals but this test is not intended to be a
#      comprehensive functional test.
#   2. Something as simple as aending a noop command and verifying the event 
#      message reception, the app version in the event message, and the app's
#      valid command count in its houseeping packet verifies the command mid,  
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'ccsds'
require 'osk_global'
require 'fsw_const'

class AppIntgTest

   attr_reader :app    # fsw_app
   attr_reader :result

  
   def initialize (app)
      
      @app = app
      @result = false
      
   end # initialize()

   # Method that will be called by function managing the integration tests. Subclasses 
   # do not override this function. This level of indirection allows this base class to
   # provide common pre and/or post test functionality.
   def run
   
      raise "#{@app.target}'s metadata doesn't identify it as a SimSat app" unless @app.cfs_targets.include?("simsat")
      @result = app_test
   
   end # run()

   # Virtual function that must be defined by subclasses. It must return a boolean
   # true/false to indicate whether test passed/failed
   def app_test
   
      puts "Undefined test for " + @app.target
   
   end # app_test()

   def result_str

      return @result ? "passed" : "failed"
  
   end

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
  
end # Class AppIntgTest