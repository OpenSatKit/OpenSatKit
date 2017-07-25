################################################################################
# Performance Monitor (PM) Screen Scripts
#
# Notes:
#   1. Written by David McComas, licensed under the copyleft GNU
#      General Public License (GPL).
#
################################################################################

require 'cfs_kit_global'

################################################################################
## Global Variables
################################################################################

PERF_MON_TRIG_POS_START   = 0
PERF_MON_TRIG_POS_CENTER  = 1
PERF_MON_TRIG_POS_END     = 2

PERF_MON_DATA_FILE = "#{FLT_WORK_DIR}/perf_mon.dat"

################################################################################
## Launch Apps
################################################################################

def perf_mon_launch_app(screen, app)

  if (app == "PERF_MONITOR_TOOL")
    spawn("java -jar #{Cosmos::USERPATH}/config/targets/CFS_KIT/tools/perf-monitor/CPM.jar")
    #spawn("java -jar #{Cosmos::USERPATH}/../cfs/tools/perfutils-java/CPM.jar")
  else
    prompt("Error in screen definition file. Undefined commmand sent to perf_mon_launch_app()")
  end

end # perf_mon_launch_app()

################################################################################
## Send Commands
################################################################################

def perf_mon_send_cmd(screen, cmd)

	if (cmd == "START_DATA_COLLECT")
    cmd ("CFE_ES START_LA_DATA with TRIGGERMODE #{PERF_MON_TRIG_POS_START}")
	elsif (cmd == "STOP_DATA_COLLECT")
    cmd_valid_cnt = tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT")
    cmd_error_cnt = tlm("CFE_ES HK_TLM_PKT CMD_ERROR_COUNT")
    seq_cnt = tlm("CFE_ES HK_TLM_PKT CCSDS_SEQUENCE")
    cmd ("CFE_ES STOP_LA_DATA with DATAFILENAME #{PERF_MON_DATA_FILE}")  
    wait("CFE_ES HK_TLM_PKT CMD_VALID_COUNT == #{cmd_valid_cnt}+1", 10)  # Delay until updated valid cmd count or timeout
    if ( (tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT") != (cmd_valid_cnt + 1)) || 
         (tlm("CFE_ES HK_TLM_PKT CMD_ERROR_COUNT") !=  cmd_error_cnt))
      if (tlm("CFE_ES HK_TLM_PKT CCSDS_SEQUENCE") == seq_cnt)
        prompt ("No telemetry received to verify the error. Verify connection and telemetry output filter table.");
      else
        prompt ("Executive Service had an error processing the command. See event message for details.")
      end
    else
      prompt ("Successfully created #{PERF_MON_DATA_FILE}. Select 'Get File' to transfer file to COSMOS then 'Launch Tool' to use the performance analyzer tool.")
    end 
	elsif (cmd == "SET_FILTER_MASK")
    num  = ask_string("Enter filter mask number. Index in 32-bit uint mask array (0..N)")
    mask = ask_string("Enter 32-bit filter mask")
    cmd ("CFE_ES SET_LA_FILTER_MASK with FILTERMASKNUM #{num}, FILTERMASK #{mask}")  
	elsif (cmd == "SET_TRIGGER_MASK")
    num  = ask_string("Enter trigger mask number. Index in 32-bit uint trigger array (0..N)")
    mask = ask_string("Enter 32-bit trigger mask")
    cmd ("CFE_ES SET_LA_TRIG_MASK with TRIGGERMASKNUM #{num}, TRIGGERMASK #{mask}")  
  end

end # perf_mon_send_cmd()

