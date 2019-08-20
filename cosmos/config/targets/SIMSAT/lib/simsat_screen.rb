###############################################################################
# SimSat Screen
#
# Notes:
#   1. This doesn't follow the pattern of one ruby script for each screen
#      convention since all of the SimSat screen are designed to be 
#      aggregated as a whole. 
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

def simsat_scr_cmd(screen, apps, cmd)
 
   case apps
   
   when "ADC"
      simsat_adc(screen,cmd)
   when "AUTONOMY"
      simsat_autonomy(screen,cmd)
   when "DATA_FILE"
      simsat_data_file(screen,cmd)
   when "HEALTH_SAFETY"
      simsat_health_safety(screen,cmd)
   when "MAINTENANCE"
      simsat_maintenance(screen,cmd)
   when "RUNTIME"
      simsat_runtime(screen,cmd)
   when "FUNC_TBL_MGMT"
      display("CFS_KIT TABLE_MGMT_SCREEN",1500,10)
   else
      raise "Error in screen definition file. Undefined app '#{apps}' sent to simsat_src_cmd()"
   end
  
end # simsat_scr_cmd()

   
#
# Attitude Determination & Control
#
def simsat_adc(screen, cmd)
 
   case cmd
   when "F42_CMD" 
      Osk::Ops::send_flt_cmd("F42", "#{screen.get_named_widget("f42_cmd").text}")
   when "F42_TLM"
      # Set default name to first combo option to simplify logic and no error checking
      scr_name = "ACTUATOR_CMD_PKT"
      case screen.get_named_widget("f42_tlm").text
      when "CONTROLLER"
         scr_name = "CONTROL_PKT"
      when "HOUSEKEEPING" 
         scr_name = "HK_TLM_PKT"
      when "SENSOR_DATA"
         scr_name = "SENSOR_DATA_PKT"
      end
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'F42 #{scr_name}'")
   when "F42_DOC"
      prompt(Osk::MSG_TBD_FEATURE)
   when "I42_CMD" 
      Osk::Ops::send_flt_cmd("I42", "#{screen.get_named_widget("I42_cmd").text}")
   when "I42_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'I42 #{scr_name}'")
   when "I42_DOC"
      prompt(Osk::MSG_TBD_FEATURE)
   when "FUNC_CONTROLLER_MGMT"
      display("CFS_KIT SIM_42_SCREEN",1500,50)
   when "DEMO"
      prompt(Osk::MSG_TBD_FEATURE)
   when "TUTORIAL"
      prompt(Osk::MSG_TBD_FEATURE)
   else
      raise "Error in screen definition file. Undefined attitude determination and control screen command '#{cmd}' sent to simsat_src_cmd()"
   end
     
end # simsat_adc()


#
# Autonomy
#
def simsat_autonomy(screen, cmd)
 
   case cmd
   when "LC_CMD" 
      Osk::Ops::send_flt_cmd("LC", "#{screen.get_named_widget("lc_cmd").text}")
   when "LC_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'LC #{scr_name}'")
   when "LC_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/lc/docs/users_guide/html/index.html")   
   when "SC_CMD" 
      Osk::Ops::send_flt_cmd("SC", "#{screen.get_named_widget("sc_cmd").text}")
   when "SC_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'SC #{scr_name}'")
   when "SC_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/sc/docs/users_guide/html/index.html")   
   when "FUNC_AUTO_MGMT"
      display("CFS_KIT AUTONOMY_MGMT_SCREEN",1500,10)
   when "DEMO"
      # Only one option
      # case screen.get_named_widget("demo").text
      display("CFS_KIT HEATER_CTRL_DEMO_SCREEN",500,50)
   when "TUTORIAL"
      # Only one option
      # screen.get_named_widget("tutorial").text
      prompt(Osk::MSG_TBD_FEATURE)
   else
      raise "Error in screen definition file. Undefined autonomy screen command '#{cmd}' sent to simsat_src_cmd()"
   end
     
end # simsat_autonomy()


#
# Data-File Management
#
def simsat_data_file(screen, cmd)
 
   case cmd
   when "DS_CMD" 
      Osk::Ops::send_flt_cmd("DS", "#{screen.get_named_widget("ds_cmd").text}")
   when "DS_TLM"
      scr_name = "FILE_INFO_PKT"
      if screen.get_named_widget("ds_cmd").text == "HOUSEKEEPING"
        scr_name = "HK_TLM_PKT"
      end
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'DS #{scr_name}'")
   when "DS_TBL"
      # Set default name to first combo option to simplify logic and no error checking
      bin_filename = FswConfigParam::DS_DEF_DEST_FILENAME
      tbl_mgr_filename = Osk::TBL_MGR_DEF_DS_FILE_TBL
      if screen.get_named_widget("ds_tbl").text == "FILTER"
         bin_filename = FswConfigParam::DS_DEF_FILTER_FILENAME
         tbl_mgr_filename = Osk::TBL_MGR_DEF_DS_FILTER_TBL
      end
      Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, bin_filename, tbl_mgr_filename)
      
   when "DS_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/ds/docs/users_guide/html/index.html")   
   when "FM_CMD" 
      Osk::Ops::send_flt_cmd("FM", "#{screen.get_named_widget("fm_cmd").text}")
   when "FM_TLM"
      scr_name = "DIR_LIST_PKT"
      case screen.get_named_widget("fm_tlm").text
      when "FILE_INFO"
         scr_name = "FILE_INFO_PKT"
      when "HOUSEKEEPING"
         scr_name = "HK_TLM_PKT"
      when "OPEN_FILES"
         scr_name = "OPEN_FILES_PKT"
      when "FREE_SPACE"
         scr_name = "FREE_SPACE_PKT"
      end
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'FM #{scr_name}'")
   when "FM_TBL"
      bin_filename = FswConfigParam::FM_TABLE_FILENAME
      tbl_mgr_filename = Osk::TBL_MGR_DEF_FM_FREESPACE
      Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, bin_filename, tbl_mgr_filename)
   when "FM_FILE"
      bin_filename = FswConfigParam::FM_DIR_LIST_FILE_DEFNAME
      tbl_mgr_filename = Osk::TBL_MGR_DEF_FM_DIR
      Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, bin_filename, tbl_mgr_filename)
   when "FM_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/fm/docs/users_guide/html/index.html")   
   when "HK_CMD" 
      Osk::Ops::send_flt_cmd("HK", "#{screen.get_named_widget("hk_cmd").text}")
   when "HK_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'HK #{scr_name}'")
   when "HK_TBL"
      bin_filename = FswConfigParam::HK_COPY_TABLE_FILENAME
      tbl_mgr_filename = Osk::TBL_MGR_DEF_HK_COPY
      Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, bin_filename, tbl_mgr_filename)
   when "HK_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/hk/docs/users_guide/html/index.html")   
   when "TFTP_CMD" 
      Osk::Ops::send_flt_cmd("TFTP", "#{screen.get_named_widget("tftp_cmd").text}")
   when "TFTP_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'TFTP #{scr_name}'")
   when "TFTP_DOC" 
      prompt(Osk::MSG_TBD_FEATURE)
   when "FUNC_FILE_MGMT"
      display("CFS_KIT FILE_MGMT_SCREEN",1500,50)
   when "FUNC_RECORDER_MGMT"
      display("CFS_KIT RECORDER_MGMT_SCREEN",1500,50)
   when "FUNC_TBL_MGMT"
      display("CFS_KIT TABLE_MGMT_SCREEN",1500,50)
   when "DEMO"
      demo_scr = "FILE_MGMT_DEMO_SCREEN"
      if screen.get_named_widget("demo").text == "RECORDER"
         demo_scr = "RECORDER_MGMT_DEMO_SCREEN"
      end
      display("CFS_KIT #{demo_scr}",500,50)
   when "TUTORIAL"
      prompt(Osk::MSG_TBD_FEATURE)
   else
      raise "Error in screen definition file. Undefined data/file management screen command '#{cmd}' sent to simsat_src_cmd()"
   end
     
end # simsat_data_file()


#
# Health & Safety
#
def simsat_health_safety(screen, cmd)
 
   case cmd
   when "CS_CMD" 
      Osk::Ops::send_flt_cmd("CS", "#{screen.get_named_widget("cs_cmd").text}")
   when "CS_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'CS #{scr_name}'")
   when "CS_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/cs/docs/users_guide/html/index.html")   
   when "HS_CMD" 
      Osk::Ops::send_flt_cmd("HS", "#{screen.get_named_widget("hs_cmd").text}")
   when "HS_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'HS #{scr_name}'")
   when "HS_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/hs/docs/users_guide/html/index.html")   
      #ug_path_filename = "#{Osk::OSK_CFS_DIR}/apps/hs/docs/users_guide/#{FswConfigParam::CS_USERS_GUIDE_FILE}"
      #spawn("evince '#{ug_path_filename}'")
   when "FUNC_TBD"
      prompt(Osk::MSG_TBD_FEATURE)
   when "DEMO"
      prompt(Osk::MSG_TBD_FEATURE)
   when "TUTORIAL"
      prompt(Osk::MSG_TBD_FEATURE)
   else
      raise "Error in screen definition file. Undefined health and safety screen command '#{cmd}' sent to simsat_src_cmd()"
   end
     
end # simsat_health_safety()


#
# Maintenance
#
def simsat_maintenance(screen, cmd)
 
   case cmd
   when "MD_CMD" 
      Osk::Ops::send_flt_cmd("MD", "#{screen.get_named_widget("md_cmd").text}")
   when "MD_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'MD #{scr_name}'")
   when "MD_TBL"
      bin_filename = "md_dw1_tbl.tbl"  #TODO - Let user choose dwell table
      tbl_mgr_filename = Osk::TBL_MGR_DEF_MD_TBL
      Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, bin_filename, tbl_mgr_filename)
   when "MD_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/md/docs/users_guide/html/index.html")   
   when "MM_CMD" 
      Osk::Ops::send_flt_cmd("MM", "#{screen.get_named_widget("mm_cmd").text}")
   when "MM_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'MM #{scr_name}'")
   when "MM_FILE"
      bin_filename = "Last-dump-filename"
      tbl_mgr_filename = Osk::TBL_MGR_DEF_MM_DMP
      Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, bin_filename, tbl_mgr_filename)
   when "MM_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/mm/docs/users_guide/html/index.html")   
   when "FUNC_MEMORY_MGMT"
      display("CFS_KIT MEMORY_MGMT_SCREEN",1500,50)
   when "DEMO"
      # Only one option
      display("CFS_KIT MEMORY_MGMT_DEMO_SCREEN",500,50)
   when "TUTORIAL"
      prompt(Osk::MSG_TBD_FEATURE)
   else
      raise "Error in screen definition file. Undefined maintenance screen command '#{cmd}' sent to simsat_src_cmd()"
   end
     
end # simsat_maintenance()


#
# Runtime Environment
#
def simsat_runtime(screen, cmd)
 
   case cmd
   when "KIT_CI_CMD" 
      Osk::Ops::send_flt_cmd("KIT_CI", "#{screen.get_named_widget("kit_ci_cmd").text}")
   when "KIT_CI_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'KIT_CI #{scr_name}'")
   when "KIT_CI_DOC"
      prompt(Osk::MSG_TBD_FEATURE)
   when "KIT_SCH_CMD" 
      Osk::Ops::send_flt_cmd("KIT_SCH", "#{screen.get_named_widget("kit_sch_cmd").text}")
   when "KIT_SCH_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'KIT_SCH #{scr_name}'")
   when "KIT_SCH_TBL"
      tbl_id = 0;
      if screen.get_named_widget("kit_sch_tbl").text == "SCHEDULER"
         tbl_id = 1;
      end
      Osk::Ops::display_json_tbl("KIT_SCH",tbl_id)
   when "KIT_SCH_DOC"
      prompt(Osk::MSG_TBD_FEATURE)
   when "KIT_TO_CMD" 
      Osk::Ops::send_flt_cmd("KIT_TO", "#{screen.get_named_widget("kit_to_cmd").text}")
   when "KIT_TO_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'KIT_TO #{scr_name}'")
   when "KIT_TO_DOC"
      prompt(Osk::MSG_TBD_FEATURE)
   when "FUNC_RUNTIME_MGMT"
      display("CFS_KIT MNG_APP_RUNTIME_SCREEN",1500,50)   
   when "FUNC_ES_APP_MGMT"
      display("CFS_KIT APP_MGMT_SCREEN",1500,10)
   when "DEMO"
      prompt(Osk::MSG_TBD_FEATURE)
   when "TUTORIAL"
      prompt(Osk::MSG_TBD_FEATURE)
   else
      raise "Error in screen definition file. Undefined runtime environment screen command '#{cmd}' sent to simsat_src_cmd()"
   end
     
end # simsat_runtime()

