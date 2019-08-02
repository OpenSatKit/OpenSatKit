###############################################################################
# cFE Time Service Screen
#
# Notes:
#   None 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_ops'
require "#{Cosmos::USERPATH}/config/targets/CFS_KIT/lib/tutorial_screen.rb"

################################################################################
## GUI Send Commands
################################################################################

def cfe_time_scr_cmd(screen, cmd)
 
   case cmd
   
   when "CMD"
      Osk::Ops::send_flt_cmd("CFE_TIME", "#{screen.get_named_widget("cmd").text}")
   
   when "TLM"
      # Only one combo option
      #screen.get_named_widget("tlm").text
      scr_name = "DIAG_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'CFE_TIME #{scr_name}'")
 
   when "TUTORIAL"
       # Only one combo option
       #screen.get_named_widget("tutorial").text
       launch_tutorial(self, "cfe", Osk::TUTORIAL_SCRIPT, "TIME")
   
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_time_src_cmd()"
   end
  
end # cfe_time_scr_cmd()
