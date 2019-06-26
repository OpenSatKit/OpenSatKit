###############################################################################
# Message Identifiers
#
# Notes:
#   1. All parameters types are strings since the typical caller context is to
#      use the message ID in a cmd or tlm string definition
# 
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require "erb"

module Fsw
   module MsgId

      # 
      # Default value that shouldn't be used but is made available so an assignment
      # can be made in ambiguous situations. This value should be rejected by the 
      # cFS since it is unused and it will be an obvious message id value error.
      #
      UNUSED_MID = "0x0000"  

      ##########################
      ## cFE App Message IDs  ##
      ##########################

      # Executive Services
      
      CFE_ES_CMD_MID          = "0x1806"
      CFE_ES_HK_TLM_MID       = "0x0800"
      CFE_ES_APP_TLM_MID      = "0x080B"
      CFE_ES_SHELL_TLM_MID    = "0x080F"
      CFE_ES_MEMSTATS_TLM_MID = "0x0810"
  
      # Event Services
      
      CFE_EVS_CMD_MID         = "0x1801"
      CFE_EVS_HK_TLM_MID      = "0x0801"
      CFE_EVS_EVENT_MSG_MID   = "0x0808"
  
      # Software Bus
      
      CFE_SB_CMD_MID          = "0x1803"  
      CFE_SB_HK_TLM_MID       = "0x0803"
      CFE_SB_STATS_TLM_MID    = "0x080A"
      CFE_SB_ALLSUBS_TLM_MID  = "0x080D"
      CFE_SB_ONESUB_TLM_MID   = "0x080E"
  
      # Table Service
      
      CFE_TBL_CMD_MID         = "0x1804"
      CFE_TBL_HK_TLM_MID      = "0x0804"
      CFE_TBL_REG_TLM_MID     = "0x080C"
  
      # Time Service
      
      CFE_TIME_CMD_MID        = "0x1805"
      CFE_TIME_HK_TLM_MID     = "0x0805"
      CFE_TIME_DIAG_TLM_MID   = "0x0806"

      #########################
      ## cFS App Message IDs ##
      #########################
  
      # CFDP
  
      CF_CMD_MID              = "0x18B3"
      CF_SEND_HK_MID          = "0x18B4" 
      CF_WAKE_UP_REQ_CMD_MID  = "0x18B5"
      CF_SPARE1_CMD_MID       = "0x18B6"
      CF_SPARE2_CMD_MID       = "0x18B7"
      CF_SPARE3_CMD_MID       = "0x18B8"
      CF_SPARE4_CMD_MID       = "0x18B9"
      CF_SPARE5_CMD_MID       = "0x18BA"
      CF_INCOMING_PDU_MID     = "0x1FFD"

      CF_HK_TLM_MID           = "0x08B0"
      CF_TRANS_TLM_MID        = "0x08B1"
      CF_CONFIG_TLM_MID       = "0x08B2"
      CF_SPARE0_TLM_MID       = "0x08B3"
      CF_SPARE1_TLM_MID       = "0x08B4"
      CF_SPARE2_TLM_MID       = "0x08B5"
      CF_SPARE3_TLM_MID       = "0x08B6"
      CF_SPARE4_TLM_MID       = "0x08B7"
      
      CF_SPACE_TO_GND_PDU_MID = "0x0FFD"
      
      # Checksum
  
      CS_CMD_MID              = "0x189F"  
      CS_HK_TLM_MID           = "0x08A4"

      # Data Storage

      DS_CMD_MID              = "0x18BB"
      DS_HK_TLM_MID           = "0x08B8"
      DS_DIAG_TLM_MID         = "0x08B9"
  
  
      # File Manager  
  
      FM_CMD_MID              = "0x188C"
      FM_HK_TLM_MID           = "0x088A"
      FM_FILE_INFO_TLM_MID    = "0x088B"  
      FM_DIR_LIST_TLM_MID     = "0x088C"
      FM_OPEN_FILES_TLM_MID   = "0x088D"
      FM_FREE_SPACE_TLM_MID   = "0x088E"

      # Housekeeping
  
      HK_CMD_MID              = "0x189A"
      HK_HK_TLM_MID           = "0x089B"
      HK_COMBINED_PKT1_MID    = "0x089C"
      HK_COMBINED_PKT2_MID    = "0x089D"
      HK_COMBINED_PKT3_MID    = "0x089E"
      HK_COMBINED_PKT4_MID    = "0x089F"

      # Health & Safety
  
      HS_CMD_MID              = "0x18AE"
      HS_HK_TLM_MID           = "0x08AD"

      # Limit Checker
  
      LC_CMD_MID              = "0x18A4"
      LC_SAMPLE_AP_MID        = "0x18A6"
      LC_HK_TLM_MID           = "0x08A7"
  
      # Memory Dwell  
  
      MD_CMD_MID              = "0x1890"
      MD_HK_TLM_MID           = "0x0890"
      MD_DWELL_PKT_MID_1      = "0x0891"
      MD_DWELL_PKT_MID_2      = "0x0892"
      MD_DWELL_PKT_MID_3      = "0x0893"
      MD_DWELL_PKT_MID_4      = "0x0894"

      # Memory Manager
  
      MM_CMD_MID              = "0x1888"
      MM_HK_TLM_MID           = "0x0887"
  
      # Stored Command

      SC_CMD_MID              = "0x18A9"
      SC_1HZ_WAKEUP_MID       = "0x18AB"
      SC_HK_TLM_MID           = "0x08AA"
  
      #########################
      ## Kit App Message IDs ##
      #########################
 
      # Benchmark
  
      BM_CMD_MID     = "0x19F0"
      BM_HK_TLM_MID	= "0x09F0"

      # F42 - 42 Simulator FSW
  
      F42_CMD_MID      = "0x19D0"
      F42_HK_TLM_MID   = "0x09D0"
      F42_CONTROL_MID  = "0x09D1"
      F42_SENSOR_MID   = "0x09E0"
      F42_ACTUATOR_MID = "0x09E1"
  
      # Heater Control
  
      HC_CMD_MID    = "0x19A5"
      HC_HK_TLM_MID = "0x0A40"

      # Heater Simulation
  
      HSIM_CMD_MID      = "0x19B1"
      HSIM_HK_TLM_MID   = "0x09B1"
      HSIM_HC_DATA_MID  = "0x19B4"
  
      # I42 - 42 Simulator Interface
  
      I42_CMD_MID      = "0x19D2"
      I42_HK_TLM_MID   = "0x09D2"

      # ISIM - Budget Sat (Bsat) Instrument Simulator (Isim)
  
      ISIM_CMD_MID      = "0x1802"
      ISIM_HK_TLM_MID   = "0x0882"
      
      # Kit Command Ingest
  
      KIT_CI_CMD_MID    = "0x1884"
      KIT_CI_HK_TLM_MID = "0x0884"

      # Kit Scheduler
  
      KIT_SCH_CMD_MID    = "0x1895"
      KIT_SCH_HK_TLM_MID = "0x0899"
  
      # Kit Telemetry Output

      KIT_TO_CMD_MID           = "0x1880"
      KIT_TO_HK_TLM_MID        = "0x0880"
      KIT_TO_DATA_TYPE_TLM_MID = "0x0881"

      # OpenSatKit Demo App 
  
      OSK_DEMO_CMD_MID    = "0x1866"
      OSK_DEMO_HK_TLM_MID = "0x0F03"
      OSK_DEMO_FR_TLM_MID = "0x0F04"

      # OpenSatKit Create App tool
      #
      # - Use cFE's Sample App MIDs as defaults
      #   for the Create App tool  
      #

      OSK_GENAPP_CMD_MID    = "0x1883"
      OSK_GENAPP_HK_TLM_MID = "0x0883"

      # Trivial File Transfer Protocol
  
      TFTP_CMD_MID    = "0x1900"
      TFTP_HK_TLM_MID = "0x0900" 
  
      ###############
      ## PiSat Kit ##
      ###############

      # Hardware Bus
  
      HB_CMD_MID    = "0x1882"
      HB_HK_TLM_MID = "0x0885"
  
      #####################
      ## Utility Methods ##
      #####################

      def self.get_msg_val(msg_id)
         begin
            raise ArgumentError, 'Message id argument is not a string' unless msg_id.is_a? String
            if defined? (msg_id) 
               @msg_val = eval(msg_id)  # This will raise an exception if not defined
            end
         rescue
            @msg_val = UNUSED_MID
         end
    
         return @msg_val
    
      end # get_msg_val()
    
   end # module MsgeId
end # module Fsw

