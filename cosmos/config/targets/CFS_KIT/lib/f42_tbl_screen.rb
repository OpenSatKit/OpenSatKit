###############################################################################
# cFS Kit F42 Control Table Management 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
################################################################################

require 'json'

require 'cfs_kit_global'
require 'file_transfer'

################################################################################
## Global Variables
################################################################################

# TODO - Create OSK global or app base class
OSK_TBL_REPLACE    = 0
OSK_TBL_UPDATE_REC = 1

# Use different file name than FSW default name so it doesn't get overwritten 

F42_CTRL_TBL_ID = 0
F42_CTRL_TBL_FILE = "osk_tmp_tbl.json"
F42_CTRL_DEF_TBL_FILE = "f42_ctrl_tbl.json"

FLT_TBL_FILE = "#{FLT_SRV_DIR}/#{F42_CTRL_TBL_FILE}"
GND_TBL_FILE = "#{GND_SRV_DIR}/#{F42_CTRL_TBL_FILE}"

FLT_DEF_TBL_FILE = "#{FLT_SRV_DIR}/#{F42_CTRL_DEF_TBL_FILE}"

################################################################################
## Send Commands
################################################################################

def f42_tbl_cmd(screen, cmd)

  if (cmd == "GET_CUR_VALUES")
  
    # Get current table values from flight table and display in screen
    cmd_cnt = tlm("F42 HK_TLM_PKT CMD_VALID_COUNT")
    cmd("F42 DUMP_TBL with ID #{F42_CTRL_TBL_ID}, TBL_FILE_NAME #{FLT_TBL_FILE}"); 
    wait("F42 HK_TLM_PKT CMD_VALID_COUNT == #{cmd_cnt}+1", 10)  # Delay until cmd count increments or timeout
    if (tlm("F42 HK_TLM_PKT CMD_VALID_COUNT") == cmd_cnt)
      prompt ("F42 dump table command failed");
	  return
    end 
	
	file_xfer = FileTransfer.new()
			
    if (file_xfer.get(FLT_TBL_FILE,GND_TBL_FILE))
       tbl_file = File.read(GND_TBL_FILE)
       tbl_hash = JSON.parse(tbl_file)
	   screen_widget = screen.get_named_widget("moi_x")
	   screen_widget.text = "#{tbl_hash["moment-of-inertia"]["x"]}"
	   screen_widget = screen.get_named_widget("moi_y")
	   screen_widget.text = "#{tbl_hash["moment-of-inertia"]["y"]}"
	   screen_widget = screen.get_named_widget("moi_z")
	   screen_widget.text = "#{tbl_hash["moment-of-inertia"]["z"]}"
	   screen_widget = screen.get_named_widget("pd_gain_param_w")
	   screen_widget.text = "#{tbl_hash["pd-gain-param"]["w"]}"
	   screen_widget = screen.get_named_widget("pd_gain_param_z")
	   screen_widget.text = "#{tbl_hash["pd-gain-param"]["z"]}"
    else
       prompt("Failed to transfer flight file #{FLT_TBL_FILE} to ground file #{GND_TBL_FILE}")
	   return
	end


  elsif (cmd == "LOAD_SCR_VALUES")

    # Write current table values from screen into ground table file and load to FSW

	tbl_hash = {}

	screen_widget = screen.get_named_widget("moi_x")
	moi_x = screen_widget.text.to_f
	screen_widget = screen.get_named_widget("moi_y")
	moi_y = screen_widget.text.to_f
	screen_widget = screen.get_named_widget("moi_z")
	moi_z = screen_widget.text.to_f
    tbl_hash["moment-of-inertia"] = { :x => moi_x, :y => moi_y, :z => moi_z}
	
	screen_widget = screen.get_named_widget("pd_gain_param_w")
	pd_gain_param_w = screen_widget.text.to_f
	screen_widget = screen.get_named_widget("pd_gain_param_z")
	pd_gain_param_z = screen_widget.text.to_f
    tbl_hash["pd-gain-param"] = { :w => pd_gain_param_w, :z => pd_gain_param_z}

    File.open("#{GND_TBL_FILE}","w") do |f| 
	   f.write(JSON.pretty_generate(tbl_hash))
	   f.write("\n")  # FSW JSMN tokenizer requires a newline after closing bracket
	   end
    
	file_xfer = FileTransfer.new()
    if (!file_xfer.put(GND_TBL_FILE,FLT_TBL_FILE))
       prompt("Failed to transfer ground file #{GND_TBL_FILE} to flight file #{FLT_TBL_FILE}")
       return
    end
    cmd_cnt = tlm("F42 HK_TLM_PKT CMD_VALID_COUNT")
    cmd("F42 LOAD_TBL with ID #{F42_CTRL_TBL_ID}, TYPE #{OSK_TBL_REPLACE}, TBL_FILE_NAME #{FLT_TBL_FILE}"); 
    wait("F42 HK_TLM_PKT CMD_VALID_COUNT == #{cmd_cnt}+1", 10)  # Delay until cmd count increments or timeout
    if (tlm("F42 HK_TLM_PKT CMD_VALID_COUNT") == cmd_cnt)
      prompt ("F42 load table command failed");
	  return
    end 
	
  elsif (cmd == "RESTORE_DEFAULTS")
    cmd_cnt = tlm("F42 HK_TLM_PKT CMD_VALID_COUNT")
    cmd("F42 LOAD_TBL with ID #{F42_CTRL_TBL_ID}, TYPE #{OSK_TBL_REPLACE}, TBL_FILE_NAME #{FLT_DEF_TBL_FILE}"); 
    wait("F42 HK_TLM_PKT CMD_VALID_COUNT == #{cmd_cnt}+1", 10)  # Delay until cmd count increments or timeout
    if (tlm("F42 HK_TLM_PKT CMD_VALID_COUNT") == cmd_cnt)
      prompt ("F42 load table command failed to restore the default values");
	  return
	end
    f42_tbl_cmd(screen, "GET_CUR_VALUES")     
  else
    prompt("Error in screen definition file. Undefined command #{cmd} sent to f42_tbl_cmd()")
  end
  
end # f42_tbl_cmd()

