###############################################################################
# BudgetSat (bSat) Mission Sim Pass
#
# Perform operational functions during a simulated ground contact.  This script
# is designed to make you think about what types of functions need to be
# performed. It is not trying to be prescriptive in how to perform them. Each
# mission has its own unique ops concept and functional requirements.
#
# Notes:
#   1. This script is part of the cFS systems engineering training module
#   2. OSK uses TFTP and not CFDP for file transfers which impacts how
#      files are transferred and managed onboard.
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_ops'

#
# Wait for contact to start
#


#
# Assess health of spacecraft
#
# - If event log is full then it should be downlinked or you may always
#   downlink DS as the means for capturing all events.  
# - 
#

Osk::Ops::send_flt_bin_file_cmd("CFE_EVS", "WRITE_LOG_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_EVS_LOG)
wait " Review event log file displayed in Table Manager"

Osk::flight.cfe_evs.send_cmd("CLEAR_LOG")


#
# Manage Onboard Files
#

# Simple demo of deleting one file; needs to be expanded

isim_filename     = "isim_00.txt"
flt_path_filename = "/cf/bsat/#{isim_filename}"
gnd_path_filename = "#{Cosmos::USERPATH}/#{Osk::REL_SRV_DIR}/#{isim_filename}"

if (Osk::system.file_transfer.get(flt_path_filename,gnd_path_filename))
   Osk::flight.fm.send_cmd("DELETE_FILE with FILENAME #{flt_path_filename}")
end
 
#
# Prepare for ops while out of contact
#
# - Optionally load next ATS
#

