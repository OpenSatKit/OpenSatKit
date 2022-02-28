###############################################################################
# Instrument Simulator (ISIM) App Management 
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

def isim_mgmt_send_cmd(screen, cmd)
 
   ### Commands
   if (cmd == "NOOP")
      Osk::flight.send_cmd("ISIM",Osk::CMD_STR_NOOP)
   elsif (cmd == "RESET")
      Osk::flight.send_cmd("ISIM",Osk::CMD_STR_RESET)
   elsif (cmd == "POWER_ON")
      Osk::flight.send_cmd("ISIM","POWER_ON_INSTR")
   elsif (cmd == "POWER_OFF")
      Osk::flight.send_cmd("ISIM","POWER_OFF_INSTR")
   elsif (cmd == "POWER_RESET")
      Osk::flight.send_cmd("ISIM","POWER_RESET_INSTR")
   elsif (cmd == "SCIENCE_ON")
      Osk::flight.send_cmd("ISIM","START_SCI_DATA")
   elsif (cmd == "SCIENCE_OFF")
      Osk::flight.send_cmd("ISIM","STOP_SCI_DATA")
   elsif (cmd == "FAULT_ON")
      Osk::flight.send_cmd("ISIM","SET_FAULT")
   elsif (cmd == "FAULT_OFF")
      Osk::flight.send_cmd("ISIM","CLEAR_FAULT")
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to isim_mgmt_send_cmd()"
   end
  
end # isim_mgmt_send_cmd()
