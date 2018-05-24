###############################################################################
# OpenSsatKit Tutorials 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
################################################################################

require 'osk_global'

################################################################################
## Launch Tutorial
################################################################################

def launch_tutorial(screen, tutorial, lesson)

   script_file = "#{Osk::TUTORIAL_DIR}/#{tutorial}/#{lesson}.rb"

   if File.exists? script_file
      spawn("ruby #{Osk::COSMOS_SCR_RUNNER} -r '#{script_file}'")
   else
      prompt("#{script_file} tutorial file does not exist")
   end

end # launch_tutorial()

