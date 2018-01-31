###############################################################################
# cFS Kit cFE Screen
#
# Notes:
#   1. TODO - Create generic command screen utility 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'cfs_kit_global'
require 'file_transfer'

################################################################################
## Send Commands
################################################################################

def cfe_send_cmd(screen, cmd)
 
  if (cmd == "ES_CMD")
    Cosmos.run_process("ruby tools/CmdSender -p \"CFE_ES NOOP\"")     
  elsif (cmd == "ES_PERF_MON")
    display("CFS_KIT PERF_MON_SCREEN",50,50)
  elsif (cmd == "ES_APP_MGMT")
    display("CFS_KIT APP_MGMT_SCREEN",50,50)   
  elsif (cmd == "EVS_CMD")
    Cosmos.run_process("ruby tools/CmdSender -p \"CFE_EVS NOOP\"")     
  elsif (cmd == "SB_CMD")
    Cosmos.run_process("ruby tools/CmdSender -p \"CFE_SB NOOP\"")     
  elsif (cmd == "TBL_CMD")
    Cosmos.run_process("ruby tools/CmdSender -p \"CFE_TBL NOOP\"")     
  elsif (cmd == "TBL_MGMT")
    display("CFS_KIT TABLE_MGMT_SCREEN",50,50)
  elsif (cmd == "TIME_CMD")
    Cosmos.run_process("ruby tools/CmdSender -p \"CFE_TIME NOOP\"")     
  else
    prompt("Error in screen definition file. Undefined command '#{cmd}' sent to cfe_send_cmd()")
  end
  
end # cfe_send_cmd()
