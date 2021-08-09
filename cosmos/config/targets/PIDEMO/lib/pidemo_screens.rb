###############################################################################
# Pi Demo Screen Scripts 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
###############################################################################

require 'cosmos'
require 'cosmos/script'
require 'osk_global'
require 'osk_system'

################################################################################
## Demo PiSat Commands
################################################################################

def pidemo_cmd(screen, cmd)

   if (cmd == "TARGET_CMD")
      target_cmd = screen.get_named_widget("target_cmd").text
      case target_cmd
      when "Start cFS"
         cmd("PIDEMO START_CFS")
      when "Stop cFS"
         cmd("PIDEMO STOP_CFS") 
      when "Reboot Pi"
         cmd("PIDEMO REBOOT_PI")
      when "Shutdown Pi" 
         cmd("PIDEMO SHUTDOWN_PI")
      when "Enable Tlm" 
         prompt("<pre>For Alan's original PiSat:\n  StreamId = 6272\n  FuncCode = 7\n  PiAddr   = 192.168.0.6</pre>")
         Cosmos.run_process("ruby tools/CmdSender -p \"KIT_TO ENABLE_TELEMETRY\"")
      else
         raise "Error in screen definition file. Undefined target command sent to pidemo_cmd()"
      end # target_cmd
   else
      raise "Error in screen definition file. Undefined command sent to pidemo_cmd()"
   end
   
end # pidemo_cmd()


################################################################################
## Demo PiSat Connect Commands 
################################################################################

def pidemo_connect_cmd(screen, cmd)

   if (cmd == "SWITCH_TO_PISAT")
      host_ip_addr = screen.get_named_widget("host_ip_addr").text
      if (host_ip_addr == "")
        host_ip_addr = Osk::COSMOS_IP_ADDR
      end
      Osk::system.switch_local_to_pisat_cfs(host_ip_addr)
      clear("PIDEMO PISAT_CONNECT_SCREEN")    
   elsif (cmd == "SWITCH_TO_LOCAL")
      Osk::system.switch_pisat_to_local_cfs()
   elsif (cmd == "CANCEL")
      clear("PIDEMO PISAT_CONNECT_SCREEN")    
   else
     prompt("Error in screen definition file. Undefined command sent to pidemo_connect_cmd()")
   end

end # End pidemo_connect_cmd()

#
# Useful code snippets
#
#sel = message_box("Press #{MSG_BUTTON_CONT} to continue",MSG_BUTTON_CONT)
#if (sel != MSG_BUTTON_CONT) then return end
#
#Cosmos.run_process("ruby tools/CmdSender -p \"CFE_ES START_APP\"")
