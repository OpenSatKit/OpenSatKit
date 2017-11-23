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

NULL_IP_ADDR   = "0.0.0.0"
LOCAL_IP_ADDR  = "127.0.0.1"
PI_SAT_IP_ADDR = "192.168.0.1"
HOST_IP_ADDR   = "192.168.0.6"

################################################################################
## Send Commands
################################################################################

def pisat_send_cmd(screen, cmd)

  if (cmd == "START_CFS")
    cmd("PICONTROL START_CFS")
  elsif (cmd == "STOP_CFS")
    cmd("PICONTROL STOP_CFS"); 
  elsif (cmd == "SHUTDOWN_PI")
	  cmd("PICONTROL SHUTDOWN_PI")
  elsif (cmd == "REBOOT_PI")
    cmd("PICONTROL REBOOT_PI")
  elsif (cmd == "ENABLE_TLM")
    Cosmos.run_process("ruby tools/CmdSender -p \"KIT_TO ENABLE_TELEMETRY\"")
  elsif (cmd == "CFS_PI")
    cmd("KIT_TO ENABLE_TELEMETRY with IP_ADDR #{NULL_IP_ADDR}")
	wait(2)
    disconnect_interface("CFS_INT")
	wait(5)
    connect_interface("CFS_INT",PI_SAT_IP_ADDR,1234,1235,nil,nil,128,nil,nil)
	wait(5)
    Cosmos.run_process("ruby tools/CmdSender -p \"KIT_TO ENABLE_TELEMETRY\"")
    #connect_interface("CFS_INT",udp_cs_interface.rb,192.168.0.1,1234,1235,nil,nil,128,nil,nil)
    #map_target_to_interface("CS","CFS_PI_INT")
  elsif (cmd == "CFS_LOCAL")
    cmd("KIT_TO ENABLE_TELEMETRY with IP_ADDR #{NULL_IP_ADDR}")
	wait(2)
    disconnect_interface("CFS_INT")
	wait(5)
    connect_interface("CFS_INT","localhost",1234,1235,nil,nil,128,nil,nil)
	wait(5)
    cmd("KIT_TO ENABLE_TELEMETRY with IP_ADDR #{LOCAL_IP_ADDR}")
    #connect_interface("CFS_INT","udp_cs_interface.rb","127.0.0.1","1234","1235","nil","nil","128","nil","nil")
    #map_target_to_interface("CS","CFS_LOCAL_INT")
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
