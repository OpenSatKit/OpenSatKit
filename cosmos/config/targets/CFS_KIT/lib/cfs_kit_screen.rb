###############################################################################
# cFS Kit Main Screen Scripts
#
# Notes:
#   1. Global kit todo list, not just main screen
#      TODO - Use similar layout for app, file, tbl mgmt screens 
#      TODO - Display ground directory listing file after transfer to ground?
#      TODO - Add drop down menus where it makes sense
#      TODO - Is there a way to have a script run when a screen is first displayed?
#             Update working directories if user changes them
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
################################################################################

require 'cosmos'
require 'cosmos/script'
Cosmos.catch_fatal_exception do
  require 'cosmos/tools/cmd_sender/cmd_sender'
  require 'cosmos/tools/tlm_viewer/screen'
  require 'cosmos/tools/tlm_viewer/tlm_viewer'
end
require 'cfs_kit_global'
require 'file_transfer'

################################################################################
## Global Variables
################################################################################

# Used by file put/get command prototypes
FLT_TEST_GET_FILE = "#{FLT_SRV_DIR}/tf_get_test_src.txt"
FLT_TEST_PUT_FILE = "#{FLT_SRV_DIR}/tf_put_test_dst.txt"
GND_TEST_GET_FILE = "#{GND_SRV_DIR}/tf_get_test_dst.txt"
GND_TEST_PUT_FILE = "#{GND_SRV_DIR}/tf_put_test_src.txt"


################################################################################
## Launch Applications
################################################################################

def cfs_kit_launch_app(screen, app)

  if (app == "CFS")
    spawn("xfce4-terminal --default-working-directory=""#{Cosmos::USERPATH}/../cfs/build/exe/cpu1"" --execute sudo ./core-cpu1""")
    wait(3)
    cmd("KIT_TO ENABLE_TELEMETRY")
  elsif (app == "BENCHMARKS")
    display("CFS_KIT BENCHMARK_SCREEN",50,50) 
  elsif (app == "PERF_MON")
    display("CFS_KIT PERF_MON_SCREEN",50,50)
  elsif (app == "RUN_TEST_SCRIPT")
    spawn("ruby #{Cosmos::USERPATH}/tools/ScriptRunner #{Cosmos::USERPATH}/procedures/kit_test/kit_test_main.rb")
    display("CFS_KIT APP_SUMMARY_SCREEN",50,50)
	elsif (app == "MANAGE_FILES")
    display("CFS_KIT FILE_MGMT_SCREEN",50,50)
  elsif (app == "MANAGE_TABLES")
    display("CFS_KIT TABLE_MGMT_SCREEN",50,50)
  elsif (app == "MANAGE_MEMORY")
    display("CFS_KIT MEMORY_MGMT_SCREEN",50,50)
  elsif (app == "MANAGE_APPS")
    display("CFS_KIT APP_MGMT_SCREEN",50,50)
  elsif (app == "APP_SUMMARY")
    display("CFS_KIT APP_SUMMARY_SCREEN",50,50)
  elsif (app == "MANAGE_RECORDER")
    display("CFS_KIT RECORDER_MGMT_SCREEN",50,50)
  elsif (app == "MANAGE_AUTONOMY")
    display("CFS_KIT AUTONOMY_MGMT_SCREEN",50,50)
  elsif (app == "SIM_42")
    display("CFS_KIT SIM_42_SCREEN",50,50)
  elsif (app == "PISAT")
	  prompt("Please ensure you are connected to the PiSat network")
    cmd("PICONTROL STARTCFS")
    wait(2)
    cmd("KIT_TO ENABLE_TELEMETRY")
    spawn("ruby #{Cosmos::USERPATH}/tools/TlmGrapher")
    display("CFS_KIT PISAT_SCREEN", 1000, 0)
  elsif (app == "TODO")
    prompt("Feature coming soon...")
  else
    prompt("Error in screen definition file. Undefined commmand sent to cfs_kit_launch_app()")
  end

end # cfs_kit_launch_app()

################################################################################
## Launch Demo
################################################################################

def cfs_kit_launch_demo(screen, demo)

  if (demo == "FILE_MGMT_DEMO")
		display("CFS_KIT FILE_MGMT_DEMO_SCREEN",50,50)
	elsif (demo == "TABLE_DEMO")
		display("CFS_KIT TABLE_MGMT_DEMO_SCREEN",50,50) 
  elsif (demo == "MEMORY_DEMO")
		display("CFS_KIT MEMORY_MGMT_DEMO_SCREEN",50,50)
  elsif (demo == "RECORDER_DEMO")
		display("CFS_KIT RECORDER_MGMT_DEMO_SCREEN",50,50)
	elsif (demo == "AUTONOMY_DEMO")
    display("CFS_KIT HEATER_CTRL_DEMO_SCREEN",50,50)
  elsif (demo == "APP_DEMO")
		display("CFS_KIT APP_MGMT_DEMO_SCREEN",50,50)
	elsif (demo == "PERF_MON_DEMO")
    display("CFS_KIT PERF_MON_DEMO_SCREEN",50,50)
  else
    prompt("Error in screen definition file. Undefined commmand sent to cfs_kit_launch_demo()")
    #Save prompt(MSG_TBD_FEATURE)  
  end

end # cfs_kit_launch_demo()

################################################################################
## Send Commands
################################################################################

def cfs_kit_send_cmd(screen, cmd)

	if (cmd == "TO_ENA_TELEMETRY")
    cmd("KIT_TO ENABLE_TELEMETRY")
	elsif (cmd == "TIME_SET_CLOCK_ZERO")
    cmd("CFE_TIME SET_CLOCK_MET with SECONDS 0, MICROSECONDS 0"); 
    cmd("CFE_TIME SET_CLOCK with SECONDS 0, MICROSECONDS 0")
  elsif (cmd == "PROTO_NEW_CMD")
	  # TODO - Some potential new commands
    # (cmd == "DISPLAY_SB_ROUTES")
    # (cmd == "DISPLAY_MSG_IDS")
    # (cmd == "CFS_HTML_DOC")
    value = combo_box("Select the command being developed", 'Put File', 'Get File', 'Display SB Routes')
    case value
    when 'Put File'
      file_xfer = FileTransfer.new()
      file_xfer.put(GND_TEST_PUT_FILE, FLT_TEST_PUT_FILE)
    when 'Get File'
      file_xfer = FileTransfer.new()
      file_xfer.get(FLT_TEST_PUT_FILE,GND_TEST_PUT_FILE)
    when 'Display SB Routes'
      cmd("CFE_SB WRITE_ROUTING_TO_FILE with FILENAME '/cf/~sb_tmp.dat'")
    end
  else
    prompt("Error in screen definition file. Undefined commmand sent to cfs_kit_send_cmd()")
  end
end # cfs_kit_send_cmd()

#
# Useful code snippets
#
#sel = message_box("Press #{BUTTON_CONT} to continue",BUTTON_CONT)
#if (sel != BUTTON_CONT) then return end  
#
#Cosmos.run_process("ruby tools/CmdSender -p \"CFE_ES START_APP\"")
