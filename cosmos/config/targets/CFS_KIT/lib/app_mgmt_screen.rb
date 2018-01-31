###############################################################################
# cFS Kit App Management 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
# 
################################################################################

require 'cfs_kit_global'
require 'cfs_kit_global'
require 'file_transfer'

################################################################################
## Global Variables
################################################################################

APP_MGMT_FLT_SRV_DIR = FLT_SRV_DIR
APP_MGMT_GND_SRV_DIR = GND_SRV_DIR

$file_xfer = FileTransfer.new()

################################################################################
## Launch Applications
################################################################################

def app_mgmt_launch_app(screen, app)

  if (app == "Eclipse")
    spawn("sudo #{Cosmos::USERPATH}/../eclipse/eclipse/eclipse")
  elsif (app == "AppGen")
    spawn("java -jar #{CFS_KIT_CREATE_APP_DIR}/CreateApp.jar", :chdir => "#{CFS_KIT_CREATE_APP_DIR}")
  elsif (app == "APP_SUMMARY")
    display("CFS_KIT APP_CFS_SUMMARY_SCREEN",50,50)
    display("CFS_KIT APP_KIT_SUMMARY_SCREEN",50,50)
  else
    prompt("Error in screen definition file. Undefined command sent to app_mgmt_launch_app()")
  end

end # app_mgmt_launch_app()

################################################################################
## Send Commands
################################################################################

def app_mgmt_send_cmd(screen, cmd)

  if (cmd == "ES_APP_TASK_INFO")
    info = combo_box("Select info to write to a file","Application", "Task")
    if (info == "Application")
      flt_full_file_name = ask_string("Enter full FSW path/filename.")
      cmd("CFE_ES WRITE_APP_INFO_TO_FILE with FILENAME #{flt_full_file_name}")
      gnd_file_name = ask_string("Enter ground filename without path. File will be in kit server location.")
      gnd_full_file_name = "#{GND_SRV_DIR}/#{gnd_file_name}"
      if ($file_xfer.get(flt_full_file_name,gnd_full_file_name))
        prompt ("In Table Manager select File->Open->cfs_kit/file_server #{gnd_file_name}. Select 'cfe_es_app_info.txt' for the definition file")
        spawn("ruby #{Cosmos::USERPATH}/tools/TableManager ")
      else
        prompt ("File transfer failed");
      end # If file transferred
    elsif (info == "Task")
      flt_full_file_name = ask_string("Enter full FSW path/filename.")
      cmd("CFE_ES WRITE_TASK_INFO_TO_FILE with FILENAME #{flt_full_file_name}")
      gnd_file_name = ask_string("Enter ground filename without path. File will be in kit server location.")
      gnd_full_file_name = "#{GND_SRV_DIR}/#{gnd_file_name}"
      if ($file_xfer.get(flt_full_file_name,gnd_full_file_name))
        prompt ("In Table Manager select File->Open->cfs_kit/file_server #{gnd_file_name}. Select 'cfe_es_task_info.txt' for the definition file")
        spawn("ruby #{Cosmos::USERPATH}/tools/TableManager ")
      else
        prompt ("File transfer failed");
      end # If file transferred
    end
  elsif (cmd == "EVS_ENA_APP_EVENT_TYPE")
    app_name = ask_string("Enter app name")
    bit_mask = ask_string("Enter event type bit mask (3..0) = (Critical, Error, Info, Debug)")
    cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME #{app_name}, BITMASK #{bit_mask}")
  elsif (cmd == "EVS_DIS_APP_EVENT_TYPE")
    app_name = ask_string("Enter app name")
    bit_mask = ask_string("Enter event type bit mask (3..0) = (Critical, Error, Info, Debug)")
    cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME #{app_name}, BITMASK #{bit_mask}")
  elsif (cmd == "KIT_TO_ADD_PKT")
    msg_id = ask_string("Enter message ID of packet to downlink")
    cmd("KIT_TO ADD_PKT with MSG_ID #{msg_id} BUF_LIM 4")
  elsif (cmd == "ES_START_APP")
    Cosmos.run_process("ruby tools/CmdSender -p \"CFE_ES START_APP\"")
  elsif (cmd == "ES_STOP_APP")
    app_name = ask_string("Enter app name")
    cmd("CFE_ES STOP_APP with APP_NAME #{app_name}")
  elsif (cmd == "ES_RELOAD_APP")
    app_name = ask_string("Enter app name")
    app_file_name = ask_string("Enter full FSW path/filename of app file to be loaded.")
    cmd("CFE_ES RELOAD_APP with APP_NAME #{app_name}, APP_FILENAME #{app_file_name}")
  elsif (cmd == "ES_SEND_APP_INFO")
    app_name = ask_string("Enter app name")
    cmd("CFE_ES SEND_APP_INFO with APP_NAME #{app_name}")
  else
    prompt("Error in screen definition file. Undefined command sent to app_mgmt_send_cmd()")
  end
end # app_mgmt_send_cmd()
