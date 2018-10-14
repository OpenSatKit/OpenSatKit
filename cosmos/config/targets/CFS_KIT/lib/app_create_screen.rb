###############################################################################
# Create Application Screen Scripts 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
###############################################################################

require 'cosmos'
require 'cosmos/script'
Cosmos.catch_fatal_exception do
  require 'cosmos/tools/cmd_sender/cmd_sender'
  require 'cosmos/tools/tlm_viewer/screen'
  require 'cosmos/tools/tlm_viewer/tlm_viewer'
end
require 'osk_global'
require 'osk_system'


################################################################################
## Screen Commands
################################################################################

def app_create(screen, cmd)

   if (cmd == "CREATE_APP")
      spawn("java -jar #{Osk::CREATE_APP_DIR}/CreateApp.jar", :chdir => "#{Osk::CREATE_APP_DIR}")
   elsif (cmd == "EDIT_CMAKE")
      Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::CFS_CMAKE_DIR}/#{Osk::CPU1_TARGET_FILE}'")
   elsif (cmd == "EDIT_STARTUP")
      Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::CFS_CMAKE_DIR}/#{Osk::CPU1_STARTUP_FILE}'")
   elsif (cmd == "BUILD_CFS")
      spawn("xfce4-terminal --default-working-directory=""#{Osk::OSK_CFS_DIR}"" --execute make install""")
   elsif (cmd == "STOP_CFS_SERVER")
   
      Osk::System.stop_cfs
      Osk::System.stop_cmd_tlm_server
      
   elsif (cmd == "START_CFS_SERVER")
   
      spawn("ruby #{Osk::COSMOS_CMD_TLM_SRV}")            
      wait 4
      Osk::System.start_cfs

   else
      prompt("Error in screen definition file. Undefined command sent to app_create()")
   end
   
end # app_create()


#
# Useful code snippets
#
#sel = message_box("Press #{BUTTON_CONT} to continue",BUTTON_CONT)
#if (sel != BUTTON_CONT) then return end  
#
#Cosmos.run_process("ruby tools/CmdSender -p \"CFE_ES START_APP\"")
