###############################################################################
# cFS Kit Recorder Management 
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

DS_DEST_FILE_CNT = Osk::Cfg.get_fsw_cfg_int_param(@APP_PREFIX_STR, "DS_DEST_FILE_CNT")
DEST_FILE_TBL_MAX_IDX = DS_DEST_FILE_CNT.to_i - 1

DS_PACKETS_IN_FILTER_TABLE = Osk::Cfg.get_fsw_cfg_int_param(@APP_PREFIX_STR, "DS_PACKETS_IN_FILTER_TABLE")
FILTER_TBL_MAX_IDX = DS_PACKETS_IN_FILTER_TABLE - 1

DS_FILTERS_PER_PACKET = Osk::Cfg.get_fsw_cfg_int_param(@APP_PREFIX_STR, "DS_PACKETS_IN_FILTER_TABLE")
FILTER_PARAM_MAX_IDX = DS_FILTERS_PER_PACKET - 1

################################################################################
## Send Commands
################################################################################

def recorder_mgmt_send_cmd(screen, cmd)

	if (cmd == "APP_STATE")
      app_state = ask_string("Configure Data Storage App: 0=Disable, 1=Enable","1")
      Osk::flight.send_cmd("DS","SET_APP_STATE with APP_STATE #{app_state}")
	elsif (cmd == "FILES_1_4_INFO")
      display("DS FILE_1_4_INFO_SCREEN",1500,50)
      Osk::flight.send_cmd("DS","SEND_FILE_INFO")
	elsif (cmd == "FILES_5_8_INFO")
      display("DS FILE_5_8_INFO_SCREEN",1500,50)
      Osk::flight.send_cmd("DS","SEND_FILE_INFO")
	elsif (cmd == "FILE_STATE")
      file_tbl_idx = ask_string("Enter destination file index 0..#{DEST_FILE_TBL_MAX_IDX}","0")
      file_state   = ask_string("Configure Destination File: 0=Disable, 1=Enable","1")
      Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX #{file_tbl_idx}, FILE_STATE #{file_state}")
	elsif (cmd == "FILE_COUNT")
      file_tbl_idx = ask_string("Enter destination file index 0..#{DEST_FILE_TBL_MAX_IDX}","0")
      sequence_cnt = ask_string("Enter sequence count portion fo filename","0")
      Osk::flight.send_cmd("DS","SET_FILE_COUNT with FILE_TBL_IDX #{file_tbl_idx}, SEQUENCE_CNT #{sequence_cnt}")
	elsif (cmd == "FILE_TYPE")
      file_tbl_idx  = ask_string("Enter destination file index 0..#{DEST_FILE_TBL_MAX_IDX}","0")
      filename_type = ask_string("Enter sequence count portion of filename","0")
      Osk::flight.send_cmd("DS","SET_FILE_TYPE with FILE_TBL_IDX #{file_tbl_idx}, FILENAME_TYPE #{filename_type}")
	elsif (cmd == "FILE_PATH")
      file_tbl_idx = ask_string("Enter destination file index 0..#{DEST_FILE_TBL_MAX_IDX}","0")
      path_name    = ask_string("Enter file path","/cf")
      Osk::flight.send_cmd("DS","SET_FILE_PATH_NAME with FILE_TBL_IDX #{file_tbl_idx}, PATH_NAME #{path_name}")
	elsif (cmd == "FILE_BASE")
      file_tbl_idx = ask_string("Enter destination file index 0..#{DEST_FILE_TBL_MAX_IDX}","0")
      base_name    = ask_string("Enter file base name","")
      Osk::flight.send_cmd("DS","SET_FILE_BASE_NAME with FILE_TBL_IDX #{file_tbl_idx}, BASE_NAME #{base_name}")
	elsif (cmd == "FILE_EXTENSION")
      file_tbl_idx = ask_string("Enter destination file index 0..#{DEST_FILE_TBL_MAX_IDX}","0")
      extension    = ask_string("Enter file extension","")
      Osk::flight.send_cmd("DS","SET_FILE_EXT with FILE_TBL_IDX #{file_tbl_idx}, EXTENSION #{extension}")
	elsif (cmd == "FILE_SIZE")
      file_tbl_idx  = ask_string("Enter destination file index 0..#{DEST_FILE_TBL_MAX_IDX}","0")
      max_file_size = ask_string("Enter the maximum file size (bytes)","")
      Osk::flight.send_cmd("DS","SET_FILE_MAX_SIZE with FILE_TBL_IDX #{file_tbl_idx}, MAX_FILE_SIZE #{max_file_size}")
	elsif (cmd == "FILE_AGE")
      file_tbl_idx = ask_string("Enter destination file index 0..#{DEST_FILE_TBL_MAX_IDX}","0")
      max_file_age = ask_string("Enter the maximum file age (secs)","")
      Osk::flight.send_cmd("DS","SET_FILE_MAX_AGE with FILE_TBL_IDX #{file_tbl_idx}, MAX_FILE_AGE #{max_file_age}")
	elsif (cmd == "FILE_CLOSE")
      close = message_box("Close one or all files?", 'One', 'All')
      if (close == "One")
         file_tbl_idx = ask_string("Enter destination file index 0..#{DEST_FILE_TBL_MAX_IDX}","0")
         Osk::flight.send_cmd("DS","CLOSE_FILE with FILE_TBL_IDX #{file_tbl_idx}")
      elsif (close == "All")
         Osk::flight.send_cmd("DS","CLOSE_ALL")
      else
         prompt("Invalid entry #{close}. Must be 'One' or 'All'")
      end
	elsif (cmd == "FILTER_FILE")
      msg_id           = ask_string("Message ID of existing entry in Packet Filter Table","0")
      filter_param_idx = ask_string("Enter index into message's array of filters: 0..#{FILTER_PARAM_MAX_IDX}","0")
      file_tbl_idx     = ask_string("Enter destination file index 0..#{DEST_FILE_TBL_MAX_IDX}","0")
      Osk::flight.send_cmd("DS","SET_FILTER_FILE with MSG_ID #{msg_id}, FILTER_PARAM_IDX #{filter_param_idx}, FILE_TBL_IDX #{file_tbl_idx}")
   elsif (cmd == "FILTER_ADD_MSG")
      msg_id = ask_string("Message ID of existing entry in Packet Filter Table","0")
      Osk::flight.send_cmd("DS","ADD_FILTER_MID with MSG_ID #{msg_id}")  
   elsif (cmd == "FILTER_ALGORITHM")
      msg_id           = ask_string("Message ID of existing entry in Packet Filter Table","0")
      filter_param_idx = ask_string("Enter index into message's array of filters: 0..#{FILTER_PARAM_MAX_IDX}","0")
      algorithm_n      = ask_string("Enter N for N of X messages will be stored starting at offset O")
      algorithm_x      = ask_string("Enter X for N of X messages will be stored starting at offset O")
      algorithm_o      = ask_string("Enter O for N of X messages will be stored starting at offset O")
      Osk::flight.send_cmd("DS","SET_FILTER_PARAM with MSG_ID #{msg_id}, FILTER_PARAM_IDX #{filter_param_idx}, ALGORITHM_N #{algorithm_n}, ALGORITHM_X #{algorithm_x}, ALGORITHM_O #{algorithm_o}")
   elsif (cmd == "FILTER_TYPE")
      msg_id           = ask_string("Message ID of existing entry in Packet Filter Table","0")
      filter_param_idx = ask_string("Enter index into message's array of filters: 0..#{FILTER_PARAM_MAX_IDX}","0")
      filter_type      = ask_string("Enter filter type: 1=Sequence Count, 2=Time","1")
      Osk::flight.send_cmd("DS","SET_FILTER_TYPE with MSG_ID #{msg_id}, FILTER_PARAM_IDX #{filter_param_idx}, FILTER_TYPE #{filter_type}")
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
   elsif (cmd == "TODO")
      mem_file_name = ask_string("Enter full FSW /path/filename file to received the dump")
      prompt(Osk::MSG_TBD_FEATURE)
   else
      raise "Error in screen definition file. Undefined command sent to recorder_mgmt_send_cmd()"
   end
  
end # recorder_mgmt_send_cmd()

