###############################################################################
# OSK Prototype App Screen Scripts 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_flight'


################################################################################
## Global Variables
################################################################################

# Use different file name than FSW default name so it doesn't get overwritten 

TBL_ID = {"XML" => 0, "SCANF" => 1, "JSON" => 2}

XML_TBL_DMP_FILE   = "osk_proto_tbl~.xml"
SCANF_TBL_DMP_FILE = "osk_proto_tbl~.scanf"
JSON_TBL_DMP_FILE  = "osk_proto_tbl~.json"

FLT_XML_TBL_DMP_FILE = "#{Osk::FLT_SRV_DIR}/#{XML_TBL_DMP_FILE}"
GND_XML_TBL_DMP_FILE = "#{Osk::GND_SRV_TBL_DIR}/#{XML_TBL_DMP_FILE}"

FLT_SCANF_TBL_DMP_FILE = "#{Osk::FLT_SRV_DIR}/#{SCANF_TBL_DMP_FILE}"
GND_SCANF_TBL_DMP_FILE = "#{Osk::GND_SRV_TBL_DIR}/#{SCANF_TBL_DMP_FILE}"

FLT_JSON_TBL_DMP_FILE = "#{Osk::FLT_SRV_DIR}/#{JSON_TBL_DMP_FILE}"
GND_JSON_TBL_DMP_FILE = "#{Osk::GND_SRV_TBL_DIR}/#{JSON_TBL_DMP_FILE}"


################################################################################
## Screen Commands
################################################################################


def osk_c_proto_app_scr_cmd(screen, cmd)

   if (cmd == "SEND_CMD")
      Osk::Ops::send_flt_cmd("OSK_C_PROTO", "#{screen.get_named_widget("cmd").text}")   
   elsif (cmd == "NOOP")
      Osk::Ops::send_flt_cmd("OSK_C_PROTO", Osk::CMD_STR_NOOP)
   elsif (cmd == "RESET")
      Osk::Ops::send_flt_cmd("OSK_C_PROTO", Osk::CMD_STR_RESET)
   elsif (cmd == "LOAD_TBL")
      Osk::Ops::load_json_tbl("OSK_C_PROTO",TBL_ID[screen.get_named_widget("tbl_id").text])
   elsif (cmd == "DUMP_TBL")
      Osk::Ops::dump_json_tbl("OSK_C_PROTO",TBL_ID[screen.get_named_widget("tbl_id").text])
   elsif (cmd == "DISPLAY_TBL")
      Osk::Ops::display_json_tbl("OSK_C_PROTO",TBL_ID[screen.get_named_widget("tbl_id").text])
   else
      prompt("Error in screen definition file. Undefined command sent to osk_c_proto_app_scr_cmd()")
   end
   
end # osk_c_proto_app_scr_cmd()

