###############################################################################
# SimSat Demo Screen
#
# Notes:
#   1. This doesn't follow the pattern of one ruby script for each screen
#      convention since all of the SimSat screen are designed to be 
#      aggregated as a whole. 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_ops'
require 'fsw_const'

require 'simsat_const'
require 'simsat_ops_example_utils'
require 'simsat_isim_mgmt'
require 'simsat_recorder_mgmt'

require 'thread'


################################################################################
## GUI Send Commands
################################################################################

def simsat_demo_scr_cmd(screen, apps, cmd)
 
   case apps
   
   when "ADC"
      simsat_demo_adc(screen,cmd)
   when "AUTONOMY"
      simsat_demo_autonomy(screen,cmd)
   when "DATA_FILE"
      simsat_demo_data_file(screen,cmd)
   when "HEALTH_SAFETY"
      simsat_demo_health_safety(screen,cmd)
   when "MAINTENANCE"
      simsat_demo_maintenance(screen,cmd)
   when "RUNTIME"
      simsat_demo_runtime(screen,cmd)
   when "FUNC_TBL_MGMT"
      display("CFS_KIT TABLE_MGMT_SCREEN",1500,10)
   else
      raise "Error in screen definition file. Undefined app '#{apps}' sent to simsat_demo_src_cmd()"
   end
  
end # simsat_demo_scr_cmd()

   
#
# Attitude Determination & Control
#
def simsat_demo_adc(screen, cmd)
 
   case cmd
   when "TBD"
   else
      raise "Error in screen definition file. Undefined attitude determination and control screen command '#{cmd}' sent to simsat_demo_src_cmd()"
   end
     
end # simsat_demo_adc()


#
# Autonomy
#
def simsat_demo_autonomy(screen, cmd)
 
   case cmd
   when "TBD"
   else
      raise "Error in screen definition file. Undefined attitude determination and control screen command '#{cmd}' sent to simsat_demo_src_cmd()"
   end
     
end # simsat_demo_autonomy()


#
# Data-File Management
#
def simsat_demo_data_file(screen, cmd)
 
   case cmd
   when "START_FILE_GEN"
      
      return unless Osk::System.check_n_start_cfs('simsat')
      
      #
      # 1. Create simsat recorder directory, can't assumes it exists
      # 2. Configure ISIM to science mode
      # 3. Enable DS to create files
      # 4. Start background script to request FM and DS status packets
      #

      Thread.new {
      
         # 1.  Create simsat recorder directory
         Osk::Ops.create_flt_dir(SimSat::FLT_SRV_DIR)
         Osk::Ops.create_flt_dir(SimSat::FLT_REC_DIR)
         
         # 2. Configure ISIM to science mode
         simsat_isim_pwr_on

         # 3. Enable DS to create files
         Osk::flight.send_cmd("DS","SET_APP_STATE with APP_STATE 1") 
         wait("DS HK_TLM_PKT APP_ENA_STATE == 'ENA'", 10)
         Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX 0, FILE_STATE 1") # Enable Event file
         wait 1
         Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX 6, FILE_STATE 1") # Enable Science Auxiliary file

         # 4. Start background script to request FM and DS status packets
         $request_tlm = SimSatReqTlm.new
      
      } # End thread
      
   when "START_PLAYBACK"
   
      #
      # 1. Disable file generation
      # 2. Load user selected TO downlink rate table file 
      # 3. Start playback using user selected file transfer method
      #

      # 1. Disable file generation
      
      Osk::flight.send_cmd("DS","SET_APP_STATE with APP_STATE 0") 
      wait("DS HK_TLM_PKT APP_ENA_STATE == 'DIS'", 10)
      Osk::flight.send_cmd("ISIM","STOP_SCI_DATA")

      # 2. Load user selected TO downlink rate table file. Don't assume table is onboard.
      
      if screen.get_named_widget("to_rate").text == "4Kbps"
         gnd_file = SimSat::KIT_TO_4K_GND_FILE
         flt_file = SimSat::KIT_TO_4K_FLT_FILE
      else
         gnd_file = SimSat::KIT_TO_32K_GND_FILE
         flt_file = SimSat::KIT_TO_32K_FLT_FILE
      end
      
      if (Osk::Ops.put_flt_file(gnd_file,flt_file))
         Osk::flight.send_cmd("KIT_TO","LOAD_TBL with ID #{FswConfigParam::KIT_TO_PKTTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{flt_file}")  
      else
         message_box("Failed to load KIT_TO table. Verify flight software configuration and COSMOS connection",false)
         return
      end
      
      # 3. Start playback using user selected file transfer method
      
      if screen.get_named_widget("file_transfer").text == "TFTP"
         simsat_recorder_playback("TFTP")
      else
         simsat_recorder_playback("CFDP")         
      end
      
   else
      raise "Error in screen definition file. Undefined data/file management screen command '#{cmd}' sent to simsat_demo_src_cmd()"
   end
     
end # simsat_demo_data_file()


#
# Health & Safety
#
def simsat_demo_health_safety(screen, cmd)
 
   case cmd
   when "TBD"
   else
      raise "Error in screen definition file. Undefined attitude determination and control screen command '#{cmd}' sent to simsat_demo_src_cmd()"
   end
     
end # simsat_demo_health_safety()


#
# Maintenance
#
def simsat_demo_maintenance(screen, cmd)
 
   case cmd
   when "TBD"
   else
      raise "Error in screen definition file. Undefined attitude determination and control screen command '#{cmd}' sent to simsat_demo_src_cmd()"
   end
     
end # simsat_demo_maintenance()


#
# Runtime Environment
#
def simsat_demo_runtime(screen, cmd)
 
   case cmd
   when "TBD"
   else
      raise "Error in screen definition file. Undefined attitude determination and control screen command '#{cmd}' sent to simsat_demo_src_cmd()"
   end
     
end # simsat_demo_runtime()

