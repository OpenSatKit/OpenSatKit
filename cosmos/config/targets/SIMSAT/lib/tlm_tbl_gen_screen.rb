###############################################################################
# Generate Telemetry Table Screen Scripts 
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
require 'tbl_tools/tlm/kit_to_tbl_gen'

################################################################################
## Screen Commands
################################################################################

def tlm_tbl_gen(screen, cmd)

   if (cmd == "CREATE_CSV")
      xls_file = open_file_dialog(SimSat::TBL_TOOL_DIR, "Select xlsx file")
      # Launch LibreOffice with template file
      #xls_template = File.join(Osk::TBL_TOOLS_DIR,"tlm","templates",Osk::TLM_TBL_GEN_INPUT_XLS_FILE)
      spawn("libreoffice --calc #{xls_file}")
   elsif (cmd == "CSV_INPUT")
      csv_file = open_file_dialog(SimSat::TBL_TOOL_DIR)
      #puts "csv_file = #{csv_file}\n"
      screen.get_named_widget("csv_input_file").text = csv_file
      screen.get_named_widget("output_dir").text = SimSat::TBL_DIR
      screen.get_named_widget("template_dir").text = Osk::TBL_TLM_TEMPLATE_DIR
      #Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::CFS_CMAKE_DIR}/#{Osk::CPU1_TARGET_FILE}'")
   elsif (cmd == "OUTPUT_DIR")
      output_dir = open_directory_dialog(SimSat::TBL_DIR,"Select output directory")
      screen.get_named_widget("output_dir").text = output_dir
   elsif (cmd == "TEMPLATE_DIR")
      template_dir = open_directory_dialog(Osk::TBL_TLM_TEMPLATE_DIR,"Select template directory")
      screen.get_named_widget("template_dir").text = template_dir
   elsif (cmd == "CREATE_TABLES")
      # TODO - Add sanity/error checks
      # Get user inputs and format for use below
      csv_path_filename = screen.get_named_widget("csv_input_file").text
      csv_filename = File.basename(csv_path_filename)
      json_template_file = File.join(screen.get_named_widget("template_dir").text,Osk::TLM_TBL_GEN_CFG_TEMPLATE_FILE)
      output_path = screen.get_named_widget("output_dir").text
      template_path = screen.get_named_widget("template_dir").text
      json_input_file = File.join(output_path,csv_filename.sub(".csv",".json"))
      
      # Read JSON template file and fill in required entries 
      json_file = File.read(json_template_file)
	   json_hash = JSON.parse(json_file)
      json_hash["tlm-tbl-tool"]["input-path"]    = File.dirname(csv_path_filename)
      json_hash["tlm-tbl-tool"]["output-path"]   = output_path
      json_hash["tlm-tbl-tool"]["template-path"] = template_path
      to = json_hash["tlm-tbl-tool"]["telemetry-output"]
      to["csv-file"] = csv_filename
      to["log-file"] = csv_filename.sub(".csv","_log.txt")
      File.open("#{json_input_file}","w") do |f| 
	      f.write(JSON.pretty_generate(json_hash))
	      f.write("\n") 
      end
      
      # Create table files
      kit_to_tbl_gen = KitToTblGen.new(json_input_file)
   
      prompt("If no error dialogs appeared then files successfully created in #{output_path}")
   
   elsif (cmd == "TRAINING_VIDEO")
      prompt(Osk::MSG_TBD_FEATURE)
      #Cosmos.open_in_web_browser("#{Osk::YOUTUBE_TRAINING_APP_CREATE_HELLO_WORLD}")   
   
   else
      prompt("Error in screen definition file. Undefined command sent to tlm_tbl_gen()")
   end
   
end # tlm_tbl_gen()

