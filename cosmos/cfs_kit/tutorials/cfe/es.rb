###############################################################################
# cFE tutorial top-level script
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

###############
## Constants ##
###############


FLT_BIN_FILE = "#{Osk::FLT_SRV_DIR}/#{Osk::TMP_BIN_FILE}"
GND_BIN_FILE = "#{Osk::GND_SRV_DIR}/#{Osk::TMP_BIN_FILE}"

ES_TMP_FILE  = "#{Osk::GND_SRV_DIR}/es_#{Osk::TMP_BIN_FILE}"

##########
## ES01 ##
##########

wait #ES01 - Startup script file. Click <Go> to begin  
puts "ES01 - Startup script file"

startup_script = open_file_dialog ("#{Osk::GND_BLD_CF_DIR}/" "Open Startup Script" ".scr")
file_data = ""
File.open(startup_script,'r') { |file| file_data = file.read() }
puts file_data  

wait  #ES01 - End/Review startup script file displayed in script runner output window

##########
## ES02 ##
##########

wait #ES02 - System Log. Click <Go> to begin  
puts "ES02 - System Log"

# Write system log to a file and transfer to the ground
cmd_cnt = tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT")
cmd("CFE_ES WRITE_SYSLOG_TO_FILE with FILENAME #{FLT_BIN_FILE}"); 
wait("CFE_ES HK_TLM_PKT CMD_VALID_COUNT == #{cmd_cnt}+1", 10)  # Delay until cmd count increments or timeout
if (tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT") == cmd_cnt)
  prompt ("Write syslog command failed. Notify the instructor.");
end 
			
if (Osk::system.file_transfer.get(FLT_BIN_FILE,GND_BIN_FILE))

  # Trouble displaying variable text fields using TableManager
  # spawn("ruby #{Cosmos::USERPATH}/tools/TableManager ")
  
  # Remove binary header and simply display it in the ScriptRunner log screen
  IO.copy_stream(GND_BIN_FILE,ES_TMP_FILE,File.size(GND_BIN_FILE)-64 ,64)
  file_data = ""
  File.open(ES_TMP_FILE,'r') { |file| file_data = file.read() }
  puts file_data  

else
  prompt ("File transfer failed. Notify the instructor.");
end # If file transferred

wait  #ES02 - End System Log

##########
## ES03 ##
##########

wait #ES03 - Exception-Reset Log. Click <Go> to begin  
puts "ES03 - Exception-Reset Log"

# Write erlog to a file and transfer to the ground
cmd_cnt = tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT")
cmd("CFE_ES WRITE_ERLOG_TO_FILE with FILENAME #{FLT_BIN_FILE}"); 
wait("CFE_ES HK_TLM_PKT CMD_VALID_COUNT == #{cmd_cnt}+1", 10)  # Delay until cmd count increments or timeout
if (tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT") == cmd_cnt)
  prompt ("Write erlog command failed. Notify the instructor.");
end 
	
if (Osk::system.file_transfer.get(FLT_BIN_FILE,GND_BIN_FILE))

   puts "Open #{FLT_BIN_FILE} in table manager by selecting File->Open->cfs_kit/file_server #{Osk::TMP_BIN_FILE}"
   puts "Select 'cfe_es_erlog.txt' when prompted for a definition file"
   spawn("ruby #{Osk::COSMOS_TBL_MANAGER}")
  
else
  prompt ("File transfer failed. Notify the instructor.");
end # If file transferred

wait  #ES03 - End Exception-Reset Log

##########
## ES04 ##
##########

wait #ES04 - Critical Data Store. Click <Go> to begin  
puts "ES04 - Critical Data Store"

# Write CDS registry to a file and transfer to the ground
cmd_cnt = tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT")
cmd("CFE_ES WRITE_CDS_REG_TO_FILE with FILENAME #{FLT_BIN_FILE}"); 
wait("CFE_ES HK_TLM_PKT CMD_VALID_COUNT == #{cmd_cnt}+1", 10)  # Delay until cmd count increments or timeout
if (tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT") == cmd_cnt)
  prompt ("Write critical data store registry command failed. Notify the instructor.");
end 
	
if (Osk::system.file_transfer.get(FLT_BIN_FILE,GND_BIN_FILE))

   puts "Open #{FLT_BIN_FILE} in table manager by selecting File->Open->cfs_kit/file_server #{Osk::TMP_BIN_FILE}"
   puts "Select 'cfe_es_cds_info.txt' when prompted for a definition file"
   spawn("ruby #{Osk::COSMOS_TBL_MANAGER} ")
  
else
  prompt ("File transfer failed. Notify the instructor.");
end # If file transferred

wait  #ES04 - End Critical Data Store Log

##########
## ES05 ##
##########

wait #ES04 - App Management. Click <Go> to begin  
puts "ES03 - Begin App Management"

prompt ("Goto 'Demo' tab and run the 'Apps' demo.")

wait  #ES05 - End App Info

##########
## ES06 ##
##########

wait #ES06 - Performance Monitor. Click <Go> to begin  
puts "ES06 - Begin Performance Monitor"

prompt ("Goto 'Demo' tab and run the 'PerfMon' demo.")

wait  #ES06 - End Performance Monitor
 
 
