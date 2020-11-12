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
require "#{Cosmos::USERPATH}/config/targets/CFS_KIT/lib/tutorial_screen.rb"

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
      cmd_name = "WRITE_REG_TO_FILE"
      bin_filename = FswConfigParam::CFE_TBL_DEFAULT_REG_DUMP_FILE
      tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_TBL_REG
      Osk::Ops::send_flt_bin_file_cmd("CFE_TBL", "#{cmd_name} with ", tbl_mgr_filename, flt_path_filename: File.join(Osk::FLT_SRV_DIR,bin_filename), prompt: false)

   when "FUNC_TBL_MGMT"
      display("CFS_KIT TABLE_MGMT_SCREEN",1500,10)
   
   when "DEMO"
      screen.get_named_widget("demo").text
      display("CFS_KIT TABLE_MGMT_DEMO_SCREEN",500,50)

   when "TUTORIAL"
      case screen.get_named_widget("tutorial").text
      when "TBL Training Video"
         Cosmos.open_in_web_browser("#{Osk::YOUTUBE_TRAINING_CFE_TBL}")      
      when "TBL Training Slides"
         spawn("evince #{Osk::CFE_TRAINING_DIR}/#{Osk::TBL_TRAINING_SLIDES_FILE}")
      when "TBL Exercise Script"
         launch_tutorial(self, "cfe", Osk::TUTORIAL_SCRIPT, "TBL")
      end
   
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_tbl_src_cmd()"
   end
  
end # cfe_tbl_scr_cmd()
