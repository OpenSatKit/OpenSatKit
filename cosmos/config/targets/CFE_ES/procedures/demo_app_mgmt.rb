################################################################################
# cFE Executive Service App Management Demo 
#
# Notes:
#   1. Debug events are enabled for the apps used during the demo.
#
# Demo Steps:
#   1. Display detailed app info
#   2. Stop an app
#   3. Start an app
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

AMD_INFO_APP = "FM"  # App used for informational demos
AMD_LOAD_APP = "MD"  # App used for stop, start, and reloads
AMD_LOAD_APP_FILE = "#{Osk::FLT_SRV_DIR}/md.so"

###########
## Setup ##
###########

display("CFE_ES APP_MGMT_SCREEN",1500,50)    
display("SIMSAT CFS_APP_SCREEN",50,50)    

cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME CFE_ES, BITMASK 0x01") # Enable debug events


wait # Setup notes: 
# The App Management and App Summary screens have been opened. Note the
# number of registered apps is less than the tasks because the cFE apps
# are not part of the registered app count and some apps have child tasks
  
######################################
## Demo 1 - Display App Information ##
######################################

wait # Demo Steps: 
# 1. Send cFE ES 'Send App Info' command for File Manager app
#    ES sends an app info telemetry packet that is displayed in 
#    the APP_MGMT_SCREEN 

Osk::flight.cfe_es.send_cmd("SEND_APP_INFO with APP_NAME #{AMD_INFO_APP}")
  
wait # Demo notes:
# The App Summary window lists all of the applications that are pre-installed
# in the SimSat reference mission. Not all of these apps are loaded in the
# cFE Education target so you won't see all of the housekeeping telemetry packets
# update. The cfe_es_startup.src file defines which apps are loaded during
# initialization and these are counted as 'registered apps'. Most of the
# information in the registered app display comes from the startup script file:
# File Name, App Name, Entry Point, Priority, Stack Size, and Exception Action.


##########################
## Demo 2 - Stop an app ##
##########################

wait  # Demo Steps:
# 1. Send cFE ES 'Send App Info' command for the Memory Dwell(MD) app
# 2. Send a Noop command to demonstrate the Memory Dwell(MD) app is operational 
# 3. Send ES command to stop the Memory Dwell(MD) app
# 4. Send a Noop command to demonstrate the Memory Dwell(MD) app is operational 

Osk::flight.cfe_es.send_cmd("SEND_APP_INFO with APP_NAME #{AMD_LOAD_APP}")
wait 2

cmd("#{AMD_LOAD_APP} NOOP")
wait 2

Osk::flight.cfe_es.send_cmd("STOP_APP with APP_NAME #{AMD_LOAD_APP}")
wait 2
            
cmd("#{AMD_LOAD_APP} NOOP")

wait # Demo notes:
# Note MD' priority. This will be changed when the app is started again
# The registered app count decrements by one after the app is stopped


###########################
## Demo 3 - Start an app ##
###########################

wait # Demo Steps: 
# 1. Send ES command to start the Memory Dwell(MD) app
# 2. Send cFE ES 'Send App Info' command for the Memory Dwell(MD) app
# 3. Send a Noop command to demonstrate the Memory Dwell(MD) app is operational 

Osk::flight.cfe_es.send_cmd("START_APP with APP_NAME #{AMD_LOAD_APP}, \
                            APP_ENTRY_POINT MD_AppMain, \
                            APP_FILENAME #{AMD_LOAD_APP_FILE}, \
                            STACK_SIZE 16384,\
                            EXCEPTION_ACTION 0, \
                            PRIORITY 100")
wait 5                                     

Osk::flight.cfe_es.send_cmd("SEND_APP_INFO with APP_NAME #{AMD_LOAD_APP}")
wait 2

cmd("#{AMD_LOAD_APP} NOOP")
 
wait # Demo notes:
# Notice the registered app count increments by one after the app is started
# and the new task priority of 100.


#############
## Cleanup ##
#############

cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME CFE_ES, BITMASK 0x01") # Disable debug events
clear("CFE_ES APP_MGMT_SCREEN")    
clear("SIMSAT CFS_APP_SCREEN")   

