###############################################################################
# File Output (FILE_OUT) App Management 
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

require 'demo_file_out'

################################################################################
## Send Commands
################################################################################

def file_out_mgmt_send_cmd(screen, cmd)
 
   case cmd
   when 'NOOP'
      Osk::flight.send_cmd("FILE_OUT",Osk::CMD_STR_NOOP)
   when 'RESET'
      Osk::flight.send_cmd("FILE_OUT",Osk::CMD_STR_RESET)
   when 'SEND_FILE'
      send_file = combo_box("Select demo file send option", 'Send text', 'Send text with cancel', 'Send binary', 'Send binary with cancel')
      case send_file
      when 'Send text'
         #demo_file_in_transfer("file_in_test.txt")
      when 'Send text with cancel'
         #demo_file_in_transfer("file_in_test.txt", true)
      when 'Send binary'
         #demo_file_in_transfer("simsat_dir.dat")
      when 'Send binary with cancel'
         #demo_file_in_transfer("simsat_dir.dat", true)
      end 
   when 'CANCEL'
      Osk::flight.send_cmd("FILE_OUT","CANCEL_TRANSFER")
   when 'PAUSE'
      Osk::flight.send_cmd("FILE_OUT","PAUSE_TRANSFER")
   when 'RESUME'
      Osk::flight.send_cmd("FILE_OUT","RESUME_TRANSFER")
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to file_out_mgmt_send_cmd()"
   end
  
end # file_in_mgmt_send_cmd()
