###############################################################################
# Defines a base class for an application being tested
#
# Notes:
#   1. TODO - Define common COSMOS DB strings in a separate file
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require_utility 'app'

class AppTest

  # Keywords used in COSMOS database
  @@CMD_STR_NOOP      = "NOOP"
  @@CMD_STR_RESET     = "RESET_COUNTERS"
  @@TLM_STR_HK_PKT    = "HK_TLM_PKT"
  @@TLM_STR_STREAM_ID = "CCSDS_STREAMID"
  @@TLM_STR_SEQUENCE  = "CCSDS_SEQUENCE"
  @@TLM_STR_LENGTH    = "CCSDS_LENGTH"
  @@TLM_STR_CHECKSUM  = "CCSDS_CHECKSUM"
  @@TLM_STR_FC        = "CCSDS_FUNCCODE"
  @@TLM_STR_CMD_VLD   = "CMD_VALID_COUNT"
  @@TLM_STR_CMD_ERR   = "CMD_ERROR_COUNT"
  
  attr_reader :app, :result

  @target_hk_str   # Convenience string that combines the COSMOS target name
                   # with the app's housekeeping packet name
  
  def initialize (_app)
    #puts '***Entering KitTest:initialize for app ' + _app.name
    @app = _app
    @target_hk_str = "#{app.target} #{app.hk_pkt}"
    @result = false
  end

  # Method that will be called by fucntion managing the integration tests. Subclasses 
  # do not override this function. This level of inidrection allows this base class to
  # provide common pre and/or post test functionality.
  def run
    #puts "***Entering KitTest:run for app " + @app.name
    @result = app_test
  end

  # Virtual function that must be defined by subclasses. It must return a boolean
  # true/false to indicate whether test passed/failed
  def app_test
    puts "Undefined test for " + @app_name
  end

  def result_str
    return @result ? "passed" : "failed"
  end

  def verify_noop (noop_fc=0)
  
    result = false
	  
    cmd_valid_cnt = tlm("#{@target_hk_str} #{@@TLM_STR_CMD_VLD}")
    cmd_error_cnt = tlm("#{@target_hk_str} #{@@TLM_STR_CMD_ERR}")
    seq_cnt = tlm("#{@target_hk_str} #{@@TLM_STR_SEQUENCE}")
    ##puts cmd_valid_cnt
    cmd("#{@app.target} #{@@CMD_STR_NOOP} with #{@@TLM_STR_STREAM_ID} #{@app.cmd_mid}, #{@@TLM_STR_SEQUENCE} 0, #{@@TLM_STR_LENGTH} 1, #{@@TLM_STR_CHECKSUM} 0, #{@@TLM_STR_FC} #{noop_fc}")
    wait("#{@target_hk_str} #{@@TLM_STR_CMD_VLD} == #{cmd_valid_cnt}+1", 10)  # Delay until updated cmd valid count or timeout. 
	
    ##puts tlm("#{@target_hk_str} CMD_VALID_COUNT")
    if ( (tlm("#{@target_hk_str} #{@@TLM_STR_CMD_VLD}") == (cmd_valid_cnt + 1)) && 
         (tlm("#{@target_hk_str} #{@@TLM_STR_CMD_ERR}") ==  cmd_error_cnt))
      result = true
    else
      if ( tlm("#{@target_hk_str} #{@@TLM_STR_CMD_VLD}") == seq_cnt)
        puts app.name + " HK telemetry packet not received"
      end      
      puts app.name + " NOOP test failed"
    end
	
    return result
    
  end
  
end # Class AppTest