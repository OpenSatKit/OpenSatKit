################################################################################
# Performance Monitor (PM) Screen Scripts
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'osk_global'
require 'osk_ops'

################################################################################
## Global Variables
################################################################################

PERF_MON_TRIG_POS_START   = 0
PERF_MON_TRIG_POS_CENTER  = 1
PERF_MON_TRIG_POS_END     = 2

PERF_MON_DATA_FILE = "#{Osk::FLT_SRV_DIR}/perf_mon.dat"

################################################################################
## Launch Apps
################################################################################

def perf_mon_launch_app(screen, app)

   if (app == "PERF_MONITOR_TOOL")
      spawn("java -jar #{Osk::PERF_MON_DIR}/CPM.jar")
      #spawn("java -jar #{Cosmos::USERPATH}/../cfs/tools/perfutils-java/CPM.jar")
   else
      raise "Error in screen definition file. Undefined command sent to perf_mon_launch_app()"
   end

end # perf_mon_launch_app()

################################################################################
## Send Commands
################################################################################

def perf_mon_send_cmd(screen, cmd)

	if (cmd == "START_DATA_COLLECT")
      Osk::flight.cfe_es.send_cmd("START_LA_DATA with TRIG_MODE #{PERF_MON_TRIG_POS_START}")
	elsif (cmd == "STOP_DATA_COLLECT")
      cmd_valid_cnt = tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT")
      cmd_error_cnt = tlm("CFE_ES HK_TLM_PKT CMD_ERROR_COUNT")
      seq_cnt = tlm("CFE_ES HK_TLM_PKT CCSDS_SEQUENCE")
      Osk::flight.cfe_es.send_cmd("STOP_LA_DATA with DATA_FILENAME #{PERF_MON_DATA_FILE}") 
      wait("CFE_ES HK_TLM_PKT CMD_VALID_COUNT == #{cmd_valid_cnt}+1", 10)  # Delay until updated valid cmd count or timeout
      if ( (tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT") != (cmd_valid_cnt + 1)) || 
           (tlm("CFE_ES HK_TLM_PKT CMD_ERROR_COUNT") !=  cmd_error_cnt))
         if (tlm("CFE_ES HK_TLM_PKT CCSDS_SEQUENCE") == seq_cnt)
            prompt("No telemetry received to verify the error.\n\nVerify connection and telemetry output filter table.");
         else
            prompt("Executive Service had an error processing the command.\n\nSee event message for details.")
         end
      else
         prompt("Successfully created #{PERF_MON_DATA_FILE}. \n\nSelect 'Get File' to transfer file to COSMOS and then \n'Launch Tool' to use the performance analyzer tool.")
      end 
	elsif (cmd == "SET_FILTER_MASK")
      num  = ask_string("Enter filter mask number. Index in 32-bit uint mask array (0..N)")
      mask = ask_string("Enter 32-bit filter mask")
      Osk::flight.cfe_es.send_cmd("SET_LA_FILTER_MASK with FILTER_MASK_NUM #{num}, FILTER_MASK #{mask}")        
	elsif (cmd == "SET_TRIGGER_MASK")
      num  = ask_string("Enter trigger mask number. Index in 32-bit uint trigger array (0..N)")
      mask = ask_string("Enter 32-bit trigger mask")
      Osk::flight.cfe_es.send_cmd("SET_LA_TRIG_MASK with TRIG_MASK_NUM #{num}, TRIG_MASK #{mask}") 
	elsif (cmd == "GET_FILE")
      # Update screen with working directories to help remind user where tables are stored
      Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_DIR, Osk::FLT_SRV_DIR)
      if (Osk::Ops::get_flt_file_prompt(Osk::GND_SRV_DIR))
         Osk::Ops::set_work_dir_widget(screen)
      end
   end

end # perf_mon_send_cmd()

