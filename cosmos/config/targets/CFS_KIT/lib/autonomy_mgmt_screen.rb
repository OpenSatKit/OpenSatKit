###############################################################################
# cFS Kit Autonomy Management 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
################################################################################

require 'osk_config'
require 'osk_system'
require 'osk_flight'
require 'osk_ops'

################################################################################
## Global Variables
################################################################################

MAX_RTS_ID = Osk::Cfg.get_fsw_cfg_int_param(@APP_PREFIX_STR, "SC_NUMBER_OF_RTS")

MAX_AP_ID = Osk::Cfg.get_fsw_cfg_int_param(@APP_PREFIX_STR, "LC_MAX_ACTIONPOINTS") - 1
MAX_WP_ID = Osk::Cfg.get_fsw_cfg_int_param(@APP_PREFIX_STR, "LC_MAX_WATCHPOINTS")  - 1


################################################################################
## Send Commands
################################################################################

def autonomy_mgmt_send_cmd(screen, cmd)

	if (cmd == "START_RTS")
      rts_id = ask_string("Enter RTS ID 1..#{MAX_RTS_ID}")
      Osk::flight.send_cmd("SC","START_RTS with RTS_ID #{rts_id}")
	elsif (cmd == "STOP_RTS")
      rts_id = ask_string("Enter RTS ID 1..#{MAX_RTS_ID}")
      Osk::flight.send_cmd("SC","STOP_RTS with RTS_ID #{rts_id}")
	elsif (cmd == "ENABLE_RTS")
      rts_id = ask_string("Enter RTS ID 1..#{MAX_RTS_ID}")
      Osk::flight.send_cmd("SC","ENABLE_RTS with RTS_ID #{rts_id}")
	elsif (cmd == "DISABLE_RTS")
      rts_id = ask_string("Enter RTS ID 1..#{MAX_RTS_ID}")
      Osk::flight.send_cmd("SC","DISABLE_RTS with RTS_ID #{rts_id}")
	elsif (cmd == "START_GROUP")
      first_rts_id = ask_string("Enter first RTS ID in the group 1..#{MAX_RTS_ID}")
      last_rts_id = ask_string("Enter last RTS ID in the group 1..#{MAX_RTS_ID}")
      Osk::flight.send_cmd("SC","START_RTS_GROUP with FIRST_RTS_ID #{first_rts_id}, LAST_RTS_ID #{last_rts_id}")
	elsif (cmd == "STOP_GROUP")
      first_rts_id = ask_string("Enter first RTS ID in the group 1..#{MAX_RTS_ID}")
      last_rts_id = ask_string("Enter last RTS ID in the group 1..#{MAX_RTS_ID}")
      Osk::flight.send_cmd("SC","STOP_RTS_GROUP with FIRST_RTS_ID #{first_rts_id}, LAST_RTS_ID #{last_rts_id}")
	elsif (cmd == "ENABLE_GROUP")
      first_rts_id = ask_string("Enter first RTS ID in the group 1..#{MAX_RTS_ID}")
      last_rts_id = ask_string("Enter last RTS ID in the group 1..#{MAX_RTS_ID}")
      Osk::flight.send_cmd("SC","ENABLE_RTS_GROUP with FIRST_RTS_ID #{first_rts_id}, LAST_RTS_ID #{last_rts_id}")
	elsif (cmd == "DISABLE_GROUP")
      first_rts_id = ask_string("Enter first RTS ID in the group 1..#{MAX_RTS_ID}")
      last_rts_id = ask_string("Enter last RTS ID in the group 1..#{MAX_RTS_ID}")
      Osk::flight.send_cmd("SC","DISABLE_RTS_GROUP with FIRST_RTS_ID #{first_rts_id}, LAST_RTS_ID #{last_rts_id}")
	elsif (cmd == "RESET_WP_STATS")
      wp_id = ask_string("Enter Watch Point ID 0..#{MAX_WP_ID}")
      Osk::flight.send_cmd("LC","RESET_WP_STATS with WP_ID #{wp_id}")
	elsif (cmd == "RESET_AP_STATS")
      ap_id = ask_string("Enter Action Point ID 0..#{MAX_AP_ID}")
      Osk::flight.send_cmd("LC","RESET_AP_STATS with AP_ID #{ap_id}")
	elsif (cmd == "SET_AP_STATE")
      ap_id    = ask_string("Enter Action Point ID 0..#{MAX_AP_ID}")
      ap_state = ask_string("Enter State: 1=Active, 2=Passive, 3=Disabled")
      Osk::flight.send_cmd("LC","SET_AP_STATE with AP_ID #{ap_id}, NEW_STATE #{ap_state}")
	elsif (cmd == "SET_AP_PERM_OFF")
      ap_id = ask_string("Enter Action Point ID 0..#{MAX_AP_ID}")
      Osk::flight.send_cmd("LC","SET_AP_PERM_OFF with AP_ID #{ap_id}")
	elsif (cmd == "SET_APP_STATE")
      app_state = ask_string("Enter State: 1=Active, 2=Passive, 3=Disabled")
      Osk::flight.send_cmd("LC","SET_APP_STATE with NEW_STATE #{app_state}")
   elsif (cmd == "TODO")
      prompt(Osk::MSG_TBD_FEATURE)
   else
      raise "Error in screen definition file. Undefined command sent to autonomy_mgmt_send_cmd()"
   end
  
end # autonomy_mgmt_send_cmd()

