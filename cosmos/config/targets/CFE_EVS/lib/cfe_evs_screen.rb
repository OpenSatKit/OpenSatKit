###############################################################################
# cFE Event Service Screen
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

def cfe_evs_scr_cmd(screen, cmd)
 
   case cmd
   
   when "CMD"
      Osk::Ops::send_flt_cmd("CFE_EVS", "#{screen.get_named_widget("cmd").text}")
   
   when "TLM"
      # Set default name to first combo option to simplify logic and no error checking
      scr_name = "EVENT_MSG_PKT"
      if screen.get_named_widget("tlm").text == "HOUSEKEEPING" 
         scr_name = "HK_TLM_PKT"
      end
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'CFE_EVS #{scr_name}'")
 
   when "FILE"
      # Set default name to first combo option to simplify logic and no error checking
      bin_filename = FswConfigParam::CFE_EVS_DEFAULT_APP_DATA_FILE
      tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_EVS_APP_INFO
      if screen.get_named_widget("file").text == "LOCAL_LOG"
         bin_filename = FswConfigParam::CFE_EVS_DEFAULT_LOG_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_EVS_LOG
      end
      Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, bin_filename, tbl_mgr_filename)

   when "TUTORIAL"
       screen.get_named_widget("tutorial").text
       launch_tutorial(self, "cfe", Osk::TUTORIAL_SCRIPT, "EVS")
   
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_evs_src_cmd()"
   end
  
end # cfe_evs_scr_cmd()
