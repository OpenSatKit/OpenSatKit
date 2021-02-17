###############################################################################
# Table Tool Screen Scripts 
#
# Notes:
#   1. 
#
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'

require 'simsat_const'

################################################################################
## Screen Commands
################################################################################

def tbl_tool(screen, cmd)

   case cmd
   when "TLM"
      display("SIMSAT TLM_TBL_GEN_SCREEN",1500,50)
   when "HK"
      display("SIMSAT HK_TBL_GEN_SCREEN",1500,50)
   when "LC"
      prompt(Osk::MSG_TBD_FEATURE)
   when "SC"
      prompt(Osk::MSG_TBD_FEATURE)
   else
      prompt("Error in screen definition file. Undefined command sent to tbl_tool()")
   end
   
end # tbl_tool()

