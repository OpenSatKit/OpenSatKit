###############################################################################
# cFE Software Bus Screen
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

def cfe_sb_scr_cmd(screen, cmd)
 
   case cmd
   
   when "CMD"
      Osk::Ops::send_flt_cmd("CFE_SB", "#{screen.get_named_widget("cmd").text}")
   
   when "TLM"
      # Set default name to first combo option to simplify logic and no error checking
      scr_name = "PREV_SUBSCRIBE_TLM_PKT" # SB has inconsistent names
      case screen.get_named_widget("tlm").text
      when "HOUSEKEEPING" 
         scr_name = "HK_TLM_PKT"
      when "ONE_SUBSCR"
         scr_name = "SUBSCRIBE_REPORT_TLM_PKT"
      when "STATS"
         scr_name = "STATS_TLM_PKT"
      end
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'CFE_SB #{scr_name}'")
 
   when "FILE"
      # Set default name to first combo option to simplify logic and no error checking
      cmd_name = "WRITE_MAP_TO_FILE"
      bin_filename = FswConfigParam::CFE_SB_DEFAULT_MAP_FILENAME
      tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_SB_MSG_MAP
      case screen.get_named_widget("file").text
      when "PIPES"
         cmd_name = "WRITE_PIPE_TO_FILE"
         bin_filename = FswConfigParam::CFE_SB_DEFAULT_PIPE_FILENAME
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_SB_PIPE
      when "ROUTES"
         cmd_name = "WRITE_ROUTING_TO_FILE"
         bin_filename = FswConfigParam::CFE_SB_DEFAULT_ROUTING_FILENAME
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_SB_ROUTES
      end
      Osk::Ops::send_flt_bin_file_cmd("CFE_SB", "#{cmd_name} with ", tbl_mgr_filename, flt_path_filename: File.join(Osk::FLT_SRV_DIR,bin_filename), prompt: false)
      #Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, bin_filename, tbl_mgr_filename)

   when "TUTORIAL"
      case screen.get_named_widget("tutorial").text
      when "SB_TRAINING_VIDEO"
         Cosmos.open_in_web_browser("#{Osk::YOUTUBE_TRAINING_CFE_SB}")   
      when "SB_TRAINING_SLIDES"
         spawn("evince #{Osk::CFE_TRAINING_DIR}/#{Osk::SB_TRAINING_SLIDES_FILE}")
      when "SB_EXERCISE_SCRIPT"
         launch_tutorial(self, "cfe", Osk::TUTORIAL_SCRIPT, "SB")
      end
   
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_sb_src_cmd()"
   end
  
end # cfe_sb_scr_cmd()
