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
require 'osk_education'
require 'osk_system'
require 'cfe_evs_const'

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
      file_selection = screen.get_named_widget("file").text
      case file_selection
      when "App Event Info"
         cmd_name = "WRITE_APP_INFO_TO_FILE"
         bin_filename = FswConfigParam::CFE_EVS_DEFAULT_APP_DATA_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_EVS_APP_INFO      
      when "Local Event Log" 
         cmd_name = "WRITE_LOG_TO_FILE"
         bin_filename = FswConfigParam::CFE_EVS_DEFAULT_LOG_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_EVS_LOG
      else
         raise "Error in Display File options. Drop down selection '#{file_selection}' is not defined in cfe_evs_scr_cmd()"
      end
      Osk::Ops::send_flt_bin_file_cmd("CFE_EVS", "#{cmd_name} with ", tbl_mgr_filename, flt_path_filename: File.join(Osk::FLT_SRV_DIR,bin_filename), prompt: false)

   when "TUTORIAL"
      case screen.get_named_widget("tutorial").text
      when "Tutorial Slides"
         Osk::System.display_pdf(Osk::cfg_target_dir_file("CFE_EVS", "docs", CFE_EVS_TUTORIAL_FILE))
      when "Tutorial Video"
         Cosmos.open_in_web_browser(CFE_EVS_YOUTUBE_OVERVIEW)
      when "Exercise Script"
         Osk::System.start_target_script("CFE_EVS",CFE_EVS_TUTORIAL_SCRIPT)
      end
   
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_evs_src_cmd()"
   end
  
end # cfe_evs_scr_cmd()
