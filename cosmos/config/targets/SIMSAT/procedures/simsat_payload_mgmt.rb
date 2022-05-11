###############################################################################
# SimpleSat (SimSat) Payload Managemet Procedures 
#
# Provide functions to manage the payload. These functions use a combination
# of PL_SIM and PL_MGR. Ideally these would be in separate libraries but 
# OSK & SimSat don't provide a complete FSW and simulation environment. For
# example, there's no FSW power manager app and OSK power simulation. PL_SIM
# simulates the payload power.  
#
# Notes:
#   1. This script is part of the cFS systems engineering training module and
#      is designed to make you think about what types of functions need to be
#      performed. It is not trying to be prescriptive in how to perform them.
#      Each mission has its own unique ops concept and functional requirements.
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

def simsat_payload_power_on(start_science=true)

   Osk::flight.send_cmd("PL_SIM", "POWER_ON")

   wait_check("PL_SIM HK_TLM_PKT PAYLOAD_POWER == 'READY'", 25) 

   Osk::flight.send_cmd("PL_MGR", "START_SCIENCE") if start_science

end # simsat_payload_sim_power_on()



def simsat_payload_power_off

   Osk::flight.send_cmd("PL_SIM","POWER_OFF")

   wait_check("PL_SIM HK_TLM_PKT PAYLOAD_POWER == 'OFF'", 10) 

end # simsat_payload_power_off()


def  simsat_payload_set_fault

   Osk::flight.send_cmd("PL_SIM","SET_FAULT")

   wait_check("PL_SIM HK_TLM_PKT DETECTOR_FAULT == 'TRUE'", 10) 

end # simsat_payload_set_fault()

def  simsat_payload_clear_fault

   Osk::flight.send_cmd("PL_SIM","CLEAR_FAULT")

   wait_check("PL_SIM HK_TLM_PKT DETECTOR_FAULT == 'FALSE'", 10) 

end # simsat_payload_clear_fault()
