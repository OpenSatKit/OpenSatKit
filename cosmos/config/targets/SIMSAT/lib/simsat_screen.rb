###############################################################################
# SimSat Screen
#
# Notes:
#   1. This doesn't follow the pattern of one ruby script for each screen
#      convention since all of the SimSat screens are designed to be 
#      aggregated as a whole. 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_ops'
require 'osk_education'
require 'osk_system'
require 'fsw_const'
require 'fsw_config_param'
require 'fm_const'
require 'hk_const'
require 'simsat_const'

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
      scr_name = "HK_TLM_PKT"
      case screen.get_named_widget("f42_tlm").text
      when "Controller"
         scr_name = "CONTROL_PKT"
      when "Controller Gains"
         scr_name = "CONTROL_GAINS_PKT"
      when "Housekeeping" 
         scr_name = "HK_TLM_PKT"
      end
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'F42 #{scr_name}'")
   when "F42_DOC"
      prompt(Osk::MSG_TBD_FEATURE)
   when "I42_CMD" 
      Osk::Ops::send_flt_cmd("I42", "#{screen.get_named_widget("I42_cmd").text}")
   when "I42_TLM"
      scr_name = "HK_TLM_PKT"
      case screen.get_named_widget("i42_tlm").text
      when "Actuator Cmd Data"  
         scr_name = "ACTUATOR_CMD_DATA_PKT"
      when "Housekeeping" 
         scr_name = "HK_TLM_PKT"
      when "Sensor Data"
         scr_name = "SENSOR_DATA_PKT"
      end
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'I42 #{scr_name}'")
   when "I42_DOC"
      prompt(Osk::MSG_TBD_FEATURE)
   when "FUNC_CONTROLLER_MGMT"
      display("CFS_KIT SIM_42_SCREEN",1500,50)
   when "DEMO"
      prompt(Osk::MSG_TBD_FEATURE)
   when "TUTORIAL"
      case screen.get_named_widget("tutorial").text
      when Osk::TXT_TUTORIAL_SLIDES
         doc_filename = Osk::cfg_target_dir_file("SIMSAT","docs",SimSat::MISSION_FSW_APPS_ADC_FILE)
         Osk::System.display_pdf(doc_filename) unless doc_filename.nil?
      end
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
      # Use default temporary binary table file names in flight and ground default table server directories
      tbl_name = FswConfigParam::DS_DESTINATION_TBL_NAME
      tbl_mgr_def_filename = Osk::TBL_MGR_DEF_DS_FILE_TBL
      if screen.get_named_widget("ds_tbl").text == "FILTER"
         tbl_name = FswConfigParam::DS_FILTER_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_DS_FILTER_TBL
      end
      Osk::Ops::send_cfe_dump_tbl_cmd("DS.#{tbl_name}", tbl_mgr_def_filename)
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
      # FM only has one table
      # Use default temporary binary table file names in flight and ground default table server directories
      Osk::Ops::send_cfe_dump_tbl_cmd("FM.#{FswConfigParam::FM_TABLE_CFE_NAME}", Osk::TBL_MGR_DEF_FM_FREESPACE)
   when "FM_FILE"
      # Only one FM command creates a file
      # Minimize user input to keep it simple & alert them to use the directory list command if they want all options 
      prompt = "Input directory to be written to a file. For more options\nuse WRITE_DIR_TO_FILE command.\n" 
      dir = ask_string(prompt, "#{Osk::FLT_SRV_DIR}")
      # Command string must end with comma because filename will be appended
      if (!dir.nil? and dir.length > 0) 
         cmd_str = "WRITE_DIR_TO_FILE with DIRECTORY #{dir}, SIZE_TIME_MODE 0, "
         bin_filename = Osk::TMP_BIN_FILE
         tbl_mgr_filename = Osk::TBL_MGR_DEF_FM_DIR
         Osk::Ops::send_flt_bin_file_cmd("FM", cmd_str , tbl_mgr_filename, flt_path_filename: File.join(Osk::FLT_SRV_DIR,bin_filename), prompt: false)
      end
   when "FM_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/fm/docs/users_guide/html/index.html")   
   when "HK_CMD" 
      Osk::Ops::send_flt_cmd("HK", "#{screen.get_named_widget("hk_cmd").text}")
   when "HK_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'HK #{scr_name}'")
   when "HK_TBL"
      # HK only has one table
      # Use default temporary binary table file names in flight and ground default table server directories
      Osk::Ops::send_cfe_dump_tbl_cmd("HK.#{FswConfigParam::HK_COPY_TABLE_NAME}", Osk::TBL_MGR_DEF_HK_COPY)
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
   when "TABLE_GEN_TOOLS"
      display("SIMSAT TBL_TOOL_SCREEN",1500,50)
   when "DEMO"

      if (Osk::System.check_n_start_cfs('simsat'))
      
         # Demo scripts manage screens & PacketViewer
         case screen.get_named_widget("demo").text
         when "Data-File Mgmt Script"
            spawn("ruby #{Osk::COSMOS_SCR_RUNNER} demo_datafile_mgmt.rb")         
         when "FM Playback Script"
            spawn("ruby #{Osk::COSMOS_SCR_RUNNER} demo_fm_playback.rb")
         when "FM Feature Script"
            spawn("ruby #{Osk::COSMOS_SCR_RUNNER} demo_fm_features.rb")
         when "HK Feature Script"
            spawn("ruby #{Osk::COSMOS_SCR_RUNNER} demo_hk_features.rb")
         else
            display_scr = nil
            case screen.get_named_widget("demo").text
            when "Data-File Mgmt Screen"
               display_scr = "SIMSAT DEMO_DATA_FILE_MGMT_SCREEN"
            when "DS Feature Screen"
               display_scr = "CFS_KIT RECORDER_MGMT_DEMO_SCREEN"
            when "FM Feature Screen"
               display_scr = "CFS_KIT FILE_MGMT_DEMO_SCREEN"
            end
            display(display_scr,500,50) unless display_scr.nil?
         end 
      end # If cFS running
      
   when "TUTORIAL"
      case screen.get_named_widget("tutorial").text
      when "Data-File Intro Video"
         Osk::education_video(SimSat::YOUTUBE_COMMUNITY_APPS_DATAFILE)    
      when "FM App Video"
         Osk::education_video(FM_YOUTUBE_OVERVIEW)
      when "HK App Video"
         Osk::education_video(HK_YOUTUBE_OVERVIEW)
      when "Comm App Intro Video"
         Osk::education_video(SimSat::YOUTUBE_COMMUNITY_APPS_INTRO)
      when "Data-File Intro Slides"
         doc_filename = Osk::cfg_target_dir_file("SIMSAT","docs",SimSat::MISSION_FSW_APPS_DATAFILE_FILE)
         Osk::System.display_pdf(doc_filename) unless doc_filename.nil?
      end

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
   when "CS_TBL"
      tbl_selection = screen.get_named_widget("cs_tbl").text
      case tbl_selection
      when "App"
         tbl_name = FswConfigParam::CS_APP_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_APP_TBL
      when "EEPROM"
         tbl_name = FswConfigParam::CS_EEPROM_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_EEPROM_TBL
      when "Memory"
         tbl_name = FswConfigParam::CS_MEMORY_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_MEMORY_TBL
      when "Tables"
         tbl_name = FswConfigParam::CS_TABLES_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_TABLES_TBL
      when "App Results"
         tbl_name = FswConfigParam::CS_APP_RES_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_APP_RES_TBL
      when "EEPROM Results"
         tbl_name = FswConfigParam::CS_EEPROM_RES_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_EEPROM_RES_TBL
      when "Memory Results"
         tbl_name = FswConfigParam::CS_MEMORY_RES_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_MEMORY_RES_TBL
      when "Tables Results"
         tbl_name = FswConfigParam::CS_TABLES_RES_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_TABLES_RES_TBL
      else
         raise "Error in Display Table options. Drop down selection '#{tbl_selection}' is not defined in simsat_health_safety()"
      end
      Osk::Ops::send_cfe_dump_tbl_cmd("CS.#{tbl_name}", tbl_mgr_def_filename)
   when "CS_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/cs/docs/users_guide/html/index.html")   
   when "HS_CMD" 
      Osk::Ops::send_flt_cmd("HS", "#{screen.get_named_widget("hs_cmd").text}")
   when "HS_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'HS #{scr_name}'")
   when "HS_TBL"
      tbl_selection = screen.get_named_widget("hs_tbl").text
      case tbl_selection
      when "App Monitor"
         tbl_name = FswConfigParam::HS_APP_MON_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_HS_AMT_TBL
      when "Event Monitor"
         tbl_name = FswConfigParam::HS_EVENT_MON_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_HS_EMT_TBL
      when "Msg Action"
         tbl_name = FswConfigParam::HS_MSG_ACT_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_HS_MAT_TBL
      when "Exec Counter"
         tbl_name = FswConfigParam::HS_EXEC_CNT_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_HS_XCT_TBL
      else
         raise "Error in Display Table options. Drop down selection '#{tbl_selection}' is not defined in simsat_health_safety()"
      end
      Osk::Ops::send_cfe_dump_tbl_cmd("HS.#{tbl_name}", tbl_mgr_def_filename)
   when "HS_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/hs/docs/users_guide/html/index.html")   
   when "FUNC_CS_MGMT"
      display("CS CS_MGMT_SCREEN",1500,50)
   when "FUNC_HS_MGMT"
      display("HS HS_MGMT_SCREEN",1500,50)
   when "DEMO"
      if (Osk::System.check_n_start_cfs('simsat'))
         # Demo scripts manage screens & PacketViewer
         case screen.get_named_widget("demo").text
         when "CS-HS Demo Script"
            spawn("ruby #{Osk::COSMOS_SCR_RUNNER} demo_health_safety.rb")
         end 
      end # If cFS running
   when "TUTORIAL"
      tutorial = screen.get_named_widget("tutorial").text
      case tutorial
      when "HS App Group Intro Slides"
         doc_filename = Osk::cfg_target_dir_file("SIMSAT","docs",SimSat::MISSION_FSW_APPS_HS_FILE)
         Osk::System.display_pdf(doc_filename) unless doc_filename.nil?
      when "HS App Group Intro Video"
         Osk::education_video(SimSat::YOUTUBE_COMMUNITY_APPS_HS)
      end 
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
      # Drop down menu label used as hash key
      tbl_name = { "Dwell Table 1" => FswConfigParam::MD_TABLE1_CFE_NAME,
                   "Dwell Table 2" => FswConfigParam::MD_TABLE2_CFE_NAME,
                   "Dwell Table 3" => FswConfigParam::MD_TABLE3_CFE_NAME,
                   "Dwell Table 4" => FswConfigParam::MD_TABLE4_CFE_NAME }  
      menu_opt = screen.get_named_widget("md_tbl").text
      if tbl_name.has_key? menu_opt
        Osk::Ops::send_cfe_dump_tbl_cmd("MD.#{tbl_name[menu_opt]}", Osk::TBL_MGR_DEF_MD_TBL)   
      else
        raise "Error in MD table options. Drop down selection #{menu_opt} is not defined in simsat_maintenance()"
      end
   when "MD_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/md/docs/users_guide/html/index.html")   
   when "MM_CMD" 
      Osk::Ops::send_flt_cmd("MM", "#{screen.get_named_widget("mm_cmd").text}")
   when "MM_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'MM #{scr_name}'")
   when "MM_FILE"
      # Only one MM command creates a file
      # Minimize user input to keep it simple & alert them to use the dump command if they want all options 
      # A '+' indicates a symbol is present
      prompt = "Dump 256 bytes of RAM. For more options\nuse DUMP_MEM_TO_FILE command. Offset\nand addresses must be formatted using ruby\nsyntax.\n" 
      mem_address = ask_string(prompt, "Enter <symbol+offset> or <address>")
      if mem_address.include? "+"
         mem_comp = mem_address.split("+")
         addr_str = "ADDR_OFFSET #{mem_comp[1]}, ADDR_SYMBOL_NAME #{mem_comp[0]}, "
      else
         addr_str = "ADDR_OFFSET #{mem_address}, ADDR_SYMBOL_NAME '', "
      end
      # Command string must end with comma because filename will be appended
      cmd_str = "DUMP_MEM_TO_FILE with MEM_TYPE #{Fsw::Const::MM_MEM_TYPE_RAM}, PAD_8 0, PAD_16 0, NUM_BYTES 256, #{addr_str}"
      bin_filename = Osk::TMP_BIN_FILE
      tbl_mgr_filename = Osk::TBL_MGR_DEF_MM_DMP
      Osk::Ops::send_flt_bin_file_cmd("MM", cmd_str , tbl_mgr_filename, flt_path_filename: File.join(Osk::FLT_SRV_DIR,bin_filename), prompt: false)
   when "MM_DOC"
      Cosmos.open_in_web_browser("#{Osk::OSK_CFS_DIR}/apps/mm/docs/users_guide/html/index.html")   
   when "FUNC_MEMORY_MGMT"
      display("CFS_KIT MEMORY_MGMT_SCREEN",1500,50)
   when "DEMO"
      if (Osk::System.check_n_start_cfs('simsat'))
         # Demo scripts manage screens & PacketViewer
         demo = screen.get_named_widget("demo").text
         case demo
         when "MM-MD Demo Screen"
            display("CFS_KIT MEMORY_MGMT_DEMO_SCREEN",500,50)
         when "MM-MD Demo Script"
            spawn("ruby #{Osk::COSMOS_SCR_RUNNER} demo_maint_mem_apps.rb")
         end 
      end # If cFS running
   when "TUTORIAL"
      tutorial = screen.get_named_widget("tutorial").text
      case tutorial
      when "Maint App Group Intro Slides"
         doc_filename = Osk::cfg_target_dir_file("SIMSAT","docs",SimSat::MISSION_FSW_APPS_MAINT_FILE)
         Osk::System.display_pdf(doc_filename) unless doc_filename.nil?
      when "Maint App Group Intro Video"
         Osk::education_video(SimSat::YOUTUBE_COMMUNITY_APPS_MAINT)    
      end 
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
      Osk::Ops::dump_json_tbl("KIT_SCH",tbl_id)
   when "KIT_SCH_DOC"
      prompt(Osk::MSG_TBD_FEATURE)
   when "KIT_TO_CMD" 
      Osk::Ops::send_flt_cmd("KIT_TO", "#{screen.get_named_widget("kit_to_cmd").text}")
   when "KIT_TO_TLM"
      # Only one option
      scr_name = "HK_TLM_PKT"
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'KIT_TO #{scr_name}'")
   when "KIT_TO_TBL"
      # Only one option
      tbl_id = 0;
      Osk::Ops::dump_json_tbl("KIT_TO",tbl_id)
   when "KIT_TO_DOC"
      prompt(Osk::MSG_TBD_FEATURE)
   when "FUNC_RUNTIME_MGMT"
      display("CFS_KIT MNG_APP_RUNTIME_SCREEN",1500,50)   
   when "FUNC_ES_APP_MGMT"
      display("CFS_KIT APP_MGMT_SCREEN",1500,10)
   when "DEMO"
      if (Osk::System.check_n_start_cfs('simsat'))
         # Demo scripts manage screens & PacketViewer
         case screen.get_named_widget("demo").text
         when "KIT_TO Stats"
            spawn("ruby #{Osk::COSMOS_SCR_RUNNER} demo_runtime_to_stats.rb")
         when "SCH_TO Tables"
            spawn("ruby #{Osk::COSMOS_SCR_RUNNER} demo_runtime_tables.rb")
         end
      end
   when "TUTORIAL"
      case screen.get_named_widget("tutorial").text
      when "Intro Video"
         Osk::education_video(SimSat::YOUTUBE_COMMUNITY_APPS_RUNTIME)    
      when Osk::TXT_TUTORIAL_SLIDES
         doc_filename = Osk::cfg_target_dir_file("SIMSAT","docs",SimSat::MISSION_FSW_APPS_RUNTIME_FILE)
         Osk::System.display_pdf(doc_filename) unless doc_filename.nil?
      end
   else
      raise "Error in screen definition file. Undefined runtime environment screen command '#{cmd}' sent to simsat_src_cmd()"
   end
     
end # simsat_runtime()

