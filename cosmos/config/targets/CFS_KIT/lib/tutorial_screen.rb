###############################################################################
# cFS Kit Tutorials 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
################################################################################

require 'cfs_kit_global'

################################################################################
## Global Variables
################################################################################


################################################################################
## Launch Tutorial
################################################################################

def launch_tutorial(screen, tutorial, lesson)

   script_file = "#{CFS_KIT_TUTORIAL_DIR}/#{tutorial}/#{lesson}.rb"

   if File.exists? script_file
      spawn("ruby #{Cosmos::USERPATH}/tools/ScriptRunner -r '#{script_file}'")
   else
      prompt("#{script_file} tutorial file does not exist")
   end

end # launch_tutorial()

