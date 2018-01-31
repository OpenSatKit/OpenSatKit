###############################################################################
# Core Flight Executive Definitions
#
# Notes:
#   1. CFE_FS_Header_t is defined in cfe_fs.h
#   2. CFE_TBL_File_Hdr_t is defined in cfs_tbl_filedef.h
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require "erb"

module CfeFile

  def self.renderFileHdr ()
    t = ERB.new(CFE_FILE_HDR)
    t.result(binding)
  end

  def self.renderTblHdr ()
    t = ERB.new(CFE_TBL_HDR)
    t.result(binding)
  end
  
  # CFE_FS_Header_t
  CFE_FILE_HDR = %Q(
    APPEND_PARAMETER "Content Type"     32 STRING "UNDEF"  "Header identifier. This file defines a CFE1 header" BIG_ENDIAN
    APPEND_PARAMETER "Subtype"          32   UINT 0 4294967295 0  "See CFE_FS_ES_ERLOG_SUBTYPE defined in cfe_fs.h" BIG_ENDIAN
    APPEND_PARAMETER "Length"           32   UINT 0 4294967295 0  "Length of primary header in bytes" BIG_ENDIAN
    APPEND_PARAMETER "Spacecraft ID"    32   UINT 0 4294967295 0  "Identifier of spacecraft that generated/received the file" BIG_ENDIAN
    APPEND_PARAMETER "Processor ID"     32   UINT 0 4294967295 0  "Identifier of the processor on the spacecraft that generated/received the file" BIG_ENDIAN
    APPEND_PARAMETER "Application ID"   32   UINT 0 4294967295 0  "Application identifier"  BIG_ENDIAN
    APPEND_PARAMETER "Time Seconds"     32   UINT 0 4294967295 0  "File creation timestamp (seconds)"  BIG_ENDIAN
    APPEND_PARAMETER "Time Subseconds"  32   UINT 0 4294967295 0  "File creation timestamp (sub-seconds)"  BIG_ENDIAN
    APPEND_PARAMETER "Description"     256 STRING "UNDEF"  "Description of table contents" BIG_ENDIAN
  )

  # CFE_TBL_File_Hdr_t
  CFE_TBL_HDR = %Q(
    APPEND_PARAMETER "Reserved"  32   UINT 0 4294967295 0  "Future Use"  BIG_ENDIAN
    APPEND_PARAMETER "Offset"    32   UINT 0 4294967295 0  "Byte Offset at which load should commence"  BIG_ENDIAN
    APPEND_PARAMETER "NumByte"   32   UINT 0 4294967295 0  "Number of bytes to load into table"  BIG_ENDIAN
    APPEND_PARAMETER "Name"     320   STRING "UNDEF"       "Description of table contents"  BIG_ENDIAN
  )
  

end # module CfeFile
