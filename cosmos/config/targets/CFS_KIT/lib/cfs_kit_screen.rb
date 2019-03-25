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
## Launch Applications
################################################################################

def cfs_kit_launch_app(screen, app)

   if (app == "CFS")
   
      # Kill all instances of the cFS before starting a new instance. 
      
      Osk::System.stop_cfs()

      # Start the cFS and enable telemetry
      
      Osk::System.start_cfs()
      
      return
      
      #    
      # When a new terminal window open user will be prompted for password 
      # since cFE is run in privileged mode. The loop after the spawn 
      # automatically enables telemetry 
      #  
      
      
      core = `pgrep core`
      if (core.length > 1)
         #puts core + " len = #{core.length}"
         `echo osk | sudo -S kill #{core}`  # Echo supplies the password
      end
      spawn("xfce4-terminal --default-working-directory=""#{Osk::CFS_EXE_DIR}"" --execute sudo ./core-cpu1""")
      #spawn("xfce4-terminal --default-working-directory=""#{Cosmos::USERPATH}/../cfs/build/exe/cpu1"" --execute sudo ./core-cpu1""")
        
      #~Osk::system.connect_to_local_cfs  # Sometimes previous session left in a bad state

      done = false
      attempts = 0
      seq_cnt = tlm("CFE_ES #{Osk::TLM_STR_HK_PKT} #{Ccsds::PRI_HDR_SEQUENCE}")
      while (!done)
         core = `pgrep core`
         if (core.length > 1)
            wait(2)
            cmd("KIT_TO ENABLE_TELEMETRY")
            done = true
            #puts core + " len = #{core.length}"
         else
            attempts += 1
            if (attempts < 5)
               wait (4)
            else
               done = true
            end
         end
         #~puts core + " len = #{core.length}"
         #~if ( tlm("CFE_ES #{Osk::TLM_STR_HK_PKT} #{Ccsds::PRI_HDR_SEQUENCE}") != seq_cnt)
         #~   done = true
         #~else
         #~   begin
         #~      cmd("KIT_TO ENABLE_TELEMETRY")
         #~   rescue
         #~      Osk::system.connect_to_local_cfs
         #~   end
         #~   attempts += 1
         #~   if (attempts < 5)
         #~      wait (4)
         #~   else
         #~      done = true
         #~   end
         #~end   
      end # while ! done
      #~puts "attempt #{attempts}"
      
   elsif (app == "CFE_SERVICES")
      display("CFS_KIT CFE_SCREEN",50,50)   
   elsif (app == "MANAGE_USER_APP")
      display("CFS_KIT USER_APP_INTG_SCREEN",50,50)   
   elsif (app == "SET_CMD_VALIDATE")
      enable = combo_box("Cmd validation verifies command counters in tlm after each\n cmd is sent. The system will run slower. \n\nDo you want to enable validation?", 'Yes','No')  
      if (enable == "Yes")
         FswApp.validate_cmd(true)
      elsif (enable == "No")
         FswApp.validate_cmd(false)
      end
   elsif (app == "BENCHMARKS")
      prompt("Note this is a prototype application")
      display("CFS_KIT BENCHMARK_SCREEN",50,50) 
   elsif (app == "PERF_MON")
      display("CFS_KIT PERF_MON_SCREEN",50,50)
   elsif (app == "RUN_TEST_SCRIPT")
      spawn("ruby #{Osk::COSMOS_SCR_RUNNER} #{Cosmos::USERPATH}/procedures/kit_test/kit_test_main.rb")
      display("CFS_KIT APP_CFS_SUMMARY_SCREEN",50,50)
      display("CFS_KIT APP_KIT_SUMMARY_SCREEN",1500,50)
   elsif (app == "MANAGE_FILES")
      display("CFS_KIT FILE_MGMT_SCREEN",50,50)
   elsif (app == "MANAGE_TABLES")
      display("CFS_KIT TABLE_MGMT_SCREEN",50,50)
   elsif (app == "MANAGE_MEMORY")
      display("CFS_KIT MEMORY_MGMT_SCREEN",50,50)
   elsif (app == "MANAGE_APPS")
      display("CFS_KIT APP_MGMT_SCREEN",50,50)
   elsif (app == "APP_SUMMARY")
      display("CFS_KIT APP_CFS_SUMMARY_SCREEN",10,10)
      display("CFS_KIT APP_KIT_SUMMARY_SCREEN",1500,10)
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
      spawn("ruby #{Osk::COSMOS_TLM_GRAPHER}")
      display("CFS_KIT PISAT_CONTROL_SCREEN", 1000, 0)
   elsif (app == "MANAGE_APP_DEV")
      #spawn("java -jar #{Osk::CREATE_APP_DIR}/CreateApp.jar", :chdir => "#{Osk::CREATE_APP_DIR}")
      display("CFS_KIT MNG_APP_DEV_SCREEN",50,50)
   elsif (app == "UPDATE_TUTORIAL")
      cfs_kit_create_tutorial_screen
   elsif (app == "CFE_USERS_GUIDE")
      Cosmos.open_in_web_browser("#{Osk::CFE_UG_DIR}/#{Osk::CFE_UG_FILE}")
   elsif (app == "OSK_TOUR")
      spawn("evince #{Osk::OSK_DOCS_DIR}/#{Osk::DOCS_TOUR_FILE}")
   elsif (app == "OSK_UG")
      spawn("evince #{Osk::OSK_DOCS_DIR}/#{Osk::DOCS_UG_FILE}")
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
   elsif (app == "TODO")
      prompt("Feature coming soon...")
   else
      raise "Error in screen definition file. Undefined command sent to cfs_kit_launch_app()"
   end

end # cfs_kit_launch_app()

################################################################################
## Launch Demo
################################################################################

def cfs_kit_launch_demo(screen, demo)

   if (demo == "FILE_MGMT_DEMO")
      display("CFS_KIT FILE_MGMT_DEMO_SCREEN",500,50)
   elsif (demo == "TABLE_DEMO")
      display("CFS_KIT TABLE_MGMT_DEMO_SCREEN",500,50) 
   elsif (demo == "MEMORY_DEMO")
      display("CFS_KIT MEMORY_MGMT_DEMO_SCREEN",500,50)
   elsif (demo == "RECORDER_DEMO")
      display("CFS_KIT RECORDER_MGMT_DEMO_SCREEN",500,50)
   elsif (demo == "AUTONOMY_DEMO")
      display("CFS_KIT HEATER_CTRL_DEMO_SCREEN",500,50)
   elsif (demo == "APP_DEMO")
      display("CFS_KIT APP_MGMT_DEMO_SCREEN",500,50)
   elsif (demo == "PERF_MON_DEMO")
      display("CFS_KIT PERF_MON_DEMO_SCREEN",500,50)
   elsif (demo == "TUTORIAL")
      display("CFS_KIT #{File.basename(Osk::TUTORIAL_SCR_FILE,'.txt')}",500,50)
   else
      raise "Error in screen definition file. Undefined command sent to cfs_kit_launch_demo()"
      #Save prompt(MSG_TBD_FEATURE)  
   end

end # cfs_kit_launch_demo()

################################################################################
## Send Commands
################################################################################

def cfs_kit_send_cmd(screen, cmd)

   if (cmd == "TO_ENA_TELEMETRY")
      # DOn't use Osk::flight because cmd validation would fail
      cmd("KIT_TO ENABLE_TELEMETRY")
   elsif (cmd == "TIME_SET_CLOCK_ZERO")
      Osk::flight.cfe_time.send_cmd("SET_CLOCK_MET with SECONDS 0, MICROSECONDS 0")
      Osk::flight.cfe_time.send_cmd("SET_CLOCK with SECONDS 0, MICROSECONDS 0") 
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
      raise "Error in screen definition file. Undefined command sent to cfs_kit_send_cmd()"
   end
end # cfs_kit_send_cmd()

################################################################################
## Create Tutorial Screen
################################################################################

TUTORIAL_SCR_HEADER = "
   ###############################################################################
   # cfs_kit Tutorial Screen
   #
   # Notes:
   #   1. Do not edit this file because it is automatically generated and your
   #      changes will not be saved.
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


   tutorial_def_file = "#{Osk::TUTORIAL_DIR}/#{Osk::TUTORIAL_DEF_FILE}"
   tutorial_scr_file = "#{Osk::SCR_DIR}/#{Osk::TUTORIAL_SCR_FILE}"
   
   #puts "Debug: #{tutorial_scr_file}"
   
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
         new_filename =  "#{Osk::SCR_DIR}/#{filename}#{file_stamp}"+File.extname(tutorial_scr_file)
         File.rename(tutorial_scr_file, new_filename)
      end
      
      File.open("#{tutorial_scr_file}","w") do |f| 
	     
         f.write ("#{TUTORIAL_SCR_HEADER}")
         
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
         
         f.write ("#{TUTORIAL_SCR_TRAILER}")

      end # File
      
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

end # cfs_kit_create_tutorial_screen()


