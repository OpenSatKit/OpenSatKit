###############################################################################
# cFS Kit Autonomy Management 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
################################################################################

require 'cfs_kit_global'
require 'cfs_kit_config' 
require 'file_transfer'

################################################################################
## Global Variables
################################################################################

MAX_RTS_ID = CfsKitConfig.get_fsw_cfg_int_param(@APP_PREFIX_STR, "SC_NUMBER_OF_RTS")

AUTONOMY_MGMT_FLT_WORK_DIR = FLT_WORK_DIR
AUTONOMY_MGMT_GND_WORK_DIR = GND_WORK_DIR

################################################################################
## Send Commands
################################################################################

def autonomy_mgmt_send_cmd(screen, cmd)

	if (cmd == "START_RTS")
    rts_id = ask_string("Enter RTS ID 1..#{MAX_RTS_ID}")
    cmd("SC START_RTS with RTS_ID #{rts_id}")
	elsif (cmd == "STOP_RTS")
    rts_id = ask_string("Enter RTS ID 1..#{MAX_RTS_ID}")
    cmd("SC STOP_RTS with RTS_ID #{rts_id}")
	elsif (cmd == "ENABLE_RTS")
    rts_id = ask_string("Enter RTS ID 1..#{MAX_RTS_ID}")
    cmd("SC ENABLE_RTS with RTS_ID #{rts_id}")
	elsif (cmd == "DISABLE_RTS")
    rts_id = ask_string("Enter RTS ID 1..#{MAX_RTS_ID}")
    cmd("SC DISABLE_RTS with RTS_ID #{rts_id}")
	elsif (cmd == "START_GROUP")
    first_rts_id = ask_string("Enter first RTS ID in the group 1..#{MAX_RTS_ID}")
    last_rts_id = ask_string("Enter last RTS ID in the group 1..#{MAX_RTS_ID}")
    cmd("SC START_RTS_GROUP with FIRST_RTS_ID #{first_rts_id}, LAST_RTS_ID #{last_rts_id}")
	elsif (cmd == "STOP_GROUP")
    first_rts_id = ask_string("Enter first RTS ID in the group 1..#{MAX_RTS_ID}")
    last_rts_id = ask_string("Enter last RTS ID in the group 1..#{MAX_RTS_ID}")
    cmd("SC STOP_RTS_GROUP with FIRST_RTS_ID #{first_rts_id}, LAST_RTS_ID #{last_rts_id}")
	elsif (cmd == "ENABLE_GROUP")
    first_rts_id = ask_string("Enter first RTS ID in the group 1..#{MAX_RTS_ID}")
    last_rts_id = ask_string("Enter last RTS ID in the group 1..#{MAX_RTS_ID}")
    cmd("SC ENABLE_RTS_GROUP with FIRST_RTS_ID #{first_rts_id}, LAST_RTS_ID #{last_rts_id}")
	elsif (cmd == "DISABLE_GROUP")
    first_rts_id = ask_string("Enter first RTS ID in the group 1..#{MAX_RTS_ID}")
    last_rts_id = ask_string("Enter last RTS ID in the group 1..#{MAX_RTS_ID}")
    cmd("SC DISABLE_RTS_GROUP with FIRST_RTS_ID #{first_rts_id}, LAST_RTS_ID #{last_rts_id}")
  elsif (app == "TODO")
    prompt("Feature coming soon...")
  else
    prompt("Error in screen definition file. Undefined commmand sent to memory_mgmt_send_cmd()")
  end
  
end # memory_mgmt_send_cmd()

