###############################################################################
# PiSat Screen Scripts
#
################################################################################

require 'cosmos'
require 'cosmos/script'
Cosmos.catch_fatal_exception do
  require 'cosmos/tools/cmd_sender/cmd_sender'
  require 'cosmos/tools/tlm_viewer/screen'
  require 'cosmos/tools/tlm_viewer/tlm_viewer'
end
require 'cfs_kit_global'


################################################################################
## Send Commands
################################################################################

def pisat_send_cmd(screen, cmd)

	if (cmd == "PICONTROL_START")
    cmd("PICONTROL STARTCFS")
	elsif (cmd == "PICONTROL_STOP")
    cmd("PICONTROL STOPCFS"); 
  elsif (cmd == "PICONTROL_SHUTDOWN")
	  cmd("PICONTROL SHUTDOWN")
  elsif (cmd == "PICONTROL_REBOOT")
    cmd("PICONTROL REBOOT")
  else
    prompt("Error in screen definition file. Undefined commmand sent to pisat_send_cmd()")
  end
end 

#
# Useful code snippets
#
#sel = message_box("Press #{BUTTON_CONT} to continue",BUTTON_CONT)
#if (sel != BUTTON_CONT) then return end  
#
#Cosmos.run_process("ruby tools/CmdSender -p \"CFE_ES START_APP\"")
