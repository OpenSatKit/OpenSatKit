###############################################################################
# OpenSatKit Configuration
#
# Notes:
#   1. This module is a central container for all of the kit components and 
#      access to all components must be done through this interface.
#   2. TODO - Use app_prefix to access app's data. Currently a flat structure.
# 
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require "erb"
require "hw_target"
require "ccsds"
require "cfe_file"
require "fsw_msg_id"
require "fsw_perf_id"
require "fsw_config_param"

module Osk
module Cfg
   
  # @return Current hardware platform's processor endianness
  def self.processor_endian
    return HwTarget::PROCESSOR_ENDIAN
  end

  # @return Current hardware platform's cpu address size
  def self.cpu_addr_size
    return HwTarget::CPU_ADDR_SIZE
  end

  # @return CCSDS command header string for COSMOS command definition file
  def self.cmd_hdr(app_prefix, msg_id, func_code, data_len)
    return Ccsds.renderCmdHdr(Fsw::MsgId.get_val(msg_id), func_code, data_len)
  end
    
  # @return CCSDS telemetry header string for COSMOS command definition file
  def self.tlm_hdr(app_prefix, msg_id)
    return Ccsds.renderTlmHdr(Fsw::MsgId.get_val(msg_id))
  end

  # @return CCSDS CFDP PDU header string for COSMOS command definition file
  def self.cfdp_pdu_cmd_hdr(app_prefix, msg_id)
    return Ccsds.renderCfdpPduCmdHdr(Fsw::MsgId.get_val(msg_id))
  end

  # @return file header string for COSMOS table definition file used to represent a binary cFE file
  def self.file_hdr
    return CfeFile.renderFileHdr
  end

  # @return table header string for COSMOS table definition file
  def self.tbl_hdr(app_prefix, tbl_name)
    return CfeFile.renderFileHdr + CfeFile.renderTblHdr
  end
  
  # @return cFS component's configuration parameter 
  def self.get_fsw_cfg_int_param(app_prefix, param)
    return FswConfigParam.get_int_val(param)
  end
  
  # @return cFS component's configuration parameter 
  def self.get_fsw_cfg_str_param(app_prefix, param)
    return FswConfigParam.get_str_val(param)
  end
 
end # module Cfg
end # module Osk
