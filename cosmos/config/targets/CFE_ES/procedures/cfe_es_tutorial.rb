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

wait #ES01 - Click <Go> to execute the steps in the comments below
# 1. Display the cFE Executive Service startup file in a text windown 
# 2. The script defines the libraries and apps that are loaded during initialization 
# 3. The comments at the end of the file describe the file contents
# 4. The first '!' defines the start of comment lines for the entire file

Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{Osk::CFS_CFSAT_CF_DIR}/#{Osk::CFE_STARTUP_FILE}'")

wait #ES01 - Click <Go> to continue to next section

###################################
## ES02 - Review system log file ##
###################################

wait #ES02 - Click <Go> to execute the steps in the comments below
# 1. Write the system log to a file
# 2. Transfer the binary file to the ground and display it using Table Manager
# 3. The system log file is used to log critical messages. Messages can be written
#    prior to the Event Service being available and in error situations when Event
#    Services may not be operating correctly 

Osk::flight.cfe_es.send_cmd("WRITE_SYSLOG_TO_FILE with FILENAME #{Osk::TMP_FLT_BIN_PATH_FILE}")
wait 2
if Osk::Ops::get_flt_file(Osk::TMP_FLT_BIN_PATH_FILE, Osk::TMP_GND_BIN_PATH_FILE)
   Cosmos.run_process("ruby lib/OskCfeFileViewer -f '#{Osk::TMP_GND_BIN_PATH_FILE}'")
else
   prompt ("Write syslog command failed. Notify the instructor.");
end

wait #ES02 - Click <Go> to continue to next section

#######################################
## ES03 - Review Exception-Reset log ##
#######################################

wait #ES03 - Click <Go> to execute the steps in the comments below
# 1. Write the exception-reset log to a file
# 2. Transfer the binary file to the ground and display it using Table Manager
# 3. The exception-reset log file is used to log detailed information about
#    the context of a reset. Many embedded processors often have multiple causes
#    and types of resets so this log can be a very helpful diagnostic tool 

wait #ES03 - Click <Go> to review the exception-reset log

Osk::Ops::send_flt_bin_file_cmd("CFE_ES", "WRITE_ERLOG_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_ES_ERLOG)

wait #ES03 - Click <Go> to continue to next section
         
################################################
## ES04 - Review Critical Data Store Registry ##
################################################

wait #ES04 - Click <Go> to execute the steps in the comments below
# 1. Write the Critical Data Store Registry to a file
# 2. Transfer the binary file to the ground and display it using Table Manager
# 3. The Critical Data Store Registry is used to preserve critical data across
#    processor resets that preserve the contents of RAM.

Osk::Ops::send_flt_bin_file_cmd("CFE_ES", "WRITE_CDS_REG_TO_FILE with ", Osk::TBL_MGR_DEF_CFE_ES_CDS_REG)

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

