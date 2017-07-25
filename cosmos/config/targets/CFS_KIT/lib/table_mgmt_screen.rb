###############################################################################
# cFS Kit Table Management 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'cfs_kit_global'
require 'file_transfer'
require 'file_mgmt_screen'

################################################################################
## Global Variables
################################################################################

TABLE_MGMT_FLT_WORK_DIR = FLT_WORK_DIR
TABLE_MGMT_GND_WORK_DIR = GND_WORK_DIR

################################################################################
## Send Commands
################################################################################

def table_mgmt_send_cmd(screen, cmd)

	if (cmd == "LOAD_TABLE")
    tbl_file_name = ask_string("Enter full FSW path/filename of table file to be loaded.")
    cmd("CFE_TBL LOAD_TBL with LOADFILENAME #{tbl_file_name}")
	elsif (cmd == "ABORT_TABLE_LOAD")
    tbl_name = ask_string("Enter complete table name (app.table) of load to be aborted.")
    cmd("CFE_TBL ABORT_LOAD with TABLENAME #{tbl_name}")
	elsif (cmd == "DUMP_TABLE")
    tbl_name = ask_string("Enter complete table name (app.table) of table to be dumped.")
    tbl_file_name = ask_string("Enter full FSW path/filename of file to received the table")
    buffer = combo_box("Select the buffer to be dumped", 'Inactive','Active')
    if (buffer == 'Active') 
      buffer_id = 1
    else
      buffer_id = 0
    end
    cmd("CFE_TBL DUMP_TBL with ACTIVETBLFLAG #{buffer_id}, TABLENAME #{tbl_name}, DUMPFILENAME #{tbl_file_name}")
	elsif (cmd == "VALIDATE")
    tbl_name = ask_string("Enter complete table name (app.table) of table to be validated.")
    buffer = combo_box("Select the buffer to be validated", 'Inactive','Active')
    if (buffer == 'Active') 
      buffer_id = 1
    else
      buffer_id = 0
    end
    cmd("CFE_TBL VALIDATE_TBL with ACTIVETBLFLAG #{buffer_id}, TABLENAME #{tbl_name}")
	elsif (cmd == "ACTIVATE")
    tbl_name = ask_string("Enter complete table name (app.table) of table to be activated.")
    cmd("CFE_TBL ACTIVATE_TBL with TABLENAME #{tbl_name}")
	elsif (cmd == "DISPLAY_ONE_REGISTRY")
    tbl_name = ask_string("Enter complete table name (app.table) of table registry to be telemetered.")
    cmd("CFE_TBL TLM_REGISTRY with TABLENAME #{tbl_name}")
	elsif (cmd == "WRITE_REGISTRY_TO_FILE")
    rg_file_name = ask_string("Enter full FSW path/filename of file to received the registry data.")
    cmd("CFE_TBL WRITE_REG_TO_FILE with DUMPFILENAME #{reg_file_name}")
	elsif (cmd == "DISPLAY_TABLE")
     Cosmos.run_process("ruby tools/TableManager")
	elsif (cmd == "GET_FILE")
    # Don't use file_mgmt because table file needs to be manipulated.
    file_mgmt_set_work_dir(screen);
    file_xfer = FileTransfer.new()
    flt_full_file_name = ask_string("Enter full FSW path/filename.")
    gnd_file_name = ask_string("Enter ground filename without path. File will be in kit server location.")
    gnd_file_name_no_hdr = gnd_file_name.split('.').first + "_no_hdr.dat" # Assumes one .
    gnd_full_file_name = "#{GND_WORK_DIR}/#{gnd_file_name}"
    gnd_full_file_name_no_hdr = "#{GND_WORK_DIR}/#{gnd_file_name_no_hdr}"
    if (file_xfer.get(flt_full_file_name,gnd_full_file_name) )
      IO.copy_stream(gnd_full_file_name,gnd_full_file_name_no_hdr,File.size(gnd_full_file_name)-64 ,64)
      prompt ("Created table file #{gnd_full_file_name_no_hdr} without the cFE header")
    end
  else
    prompt("Error in screen definition file. Undefined commmand sent to table_mgmt_send_cmd()")
  end
  
end # table_mgmt_send_cmd()

