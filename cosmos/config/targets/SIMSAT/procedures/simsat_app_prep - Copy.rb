###############################################################################
# Simple Sat (SimSat) App Prep
#
# This script configures the apps used for an example mission sim. Some of the
# configurations may be redundant with OSK's default configuration but they are
# done here to ensure the proper sim configuration. Tables required by the sim
# are part of OSK's default tables so they do not need to be loaded. 
#
# Notes:
#   1. This script is part of the cFS systems engineering training module
#   2. The iSim app uses more event messages than a typical flight app. The
#      events are used for illustrative purposes.  
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_ops'
require 'ccsds'

def verify_noop(target,noop_fc=0)
  
   result = false
	
   target_hk_str = target + " " + Osk::TLM_STR_HK_PKT
   
   cmd_valid_cnt = tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_VLD}")
   cmd_error_cnt = tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_ERR}")
   seq_cnt       = tlm("#{target_hk_str} #{Ccsds::PRI_HDR_SEQUENCE}")

   cmd("#{target} #{Osk::CMD_STR_NOOP} #{Ccsds::SEC_HDR_FUNCCODE} #{noop_fc}")
   wait("#{@target_hk_str} #{Osk::TLM_STR_CMD_VLD} == #{cmd_valid_cnt}+1", 10)  # Delay until updated cmd valid count or timeout. 
	
    ##puts tlm("#{@target_hk_str} CMD_VALID_COUNT")
    if ( (tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_VLD}") == (cmd_valid_cnt + 1)) && 
         (tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_ERR}") ==  cmd_error_cnt))
      result = true
    else
      if ( tlm("#{target_hk_str} #{Osk::TLM_STR_CMD_VLD}") == seq_cnt)
        puts app.name + " HK telemetry packet not received"
      end      
      puts app.name + " NOOP test failed"
    end
	
    return result
    
end # verify_noop()
  
#########################
## Runtime Environment ##
#########################

verify_noop ("KIT_CI",1)
verify_noop ("KIT_SCH",1)
verify_noop ("KIT_TO",1)


##########################
## Data/File Management ##
##########################

verify_noop ("FM")

verify_noop ("DS")

Osk::flight.send_cmd("DS","SET_APP_STATE with APP_STATE 1") # Enable DS to start creating packet files
wait 2

Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX 0, FILE_STATE 1") # Enable Event file
wait 2

Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX 6, FILE_STATE 1") # Enable Science Auxiliary file

verify_noop ("HK")

Osk::flight.send_cmd("KIT_SCH","CFG_SCH_ENTRY with SLOT 2, ACTIVITY 6, CONFIG 1") # Enable HK Combo Pkt #1 
wait 2

Osk::flight.send_cmd("KIT_SCH","CFG_SCH_ENTRY with SLOT 2, ACTIVITY 7, CONFIG 1") # Enable HK Combo Pkt #2
wait  # Review configuration

verify_noop ("TFTP",1)

 
##############
## Autonomy ##
##############

verify_noop ("SC")

Osk::flight.send_cmd("SC","ENABLE_RTS with RTS_ID 6") # Enable ISIM power off RTS
wait  # Review configuration

verify_noop ("LC")

Osk::flight.send_cmd("LC","SET_APP_STATE with NEW_STATE 1") # Set LC to active mode
wait 2

Osk::flight.send_cmd("LC","SET_AP_STATE with AP_ID 2, NEW_STATE 1") # Enable ISIM Fault AP
wait  # Review configuration

########################################
## Attitude Determination and Control ##
########################################

verify_noop ("I42",1)

verify_noop ("F42",1)


#####################
## Health & Safety ##
#####################

verify_noop ("CS")

#verify_noop ("CS")

#################
## Maintenance ##
#################

verify_noop ("MD")

verify_noop ("MM")


