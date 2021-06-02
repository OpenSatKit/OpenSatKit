###############################################################################
# SimpleSat Ops Example Utilities
#
# These utilities help configure the COSMOS/cFS ops example runtime 
# environment. They are contained in a single file to separate them from other
# scripts that are more representative of 'real' test and operational scripts.
# For example instrument management and cFS app configuration scripts are not
# included in this file because a mission oudl typically have scripts that 
# perform those functions so they are in their own files so they can serve as
# examples.
#
# Notes:
#   1. This script is part of a training module. It is for illustrative
#      purposes and not intended to be an actual ops script. 
#   2. The background thread requests status telemetry packets to update the
#      example osp status regardless of whether or a ground contact is active. 
#   3. Threading doesn't seem to be supported by ScripRunner so a new 
#      ReqTlmThread must be started from the TlmViewer script.  
#
# Global Script Variables:
#   None - Global variables should not be used since the some are called by
#          screen scripts (i.e. TlmViewer) and others by ScriptRunner and
#          they have different runtime environments.
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'osk_ops'

require 'simsat_const'
require 'simsat_isim_mgmt'
require 'simsat_req_tlm'

require 'thread'

#
# Create a local screen so variables can be displayed on the screen. Generating
# the local screen creates warnings that can be ignored
#
def simsat_create_ops_screen
   
   ops_screen_def = get_screen_definition("SIMSAT OPS_SCREEN")
   screen = local_screen("SIMSAT OPS_SCREEN", ops_screen_def.gsub("#local#","") , 100, 100)

end # simsat_create_ops_screen()

#
# This is called from a screen to put FSW in a known state and to allow
# a background thread to be created. 
#
def simsat_ops_example_setup(start_cfs)
   
   if start_cfs
   
      # Simplest way to get into a known configuration is to restart the flight software   
      message_box("The flight software will be restarted to establish a known configuration. A terminal window will be created to run the FSW. Enter your user password when prompted.",false)
   
      Osk::System.stop_n_start_cfs('simsat')
   
   end # End if start_cfs
   
   #
   # Delete onboard recorder files used in the example. Do not blindly
   # clear all files to allow the simsat directory to be used for other
   # files if needed.
   #
   # Since this is not an operational script example it "cheats" by directly
   # deleting files in the FSW recorder
   #
   simsat_files = File.join(SimSat::GND2FSW_REC_DIR,"*")

   begin
            
      event_file_base = File.join(SimSat::GND2FSW_REC_DIR,SimSat::EVENT_FILENAME_BASE)
      Dir.glob(simsat_files).select{ |file| /^#{event_file_base}/.match file}.each { |file| File.delete(file)}
      
      sci_aux_file_base = File.join(SimSat::GND2FSW_REC_DIR,SimSat::SCI_AUX_FILENAME_BASE)
      Dir.glob(simsat_files).select{ |file| /^#{sci_aux_file_base}/.match file }.each { |file| File.delete(file)}
      
      isim_file_base = File.join(SimSat::GND2FSW_REC_DIR,SimSat::ISIM_FILENAME_BASE)
      Dir.glob(simsat_files).select{ |file| /^#{isim_file_base}/.match file }.each { |file| File.delete(file)}
      
      File.delete(SimSat::GND2FSW_STOP_OPS_FILE) unless !File.exist?(SimSat::GND2FSW_STOP_OPS_FILE)
      
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

   # Background thread works within TlmViewer scripting context and not
   # ScriptRunner so it will keep executing.
   $request_tlm = SimSatReqTlm.new
   
   Osk::flight.cfe_time.send_cmd("SET_CLOCK_MET with SECONDS 0, MICROSECONDS 0")
   Osk::flight.cfe_time.send_cmd("SET_CLOCK with SECONDS 0, MICROSECONDS 0") 

end # simsat_ops_example_setup()


#
# This is called from the ScriptRunner script 
#
def simsat_ops_example_teardown
   
   
   # Create an ops_example stop file to signal the background
   # thread that the example is over
   
   t = Time.new 
   time_stamp = "#{t.year}_#{t.month}_#{t.day}_#{t.hour}#{t.min}#{t.sec}"

   begin
         
      # Always overwrite the temp file      
      File.open(SimSat::GND2FSW_STOP_OPS_FILE,"w") do |f| 
           
         f.write ("Ops example terminated at #{time_stamp}")

      end # File
         
   rescue Exception => e
      puts e.message
      puts e.backtrace.inspect  
   end

   # Configure apps to less active state & to stop generating files
   
   Osk::flight.send_cmd("DS","SET_APP_STATE with APP_STATE 0") 
   wait("DS HK_TLM_PKT APP_ENA_STATE == 'DIS'", 10)
   wait 1

   Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX 0, FILE_STATE 0")  # Disable event file
   wait 1
   Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX 6, FILE_STATE 0")  # Disable science auxiliary file
   wait 1
   Osk::flight.send_cmd("DS","SEND_FILE_INFO")

   Osk::flight.send_cmd("SC","DISABLE_RTS with RTS_ID 6") # Disable ISIM power off RTS
   wait("SC HK_TLM_PKT RTS_6_DIS == 'TRUE'", 10)

   Osk::flight.send_cmd("LC","SET_APP_STATE with NEW_STATE 3") # Disable Limit Checker
   wait("LC HK_TLM_PKT APP_STATE == 'DISABLED'", 10)

   Osk::flight.send_cmd("LC","SET_AP_STATE with AP_ID 2, NEW_STATE 3") # Disable ISIM Fault Action Point
   wait("LC HK_TLM_PKT AP_2_STATE == 'DISABLED'", 10)

   simsat_isim_pwr_off
  
   # Restore default OSK scheduler table
   Osk::flight.send_cmd("KIT_SCH","LOAD_TBL with ID #{FswConfigParam::KIT_SCH_SCHTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{$SIMSAT_SCH_TBL_FLT_FILENAME}")  

end # simsat_ops_example_teardown()
