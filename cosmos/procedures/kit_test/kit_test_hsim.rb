###############################################################################
# Verify the Heater Simulator application is operational within the 
# starter kit.
#
# Notes:
#   1. Written by David McComas, licensed under the copyleft GNU
#      General Public License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require_utility 'kit_test_app'

class KitTestHsim < KitTestApp

  def initialize (_app)
    super(_app)
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
  def verify_running (noop_fc=0)
  
    result = false
	  
    cmd_valid_cnt = tlm("#{@target_hk_str} #{@@TLM_STR_CMD_VLD}")
    cmd_error_cnt = tlm("#{@target_hk_str} #{@@TLM_STR_CMD_ERR}")
    
    seq_cnt = tlm("#{@target_hk_str} #{@@TLM_STR_SEQUENCE}")
	  cmd("#{@app.target} #{@@CMD_STR_NOOP} with #{@@TLM_STR_STREAM_ID} #{@app.cmd_mid}, #{@@TLM_STR_SEQUENCE} 0, #{@@TLM_STR_LENGTH} 1, #{@@TLM_STR_CHECKSUM} 0, #{@@TLM_STR_FC} #{noop_fc}")
    wait("#{@target_hk_str} #{@@TLM_STR_SEQUENCE} >= #{seq_cnt}+1", 10)  # Delay until updated sequence count or timeout. . '!=' accounts for rollover
	
    if ( (tlm("#{@target_hk_str} #{@@TLM_STR_CMD_VLD}") !=  cmd_valid_cnt) && 
         (tlm("#{@target_hk_str} #{@@TLM_STR_CMD_ERR}") ==  cmd_error_cnt))
      result = true
    else
      puts app.name + " NOOP test failed"
    end
	
    return result

  end #
  
end # Class KitTestHsim