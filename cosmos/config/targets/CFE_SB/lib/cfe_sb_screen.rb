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
require 'osk_education'
require 'osk_system'
require 'cfe_sb_const'

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
      file_selection = screen.get_named_widget("file").text
      case file_selection
      when "Message Map"
         cmd_name = "WRITE_MAP_TO_FILE"
         bin_filename = FswConfigParam::CFE_SB_DEFAULT_MAP_FILENAME
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_SB_MSG_MAP
      when "Pipe Registry"
         cmd_name = "WRITE_PIPE_TO_FILE"
         bin_filename = FswConfigParam::CFE_SB_DEFAULT_PIPE_FILENAME
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_SB_PIPE
      when "Message Routes"
         cmd_name = "WRITE_ROUTING_TO_FILE"
         bin_filename = FswConfigParam::CFE_SB_DEFAULT_ROUTING_FILENAME
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_SB_ROUTES
      else
         raise "Error in Display File options. Drop down selection '#{file_selection}' is not defined in cfe_sb_scr_cmd()"
      end
      Osk::Ops::send_flt_bin_file_cmd("CFE_SB", "#{cmd_name} with ", tbl_mgr_filename, flt_path_filename: File.join(Osk::FLT_SRV_DIR,bin_filename), prompt: false)

   when "TUTORIAL"
      case screen.get_named_widget("tutorial").text
      when "Tutorial Slides"
         Osk::System.display_pdf(Osk::cfg_target_dir_file("CFE_SB", "docs", CFE_SB_TUTORIAL_FILE))
      when "Tutorial Video"
         Cosmos.open_in_web_browser(CFE_SB_YOUTUBE_OVERVIEW)
      when "Exercise Script"
         Osk::System.start_target_script("CFE_SB",CFE_SB_TUTORIAL_SCRIPT)
      end
   
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_sb_src_cmd()"
   end
  
end # cfe_sb_scr_cmd()
