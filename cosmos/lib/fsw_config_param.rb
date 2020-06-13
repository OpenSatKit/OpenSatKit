###############################################################################
# Flight Software Configuration Parameters
#
# Notes:
#   1. All configuration parameters are contained in this file.
#   2. The names are identical to the FSW #define
#   3. TODO - Modularize files. Use dictionary? Use EDS? Auto generate?
#   4. TODO - Not all defintiions are config params. See MD and OSK_APP_FW
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

  CS_USERS_GUIDE_FILE = "CFS HS User Guide Doc No 582-2013-002"

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
  
  #########################
  ## App - House Keeping ##
  #########################

  HK_COPY_TABLE_ENTRIES  = 128
  HK_COPY_TBL_NAME       = "CopyTable"
  HK_COPY_TABLE_FILENAME = "hk_cpy_tbl.tbl"
  
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

  MD_TABLE1_CFE_NAME        = "DWELL_TABLE1"
  MD_SIGNATURE_OPTION       = true            # Ena/Dis text to describe the table
  MD_SIGNATURE_FIELD_LENGTH = 32              # Number of character's in signature text
  MD_DWELL_TABLE_SIZE       = 25              # Number of dwell entries per table

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

  ########################
  ## App - OSK_APP_FSW  ##
  ########################

  # Table load command 
  OSK_TBL_REPLACE    = 0 
  OSK_TBL_UPDATE_REC = 1

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

