################################################################################
# cFS Kit Main Screen Scripts
#
# Notes:
#   1. Global kit todo list, not just main screen
#      TODO - Use similar layout for app, file, tbl mgmt screens 
#      TODO - Display ground directory listing file after transfer to ground?
#      TODO - Add drop down menus where it makes sense
#      TODO - Is there a way to have a script run when a screen is first displayed?
#             Update working directories if user changes them
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
require 'osk_system'

################################################################################
## Global Variables
################################################################################

# Used by file put/get command prototypes
FLT_TEST_GET_FILE = "#{FLT_SRV_DIR}/tf_get_test_src.txt"
FLT_TEST_PUT_FILE = "#{FLT_SRV_DIR}/tf_put_test_dst.txt"
GND_TEST_GET_FILE = "#{GND_SRV_DIR}/tf_get_test_dst.txt"
GND_TEST_PUT_FILE = "#{GND_SRV_DIR}/tf_put_test_src.txt"


################################################################################
## Launch Applications
################################################################################

def cfs_kit_launch_app(screen, app)

  if (app == "CFS")
    spawn("xfce4-terminal --default-working-directory=""#{Cosmos::USERPATH}/../cfs/build/exe/cpu1"" --execute sudo ./core-cpu1""")
    wait(3)
    #cmd("KIT_TO ENABLE_TELEMETRY")
  elsif (app == "CFE_SERVICES")
    display("CFS_KIT CFE_SCREEN",50,50)   
  elsif (app == "BENCHMARKS")
    prompt("Note this is a prototype application")
    display("CFS_KIT BENCHMARK_SCREEN",50,50) 
  elsif (app == "PERF_MON")
    display("CFS_KIT PERF_MON_SCREEN",50,50)
  elsif (app == "RUN_TEST_SCRIPT")
    spawn("ruby #{Cosmos::USERPATH}/tools/ScriptRunner #{Cosmos::USERPATH}/procedures/kit_test/kit_test_main.rb")
    display("CFS_KIT APP_CFS_SUMMARY_SCREEN",50,50)
    display("CFS_KIT APP_KIT_SUMMARY_SCREEN",50,50)
  elsif (app == "MANAGE_FILES")
    display("CFS_KIT FILE_MGMT_SCREEN",50,50)
  elsif (app == "MANAGE_TABLES")
    display("CFS_KIT TABLE_MGMT_SCREEN",50,50)
  elsif (app == "MANAGE_MEMORY")
    display("CFS_KIT MEMORY_MGMT_SCREEN",50,50)
  elsif (app == "MANAGE_APPS")
    display("CFS_KIT APP_MGMT_SCREEN",50,50)
  elsif (app == "APP_SUMMARY")
    display("CFS_KIT APP_CFS_SUMMARY_SCREEN",50,50)
    display("CFS_KIT APP_KIT_SUMMARY_SCREEN",50,50)
  elsif (app == "MANAGE_RECORDER")
    display("CFS_KIT RECORDER_MGMT_SCREEN",50,50)
  elsif (app == "MANAGE_AUTONOMY")
    display("CFS_KIT AUTONOMY_MGMT_SCREEN",50,50)
  elsif (app == "SIM_42")
    display("CFS_KIT SIM_42_SCREEN",50,50)
  elsif (app == "PISAT")
	#prompt("Please ensure you are connected to the PiSat network")
    #cmd("PICONTROL STARTCFS")
    #wait(2)
    #cmd("KIT_TO ENABLE_TELEMETRY")
    spawn("ruby #{Cosmos::USERPATH}/tools/TlmGrapher")
    display("CFS_KIT PISAT_CONTROL_SCREEN", 1000, 0)
  elsif (app == "CREATE_APP")
    #prompt("Launching CreateApp...")
    #spawn("java -jar /mnt/hgfs/OpenSatKit/cosmos/cfs_kit/tools/create-app/CreateApp.jar")
    spawn("java -jar #{CFS_KIT_CREATE_APP_DIR}/CreateApp.jar", :chdir => "#{CFS_KIT_CREATE_APP_DIR}")
  elsif (app == "UPDATE_TUTORIAL")
    cfs_kit_create_tutorial_screen
  elsif (app == "CFE_USERS_GUIDE")
    Cosmos.open_in_web_browser("#{CFS_KIT_CFE_UG_DIR}/#{OSK_CFE_UG_FILE}")
  elsif (app == "TODO")
    prompt("Feature coming soon...")
  else
    prompt("Error in screen definition file. Undefined command sent to cfs_kit_launch_app()")
  end

end # cfs_kit_launch_app()

################################################################################
## Launch Demo
################################################################################

def cfs_kit_launch_demo(screen, demo)

  if (demo == "FILE_MGMT_DEMO")
    display("CFS_KIT FILE_MGMT_DEMO_SCREEN",50,50)
  elsif (demo == "TABLE_DEMO")
    display("CFS_KIT TABLE_MGMT_DEMO_SCREEN",50,50) 
  elsif (demo == "MEMORY_DEMO")
    display("CFS_KIT MEMORY_MGMT_DEMO_SCREEN",50,50)
  elsif (demo == "RECORDER_DEMO")
    display("CFS_KIT RECORDER_MGMT_DEMO_SCREEN",50,50)
  elsif (demo == "AUTONOMY_DEMO")
    display("CFS_KIT HEATER_CTRL_DEMO_SCREEN",50,50)
  elsif (demo == "APP_DEMO")
    display("CFS_KIT APP_MGMT_DEMO_SCREEN",50,50)
  elsif (demo == "PERF_MON_DEMO")
    display("CFS_KIT PERF_MON_DEMO_SCREEN",50,50)
  elsif (demo == "TUTORIAL")
    display("CFS_KIT TUTORIAL_SCREEN",50,50)
  else
    prompt("Error in screen definition file. Undefined command sent to cfs_kit_launch_demo()")
    #Save prompt(MSG_TBD_FEATURE)  
  end

end # cfs_kit_launch_demo()

################################################################################
## Send Commands
################################################################################

def cfs_kit_send_cmd(screen, cmd)

  if (cmd == "TO_ENA_TELEMETRY")
    cmd("KIT_TO ENABLE_TELEMETRY")
  elsif (cmd == "TIME_SET_CLOCK_ZERO")
    cmd("CFE_TIME SET_CLOCK_MET with SECONDS 0, MICROSECONDS 0"); 
    cmd("CFE_TIME SET_CLOCK with SECONDS 0, MICROSECONDS 0")
  elsif (cmd == "PROTO_NEW_CMD")
	  # TODO - Some potential new commands
    # (cmd == "DISPLAY_SB_ROUTES")
    # (cmd == "DISPLAY_MSG_IDS")
    # (cmd == "CFS_HTML_DOC")
    value = combo_box("Select the command being developed", 'Put File', 'Get File', 'Display SB Routes')
    case value
    when 'Put File'
      file_xfer = Osk::system.file_transfer
      file_xfer.put(GND_TEST_PUT_FILE, FLT_TEST_PUT_FILE)
    when 'Get File'
      file_xfer = Osk::system.file_transfer
      file_xfer.get(FLT_TEST_PUT_FILE,GND_TEST_PUT_FILE)
    when 'Display SB Routes'
      cmd("CFE_SB WRITE_ROUTING_TO_FILE with FILENAME '/cf/~sb_tmp.dat'")
    end
  else
    prompt("Error in screen definition file. Undefined command sent to cfs_kit_send_cmd()")
  end
end # cfs_kit_send_cmd()

################################################################################
## Create Tutorial Screen
################################################################################

TUTORIAL_SCR_HEADER = "
   ###############################################################################
   # cfs_kit Tutorial Screen
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

TUTORIAL_SCR_TRAILER = "
   END # Vertical Box
   "

def cfs_kit_create_tutorial_screen


   tutorial_def_file = "#{CFS_KIT_TUTORIAL_DIR}/#{TUTORIAL_DEF_FILE}"
   tutorial_scr_file = "#{CFS_KIT_SCR_DIR}/#{TUTORIAL_SCR_FILE}"
   
   puts "#{tutorial_scr_file}"
   
   t = Time.new 
   file_stamp = "_#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"
   
   begin
      
      json_file = File.read(tutorial_def_file)
      json_hash = JSON.parse(json_file)
    
      #puts json_hash
      #puts json_hash["tutorials"]
      #puts json_hash["tutorials"][0]["name"]
      
      if File.exists? tutorial_scr_file
         filename = File.basename(tutorial_scr_file, File.extname(tutorial_scr_file))
         new_filename =  "#{CFS_KIT_SCR_DIR}/#{filename}#{file_stamp}"+File.extname(tutorial_scr_file)
         File.rename(tutorial_scr_file, new_filename)
      end
      
      File.open("#{tutorial_scr_file}","w") do |f| 
	     
         f.write ("#{TUTORIAL_SCR_HEADER}")
         
         json_hash["tutorials"].each do |tutorial|
            
            lesson_str = "#{tutorial["lessons"]}"
            lesson_str = lesson_str[1,(lesson_str.length-2)]  # Remove brackets [] and keep quotes around elements
            
            tutorial_menu = "
               HORIZONTAL 10
               BUTTON '#{tutorial["button"]}' 'require \"#{Cosmos::USERPATH}/config/targets/CFS_KIT/lib/tutorial_screen.rb\"; tutorial = combo_box(\"#{tutorial["user-prompt"]}\",#{lesson_str}); launch_tutorial(self, \"#{tutorial["directory"]}\", tutorial)'
               SPACER 20 0 MAXIMUM FIXED          
               LABEL \"#{tutorial["description"]}\"
               SPACER 50 0 MINIMUMEXPANDING FIXED          
               END # Horizontal"
            
            f.write (tutorial_menu)
         
         end # Tutorial
         
         f.write ("#{TUTORIAL_SCR_TRAILER}")

      end # File
      
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

end # cfs_kit_create_tutorial_screen()


#
# Useful code snippets
#
#sel = message_box("Press #{BUTTON_CONT} to continue",BUTTON_CONT)
#if (sel != BUTTON_CONT) then return end  
#
#Cosmos.run_process("ruby tools/CmdSender -p \"CFE_ES START_APP\"")
