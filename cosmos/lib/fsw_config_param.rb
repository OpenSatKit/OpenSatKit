###############################################################################
# Flight Software Configuration Parameters
#
# Notes:
#   1. All FSW configuration parameters are contained in this file.
#   2. cFS apps are inconsistent with what's defined in a "_cfg.h" file and
#      what's defined as a constant in the 'fsw/src' file directory. Some of
#      these definitions such as a default table filename are contained in this
#      file because many cFS apps define this type of constant in their 
#      "_cfg.h" files. fsw_const.rb captures FSW constants that are required
#      by the ground system. These are typically definitions like command
#      parameter options and state definitions.  
#   3. If the parameter is from a "_cfg.h" the name used in this file is 
#      identical to the FSW #define. If the parameter is from a another
#      header file the name may not be preserved. 
#   4. TODO - Modularize files. Use dictionary? Use EDS? Auto generate?
#   5. TODO - Not all defintions are config params. See MD
# 
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require "erb"

module FswConfigParam

  BOOT_PATH = "/cf"

  ##########
  ## OSAL ##
  ##########

  OS_MAX_API_NAME  = 20
  OS_MAX_FILE_NAME = 24  # OSK custom default
  OS_MAX_PATH_LEN  = 64
  OS_MAX_SYM_LEN   = 64
  OS_MAX_TASKS     = 64
  OS_MAX_NUM_OPEN_FILES = 50

  #########
  ## cFE ##
  #########
  
  CFE_MISSION_MAX_API_LEN = 20
  
  CFE_PLATFORM_SB_HIGHEST_VALID_MSGID = 0x1FFF
  CFE_PLATFORM_SB_MAX_PIPE_DEPTH = 16

  CFE_ES_CDS_MAX_NUM_ENTRIES     =  512
  CFE_ES_MAX_APPLICATIONS        =   32
  CFE_ES_ER_LOG_ENTRIES          =   20
  CFE_ES_ER_LOG_MAX_CONTEXT_SIZE =  128
  CFE_ES_SYSTEM_LOG_SIZE         = 3072
  CFE_ES_DEFAULT_APP_LOG_FILE      = "cfe_es_app_info.log"
  CFE_ES_DEFAULT_CDS_REG_DUMP_FILE = "cfe_cds_reg.log"
  CFE_ES_DEFAULT_ER_LOG_FILE       = "cfe_erlog.log"
  CFE_ES_DEFAULT_SYSLOG_FILE       = "cfe_es_syslog.log"
  CFE_ES_DEFAULT_TASK_LOG_FILE     = "cfe_es_task_info.log"
  
  CFE_EVS_LOG_MAX            =   20
  CFE_EVS_MAX_EVENT_FILTERS  =    8
  CFE_EVS_MAX_MESSAGE_LENGTH =  122
  CFE_EVS_DEFAULT_APP_DATA_FILE = "cfe_evs_app.dat"
  CFE_EVS_DEFAULT_LOG_FILE      = "cfe_evs_log.dat"   # Default is cfe_evs.log, but .dat works better with COSMOS Table Manager
  
  CFE_SB_MAX_PIPES = 64
  CFE_SB_DEFAULT_MAP_FILENAME     = "cfe_sb_msgmap.dat"
  CFE_SB_DEFAULT_PIPE_FILENAME    = "cfe_sb_pipe.dat"
  CFE_SB_DEFAULT_ROUTING_FILENAME = "cfe_sb_route.dat"
  CFE_SB_MAX_NUMBER_OF_MSG_KEYS   = CFE_PLATFORM_SB_HIGHEST_VALID_MSGID + 1

  CFE_TBL_MAX_FULL_NAME_LEN  =   40
  CFE_TBL_MAX_NAME_LENGTH    =   16
  CFE_TBL_MAX_NUM_TABLES     =  128
  CFE_TBL_DEFAULT_REG_DUMP_FILE  =  "cfe_tbl_reg.log"
  
  ####################
  ## App - Checksum ##
  ####################
  
  CS_MAX_NUM_APP_TABLE_ENTRIES    = 24
  CS_MAX_NUM_EEPROM_TABLE_ENTRIES = 16
  CS_MAX_NUM_MEMORY_TABLE_ENTRIES = 16
  CS_MAX_NUM_TABLES_TABLE_ENTRIES = 24
  
  # Not config parameters, defined in cs_tbldefs.h
  # res = results
  
  CS_APP_TBL_NAME        = "DefAppTbl"
  CS_APP_RES_TBL_NAME    = "ResAppTbl"
  
  CS_EEPROM_TBL_NAME     = "DefEepromTbl"
  CS_EEPROM_RES_TBL_NAME = "ResEepromTbl"

  CS_MEMORY_TBL_NAME     = "DefMemoryTbl"
  CS_MEMORY_RES_TBL_NAME = "ResMemoryTbl"

  CS_TABLES_TBL_NAME     = "DefTablesTbl"
  CS_TABLES_RES_TBL_NAME = "ResTablesTbl"


  ########################
  ## App - Data Storage ##
  ########################

  DS_DEST_FILE_CNT        = 16
  DS_TOTAL_FNAME_BUFSIZE  = OS_MAX_PATH_LEN
  DS_PATHNAME_BUFSIZE     = OS_MAX_PATH_LEN
  DS_BASENAME_BUFSIZE     = OS_MAX_PATH_LEN
  DS_EXTENSION_BUFSIZE    =  8
  DS_PACKETS_IN_FILTER_TABLE = 256
  DS_FILTERS_PER_PACKET   =  4
  DS_DESCRIPTOR_BUFSIZE   = 32
  DS_DESTINATION_TBL_NAME = "FILE_TBL"
  DS_DEF_DEST_FILENAME    = "ds_file_tbl.tbl"
  DS_FILTER_TBL_NAME      = "FILTER_TBL"
  DS_DEF_FILTER_FILENAME  = "ds_filter_tbl.tbl"
  
  ###################
  ## App - Example ##
  ###################

  EXAMPLE_TBL_NAME = "MyFirstTbl"
  
  ########################
  ## App - File Manager ##
  ########################

  FM_DIR_LIST_FILE_DEFNAME = "fm_dirlist.out"
  FM_DIR_LIST_PKT_ENTRIES  = 20

  FM_TABLE_CFE_NAME    = "FreeSpace"
  FM_TABLE_DEF_NAME    = "fm_freespace.tbl"
  FM_TABLE_ENTRY_COUNT = 8
  FM_TABLE_FILENAME    = "fm_freespace.tbl"
  
  FM_DIR_FILE_HDR_LEN  = 72 # Not a config parameter but FM definition
  FM_DIR_FILE_REC_LEN  = 76 # Not a config parameter but FM definition
  
  FM_DIR_LIST_FILE_ENTRIES = 200

  ###########################
  ## App - Health & Safety ##
  ###########################

  HS_MAX_MSG_ACT_TYPES    =  8
  HS_MAX_MSG_ACT_SIZE     = 16
  HS_MSG_ACT_TBL_NAME     = "MsgActs_Tbl"
  
  HS_MAX_EXEC_CNT_SLOTS   = 32
  HS_EXEC_CNT_TBL_NAME    = "ExeCount_Tbl"
  
  HS_MAX_MONITORED_APPS   = 32
  HS_APP_MON_TBL_NAME     = "AppMon_Tbl"
  
  HS_MAX_MONITORED_EVENTS = 16
  HS_EVENT_MON_TBL_NAME   = "EventMon_Tbl"
  
  HS_MAX_RESTART_ACTIONS  =  3
 
  #########################
  ## App - House Keeping ##
  #########################

  HK_COPY_TABLE_ENTRIES  = 128
  HK_COPY_TABLE_FILENAME = "hk_cpy_tbl.tbl"
  HK_COPY_TABLE_NAME     = "CopyTable"
  HK_RUNTIME_TABLE_NAME  = "RuntimeTable"
  
  #########################
  ## App - Kit Scheduler ##
  #########################

  KIT_SCH_MSGTBL_ID = 0
  KIT_SCH_SCHTBL_ID = 1
  
  KIT_SCH_SCHTBL_SLOTS               =  5
  KIT_SCH_SCHTBL_ACTIVITIES_PER_SLOT = 10
  KIT_SCH_MSGTBL_MAX_MSG_WORDS       =  8
  
  KIT_SCH_DEF_MSGTBL_FILENAME  = "osk_sch_msg_tbl.json"
  KIT_SCH_DEF_SCHTBL_FILENAME  = "osk_sch_sch_tbl.json"

  ################################
  ## App - Kit Telemetry Output ##
  ################################
    
  KIT_TO_PKTTBL_ID = 0
  KIT_TO_DEF_PKTTBL_FILENAME  = "osk_to_pkt_tbl.json"
    
  KIT_TO_PKTMGR_STATS_STARTUP_INIT_MS     = 20000
  KIT_TO_PKTMGR_STATS_RECONFIG_INIT_MS    =  5000
  KIT_TO_PKTMGR_COMPUTE_STATS_INTERVAL_MS = 10000
  
  KIT_TO_EVT_PLBK_LOG_FILE            = "kit_to_evt_log.dat"
  KIT_TO_EVT_PLBK_EVENTS_PER_TLM_MSG  = 4
  
  ##########################
  ## App - Limit Checker  ##
  ##########################

  LC_MAX_WATCHPOINTS  = 176
  LC_HKWR_NUM_BYTES   = (((LC_MAX_WATCHPOINTS  + 15) / 16) * 4)   # 2 bits per WP and keep array on longword boundary

  LC_MAX_ACTIONPOINTS = 176
  LC_HKAR_NUM_BYTES   = (((LC_MAX_ACTIONPOINTS +  7) /  8) * 4)   # 4 bits per AP and keep array on longword boundary
  

  ########################
  ## App - Memory Dwell ##
  ########################

  MD_SIGNATURE_OPTION       = true            # Ena/Dis text to describe the table
  MD_SIGNATURE_FIELD_LENGTH = 32              # Number of character's in signature text
  MD_DWELL_TABLE_SIZE       = 25              # Number of dwell entries per table

  # Table names are not in cfg file. They are hardcoded in table source file
  MD_TABLE1_CFE_NAME = "DWELL_TABLE1"  
  MD_TABLE2_CFE_NAME = "DWELL_TABLE2"  
  MD_TABLE3_CFE_NAME = "DWELL_TABLE3"  
  MD_TABLE4_CFE_NAME = "DWELL_TABLE4"  

  ##########################
  ## App - Memory Manager ##
  ##########################

  # MM_AppData+256 is a safe area in MM's global data structure that can be used
  # as a working buffer during the demo

  MM_SYMBOL = "MM_AppData"
  MM_OFFSET_W0 = 256
  MM_OFFSET_W1 = 260
  MM_OFFSET_W2 = 264
  MM_OFFSET_W3 = 268

  
  ##########################
  ## App - Stored Command ##
  ##########################

  SC_NUMBER_OF_RTS = 64
  
  
  #####################
  ## Utility Methods ##
  #####################

  def self.get_int_val(param)
    begin
      raise ArgumentError, 'Parameter argument is not an integer' unless param.is_a? String
      if defined? (param) 
        @param_int_val = eval(param)  # This will raise an exception if not defined
      end
    rescue
      #TODO - Notification 
      @param_int_val = 0
    end    
    return @param_int_val
  end # get_int_val()

  def self.get_str_val(param)
    begin
      raise ArgumentError, 'Parameter argument is not a string' unless param.is_a? String
      if defined? (param) 
        @param_str_val = eval(param)  # This will raise an exception if not defined
      end
    rescue
      #TODO - Notification 
      @param_str_val = ""
    end
    
    return @param_str_val
    
  end # get_str_val()
  
end # module FswConfigParam

