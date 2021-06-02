###############################################################################
# Spacecraft Ops Screen
#
# Notes:
#   1. This doesn't follow the pattern of one ruby script for each screen
#      convention since all of the SimSat screens are designed to be 
#      aggregated as a whole. 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_ops'
require 'osk_education'
require 'fsw_const'
require 'fsw_config_param'


################################################################################
## GUI Send Commands
################################################################################

def scsim_ops_send_cmd(screen, cmd)
 
   case cmd 
   when "START_SIM"
      sim_id = screen.get_named_widget("sim_id").text
      if (sim_id.include? "1")
        cmd("SCSIM START_SIM WITH ID 1")
      else
        cmd("SCSIM START_SIM WITH ID 2")
      end
   when "STOP_SIM"
      cmd("SCSIM STOP_SIM")
   when "START_EVT_PLBK"
      Osk::Ops::send_flt_cmd("KIT_TO", "START_EVT_PLBK")
   when "STOP_EVT_PLBK"
      Osk::Ops::send_flt_cmd("KIT_TO", "STOP_EVT_PLBK")
   when "START_REC_PLBK"
      Osk::Ops::send_flt_cmd("SCSIM", "START_REC_PLBK")
   when "STOP_REC_PLBK"
      Osk::Ops::send_flt_cmd("SCSIM", "STOP_REC_PLBK")
   when "UPLOAD_ATS"
      prompt("Upload ATS non implemented")      
   else
      raise "Error in screen definition file. Undefined cmd '#{cmd}' sent to scsim_ops_send_cmd()"
   end
  
end # scsim_ops_send_cmd()

   