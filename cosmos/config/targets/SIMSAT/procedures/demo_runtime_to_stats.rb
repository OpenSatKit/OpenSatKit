################################################################################
# Runtime Apps TO Statistics Demo 
#
# Notes:
#   1. Developed for the YouTube Runtime Apps training video and 
#      based on the KIT_TO Packet Table Filter functional test. Instead of being
#      designed as a test that verifies against expected results this demo
#      simply reports what was computed for packets/s and bytes/s
#   2. Runtime App Demo (RAD) used as global variable prefix
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

######################
## Global Variables ##
######################

# 
# Scheduler and packet test tables are use housekeeping packets that are
# mature and unlikely to change much in teh future
#
# Slot 1
#   CFE_ES   156
#   CFE_EVS  156
# Slot 2
#   CFE_SB    40
# Slot 3
#   CFE_TBL  296
#   CFE_TIME  44
# Slot 3
#   KIT_CI    36
#   KIT_SCH   66
# Slot 4
#   KIT_TO    48
# --------------
#            842 bytes
#
# All packets at 1.0Hz:  8 pkts/sec, 842 bytes/sec
# All packets at 0.25Hz: 2 pkts/sec, 210 bytes/sec
#
#

RAD_1S_PKTS_PER_SEC  = 8      
RAD_1S_BYTES_PER_SEC = 842      

RAD_4S_PKTS_PER_SEC  = 2      
RAD_4S_BYTES_PER_SEC = 210

RAD_STATS_TRUE_TLM_TIMEOUT = (FswConfigParam::KIT_TO_PKTMGR_STATS_RECONFIG_INIT_MS/1000 *
                              FswConfigParam::KIT_TO_PKTMGR_COMPUTE_STATS_INTERVAL_MS/1000) + 8 # Add tlm filter slop
                                  
RAD_1S_SCH_TBL_FILENAME  = "test_1s_sch_tbl.json"
RAD_4S_SCH_TBL_FILENAME  = "test_4s_sch_tbl.json"
RAD_1S_SCH_TBL_GND_FILENAME = Osk::cfg_target_dir_file("KIT_SCH","tables",RAD_1S_SCH_TBL_FILENAME) 
RAD_4S_SCH_TBL_GND_FILENAME = Osk::cfg_target_dir_file("KIT_SCH","tables",RAD_4S_SCH_TBL_FILENAME) 
RAD_1S_SCH_TBL_FLT_FILENAME = File.join(Osk::FLT_SRV_DIR,RAD_1S_SCH_TBL_FILENAME)
RAD_4S_SCH_TBL_FLT_FILENAME = File.join(Osk::FLT_SRV_DIR,RAD_4S_SCH_TBL_FILENAME)

RAD_1S_PKT_TBL_FILENAME  = "test_1s_to_pkt_tbl.json"
RAD_4S_PKT_TBL_FILENAME  = "test_4s_to_pkt_tbl.json"
RAD_1S_PKT_TBL_GND_FILENAME = Osk::cfg_target_dir_file("KIT_TO","tables",RAD_1S_PKT_TBL_FILENAME) 
RAD_4S_PKT_TBL_GND_FILENAME = Osk::cfg_target_dir_file("KIT_TO","tables",RAD_4S_PKT_TBL_FILENAME) 
RAD_1S_PKT_TBL_FLT_FILENAME = File.join(Osk::FLT_SRV_DIR,RAD_1S_PKT_TBL_FILENAME)
RAD_4S_PKT_TBL_FLT_FILENAME = File.join(Osk::FLT_SRV_DIR,RAD_4S_PKT_TBL_FILENAME)

spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'KIT_TO HK_TLM_PKT'")

# 
# Demo Steps
#
# 1. Transfer scheduler and packet test tables
# 2. Configure 1s demo
# 3. Collect and report 1s data
# 4. Configure 4s demo
# 5. Collect and report 4s data
#
# Data collection and verification strategy & notes
# - Collect multiple HK packets, compute the average stats and report percentage
#   relative to expected stats
# - KIT_SCH and KIT_TO are not synched so the phasing of which packets are processed
#   between each KIT_TO execution are not consistent each time the demo is 
#   run. You'll typically see the bytes/sec bounced between two values such as
#   201 and 219 for the 0.25Hz case so the aevrage won't be reported as 210 exactly. 
# - Even though KIT_TO uses OS_TaskDelay()to manage its main loop any drift is
#   neglible for a short demo
#

#
# 1. Transfer test tables
#    - Transfer all test table files while TFTP telemetry still active 
#
wait # Review demo steps and approach. Click <Go> to transfer test tables

return false unless Osk::Ops.put_flt_file(RAD_1S_SCH_TBL_GND_FILENAME,RAD_1S_SCH_TBL_FLT_FILENAME)
return false unless Osk::Ops.put_flt_file(RAD_4S_SCH_TBL_GND_FILENAME,RAD_4S_SCH_TBL_FLT_FILENAME)
return false unless Osk::Ops.put_flt_file(RAD_1S_PKT_TBL_GND_FILENAME,RAD_1S_PKT_TBL_FLT_FILENAME)
return false unless Osk::Ops.put_flt_file(RAD_4S_PKT_TBL_GND_FILENAME,RAD_4S_PKT_TBL_FLT_FILENAME)

# 
# 2. Configure 1s demo
#    - Load "1s" test scheduler table that requests HK tlm at 1s intervals
#    - Filter all events so they don't impact stats
#    - Load "1s" test TO packet table that sends every packet. Table loads reset stat computation. 
#
wait # Review test table configuration. Click <Go> to configure 1s demo 

Osk::flight.send_cmd("KIT_SCH","LOAD_TBL with ID #{FswConfigParam::KIT_SCH_SCHTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{RAD_1S_SCH_TBL_FLT_FILENAME}")  
wait 1
Osk::flight.send_cmd("KIT_TO","LOAD_TBL with ID #{FswConfigParam::KIT_TO_PKTTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{RAD_1S_PKT_TBL_FLT_FILENAME}")  
wait 1      
Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 0, FILTER_PARAM_X 0, FILTER_PARAM_O 0")  

wait("KIT_TO HK_TLM_PKT STATS_VALID == 'FALSE'", 10)
wait("KIT_TO HK_TLM_PKT STATS_VALID == 'TRUE'", RAD_STATS_TRUE_TLM_TIMEOUT)
  
  
#
# 3. Collect and report 1s data
#
wait # Review 1s configuraton. Click <Go> to collect and report 1s data

pkts_per_sec  = tlm("KIT_TO HK_TLM_PKT PKTS_PER_SEC")
bytes_per_sec = tlm("KIT_TO HK_TLM_PKT BYTES_PER_SEC")
seq_cnt = tlm("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE")
3.times { |i|
   puts "Loop #{i}: seq_cnt=#{seq_cnt}, pkts_per_sec=#{pkts_per_sec}, bytes_per_sec=#{bytes_per_sec}\n"
   wait("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE == #{seq_cnt}+3", 20) 
   pkts_per_sec  += tlm("KIT_TO HK_TLM_PKT PKTS_PER_SEC")
   bytes_per_sec += tlm("KIT_TO HK_TLM_PKT BYTES_PER_SEC") 
   seq_cnt = tlm("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE")
}

avg_pkts_per_sec  = pkts_per_sec/4.0
avg_bytes_per_sec = bytes_per_sec/4.0
pkts_per_sec_percentage  = (avg_pkts_per_sec/RAD_1S_PKTS_PER_SEC)*100      
bytes_per_sec_percentage = (avg_bytes_per_sec/RAD_1S_BYTES_PER_SEC)*100      
msg_box_str = "1s Demo Results\n"\
              "#{avg_pkts_per_sec.to_s} pkts/s within #{pkts_per_sec_percentage.round(2).to_s}% of #{RAD_1S_PKTS_PER_SEC.to_s}\n"\
              "#{avg_bytes_per_sec.to_s} bytes/s within #{bytes_per_sec_percentage.round(2).to_s}% of #{RAD_1S_BYTES_PER_SEC}"
message_box(msg_box_str,false)

#
# 4. Configure 4s test
#    - Turn on event filtering as last step before data collection 
#
wait # Review 1s results. Click <Go> to configure 4s demo

Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 1, FILTER_PARAM_X 1, FILTER_PARAM_O 0")  
wait 1
Osk::flight.send_cmd("KIT_TO","LOAD_TBL with ID #{FswConfigParam::KIT_TO_PKTTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{RAD_4S_PKT_TBL_FLT_FILENAME}")  
wait 1      
Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 0, FILTER_PARAM_X 0, FILTER_PARAM_O 0")  

wait("KIT_TO HK_TLM_PKT STATS_VALID == 'FALSE'", 10)
wait("KIT_TO HK_TLM_PKT STATS_VALID == 'TRUE'", RAD_STATS_TRUE_TLM_TIMEOUT)
  

#
# 5. Collect and report 4s data
#
wait # Review 4s configuraton. Click <Go> to collect and report 4s data

pkts_per_sec  = tlm("KIT_TO HK_TLM_PKT PKTS_PER_SEC")
bytes_per_sec = tlm("KIT_TO HK_TLM_PKT BYTES_PER_SEC")
seq_cnt = tlm("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE")
7.times { |i|
   puts "Loop #{i}: seq_cnt=#{seq_cnt}, pkts_per_sec=#{pkts_per_sec}, bytes_per_sec=#{bytes_per_sec}\n"
   wait("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE >= #{seq_cnt}+3", 24) 
   pkts_per_sec  += tlm("KIT_TO HK_TLM_PKT PKTS_PER_SEC")
   bytes_per_sec += tlm("KIT_TO HK_TLM_PKT BYTES_PER_SEC") 
   seq_cnt = tlm("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE")
}

avg_pkts_per_sec  = pkts_per_sec/8.0
avg_bytes_per_sec = bytes_per_sec/8.0
pkts_per_sec_percentage  = (avg_pkts_per_sec/RAD_4S_PKTS_PER_SEC)*100      
bytes_per_sec_percentage = (avg_bytes_per_sec/RAD_4S_BYTES_PER_SEC)*100      
msg_box_str = "4s Demo Results\n"\
              "#{avg_pkts_per_sec.to_s} pkts/s within #{pkts_per_sec_percentage.round(2).to_s}% of #{RAD_4S_PKTS_PER_SEC.to_s}\n"\
              "#{avg_bytes_per_sec.to_s} bytes/s within #{bytes_per_sec_percentage.round(2).to_s}% of #{RAD_4S_BYTES_PER_SEC}"
message_box(msg_box_str,false)


#############
## Cleanup ##
#############
wait # Review 4s results. Click <Go> to clean up after demo

#
# Restore the default SCH and TO tables
#

Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 1, FILTER_PARAM_X 1, FILTER_PARAM_O 0")  
wait 1
Osk::flight.send_cmd("KIT_SCH","LOAD_TBL with ID #{FswConfigParam::KIT_SCH_SCHTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{File.join(Osk::FLT_SRV_DIR,FswConfigParam::KIT_SCH_DEF_SCHTBL_FILENAME)}")  
wait 1
Osk::flight.send_cmd("KIT_TO","LOAD_TBL with ID #{FswConfigParam::KIT_TO_PKTTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{File.join(Osk::FLT_SRV_DIR,FswConfigParam::KIT_TO_DEF_PKTTBL_FILENAME)}")  

