################################################################################
# cFE Event Service  
#
# Notes:
#   1. Debug events are enabled for the apps used during the demo.
#
# Demo Steps:
#   1. Enable/disable KIT_SCH debug events
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'osk_global'
require 'osk_system'
require 'osk_flight'

######################
## Global Variables ##
######################


###########
## Setup ##
###########

display("CFE_EVS CFE_EVS_SCREEN",50,50)    
 
cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME CFE_EVS, BITMASK 0x01") # Enable debug events

  
##################################################
## Demo 1 - Enable/disable KIT_SCH debug events ##
##################################################

wait # Demo Steps: 
# 1. Enable KIT_SCH debug event messages
# 2. Wait and observe event message generation  
# 3. Disable KIT_SCH debug event messages

Osk::flight.cfe_evs.send_cmd("ENA_APP_EVENT_TYPE with APP_NAME KIT_SCH, BITMASK 1"); # Enable debug messages
wait 3
Osk::flight.cfe_evs.send_cmd("DIS_APP_EVENT_TYPE with APP_NAME KIT_SCH, BITMASK 1"); # Disable debug messages

wait # Demo notes:
# KIT_SCH has a 4Hz execution rate so enabling its debug events will cause a large volume of debug
# messages to be sent. Debug messages can be helpful when troubleshooting problems. Developers shoud take 
# care when defining event types because normal operations should not cluttered with too many informational
# messages.  Informational messages should communication significant events.
# Event Service supports 4 types/level of events that are defined using bit poitions in the command:
#    Bit 0: Debug
#    Bit 1: Informoation
#    Bit 2: Error
#    Bit 3: Critical


#############
## Cleanup ##
#############

cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME CFE_EVS, BITMASK 0x01") # Disable debug events

