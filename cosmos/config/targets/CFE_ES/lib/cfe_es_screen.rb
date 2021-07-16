###############################################################################
# cFE Executive Service Screen
#
# Notes:
#   1. Starts cfsat target if cFS is not running when a demo is selected. cfsat
#      is minimum app configuration that allows the demo to run
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_ops'
require 'osk_education'
require 'osk_system'
require 'cfe_es_const'

################################################################################
## GUI Send Commands
################################################################################

def cfe_es_scr_cmd(screen, cmd)
 
   case cmd
   
   when "CMD"
      Osk::Ops::send_flt_cmd("CFE_ES", "#{screen.get_named_widget("cmd").text}")
   
   when "TLM"
      # Set default name to first combo option to simplify logic and no error checking
      scr_name = "APP_INFO_TLM_PKT"
      case screen.get_named_widget("tlm").text
      when "HOUSEKEEPING" 
         scr_name = "HK_TLM_PKT"
      when "MEM_POOL"
         scr_name = "MEM_POOL_STATS_TLM_PKT"
      when "SHELL"
         scr_name = "SHELL_TLM_PKT"
      end
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'CFE_ES #{scr_name}'")
 
   when "FILE"
      file_selection = screen.get_named_widget("file").text
      case file_selection
      when "App Info"
         cmd_name = "WRITE_APP_INFO_TO_FILE"
         bin_filename = FswConfigParam::CFE_ES_DEFAULT_APP_LOG_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_ES_APP_INFO      
      when "Critical Data Storage Registry" 
         cmd_name = "WRITE_CDS_REG_TO_FILE"
         bin_filename = FswConfigParam::CFE_ES_DEFAULT_CDS_REG_DUMP_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_ES_CDS_REG
      when "Exception-Reset Log"
         cmd_name = "WRITE_ERLOG_TO_FILE"
         bin_filename = FswConfigParam::CFE_ES_DEFAULT_ER_LOG_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_ES_ERLOG
      when "System Log"
         cmd_name = "WRITE_SYSLOG_TO_FILE"
         bin_filename = FswConfigParam::CFE_ES_DEFAULT_SYSLOG_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_ES_SYSLOG
      when "Task Info"
         cmd_name = "WRITE_TASK_INFO_TO_FILE"
         bin_filename = FswConfigParam::CFE_ES_DEFAULT_TASK_LOG_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_ES_TASK_INFO
      else
         raise "Error in Display File options. Drop down selection '#{file_selection}' is not defined in cfe_es_scr_cmd()"
      end
      Osk::Ops::send_flt_bin_file_cmd("CFE_ES", "#{cmd_name} with ", tbl_mgr_filename, flt_path_filename: File.join(Osk::FLT_SRV_DIR,bin_filename), prompt: false)
      
   when "FUNC_APP_MGMT"
      display("CFS_KIT APP_MGMT_SCREEN",1500,10)
   
   when "FUNC_PERF_MON"
      display("CFS_KIT PERF_MON_SCREEN",1500,10)

   when "DEMO"
      Osk::System.check_n_start_cfs('cfsat')
      case screen.get_named_widget("demo").text
      when "App Management Screen"
         display("CFS_KIT APP_MGMT_DEMO_SCREEN",500,50)
      when "Perf Monitor Screen"
         display("CFS_KIT PERF_MON_DEMO_SCREEN",500,50)
      when "Perf Monitor Script"
         spawn("ruby #{Osk::COSMOS_SCR_RUNNER} demo_perf_mon.rb")
      end

   when "TUTORIAL"
      case screen.get_named_widget("tutorial").text
      when "Tutorial Slides"
         Osk::System.display_pdf(Osk::cfg_target_dir_file("CFE_ES", "docs", CFE_ES_TUTORIAL_FILE))
      when "Exercise Script"
         Osk::System.start_target_script("CFE_ES",CFE_ES_TUTORIAL_SCRIPT)
      when "Perf Monitor Video"
         Osk::education_video(CFE_ES_YOUTUBE_PERF_MON)
      end
   
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_es_src_cmd()"
   end
  
end # cfe_es_scr_cmd()
