###############################################################################
# cFE Executive Service tutorial top-level script
# 
# Notes:
#   1. This tutorial is part of the cFE Service training module and implements
#      the ES exercises.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_ops'

###########
## Setup ##
###########

Osk::System.check_n_start_cfs('cfsat')

###########################################
## ES01 - Review cFE startup script file ##
###########################################

wait #ES01 - Click <Go> to begin  
Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::CFS_CFSAT_CF_DIR}/#{Osk::CFE_STARTUP_FILE}'")
puts "ES01 - Review startup script file displayed in popup window"
wait #ES01 - Click <Go> to continue to next section

###################################
## ES02 - Review system log file ##
###################################

wait #ES02 - Click <Go> to review the system log file

Osk::flight.cfe_es.send_cmd("WRITE_SYSLOG_TO_FILE with FILENAME #{Osk::TMP_FLT_BIN_PATH_FILE}")
wait 2
if Osk::Ops::get_flt_file(Osk::TMP_FLT_BIN_PATH_FILE, Osk::TMP_GND_BIN_PATH_FILE)
   Cosmos.run_process("ruby lib/OskCfeFileViewer -f '#{Osk::TMP_GND_BIN_PATH_FILE}'")
else
   prompt ("Write syslog command failed. Notify the instructor.");
end

puts "ES02 - Review system log file displayed in popup window"
wait #ES02 - Click <Go> to continue to next section

#######################################
## ES03 - Review Exception-Reset log ##
#######################################

wait #ES03 - Click <Go> to review the exception-reset log

Osk::Ops::send_flt_bin_file_cmd("CFE_ES", "WRITE_ERLOG_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_ES_ERLOG)

puts "ES03 - Review Exception-Reset log file displayed in Table Manager"
wait #ES03 - Click <Go> to continue to next section
         
################################################
## ES04 - Review Critical Data Store Registry ##
################################################

wait #ES04 - Click <Go> to review the critical data store registry

Osk::Ops::send_flt_bin_file_cmd("CFE_ES", "WRITE_CDS_REG_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_ES_CDS_REG)

puts "ES04 - Review Critical Data Store Registry file displayed in Table Manager"
wait #ES04 - Click <Go> to continue to next section

################################
## ES05 - App Management Demo ##
################################

wait #ES05 - Click <Go> to start app management demo
display("CFS_KIT APP_MGMT_DEMO_SCREEN",500,50)
wait #ES05 - Click <Go> to continue to next section

#####################################
## ES06 - Performance Monitor Demo ##
#####################################

wait #ES06 - Click <Go> to begin  
display("CFS_KIT PERF_MON_DEMO_SCREEN",500,50)
wait #ES06 - Click <Go> to exit
 
#############
## Cleanup ##
#############

