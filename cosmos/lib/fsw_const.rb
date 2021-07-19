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
      
      CFE_EVS_DEBUG_MASK       = 0b0001
      CFE_EVS_INFORMATION_MASK = 0b0010
      CFE_EVS_ERROR_MASK       = 0b0100
      CFE_EVS_CRITICAL_MASK    = 0b1000
      
      CFE_EVS_NO_FILTER        = 0x0000       # Stops any filtering.  All messages are sent.
      CFE_EVS_FIRST_ONE_STOP   = 0xFFFF       # Sends the first event.  All remaining messages are filtered.
      CFE_EVS_FIRST_TWO_STOP   = 0xFFFE       # Sends the first 2 events.  All remaining messages are filtered.
      CFE_EVS_FIRST_4_STOP     = 0xFFFC       # Sends the first 4 events.  All remaining messages are filtered.
      CFE_EVS_FIRST_8_STOP     = 0xFFF8       # Sends the first 8 events.  All remaining messages are filtered.
      CFE_EVS_FIRST_16_STOP    = 0xFFF0       # Sends the first 16 events.  All remaining messages are filtered.
      CFE_EVS_FIRST_32_STOP    = 0xFFE0       # Sends the first 32 events.  All remaining messages are filtered.
      CFE_EVS_FIRST_64_STOP    = 0xFFC0       # Sends the first 64 events.  All remaining messages are filtered.
      CFE_EVS_EVERY_OTHER_ONE  = 0x0001       # Sends every other event.
      CFE_EVS_EVERY_OTHER_TWO  = 0x0002       # Sends two, filters one, sends two, filters one, etc
      CFE_EVS_EVERY_FOURTH_ONE = 0x0003       # Sends every fourth event message.  All others are filtered.

      #########################
      ## CFE - Table Service ##
      #########################

      CFE_TBL_BufferSelect_INACTIVE = 0
      CFE_TBL_BufferSelect_ACTIVE   = 1

      ########################
      ## CFE - Time Service ##
      ########################

      CFE_TIME_FLY_ON_EID  = 20
      CFE_TIME_FLY_OFF_EID = 21

      ###################
      ## CS - Checksum ##
      ###################


      # cs_msgdefs.h - Table & table entry states
      # The comments for EMPTY and UNDEFINED are from the header and I find them ocnfusing
      # especially since the default table uses 'CS_STATE_EMPTY' seems to me to be unused
      # rather than an invalid state
      
      CS_STATE_EMPTY     = 0x00  # The entry is not valid
      CS_STATE_ENABLED   = 0x01  # Entry is enabled to be checksummed or the table is enabled
      CS_STATE_DISABLED  = 0x02  # Entry is disabled for checksumming or the table is disabled
      CS_STATE_UNDEFINED = 0x03  # The entry is not in the table so the state is undefined

      # cs_msgdefs.h - Table Number Definitions
      CS_CFECORE      = 0
      CS_OSCORE       = 1
      CS_EEPROM_TABLE = 2
      CS_MEMORY_TABLE = 3
      CS_TABLES_TABLE = 4   
      CS_APP_TABLE    = 5
      CS_NUM_TABLES   = 6

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
      
      ##########################
      ## HS - Health & Safety ##
      ##########################
      
      HS_STATE_DISABLED    = 0
      HS_STATE_ENABLED     = 1

      HS_LOADED_XCT        = 0x01
      HS_LOADED_MAT        = 0x02
      HS_LOADED_AMT        = 0x04
      HS_LOADED_EMT        = 0x08
      HS_CDS_IN_USE        = 0x10

      HS_INVALID_EXECOUNT  = 0xFFFFFFFF

      # hs_tbldefs.h

      HS_AMT_ACT_NOACT        =  0    # No action is taken
      HS_AMT_ACT_PROC_RESET   =  1    # Generates Processor Reset on failure
      HS_AMT_ACT_APP_RESTART  =  2    # Attempts to restart application on failure
      HS_AMT_ACT_EVENT        =  3    # Generates event message on failure
      HS_AMT_ACT_LAST_NONMSG  =  3    # Index for finding end of non-message actions

      HS_EMT_ACT_NOACT        =  0    # No action is taken
      HS_EMT_ACT_PROC_RESET   =  1    # Generates Processor Reset on detection
      HS_EMT_ACT_APP_RESTART  =  2    # Attempts to restart application on detection
      HS_EMT_ACT_APP_DELETE   =  3    # Deletes application on detection
      HS_EMT_ACT_LAST_NONMSG  =  3    # Index for finding end of non-message actions

      HS_XCT_TYPE_NOTYPE      =  0    # No type
      HS_XCT_TYPE_APP_MAIN    =  1    # Counter for Application Main task
      HS_XCT_TYPE_APP_CHILD   =  2    # Counter for Application Child task
      HS_XCT_TYPE_DEVICE      =  3    # Counter for Device Driver
      HS_XCT_TYPE_ISR         =  4    # Counter for Interrupt Service Routine

      HS_MAT_STATE_DISABLED   =  0    # Message Actions are Disabled
      HS_MAT_STATE_ENABLED    =  1    # Message Actions are Enabled
      HS_MAT_STATE_NOEVENT    =  2    # Message Actions are Enabled but produce no events

      HS_AMTVAL_NO_ERR        =  0    # No error
      HS_AMTVAL_ERR_ACT       = -1    # Invalid ActionType specified
      HS_AMTVAL_ERR_NUL       = -2    # Null Safety Buffer not Null

      HS_EMTVAL_NO_ERR        =  0    # No error
      HS_EMTVAL_ERR_ACT       = -1    # Invalid ActionType specified
      HS_EMTVAL_ERR_NUL       = -2    # Null Safety Buffer not Null

      HS_XCTVAL_NO_ERR        =  0    # No error
      HS_XCTVAL_ERR_TYPE      = -1    # Invalid Counter Type specified
      HS_XCTVAL_ERR_NUL       = -2    # Null Safety Buffer not Null

      HS_MATVAL_NO_ERR        =  0    # No error
      HS_MATVAL_ERR_ID        = -1    # Invalid Message ID specified
      HS_MATVAL_ERR_LEN       = -2    # Invalid Length specified
      HS_MATVAL_ERR_ENA       = -3    # Invalid Enable State specified



      ########################
      ## LC - Limit Checker ##
      ########################

      LC_STATE_ACTIVE     = 1
      LC_STATE_PASSIVE    = 2
      LC_STATE_DISABLED   = 3

      #########################
      ## MM - Memory Manager ##
      #########################

      MM_MEM_TYPE_UNDEF  = 0
      MM_MEM_TYPE_RAM    = 1
      MM_MEM_TYPE_EEPROM = 2


      ###############################
      ## OSK_C_FW - Packet Utility ##
      ###############################

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

