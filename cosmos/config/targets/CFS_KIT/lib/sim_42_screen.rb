###############################################################################
# cFS Kit 42 Simulator Management 
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
## Send Commands
################################################################################

def sim_42_send_cmd(screen, cmd)

  if (cmd == "RUN")
    continue = message_box("This runs an example simultion with 42. Continue?",'Yes','No')
    if (continue == 'Yes')
       spawn("xfce4-terminal --default-working-directory=""#{Cosmos::USERPATH}/#{REL_DIR_42}"" --execute ./42""")
       wait(20)
       cmd("I42 CONNECT_TO_42")
    end
  elsif (cmd == "CONNECT")
    continue = message_box("The 42 simulator must be running for connect to succeed. Continue?",'Yes','No')
    if (continue == 'Yes')
       cmd("I42 CONNECT_TO_42")
    end
  elsif (app == "TODO")
    prompt("Feature coming soon...")
  else
    prompt("Error in screen definition file. Undefined commmand sent to sim_42_send_cmd()")
  end
  
end # sim_42_send_cmd()

