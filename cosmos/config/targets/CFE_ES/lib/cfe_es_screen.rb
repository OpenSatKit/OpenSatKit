###############################################################################
# cFE Executive Service Screen
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
      # Set default name to first combo option to simplify logic and no error checking
      bin_filename = FswConfigParam::CFE_ES_DEFAULT_APP_LOG_FILE
      tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_ES_APP_INFO
      case screen.get_named_widget("file").text
      when "CDS" 
         bin_filename = FswConfigParam::CFE_ES_DEFAULT_CDS_REG_DUMP_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_ES_CDS_REG
      when "ER_LOG"
         bin_filename = FswConfigParam::CFE_ES_DEFAULT_ER_LOG_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_ES_ERLOG
      when "SYS_LOG"
         bin_filename = FswConfigParam::CFE_ES_DEFAULT_SYSLOG_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_ES_SYSLOG
      when "TASK_INFO"
         bin_filename = FswConfigParam::CFE_ES_DEFAULT_TASK_LOG_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_CFE_ES_TASK_INFO
      end
      Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, bin_filename, tbl_mgr_filename)

   when "FUNC_APP_MGMT"
      display("CFS_KIT APP_MGMT_SCREEN",1500,10)
   
   when "FUNC_PERF_MON"
      display("CFS_KIT PERF_MON_SCREEN",1500,10)

   when "DEMO"
      Osk::System.check_n_start_cfs
      case screen.get_named_widget("demo").text
      when "App Management"
         display("CFS_KIT APP_MGMT_DEMO_SCREEN",500,50)
      when "Performance Monitor"
         display("CFS_KIT PERF_MON_DEMO_SCREEN",500,50)
      when "Performance Mon Script"
         spawn("ruby #{Osk::COSMOS_SCR_RUNNER} demo_perf_mon.rb")
      end

   when "TUTORIAL"
      case screen.get_named_widget("tutorial").text
      when "cFE Training Slides"
         spawn("evince #{Osk::CFE_TRAINING_DIR}/#{Osk::CFE_TRAINING_SLIDES_FILE}")
      when "ES Exercise Script"
         Osk::education_tutorial(Osk::TUTORIAL_SCRIPT, "cfe", "ES")
      when "cFE Exercise Slides"
         spawn("evince #{Osk::CFE_TRAINING_DIR}/#{Osk::CFE_EXERCISE_SLIDES_FILE}")
      when "Perf Mon Training Video"
         Osk::education_video(CFE_ES_YOUTUBE_PERF_MON)
      end
   
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cfe_es_src_cmd()"
   end
  
end # cfe_es_scr_cmd()
