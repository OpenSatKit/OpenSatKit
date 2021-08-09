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
require 'cosmos/tools/table_manager/table_config'
require 'cosmos/tools/table_manager/table_manager_core'
#Cosmos.catch_fatal_exception do
#  require 'cosmos/tools/cmd_sender/cmd_sender'
#  require 'cosmos/tools/tlm_viewer/screen'
#  require 'cosmos/tools/tlm_viewer/tlm_viewer'
#end
require 'ccsds'
require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'osk_education'
require 'fsw_app'
require 'simsat_ops_example_utils' 
require 'simsat_recorder_mgmt' 
require 'user_version'
require 'fileutils'
require 'osk_c_tutor_scr'
require 'cfs_kit_const'
require 'scsim_const'
require 'cfsat_const'
require 'pisat_const'
require 'simsat_const'
require 'sandbox_const'


################################################################################
## Global Variables
################################################################################

# Used by file put/get command prototypes
FLT_TEST_GET_FILE = File.join(Osk::FLT_SRV_DIR,"tf_get_test_src.txt")
FLT_TEST_PUT_FILE = File.join(Osk::FLT_SRV_DIR,"tf_put_test_dst.txt")
GND_TEST_GET_FILE = File.join(Osk::GND_SRV_DIR,"tf_get_test_dst.txt")
GND_TEST_PUT_FILE = File.join(Osk::GND_SRV_DIR,"tf_put_test_src.txt")


################################################################################
## Common Functions
################################################################################

def cfs_kit_scr_common(screen, cmd)

   case cmd
   
   when "STOP_CFS"
      # Hopefully ES cleans up resources and does a controlled shutdown
      Osk::flight.cfe_es.send_cmd("RESET with RESTART_TYPE 1")
      wait 2
      Osk::System.stop_cfs() # Just in case orphans exists
   
   when "START_42"
      Osk::System.start_42

   when "STOP_42"
      Osk::System.stop_42

   when "CONFIG_SYS_SIMSAT", "CONFIG_SYS_CFSAT", "CONFIG_SYS_PISAT", "CONFIG_SYS_SANDBOX"
      target = cmd.sub(/CONFIG_SYS_/,'').downcase
      user_selection = screen.get_named_widget("config_sys_#{target}").text
      if user_selection == "About"
         about_str = ["Quick access to common commands used to configure the target system:", 
                      "                                                 ",
                      "<pre><b>Enable Telemetry</b>       - Command telemetry output app to enable sending telemetry to COSMOS.</pre>",
                      "<pre><b>Reset Time</b>             - Set cFS time to 0.</pre>",
                      "<pre><b>Ena/Dis Gnd Cmd Verify</b> - Enable/Disable ground verification of command counters using telemetry. Command verification slows system response.</pre>",
                      "<pre><b>Config App Events</b>      - Enable/Disable event types for an application. Typically used for debugging.</pre>",
                      "<pre><b>Ena/Dis Flywheel Event</b> - Enable/Disable cFE TIME's start/stop flywheel events. Occurs freqeuntly since OSK/Linux environment is non-realtime. </pre>",
                      "<pre><b>Ena/Dis HS Aliveness</b>   - Enable/Disable Health & Safety app's aliveness dot character output in the cFS terminal window.</pre>"]
         cfs_kit_create_about_screen("Configure System",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      else 
         case user_selection
         when "Enable Telemetry"
            # Don't use Osk::flight because cmd validation would fail
            cmd("KIT_TO ENABLE_TELEMETRY")
         when "Reset Time"
            Osk::flight.cfe_time.send_cmd("SET_CLOCK_MET with SECONDS 0, MICROSECONDS 0")
            Osk::flight.cfe_time.send_cmd("SET_CLOCK with SECONDS 0, MICROSECONDS 0") 
         when "Ena/Dis Gnd Cmd Verify"
            # COSMOS *_box methods have a bug with the cancel option. The space in Cancel is intentional. Message box displays selections in reverse order with the last selection highlighted. 
            enable = message_box("Cmd validation verifies command counters in tlm after each\ncmd is sent. The system will run slower. \n\nDo you want to enable validation?", 'Yes', 'No', 'Cancel ', false)
            if (enable == Osk::MSG_BUTTON_YES)
               FswApp.validate_cmd(true)
            elsif (enable == Osk::MSG_BUTTON_NO)
               FswApp.validate_cmd(false)
            end
         when "Ena/Dis Flywheel Event"
            # COSMOS *_box methods have a bug with the cancel option. The space in Cancel is intentional. Message box displays selections in reverse order with the last selection highlighted. 
            enable = message_box("cFE TIME outputs an event when it starts/stops flywheel\n mode which occurs when time can't synch to the 1Hz pulse.\n\nDo you want to enable the event messages?", 'Cancel ', 'Disable', 'Enable', false)
            if (enable == 'Enable')
               cmd("CFE_EVS SET_FILTER_MASK with APP_NAME CFE_TIME, EVENT_ID #{Fsw::Const::CFE_TIME_FLY_ON_EID}, MASK #{Fsw::Const::CFE_EVS_NO_FILTER}")
               wait(1)
               cmd("CFE_EVS SET_FILTER_MASK with APP_NAME CFE_TIME, EVENT_ID #{Fsw::Const::CFE_TIME_FLY_OFF_EID}, MASK #{Fsw::Const::CFE_EVS_NO_FILTER}")
            elsif (enable == 'Disable')
               cmd("CFE_EVS SET_FILTER_MASK with APP_NAME CFE_TIME, EVENT_ID #{Fsw::Const::CFE_TIME_FLY_ON_EID}, MASK #{Fsw::Const::CFE_EVS_FIRST_ONE_STOP}")
               wait(1)
               cmd("CFE_EVS SET_FILTER_MASK with APP_NAME CFE_TIME, EVENT_ID #{Fsw::Const::CFE_TIME_FLY_OFF_EID}, MASK #{Fsw::Const::CFE_EVS_FIRST_ONE_STOP}")
            end
         when "Ena/Dis HS Aliveness"
            # COSMOS *_box methods have a bug with the cancel option. The space in Cancel is intentional.
            enable = message_box("Health & Safety periodically outputs a dot character to the\ncFS terminal window to indicate it is running.\n\nDo you want to enable the aliveness indicator?", 'Cancel ', 'Disable', 'Enable', false)
            if (enable == 'Enable')
               Osk::flight.send_cmd("HS","ENA_ALIVENESS")
            elsif (enable == 'Disable')
               Osk::flight.send_cmd("HS","DIS_ALIVENESS")
            end
         when "Config App Events"
            display("CFE_EVS CFG_APP_EVENT_SCREEN",50,50)
         end
      end # user_selection
      
   when "LEARN_OSK_SIMSAT", "LEARN_OSK_CFSAT", "LEARN_OSK_SANDBOX"
      target = cmd.sub(/LEARN_OSK_/,'').downcase
      user_selection = screen.get_named_widget("learn_osk_#{target}").text
      case user_selection
      when "About"
         about_str = ["<b>OpenSatKit Main Page Tabs</b>",
                      "<pre>   <b>Mission FSW</b> - Resources for developing Mission FSW using the SimSat reference mission</pre>",
                      "<pre>   <b>cFS FSW Eng</b> - Resources for learning the cFE services and developing cFS apps</pre>",
                      "<pre>   <b>PiSat</b>       - Configure a Raspberry Pi with an OSK cFS distribution and connect it to COSMOS</pre>",
                      "<pre>   <b>R&D</b>         - A collection of research and development apps</pre>",
                      "      ",
                      "<b>This Drop Down Menu</b>",
                      "<pre>   <b>OSK Quick Start</b>    - Guides the user for how to best use OSK to meet their needs</pre>",
                      "<pre>   <b>OSK Overview Video</b> - Introduces OSK and its goals</pre>",
                      "<pre>   <b>OSK Version IDs</b>    - Display version identifiers for all OSK components</pre>"]
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
      when "OSK Overview Video"
         Osk::education_video(CFS_KIT_YOUTUBE_OSK_OVERVIEW)
      else 
         # Displaying a document
         doc_filename = nil
         case user_selection
         when "OSK Quick Start Doc"
            doc_filename = "#{Osk::OSK_DOCS_DIR}/#{Osk::OSK_QS_FILE}"
         end
         Osk::System.display_pdf(doc_filename) unless doc_filename.nil?
      end

   else
      raise "Error in screen definition file. Undefined command sent to cfs_kit_scr_common()"
   end # cmd case   

end # def cfs_kit_scr_common()


################################################################################
## cFS Education - cfsat
################################################################################

def cfs_kit_scr_cfsat(screen, cmd)

   case cmd
   
   ## System
   
   when "START_CFS"
      Osk::System.stop_n_start_cfs('cfsat')
      

   ## Docs & Videos

   when "QUICK_START"
      doc_filename = Osk::cfg_target_dir_file("CFSAT", "docs", CfSat::QUICK_START_FILE)
      Osk::System.display_pdf(doc_filename) unless doc_filename.nil?

   when "DOCS_VIDEOS"
      user_selection = screen.get_named_widget("cfsat_docs_videos").text
      if user_selection == "About"
         about_str = ["<pre><b>cFS Overview Slides</b>  - Overview of cFS program, business model, and architecture</pre>",
                      "<pre><b>cFS Overview Video</b>   - Overview of the core Flight System (cFS) architecture</pre>",
                      "<pre><b>Building cFS Video</b>   - Shows how to compile and link the core Flight System (cFS)</pre>",
                      "<pre><b>cFE Overview Slides</b>  - Overview of the core Flight Executive architecture and services</pre>",
                      "                                           ",
                      "Each screen in the <i>cFE</i> and <i>Develop cFS Apps</i> sections have links to more learning resources"]
         cfs_kit_create_about_screen("Learn cFS",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      else 
         doc_filename = nil
         case user_selection
         when "cFS Overview Video"
            Osk::education_video(CfSat::CFS_OVERVIEW_YOUTUBE)
         when "cFS Overview Slides"
            doc_filename = Osk::cfg_target_dir_file("CFSAT", "docs", CfSat::CFS_OVERVIEW_FILE)
         when "Building cFS Video"
            Osk::education_video(CfSat::CFS_BUILD_OVERVIEW_YOUTUBE)
         when "cFE Overview Slides"
            doc_filename = Osk::cfg_target_dir_file("CFSAT", "docs", CfSat::CFE_OVERVIEW_FILE)         
         end
         Osk::System.display_pdf(doc_filename) unless doc_filename.nil?
      end
      
   ## core Flight Executive

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
      Cosmos.open_in_web_browser(File.join(Osk::CFE_UG_DIR,Osk::CFE_UG_FILE))
   
   ## Develop Apps

   when "CFE_APP_DEV_GUIDE"
      # cFE does not deliver PDF file so I generate it in OSK's docs directory
      Osk::System.display_pdf(File.join(Osk::CFS_DOCS_DIR,Osk::CFE_APP_DEV_FILE))
   
   when "OSK_APP_DEV_GUIDE"
      osk_app_dev_file = File.join(Osk::OSK_DOCS_DIR,Osk::OSK_APP_DEV_FILE)
      Osk::System.display_pdf(osk_app_dev_file)
   when "CREATE_APP"
      display("CFSAT CREATE_APP_SCREEN",50,50)
   
   when "OSK_APP_TUTORIAL"
      # Generate and display tutorial screen
      osk_c_tutor_start
      display("CFSAT OSK_C_TUTOR_SCR")
   
   when "DEV_ADC_APP"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Guide for creating an app from control algorithms developed in 42 simulator")   
   
   when "DEV_ECI_APP"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Guide for creating an app using the External Code Interface\nSee https://github.com/nasa/ECI")   
   
   else
      raise "Error in screen definition file. Undefined command sent to cfs_kit_scr_cfsat()"
   end # cmd case

end # cfs_kit_scr_cfsat()


################################################################################
## Mission FSW - simsat
################################################################################

def cfs_kit_scr_simsat(screen, cmd)

   doc_filename = nil
   case cmd
   
   ## System
   
   when "START_CFS", "START_CFS_42"
   
      Osk::System.stop_n_start_cfs('simsat')
      
      if (cmd == "START_CFS_42")
         wait 3                      # Give cFS chance to start
         Osk::System.start_42(true)  # true causes 42 simulator screen to be displayed
      end

   ## Docs & Videos

   when "QUICK_START"
      doc_filename = Osk::cfg_target_dir_file("SIMSAT", "docs", SimSat::MISSION_FSW_QS_FILE)
      Osk::System.display_pdf(doc_filename) unless doc_filename.nil?
   when "DOCS_VIDEOS"
      user_selection = screen.get_named_widget("simsat_docs_videos").text
      case user_selection
      when "About"
         about_str = ["<b>Develop Mission FSW Screen</b>",
                      "<pre>   <b>System Section</b> - Manage cFS SimSat target and access OSK docs & videos</pre>",
                      "<pre>   <b>Docs & Videos</b>  - Launch system level docs and videos about mission FSW development with SimSat</pre>",
                      "<pre>   <b>App Groups</b>     - Access to resources for groups of apps that work together</pre>",
                      "<pre>   <b>Tune, Ver, Val</b> - Access to resources used to tune and test your system</pre>",
                      "      ",
                      "<b>This Drop Down Menu</b>",
                      "<pre>   <b>SimSat Overview</b> - Describes OSK SimSat reference mission and how to evolve for your mission FSW</pre>"]
         cfs_kit_create_about_screen("Develop Mission FSW",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      when "About Tune, Verify, Validate"
         about_str = ["USe the following resources to tune and test the SimSat target:",
                      "                                             " ,                     
                      "<pre><b>Perf Monitor</b>     - Manage & demo the performance monitor tool to help tune your system</pre>",
                      "<pre><b>Unit Test</b>        - Compile & run individual executables that verify cFE and app components</pre>",
                      "<pre><b>Functional Test</b>  - Verify requirements. Uses Test Runner with 3 test suites: cFS, OSK, Mission</pre>",
                      "<pre><b>Integration Test</b> - Verify basic app functionality as a part of the SimSat system. Uses Script Runner</pre>",
                      "<pre><b>Sys&Ops Examples</b> - System functional test and ops example scripts. Uses Script Runner</pre>"]            
         cfs_kit_create_about_screen("Tune, Verify, and Validate",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      when "SimSat Overview Doc"
         doc_filename = Osk::cfg_target_dir_file("SIMSAT","docs",SimSat::MISSION_FSW_SIMSAT_FILE)
         Osk::System.display_pdf(doc_filename) unless doc_filename.nil?
      end

   ## App Groups

   when "RUNTIME_APPS"
      display("SIMSAT RUNTIME_SCREEN",50,50)
   
   when "DATA_FILE_APPS"
      display("SIMSAT DATA_FILE_SCREEN",50,50)
   
   when "AUTONOMY_APPS"
      display("SIMSAT AUTONOMY_SCREEN",50,50)
   
   when "ADC_APPS"
      display("SIMSAT ADC_SCREEN",50,50)
   
   when "MAINTENANCE_APPS"
      display("SIMSAT MAINTENANCE_SCREEN",50,50)
   
   when "HEALTH_SAFETY_APPS"
      display("SIMSAT HEALTH_SAFETY_SCREEN",50,50)

   ## Tune, Verify, and Validate

   when "PERF_MON_MGMT"
      display("CFS_KIT PERF_MON_SCREEN",50,50)
   
   when "PERF_MON_DEMO"
      display("CFS_KIT PERF_MON_DEMO_SCREEN",500,50)
    
   when "UNIT_TEST"
      Osk::System.check_n_start_cfs('simsat')
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Run unit tests for all installed apps.\nThiswill be added when OSK is updated to cFE 7.0")   

   when "INTG_TEST"
      Osk::System.check_n_start_cfs('simsat')
      spawn("ruby #{Osk::COSMOS_SCR_RUNNER} simsat_intg_test.rb")
      display("SIMSAT CFS_APP_SCREEN",50,50)
      display("SIMSAT OSK_APP_SCREEN",1500,50)

   when "FUNC_TEST"
      # Test suite file defined in  ~\cosmos\config\tools\test_runner\test_runner.txt
      # Ensure simsat target is running & it's the default state
      Osk::System.check_n_start_cfs('simsat')
      user_selection = screen.get_named_widget("app_func_test").text
      case user_selection
      when "Manually Select"
         test_suite = ""
      when "cFS Test Suite"
         test_suite = "--suite SimSatFuncTestSuiteCfs"
      when "OSK Test Suite"
         test_suite = "--suite SimSatFuncTestSuiteOsk"
      end # user_selection
      spawn("ruby #{Osk::COSMOS_TST_RUNNER} #{test_suite}")

   when "SYS_OPS_EX"
      # v3.0: Only one selection. Need to break down into educational pieces
      # No need to display a screen because a local screen is created by simsat_ops_example
      status_bar("Preparing for ops example. This could take several seconds.")
      simsat_ops_example_setup(true) # true => force a cFS restart 
      status_bar("Spawning Script Runner to run the ops example script.")
      spawn("ruby #{Osk::COSMOS_SCR_RUNNER} simsat_ops_example.rb")
    
   else
      raise "Error in screen definition file. Undefined command sent to cfs_kit_scr_simsat()"
   end # cmd case

end # cfs_kit_scr_simsat()


################################################################################
## PiSat
################################################################################

def cfs_kit_scr_pisat(screen, cmd)

   case cmd
   
   ## System

   when "CONFIG_SYS_PISAT"
      # Intercept the common config cmd to allow user to specify IP address for enabling telemetry       
      user_selection = screen.get_named_widget("config_sys_pisat").text
      if (user_selection == "Enable Telemetry")
         Cosmos.run_process("ruby tools/CmdSender -p \"KIT_TO ENABLE_TELEMETRY\"")
      else
         cfs_kit_scr_common(self,cmd)
      end

   when "CFS_START"
      cmd("PISAT CFS_START")
   
   when "CFS_STOP"
      cmd("PISAT CFS_STOP") 

   when "PI_NOOP"
      cmd("PISAT PI_NOOP")
   
   when "PI_ENA_TLM"
      cmd("PISAT PI_ENA_TLM")

   when "PI_CTRL_EXIT"
      cmd("PISAT PI_CTRL_EXIT")

   when "PI_REBOOT"
      cmd("PISAT PI_REBOOT")
   
   when "PI_SHUTDOWN"
      cmd("PISAT PI_SHUTDOWN")

   when "CMD_TLM_SERVER"
      cmd_tlm_server = screen.get_named_widget("cmd_tlm_server").text
      prompt("Switching between a local cFS local and a remote PiSat without restarting COSMOS is not supported at this time. See cosmos/lib/hw_target.rb for switching between targets.")
      return
      case cmd_tlm_server
      when "PiSat"
         display("PICONTROL PISAT_CONNECT_SCREEN",50,50)
      when "Local cFS"
         pisat_connection(screen,"SWITCH_TO_LOCAL");
      end   
   

   ## Pi-Sat Docs & Videos
   
   when "QUICK_START"
      doc_filename = Osk::cfg_target_dir_file("PISAT", "docs", PiSat::QUICK_START_FILE)
      Osk::System.display_pdf(doc_filename) unless doc_filename.nil?
   when "DOCS_VIDEOS"
      user_selection = screen.get_named_widget("pisat_docs_videos").text
      case user_selection
      when "About"
         about_str = ["<b>Pi-Sat Screen</b>",
                      "<pre>   <b>System Section</b> - Manage remote Pi-Sat target, configure COSMOS cmd-tlm server, and enable tlm</pre>",
                      "<pre>   <b>Docs & Videos</b>  - Access <i>Getting Started Guide</i> and other docs and videos about using the Pi-Sat target</pre>",
                      "<pre>   <b>Pi-Sat Screens</b> - Launch screens for specific Pi-Sat apps</pre>",
                      "      ",
                      "<b>This Drop Down Menu</b>",
                      "<pre>   <b>Coming Soon...</b></pre>"]
         cfs_kit_create_about_screen("Pi-Sat",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
      end
   ## Pi-Sat Screens
   
   when "GPIO_DEMO_SCR"
      display("GPIO_DEMO GPIO_DEMO_SCREEN",50,50)
   
   else
      raise "Error in screen definition file. Undefined command #{cmd} sent to cfs_kit_scr_pisat()"
   end # cmd case

end # cfs_kit_scr_pisat() 


################################################################################
## R&D - sandbox
################################################################################

def cfs_kit_scr_sandbox(screen, cmd)

   case cmd
   
   ## System
   
   when "START_CFS", "START_CFS_42"
   
      Osk::System.stop_n_start_cfs('sandbox')
      
      if (cmd == "START_CFS_42")
         wait 3                      # Give cFS chance to start
         Osk::System.start_42(true)  # true causes 42 simulator screen to be displayed
      end

   ## Docs & Videos

   when "QUICK_START"
      doc_filename = Osk::cfg_target_dir_file("SANDBOX", "docs", Sandbox::RND_QS_FILE)
      Osk::System.display_pdf(doc_filename) unless doc_filename.nil?
   when "DOCS_VIDEOS"
      user_selection = screen.get_named_widget("rnd_docs_videos").text
      case user_selection
      when "About"
         about_str = ["<b>R&D Screen</b>",
                      "<pre>   <b>System Section</b> - Manage sandbox target and access OSK docs and videos</pre>",
                      "<pre>   <b>Docs & Videos</b>  - Launch docs and videos about R&D apps</pre>",
                      "<pre>   <b>Manage Apps</b>    - Add/remove pre-compiled apps and launch app mgmt screens</pre>",
                      "<pre>   <b>Prototype Apps</b> - Launch prototype app screens. Not all apps have screens</pre>",
                      "<pre>   <b>Bridges</b>        - Placeholder for future work</pre>",
                      "      ",
                      "<b>This Drop Down Menu</b>",
                      "<pre>   <b>SCSIM Video</b>    - Shows SCSIM app being used to simulate a ground pass</pre>"]
         cfs_kit_create_about_screen("Research & Development",about_str)
         display("CFS_KIT #{File.basename(Osk::ABOUT_SCR_FILE,'.txt')}",50,50)
         doc_filename = nil
      when "SCSIM Video"
         Osk::education_video(SCSIM_YOUTUBE_OVERVIEW)
      end

   ## Manage Apps
   
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
         Osk::System.start_cfs('sandbox')
         wait 4  # Give the cFS time to start
      end
      fsw_app_list = cfs_kit_create_fsw_app_list
      total_app_list = Osk::flight.app.keys - fsw_app_list
      # v3.0 - Used this feature for the sandbox target which is all non-simsat targets. This 
      #        needs to be cleaned up in a future release
      app_list = []
      total_app_list.each do |app|
         if (Osk::flight.app[app.to_s].cfs_targets.include?("sandbox"))
            app_list << app.to_s
         end
      end
      #~puts "fsw_apps: #{fsw_apps}\n"
      #~puts "Flight.app: #{Osk::flight.app.keys}\n"
      #~puts "Available Apps: #{app_list}\n"
      cfs_kit_add_app_screen("Add App from Compiled Prototypes", app_list)
      display("CFS_KIT #{File.basename(Osk::ADD_APP_SCR_FILE,'.txt')}",1000,10)
   
   when "ADD_APP_LOAD"
      app_name = screen.get_named_widget("app_name").text
      app = Osk::flight.app[app_name]         
      Osk::flight.cfe_es.send_cmd("START_APP with APP_NAME #{app.fsw_name}, APP_ENTRY_POINT #{app.entry_symbol}, APP_FILENAME #{app.obj_path_filename}, STACK_SIZE #{app.stack_size}, EXCEPTION_ACTION 0, PRIORITY #{app.priority}")
      prompt("The cFS terminal window should display a #{app_name} initialization message.\nUse the cFS Command and Telmetry Server's window to access the app's Cmd & Tlm Packets")
      clear("CFS_KIT #{File.basename(Osk::ADD_APP_SCR_FILE,'.txt')}")

   when "ADD_REMOVE_APP_DESCR"
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
   
   when "REMOVE_APP"
      if (Osk::System.cfs_running?)
         fsw_app_list = cfs_kit_create_fsw_app_list
         app_list = fsw_app_list - ["CFE_ES", "CFE_EVS", "CFE_SB", "CFE_TBL", "CFE_TIME", "KIT_CI", "KIT_SCH", "KIT_TO"]
         cfs_kit_remove_app_screen("Remove App", app_list)
         display("CFS_KIT #{File.basename(Osk::REMOVE_APP_SCR_FILE,'.txt')}",1000,10)
      else
         prompt("There's no cFS target running to remove apps from")
      end
   
   when "REMOVE_APP_UNLOAD"
      app_name = screen.get_named_widget("app_name").text
      Osk::flight.cfe_es.send_cmd("STOP_APP with APP_NAME #{app_name}")

   when "ES_APP_MGMT"
      display("CFS_KIT APP_MGMT_SCREEN",50,50)
   
   when "CONFIG_SCH_TO"
      display("CFS_KIT MNG_APP_RUNTIME_SCREEN",50,50)
   
   
   ## Prototype Apps
   
   when "RUN_BENCHMARKS"
      prompt("This is a prototype app that will be updated for benchmarking\nremote targets. The BM app will be loaded.")
      display("CFS_KIT BENCHMARK_SCREEN",50,50)
      Osk::Ops.load_app("BM") unless Osk::Ops.app_loaded?("BM")

   when "OSK_DEMO_APP"
      display("OSK_C_DEMO DEMO_OPS_SCREEN",50,50)

  
   ## Bridge Apps
   
   when "SBN"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Work with Software Bus Network app")   

   when "ROS2"
      prompt(Osk::MSG_TBD_FEATURE + "\n" + "Work with the Robotic Operating System")   

   else
      raise "Error in screen definition file. Undefined command sent to cfs_kit_scr_sandbox()"
   end # cmd case

end # cfs_kit_scr_sandbox()


################################################################################
## Create FSW app list
################################################################################

def cfs_kit_create_fsw_app_list

   Osk::flight.cfe_es.send_cmd("WRITE_APP_INFO_TO_FILE with FILENAME #{Osk::TMP_FLT_BIN_PATH_FILE}")
   wait 1
   #~if (Osk::system.file_transfer.get(Osk::TMP_FLT_BIN_PATH_FILE,Osk::TMP_GND_BIN_PATH_FILE,10)) # 10 sec timeout
   # Directly access the FSW file without transferring it to make this more responsive
   tbl_mgr_core = Cosmos::TableManagerCore.new                 
   app_info_bin_path_file = File.join(Osk::GND_TO_SANDBOX_SRV_DIR,Osk::TMP_BIN_FILE)
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
   #~end # End if downlinked App Info file

   return fsw_apps
   
end # cfs_kit_create_fsw_app_list

################################################################################
## Leftovers from OSK 1.7 that need to be migrated
################################################################################

def cfs_kit_launch_app(screen, app)


   if (app == "UPDATE_TUTORIAL")
      # An exception will report any errors   
      if cfs_kit_create_tutorial_screen
         prompt ("Successfuly created tutorial screen file #{tutorial_scr_file}\nusing #{tutorial_def_file}")
      end
   elsif (app == "PROTO_APP")
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

   tutorial_scr_file = File.join(Osk::CFS_KIT_SCR_DIR,Osk::TUTORIAL_SCR_FILE)

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
   
   tutorial_def_file = File.join(Osk::TUTORIAL_DIR,Osk::TUTORIAL_DEF_FILE)
   tutorial_scr_file = File.join(Osk::CFS_KIT_SCR_DIR,Osk::TUTORIAL_SCR_FILE)

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
   <% 
      requie 'tutorial_screen'
   %>
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
               BUTTON '#{tutorial["button"]}' 'tutorial = combo_box(\"#{tutorial["user-prompt"]}\",#{lesson_str}); launch_tutorial(self, \"#{tutorial["directory"]}\", \"#{tutorial["format"]}\", tutorial)'
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
   <% require 'cfs_kit_screen' %>
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
     BUTTON 'Description' 'cfs_kit_scr_sandbox(self,\"ADD_REMOVE_APP_DESCR\")'
     BUTTON 'Load App'    'cfs_kit_scr_sandbox(self,\"ADD_APP_LOAD\")'
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
## Create Remove App Screen
################################################################################

def cfs_kit_remove_app_screen(scr_title, app_list)

   t = Time.new 
   time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"

   scr_header = "
   ###############################################################################
   # Remove App Screen
   #
   # Notes:
   #   1. Do not edit this file because it is automatically generated and your
   #      changes will not be saved.
   #   2. File created by cfs_kit_remove_app_screen.rb on #{time_stamp}
   #
   # License:
   #   Written by David McComas, licensed under the copyleft GNU General Public
   #   License (GPL). 
   #
   ###############################################################################
   <% require 'cfs_kit_screen' %>
   SCREEN AUTO AUTO 0.5
   GLOBAL_SETTING BUTTON BACKCOLOR 221 221 221
  
   TITLE \"#{scr_title}\"
     SETTING BACKCOLOR 162 181 205
     SETTING TEXTCOLOR black

   VERTICAL
   LABEL \"              \"
   LABEL \"              \"

   LABEL \"Drop down menu contains apps currently running in cfS target.\"
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
   
   scr_file = File.join(Osk::CFS_KIT_SCR_DIR,Osk::REMOVE_APP_SCR_FILE)

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
     BUTTON 'Description' 'cfs_kit_scr_sandbox(self,\"ADD_REMOVE_APP_DESCR\")'
     BUTTON 'Stop App'    'cfs_kit_scr_sandbox(self,\"REMOVE_APP_UNLOAD\")'
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

end # cfs_kit_remove_app_screen()


################################################################################
## Version Identification Screen
################################################################################

def cfs_kit_create_version_screen

   t = Time.new 
   time_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"

   scr_header = "
   ###############################################################################
   # Version Identification Screen
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

