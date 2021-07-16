###############################################################################
# OSK Global
#
# Notes:
#   1. SimSat definitions are in config/targets/SIMSAT/lib/simsat_const.rb
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'cosmos'

module Osk


   ######################
   ## cFS Target Names ##
   ######################

   SIMSAT_TARGET  = "simsat"   # Mission FSW 
   CFSAT_TARGET   = "cfsat"    # cFS Education
   SANDBOX_TARGET = "sandbox"  # Research & Development
   PISAT_TARGET   = "pisat"    # Raspberry Pi
   
   ###########################
   ## JSON Naming standards ##
   ###########################

   JSON_APP_TYPE_CFS = "cfs"
   JSON_APP_TYPE_OSK = "osk"
   
   ######################
   ## Network Settings ##
   ######################

   NULL_IP_ADDR  = "0.0.0.0"

   # The following definitions must match cmd_tlm_Server.txt definitions
   COSMOS_IP_ADDR = "127.0.0.1"
   COSMOS_CFS_INT = "LOCAL_CFS_INT"

   PISAT_IP_ADDR = "192.168.0.1"
   PISAT_CFS_INT = "PISAT_CFS_INT"
   PISAT_CONTROL = "PISAT_CONTROL"
   HOST_IP_ADDR  = "192.168.0.6"

   CFS_CMD_PORT  = "1234"
   CFS_TLM_PORT  = "1235"

   # These are generic protocol-independent settings
   GET_FILE_TIMEOUT = 15   # Telemetry get counter verification timeout 
   PUT_FILE_TIMEOUT = 15   # Telemetry put counter verification timeout 

   OVERRIDE_OFF      = "OVERRIDE_OFF"
   OVERRIDE_TO_TRUE  = "OVERRIDE_TO_TRUE"
   OVERRIDE_TO_FALSE = "OVERRIDE_TO_FALSE"
   
   OVERRIDE_OPT = [OVERRIDE_OFF, OVERRIDE_TO_TRUE, OVERRIDE_TO_FALSE]
   
   ###################
   ## CFDP Settings ##
   ###################

   CFDP_APP_ENTITY_ID = "23"     # Ground tool prepends "0."
   CFDP_GND_ENTITY_ID = "0.21"
      
   ###################
   ## TFTP Settings ##
   ###################

   TFTP_GET_TIMEOUT = 15   # Telemetry get counter verification timeout 
   TFTP_PUT_TIMEOUT = 15   # Telemetry put counter verification timeout 
   
   #######################
   ## GUI Text Messages ##
   #######################

   MSG_BUTTON_YES    = "Yes"
   MSG_BUTTON_NO     = "No"
   MSG_BUTTON_CONT   = "OK"
   MSG_BUTTON_CANCEL = "Cancel"  # Equal to hard coded message_box() string
   MSG_UNDEFSTR      = "Undefined"

   MSG_TBD_FEATURE = "Feature coming soon..."

   TXT_TUTORIAL_VIDEO  = "Tutorial Video"
   TXT_TUTORIAL_SLIDES = "Tutorial Slides"
   
   
   def Osk::time_stamp
      time = Time.new 
      time_stamp = "#{time.strftime("%m/%d/%Y")} #{time.strftime("%I:%M %p")}"
   end
   
   
   ################
   ## File Names ##
   ################
   
   # Use "_SCR" for automatically generated screen files
   
   OSK_QS_FILE      = "OSK-Quick-Start.pdf"
   OSK_APP_DEV_FILE = "OSK-App-Dev-Guide.pdf" 
                  
   DOCS_QS_RND_FILE          = "OSK-QS-RnD.pdf"
   DOCS_QS_PISAT_FILE        = "OSK-QS-PiSat.pdf"
    
   ABOUT_SCR_FILE      = "about_scr.txt"
   VERSION_SCR_FILE    = "version_scr.txt"
   ADD_APP_SCR_FILE    = "add_app_scr.txt"
   REMOVE_APP_SCR_FILE = "remove_app_scr.txt"

   TUTORIAL_DEF_FILE  = "osk_tutorials.json"
   TUTORIAL_SCR_FILE  = "tutorial_scr.txt"

   CREATE_APP_JSON_FILE     = "create-app.json"
   CREATE_APP_TEMPLATE_FILE = "app-template.json"
   CREATE_APP_SCR_FILE      = "create_app_scr.txt"
   TEMPLATE_INFO_SCR_FILE   = "template_info_scr.txt"

   # Table tools
   HK_TBL_GEN_CFG_TEMPLATE_FILE  = "hk_tbl_gen_cfg.json"
   TLM_TBL_GEN_INPUT_XLS_FILE    = "tlm_tbl_gen_inp.xlsx"
   TLM_TBL_GEN_CFG_TEMPLATE_FILE = "kit_to_ds_tbl_cfg.json"

  
   CFE_UG_FILE      = "index.html"
   CFE_APP_DEV_FILE = "cFE_Application_Developers_Guide.pdf"
   CFE_TRAINING_SLIDES_FILE = "cFS_Training-cFE_Services.pdf"
   EVS_TRAINING_SLIDES_FILE = "OSK-T02-1-cFE-EVS.pdf"
   SB_TRAINING_SLIDES_FILE  = "OSK-T02-1-cFE-SB.pdf"
   TBL_TRAINING_SLIDES_FILE = "OSK-T02-1-cFE-TBL.pdf"
   CFE_EXERCISE_SLIDES_FILE = "OSK_Training-cFE_Exercises.pdf"

   CFE_STARTUP_FILE = "cfe_es_startup.scr"
   
   OSK_TARGETS_FILE  = "targets.cmake"
   SIMSAT_STARTUP_FILE  = "#{SIMSAT_TARGET.downcase}_#{CFE_STARTUP_FILE}"
   CFSAT_STARTUP_FILE   = "#{CFSAT_TARGET.downcase}_#{CFE_STARTUP_FILE}"
   SANDBOX_STARTUP_FILE = "#{SANDBOX_TARGET.downcase}_#{CFE_STARTUP_FILE}"
   
   JSON_TBL_MGMT_SCR_FILE = "json_table_mgmt_scr.txt"
   
   TMP_BIN_FILE  = "osk_tmp_bin.dat"
   TMP_TBL_FILE  = "osk_tmp_tbl.dat"
   TMP_JSON_FILE = "osk_tmp.json"
   TMP_TXT_FILE  = "osk_tmp.txt"
   
   TMP_FLT_CMD_SCR_FILE = "osk_tmp_flt_cmd_scr.txt"
   TMP_FLT_CMD_SCR_STR  = "OSK_TMP_FLT_CMD_SCR"

   TMP_TBL_MGR_SCR_FILE = "osk_tmp_tbl_mgr_scr.txt"
   TMP_TBL_MGR_SCR_STR  = "OSK_TMP_TBL_MGR_SCR"
   
   TMP_GET_BIN_FILE_SCR_FILE = "osk_tmp_get_bin_file_scr.txt"
   TMP_GET_BIN_FILE_SCR_STR  = "OSK_TMP_GET_BIN_FILE_SCR"

   # Tag appended to dump file basename to identify as a dump file
   DMP_FILE_TAG = "~"
   
   #########################################
   ## Table Manager Definition File Names ##
   #########################################

   TBL_MGR_DEF_CFE_ES_APP_INFO  = "cfe_es_app_info.txt"
   TBL_MGR_DEF_CFE_ES_CDS_REG   = "cfe_es_cds_reg.txt"
   TBL_MGR_DEF_CFE_ES_ERLOG     = "cfe_es_erlog.txt"
   TBL_MGR_DEF_CFE_ES_SYSLOG    = "cfe_es_syslog.txt"
   TBL_MGR_DEF_CFE_ES_TASK_INFO = "cfe_es_task_info.txt"
   
   TBL_MGR_DEF_CFE_EVS_LOG      = "cfe_evs_log.txt"
   TBL_MGR_DEF_CFE_EVS_APP_INFO = "cfe_evs_app_info.txt"

   TBL_MGR_DEF_CFE_SB_PIPE    = "cfe_sb_pipe_info.txt"
   TBL_MGR_DEF_CFE_SB_ROUTES  = "cfe_sb_routes.txt"
   TBL_MGR_DEF_CFE_SB_MSG_MAP = "cfe_sb_msg_map.txt"

   TBL_MGR_DEF_CFE_TBL_REG = "cfe_tbl_reg.txt"

   TBL_MGR_DEF_CS_APP_TBL        = "cs_app_tbl.txt"
   TBL_MGR_DEF_CS_EEPROM_TBL     = "cs_eeprom_tbl.txt"
   TBL_MGR_DEF_CS_MEMORY_TBL     = "cs_memory_tbl.txt"
   TBL_MGR_DEF_CS_TABLES_TBL     = "cs_tables_tbl.txt"
   TBL_MGR_DEF_CS_APP_RES_TBL    = "cs_app_res_tbl.txt"
   TBL_MGR_DEF_CS_EEPROM_RES_TBL = "cs_eeprom_res_tbl.txt"
   TBL_MGR_DEF_CS_MEMORY_RES_TBL = "cs_memory_res_tbl.txt"
   TBL_MGR_DEF_CS_TABLES_RES_TBL = "cs_tables_res_tbl.txt"
 
   TBL_MGR_DEF_DS_FILE_TBL   = "ds_file_tbl.txt"
   TBL_MGR_DEF_DS_FILTER_TBL = "ds_filter_tbl.txt"
   TBL_MGR_DEF_DS_EVENT_LOG  = "ds_event_log.txt"
   
   TBL_MGR_DEF_FM_DIR        = "fm_dir_list.txt"
   TBL_MGR_DEF_FM_FREESPACE  = "fm_freespace_tbl.txt"
   
   TBL_MGR_DEF_HK_COPY       = "hk_copy_tbl.txt"
   
   TBL_MGR_DEF_HS_AMT_TBL = "hs_amt_tbl.txt"
   TBL_MGR_DEF_HS_EMT_TBL = "hs_emt_tbl.txt"
   TBL_MGR_DEF_HS_MAT_TBL = "hs_mat_tbl.txt"
   TBL_MGR_DEF_HS_XCT_TBL = "hs_xct_tbl.txt"
   
   TBL_MGR_DEF_MD_TBL  = "md_tbl.txt"
   TBL_MGR_DEF_MM_DMP  = "mm_dump.txt"
   
   ############################
   ## COSMOS Directory Paths ##
   ############################
   
   COSMOS_CFG_TARGET_DIR = File.join(Cosmos::USERPATH, 'config', 'targets')
   def Osk::cfg_target_dir_file(target, subdir, file)
      return File.join(cfg_target_dir(target,subdir),file)
   end
   
   def Osk::cfg_target_dir(target,subdir)
      return File.join(COSMOS_CFG_TARGET_DIR,target.upcase,subdir.downcase)
   end

   COSMOS_CFG_TOOL_DIR   = File.join(Cosmos::USERPATH, 'config', 'tools')
   COSMOS_CFG_TBL_MGR_DIR = File.join(COSMOS_CFG_TOOL_DIR,'table_manager')
   
   COSMOS_CFG_EDITOR  = "#{Cosmos::USERPATH}/tools/ConfigEditor"
   COSMOS_PKT_VIEWER  = "#{Cosmos::USERPATH}/tools/PacketViewer"
   COSMOS_SCR_RUNNER  = "#{Cosmos::USERPATH}/tools/ScriptRunner"
   COSMOS_TBL_MANAGER = "#{Cosmos::USERPATH}/tools/TableManager"
   COSMOS_TLM_GRAPHER = "#{Cosmos::USERPATH}/tools/TlmGrapher"
   COSMOS_CMD_TLM_SRV = "#{Cosmos::USERPATH}/tools/CmdTlmServer"
   COSMOS_TST_RUNNER  = "#{Cosmos::USERPATH}/tools/TestRunner"

   #########################
   ## OSK Directory Paths ##
   #########################

   # Directory paths relative to Cosmos::USERPATH
   REL_DIR_CFS  = "../cfs"
   REL_DIR_42   = "../42"
   REL_DIR_DOCS = "../docs"

   OSK_42_DIR   = File.expand_path(REL_DIR_42,Cosmos::USERPATH)
   OSK_CFS_DIR  = File.expand_path(REL_DIR_CFS,Cosmos::USERPATH)
   OSK_DOCS_DIR = File.expand_path(REL_DIR_DOCS,Cosmos::USERPATH)

   CFS_DOCS_DIR = File.join(OSK_DOCS_DIR,"cfs")
   CFE_UG_DIR   = File.join(CFS_DOCS_DIR,"cfe_users_guide")

   MISSION_FSW_DOC_DIR  = File.join(OSK_DOCS_DIR,"mission-fsw")
   PI_SAT_DOC_DIR       = File.join(OSK_DOCS_DIR,"pi-sat")
   RESEARCH_DEV_DOC_DIR = File.join(OSK_DOCS_DIR,"research-dev")

   CFS_CMAKE_DIR        = "#{OSK_CFS_DIR}/osk_defs"
   CFS_SIMSAT_EXE_DIR   = "#{OSK_CFS_DIR}/build/exe/simsat"
   CFS_SIMSAT_CF_DIR    = "#{OSK_CFS_DIR}/build/exe/simsat/cf"
   CFS_CFSAT_EXE_DIR    = "#{OSK_CFS_DIR}/build/exe/cfsat"
   CFS_CFSAT_CF_DIR     = "#{OSK_CFS_DIR}/build/exe/cfsat/cf"
   CFS_SANDBOX_EXE_DIR  = "#{OSK_CFS_DIR}/build/exe/sandbox"
   CFS_SANDBOX_CF_DIR   = "#{OSK_CFS_DIR}/build/exe/sandbox/cf"
   
   REL_SRV_DIR     = "cfs_kit/file_server"
   REL_SRV_TBL_DIR = "cfs_kit/file_server/tables"
   GND_SRV_DIR     = "#{Cosmos::USERPATH}/#{REL_SRV_DIR}"
   GND_SRV_TBL_DIR = "#{Cosmos::USERPATH}/#{REL_SRV_TBL_DIR}"

   FLT_SRV_DIR = "/cf"
   FLT_SRV_DIR_SEP = FLT_SRV_DIR + "/"
   FLT_DEMO_DIR    = File.join(Osk::FLT_SRV_DIR,"demo")
   FLT_TEST_DIR    = File.join(Osk::FLT_SRV_DIR,"test")
   GND_TO_SIMSAT_SRV_DIR  = File.join(OSK_CFS_DIR,'build','exe','simsat','cf')
   GND_TO_CFSAT_SRV_DIR   = File.join(OSK_CFS_DIR,'build','exe','cfsat','cf')
   GND_TO_SANDBOX_SRV_DIR = File.join(OSK_CFS_DIR,'build','exe','sandbox','cf')
   
   def Osk::fsw_app_dir(app)
      return File.join(Osk::OSK_CFS_DIR,'apps',app)
   end

   CFS_KIT_LIB_DIR = Osk::cfg_target_dir("CFS_KIT","lib")
   CFS_KIT_SCR_DIR = Osk::cfg_target_dir("CFS_KIT","screens")
   
   REL_TUTORIAL_DIR   = "cfs_kit/tutorials"
   TUTORIAL_DIR       = "#{Cosmos::USERPATH}/cfs_kit/tutorials"
   CFE_TRAINING_DIR   = "#{Osk::TUTORIAL_DIR}/cfe/training"
   OSK_APPS_TRAIN_DIR = "#{Osk::TUTORIAL_DIR}/op_apps"
   
   TBL_TOOLS_DIR  = "#{Cosmos::USERPATH}/lib/tbl_tools"
   TBL_HK_TEMPLATE_DIR  = "#{TBL_TOOLS_DIR}/HK/templates"
   TBL_HK_TEST_DIR      = "#{TBL_TOOLS_DIR}/HK/test"
   TBL_TLM_TEMPLATE_DIR = "#{TBL_TOOLS_DIR}/tlm/templates"
   TOOLS_DIR      = "#{Cosmos::USERPATH}/cfs_kit/tools"
   PERF_MON_DIR   = "#{TOOLS_DIR}/perf-monitor"
   CREATE_APP_DIR = "#{TOOLS_DIR}/create-app"

   PROC_DIR        = "#{Cosmos::USERPATH}/procedures"
   
   TMP_FLT_BIN_PATH_FILE = "#{FLT_SRV_DIR}/#{TMP_BIN_FILE}"
   TMP_GND_BIN_PATH_FILE = "#{GND_SRV_DIR}/#{TMP_BIN_FILE}"
    
   TMP_FLT_TBL_PATH_FILE = "#{FLT_SRV_DIR}/#{TMP_TBL_FILE}"
   TMP_GND_TBL_PATH_FILE = "#{GND_SRV_TBL_DIR}/#{TMP_TBL_FILE}"

   TMP_FLT_TXT_PATH_FILE = "#{FLT_SRV_DIR}/#{TMP_TXT_FILE}"
   TMP_GND_TXT_PATH_FILE = "#{GND_SRV_DIR}/#{TMP_TXT_FILE}"


   #################################
   ## Demo Standard Text Strings  ## 
   #################################

   DEMO_STEP_NO_INFO = "\n\nNo additional information for this demo step."

   ###################################
   ## Screen Standard Text Strings  ## 
   ###################################

   WIDGET_GND_WORK_DIR = "gnd_work_dir"
   WIDGET_FLT_WORK_DIR = "flt_work_dir"
   
   ################################
   ## Standard Cmd Tlm mnemonics ## 
   ################################
   # - ccsds.rb defines the CCSDS header constants
   
   CMD_STR_NOOP      = "NOOP"
   CMD_STR_RESET     = "RESET_CTRS"
   CMD_STR_LOAD_TBL  = "LOAD_TBL"
   CMD_STR_DUMP_TBL  = "DUMP_TBL"
   CMD_DEF_FILENAME  = TMP_BIN_FILE
   CMD_DEF_TBL_NAME  = TMP_TBL_FILE
   CMD_DEF_DIR_NAME  = FLT_SRV_DIR
   
   TLM_STR_HK_PKT    = "HK_TLM_PKT"
   TLM_STR_CMD_VLD   = "CMD_VALID_COUNT"
   TLM_STR_CMD_ERR   = "CMD_ERROR_COUNT"
   
   ############################
   ## cFS Target Descriptors ##
   ############################
   
   SIMSAT_CFS_TARGET  = { dir: CFS_SIMSAT_EXE_DIR,  exe: "core-#{SIMSAT_TARGET}"  }
   CFSAT_CFS_TARGET   = { dir: CFS_CFSAT_EXE_DIR,   exe: "core-#{CFSAT_TARGET}"   }
   SANDBOX_CFS_TARGET = { dir: CFS_SANDBOX_EXE_DIR, exe: "core-#{SANDBOX_TARGET}" }

   CFS_TARGETS = { simsat: SIMSAT_CFS_TARGET, cfsat: CFSAT_CFS_TARGET, sandbox: SANDBOX_CFS_TARGET }

end # Module Osk
