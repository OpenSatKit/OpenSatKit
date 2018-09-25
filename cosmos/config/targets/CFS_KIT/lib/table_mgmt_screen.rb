###############################################################################
# cFS Kit Table Management 
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
      put_file = combo_box("Transfer file from ground to flight?", 'Yes','No')
      if (put_file == "Yes")
         Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_TBL_DIR, Osk::FLT_SRV_DIR)
         if (Osk::Ops::put_flt_file_prompt(Osk::GND_SRV_TBL_DIR))
            Osk::Ops::set_work_dir_widget(screen)
         end
      end 
      tbl_path_filename = ask_string("Enter full FSW /path/filename of table file to be loaded.","#{Osk::Ops::flt_path_filename}")
      Osk::flight.cfe_tbl.send_cmd("LOAD_TBL with FILENAME #{tbl_path_filename}")
	elsif (cmd == "ABORT_TABLE_LOAD")
      tbl_name = ask_string("Enter complete table name (app.table) of load to be aborted.")
      Osk::flight.cfe_tbl.send_cmd("ABORT_LOAD with TABLE_NAME #{tbl_name}")
	elsif (cmd == "DUMP_TABLE")
      tbl_name = ask_string("Enter complete table name (app.table) of table to be dumped.")
      flt_tbl_path_filename = ask_string("Enter full FSW /path/filename of file to receive the table", Osk::TMP_FLT_BIN_PATH_FILE)
      buffer = combo_box("Select the buffer to be dumped", 'Inactive','Active')
      if (buffer == 'Active') 
         buffer_id = 1
      else
         buffer_id = 0
      end
      cmd_str = "DUMP_TBL with ACTIVE_TBL_FLAG #{buffer_id}, TABLE_NAME #{tbl_name},"
      Osk::Ops::send_flt_bin_file_cmd("CFE_TBL", cmd_str, "Definition file for #{tbl_name}", flt_path_filename: flt_tbl_path_filename, gnd_rel_path: Osk::REL_SRV_TBL_DIR)
   elsif (cmd == "VALIDATE")
      tbl_name = ask_string("Enter complete table name (app.table) of table to be validated.")
      buffer = combo_box("Select the buffer to be validated", 'Inactive','Active')
      if (buffer == 'Active') 
         buffer_id = 1
      else
         buffer_id = 0
      end
      Osk::flight.cfe_tbl.send_cmd("VALIDATE_TBL with ACTIVE_TBL_FLAG #{buffer_id}, TABLE_NAME #{tbl_name}")
	elsif (cmd == "ACTIVATE")
      tbl_name = ask_string("Enter complete table name (app.table) of table to be activated.")
      Osk::flight.cfe_tbl.send_cmd("ACTIVATE_TBL with TABLE_NAME #{tbl_name}")
	elsif (cmd == "DISPLAY_ONE_REGISTRY")
      tbl_name = ask_string("Enter complete table name (app.table) of table registry to be telemetered.")
      Osk::flight.cfe_tbl.send_cmd("TLM_REGISTRY with TABLE_NAME #{tbl_name}")
	elsif (cmd == "WRITE_REGISTRY_TO_FILE")
      flt_reg_path_filename = ask_string("Enter full flight /path/filename of file to receive the registry data.", Osk::TMP_FLT_BIN_PATH_FILE)
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
