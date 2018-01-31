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

module CcsdsCmdTlm

  def self.renderCmdHdr (stream_id, func_code, data_len)
    @stream_id = stream_id
    @func_code = func_code
    @ccsds_len = data_len + 1  # How CCSDS defines packet length
    t = ERB.new(CFS_CMD_HDR_3)
    t.result(binding)
  end
    
  def self.renderTlmHdr (stream_id)
    @stream_id = stream_id
    t = ERB.new(CFS_TLM_HDR)
    t.result(binding)
  end

  def self.renderIgnores ()
    t = ERB.new(IGNORE_STRING)
    t.result(binding)
  end

  # Create single definition for strings to ensure they're common
  CCSDS_STREAMID = "CCSDS_STREAMID"
  CCSDS_SEQUENCE = "CCSDS_SEQUENCE"
  CCSDS_LENGTH   = "CCSDS_LENGTH"
  CCSDS_CHECKSUM = "CCSDS_CHECKSUM"
  CCSDS_FUNCCODE = "CCSDS_FUNCCODE"
  CCSDS_SECONDS  = "CCSDS_SECONDS"
  CCSDS_SUBSECS  = "CCSDS_SUBSECS"
      
  # CeRes
  CFS_CMD_HDR_1 = %Q(
    APPEND_ID_PARAMETER #{CCSDS_STREAMID} 16 UINT MIN_UINT16 MAX_UINT16 \<%= @stream_id %\> \"Packet Indentification\"  BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_SEQUENCE} 16 UINT MIN_UINT16 MAX_UINT16 0xC000              \"Packet Sequence Counter\" BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_LENGTH}   16 UINT MIN_UINT16 MAX_UINT16 \<%= @ccsds_len %\> \"Packet Data Length\"      BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_FUNCCODE}  8 UINT MIN_UINT8  MAX_UINT8  \<%= @func_code %\> \"Command Function Code\"   BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_CHECKSUM}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"CCSDS Command Checksum\"  BIG_ENDIAN
  )

  # CeRes without Big Endian
  CFS_CMD_HDR_2 = %Q(
    APPEND_ID_PARAMETER #{CCSDS_STREAMID} 16 UINT MIN_UINT16 MAX_UINT16 \<%= @stream_id %\> \"Packet Indentification\"  BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_SEQUENCE} 16 UINT MIN_UINT16 MAX_UINT16 0xC000              \"Packet Sequence Counter\" BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_LENGTH}   16 UINT MIN_UINT16 MAX_UINT16 \<%= @ccsds_len %\> \"Packet Data Length\"      BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_FUNCCODE}  8 UINT MIN_UINT8  MAX_UINT8  \<%= @func_code %\> \"Command Function Code\"
    APPEND_PARAMETER    #{CCSDS_CHECKSUM}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"CCSDS Command Checksum\"
  )

  # My original with Big Endian
  CFS_CMD_HDR_3 = %Q(
    APPEND_ID_PARAMETER #{CCSDS_STREAMID} 16 UINT MIN_UINT16 MAX_UINT16 \<%= @stream_id %\> \"Packet Indentification\"  BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_SEQUENCE} 16 UINT MIN_UINT16 MAX_UINT16 0xC000              \"Packet Sequence Counter\" BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_LENGTH}   16 UINT MIN_UINT16 MAX_UINT16 \<%= @ccsds_len %\> \"Packet Data Length\"      BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_CHECKSUM}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"CCSDS Command Checksum\"  BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_FUNCCODE}  8 UINT MIN_UINT8  MAX_UINT8  \<%= @func_code %\> \"Command Function Code\"   BIG_ENDIAN
  )

  # My original
  CFS_CMD_HDR_4 = %Q(
    APPEND_ID_PARAMETER #{CCSDS_STREAMID} 16 UINT MIN_UINT16 MAX_UINT16 \<%= @stream_id %\> \"Packet Indentification\"  BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_SEQUENCE} 16 UINT MIN_UINT16 MAX_UINT16 0xC000              \"Packet Sequence Counter\" BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_LENGTH}   16 UINT MIN_UINT16 MAX_UINT16 \<%= @ccsds_len %\> \"Packet Data Length\"      BIG_ENDIAN
    APPEND_PARAMETER    #{CCSDS_CHECKSUM}  8 UINT MIN_UINT8  MAX_UINT8  0                   \"CCSDS Command Checksum\"
    APPEND_PARAMETER    #{CCSDS_FUNCCODE}  8 UINT MIN_UINT8  MAX_UINT8  \<%= @func_code %\> \"Command Function Code\"
  )

  CFS_TLM_HDR = %Q(
    APPEND_ID_ITEM    #{CCSDS_STREAMID} 16 UINT \<%= @stream_id %\> \"Packet Identification\"  BIG_ENDIAN
      FORMAT_STRING  "0x%04X"
    APPEND_ITEM       #{CCSDS_SEQUENCE} 16 UINT \"Packet Sequence Counter\"                    BIG_ENDIAN
    APPEND_ITEM       #{CCSDS_LENGTH}   16 UINT \"Packet Length\"                              BIG_ENDIAN
    APPEND_ITEM       #{CCSDS_SECONDS}  32 UINT \"Tlm Secondary Header - Seconds\"
      #FORMAT_STRING "0x%04X"
    APPEND_ITEM       #{CCSDS_SUBSECS}  16 UINT \"Tlm Secondary Header - Subseconds\"
      #FORMAT_STRING "0x%04X"
  )

end # module CcsdsCmdTlm
