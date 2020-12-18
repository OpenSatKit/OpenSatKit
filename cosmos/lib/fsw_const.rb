###############################################################################
# Flight Software Constants
#
# Notes:
#   1. Defines FSW #defines and enumerations
# 
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require "erb"

module Fsw
   module Const

      #############################
      ## CFE - Executive Service ##
      #############################

      CFE_ES_PERF_IDLE                 = 0
      CFE_ES_PERF_WAITING_FOR_TRIGGER  = 1
      CFE_ES_PERF_TRIGGERED            = 2
      CFE_ES_PERF_MAX_STATES           = 3

      CFE_ES_PERF_TRIGGER_START   = 0
      CFE_ES_PERF_TRIGGER_CENTER  = 1
      CFE_ES_PERF_TRIGGER_END     = 2
      CFE_ES_PERF_MAX_MODES       = 3

      #########################
      ## CFE - Event Service ##
      #########################

      CFE_EVS_DEBUG        = 1
      CFE_EVS_INFORMATION  = 2
      CFE_EVS_ERROR        = 3
      CFE_EVS_CRITICAL     = 4


      #########################
      ## CFE - Table Service ##
      #########################

      CFE_TBL_BufferSelect_INACTIVE = 0
      CFE_TBL_BufferSelect_ACTIVE   = 1

      #######################
      ## FM - File Manager ##
      #######################

      FM_IGNORE_CRC           = 0 
      FM_COMPUTE_CRC_16       = 2  # Not explicitly defined in the FSW but it should be

      FM_NAME_IS_INVALID      = 0
      FM_NAME_IS_NOT_IN_USE   = 1
      FM_NAME_IS_FILE_OPEN    = 2
      FM_NAME_IS_FILE_CLOSED  = 3
      FM_NAME_IS_DIRECTORY    = 4

      FM_TABLE_ENTRY_DISABLED = 0
      FM_TABLE_ENTRY_ENABLED  = 1
      FM_TABLE_ENTRY_UNUSED   = 2
      
      ########################
      ## LC - Limit Checker ##
      ########################

      LC_STATE_ACTIVE     = 1
      LC_STATE_PASSIVE    = 2
      LC_STATE_DISABLED   = 3


      ##############################
      ## OSK_C_FW - Table Manager ##
      ##############################

      OSK_PKTUTIL_FILTER_ALWAYS     = 1
	   OSK_PKTUTIL_FILTER_BY_SEQ_CNT = 2
	   OSK_PKTUTIL_FILTER_BY_TIME    = 3
	   OSK_PKTUTIL_FILTER_NEVER      = 4
      
      
      ##############################
      ## OSK_C_FW - Table Manager ##
      ##############################

      OSK_TBLMGR_STATUS_UNDEF    = 0
      OSK_TBLMGR_STATUS_VALID    = 1
      OSK_TBLMGR_STATUS_INVALID  = 2

      OSK_TBLMGR_ACTION_UNDEF    = 0
      OSK_TBLMGR_ACTION_REGISTER = 1
      OSK_TBLMGR_ACTION_LOAD     = 2
      OSK_TBLMGR_ACTION_DUMP     = 3

      OSK_TBLMGR_LOAD_REPLACE    = 0   # Replace the entire table
      OSK_TBLMGR_LOAD_UPDATE     = 1   # Update only items contained in the load file


      #####################
      ## Utility Methods ##
      #####################

      def self.get_val(const_str)
         begin
            raise ArgumentError, 'Constant argument is not a string' unless const_str.is_a? String
            if defined? (const_str) 
               @const_val = eval(const_str)  # This will raise an exception if not defined
            end
         rescue
            @const_val = UNUSED_MID
         end
    
         return @const_val
    
      end # get_val()

   end # module Const
end # module Fsw

