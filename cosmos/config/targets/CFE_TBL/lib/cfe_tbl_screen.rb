###############################################################################
# cFE Table Service Screen
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
require 'cfe_tbl_const'

################################################################################
## GUI Send Commands
################################################################################

def cfe_tbl_scr_cmd(screen, cmd)
 
   case cmd
   
   when "CMD"
      Osk::Ops::send_flt_cmd("CFE_TBL", "#{screen.get_named_widget("cmd").text}")
   
   when "TLM"
      # Set default name to first combo option to simplify logic and no error checking
      scr_name = "HK_TLM_PKT"
      if screen.get_named_widget("tlm").text
         scr_name = "TBL_REGISTRY_PKT"
      end
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'CFE_TBL #{scr_name}'")
 
   when "FILE"
      file_selection = screen.get_named_widget("file").text
      if file_selection == "Registry"
         cmd_name = "WRITE_REG_TO_FILE"
         bin_filename = FswConfigParam::CFE_TBL_DEFAULT_REG_DUMP_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_TBL_REG
      else
         raise "Error in Display File options. Drop down selection '#{file_selection}' is not defined in cfe_tbl_scr_cmd()"
      end
      Osk::Ops::send_flt_bin_file_cmd("CFE_TBL", "#{cmd_name} with ", tbl_mgr_filename, flt_path_filename: File.join(Osk::FLT_SRV_DIR,bin_filename), prompt: false)

   when "FUNC_TBL_MGMT"
      display("CFE_TBL TABLE_MGMT_SCREEN",1500,10)
   
   when "DEMO"
      # The screen demos are not currently used in ES screen but
      # I've left the hooks in below
      Osk::System.check_n_start_cfs('cfsat')
      case screen.get_named_widget("demo").text
      when "Load/Dump Table"
         spawn("ruby #{Osk::COSMOS_SCR_RUNNER} demo_tbl_load_dump.rb")
      when "Load-Dump Screen"
         display("CFE_TBL TABLE_MGMT_DEMO_SCREEN",500,50)
      end

   when "TUTORIAL"
      case screen.get_named_widget("tutorial").text
      when "Tutorial Slides"
         Osk::System.display_pdf(Osk::cfg_target_dir_file("CFE_TBL", "docs", CFE_TBL_TUTORIAL_FILE))
      when "Tutorial Video"
         Cosmos.open_in_web_browser(CFE_TBL_YOUTUBE_OVERVIEW)
      when "Exercise Script"
         Osk::System.start_target_script("CFE_TBL",CFE_TBL_TUTORIAL_SCRIPT)
      end
   
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_tbl_src_cmd()"
   end
  
end # cfe_tbl_scr_cmd()
