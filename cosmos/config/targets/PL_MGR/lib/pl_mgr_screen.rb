###############################################################################
# Payload Manager App Management 
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

def pl_mgr_send_cmd(screen, cmd)
 
   ### Commands
   if (cmd == "NOOP")
      Osk::flight.send_cmd("PL_MGR",Osk::CMD_STR_NOOP)
   elsif (cmd == "RESET")
      Osk::flight.send_cmd("PL_MGR",Osk::CMD_STR_RESET)
   elsif (cmd == "SCIENCE_ON")
      Osk::flight.send_cmd("PL_MGR","START_SCIENCE")
   elsif (cmd == "SCIENCE_OFF")
      Osk::flight.send_cmd("PL_MGR","STOP_SCIENCE")
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to pl_mgr_send_cmd()"
   end
  
end # pl_mgr_send_cmd()
