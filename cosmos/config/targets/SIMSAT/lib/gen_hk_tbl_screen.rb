###############################################################################
# Generate Housekeeping Copy Table Screen Scripts 
#
# Notes:
#   1. 
#
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'

require 'simsat_const'
require 'tbl_tools/hk/gen_hk_tbl'

################################################################################
## Screen Commands
################################################################################

def gen_hk_tbl(screen, cmd)

   if (cmd == "CREATE_JSON")
      json_file = open_file_dialog(Osk::TBL_HK_TEMPLATE_DIR, "Select JSON file")
      Cosmos.open_in_text_editor(json_file)
      #Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::CFS_CMAKE_DIR}/#{Osk::CPU1_TARGET_FILE}'")
   elsif (cmd == "JSON_INPUT")
      json_file = open_file_dialog(SimSat::TBL_TOOL_DIR)
      screen.get_named_widget("json_input_file").text = json_file
      screen.get_named_widget("output_dir").text = SimSat::TBL_DIR
      screen.get_named_widget("template_dir").text = Osk::TBL_HK_TEMPLATE_DIR
   elsif (cmd == "OUTPUT_DIR")
      output_dir = open_directory_dialog(SimSat::TBL_TOOL_DIR,"Select output directory")
      screen.get_named_widget("output_dir").text = output_dir
   elsif (cmd == "TEMPLATE_DIR")
      template_dir = open_directory_dialog(Osk::TBL_TLM_TEMPLATE_DIR,"Select template directory")
      screen.get_named_widget("template_dir").text = template_dir
   elsif (cmd == "CREATE_TABLE")      
      # TODO - Add snaity & error checks

      # Temporary setup to bypass GUI inputs
      tbl_gen_cfg_template_file = File.read("#{Cosmos::USERPATH}/lib/tbl_tools/hk/templates/hk_tbl_gen_cfg_template.json")
	   json_hash = JSON.parse(tbl_gen_cfg_template_file)
      json_hash["hk-tbl-tool"]["input-path"]    = "#{Cosmos::USERPATH}/lib/tbl_tools/hk/test/"
      json_hash["hk-tbl-tool"]["output-path"]   = "#{Cosmos::USERPATH}/lib/tbl_tools/hk/test/"
      json_hash["hk-tbl-tool"]["template-path"] = "#{Cosmos::USERPATH}/lib/tbl_tools/hk/templates/"
      hk = json_hash["hk-tbl-tool"]["housekeeping"]
      hk["json-hk-file"] = "hk_cpy_tbl.json"
      hk["log-file"] = "hk_cpy_tbl.log"
      tbl_gen_cfg_file = "#{Cosmos::USERPATH}/lib/tbl_tools/hk/test/hk_tbl_gen_cfg.json"
      File.open(tbl_gen_cfg_file,"w") do |f| 
	      f.write(JSON.pretty_generate(json_hash))
	      f.write("\n")  # FSW JSMN tokenizer requires a newline after closing bracket
      end
      
      # Create table files
      gen_hk_tbl = GenHkTbl.new(tbl_gen_cfg_file)
      return

      # Get user inputs and format for use below
      json_path_filename = screen.get_named_widget("json_input_file").text
      json_filename = File.basename(json_path_filename)
      json_template_file = File.join(screen.get_named_widget("template_dir").text,Osk::GEN_HK_TBL_JSON_TEMPLATE_FILE)
      output_path = screen.get_named_widget("output_dir").text
      template_path = screen.get_named_widget("template_dir").text
      json_input_file = File.join(output_path,json_filename)
      
      # Read JSON template file and fill in required entries 
      json_file = File.read(json_template_file)
	   json_hash = JSON.parse(json_file)
      json_hash["hk-tbl-tool"]["input-path"]    = File.dirname(json_path_filename)
      json_hash["hk-tbl-tool"]["output-path"]   = output_path
      json_hash["hk-tbl-tool"]["template-path"] = template_path
      to = json_hash["hk-tbl-tool"]["housekeeping"]
      to["json-hk-file"] = json_filename
      to["log-file"] = json_filename.sub(".json","_log.txt")
      File.open("#{json_input_file}","w") do |f| 
	      f.write(JSON.pretty_generate(json_hash))
	      f.write("\n")  # FSW JSMN tokenizer requires a newline after closing bracket
      end
      
      # Create table files
      gen_hk_tbl = GenHkTbl.new(json_input_file)
   
   elsif (cmd == "TRAINING_VIDEO")
      prompt(Osk::MSG_TBD_FEATURE)
      #Cosmos.open_in_web_browser("#{Osk::YOUTUBE_TRAINING_APP_CREATE_HELLO_WORLD}")   
   
   else
      prompt("Error in screen definition file. Undefined command sent to gen_hk_tbl()")
   end
   
end # gen_hk_tbl()

