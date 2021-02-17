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
require 'tbl_tools/hk/hk_tbl_gen'

################################################################################
## Screen Commands
################################################################################

def hk_tbl_gen(screen, cmd)

   case cmd
   when "CREATE_JSON"
      json_file = open_file_dialog(Osk::TBL_HK_TEST_DIR, "Select JSON file")
      Cosmos.open_in_text_editor(json_file)
      #Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::CFS_CMAKE_DIR}/#{Osk::CPU1_TARGET_FILE}'")
   when "JSON_INPUT"
      json_file = open_file_dialog(Osk::TBL_HK_TEST_DIR)
      screen.get_named_widget("json_input_file").text = json_file
   when "OUTPUT_DIR"
      output_dir = open_directory_dialog(Osk::TBL_HK_TEST_DIR,"Select output directory")
      screen.get_named_widget("output_dir").text = output_dir
   when "TEMPLATE_DIR"
      template_dir = open_directory_dialog(Osk::TBL_TLM_TEMPLATE_DIR,"Select template directory")
      screen.get_named_widget("template_dir").text = template_dir
   when "CREATE_TABLE"      
      # TODO - Add sanity/error checks
      # Get user inputs and format for use in JSON config file 
      json_input_path_filename = screen.get_named_widget("json_input_file").text
      json_input_filename = File.basename(json_input_path_filename)
      input_path    = File.dirname(json_input_path_filename)
      output_path   = screen.get_named_widget("output_dir").text
      template_path = screen.get_named_widget("template_dir").text

      json_template_file = File.join(Osk::TBL_HK_TEMPLATE_DIR, Osk::HK_TBL_GEN_CFG_TEMPLATE_FILE)
      tbl_gen_cfg_template_file = File.read(json_template_file)
	   json_hash = JSON.parse(tbl_gen_cfg_template_file)
      
      json_hash["hk-tbl-tool"]["input-path"]    = input_path
      json_hash["hk-tbl-tool"]["output-path"]   = output_path
      json_hash["hk-tbl-tool"]["template-path"] = template_path
      json_hash["hk-tbl-tool"]["log-file"]      = json_input_filename.sub(".json","_log.txt")
      cfs_config = json_hash["hk-tbl-tool"]["cfs"]
      cfs_config["json-hk-file"] = json_input_filename
      
      tbl_gen_cfg_file = File.join(input_path, Osk::HK_TBL_GEN_CFG_TEMPLATE_FILE)
      File.open(tbl_gen_cfg_file,"w") do |f| 
         json_text = JSON.pretty_generate(json_hash).sub("@src-file@",json_template_file).sub("@date-time@",Time.now.to_s)
	      f.write(json_text)
	      f.write("\n")
      end
      
      # Create table files
      hk_tbl_gen = HkTblGen.new(tbl_gen_cfg_file)
   
      prompt("If no error dialogs appeared then files successfully created in #{output_path}")
      
   when "TRAINING_VIDEO"
      prompt(Osk::MSG_TBD_FEATURE)
      #Cosmos.open_in_web_browser("#{Osk::YOUTUBE_TRAINING_APP_CREATE_HELLO_WORLD}")   
   
   else
      prompt("Error in screen definition file. Undefined command sent to hk_tbl_gen()")
   end
   
end # hk_tbl_gen()

