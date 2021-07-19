###############################################################################
# OSK C Demo Screen
#
# Notes:
#   None
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_ops'
require 'osk_education'
require 'fsw_const'
require 'fsw_config_param'

################################################################################
## GUI Send Commands
################################################################################

def osk_c_demo_ops_cmd(screen, cmd)
 
   case cmd 
   when "NOOP"
      Osk::Ops::send_flt_cmd("OSK_C_DEMO", Osk::CMD_STR_NOOP)
   when "RESET"
      Osk::Ops::send_flt_cmd("OSK_C_DEMO", Osk::CMD_STR_RESET)
   when "LOAD_TBL"
      Osk::Ops::load_json_tbl("OSK_C_DEMO", OskCDemo::MSGLOG_TBL_ID)
   when "DUMP_TBL"
      Osk::Ops::dump_json_tbl("OSK_C_DEMO", OskCDemo::MSGLOG_TBL_ID)
   when "START_LOG"
      msg_id = ask_string("Input message ID (cFE EVS HK default)", Fsw::MsgId::CFE_EVS_HK_TLM_MID)
      Osk::flight.send_cmd("OSK_C_DEMO", "START_LOG with MSG_ID #{msg_id}")
   when "STOP_LOG"
      Osk::Ops::send_flt_cmd("OSK_C_DEMO", "STOP_LOG")
   when "START_PLAYBK"
      Osk::Ops::send_flt_cmd("OSK_C_DEMO", "START_PLAYBK")
   when "STOP_PLAYBK"
      Osk::Ops::send_flt_cmd("OSK_C_DEMO", "STOP_PLAYBK")
   when "DISPLAY_FILE"
      def_msg_id = Fsw::MsgId::CFE_EVS_HK_TLM_MID.sub('0x','')
      msg_id = ask_string("Input message ID as four hex digits (e.g. '#{def_msg_id}'= cFE EVS HK default)", def_msg_id)
      flt_path_filename = "#{OskCDemo::LOG_FILE_PREFIX}#{msg_id}#{OskCDemo::LOG_FILE_EXT}"
      gnd_filename = flt_path_filename[(flt_path_filename.rindex('/')+1)..flt_path_filename.length]
      Osk::Ops::get_flt_txt_file(flt_path_filename, Osk::REL_SRV_TBL_DIR, gnd_filename)
   when "SCRIPT_FUNC_TEST"
      Osk::System.check_n_start_cfs('prompt_for_target')
      test_suite = "--suite SandboxFuncTestSuite"
      test_group = "--group OskCDemoFuncTest"
      spawn("ruby #{Osk::COSMOS_TST_RUNNER} #{test_suite} #{test_group}")
   when "SCRIPT_OPS_EXAMPLE"
      Osk::System.check_n_start_cfs('prompt_for_target')
      spawn("ruby #{Osk::COSMOS_SCR_RUNNER} osk_c_demo_ops_example.rb")
   else
      raise "Error in screen definition file. Undefined cmd '#{cmd}' sent osk_c_demo_ops_cmd()"
   end
  
end # osk_c_demo_ops_cmd()

   