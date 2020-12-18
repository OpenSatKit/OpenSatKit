###############################################################################
# Kit Telemetry Output (KIT_TO) Application Test
#
# Notes:
#   1. Loaded by kit test suite so it can assume common utilities have been 
#      preloaded.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'app_func_test'

class KitToFuncTest < Cosmos::Test

   include AppFuncTest
   
   @@TEST_1S_SCH_TBL_FILENAME  = "test_1s_sch_tbl.json"
   @@TEST_4S_SCH_TBL_FILENAME  = "test_4s_sch_tbl.json"
   @@TEST_1S_SCH_TBL_GND_FILENAME = Osk::cfg_target_dir_file("KIT_SCH","tables",@@TEST_1S_SCH_TBL_FILENAME) 
   @@TEST_4S_SCH_TBL_GND_FILENAME = Osk::cfg_target_dir_file("KIT_SCH","tables",@@TEST_4S_SCH_TBL_FILENAME) 
   @@TEST_1S_SCH_TBL_FLT_FILENAME = File.join(Osk::FLT_SRV_DIR,@@TEST_1S_SCH_TBL_FILENAME)
   @@TEST_4S_SCH_TBL_FLT_FILENAME = File.join(Osk::FLT_SRV_DIR,@@TEST_4S_SCH_TBL_FILENAME)

   @@TEST_1S_PKT_TBL_FILENAME  = "test_1s_to_pkt_tbl.json"
   @@TEST_4S_PKT_TBL_FILENAME  = "test_4s_to_pkt_tbl.json"
   @@TEST_1S_PKT_TBL_GND_FILENAME = Osk::cfg_target_dir_file("KIT_TO","tables",@@TEST_1S_PKT_TBL_FILENAME) 
   @@TEST_4S_PKT_TBL_GND_FILENAME = Osk::cfg_target_dir_file("KIT_TO","tables",@@TEST_4S_PKT_TBL_FILENAME) 
   @@TEST_1S_PKT_TBL_FLT_FILENAME = File.join(Osk::FLT_SRV_DIR,@@TEST_1S_PKT_TBL_FILENAME)
   @@TEST_4S_PKT_TBL_FLT_FILENAME = File.join(Osk::FLT_SRV_DIR,@@TEST_4S_PKT_TBL_FILENAME)

   @@app_name = "KIT_TO"
   @@hk_str = "#{@@app_name} #{Osk::TLM_STR_HK_PKT}"
   @@tbl_dmp_file = "/cf/osk_to_pkt_tbl~.json"
   
   def initialize
      super()
      @app = app_func_test_init("#{@@app_name}")
      FswApp.validate_cmd(true)
   end

   def setup
      status_bar("setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      wait(2)
   end

   def teardown
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      wait(2)
   end

 
   def test_default_tbl
      status_bar("Default Table Test")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"

      #TODO - Add event message verification
      
      cmd_str = "DUMP_TBL with ID #{FswConfigParam::KIT_TO_PKTTBL_ID}, FILENAME #{@@tbl_dmp_file}"
      AppFuncTest.send_cmd(@app,cmd_str, expect_event: false)

      cmd_str = "LOAD_TBL with ID #{FswConfigParam::KIT_TO_PKTTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{@@tbl_dmp_file}"
      AppFuncTest.send_cmd(@app,cmd_str, expect_event: false)

      return ( (tlm("#{@@hk_str} LAST_TBL_LOAD_STATUS")    == Fsw::Const::OSK_TBLMGR_STATUS_VALID) &&
               (tlm("#{@@hk_str} LAST_TBL_LOAD_ATTR_ERRS") == 0) )
      
   end


   def test_stats
      # 
      # Use housekeeping packets with stable packet sizes (mature apps with low risk of 
      # changing) to verify TO's stats.
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
      # Verification Notes
      # - KIT_SCH and KIT_TO are not synched so the phasing of which packets are processed
      #   between each KIT_TO execution are not consistent each time the system is 
      #   started. You'll typically see the bytes/sec bounced between two values such as
      #   201 and 219 for the 0.25Hz case so the aevrage won't be reported as 210 exactly. 
      # - Note even though KIT_TO uses OS_TaskDelay()to manage its main loop any drift is
      #   neglible for a short test
      # - The approach is to collect 4 HK packets and verify the average is within
      #   +/-TBD% of the expected average.
      #

      #
      # Verify Strategy
      #
      # Configure the system to only generate the packets listed above
      # - Load new scheduler table
      # - Disable F42 controller packet (data driven, not scheduled)
      # - Restart stats by setting KIT_TO's app main loop delay
      # - Collect multiple HK samples and verify average
      #
      
      tolerance = 0.03
      
      test_1s_pkts_per_sec  = 8      
      test_1s_bytes_per_sec = 842      
      test_1s_min_bytes_per_sec = test_1s_bytes_per_sec - test_1s_bytes_per_sec*tolerance
      test_1s_max_bytes_per_sec = test_1s_bytes_per_sec + test_1s_bytes_per_sec*tolerance
      test_1s_passed = false
      puts "**** test_1s_min_bytes_per_sec=#{test_1s_min_bytes_per_sec.to_s}, test_1s_max_bytes_per_sec=#{test_1s_max_bytes_per_sec}\n"
      
      test_4s_pkts_per_sec = 2      
      test_4s_bytes_per_sec = 210      
      test_4s_min_bytes_per_sec = test_4s_bytes_per_sec - test_4s_bytes_per_sec*tolerance
      test_4s_max_bytes_per_sec = test_4s_bytes_per_sec + test_4s_bytes_per_sec*tolerance
      test_4s_passed = false
      puts "**** test_4s_min_bytes_per_sec=#{test_4s_min_bytes_per_sec.to_s}, test_4s_max_bytes_per_sec=#{test_4s_max_bytes_per_sec}\n"
      
      # Transfer both test scheduler table files while TFTP telemetry still active 
      if (!Osk::Ops.put_flt_file(@@TEST_1S_SCH_TBL_GND_FILENAME,@@TEST_1S_SCH_TBL_FLT_FILENAME)) 
         return false
      end
      if (!Osk::Ops.put_flt_file(@@TEST_4S_SCH_TBL_GND_FILENAME,@@TEST_4S_SCH_TBL_FLT_FILENAME)) 
         return false
      end
 
     
      #
      # Configure 4s test
      # - Turn on event filtering as last step before data collection 
      #
      Osk::flight.send_cmd("KIT_SCH","LOAD_TBL with ID #{FswConfigParam::KIT_SCH_SCHTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{@@TEST_4S_SCH_TBL_FLT_FILENAME}")  
      wait 1
      Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::F42_CTRL_TLM_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 0, FILTER_PARAM_X 0, FILTER_PARAM_O 0")  
      wait 1
      Osk::flight.send_cmd("KIT_TO","SET_RUN_LOOP_DELAY with RUN_LOOP_DELAY 500")  
      wait("KIT_TO HK_TLM_PKT STATS_VALID == 'FALSE'", 10)
      wait("KIT_TO HK_TLM_PKT STATS_VALID == 'TRUE'", 100) #TODO - Use config parameters to estimate timeout

      Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 0, FILTER_PARAM_X 0, FILTER_PARAM_O 0")  

      #
      # Collect and verify 4s data
      #
      
      pkts_per_sec  = tlm("KIT_TO HK_TLM_PKT PKTS_PER_SEC")
      bytes_per_sec = tlm("KIT_TO HK_TLM_PKT BYTES_PER_SEC")
      seq_cnt = tlm("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE")
      5.times { |i|
         puts "Loop #{i}: seq_cnt=#{seq_cnt}, pkts_per_sec=#{pkts_per_sec}, bytes_per_sec=#{bytes_per_sec}\n"
         wait("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE >= #{seq_cnt}+3", 24) 
         pkts_per_sec  += tlm("KIT_TO HK_TLM_PKT PKTS_PER_SEC")
         bytes_per_sec += tlm("KIT_TO HK_TLM_PKT BYTES_PER_SEC") 
         seq_cnt = tlm("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE")
      }

      avg_pkts_per_sec  = pkts_per_sec/6.0
      avg_bytes_per_sec = bytes_per_sec/6.0
      passed_4s_test = ((test_4s_pkts_per_sec == avg_pkts_per_sec.round()) and
                        (test_4s_min_bytes_per_sec <= avg_bytes_per_sec)   and 
                        (test_4s_max_bytes_per_sec >= avg_bytes_per_sec))
      puts "**** 4s Test Results **** Pkts/Sec=#{avg_pkts_per_sec.to_s}, Bytes/Sec=#{avg_bytes_per_sec.to_s}, passed_4s_test=#{passed_4s_test}\n"
      
      #
      # Configure 1s test
      # - Turn off event filtering during setup so get event messages
      #
      
      Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 1, FILTER_PARAM_X 1, FILTER_PARAM_O 0")  
      wait 1
      Osk::flight.send_cmd("KIT_SCH","LOAD_TBL with ID #{FswConfigParam::KIT_SCH_SCHTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{@@TEST_1S_SCH_TBL_FLT_FILENAME}")  
      wait 1
      Osk::flight.send_cmd("KIT_TO","SET_RUN_LOOP_DELAY with RUN_LOOP_DELAY 500")  
      wait("KIT_TO HK_TLM_PKT STATS_VALID == 'FALSE'", 10)
      wait("KIT_TO HK_TLM_PKT STATS_VALID == 'TRUE'", 100) #TODO - Use config parameters to estimate timeout

      Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 0, FILTER_PARAM_X 0, FILTER_PARAM_O 0")  

      #
      # Collect and verify 1s data
      #

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
      passed_1s_test = ((test_1s_pkts_per_sec == avg_pkts_per_sec.round()) and
                        (test_1s_min_bytes_per_sec <= avg_bytes_per_sec)   and 
                        (test_1s_max_bytes_per_sec >= avg_bytes_per_sec))
      puts "**** 1s Test Results **** Pkts/Sec=#{avg_pkts_per_sec.to_s}, Bytes/Sec=#{avg_bytes_per_sec.to_s}, passed_4s_test=#{passed_1s_test}\n"

      #
      # Restore the default scheduler table
      #
      Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 1, FILTER_PARAM_X 1, FILTER_PARAM_O 0")  
      wait 1
      Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::F42_CTRL_TLM_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 1, FILTER_PARAM_X 1, FILTER_PARAM_O 0")  
      wait 1
      Osk::flight.send_cmd("KIT_SCH","LOAD_TBL with ID #{FswConfigParam::KIT_SCH_SCHTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{File.join(Osk::FLT_SRV_DIR,FswConfigParam::KIT_SCH_DEF_SCHTBL_FILENAME)}")  
      
      return (test_1s_passed and test_4s_passed)
      
   end # test_stats


   def test_pkt_tbl_filters
      # 
      # Use an approach similar to test_stats() so the same data can be used except use TO's packet
      # table filters to change the downlink rates. This test was written 2nd so may be able to 
      # remove test_stats or move it to a KIT_SCH test since it verifies the scheduler algorithms.
      # 
      
      tolerance = 0.03
      
      test_1s_pkts_per_sec  = 8      
      test_1s_bytes_per_sec = 842      
      test_1s_min_bytes_per_sec = test_1s_bytes_per_sec - test_1s_bytes_per_sec*tolerance
      test_1s_max_bytes_per_sec = test_1s_bytes_per_sec + test_1s_bytes_per_sec*tolerance
      test_1s_passed = false
      puts "**** test_1s_min_bytes_per_sec=#{test_1s_min_bytes_per_sec.to_s}, test_1s_max_bytes_per_sec=#{test_1s_max_bytes_per_sec}\n"
      
      test_4s_pkts_per_sec = 2      
      test_4s_bytes_per_sec = 210      
      test_4s_min_bytes_per_sec = test_4s_bytes_per_sec - test_4s_bytes_per_sec*tolerance
      test_4s_max_bytes_per_sec = test_4s_bytes_per_sec + test_4s_bytes_per_sec*tolerance
      test_4s_passed = false
      puts "**** test_4s_min_bytes_per_sec=#{test_4s_min_bytes_per_sec.to_s}, test_4s_max_bytes_per_sec=#{test_4s_max_bytes_per_sec}\n"
      
      # Transfer all test table files while TFTP telemetry still active 
      return false unless Osk::Ops.put_flt_file(@@TEST_1S_SCH_TBL_GND_FILENAME,@@TEST_1S_SCH_TBL_FLT_FILENAME)
      return false unless Osk::Ops.put_flt_file(@@TEST_4S_SCH_TBL_GND_FILENAME,@@TEST_4S_SCH_TBL_FLT_FILENAME)
      return false unless Osk::Ops.put_flt_file(@@TEST_1S_PKT_TBL_GND_FILENAME,@@TEST_1S_PKT_TBL_FLT_FILENAME)
      return false unless Osk::Ops.put_flt_file(@@TEST_4S_PKT_TBL_GND_FILENAME,@@TEST_4S_PKT_TBL_FLT_FILENAME)

      # 
      # Configure 1s test
      # 1. Load "1s" test scheduler table that requests HK tlm at 1s intervals
      # 2. Filter all events so they don't impact stats
      # 3. Load "1s" test TO packet table that sends every packet. Table loads reset stat computation. 
      #
      Osk::flight.send_cmd("KIT_SCH","LOAD_TBL with ID #{FswConfigParam::KIT_SCH_SCHTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{@@TEST_1S_SCH_TBL_FLT_FILENAME}")  
      wait 1
      Osk::flight.send_cmd("KIT_TO","LOAD_TBL with ID #{FswConfigParam::KIT_TO_PKTTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{@@TEST_1S_PKT_TBL_FLT_FILENAME}")  
      wait 1      
      Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 0, FILTER_PARAM_X 0, FILTER_PARAM_O 0")  
      
      wait("KIT_TO HK_TLM_PKT STATS_VALID == 'FALSE'", 10)
      wait("KIT_TO HK_TLM_PKT STATS_VALID == 'TRUE'", 100) #TODO - Use config parameters to estimate timeout
     
      #
      # Collect and verify 1s data
      #

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
      passed_1s_test = ((test_1s_pkts_per_sec == avg_pkts_per_sec.round()) and
                        (test_1s_min_bytes_per_sec <= avg_bytes_per_sec)   and 
                        (test_1s_max_bytes_per_sec >= avg_bytes_per_sec))
      puts "**** 1s Test Results **** Pkts/Sec=#{avg_pkts_per_sec.to_s}, Bytes/Sec=#{avg_bytes_per_sec.to_s}, passed_4s_test=#{passed_1s_test}\n"

      #
      # Configure 4s test
      # - Turn on event filtering as last step before data collection 
      #
      Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 1, FILTER_PARAM_X 1, FILTER_PARAM_O 0")  
      wait 1
      Osk::flight.send_cmd("KIT_TO","LOAD_TBL with ID #{FswConfigParam::KIT_TO_PKTTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{@@TEST_4S_PKT_TBL_FLT_FILENAME}")  
      wait 1      
      Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 0, FILTER_PARAM_X 0, FILTER_PARAM_O 0")  
      
      wait("KIT_TO HK_TLM_PKT STATS_VALID == 'FALSE'", 10)
      wait("KIT_TO HK_TLM_PKT STATS_VALID == 'TRUE'", 100) #TODO - Use config parameters to estimate timeout


      #
      # Collect and verify 4s data
      #
      
      pkts_per_sec  = tlm("KIT_TO HK_TLM_PKT PKTS_PER_SEC")
      bytes_per_sec = tlm("KIT_TO HK_TLM_PKT BYTES_PER_SEC")
      seq_cnt = tlm("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE")
      5.times { |i|
         puts "Loop #{i}: seq_cnt=#{seq_cnt}, pkts_per_sec=#{pkts_per_sec}, bytes_per_sec=#{bytes_per_sec}\n"
         wait("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE >= #{seq_cnt}+3", 24) 
         pkts_per_sec  += tlm("KIT_TO HK_TLM_PKT PKTS_PER_SEC")
         bytes_per_sec += tlm("KIT_TO HK_TLM_PKT BYTES_PER_SEC") 
         seq_cnt = tlm("KIT_TO HK_TLM_PKT CCSDS_SEQUENCE")
      }

      avg_pkts_per_sec  = pkts_per_sec/6.0
      avg_bytes_per_sec = bytes_per_sec/6.0
      passed_4s_test = ((test_4s_pkts_per_sec == avg_pkts_per_sec.round()) and
                        (test_4s_min_bytes_per_sec <= avg_bytes_per_sec)   and 
                        (test_4s_max_bytes_per_sec >= avg_bytes_per_sec))
      puts "**** 4s Test Results **** Pkts/Sec=#{avg_pkts_per_sec.to_s}, Bytes/Sec=#{avg_bytes_per_sec.to_s}, passed_4s_test=#{passed_4s_test}\n"
      
      #
      # Restore the default SCH and TO tables
      #
      Osk::flight.send_cmd("KIT_TO","UPDATE_FILTER with MSG_ID #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID}, FILTER_TYPE #{Fsw::Const::OSK_PKTUTIL_FILTER_BY_SEQ_CNT}, FILTER_PARAM_N 1, FILTER_PARAM_X 1, FILTER_PARAM_O 0")  
      wait 1
      Osk::flight.send_cmd("KIT_SCH","LOAD_TBL with ID #{FswConfigParam::KIT_SCH_SCHTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{File.join(Osk::FLT_SRV_DIR,FswConfigParam::KIT_SCH_DEF_SCHTBL_FILENAME)}")  
      wait 1
      Osk::flight.send_cmd("KIT_TO","LOAD_TBL with ID #{FswConfigParam::KIT_TO_PKTTBL_ID}, TYPE #{Fsw::Const::OSK_TBLMGR_LOAD_REPLACE}, FILENAME #{File.join(Osk::FLT_SRV_DIR,FswConfigParam::KIT_TO_DEF_PKTTBL_FILENAME)}")  
      
      return (test_1s_passed and test_4s_passed)
      
   end # test_pkt_tbl_filters

end # Class KitToFuncTest
