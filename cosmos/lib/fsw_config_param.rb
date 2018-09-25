###############################################################################
# Flight Software Configuration Parameters
#
# Notes:
#   1. All configuration parameters are contained in this file.
#   2. The names are identical to the FSW #define
#   3. TODO - Modularize files. Use dictionary? Use EDS? Auto generate?
# 
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require "erb"

module FswConfigParam


  ##########
  ## OSAL ##
  ##########

  OS_MAX_API_NAME  = 20
  OS_MAX_PATH_LEN  = 64
  OS_MAX_SYM_LEN   = 64
  OS_MAX_TASKS     = 64

  #########
  ## cFE ##
  #########

  CFE_ES_CDS_MAX_NUM_ENTRIES     =  512
  CFE_ES_MAX_APPLICATIONS        =   32
  CFE_ES_ER_LOG_ENTRIES          =   20
  CFE_ES_ER_LOG_MAX_CONTEXT_SIZE =  128
  CFE_ES_SYSTEM_LOG_SIZE         = 3072
  
  CFE_EVS_LOG_MAX            =   20
  CFE_EVS_MAX_EVENT_FILTERS  =    8
  CFE_EVS_MAX_MESSAGE_LENGTH =  122
  
  CFE_SB_MAX_PIPES = 64

  CFE_TBL_MAX_FULL_NAME_LEN  =   40
  CFE_TBL_MAX_NAME_LENGTH    =   16
  CFE_TBL_MAX_NUM_TABLES     =  128

  
  ########################
  ## App - Data Storage ##
  ########################

  DS_DEST_FILE_CNT       = 16
  DS_TOTAL_FNAME_BUFSIZE = OS_MAX_PATH_LEN
  DS_PATHNAME_BUFSIZE    = OS_MAX_PATH_LEN
  DS_BASENAME_BUFSIZE    = OS_MAX_PATH_LEN
  DS_EXTENSION_BUFSIZE   = 8
  DS_PACKETS_IN_FILTER_TABLE = 256
  DS_FILTERS_PER_PACKET  = 4
  
  ########################
  ## App - File Manager ##
  ########################

  FM_TABLE_CFE_NAME    = "FreeSpace"
  FM_TABLE_ENTRY_COUNT = 8
  
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

