################################################################################
# Runtime Apps Table Demo 
#
# Notes:
#   1. Developed for the YouTube Runtime Apps training video. Uses Limit
#      Checker's configuration to illustrate how an app is integrated into
#      a cFS system
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


# 
# Limit Checker Runtime configuration
#
#   lc_msgids.h 
#     #define LC_SEND_HK_MID   0x18A5
#     #define LC_SAMPLE_AP_MID 0x18A6
#     #define LC_HK_TLM_MID    0x08A7
#
#   KIT_SCH Message Table
#     [16] LC_SEND_HK_MID
#     [17] LC_SAMPLE_AP_MID
#
#   KIT_SCH Schedule Table
#     [1, 0] LC Execution(Sample AP)
#     [2,14] LC Housekeeping
#
#   KIT_TO Packet Table
#     [0x08A7] Filter(X of N with offset O): X=1, N=1, O=0
#

display ("CFS_KIT MNG_APP_RUNTIME_SCREEN")
display ("CFS_KIT AUTONOMY_MGMT_SCREEN")

wait # Review Limit Checker configuration

# 
# Demo steps:
# 1. Send Message Table telemetry for index 17
#    - FSW looks up first scheduler table entry using that message ID and includes that in telemetry
#    - Scheduler table entry is false so LC Action Points are not being sampled
#
Osk::flight.send_cmd("KIT_SCH","SEND_MSG_TBL_ENTRY with MSG_TBL_IDX 17")  
wait 
#
# 2. Set LC to passive mode
#    - Enabled LC's action point evaluation but no actiosn performed
#    - AP sample count remains static because scheduler table entry is disabled
#
Osk::flight.send_cmd("LC","SET_APP_STATE with NEW_STATE #{Fsw::Const::LC_STATE_PASSIVE}")
wait 
#
# 3. Enable LC's sample action point scheduler table entry 
#    - LC Sample AP increments by 4 (Assumes default LC AP table only has one AP enabled)
#
Osk::flight.send_cmd("KIT_SCH","CFG_SCH_TBL_ENTRY with SLOT 1, ACTIVITY 0, ENABLED 1")  
wait 1
Osk::flight.send_cmd("KIT_SCH","SEND_MSG_TBL_ENTRY with MSG_TBL_IDX 17")  
wait
#
# 4. Change LC's HK request in scheduler entry [2,14] to 1 second
#    - LC Sample AP increments by 1 because HK packet request is being sent at 1Hz
#
Osk::flight.send_cmd("KIT_SCH","LOAD_SCH_TBL_ENTRY with SLOT 2, ACTIVITY 14, ENABLED 1, PERIOD 1, OFFSET 0, MSG_TBL_IDX 16")  
wait 
#
# 5. Show TO packet entry for 0x08A7 (LC housekeeping)
#    - Filter is sequence count and sending 1 of 1 (every packet)
#
Osk::flight.send_cmd("KIT_TO","SEND_PKT_TLM with MSG_ID 0x08A7")  
wait 
#
# 6. Change KIT_TO packet filter to 1 of 4
#    - LC sample Ap goes back to incrementing by 4 due to packet filter and not the scheduler 
# 
Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID 0x08A7, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 1, FILTER_PARAM_X 4, FILTER_PARAM_O 0")  
wait 1
Osk::flight.send_cmd("KIT_TO","SEND_PKT_TLM with MSG_ID 0x08A7")
wait


#############
## Cleanup ##
#############

#
# Restore the default SCH and TO tables
#

Osk::flight.send_cmd("LC","SET_APP_STATE with NEW_STATE #{Fsw::Const::LC_STATE_DISABLED}")
wait 1
Osk::flight.send_cmd("KIT_SCH","CFG_SCH_TBL_ENTRY with SLOT 1, ACTIVITY 0, ENABLED 0")
wait 1
Osk::flight.send_cmd("KIT_SCH","LOAD_SCH_TBL_ENTRY with SLOT 2, ACTIVITY 14, ENABLED 1, PERIOD 4, OFFSET 0, MSG_TBL_IDX 16") 
wait 1
Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID 0x08A7, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 1, FILTER_PARAM_X 1, FILTER_PARAM_O 0")  
wait 1

clear ("CFS_KIT MNG_APP_RUNTIME_SCREEN")
clear ("CFS_KIT AUTONOMY_MGMT_SCREEN")