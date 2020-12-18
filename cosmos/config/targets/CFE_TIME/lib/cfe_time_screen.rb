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
require 'osk_education'
require 'cfe_time_const'

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
      case screen.get_named_widget("tutorial").text
      when "cFE Training Slides"
         spawn("evince #{Osk::CFE_TRAINING_DIR}/#{Osk::CFE_TRAINING_SLIDES_FILE}")
      when "Time Exercise Script"
         Osk::education_tutorial(Osk::TUTORIAL_SCRIPT, "cfe", "TIME")
      when "cFE Exercise Slides"
         spawn("evince #{Osk::CFE_TRAINING_DIR}/#{Osk::CFE_EXERCISE_SLIDES_FILE}")
      end
   
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_time_src_cmd()"
   end
  
end # cfe_time_scr_cmd()
