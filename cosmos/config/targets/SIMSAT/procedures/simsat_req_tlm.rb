###############################################################################
# SimpleSat Request Telemetry Class
#
# Used to as a background thread that requests status telemetry packets so 
# DS and FM telemetry points are updatedd
#
# Notes:
#   1. This script is part of a training module. It is for illustrative
#      purposes and not intended to be an actual ops script. 
#   2. This scrpt is ignorant of whether a ground contact is being
#      simulated as active or inactive. 
#   3. Threading doesn't seem to be supported by ScripRunner so a new 
#      new thread must be started from the TlmViewer script if this is
#      used as part of a screen driven demo.  
#
# Global Script Variables:
#   None - Global variables should not be used since the some are called by
#          screen scripts (i.e. TlmViewer) and others by ScriptRunner and
#          they have different runtime environments.
#
###############################################################################

require 'thread'

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'osk_ops'

require 'simsat_const'
require 'simsat_isim_mgmt'


#
# The thread can be stopped in two ways:
# 1. Direct invocation o fthe stop() method
# 2. The creation of a special file SimSat::GND2FSW_STOP_OPS_FILE
#
class SimSatReqTlm < Thread

   def initialize
      @ops_active = true
    
      super do
    
         while (@ops_active)
         
            Osk::flight.send_cmd("DS","SEND_FILE_INFO")
            wait 2
         
            Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{SimSat::FLT_REC_DIR}, DIR_LIST_OFFSET 0")
            wait 2

            @ops_active = !File.exist?(SimSat::GND2FSW_STOP_OPS_FILE)

         end # while simsat_ops_enable
    
      end # super
      
   end # initialize () 
    
   def stop
      @op_active = false
   end
    
end # Class SimSatReqTlm

