###############################################################################
# OSK Ops
#
# Notes:
#   1. Defines methods for performing operational functions. These functions
#      require multiple OSK objects so they aren't defined in the specific
#      flight or ground files.
#   2. One class of methods dynamically create screens that are used to  
#      manage a transaction between the ground and flight. In the model-view-
#      contoller design pattern the screens are providing a view that controls
#      the user's interactions with the ground and flight "models" that are
#      COSMOS tools and FSW apps.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'cosmos'
require 'osk_global'
require 'osk_system'
require 'osk_flight'

module Osk
module Ops


   ############################################################################
   ##  Methods that effect global behavior
   ############################################################################

   @@suppress_exception = false
   
   #
   # Useful to disable exceptions for autonomous scripts without user
   # interaction
   #
   def self.suppress_exception(boolean)
      @@suppress_exception = boolean
   end
   
   def self.raise_exception(msg_str)
      if (!@@suppress_exception)
         raise msg_str
      end
   end

   ############################################################################
   ##  File Transfer
   ############################################################################
   
   #
   # gnd/flt_path_file_names set to path/filename of last successful transfer. 
   # Screens can read and display them.
   #
   def self.gnd_path_filename 
      if (!defined? @gnd_path_filename)
         @gnd_path_filename = GND_SRV_DIR
      end
      return @gnd_path_filename
   end # gnd_path_filename

   def self.flt_path_filename 
      if (!defined? @flt_path_filename)
         @flt_path_filename = FLT_SRV_DIR
      end
      return @flt_path_filename
   end # flt_path_filename

   def self.get_flt_file(flt_path_filename, gnd_path_filename)
      got_file = false
      if (Osk::system.file_transfer.get(flt_path_filename,gnd_path_filename))
         @gnd_path_filename = gnd_path_filename
         @flt_path_filename = flt_path_filename
         got_file = true
      end
      return got_file
   end # get_flt_file()

   # Default gnd_path shold be overridden for tables that have a separate file server directory
   def self.get_flt_file_prompt(gnd_path = GND_SRV_DIR)
      flt_path_filename = ask_string("Enter full FSW /path/filename.","#{FLT_SRV_DIR}/")
      gnd_filename = ask_string("Enter ground filename without path. File will be located in #{gnd_path}.",File.basename(flt_path_filename))
      gnd_path_filename = "#{gnd_path}/#{gnd_filename}"
      got_file = get_flt_file(flt_path_filename,gnd_path_filename)
      return got_file
   end # End get_flt_file_prompt()

   def self.put_flt_file(gnd_path_filename,flt_path_filename)
      put_file = false
      if (Osk::system.file_transfer.put(gnd_path_filename,flt_path_filename))
         @gnd_path_filename = gnd_path_filename
         @flt_path_filename = flt_path_filename
         put_file = true
      end
      return put_file
   end # End put_flt_file()

   # Default gnd_path should be overriden for tables that have a separate file server directory
   def self.put_flt_file_prompt(gnd_path = GND_SRV_DIR)
      #gnd_filename = ask_string("Enter ground filename without path. File will be located in #{gnd_path}.")
      #gnd_path_filename = "#{gnd_path}/#{gnd_filename}"
      gnd_path_filename = open_file_dialog(gnd_path)
      gnd_filename = File.basename(gnd_path_filename)
      flt_path_filename = ask_string("Enter full FSW /path/filename.             ","#{FLT_SRV_DIR}/#{gnd_filename}")
      put_file = put_flt_file(gnd_path_filename,flt_path_filename)
      return put_file
   end # End put_flt_file_prompt()

   
   ############################################################################
   ##  Multi-step cFS Functions
   ############################################################################
   
   # 
   # Send a flight command that creates a binary file 
   # 1. Issue the flight command that creates the file
   # 2. Optionally transfer file to the ground and display 
   #    the file using the COSMOS Table Manager tool
   # 
   # Use the default server directories and temporary file names
   #
   # NOTE: THE COMMAND DEFINITION MUST USE THE KEYWORD 'FILENAME' AND 
   #       cmd_str MUST CONTAIN THE ENTIRE COMMAND EXCEPT FOR THE FILENAME
   #       PARAMETER. cmd_str MUST BE FORMATTED WITH OR WITHOUT A COMMA SO
   #       FILENAME CAN SIMPLY BE APPENDED. See table_mgmt_screen.rb's
   #       'write table registry to file' for an example.    
   #
   def self.send_flt_bin_file_cmd(app_name, cmd_str, tbl_mgr_def_filename, flt_path_filename: Osk::TMP_FLT_BIN_PATH_FILE, gnd_rel_path: Osk::REL_SRV_DIR)
      cmd_str = "#{cmd_str} FILENAME #{flt_path_filename}"
      if (Osk::flight.app[app_name].send_cmd(cmd_str))
         get_file = combo_box("Transfer file from flight to ground and display it?", 'Yes','No')
         if (get_file == "Yes")
            filename = File.basename(flt_path_filename)
            get_flt_bin_file(flt_path_filename, gnd_rel_path, filename, tbl_mgr_def_filename)
         end
      else
         raise_exception "osk_ops.send_flt_bin_file_cmd() - #{app_name} command '#{cmd_str}' failed"
      end
   end # send_flt_bin_file_cmd()
   
   #
   # 1. Transfer a binary file from flight to ground 
   # 2. Launch COSMOS Table Manager tool to display the file
   # 
   # Ground relative directory path and filename passed in separately to help with user 
   # Table Manager instructions.
   #
   def self.get_flt_bin_file(flt_path_filename, gnd_rel_path, gnd_filename, tbl_mgr_def_filename)
      gnd_path_filename = "#{Cosmos::USERPATH}/#{gnd_rel_path}/#{gnd_filename}"
      if (Osk::system.file_transfer.get(flt_path_filename,gnd_path_filename))
         launch_tbl_mgr(gnd_rel_path, gnd_filename, tbl_mgr_def_filename)
      else  
         raise_exception "osk_ops.get_flt_bin_file() - File transfer from flight to ground failed" 
      end
      
   end # get_flt_bin_file()

   # 
   # Send a flight command that creates a text file 
   # 1. Issue the flight command that creates the file
   # 2. Optionally transfer file to the ground and display 
   #    the file using the OSK's text file viewer
   # 
   # Use the default server directories and temporary file names
   #
   # NOTE: THE COMMAND DEFINITION MUST USE THE KEYWORD 'FILENAME' AND 
   #       cmd_str MUST CONTAIN THE ENTIRE COMMAND EXCEPT FOR THE FILENAME
   #       PARAMETER. cmd_str MUST BE FORMATTED WITH OR WITHOUT A COMMA SO
   #       FILENAME CAN SIMPLY BE APPENDED. See json_table_mgmt_screen.rb's
   #       load and dump for an example.    
   #
   def self.send_flt_txt_file_cmd(app_name, cmd_str, flt_path_filename: Osk::TMP_FLT_TXT_PATH_FILE, gnd_rel_path: Osk::REL_SRV_TBL_DIR)
      cmd_str = "#{cmd_str} FILENAME #{flt_path_filename}"
      if (Osk::flight.app[app_name].send_cmd(cmd_str))
         get_file = combo_box("Transfer file from flight to ground and display it?", 'Yes','No')
         if (get_file == "Yes")
            filename = File.basename(flt_path_filename)
            get_flt_txt_file(flt_path_filename, gnd_rel_path, filename)
         end
      else
         raise_exception "osk_ops.send_flt_txt_file_cmd() - #{app_name} command '#{cmd_str}' failed"
      end
   end # send_flt_txt_file_cmd()
   
   #
   # 1. Transfer a text file from flight to ground 
   # 2. Launch OSK's text file viewer to display the file
   # 
   def self.get_flt_txt_file(flt_path_filename, gnd_rel_path, gnd_filename)
      gnd_path_filename = "#{Cosmos::USERPATH}/#{gnd_rel_path}/#{gnd_filename}"
      if (Osk::system.file_transfer.get(flt_path_filename,gnd_path_filename))
         Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{gnd_path_filename}'")
      else  
         raise_exception "osk_ops.get_flt_txt_file() - File transfer from flight to ground failed" 
      end
      
   end # get_flt_txt_file()

   
   ############################################################################
   ##  Methods that support sending commands from a screen 
   ############################################################################

   #
   # Set the get/put working directories. The screens must use the 'standard'
   # working directoru named widgets
   #   
   def self.set_work_dir_widget(screen, gnd_path_filename=Osk::Ops::gnd_path_filename, flt_path_filename=Osk::Ops::flt_path_filename)

      gnd_work_dir_widget = screen.get_named_widget(WIDGET_GND_WORK_DIR)
      gnd_work_dir_widget.text = gnd_path_filename

      flt_work_dir_widget = screen.get_named_widget(WIDGET_FLT_WORK_DIR)
      flt_work_dir_widget.text = flt_path_filename

   end # set_work_dir_widget()

   #
   # Designed to be called from a screen. It provides a lighter weight 
   # method (rather than cmd-tlm-server tool) to get user input for command
   # parameters.
   # - If command has no parameters, cmd is sent immediatley
   # - If command has parameters a parameter input screen is created
   #
   def self.send_flt_cmd (target_name, cmd_name)

      cmd_param_list = get_cmd_param_list(target_name, cmd_name)
      # CCSDS header fields counted as parameters 
      if (cmd_param_list.length <= 5)
         Osk::flight.app[target_name].send_cmd(cmd_name)
         #~cmd("#{target_name} #{cmd_name}"); 
      else
         create_flt_cmd_param_scr(target_name, cmd_name, cmd_param_list.drop(5))
         display("CFS_KIT #{TMP_FLT_CMD_SCR_STR}",50,50)
      end

   end #send_flt_cmd()

   #
   # Reads command parameters from temp input screen then sends the cmd 
   #
   def self.send_flt_cmd_with_param_scr (screen, target_name, cmd_name, param_cnt)

      #puts "cfe_send_cmd_with_param(): #{target_name}, #{cmd_name}, #{param_cnt}"
      # Read parameters from screen
      cmd_param_str = "with #{screen.get_named_widget("param_1").text} #{screen.get_named_widget("value_1").text}"
      for i in 2..param_cnt
         cmd_param_str << " , #{screen.get_named_widget("param_#{i}").text} #{screen.get_named_widget("value_#{i}").text}"
      end

      Osk::flight.app[target_name].send_cmd("#{cmd_name} #{cmd_param_str}")
      
      # Doesn't always clear display. Haven't determined if just when an exception occurs
      clear("CFS_KIT #{TMP_FLT_CMD_SCR_STR}")
         
   end # send_flt_cmd_with_param_scr()


   def self.create_flt_cmd_param_scr (target_name, cmd_name, cmd_param_list)

      t = Time.new 
      time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"

      cmd_scr_header = "
      ###############################################################################
      # Command Screen
      #
      # Temporary screen automatically created to allow the user to use a simple GUI
      # to enter command parameters. Basically a short cut to COSMOS Command and 
      # Telemetry Server capability.  
      #
      # Notes:
      #   1. Do not edit this file because it is automatically generated and your
      #      changes will not be saved.
      #   2. File created by osk_ops.rb on #{time_stamp}
      #
      # License:
      #   Written by David McComas, licensed under the copyleft GNU General Public
      #   License (GPL).
      #
      ###############################################################################

      SCREEN AUTO AUTO 0.5
      GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
     
      TITLE #{cmd_name}
         SETTING BACKCOLOR 162 181 205
         SETTING TEXTCOLOR black

      VERTICALBOX \"Command Parameters\"
      MATRIXBYCOLUMNS 2\n" 

      cmd_scr_trailer = "
      END # Matrix
      END # Vertical

      HORIZONTALLINE

      MATRIXBYCOLUMNS 2      
         BUTTON 'Send'   'require \"#{Cosmos::USERPATH}/lib/osk_ops.rb\"; Osk::Ops::send_flt_cmd_with_param_scr(self, \"#{target_name}\", \"#{cmd_name}\", #{cmd_param_list.length});'
         BUTTON 'Cancel' 'clear(\"CFS_KIT #{TMP_FLT_CMD_SCR_STR}\")'
      End # Matrix\n"

      #puts "GUI: #{target_name} #{cmd_name}"
      #puts "Cmd param length = #{cmd_param_list.length}"
      #puts "#{cmd_param_list}"
      
      cmd_scr_file = "#{Osk::SCR_DIR}/#{Osk::TMP_FLT_CMD_SCR_FILE}"
      #puts "#{cmd_scr_file}"
      
      begin
         
         
         # Always overwrite the temp file      
         File.open("#{cmd_scr_file}","w") do |f| 
           
            f.write ("#{cmd_scr_header}")

            param_num = 1
            cmd_param_str = ""
            cmd_param_list.each do |param|
               cmd_param_str << "   NAMED_WIDGET param_#{param_num} LABEL '#{param[0]}'\n"         
               cmd_param_str << "   NAMED_WIDGET value_#{param_num} TEXTFIELD 16 \"#{param[1]}\"\n"
               param_num += 1
            end
            
            f.write ("#{cmd_param_str}")
            f.write ("#{cmd_scr_trailer}")

         end # File
         
      rescue Exception => e
         puts e.message
         puts e.backtrace.inspect  
      end

   end # create_flt_cmd_param_scr()

   ############################################################################
   ## Provide user with screen to launch COSMOS table manager. No user inputs
   ## allowed.
   ############################################################################

   def self.launch_tbl_mgr(rel_bin_file_path, bin_filename, tbl_def_filename)

      @@rel_server_dir = rel_bin_file_path
      create_tbl_mgr_scr(rel_bin_file_path, bin_filename, tbl_def_filename)
      display("CFS_KIT #{TMP_TBL_MGR_SCR_STR}",50,50)

   end #launch_tbl_mgr()

   def self.create_tbl_mgr_scr(rel_bin_file_path, bin_filename, tbl_def_filename)

      t = Time.new 
      time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"

      tbl_mgr_scr_header = "
      ###############################################################################
      # Provide instructions for user to display a binary file using Table Manager
      #
      # Notes:
      #   1. Do not edit this file because it is automatically generated and your
      #      changes will not be saved.
      #   2. File created by osk_ops.rb on #{time_stamp}
      #
      # License:
      #   Written by David McComas, licensed under the copyleft GNU General Public
      #   License (GPL).
      #
      ###############################################################################

      SCREEN AUTO AUTO 0.5
      GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
  
      TITLE \"Launch Table Manager\"
         SETTING BACKCOLOR 162 181 205
         SETTING TEXTCOLOR black

      LABEL \" \"
   
      VERTICALBOX \"Display File in Table Manager\"
         LABEL \"1. Click 'Table Manager' button to start the Table Manager tool\"
         LABEL \"2. Within Table Manager select File->Open->#{rel_bin_file_path}\" 
         LABEL \"       > Select '#{bin_filename}' and click 'Open'\"
         LABEL \"       > When prompted select '#{tbl_def_filename}' definition file\"
         LABEL \"       > Ignore 'binary file size does not match definition file size' warning\"
      END # Vertical Box
   

      VERTICALBOX
         MATRIXBYCOLUMNS 3      
            BUTTON 'Table Manager' 'spawn(\"ruby #{COSMOS_TBL_MANAGER}\")'
            BUTTON 'Done'          'clear(\"CFS_KIT OSK_TMP_TBL_MGR_SCR\")'
         End # Matrix
      END # Vertical Box\n"

      tbl_mgr_scr_trailer = "\n"
   
      tbl_mgr_scr_file = "#{Osk::SCR_DIR}/#{TMP_TBL_MGR_SCR_FILE}"
      
      begin
         
         # Always overwrite the temp file      
         File.open("#{tbl_mgr_scr_file}","w") do |f| 
           
            f.write ("#{tbl_mgr_scr_header}")         
            f.write ("#{tbl_mgr_scr_trailer}")

         end # File
         
      rescue Exception => e
         puts e.message
         puts e.backtrace.inspect  
      end

   end # create_tbl_mgr_scr()
   
   ############################################################################
   ##  Methods that support transferring files from flight to ground and
   ##  displaying them using COSMOS tools
   ############################################################################

   def self.get_flt_bin_file_scr(flt_path_filename, gnd_path, gnd_filename, tbl_def_filename)

      create_get_bin_file_scr(flt_path_filename, gnd_path, gnd_filename, tbl_def_filename)
      display("CFS_KIT #{TMP_GET_BIN_FILE_SCR_STR}",50,50)

   end #get_flt_bin_file_scr()

   def self.create_get_bin_file_scr(title, flt_path_filename, gnd_path, gnd_filename, tbl_def_filename)

      t = Time.new 
      time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"

      get_bin_file_scr_header = "
      ###############################################################################
      # Get and Display Binary File Screen
      #
      # Temporary screen automatically created to allow the user to get a file from
      # the FSW, transfer the file, and display the file using the COSMOS Table
      # Manager tool.
      #
      # Notes:
      #   1. Do not edit this file because it is automatically generated and your
      #      changes will not be saved.
      #   2. File created by osk_ops.rb on #{time_stamp}
      #
      # License:
      #   Written by David McComas, licensed under the copyleft GNU General Public
      #   License (GPL).
      #
      ###############################################################################

      SCREEN AUTO AUTO 0.5
      GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
     
      TITLE \"Get and Display #{}\"
         SETTING BACKCOLOR 162 181 205
         SETTING TEXTCOLOR black

      LABEL \" \"
      VERTICALBOX \"Get File from Flight\"
         LABEL \"1. Enter flight and ground file information below\"
         LABEL \"2. Select <Get File> to transfer file to the ground\"
      END # Vertical Box
      
      VERTICALBOX \"Display File in Table Manager\"
         LABEL \"1. Select <Table Manager> to start the Table Manager tool\"
         LABEL \"2. Within Table Manager select File->Open->#{gnd_filename}\"
         LABEL \"       > When prompted select {tbl_def_filename} definition file\"
         LABEL \"       > Ignore 'binary file size does not match definition file size' warning\"
      END # Vertical Box
      
      VERTICALBOX \"Ground Path/filename\" 10
         NAMED_WIDGET gnd_path_filename TEXTFIELD 256 \"#{gnd_path}/#{gnd_filename}\"
         SETTING HEIGHT 20
      END # Vertical Box

      VERTICALBOX \"Flight Path/filename\" 10
         NAMED_WIDGET flt_path_filename TEXTFIELD 256 \"#{flt_path_filename}\"
         SETTING HEIGHT 20
      END # Vertical Box

      VERTICALBOX
         MATRIXBYCOLUMNS 3      
            BUTTON 'Get File'      'require \"#{Cosmos::USERPATH}/lib/osk_ops.rb\"; Osk::Ops::get_flt_file_prompt(Osk::GND_SRV_TBL_DIR)'
            BUTTON 'Table Manager' 'spawn(\"ruby #{COSMOS_TBL_MANAGER}\")'
            BUTTON 'Done'          'clear(\"CFS_KIT OSK_TMP_GET_BIN_FILE_SCR\")'
         End # Matrix
      END # Vertical Box\n"

      get_bin_file_scr_trailer = "\n"
      
      get_bin_file_scr_file = "#{Osk::SCR_DIR}/#{TMP_GET_BIN_FILE_SCR_FILE}"
      
      begin
         
         # Always overwrite the temp file      
         File.open("#{get_bin_file_scr_file}","w") do |f| 
           
            f.write ("#{get_bin_file_scr_header}")         
            f.write ("#{get_bin_file_scr_trailer}")

         end # File
         
      rescue Exception => e
         puts e.message
         puts e.backtrace.inspect  
      end

   end # create_get_bin_file_scr()

end # Module Ops
end # Module Osk

