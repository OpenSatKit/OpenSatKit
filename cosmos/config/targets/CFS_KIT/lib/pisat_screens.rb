###############################################################################
# PiSat Screen Scripts 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
###############################################################################

require 'cosmos'
require 'cosmos/script'
Cosmos.catch_fatal_exception do
  require 'cosmos/tools/cmd_sender/cmd_sender'
  require 'cosmos/tools/tlm_viewer/screen'
  require 'cosmos/tools/tlm_viewer/tlm_viewer'
end
require 'osk_global'
require 'osk_system'

################################################################################
## PiSat Control
################################################################################

def pisat_control(screen, cmd)

   if (cmd == "START_CFS")
      cmd("PICONTROL START_CFS")
   elsif (cmd == "STOP_CFS")
      cmd("PICONTROL STOP_CFS"); 
   elsif (cmd == "SHUTDOWN_PI")
      cmd("PICONTROL SHUTDOWN_PI")
   elsif (cmd == "REBOOT_PI")
      cmd("PICONTROL REBOOT_PI")
   elsif (cmd == "SWITCH_TO_PISAT")
      display("CFS_KIT PISAT_CONNECT_SCREEN",50,50)
   elsif (cmd == "SWITCH_TO_LOCAL")
      pisat_connection(screen,"SWITCH_TO_LOCAL");
   elsif (cmd == "ENABLE_TLM")
      Cosmos.run_process("ruby tools/CmdSender -p \"KIT_TO ENABLE_TELEMETRY\"")
   else
      prompt("Error in screen definition file. Undefined command sent to pisat_send_cmd()")
   end
   
end # pisat_control()


################################################################################
## PiSat Control
################################################################################

def pisat_connection(screen, cmd)

   if (cmd == "SWITCH_TO_PISAT")
      host_ip_addr = screen.get_named_widget("host_ip_addr").text
      if (host_ip_addr == "")
        host_ip_addr = Osk::COSMOS_IP_ADDR
      end
      Osk::system.switch_local_to_pisat_cfs(host_ip_addr)
      clear("CFS_KIT PISAT_CONNECT_SCREEN")    
   elsif (cmd == "SWITCH_TO_LOCAL")
      Osk::system.switch_pisat_to_local_cfs()
   elsif (cmd == "CANCEL")
      clear("CFS_KIT PISAT_CONNECT_SCREEN")    
   else
     prompt("Error in screen definition file. Undefined command sent to pisat_send_cmd()")
   end

end # End pisat_connection()

#
# Useful code snippets
#
#sel = message_box("Press #{MSG_BUTTON_CONT} to continue",MSG_BUTTON_CONT)
#if (sel != MSG_BUTTON_CONT) then return end
#
#Cosmos.run_process("ruby tools/CmdSender -p \"CFE_ES START_APP\"")
