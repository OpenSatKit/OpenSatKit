###############################################################################
# Flight Software Configuration Parameters
#
# Notes:
#   1. All parameters are contained in this file.
#   2. TODO - Modularize files. Use dictionary? May wait until EDS available
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
  OS_MAX_TASKS     = 64

  #########
  ## cFE ##
  #########

  
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

