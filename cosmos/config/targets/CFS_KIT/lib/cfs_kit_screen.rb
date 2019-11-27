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
end
require 'ccsds'
require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'fsw_app'
require 'simsat_ops_example_utils' 
require 'simsat_file_mgmt' 
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
   when "START_CFS"
   
      # Kill all instances of the cFS before starting a new instance. 
      Osk::System.stop_cfs
      Osk::System.start_cfs  # Enables telemetry
      
   when "START_CFS_42"
         
      # Kill all instances of the cFS before starting a new instance. 

      Osk::System.stop_cfs
      Osk::System.start_cfs  # Enables telemetry

      Osk::System.start_42(true)  # true causes 42 simulator screen to be displayed
              
   when "STOP_CFS"
      # Hopefully ES cleans up resources and does a controlled shutdown
      Osk::flight.cfe_es.send_cmd("RESET with RESTART_TYPE 1")
      #Osk::System.stop_cfs()
   
   when "STOP_42"
      Osk::System.stop_42
            
   when "CONFIG_SYS"
      user_selection = screen.get_named_widget("config_sys").text
      if user_selection == "About"
         about_str = ["<b>Enable Telemetry</b> - Send command to the telemetry output app to enable telemetry.",
                      "<b>Reset Time</b> - Set cFS time to 0.",
                      "<b>Config cmd validation</b> - Ena/Dis cmd counter telemetry verification."]
         cfs_kit_create_about_screen("Configure System",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      else 
         case user_selection
         when "Enable_Tlm"
            # Don't use Osk::flight because cmd validation would fail
            cmd("KIT_TO ENABLE_TELEMETRY")
         when "Reset_Time"
            Osk::flight.cfe_time.send_cmd("SET_CLOCK_MET with SECONDS 0, MICROSECONDS 0")
            Osk::flight.cfe_time.send_cmd("SET_CLOCK with SECONDS 0, MICROSECONDS 0") 
         when "Config_Cmd_Verify"
            enable = combo_box("Cmd validation verifies command counters in tlm after each\n cmd is sent. The system will run slower. \n\nDo you want to enable validation?", Osk::MSG_BUTTON_YES,Osk::MSG_BUTTON_NO)
            if (enable == Osk::MSG_BUTTON_YES)
               FswApp.validate_cmd(true)
            elsif (enable == Osk::MSG_BUTTON_NO)
               FswApp.validate_cmd(false)
            end
         end
      end
      
   when "LEARN_OSK"
      user_selection = screen.get_named_widget("learn_osk").text
      case user_selection
      when "About"
         about_str = ["<b>SimSat Overview</b> - Describe Simple Sat reference architecture and operational contect.",
                      "<b>OSK Quick Start</b> - Highlights OSK features to allow a user to start experimenting.",
                      "<b>OSK Users Guide</b> - Indepth description of all of the OSKs features with some design descriptions.",            
                      "<b>OSK Version</b>     - Display the OpenSatKit version identifier."]
         cfs_kit_create_about_screen("Learn OSK",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      when "OSK_Version"
         about_str = ["<b>OpenSatKit</b> provides a complete <b>Core Flight System</b> (<i>https://github.com/nasa/cfs</i>) training and application",
                      "development environment that includes the <b>COSMOS</b> (<i>https://cosmosrb.com</i>) mission",
                      "control software and the <b>42 Dynamic Simulator</b> (<i>https://software.nasa.gov/software/GSC-16720-1</i>)",
                      " ",
                      "A special thanks to the following open source projects:",
                      "   NASA/Goddard Core Flight System",
                      "   Ball Aerospace COSMOS",
                      "   NASA/Goddard 42 Simulator",
                      " "]
         cfs_kit_create_about_screen("Version: #{USER_VERSION}",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      else 
         # Default to first choice - SimpleSat_Overview
         doc_dir_filename = Osk::target_dir_file("SIMSAT","docs",Osk::SIMSAT_OVERVIEW_FILE)
         case user_selection
         when "OSK_Quick_Start"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::DOCS_QUICK_START_FILE}"
         when "OSK_Users_Guide"
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
         when "cFS_Training_Intro"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::TRAIN_CFS_INTRO_FILE}"
         when "cFE_Service_Training"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::TRAIN_CFE_SERVICE_FILE}"
         when "OSK_cFE_Exercises"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::TRAIN_OSK_CFE_SERVICE_FILE}"
         when "cFE_App_Training"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::TRAIN_CFE_APP_DEV_FILE}"
         when "OSK_cFE_App_Exercises"
            doc_dir_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::TRAIN_OSK_CFE_APP_DEV_FILE}"
         end
         spawn("evince #{doc_dir_filename}")
      end
      
   when "RUN_SCRIPT"
      user_selection = screen.get_named_widget("run_script").text
      if user_selection == "About"      
         about_str = ["<b>Functional Test</b> - Verify requirements. Uses Test Runner with 3 test suites: cFS, OSK, Mission.",
                      "<b>Integration Test</b> - Verify basic app functionality as a part of the SimSat system. Uses Script Runner.",
                      "<b>Ops Example</b> - Configures SimSat and performs a single operational contact. Uses Script Runner."]            
         cfs_kit_create_about_screen("Example Script",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      else 
         cfs_started = false
         if (not Osk::System.cfs_running?)
            continue = message_box("The flight software is not running. Select <Yes> to start the FSW and run the script.  A terminal window will be created to run the FSW. Enter '#{Osk::PASSWORD}' for the password.",Osk::MSG_BUTTON_YES,Osk::MSG_BUTTON_NO,false)            #puts "continue = #{continue}"
            return unless continue == Osk::MSG_BUTTON_YES
            Osk::System.start_cfs  # Enables telemetry
            cfs_started = true 
         end
         case user_selection
         when "Functional_Test"
            # Test suite file defined in  ~\cosmos\config\tools\test_runner\test_runner.txt
            spawn("ruby #{Osk::COSMOS_TST_RUNNER}")
         when "Integration_Test"
            spawn("ruby #{Osk::COSMOS_SCR_RUNNER} simsat_intg_test.rb")
            display("SIMSAT CFS_APP_SCREEN",50,50)
            display("SIMSAT OSK_MISS_APP_SCREEN",1500,50)
         when "Ops_Example"
            # No need to display a screen because a local screen is created by simsat_ops_example
            status_bar("Preparing for ops example. This could take several seconds.")
            simsat_ops_example_setup(!cfs_started)            
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
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Add an app from existing app library")   
   
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

   tutorial_scr_file = "#{Osk::SCR_DIR}/#{Osk::TUTORIAL_SCR_FILE}"

   scr_tutorial_dir = File.open(tutorial_scr_file) {|f| f.readline}

   if scr_tutorial_dir.index(Osk::SCR_DIR).nil? 
      cfs_kit_create_tutorial_screen
   end
   
   display("CFS_KIT #{File.basename(Osk::TUTORIAL_SCR_FILE,'.txt')}",500,50)
      
end # cfs_kit_verify_tutorial_screen


def cfs_kit_create_tutorial_screen

   status = false
   
   t = Time.new 
   time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"
   
   tutorial_def_file = "#{Osk::TUTORIAL_DIR}/#{Osk::TUTORIAL_DEF_FILE}"
   tutorial_scr_file = "#{Osk::SCR_DIR}/#{Osk::TUTORIAL_SCR_FILE}"

   # Directory in first line is assumed by other functions
   tutorial_scr_header = "##{Osk::SCR_DIR}
   ###############################################################################
   # cfs_kit Tutorial Screen
   #
   # Notes:
   #   1. Do not edit this file because it is automatically generated and your
   #      changes will not be saved.
   #   2. File created by cfs_kit_screen.rb on #{time_stamp}
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
      #   new_filename =  "#{Osk::SCR_DIR}/#{filename}#{time_stamp}"+File.extname(tutorial_scr_file)
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
   
   scr_file = File.join(Osk::SCR_DIR,Osk::ABOUT_SCR_FILE)

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

