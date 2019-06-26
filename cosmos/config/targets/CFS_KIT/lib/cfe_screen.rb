###############################################################################
# cFS Kit cFE Screen
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

################################################################################
## GUI Send Commands
################################################################################

def cfe_send_cmd(screen, cmd)
 
   if (cmd == "ES_CMD")
      Osk::Ops::send_flt_cmd("CFE_ES", "#{screen.get_named_widget("es_cmd").text}")
   elsif (cmd == "ES_PERF_MON")
      display("CFS_KIT PERF_MON_SCREEN",1500,10)
   elsif (cmd == "ES_APP_MGMT")
      display("CFS_KIT APP_MGMT_SCREEN",1500,10)   
   elsif (cmd == "EVS_CMD")
      Osk::Ops::send_flt_cmd("CFE_EVS", "#{screen.get_named_widget("evs_cmd").text}")
   elsif (cmd == "SB_CMD")
      Osk::Ops::send_flt_cmd("CFE_SB", "#{screen.get_named_widget("sb_cmd").text}")
   elsif (cmd == "TBL_CMD")
      Osk::Ops::send_flt_cmd("CFE_TBL", "#{screen.get_named_widget("tbl_cmd").text}")
   elsif (cmd == "TBL_MGMT")
      display("CFS_KIT TABLE_MGMT_SCREEN",1500,10)
   elsif (cmd == "TIME_CMD")
      Osk::Ops::send_flt_cmd("CFE_TIME", "#{screen.get_named_widget("time_cmd").text}")
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_send_cmd()"
   end
  
end # cfe_send_cmd()
