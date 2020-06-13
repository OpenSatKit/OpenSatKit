###############################################################################
# OSK Global
#
# Notes:
#   1. System is the primary entry point into the OpenSatKit framework. It
#      serves as a central access point for all OSK configurations and
#      definitions. Whenever possible it uses existing COSMOS definitions
#      to prevent multiple definitions
#   2. System is designed as a Singleton 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'cosmos'

module Osk

   PASSWORD = "osk"

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
   
   ###################
   ## Text Messages ##
   ###################

   MSG_BUTTON_YES    = "Yes"
   MSG_BUTTON_NO     = "No"
   MSG_BUTTON_CONT   = "OK"
   MSG_BUTTON_CANCEL = "Cancel"  # Equal to hard coded message_box() string
   MSG_UNDEFSTR      = "Undefined"

   MSG_TBD_FEATURE = "Feature coming soon..."

   ################
   ## File Names ##
   ################
   
   # Use "_SCR" for automatically generated screen files
   
   DOCS_QUICK_START_FILE  = "OSK-Quick-Start-Guide.pdf"
   DOCS_USERS_GUIDE_FILE  = "OSK-Users-Guide.pdf"

   TRAIN_OSK_INTRO_FILE       = "OSK-Training-Intro.pdf"
   TRAIN_OSK_CFE_SERVICE_FILE = "OSK-Ex-cFE_02A-cFE-Services.pdf"
   TRAIN_OSK_CFE_APP_DEV_FILE = "OSK-Ex-cFE_02B-cFE-App-Dev.pdf"
   
   TRAIN_CFS_INTRO_FILE    = "cFS_Training_01-Intro.pdf"
   TRAIN_CFE_SERVICE_FILE  = "cFS_Training_02A-cFE-Overview.pdf"
   TRAIN_CFE_APP_DEV_FILE  = "cFS_Training_02B-cFE-App-Dev.pdf"

   ABOUT_SCR_FILE   = "about_scr.txt"
   ADD_APP_SCR_FILE = "add_app_scr.txt"

   TUTORIAL_DEF_FILE  = "osk_tutorials.json"
   TUTORIAL_SCR_FILE  = "tutorial_scr.txt"

   CREATE_APP_JSON_FILE     = "create-app.json"
   CREATE_APP_TEMPLATE_FILE = "app-template.json"
   CREATE_APP_SCR_FILE      = "create_app_scr.txt"
   TEMPLATE_INFO_SCR_FILE   = "template_info_scr.txt"

   SIMSAT_OVERVIEW_FILE = "OSK-Simple-Sat.pdf"
   
   # Tutorial file format types
   TUTORIAL_HTML   = "HTML"
   TUTORIAL_PDF    = "PDF"
   TUTORIAL_SCRIPT = "SCRIPT"

   CFE_UG_FILE      = "index.html"
   CFE_APP_DEV_FILE = "cFE_Application_Developers_Guide.pdf"
   CFE_TRAINING_SLIDES_FILE = "cFS_Training-cFE_Services.pdf"
   EVS_TRAINING_SLIDES_FILE = "OSK-T02-1-cFE-EVS.pdf"
   CFE_EXERCISE_SLIDES_FILE = "OSK_Training-cFE_Exercises.pdf"

   CFE_STARTUP_FILE = "cfe_es_startup.scr"
   
   CPU1_TARGET_FILE  = "targets.cmake"
   CPU1_PLT_CFG_FILE = "cpu1_platform_cfg.h"
   CPU1_MSG_ID_FILE  = "cpu1_msgids.h"
   CPU1_OS_CFG_FILE  = "cpu1_osconfig.h"
   CPU1_STARTUP_FILE = "cpu1_cfe_es_startup.scr"
   
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

   TBL_MGR_DEF_DS_FILE_TBL   = "ds_file_tbl.txt"
   TBL_MGR_DEF_DS_FILTER_TBL = "ds_filter_tbl.txt"
   TBL_MGR_DEF_DS_EVENT_LOG  = "ds_event_log.txt"
   TBL_MGR_DEF_FM_DIR        = "fm_dir_list.txt"
   TBL_MGR_DEF_FM_FREESPACE  = "fm_freespace_tbl.txt"
   TBL_MGR_DEF_HK_COPY       = "hk_copy_tbl.txt"
   TBL_MGR_DEF_MD_TBL        = "md_tbl.txt"
   TBL_MGR_DEF_MM_DMP        = "mm_dump.txt"
   
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

   CFS_EXE_DIR     = "#{OSK_CFS_DIR}/build/exe/cpu1"
   CFS_EXE_CF_DIR  = "#{OSK_CFS_DIR}/build/exe/cpu1/cf"
   CFS_CMAKE_DIR   = "#{OSK_CFS_DIR}/osk_defs"
   
   REL_SRV_DIR     = "cfs_kit/file_server"
   REL_SRV_TBL_DIR = "cfs_kit/file_server/tables"
   GND_SRV_DIR     = "#{Cosmos::USERPATH}/#{REL_SRV_DIR}"
   GND_SRV_TBL_DIR = "#{Cosmos::USERPATH}/#{REL_SRV_TBL_DIR}"

   FLT_SRV_DIR = "/cf"
   GND_TO_FLT_SRV_DIR = File.join(OSK_CFS_DIR,'build','exe','cpu1','cf')
   
   CFS_KIT_LIB_DIR = Osk::cfg_target_dir("CFS_KIT","lib")
   CFS_KIT_SCR_DIR = Osk::cfg_target_dir("CFS_KIT","screens")
   
   CFE_DOCS_DIR   = "#{Cosmos::USERPATH}/cfs_kit/docs"
   CFE_UG_DIR     = "#{CFE_DOCS_DIR}/cfe_users_guide"

   REL_TUTORIAL_DIR  = "cfs_kit/tutorials"
   TUTORIAL_DIR      = "#{Cosmos::USERPATH}/cfs_kit/tutorials"
   CFE_TRAINING_DIR  = "#{Osk::TUTORIAL_DIR}/cfe/training"

   TOOLS_DIR      = "#{Cosmos::USERPATH}/cfs_kit/tools"
   PERF_MON_DIR   = "#{TOOLS_DIR}/perf-monitor"
   CREATE_APP_DIR = "#{TOOLS_DIR}/create-app"

   PROC_DIR        = "#{Cosmos::USERPATH}/procedures"
   
   
   TMP_FLT_BIN_PATH_FILE = "#{FLT_SRV_DIR}/#{TMP_BIN_FILE}"
   TMP_GND_BIN_PATH_FILE = "#{GND_SRV_DIR}/#{TMP_BIN_FILE}"
    
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
   CMD_DEF_DIR_NAME  = FLT_SRV_DIR
  
   TLM_STR_HK_PKT    = "HK_TLM_PKT"
   TLM_STR_CMD_VLD   = "CMD_VALID_COUNT"
   TLM_STR_CMD_ERR   = "CMD_ERROR_COUNT"
   
   
end # Module Osk
