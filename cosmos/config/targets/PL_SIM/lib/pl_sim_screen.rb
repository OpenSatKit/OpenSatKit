###############################################################################
# Payload Simulator App Management 
#
# Notes:
#   None    
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_system'
require 'osk_flight'
require 'osk_ops'


################################################################################
## Send Commands
################################################################################

def pl_sim_send_cmd(screen, cmd)
 
   ### Commands
   if (cmd == "NOOP")
      Osk::flight.send_cmd("PL_SIM",Osk::CMD_STR_NOOP)
   elsif (cmd == "RESET")
      Osk::flight.send_cmd("PL_SIM",Osk::CMD_STR_RESET)
   elsif (cmd == "POWER_ON")
      Osk::flight.send_cmd("PL_SIM","POWER_ON")
   elsif (cmd == "POWER_OFF")
      Osk::flight.send_cmd("PL_SIM","POWER_OFF")
   elsif (cmd == "POWER_RESET")
      Osk::flight.send_cmd("PL_SIM","POWER_RESET")
   elsif (cmd == "FAULT_ON")
      Osk::flight.send_cmd("PL_SIM","SET_FAULT")
   elsif (cmd == "FAULT_OFF")
      Osk::flight.send_cmd("PL_SIM","CLEAR_FAULT")
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to pl_sim_send_cmd()"
   end
  
end # pl_sim_send_cmd()
