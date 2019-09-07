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
      Osk::System.stop_cfs()
      Osk::System.start_cfs()  # Enables telemetry
      
   when "START_CFS_42"
   
      prompt("The 42 simulator takes about 20 seconds to initialize.")
      
      # Kill all instances of the cFS before starting a new instance. 

      Osk::System.stop_cfs()
      Osk::System.start_cfs()  # Enables telemetry

      spawn("xfce4-terminal --default-working-directory=""#{Cosmos::USERPATH}/#{Osk::REL_DIR_42}"" --execute ./42 OSK""")
      wait(15)
      Osk::flight.send_cmd("I42","CONNECT_42")
     
   when "STOP_CFS"
      # Hopefully ES cleans up resources and does a controlled shutdown
      Osk::flight.cfe_es.send_cmd("RESET with RESTART_TYPE 1")
      #Osk::System.stop_cfs()
   
   when "STOP_42"
      Osk::flight.send_cmd("I42","DISCONNECT_42")
   
   when "CONFIG_SYS"
      case screen.get_named_widget("sys_cmd").text
      when "ENA_TLM"
         # Don't use Osk::flight because cmd validation would fail
         cmd("KIT_TO ENABLE_TELEMETRY")
      when "RESET_TIME"
         Osk::flight.cfe_time.send_cmd("SET_CLOCK_MET with SECONDS 0, MICROSECONDS 0")
         Osk::flight.cfe_time.send_cmd("SET_CLOCK with SECONDS 0, MICROSECONDS 0") 
      when "SET_GND_CMD_MODE"
         enable = combo_box("Cmd validation verifies command counters in tlm after each\n cmd is sent. The system will run slower. \n\nDo you want to enable validation?", 'Yes','No')  
         if (enable == "Yes")
            FswApp.validate_cmd(true)
         elsif (enable == "No")
            FswApp.validate_cmd(false)
         end
      end
      
   when "OSK_QUICK_START"
      spawn("evince #{Osk::OSK_DOCS_DIR}/#{Osk::DOCS_TOUR_FILE}")

   when "OSK_USERS_GUIDE"
      spawn("evince #{Osk::OSK_DOCS_DIR}/#{Osk::DOCS_UG_FILE}")

   when "RUN_OPS_EXAMPLE"
      spawn("ruby #{Osk::COSMOS_SCR_RUNNER} simsat_ops_example.rb")
      display("SIMSAT SIMSAT_OPS_SCREEN",50,50)
      spawn("evince #{Osk::target_dir_file("SIMSAT","docs",Osk::OSK_TRAINING_SIMSAT)}")
   
   when "RUN_INTG_TEST"
      spawn("ruby #{Osk::COSMOS_SCR_RUNNER} simsat_intg_test.rb")
      display("SIMSAT SIMSAT_CFS_APP_SCREEN",50,50)
      display("SIMSAT SIMSAT_OSK_APP_SCREEN",1500,50)

   when "SIMSAT_RUNTIME"
      display("SIMSAT SIMSAT_RUNTIME_SCREEN",50,50)
   
   when "SIMSAT_DATA_FILE"
      display("SIMSAT SIMSAT_DATA_FILE_SCREEN",50,50)
   
   when "SIMSAT_AUTONOMY"
      display("SIMSAT SIMSAT_AUTONOMY_SCREEN",50,50)
   
   when "SIMSAT_ADC"
      display("SIMSAT SIMSAT_ADC_SCREEN",50,50)
   
   when "SIMSAT_MAINTENANCE"
      display("SIMSAT SIMSAT_MAINTENANCE_SCREEN",50,50)
   
   when "SIMSAT_HEALTH_SAFETY"
      display("SIMSAT SIMSAT_HEALTH_SAFETY_SCREEN",50,50)
   
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


