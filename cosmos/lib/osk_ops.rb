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
   ##  Application Management
   ############################################################################

   #
   # Check if an application is loaded 
   #  - app_name is case sensitive
   #  - Disable events because unloaded error msg can be distracting if
   #    just checking to determine whether an app is loaded   
   #
   def self.app_loaded?(app_name)
   
      app_loaded = false
      
      app = Osk::flight.app[app_name]
      if app.nil?
         raise_exception "osk_ops.app_loaded?() - #{app_name} not registered in osk_flight.rb"
      else
      
         cmd("CFE_EVS DIS_APP_EVENT_GEN with APP_NAME CFE_ES")

         seq_cnt = tlm("CFE_ES APP_INFO_TLM_PKT #{Ccsds::PRI_HDR_SEQUENCE}")

         Osk::flight.cfe_es.send_cmd("SEND_APP_INFO with APP_NAME #{app_name}")
         wait 1
   
         app_loaded = ( tlm("CFE_ES APP_INFO_TLM_PKT #{Ccsds::PRI_HDR_SEQUENCE}") != seq_cnt and
                        tlm("CFE_ES APP_INFO_TLM_PKT NAME") == app_name) 
         
         cmd("CFE_EVS ENA_APP_EVENT_GEN with APP_NAME CFE_ES")
      
      
      end

      return app_loaded
      
   end # app_loaded?()
   
   #
   # Load an app that already exists in the Osk::flight.app hash. If the app
   # exists in the hash then this function can assume appropriate data has
   # been defined. The data contents may be incorrect and the FSW will 
   # verify them.
   #
   def self.load_app(app_name)
   
      app = Osk::flight.app[app_name]

      # TODO - Add error checks
      Osk::flight.cfe_es.send_cmd( \
         "START_APP with APP_NAME #{app.fsw_name}, \
         APP_ENTRY_POINT #{app.entry_symbol}, \
         APP_FILENAME #{app.obj_path_filename}, \
         STACK_SIZE #{app.stack_size},\
         EXCEPTION_ACTION 0, \
         PRIORITY #{app.priority}")
   
   end # load_app()

   ############################################################################
   ##  File & Directory Management
   ############################################################################

   #
   # Create a directory 
   # - Note size can be 0 for an empty directory when checking if it exists so
   #   also check mod time. 
   #
   def self.create_flt_dir(dir_name)

      seq_count = tlm("FM FILE_INFO_PKT CCSDS_SEQUENCE")
      Osk::flight.send_cmd("FM","SEND_FILE_INFO with FILENAME #{dir_name}")
      wait("FM FILE_INFO_PKT CCSDS_SEQUENCE != #{seq_count}", 5)
      if (tlm("FM FILE_INFO_PKT SIZE") == 0 and tlm("FM FILE_INFO_PKT LAST_MOD_TIME") == 0)
         Osk::flight.send_cmd("FM","CREATE_DIR with DIRECTORY #{dir_name}")
      end

   end # create_flt_dir()
   
   
   def self.delete_flt_file(filename)

      seq_count = tlm("FM FILE_INFO_PKT CCSDS_SEQUENCE")
      Osk::flight.send_cmd("FM","SEND_FILE_INFO with FILENAME #{filename}")
      wait("FM FILE_INFO_PKT CCSDS_SEQUENCE != #{seq_count}", 5)
      if (tlm("FM FILE_INFO_PKT SIZE") != 0 or tlm("FM FILE_INFO_PKT LAST_MOD_TIME") != 0)
         Osk::flight.send_cmd("FM","DELETE_FILE with FILENAME #{filename}")
      end

   end # create_flt_file()

   def self.delete_flt_dir(dir_name)

      seq_count = tlm("FM FILE_INFO_PKT CCSDS_SEQUENCE")
      Osk::flight.send_cmd("FM","SEND_FILE_INFO with FILENAME #{dir_name}")
      wait("FM FILE_INFO_PKT CCSDS_SEQUENCE != #{seq_count}", 5)
      if (tlm("FM FILE_INFO_PKT SIZE") != 0 or tlm("FM FILE_INFO_PKT LAST_MOD_TIME") != 0)
         Osk::flight.send_cmd("FM","DELETE_DIR with DIRECTORY #{dir_name}")
      end

   end # create_flt_dir()


   ############################################################################
   ##  File Transfer
   ############################################################################

   #
   # Appends a Osk::DMP_FILE_TAG character to the end of the base filename.
   # Note os_config.h defines the maximum filename and this extra character
   # could cause a write error if a filename is already at the max characters.   
   #   
   def self.create_dmp_filename(path_filename) 
   
   	extension = path_filename.split(".").last
      ext_index = path_filename.rindex(".")
      base_path_filename = path_filename.slice(0..(ext_index - 1))
      dmp_path_filename = base_path_filename + Osk::DMP_FILE_TAG + "." + extension
   
      #puts "#{path_filename}\n#{extension}\n#{base_path_filename}\n#{dmp_path_filename}\n"
      return dmp_path_filename
      
   end #create_dmp_filename()
   
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

   # Default gnd_path should be overridden for tables that have a separate file server directory
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

   # Default gnd_path should be overridden for tables that have a separate file server directory
   def self.put_flt_file_prompt(gnd_path = GND_SRV_DIR)
      #gnd_filename = ask_string("Enter ground filename without path. File will be located in #{gnd_path}.")
      #gnd_path_filename = "#{gnd_path}/#{gnd_filename}"
      gnd_path_filename = open_file_dialog(gnd_path)
      gnd_filename = File.basename(gnd_path_filename)
      flt_path_filename = ask_string("Enter full FSW /path/filename.             ","#{FLT_SRV_DIR}/#{gnd_filename}")
      put_file = put_flt_file(gnd_path_filename,flt_path_filename)
      return [put_file, flt_path_filename] 
   end # End put_flt_file_prompt()

   
   ############################################################################
   ##  Multi-step cFS Functions
   ############################################################################
   
   # 
   # Send a flight command that creates a binary file 
   # 1. Issue the flight command that creates the file
   # 2. Optionally transfer file to the ground and display 
   #    the file using the COSMOS Table Manager tool
   #    prompt - If true, then prompt the user if they want to transfer the
   #             file. If false, then always transfer the file 
   # 
   # Use the default server directories and temporary file names
   #
   # NOTE: THE COMMAND DEFINITION MUST USE THE KEYWORD 'FILENAME' AND 
   #       cmd_str MUST CONTAIN THE ENTIRE COMMAND EXCEPT FOR THE FILENAME
   #       PARAMETER. cmd_str MUST BE FORMATTED WITH OR WITHOUT A COMMA SO
   #       FILENAME CAN SIMPLY BE APPENDED. See table_mgmt_screen.rb's
   #       'write table registry to file' for an example.    
   #
   def self.send_flt_bin_file_cmd(app_name, cmd_str, tbl_mgr_def_filename, flt_path_filename: Osk::TMP_FLT_BIN_PATH_FILE, 
                                  gnd_rel_path: Osk::REL_SRV_DIR, prompt: true)
      cmd_str = "#{cmd_str} FILENAME #{flt_path_filename}"

      if (Osk::flight.app[app_name].send_cmd(cmd_str, Osk::OVERRIDE_TO_TRUE))  # Force validation because some file writes take a while
         
         if (prompt)
            get_file = message_box("#{app_name} created #{flt_path_filename}.\n\nWould you like to transfer the file from flight to ground and display it in Table Manager?\n", Osk::MSG_BUTTON_YES, Osk::MSG_BUTTON_NO,false)
         else
            get_file = Osk::MSG_BUTTON_YES
         end
         if (get_file == Osk::MSG_BUTTON_YES)
            filename = File.basename(flt_path_filename)
            get_flt_bin_file(flt_path_filename, gnd_rel_path, filename, tbl_mgr_def_filename)
         end
      else
         raise_exception "osk_ops.send_flt_bin_file_cmd() - #{app_name} command '#{cmd_str}' failed"
      end
   end # send_flt_bin_file_cmd()
 
   #
   # Uses generic send_flt_bin_file_cmd() to do the heavy listing but simplifies the user interface for the common
   # task of dumping and displaying cFE tables.
   #
   # Initial version intentionally kept simple. The interface should only be made more complex if the needed arises.
   # 1. Always dump the active table
   # 2. Always use the ground table server location   
   #
   def self.send_cfe_dump_tbl_cmd(tbl_name, tbl_mgr_def_filename, flt_tbl_path_filename: Osk::TMP_FLT_TBL_PATH_FILE, prompt: true)

      send_flt_bin_file_cmd("CFE_TBL", 
         "DUMP_TBL with ACTIVE_TBL_FLAG #{Fsw::Const::CFE_TBL_BufferSelect_ACTIVE}, TABLE_NAME #{tbl_name}, ", 
         tbl_mgr_def_filename, flt_path_filename: flt_tbl_path_filename, gnd_rel_path: REL_SRV_TBL_DIR, prompt: true)

   end # send_flt_bin_file_cmd()
 
   #
   # 1. Transfer a binary file from flight to ground 
   # 2. Launch COSMOS Table Manager tool to display the file if 
   # 
   # Ground relative directory path and filename passed in separately to help with user 
   # Table Manager instructions.
   #
   def self.get_flt_bin_file(flt_path_filename, gnd_rel_path, gnd_filename, tbl_mgr_def_filename, tlm_timeout=Osk::GET_FILE_TIMEOUT)
      gnd_path_filename = "#{Cosmos::USERPATH}/#{gnd_rel_path}/#{gnd_filename}"
      if (Osk::system.file_transfer.get(flt_path_filename,gnd_path_filename,tlm_timeout))
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
      if (Osk::flight.app[app_name].send_cmd(cmd_str, Osk::OVERRIDE_TO_TRUE))  # Force validation because some file writes take a while
         get_file = combo_box("Transfer file from flight to ground and display it?", Osk::MSG_BUTTON_YES,Osk::MSG_BUTTON_NO)
         if (get_file == Osk::MSG_BUTTON_YES)
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
   # working directory named widgets
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
   # - If command has no parameters, cmd is sent immediately
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
     
      TITLE #{target_name}-#{cmd_name}
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
      
      cmd_scr_file = "#{Osk::CFS_KIT_SCR_DIR}/#{Osk::TMP_FLT_CMD_SCR_FILE}"
      #puts "#{cmd_scr_file}"
      
      begin
         
         
         # Always overwrite the temp file      
         File.open("#{cmd_scr_file}","w") do |f| 
           
            f.write ("#{cmd_scr_header}")

            param_num = 1
            cmd_param_str = ""
            cmd_param_list.each do |param|
               cmd_param_str << "   NAMED_WIDGET param_#{param_num} LABEL '#{param[0]}'\n"         
               cmd_param_str << "   NAMED_WIDGET value_#{param_num} TEXTFIELD 64 \"#{param[1]}\"\n"  # Large enough for filenames
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
   
      tbl_mgr_scr_file = "#{Osk::CFS_KIT_SCR_DIR}/#{TMP_TBL_MGR_SCR_FILE}"
      
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
      
      get_bin_file_scr_file = "#{Osk::CFS_KIT_SCR_DIR}/#{TMP_GET_BIN_FILE_SCR_FILE}"
      
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

   ############################################################################
   ##  JSON Table Management
   ############################################################################

   
   def self.get_app(app_name, tbl_id)
      
      #TODO - Error checks
      app = Osk::flight.app[app_name]
      tbl_filename = app.tables[tbl_id].filename

      return [app, tbl_filename]
      
   end # get_app()
   
   def self.load_json_tbl(app_name, tbl_id)

      app, tbl_filename = get_app(app_name, tbl_id)

      put_file = combo_box("Transfer file from ground to flight?", Osk::MSG_BUTTON_YES,Osk::MSG_BUTTON_NO)
      if (put_file == Osk::MSG_BUTTON_YES)
         load_tbl, flt_path_filename = put_flt_file_prompt(GND_SRV_TBL_DIR)
      else
         load_tbl = true
         flt_path_filename = "#{Osk::FLT_SRV_DIR}/#{tbl_filename}"
      end 
   
      if (load_tbl == true)
         tbl_path_filename = ask_string("Enter full FSW /path/filename of table file to be loaded.","#{flt_path_filename}")
         load_type = combo_box("Select load type", 'Replace Table','Update Records')
         load_type == 'Replace Table' ? type = 0 : type = 1 
         app.send_cmd("#{Osk::CMD_STR_LOAD_TBL} with ID #{tbl_id}, TYPE #{type}, FILENAME #{tbl_path_filename}")
      end # if load tbl
   
   end # load_json_tbl()

   def self.dump_json_tbl(app_name, tbl_id)

      app, tbl_filename = get_app(app_name, tbl_id)
      tbl_path_filename = "#{Osk::FLT_SRV_DIR}/#{tbl_filename}"      

      tbl_path_filename = ask_string("Enter full flight /path/filename of file to receive the table.", create_dmp_filename(tbl_path_filename))
      Osk::Ops::send_flt_txt_file_cmd(app_name, "#{Osk::CMD_STR_DUMP_TBL} with ID #{tbl_id}, ", flt_path_filename: tbl_path_filename) 

   end # dump_json_tbl()

   #
   # Logic is tailored for dump table use case. It tries to locate a dump file and if not found it
   # asks the user if they want to display a load file (if present) 
   #
   def self.display_json_tbl(app_name, tbl_id)

      app, tbl_filename = get_app(app_name, tbl_id)

      tbl_path_filename = "#{Osk::GND_SRV_TBL_DIR}/#{tbl_filename}"
      dmp_tbl_path_filename = create_dmp_filename(tbl_path_filename)

      if File.file?(dmp_tbl_path_filename)
         Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{dmp_tbl_path_filename}'")
      else
         if File.file?(tbl_path_filename)
            prompt("Dump file not found. Displaying load file #{tbl_path_filename}")
            Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{tbl_path_filename}'")
         else
            answer = message_box("#{dmp_tbl_path_filename} and #{tbl_path_filename} do not exist. Launch empty file viewer?",Osk::MSG_BUTTON_YES,Osk::MSG_BUTTON_NO,false)
            if (answer == Osk::MSG_BUTTON_YES)
               Cosmos.run_process("ruby lib/OskTxtFileViewer")
            end
         end
      end

   end # display_json_tbl()

end # Module Ops
end # Module Osk

