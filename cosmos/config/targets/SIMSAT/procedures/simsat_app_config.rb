###############################################################################
# Simple Sat (SimSat) App Config
#
# This script configures the apps used for an example mission sim. Some of the
# configurations may be redundant with OSK's default configuration but they are
# done here to ensure the proper sim configuration. Tables required by the sim
# are part of OSK's default tables so they do not need to be loaded. 
#
# ToDo:
#   1. Replace hard waits with automated verification
#
# Notes:
#   1. This script is part of a training module. It is for illustrative
#      purposes and not intended to be an actual ops script. The verify_noop
#      calls ensure each ap required for the example is loaded and running.
#      The noop calls are commented out if another command is sent to an app
#      because that command will confirm the app is running.  
#   2. The ISIM app uses more event messages than a typical flight app. The
#      events are used for illustrative purposes.  
#
# Preconditions
#   1. Background thread periodically requesting DS FILE_INFO_PKT packets is
#      running
#
# Global Script Variables:
#   simsat_ops_enable - Boolean indicating whether ops example is active
#   simsat_ops_status - Text string displayed on ops example screen
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'osk_ops'
require 'ccsds'

require 'fsw_const'

require 'simsat_const'

def verify_noop (target,noop_fc=0)
  
   result = false
	
   target_hk_str = "#{target} #{Osk::TLM_STR_HK_PKT}" 
   
   cmd_valid_cnt = tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_VLD}")
   cmd_error_cnt = tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_ERR}")
   seq_cnt       = tlm("#{target_hk_str} #{Ccsds::PRI_HDR_SEQUENCE}")

   cmd("#{target} #{Osk::CMD_STR_NOOP} with #{Ccsds::SEC_HDR_FUNCCODE} #{noop_fc}")
   wait("#{target_hk_str} #{Osk::TLM_STR_CMD_VLD} == #{cmd_valid_cnt}+1", 10)  # Delay until updated cmd valid count or timeout. 
	
    ##puts tlm("#{target_hk_str} CMD_VALID_COUNT")
    if ( (tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_VLD}") == (cmd_valid_cnt + 1)) && 
         (tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_ERR}") ==  cmd_error_cnt))
      result = true
    else
      if ( tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_VLD}") == seq_cnt)
        puts target + " HK telemetry packet not received"
      end      
      puts target + " NOOP test failed"
    end
	
    return result
    
end # verify_noop()
  
#########################
## Runtime Environment ##
#########################

simsat_ops_status = "Verifying Command Ingest App (KIT_CI) Noop command."
verify_noop("KIT_CI")

simsat_ops_status = "Load SimSat Scheduler Table: Transfer #{$SIMSAT_SCH_TBL_GND_FILENAME} and load #{SimSat::SCH_TBL_FILENAME}"
if (Osk::Ops.put_flt_file($SIMSAT_SCH_TBL_GND_FILENAME,$SIMSAT_SCH_TBL_FLT_FILENAME)) 
   Osk::flight.send_cmd("KIT_SCH","LOAD_TBL with ID #{FswConfigParam::KIT_SCH_SCHTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{$SIMSAT_SCH_TBL_FLT_FILENAME}")  
else
   message_box("Failed to load SimSat scheduler table #{SimSat::SCH_TBL_FILENAME}",false)
end

simsat_ops_status = "Verifying Telemetry Output App (KIT_TO) Noop command."
verify_noop("KIT_TO")


##########################
## Data/File Management ##
##########################

#simsat_ops_status = "Verifying File Manager App Noop command."
#Background thread issues FM commands so can't use: verify_noop("FM")
#Background thread issues DS commands so can't use: verify_noop("DS")

simsat_ops_status = "Enable Data Storage (DS) to start creating packet files"
Osk::flight.send_cmd("DS","SET_APP_STATE with APP_STATE 1") 
wait("DS HK_TLM_PKT APP_ENA_STATE == 'ENA'", 10)


simsat_ops_status = "Enable DS event and auxiliary file generation"
Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX 0, FILE_STATE 1") # Enable Event file
wait 2
Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX 6, FILE_STATE 1") # Enable Science Auxiliary file

wait("DS FILE_INFO_PKT FILE1_ENABLE == 1", 6)  # Assumes background thread requesting DS INFO packet has been started
wait("DS FILE_INFO_PKT FILE7_ENABLE == 1", 6)


simsat_ops_status = "Verifying Housekeeping (TFTP) app noop command"
verify_noop("HK")
# SimSat scheduler table enables 1Hz HK combo packets by default
#simsat_ops_status = "Enabling Housekeeping to create auxiliary packets."
#Osk::flight.send_cmd("KIT_SCH","CFG_SCH_TBL_ENTRY with SLOT 1, ACTIVITY 1, CONFIG 1") # Enable HK Combo Pkt #1 
#wait 2
#Osk::flight.send_cmd("KIT_SCH","CFG_SCH_TBL_ENTRY with SLOT 1, ACTIVITY 2, CONFIG 1") # Enable HK Combo Pkt #2

simsat_ops_status = "Verifying Trivial File Transfer Protocol (TFTP) app noop command"
verify_noop("TFTP")

simsat_ops_status = "Review Data/File Management app configurations"
status_bar("Review Data/File Management app configurations")
wait  # <Go> to continue
 
##############
## Autonomy ##
##############

#verify_noop("SC")

simsat_ops_status = "Enabling ISIM power off Relative Time Sequence (RTS) commands"
Osk::flight.send_cmd("SC","ENABLE_RTS with RTS_ID 6")
wait("SC HK_TLM_PKT RTS_6_DIS == 'FALSE'", 10)

#verify_noop("LC")

simsat_ops_status = "Setting Limit Checker to active mode"
Osk::flight.send_cmd("LC","SET_APP_STATE with NEW_STATE 1")
wait("LC HK_TLM_PKT APP_STATE == 'ACTIVE'", 10)

simsat_ops_status = "Enabling ISIM Fault Action Point"
Osk::flight.send_cmd("LC","SET_AP_STATE with AP_ID 2, NEW_STATE 1")
wait("LC HK_TLM_PKT AP_2_STATE == 'ACTIVE'", 10)

simsat_ops_status = "Review autonomy app configurations"
status_bar("Review autonomy app configurations")
wait  # <Go> to continue

########################################
## Attitude Determination and Control ##
########################################

simsat_ops_status = "Verfying 42 Interface (I42) app noop command"
verify_noop("I42")

simsat_ops_status = "Verifying 42 FSW Controller (F42) app noop command"
verify_noop("F42")


#####################
## Health & Safety ##
#####################

simsat_ops_status = "Verifying Checksum (CS) app noop command"
verify_noop("CS")

# HS not loaded to due CPU function issue: verify_noop("HS")

#################
## Maintenance ##
#################

simsat_ops_status = "Verifying Memory Dwell (MD) app noop command"
verify_noop("MD")

simsat_ops_status = "Verifying Memory Manager(MM) app noop command"
verify_noop("MM")

