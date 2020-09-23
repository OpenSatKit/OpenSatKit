###############################################################################
# cFS Kit Table Management 
#
# Notes:
#   1. The user interaction with loads and dumps should be as close as possible
#      to the JSON interface defined in json_table_mgmt
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_system'
require 'osk_flight'
require 'osk_ops'

################################################################################
## Send Commands
################################################################################

def table_mgmt_send_cmd(screen, cmd)

   if (cmd == "LOAD_TABLE")
      load_tbl = true
      put_file = combo_box("Transfer file from ground to flight?", Osk::MSG_BUTTON_YES,Osk::MSG_BUTTON_NO)
      if (put_file == Osk::MSG_BUTTON_YES)
         Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_TBL_DIR, Osk::FLT_SRV_DIR)
         if (Osk::Ops::put_flt_file_prompt(Osk::GND_SRV_TBL_DIR))
            Osk::Ops::set_work_dir_widget(screen)
         else
            load_tbl = false
         end
      end # if puf_file
      if (load_tbl)
         tbl_path_filename = ask_string("Enter full FSW /path/filename of table file to be loaded.","#{Osk::Ops::flt_path_filename}")
         if (Osk::flight.cfe_tbl.send_cmd("LOAD_TBL with FILENAME #{tbl_path_filename}"))
            tbl_name = ask_string("Enter table name 'app.table' for validation/activation commands")
            if (Osk::flight.cfe_tbl.send_cmd("VALIDATE_TBL with ACTIVE_TBL_FLAG 0, TABLE_NAME #{tbl_name}"))
               wait 10 # Most apps check pending loads on housekeeping cycle (every 3-5 seconds) 
               Osk::flight.cfe_tbl.send_cmd("ACTIVATE_TBL with TABLE_NAME #{tbl_name}")
            end
         end
      end # if load_tbl
	elsif (cmd == "ABORT_TABLE_LOAD")
      tbl_name = ask_string("Enter complete table name (app.table) of load to be aborted.")
      Osk::flight.cfe_tbl.send_cmd("ABORT_LOAD with TABLE_NAME #{tbl_name}")
	elsif (cmd == "DUMP_TABLE")
      tbl_name = ask_string("Enter complete table name (app.table) of table to be dumped.")
      flt_tbl_path_filename = ask_string("Enter full FSW /path/filename of file to receive the table", Osk::TMP_FLT_TBL_PATH_FILE)
      buffer = combo_box("Select the buffer to be dumped", 'Inactive','Active')
      buffer == 'Active' ? buffer_id = 1 : buffer_id = 0
      cmd_str = "DUMP_TBL with ACTIVE_TBL_FLAG #{buffer_id}, TABLE_NAME #{tbl_name},"
      Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_TBL_DIR, File.dirname(flt_tbl_path_filename))
      Osk::Ops::send_flt_bin_file_cmd("CFE_TBL", cmd_str, "Definition file for #{tbl_name}", flt_path_filename: flt_tbl_path_filename, gnd_rel_path: Osk::REL_SRV_TBL_DIR)
   elsif (cmd == "VALIDATE")
      tbl_name = ask_string("Enter complete table name (app.table) of table to be validated.")
      buffer = combo_box("Select the buffer to be validated", 'Inactive','Active')
      buffer == 'Active' ? buffer_id = 1 : buffer_id = 0
      Osk::flight.cfe_tbl.send_cmd("VALIDATE_TBL with ACTIVE_TBL_FLAG #{buffer_id}, TABLE_NAME #{tbl_name}")
	elsif (cmd == "ACTIVATE")
      tbl_name = ask_string("Enter complete table name (app.table) of table to be activated.")
      Osk::flight.cfe_tbl.send_cmd("ACTIVATE_TBL with TABLE_NAME #{tbl_name}")
	elsif (cmd == "DISPLAY_ONE_REGISTRY")
      tbl_name = ask_string("Enter complete table name (app.table) of table registry to be telemetered.")
      Osk::flight.cfe_tbl.send_cmd("SEND_REGISTRY with TABLE_NAME #{tbl_name}")
	elsif (cmd == "WRITE_REGISTRY_TO_FILE")
      flt_reg_path_filename = ask_string("Enter full flight /path/filename of file to receive the registry data.", Osk::TMP_FLT_TBL_PATH_FILE)
      Osk::Ops::send_flt_bin_file_cmd("CFE_TBL", "WRITE_REG_TO_FILE with ",Osk::TBL_MGR_DEF_CFE_TBL_REG, flt_path_filename: flt_reg_path_filename)
	elsif (cmd == "DISPLAY_TABLE")
      Cosmos.run_process("ruby tools/TableManager")
	elsif (cmd == "GET_FILE")
      # Update screen with working directories to help remind user where tables are stored
      Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_TBL_DIR, Osk::FLT_SRV_DIR)
      if (Osk::Ops::get_flt_file_prompt(Osk::GND_SRV_TBL_DIR))
         Osk::Ops::set_work_dir_widget(screen)
      end
   elsif (cmd == "PUT_FILE")
      # Update screen with working directories to help remind user where tables are stored
      Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_TBL_DIR, Osk::FLT_SRV_DIR)
      if (Osk::Ops::put_flt_file_prompt(Osk::GND_SRV_TBL_DIR))
         Osk::Ops::set_work_dir_widget(screen)
      end
   else
      raise "Error in screen definition file. Undefined command sent to table_mgmt_send_cmd()"
   end
  
end # table_mgmt_send_cmd()
