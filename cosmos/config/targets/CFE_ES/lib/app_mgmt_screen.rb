###############################################################################
# cFS Kit App Management 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
# 
################################################################################

require 'osk_system'
require 'osk_flight'
require 'osk_ops'
require 'fsw_config_param'


################################################################################
## Launch Applications
################################################################################

def app_mgmt_launch_app(screen, app)

   if (app == "Eclipse")
      spawn("sudo #{Cosmos::USERPATH}/../eclipse/eclipse/eclipse")
   elsif (app == "AppGen")
      spawn("java -jar #{Osk::CREATE_APP_DIR}/CreateApp.jar", :chdir => "#{Osk::CREATE_APP_DIR}")
   elsif (app == "CREATE_APP")
      display("CFS_KIT MNG_APP_DEV_SCREEN",50,50)
   elsif (app == "APP_SUMMARY")
      display("SIMSAT CFS_APP_SCREEN",50,50)
      display("SIMSAT OSK_MISS_APP_SCREEN",1500,50)
   else
      raise "Error in screen definition file. Undefined command sent to app_mgmt_launch_app()"
   end

end # app_mgmt_launch_app()

################################################################################
## Send Commands
################################################################################

def app_mgmt_send_cmd(screen, cmd)

   if (cmd == "ES_APP_TASK_INFO")
      info = combo_box("Select info to write to a file","Application", "Task")
      if (info == "Application")    
         Osk::Ops::send_flt_bin_file_cmd("CFE_ES", "WRITE_APP_INFO_TO_FILE with ", FswConfigParam::CFE_ES_DEFAULT_APP_LOG_FILE)
      elsif (info == "Task")
         Osk::Ops::send_flt_bin_file_cmd("CFE_ES", "WRITE_TASK_INFO_TO_FILE with ", FswConfigParam::CFE_ES_DEFAULT_TASK_LOG_FILE)
      end
   elsif (cmd == "EVS_ENA_APP_EVENT_TYPE")
      app_name = ask_string("Enter app name")
      bit_mask = ask_string("Enter event type bit mask (3..0) = (Critical, Error, Info, Debug)")
      Osk::flight.cfe_evs.send_cmd("ENA_APP_EVENT_TYPE with APP_NAME #{app_name}, BITMASK #{bit_mask}")
   elsif (cmd == "EVS_DIS_APP_EVENT_TYPE")
      app_name = ask_string("Enter app name")
      bit_mask = ask_string("Enter event type bit mask (3..0) = (Critical, Error, Info, Debug)")
      Osk::flight.cfe_evs.send_cmd("DIS_APP_EVENT_TYPE with APP_NAME #{app_name}, BITMASK #{bit_mask}")
   elsif (cmd == "KIT_TO_ADD_PKT")
      msg_id = ask_string("Enter message ID of packet to downlink")
      Osk::flight.send_cmd("KIT_TO","ADD_PKT with MSG_ID #{msg_id}, BUF_LIM 4")
   elsif (cmd == "ES_START_APP")
      Osk::Ops::send_flt_cmd("CFE_ES","START_APP")
      #~Cosmos.run_process("ruby tools/CmdSender -p \"CFE_ES START_APP\"")
   elsif (cmd == "ES_STOP_APP")
      app_name = ask_string("Enter app name")
      Osk::flight.cfe_es.send_cmd("STOP_APP with APP_NAME #{app_name}")
   elsif (cmd == "ES_RELOAD_APP")
      app_name = ask_string("Enter app name")
      app_file_name = ask_string("Enter full FSW path/filename of app file to be loaded.")
      Osk::flight.cfe_es.send_cmd("RELOAD_APP with APP_NAME #{app_name}, APP_FILENAME #{app_file_name}")
   elsif (cmd == "ES_SEND_APP_INFO")
      app_name = ask_string("Enter app name")
      Osk::flight.cfe_es.send_cmd("SEND_APP_INFO with APP_NAME #{app_name}")
	elsif (cmd == "GET_FILE")
      # Update screen with working directories to help remind user where tables are stored
      Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_DIR, Osk::FLT_SRV_DIR)
      if (Osk::Ops::get_flt_file_prompt(Osk::GND_SRV_DIR))
         Osk::Ops::set_work_dir_widget(screen)
      end
   elsif (cmd == "PUT_FILE")
      # Update screen with working directories to help remind user where tables are stored
      Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_DIR, Osk::FLT_SRV_DIR)
      if (Osk::Ops::put_flt_file_prompt(Osk::GND_SRV_DIR))
         Osk::Ops::set_work_dir_widget(screen)
      end
   else
     raise "Error in screen definition file. Undefined command sent to app_mgmt_send_cmd()"
  end

end # app_mgmt_send_cmd()
