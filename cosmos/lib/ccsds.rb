###############################################################################
# CCSDS Command and Telemetry Definitions
#
# Notes:
#   1. The command and telemetry primary headers are always big endian. The
#      remainder of the packet is processor specific. The render functions
#      don't explicitly add the processor endianness to the secondary header
#      because it is assumed the packet declaration line defines the endianness.
#   TODO - Confirm secondary endianness
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require "erb"
require "hw_target"

module Ccsds

   def self.renderCmdHdr (stream_id, func_code, data_len)
      @stream_id = stream_id
      @func_code = func_code
      @ccsds_len = data_len + 1  # How CCSDS defines packet length
      # OSK 3.2: The PiSat cFS target uses cFE 2.8 (Bootes) and the secondary header is big endian.
      # Once OSK can upgrade to cFE 2.8 (after NASA Goddard apps upgraded/released) this will be removed    
      if HwTarget::ID == "PISAT"
         t = ERB.new(CFS_CMD_HDR_1)
      else
         t = ERB.new(CFS_CMD_HDR_3)
      end
      t.result(binding)
   end
    
   def self.renderTlmHdr (stream_id)
      @stream_id = stream_id
      if HwTarget::ID == "PISAT"
         t = ERB.new(CFS_TLM_HDR_PISAT)
      else
         t = ERB.new(CFS_TLM_HDR)
      end
      t.result(binding)
   end

   def self.renderCfdpPduCmdHdr (stream_id)
      @stream_id = stream_id
      t = ERB.new(CFDP_PDU_CMD_HDR)
      t.result(binding)
   end

   def self.renderIgnores ()
      t = ERB.new(IGNORE_STRING)
      t.result(binding)
   end

   # Create single definition for strings to ensure they're common
   PRI_HDR_STREAMID = "CCSDS_STREAMID"
   PRI_HDR_SEQUENCE = "CCSDS_SEQUENCE"
   PRI_HDR_LENGTH   = "CCSDS_LENGTH"
   SEC_HDR_CHECKSUM = "CCSDS_CHECKSUM"
   SEC_HDR_FUNCCODE = "CCSDS_FUNCCODE"
   SEC_HDR_SECONDS  = "CCSDS_SECONDS"
   SEC_HDR_SUBSECS  = "CCSDS_SUBSECS"
      
   # CeRes
   CFS_CMD_HDR_1 = %Q(
      APPEND_ID_PARAMETER #{PRI_HDR_STREAMID} 16 UINT MIN_UINT16 MAX_UINT16 \<%= @stream_id %\> \"Packet Identification\"   BIG_ENDIAN
      APPEND_PARAMETER    #{PRI_HDR_SEQUENCE} 16 UINT MIN_UINT16 MAX_UINT16 0xC000              \"Packet Sequence Counter\" BIG_ENDIAN
      APPEND_PARAMETER    #{PRI_HDR_LENGTH}   16 UINT MIN_UINT16 MAX_UINT16 \<%= @ccsds_len %\> \"Packet Data Length\"      BIG_ENDIAN
      APPEND_PARAMETER    #{SEC_HDR_FUNCCODE}  8 UINT MIN_UINT8  MAX_UINT8  \<%= @func_code %\> \"Command Function Code\"   BIG_ENDIAN
      APPEND_PARAMETER    #{SEC_HDR_CHECKSUM}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"CCSDS Command Checksum\"  BIG_ENDIAN
   )

   # CeRes without Big Endian
   CFS_CMD_HDR_2 = %Q(
      APPEND_ID_PARAMETER #{PRI_HDR_STREAMID} 16 UINT MIN_UINT16 MAX_UINT16 \<%= @stream_id %\> \"Packet Identification\"   BIG_ENDIAN
      APPEND_PARAMETER    #{PRI_HDR_SEQUENCE} 16 UINT MIN_UINT16 MAX_UINT16 0xC000              \"Packet Sequence Counter\" BIG_ENDIAN
      APPEND_PARAMETER    #{PRI_HDR_LENGTH}   16 UINT MIN_UINT16 MAX_UINT16 \<%= @ccsds_len %\> \"Packet Data Length\"      BIG_ENDIAN
      APPEND_PARAMETER    #{SEC_HDR_FUNCCODE}  8 UINT MIN_UINT8  MAX_UINT8  \<%= @func_code %\> \"Command Function Code\"
      APPEND_PARAMETER    #{SEC_HDR_CHECKSUM}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"CCSDS Command Checksum\"
   )

   # My original with Big Endian
   CFS_CMD_HDR_3 = %Q(
      APPEND_ID_PARAMETER #{PRI_HDR_STREAMID} 16 UINT MIN_UINT16 MAX_UINT16 \<%= @stream_id %\> \"Packet Identification\"   BIG_ENDIAN
      APPEND_PARAMETER    #{PRI_HDR_SEQUENCE} 16 UINT MIN_UINT16 MAX_UINT16 0xC000              \"Packet Sequence Counter\" BIG_ENDIAN
      APPEND_PARAMETER    #{PRI_HDR_LENGTH}   16 UINT MIN_UINT16 MAX_UINT16 \<%= @ccsds_len %\> \"Packet Data Length\"      BIG_ENDIAN
      APPEND_PARAMETER    #{SEC_HDR_CHECKSUM}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"CCSDS Command Checksum\"  BIG_ENDIAN
      APPEND_PARAMETER    #{SEC_HDR_FUNCCODE}  8 UINT MIN_UINT8  MAX_UINT8  \<%= @func_code %\> \"Command Function Code\"   BIG_ENDIAN
   )

   # My original
   CFS_CMD_HDR_4 = %Q(
      APPEND_ID_PARAMETER #{PRI_HDR_STREAMID} 16 UINT MIN_UINT16 MAX_UINT16 \<%= @stream_id %\> \"Packet Identification\"   BIG_ENDIAN
      APPEND_PARAMETER    #{PRI_HDR_SEQUENCE} 16 UINT MIN_UINT16 MAX_UINT16 0xC000              \"Packet Sequence Counter\" BIG_ENDIAN
      APPEND_PARAMETER    #{PRI_HDR_LENGTH}   16 UINT MIN_UINT16 MAX_UINT16 \<%= @ccsds_len %\> \"Packet Data Length\"      BIG_ENDIAN
      APPEND_PARAMETER    #{SEC_HDR_CHECKSUM}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"CCSDS Command Checksum\"
      APPEND_PARAMETER    #{SEC_HDR_FUNCCODE}  8 UINT MIN_UINT8  MAX_UINT8  \<%= @func_code %\> \"Command Function Code\"
   )

   
   CFS_TLM_HDR = %Q(
      APPEND_ID_ITEM    #{PRI_HDR_STREAMID} 16 UINT \<%= @stream_id %\> \"Packet Identification\"  BIG_ENDIAN
         FORMAT_STRING  "0x%04X"
      APPEND_ITEM       #{PRI_HDR_SEQUENCE} 16 UINT \"Packet Sequence Counter\"                    BIG_ENDIAN
      APPEND_ITEM       #{PRI_HDR_LENGTH}   16 UINT \"Packet Length\"                              BIG_ENDIAN
      APPEND_ITEM       #{SEC_HDR_SECONDS}  32 UINT \"Tlm Secondary Header - Seconds\"
         #FORMAT_STRING "0x%04X"
      APPEND_ITEM       #{SEC_HDR_SUBSECS}  16 UINT \"Tlm Secondary Header - Subseconds\"
         #FORMAT_STRING "0x%04X"
   )

   # APPEND_ITEM       "PRI_HDR_ALIGN"     16 UINT \"Primary Header 32-bit alignment\"            BIG_ENDIAN
   CFS_TLM_HDR_PISAT = %Q(
      APPEND_ID_ITEM    #{PRI_HDR_STREAMID} 16 UINT \<%= @stream_id %\> \"Packet Identification\"  BIG_ENDIAN
         FORMAT_STRING  "0x%04X"
      APPEND_ITEM       #{PRI_HDR_SEQUENCE} 16 UINT \"Packet Sequence Counter\"                    BIG_ENDIAN
      APPEND_ITEM       #{PRI_HDR_LENGTH}   16 UINT \"Packet Length\"                              BIG_ENDIAN
      APPEND_ITEM       #{SEC_HDR_SECONDS}  32 UINT \"Secondary Header: Seconds\"                  BIG_ENDIAN
      APPEND_ITEM       #{SEC_HDR_SUBSECS}  16 UINT \"Secondary Header: Subseconds\"               BIG_ENDIAN
      APPEND_ITEM       "SEC_HDR_ALIGN"     32 UINT \"Secondary Header: 32-bit alignment\"         BIG_ENDIAN
   )

   CFDP_PDU_CMD_HDR = %Q(
      APPEND_ID_PARAMETER #{PRI_HDR_STREAMID} 16 UINT MIN_UINT16 MAX_UINT16 \<%= @stream_id %\> \"Packet Identification\"   BIG_ENDIAN    
      APPEND_PARAMETER    #{PRI_HDR_SEQUENCE} 16 UINT MIN_UINT16 MAX_UINT16 0xC000              \"Packet Sequence Counter\" BIG_ENDIAN
      APPEND_PARAMETER    #{PRI_HDR_LENGTH}   16 UINT MIN_UINT16 MAX_UINT16 6                   \"Packet Data Length\"      BIG_ENDIAN
      APPEND_PARAMETER    #{SEC_HDR_FUNCCODE}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"Command Function Code\"   BIG_ENDIAN
      APPEND_PARAMETER    #{SEC_HDR_CHECKSUM}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"CCSDS Command Checksum\"
   )

   CFDP_PDU_HDR = %Q(
      APPEND_ID_PARAMETER #{PRI_HDR_STREAMID} 16 UINT MIN_UINT16 MAX_UINT16 \<%= @stream_id %\> \"Packet Identification\"   BIG_ENDIAN    
      APPEND_PARAMETER    #{PRI_HDR_SEQUENCE} 16 UINT MIN_UINT16 MAX_UINT16 0xC000              \"Packet Sequence Counter\" BIG_ENDIAN
      APPEND_PARAMETER    #{PRI_HDR_LENGTH}   16 UINT MIN_UINT16 MAX_UINT16 6                   \"Packet Data Length\"      BIG_ENDIAN
      APPEND_PARAMETER    #{SEC_HDR_FUNCCODE}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"Command Function Code\"   BIG_ENDIAN
      APPEND_PARAMETER    #{SEC_HDR_CHECKSUM}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"CCSDS Command Checksum\"
   )

end # module Ccsds
