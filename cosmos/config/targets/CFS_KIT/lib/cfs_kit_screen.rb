################################################################################
# cFS Kit Main Screen Scripts
#
# Notes:
#   None
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
################################################################################

require 'cosmos'
require 'cosmos/script'
Cosmos.catch_fatal_exception do
  require 'cosmos/tools/cmd_sender/cmd_sender'
  require 'cosmos/tools/tlm_viewer/screen'
  require 'cosmos/tools/tlm_viewer/tlm_viewer'
  require 'cosmos/tools/table_manager/table_config'
  require 'cosmos/tools/table_manager/table_manager_core'
end
require 'ccsds'
require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'fsw_app'
require 'simsat_ops_example_utils' 
require 'simsat_recorder_mgmt' 
require 'user_version'

require 'fileutils'

################################################################################
## Global Variables
################################################################################

# Used by file put/get command prototypes
FLT_TEST_GET_FILE = "#{Osk::FLT_SRV_DIR}/tf_get_test_src.txt"
FLT_TEST_PUT_FILE = "#{Osk::FLT_SRV_DIR}/tf_put_test_dst.txt"
GND_TEST_GET_FILE = "#{Osk::GND_SRV_DIR}/tf_get_test_dst.txt"
GND_TEST_PUT_FILE = "#{Osk::GND_SRV_DIR}/tf_put_test_src.txt"


################################################################################
## Explore cFS/SimSat
################################################################################

def cfs_kit_scr_explore_cfs(screen, cmd)

   case cmd
   when "START_CFS", "START_CFS_42"
   
      Osk::System.stop_n_start_cfs
      
      if (cmd == "START_CFS_42")
         wait 3                      # Give cFS chance to start
         Osk::System.start_42(true)  # true causes 42 simulator screen to be displayed
      end
      
   when "STOP_CFS"
      # Hopefully ES cleans up resources and does a controlled shutdown
      Osk::flight.cfe_es.send_cmd("RESET with RESTART_TYPE 1")
      wait 2
      Osk::System.stop_cfs() # Just in case orphans exists
   
   when "START_42"
      Osk::System.start_42

   when "STOP_42"
      Osk::System.stop_42
            
   when "CONFIG_SYS"
      user_selection = screen.get_named_widget("config_sys").text
      if user_selection == "About"
         about_str = ["Quick access to common commands used to configure the system:", 
                      "                                                 ",
                      "<b>Enable Telemetry</b> - Command telemetry output app to enable sending telemetry to COSMOS.",
                      "<b>Reset Time</b> - Set cFS time to 0.",
                      "<b>Ena/Dis Gnd Cmd Validation</b> - Enable/Disable ground verification of command counters using telemetry. Command verification slows system response.",
                      "<b>Config App Events</b> - Enable/Disable event types for an application. Typically used for debugging."]
         cfs_kit_create_about_screen("Configure System",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      else 
         case user_selection
         when "Enable Tlm"
            # Don't use Osk::flight because cmd validation would fail
            cmd("KIT_TO ENABLE_TELEMETRY")
         when "Reset Time"
            Osk::flight.cfe_time.send_cmd("SET_CLOCK_MET with SECONDS 0, MICROSECONDS 0")
            Osk::flight.cfe_time.send_cmd("SET_CLOCK with SECONDS 0, MICROSECONDS 0") 
         when "Ena/Dis Gnd Cmd Verify"
            enable = combo_box("Cmd validation verifies command counters in tlm after each\n cmd is sent. The system will run slower. \n\nDo you want to enable validation?", Osk::MSG_BUTTON_YES,Osk::MSG_BUTTON_NO)
            if (enable == Osk::MSG_BUTTON_YES)
               FswApp.validate_cmd(true)
            elsif (enable == Osk::MSG_BUTTON_NO)
               FswApp.validate_cmd(false)
            end
         when "Config App Events"
            display("CFE_EVS CFG_APP_EVENT_SCREEN",50,50)
         end
      end
      
   when "LEARN_OSK"
      user_selection = screen.get_named_widget("learn_osk").text
      case user_selection
      when "About"
         about_str = ["<b>Explore cFS/SimSat Main Page</b>",
                      "<pre>   <b>System Section</b> - Access to system level documentation and functionality</pre>",
                      "<pre>   <b>Applications</b>   - Access to resources for groups of apps that work together</pre>",
                      "<pre>   <b>cFE Services</b>   - Access to resources for each of the cFE services</pre>",
                      "      ",
                      "<b>This Drop Down Menu</b>",
                      "<pre>   <b>SimSat Overview</b> - Describe Simple Sat reference architecture and operational contect.</pre>",
                      "<pre>   <b>OSK Quick Start</b> - Highlights OSK features to allow a user to start experimenting.</pre>",
                      "<pre>   <b>OSK Users Guide</b> - Indepth description of all of the OSKs features with some design descriptions.</pre>",            
                      "<pre>   <b>OSK Version IDs</b> - Display version identifiers for all OSK components.</pre>"]
         cfs_kit_create_about_screen("Learn OSK",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      when "OSK Version IDs"
         cfs_kit_create_version_screen
         display("CFS_KIT #{File.basename(Osk::VERSION_SCR_FILE,'.txt')}",50,50)
         #about_str = ["<b>OpenSatKit</b> provides a complete <b>Core Flight System</b> (<i>https://github.com/nasa/cfs</i>) training and application",
         #             "development environment that includes the <b>COSMOS</b> (<i>https://cosmosrb.com</i>) mission",
         #             "control software and the <b>42 Dynamic Simulator</b> (<i>https://software.nasa.gov/software/GSC-16720-1</i>)",
         #             " ",
         #             "A special thanks to the following open source projects:",
         #             "   NASA/Goddard Core Flight System",
         #             "   Ball Aerospace COSMOS",
         #             "   NASA/Goddard 42 Simulator",
         #             " "]
         #cfs_kit_create_about_screen("Version: #{USER_VERSION}",about_str)
         #display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      else 
         # Displaying a document
         # Default to first choice - "SimpleSat Overview"
         doc_dir_filename = Osk::cfg_target_dir_file("SIMSAT","docs",Osk::SIMSAT_OVERVIEW_FILE)
         case user_selection
         when "OSK Quick Start"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::DOCS_QUICK_START_FILE}"
         when "OSK Users Guide"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::DOCS_USERS_GUIDE_FILE}"
         end
         spawn("evince #{doc_dir_filename}")
      end
      
   when "LEARN_CFS"
      user_selection = screen.get_named_widget("learn_cfs").text
      if user_selection == "About"
         about_str = ["<b>cFS Training Intro</b> - Overview of cFS program, business model and architecture.",
                      "<b>cFE Service Training</b> - Describes Core Flight Executive five services.",
                      "<b>OSK cFE Exercises</b> - Exercises for the cFE Service Training module.",
                      "<b>cFE App Training</b> - Describes cFE application development and runtime environment.",
                      "<b>OSK cFE Exercises</b> - Exercises for the cFE Application Training module."]        
         cfs_kit_create_about_screen("Learn cFS",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      else 
         # Default to first choice - SimpleSat_Overview
         doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::TRAIN_OSK_INTRO_FILE}"
         case user_selection
         when "cFS Training Intro"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::TRAIN_CFS_INTRO_FILE}"
         when "cFE Service Training"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::TRAIN_CFE_SERVICE_FILE}"
         when "OSK cFE Exercises"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::TRAIN_OSK_CFE_SERVICE_FILE}"
         when "cFE App Training"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::TRAIN_CFE_APP_DEV_FILE}"
         when "OSK cFE App Exercises"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::TRAIN_OSK_CFE_APP_DEV_FILE}"
         end
         spawn("evince #{doc_dir_filename}")
      end
      
   when "SIMPLE_SAT"
      user_selection = screen.get_named_widget("simple_sat").text
      if user_selection == "About"      
         about_str = ["Run scripts that work with the SimpleSat reference mission. Refer to <i>'SimpleSat Overview'</i> doc accessed",
                      "using the <i>'Learn OpenSatKit'</i> drop down menu.",
                      "                                             " ,                     
                      "<b>Functional Test</b> - Verify requirements. Uses Test Runner with 3 test suites: cFS, OSK, Mission.",
                      "<b>Integration Test</b> - Verify basic app functionality as a part of the SimSat system. Uses Script Runner.",
                      "<b>Ops Example</b> - Configures SimSat and performs a single operational contact. Uses Script Runner."]            
         cfs_kit_create_about_screen("Simple Sat",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      else 
         cfs_started = Osk::System.check_n_start_cfs
         case user_selection
         when "Functional Test"
            # Test suite file defined in  ~\cosmos\config\tools\test_runner\test_runner.txt
            spawn("ruby #{Osk::COSMOS_TST_RUNNER}")
         when "Integration Test"
            spawn("ruby #{Osk::COSMOS_SCR_RUNNER} simsat_intg_test.rb")
            display("SIMSAT CFS_APP_SCREEN",50,50)
            display("SIMSAT OSK_MISS_APP_SCREEN",1500,50)
         when "Ops Example"
            # No need to display a screen because a local screen is created by simsat_ops_example
            status_bar("Preparing for ops example. This could take several seconds.")
            simsat_ops_example_setup(!cfs_started) # If cFS wasn't just restarted then force a restart 
            status_bar("Spawning Script Runner to run the ops example script.")
            spawn("ruby #{Osk::COSMOS_SCR_RUNNER} simsat_ops_example.rb")
         end
      end

   when "SIMSAT_RUNTIME"
      display("SIMSAT RUNTIME_SCREEN",50,50)
   
   when "SIMSAT_DATA_FILE"
      display("SIMSAT DATA_FILE_SCREEN",50,50)
   
   when "SIMSAT_AUTONOMY"
      display("SIMSAT AUTONOMY_SCREEN",50,50)
   
   when "SIMSAT_ADC"
      display("SIMSAT ADC_SCREEN",50,50)
   
   when "SIMSAT_MAINTENANCE"
      display("SIMSAT MAINTENANCE_SCREEN",50,50)
   
   when "SIMSAT_HEALTH_SAFETY"
      display("SIMSAT HEALTH_SAFETY_SCREEN",50,50)
   
   when "CFE_EVENT_SERVICE"
      display("CFE_EVS CFE_EVS_SCREEN",50,50)
   
   when "CFE_EXECUTIVE_SERVICE"
      display("CFE_ES CFE_ES_SCREEN",50,50)
   
   when "CFE_SOFTWARE_BUS"
      display("CFE_SB CFE_SB_SCREEN",50,50)
   
   when "CFE_TABLE_SERVICE"
      display("CFE_TBL CFE_TBL_SCREEN",50,50)
   
   when "CFE_TIME_SERVICE"
      display("CFE_TIME CFE_TIME_SCREEN",50,50)
   
   when "CFE_USERS_GUIDE"
      Cosmos.open_in_web_browser("#{Osk::CFE_UG_DIR}/#{Osk::CFE_UG_FILE}")
   
   else
      raise "Error in screen definition file. Undefined command sent to cfs_kit_scr_explore_cfs()"
   end # cmd case

end # cfs_kit_scr_explore_cfs()

################################################################################
## Develop Apps
################################################################################


def cfs_kit_scr_develop_apps(screen, cmd)

   case cmd
   when "CFE_APP_DEV_GUIDE"
      # cFE does not deliver PDF file so I generate it in OSK's docs directory
      spawn("evince #{Osk::OSK_DOCS_DIR}/#{Osk::CFE_APP_DEV_FILE}")
   
   when "OSK_APP_DEV_GUIDE"
      prompt(Osk::MSG_TBD_FEATURE)
   
   when "OSK_APP_TRAIN_SLIDES"
      prompt(Osk::MSG_TBD_FEATURE)   

   when "OSK_DEMO_APP"
      prompt(Osk::MSG_TBD_FEATURE)   
   
   when "ADD_APP"
      # 
      # Create a list of apps that have been compiled (.o file exists in /cf directory) but have
      # not been loaded. Create a new screen that allows the user to select from a list to load
      # an app. This is a precurser to an app store
      #
      # 1. Get current FSW apps
      #    A. Get cFE ES app log
      #    B. Parse file and create a hash      
      # 2. Using OSK's internal App list of compiled apps create
      #    a list of potential apps that can be loaded by the user
      # 3. Create user screen
      #
      if (not Osk::System.cfs_running?)
         continue = message_box("This feature allows you to install an app to a running cFS system. Select <Yes> to start the FSW and run the script.  A terminal window will be created to run the FSW. Enter your user password when prompted.",Osk::MSG_BUTTON_YES,Osk::MSG_BUTTON_NO,false) #puts "continue = #{continue}"
         return unless continue == Osk::MSG_BUTTON_YES
         Osk::System.start_cfs
         wait 4  # Give the cFS time to start
      end
      Osk::flight.cfe_es.send_cmd("WRITE_APP_INFO_TO_FILE with FILENAME #{Osk::TMP_FLT_BIN_PATH_FILE}")
      wait 1
      #~if (Osk::system.file_transfer.get(Osk::TMP_FLT_BIN_PATH_FILE,Osk::TMP_GND_BIN_PATH_FILE,10)) # 10 sec timeout
      # Directly access the FSW file without transferring it to make this more responsive
         tbl_mgr_core = Cosmos::TableManagerCore.new                 
         app_info_bin_path_file = File.join(Osk::GND_TO_FLT_SRV_DIR,Osk::TMP_BIN_FILE)
         app_info_def_path_file = File.join(Osk::COSMOS_CFG_TBL_MGR_DIR,Osk::TBL_MGR_DEF_CFE_ES_APP_INFO)
         # Expect to get an exception because binary file size will be less than max number of records
         begin
            tbl_mgr_core.file_open(app_info_bin_path_file, app_info_def_path_file)
         rescue Cosmos::TableManagerCore::MismatchError => err
            #Quietly accept the exception
            #prompt("Caught MismatchError Exception\n#{err}")
         end
         fsw_apps = []
         tbl_mgr_core.config.tables.each do |table_name, table|
            #puts "table: Name=#{table.table_name}, rows=#{table.num_rows}, cols=#{table.num_columns}\n"
            ttable = tbl_mgr_core.config.table(table_name)
            ttable.sorted_items.each do |item|
               value = table.read(item.name, :FORMATTED)
               value = "0x" + value.simple_formatted unless value.is_printable?  # Handle binary strings
               #puts "ttable: item.name=#{item.name}, value=#{value}\n"
               # Find the APPxx_NAME entry and exclude the APPxx_MAIN_NAME
               if (item.name.include? "_NAME" and not item.name.include? "_MAIN_NAME")
                  fsw_apps << value unless value == ""
               end 
            end
         end
         app_list = Osk::flight.app.keys - fsw_apps
         # Remove apps that aren't built by default
         app_list.each do |app|
            if (not Osk::flight.app[app.to_s].sys_build)
               puts "Removing " + Osk::flight.app[app.to_s].fsw_name + "\n"
               app_list.delete(app)
            end
         end
         #~puts "Osk::flight.app[app].sys_build  = " + Osk::flight.app[app.to_s].sys_build.to_s + "\n"
         #~puts "fsw_apps: #{fsw_apps}\n"
         #~puts "Flight.app: #{Osk::flight.app.keys}\n"
         #~puts "Available Apps: #{app_list}\n"
      #~end # End if downlinked App Info file
      cfs_kit_add_app_screen("Add Application from Catalog", app_list)
      display("CFS_KIT #{File.basename(Osk::ADD_APP_SCR_FILE,'.txt')}",1000,10)
   when "ADD_APP_DESCR"
      app_name = screen.get_named_widget("app_name").text
      app_descr_array = Osk::flight.app[app_name].description 
      i = 1
      app_descr_array.each do |app_descr|
         scr_descr = screen.get_named_widget("descr_#{i}")
         scr_descr.text = app_descr
         i += 1
      end
      while i < 8 do  
         scr_descr = screen.get_named_widget("descr_#{i}")
         scr_descr.text = "         "
         i += 1
      end
   when "ADD_APP_LOAD"
      app_name = screen.get_named_widget("app_name").text
      app = Osk::flight.app[app_name]         
      Osk::flight.cfe_es.send_cmd("START_APP with APP_NAME #{app.fsw_name}, APP_ENTRY_POINT #{app.entry_symbol}, APP_FILENAME #{app.obj_path_filename}, STACK_SIZE #{app.stack_size}, EXCEPTION_ACTION 0, PRIORITY #{app.priority}")
      prompt("The cFS terminal window should display a #{app_name} initialization message.\nUse the cFS Command and Telmetry Server's window to access the app's Cmd & Tlm Packets")
      clear("CFS_KIT #{File.basename(Osk::ADD_APP_SCR_FILE,'.txt')}")
   when "REMOVE_APP"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Remove an app from cFE startup scr")   
   
   when "CREATE_APP"
      display("CFS_KIT MNG_APP_DEV_SCREEN",50,50)
   
   when "MANAGE_APP_RUNTIME"
      display("CFS_KIT MNG_APP_RUNTIME_SCREEN",50,50)   

   when "DEV_ADC_APP"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Guide for creating an app from control algorithms developed in 42 simulator")   
   
   when "DEV_ECI_APP"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Guide for creating an app using the External Code Interface")   
   
   when "PERF_MON_MGMT"
      display("CFS_KIT PERF_MON_SCREEN",50,50)
   
   when "PERF_MON_DEMO"
      display("CFS_KIT PERF_MON_DEMO_SCREEN",500,50)

   when "UNIT_TEST"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Run unit tests for all installed apps")   

   when "INTG_TEST"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Run integration test all installed apps")   

   else
      raise "Error in screen definition file. Undefined command sent to cfs_kit_scr_develop_apps()"
   end # cmd case

end # cfs_kit_scr_develop_apps()


################################################################################
## Extend OSK
################################################################################


def cfs_kit_scr_extend_osk(screen, cmd)

   case cmd
   when "PISAT"
	   #prompt("Please ensure you are connected to the PiSat network")
      #cmd("PICONTROL STARTCFS")
      #wait(2)
      #cmd("KIT_TO ENABLE_TELEMETRY")
      spawn("ruby #{Osk::COSMOS_TLM_GRAPHER}")
      display("CFS_KIT PISAT_CONTROL_SCREEN", 1000, 0)

   when "RUN_BENCHMARKS"
      prompt("This is a prototype app that will be updated for benchmarking\nremote targets. The BM app will be loaded.")
      display("CFS_KIT BENCHMARK_SCREEN",50,50)
      Osk::Ops.load_app("BM") unless Osk::Ops.app_loaded?("BM")

   when "RUN_PLATFORM_CERT"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Run platform certification against a target")   

   when "SBN"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Work with Software Bus Netowrk app")   

   when "ROS2"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Wrok with the Robotic Operating System")   

   when "UPDATE_CFE"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Instructions for ugrading to a new cFE version")   
   
   when "UPDATE_APP"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Instructions for ugrading an installed open source app")   
   
   else
      raise "Error in screen definition file. Undefined command sent to cfs_kit_scr_develop_apps()"
   end # cmd case

end # cfs_kit_scr_extend_osk()


################################################################################
## Leftovers from OSK 1.7 that need to be migrated
################################################################################

def cfs_kit_launch_app(screen, app)


   if (app == "UPDATE_TUTORIAL")
      # An exception will report any errors   
      if cfs_kit_create_tutorial_screen
         prompt ("Successfuly created tutorial screen file #{tutorial_scr_file}\nusing #{tutorial_def_file}")
      end
   elsif (app == "PROTO_APPP")
      #TODO - Investigate generic text table editor or tutorial screen
	   Cosmos.run_process("ruby lib/OskCfeFileViewer -f '/mnt/hgfs/OpenSatKit/cosmos/cfs_kit/file_server/cfe_es_syslog.dat'")
	   #Cosmos.run_process("ruby lib/OskTxtFileViewer -f '/mnt/hgfs/OpenSatKit/cosmos/test.json'")
      #require 'osk_tbl_editor'
      #Cosmos.run_process("ruby lib/OskTblEditor")
	   #require 'cfs_fcx_cmdgen'
      #Cosmos.run_process("ruby lib/CfsFcxCmdGen")
      #Cosmos.run_process("ruby tools/ConfigEditor")
      #Cosmos::OskTblEditor.run
      #Cosmos.run_cosmos_tool('ConfigEditor')

   elsif (app == "TUTORIAL")
      cfs_kit_launch_tutorial_screen
   end

end # cfs_kit_launch_app()


################################################################################
## Create Tutorial Screen
################################################################################

#
# When OSK is first installed the tutorial screen has the wrong absolute 
# address. Some user's ignore the instructions to run 'Update'. This function
# checks the absolute path used when the tutorial screen was created and if it
# doesn't match the current installation it creates a new tutorial screen. No
# user message is sent when the scr is created since they don't expect anything. 
#
def cfs_kit_launch_tutorial_screen

   tutorial_scr_file = "#{Osk::CFS_KIT_SCR_DIR}/#{Osk::TUTORIAL_SCR_FILE}"

   scr_tutorial_dir = File.open(tutorial_scr_file) {|f| f.readline}

   if scr_tutorial_dir.index(Osk::CFS_KIT_SCR_DIR).nil? 
      cfs_kit_create_tutorial_screen
   end
   
   display("CFS_KIT #{File.basename(Osk::TUTORIAL_SCR_FILE,'.txt')}",500,50)
      
end # cfs_kit_verify_tutorial_screen


def cfs_kit_create_tutorial_screen

   status = false
   
   t = Time.new 
   time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"
   
   tutorial_def_file = "#{Osk::TUTORIAL_DIR}/#{Osk::TUTORIAL_DEF_FILE}"
   tutorial_scr_file = "#{Osk::CFS_KIT_SCR_DIR}/#{Osk::TUTORIAL_SCR_FILE}"

   # Directory in first line is assumed by other functions
   tutorial_scr_header = "##{Osk::CFS_KIT_SCR_DIR}
   ###############################################################################
   # cfs_kit Tutorial Screen
   #
   # Notes:
   #   1. Do not edit this file because it is automatically generated and your
   #      changes will not be saved.
   #   2. File created by cfs_kit_create_tutorial_screen.rb on #{time_stamp}
   #
   # License:
   #   Written by David McComas, licensed under the copyleft GNU General Public
   #   License (GPL).
   #
   ###############################################################################

   SCREEN AUTO AUTO 0.5
   GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
  
   TITLE \"Tutorials\"
   SETTING BACKCOLOR 162 181 205
   SETTING TEXTCOLOR black
      
   VERTICALBOX \"\" 10
   "

   tutorial_scr_footer = "
   END # Vertical Box
   "
   begin
      
      json_file = File.read(tutorial_def_file)
      json_hash = JSON.parse(json_file)
    
      #puts json_hash
      #puts json_hash["tutorials"]
      #puts json_hash["tutorials"][0]["name"]

      #3/26/19 - Not seeing benefit of saving old file. If decide to keep it should go in
      #          a temp directory and not cluttter cfs_kit/screens
      #if File.exists? tutorial_scr_file
      #   filename = File.basename(tutorial_scr_file, File.extname(tutorial_scr_file))
      #   new_filename =  "#{Osk::CFS_KIT_SCR_DIR}/#{filename}#{time_stamp}"+File.extname(tutorial_scr_file)
      #   File.rename(tutorial_scr_file, new_filename)
      #end
      
      File.open(tutorial_scr_file,"w") do |f| 
	     
         f.write ("#{tutorial_scr_header}")
         
         json_hash["tutorials"].each do |tutorial|
            
            lesson_str = "#{tutorial["lessons"]}"
            lesson_str = lesson_str[1,(lesson_str.length-2)]  # Remove brackets [] and keep quotes around elements
            
            tutorial_menu = "
               HORIZONTAL 10
               BUTTON '#{tutorial["button"]}' 'require \"#{Cosmos::USERPATH}/config/targets/CFS_KIT/lib/tutorial_screen.rb\"; tutorial = combo_box(\"#{tutorial["user-prompt"]}\",#{lesson_str}); launch_tutorial(self, \"#{tutorial["directory"]}\", \"#{tutorial["format"]}\", tutorial)'
               SPACER 20 0 MAXIMUM FIXED          
               LABEL \"#{tutorial["description"]}\"
               SPACER 50 0 MINIMUMEXPANDING FIXED          
               END # Horizontal"
            
            f.write (tutorial_menu)
         
         end # Tutorial
         
         f.write ("#{tutorial_scr_footer}")

      end # File

      status = true
      
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

   return status
   
end # cfs_kit_create_tutorial_screen()


################################################################################
## Create Template Info Screen
################################################################################

def cfs_kit_create_tutorial_screen

   status = false
   
   t = Time.new 
   time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"
   
   tutorial_def_file = "#{Osk::TUTORIAL_DIR}/#{Osk::TUTORIAL_DEF_FILE}"
   tutorial_scr_file = "#{Osk::CFS_KIT_SCR_DIR}/#{Osk::TUTORIAL_SCR_FILE}"

   # Directory in first line is assumed by other functions
   tutorial_scr_header = "##{Osk::CFS_KIT_SCR_DIR}
   ###############################################################################
   # cfs_kit Tutorial Screen
   #
   # Notes:
   #   1. Do not edit this file because it is automatically generated and your
   #      changes will not be saved.
   #   2. File created by cfs_kit_create_tutorial_screen.rb on #{time_stamp}
   #
   # License:
   #   Written by David McComas, licensed under the copyleft GNU General Public
   #   License (GPL).
   #
   ###############################################################################

   SCREEN AUTO AUTO 0.5
   GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
  
   TITLE \"Tutorials\"
   SETTING BACKCOLOR 162 181 205
   SETTING TEXTCOLOR black
      
   VERTICALBOX \"\" 10
   "

   tutorial_scr_footer = "
   END # Vertical Box
   "
   begin
      
      json_file = File.read(tutorial_def_file)
      json_hash = JSON.parse(json_file)
    
      #puts json_hash
      #puts json_hash["tutorials"]
      #puts json_hash["tutorials"][0]["name"]

      #3/26/19 - Not seeing benefit of saving old file. If decide to keep it should go in
      #          a temp directory and not cluttter cfs_kit/screens
      #if File.exists? tutorial_scr_file
      #   filename = File.basename(tutorial_scr_file, File.extname(tutorial_scr_file))
      #   new_filename =  "#{Osk::CFS_KIT_SCR_DIR}/#{filename}#{time_stamp}"+File.extname(tutorial_scr_file)
      #   File.rename(tutorial_scr_file, new_filename)
      #end
      
      File.open(tutorial_scr_file,"w") do |f| 
	     
         f.write ("#{tutorial_scr_header}")
         
         json_hash["tutorials"].each do |tutorial|
            
            lesson_str = "#{tutorial["lessons"]}"
            lesson_str = lesson_str[1,(lesson_str.length-2)]  # Remove brackets [] and keep quotes around elements
            
            tutorial_menu = "
               HORIZONTAL 10
               BUTTON '#{tutorial["button"]}' 'require \"#{Cosmos::USERPATH}/config/targets/CFS_KIT/lib/tutorial_screen.rb\"; tutorial = combo_box(\"#{tutorial["user-prompt"]}\",#{lesson_str}); launch_tutorial(self, \"#{tutorial["directory"]}\", \"#{tutorial["format"]}\", tutorial)'
               SPACER 20 0 MAXIMUM FIXED          
               LABEL \"#{tutorial["description"]}\"
               SPACER 50 0 MINIMUMEXPANDING FIXED          
               END # Horizontal"
            
            f.write (tutorial_menu)
         
         end # Tutorial
         
         f.write ("#{tutorial_scr_footer}")

      end # File

      status = true
      
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

   return status
   
end # cfs_kit_create_tutorial_screen()

################################################################################
## Create About Info Screen
################################################################################


def cfs_kit_create_about_screen(scr_title, scr_text)

   t = Time.new 
   time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"

   scr_header = "
   ###############################################################################
   # cfs_kit About Screen
   #
   # Notes:
   #   1. Do not edit this file because it is automatically generated and your
   #      changes will not be saved.
   #   2. File created by create_app_screen.rb on #{time_stamp}
   #
   # License:
   #   Written by David McComas, licensed under the copyleft GNU General Public
   #   License (GPL).
   #
   ###############################################################################

   SCREEN AUTO AUTO 0.5
   GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
  
   TITLE \"#{scr_title}\"
   SETTING BACKCOLOR 162 181 205
   SETTING TEXTCOLOR black
      
   VERTICALBOX \"\" 10
   "

   scr_trailer = "
   END # Vertical Box
   "
   
   scr_file = File.join(Osk::CFS_KIT_SCR_DIR,Osk::ABOUT_SCR_FILE)

   begin
         
      # Always overwrite the temp file      
      File.open(scr_file,"w") do |f| 
           
         f.write (scr_header)

         #f.write ("\n   LABEL \"  \"\n")
  
         info_line = 1
         info_line_str = ""
         scr_text.each do |line|
            info_line_str << "   NAMED_WIDGET line_#{info_line} LABEL \"#{line}\"\n"         
            info_line_str << "   SETTING TEXTCOLOR 0 0 153\n"
            info_line += 1
         end
            
         f.write (info_line_str)
         f.write ("\n   LABEL \"  \"\n")
         f.write (scr_trailer)

      end # File
         
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

end # cfs_kit_create_about_screen()


################################################################################
## Create Add App Screen
################################################################################

def cfs_kit_add_app_screen(scr_title, app_list)

   t = Time.new 
   time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"

   scr_header = "
   ###############################################################################
   # Create App Screen
   #
   # Notes:
   #   1. Do not edit this file because it is automatically generated and your
   #      changes will not be saved.
   #   2. File created by cfs_kit_add_app_screen.rb on #{time_stamp}
   #
   # License:
   #   Written by David McComas, licensed under the copyleft GNU General Public
   #   License (GPL). 
   #
   ###############################################################################

   SCREEN AUTO AUTO 0.5
   GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
  
   TITLE \"#{scr_title}\"
     SETTING BACKCOLOR 162 181 205
     SETTING TEXTCOLOR black

   VERTICAL
   LABEL \"              \"
   LABEL \"              \"

   LABEL \"Drop down menu contains apps in the app catalog not currently running in the FSW.\"
   LABEL \"This is a prototype. Options will be added to\"
   LABEL \"1. Search online resources for available apps and install them in OSK\"
   LABEL \"2. Allow the user to control future app build and startup default behavior\"
   LABEL \"\"
   LABEL \"\"
   "

   scr_trailer = "
   HORIZONTALLINE
      HORIZONTALLINE
      LABEL \"App Overview\"
      SCROLLWINDOW
      VERTICAL
        NAMED_WIDGET descr_1 LABEL \"                                                                                                                                                \"
        NAMED_WIDGET descr_2 LABEL \"                                                                                                                                                \"
        NAMED_WIDGET descr_3 LABEL \"                                                                                                                                                \"
        NAMED_WIDGET descr_4 LABEL \"                                                                                                                                                \"
        NAMED_WIDGET descr_5 LABEL \"                                                                                                                                                \"
        NAMED_WIDGET descr_6 LABEL \"                                                                                                                                                \"
        NAMED_WIDGET descr_7 LABEL \"                                                                                                                                                \"
        NAMED_WIDGET descr_8 LABEL \"                                                                                                                                                \"
      END # Vertical
      END # Scroll Window


   END # Vertical
   "
   
   scr_file = File.join(Osk::CFS_KIT_SCR_DIR,Osk::ADD_APP_SCR_FILE)

   begin
         
      # Always overwrite the temp file      
      File.open(scr_file,"w") do |f| 
           
         f.write (scr_header)

         combo_text = ""
         app_list.each do |app|
            combo_text << "'" + app + "' "
         end
         
      scr_text = "
   MATRIXBYCOLUMNS 4
     LABEL \"Available Apps\"
     NAMED_WIDGET app_name COMBOBOX #{combo_text} 
     BUTTON 'Description' 'require \"/mnt/hgfs/OpenSatKit/cosmos/config/targets/CFS_KIT/lib/cfs_kit_screen.rb\"; cfs_kit_scr_develop_apps(self,\"ADD_APP_DESCR\")'
     BUTTON 'Load App' 'require \"/mnt/hgfs/OpenSatKit/cosmos/config/targets/CFS_KIT/lib/cfs_kit_screen.rb\"; cfs_kit_scr_develop_apps(self,\"ADD_APP_LOAD\")'
     SETTING BACKCOLOR 0 200 0
   END # Matrix
   "

         f.write (scr_text)
         
         f.write (scr_trailer)

      end # File
         
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

end # cfs_kit_add_app_screen()


################################################################################
## Version Identification Screen
################################################################################

def cfs_kit_create_version_screen

   t = Time.new 
   time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"

   scr_header = "
   ###############################################################################
   # Version Identififcation Screen
   #
   # Notes:
   #   1. Do not edit this file because it is automatically generated and your
   #      changes will not be saved.
   #   2. File created by cfs_kit_add_app_screen.rb on #{time_stamp}
   #
   # License:
   #   Written by David McComas, licensed under the copyleft GNU General Public
   #   License (GPL). 
   #
   ###############################################################################

   SCREEN AUTO AUTO 0.5
   GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
  
   TITLE \"OSK #{USER_VERSION_OSK}\"
     SETTING BACKCOLOR 162 181 205
     SETTING TEXTCOLOR black

   VERTICALBOX \"System Components\"
     MATRIXBYCOLUMNS 3 5 5
   
       LABEL \"<b>Name</b>\"
       LABEL \"<b>Version</b>\"
       LABEL \"<b>Supplier</b>\"
       
       LABEL \"COSMOS\"
       LABEL \"#{COSMOS_VERSION}\"
       LABEL \"Ball Aerospace\"
       
       LABEL \"COSMOS-CFDP\"
       LABEL \"#{USER_VERSION_CFDP}\"
       LABEL \"Visiona\"
       
       LABEL \"42\"
       LABEL \"#{USER_VERSION_42}\"
       LABEL \"NASA/Goddard\"

       LABEL \"cFE\"
       LABEL \"#{USER_VERSION_CFE}\"
       LABEL \"NASA/Goddard\"

       LABEL \"OSAL\"
       LABEL \"#{USER_VERSION_OSAL}\"
       LABEL \"NASA/Goddard\"

       LABEL \"PSP\"
       LABEL \"#{USER_VERSION_PSP}\"
       LABEL \"NASA/Goddard\"
     
     END # Matrix
   END # VerticalBox

   HORIZONTALLINE

   VERTICALBOX \"FSW Applications\"
     SCROLLWINDOW
     MATRIXBYCOLUMNS 3 5 5
     
       LABEL \"<b>Name</b>\"
       LABEL \"<b>Version</b>\"
       LABEL \"<b>Supplier</b>\"
   
   " # End scr_header
       
       
   scr_trailer = "
     END # Scroll Window
     END # Matrix
   END # VerticalBox
   "
   scr_file = File.join(Osk::CFS_KIT_SCR_DIR,Osk::VERSION_SCR_FILE)

   begin
         
      # Always overwrite the auto generated screen file      
      File.open(scr_file,"w") do |f| 
           
         f.write (scr_header)

         Osk::flight.app.each_value { |app|
            # Skip CFE apps
            if not app.fsw_name.include? "CFE_"
               f.write ("       LABEL \"#{app.title}\"\n")
               f.write ("       LABEL \"#{app.version}\"\n")
               f.write ("       LABEL \"#{app.owner}\"\n\n")         
            end
         }
         
         f.write (scr_trailer)

      end # File
         
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

end # cfs_kit_create_version_screen()

