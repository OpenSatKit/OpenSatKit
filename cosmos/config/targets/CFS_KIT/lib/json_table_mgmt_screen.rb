###############################################################################
# JSON Table Management Screen
#
# Notes:
#   1. The user interaction with loads and dumps should be as close as possible
#      to the cFS interface defined in table_mgmt_screen.mgmt.rb.
#   2. create_json_table_mgmt_scr() is called during system initializtion and
#      creates a screen that supports all of the apps using the OSK JSON table
#      framework.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'json'

require 'cosmos'

require 'osk_global'
require 'osk_system'
require 'osk_ops'
require 'osk_flight'

################################################################################
## Screen Commands
################################################################################

def json_table_mgmt(screen, cmd)

   app_menu = screen.get_named_widget("app").text
   app_token = app_menu.split('-')
   
   app_name     = app_token[0]
   tbl_id       = app_token[1].to_i

   app = Osk::flight.app[app_name]
   tbl_filename = app.tables[tbl_id].filename
   
   #puts app.tables[tbl_id].name
   
   if (cmd == "LOAD")
      put_file = combo_box("Transfer file from ground to flight?", 'Yes','No')
      if (put_file == "Yes")
         Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_TBL_DIR, Osk::FLT_SRV_DIR)
         if (Osk::Ops::put_flt_file_prompt(Osk::GND_SRV_TBL_DIR))
            Osk::Ops::set_work_dir_widget(screen)
         end
      end 
      tbl_path_filename = ask_string("Enter full FSW /path/filename of table file to be loaded.","#{Osk::Ops::flt_path_filename}")
      load_type = combo_box("Select load type", 'Replace Table','Update Records')
      load_type == 'Replace Table' ? type = 0 : type = 1 
      Osk::flight.app[app_name].send_cmd("#{Osk::CMD_STR_LOAD_TBL} with ID #{tbl_id}, TYPE #{type}, FILENAME #{tbl_path_filename}")
   elsif (cmd == "DUMP")
      tbl_path_filename = ask_string("Enter full flight /path/filename of file to receive the table.", Osk::TMP_FLT_TXT_PATH_FILE)
      Osk::Ops::send_flt_txt_file_cmd(app_name, "#{Osk::CMD_STR_DUMP_TBL} with ID #{tbl_id}, ", flt_path_filename: tbl_path_filename) 
   elsif (cmd == "DISPLAY")
      tbl_path_filename = "#{Osk::GND_SRV_TBL_DIR}/#{tbl_filename}"
      if File.file?(tbl_path_filename)
         Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{tbl_path_filename}'")
      else
         answer = message_box("Default table #{tbl_path_filename} does not exist. Launching empty file viewer?","Yes","No",false)
         if (answer == "Yes")
            Cosmos.run_process("ruby lib/OskTxtFileViewer")
         end
      end
   else
      prompt("Error in screen definition file. Undefined command sent to json_table_mgmt()")
   end
   
end # json_table_mgmt()

