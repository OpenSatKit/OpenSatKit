################################################################################
# Data-File app Management Demo 
#
# Notes:
#   1. Developed for the YouTube Data-File Management Apps training video.
# 
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'fsw_const'
require 'fsw_config_param'

require 'simsat_const'
require 'simsat_ops_example_utils'
require 'simsat_isim_mgmt'
require 'simsat_recorder_mgmt'
require 'simsat_req_tlm'

###########
## Setup ##
###########

Osk::System.check_n_start_cfs('simsat')

display ("SIMSAT DEMO_DATA_FILE_MGMT_SCREEN")

# Creates simsat recorder directory if needed
Osk::Ops.create_flt_dir(SimSat::FLT_SRV_DIR)  
Osk::Ops.create_flt_dir(SimSat::FLT_REC_DIR)  

################################
## Start Data File Generation ##
################################

#
# 1. Configure ISIM to science mode
# 2. Enable DS to create files
# 3. Start background script to request FM and DS status packets
#

# 1. Configure ISIM to science mode
simsat_isim_pwr_on

# 2. Enable DS to create files
Osk::flight.send_cmd("DS","SET_APP_STATE with APP_STATE 1") 
wait("DS HK_TLM_PKT APP_ENA_STATE == 'ENA'", 10)
Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX 0, FILE_STATE 1") # Enable Event file
wait 1
Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX 6, FILE_STATE 1") # Enable Science Auxiliary file

# 3. Start background script to request FM and DS status packets
$request_tlm = SimSatReqTlm.new

status_bar("Wait until some files have been created in the recorder") 
wait  # <Go> to continue

#############################
## Playback Recorder Files ##
#############################

#
# 1. Disable file generation
# 2. Load user selected TO downlink rate table file 
# 3. Start playback using user selected file transfer method
#

# 1. Disable file generation

Osk::flight.send_cmd("DS","SET_APP_STATE with APP_STATE 0") 
wait("DS HK_TLM_PKT APP_ENA_STATE == 'DIS'", 10)
Osk::flight.send_cmd("ISIM","STOP_SCI_DATA")

# 2. Load user selected TO downlink rate table file. Don't assume table is onboard.

to_rate = message_box("Select telemetry output downlink rate table","4K bps","32K bps",false)
if (to_rate =="4K bps")
   gnd_file = SimSat::KIT_TO_4K_GND_FILE
   flt_file = SimSat::KIT_TO_4K_FLT_FILE
else
   gnd_file = SimSat::KIT_TO_32K_GND_FILE
   flt_file = SimSat::KIT_TO_32K_FLT_FILE
end

if (Osk::Ops.put_flt_file(gnd_file,flt_file))
   Osk::flight.send_cmd("KIT_TO","LOAD_TBL with ID #{FswConfigParam::KIT_TO_PKTTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{flt_file}")  
else
   message_box("Failed to load KIT_TO table. Verify flight software configuration and COSMOS connection",false)
   return
end

# 3. Start playback using user selected file transfer method

file_transfer = message_box("Select file transfer app","CFDP","TFTP",false)
if (file_transfer == "CFDP")
   simsat_recorder_playback("CFDP")
else
   simsat_recorder_playback("TFTP")         
end

status_bar("Observe recorder directory has been emptied") 
wait  # <Go> to continue

#############
## Cleanup ##
#############

clear ("SIMSAT DEMO_DATA_FILE_MGMT_SCREEN")
