###############################################################################
# cFS Kit App Management 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
# 
################################################################################

require 'cfs_kit_global'

################################################################################
## Global Variables
################################################################################


################################################################################
## Launch Applications
################################################################################

def app_mgmt_launch_app(screen, app)

  if (app == "Eclipse")
		spawn("sudo #{Cosmos::USERPATH}/../eclipse/eclipse/eclipse")
  elsif (app == "AppGen")
		spawn("java -jar #{CFS_KIT_GEN_APP_DIR}/CreateApp.jar")
  elsif (app == "APP_SUMMARY")
    display("CFS_KIT APP_SUMMARY_SCREEN",50,50)
  else
    prompt("Error in screen definition file. Undefined commmand sent to app_mgmt_launch_app()")
  end

end # app_mgmt_launch_app()

################################################################################
## Send Commands
################################################################################

def app_mgmt_send_cmd(screen, cmd)

  if (cmd == "ES_DISPLAY_APPS")
    prompt(MSG_TBD_FEATURE)
  elsif (cmd == "EVS_ENA_APP_EVENT_TYPE")
    app_name = ask_string("Enter app name")
    bit_mask = ask_string("Enter event type bit mask (0..3) = (Debug, Info, Error, Critical)")
    cmd("CFE_EVS ENA_APP_EVENT_TYPE with APPNAME #{app_name}, BITMASK #{bit_mask}")
  elsif (cmd == "EVS_DIS_APP_EVENT_TYPE")
    app_name = ask_string("Enter app name")
    bit_mask = ask_string("Enter event type bit mask (0..3) = (Debug, Info, Error, Critical)")
    cmd("CFE_EVS DIS_APP_EVENT_TYPE with APPNAME #{app_name}, BITMASK #{bit_mask}")
  elsif (cmd == "KIT_TO_ADD_PKT")
    msg_id = ask_string("Enter message ID of packet to downlink")
    cmd("KIT_TO ADD_PKT with MSG_ID #{msg_id} BUF_LIM 4")
  elsif (cmd == "ES_START_APP")
    Cosmos.run_process("ruby tools/CmdSender -p \"CFE_ES START_APP\"")
  elsif (cmd == "ES_STOP_APP")
    app_name = ask_string("Enter app name")
    cmd("CFE_ES STOP_APP with APPLICATION #{app_name}")
  elsif (cmd == "ES_RELOAD_APP")
    app_name = ask_string("Enter app name")
    app_file_name = ask_string("Enter full FSW path/filename of app file to be loaded.")
    cmd("CFE_ES RELOAD_APP with APPLICATION #{app_name}, APPFILENAME #{app_file_name}")
  elsif (cmd == "ES_QUERY_APP")
    app_name = ask_string("Enter app name")
    cmd("CFE_ES QUERY_APP with APPLICATION #{app_name}")
  else
    prompt("Error in screen definition file. Undefined commmand sent to app_mgmt_send_cmd()")
  end
end # app_mgmt_send_cmd()
