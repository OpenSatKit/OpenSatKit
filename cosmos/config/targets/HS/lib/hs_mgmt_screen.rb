###############################################################################
# Health & Safety App Management 
#
# Notes:
#   1.  Most HS commands don't require parameters and could be sent directly
#       from the screen definition file. However dispatching all of the 
#       commands in the same manner using this script file isolates any future
#       command related changes to this file.    
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_system'
require 'osk_flight'
require 'osk_ops'


################################################################################
## Send Commands
################################################################################

def hs_mgmt_send_cmd(screen, cmd)
 
   ### Commands
   if (cmd == "NOOP")
      Osk::flight.send_cmd("HS",Osk::CMD_STR_NOOP)
   elsif (cmd == "RESET")
      Osk::flight.send_cmd("HS",Osk::CMD_STR_RESET)
   elsif (cmd == "ENA_ALIVENESS")
      Osk::flight.send_cmd("HS","ENA_ALIVENESS")
   elsif (cmd == "DIS_ALIVENESS")
      Osk::flight.send_cmd("HS","DIS_ALIVENESS")
   elsif (cmd == "ENA_APP_MON")
      Osk::flight.send_cmd("HS","ENA_APP_MON")
   elsif (cmd == "DIS_APP_MON")
      Osk::flight.send_cmd("HS","DIS_APP_MON")
   elsif (cmd == "ENA_EVENT_MON")
      Osk::flight.send_cmd("HS","ENA_EVENT_MON")
   elsif (cmd == "DIS_EVENT_MON")
      Osk::flight.send_cmd("HS","DIS_EVENT_MON")
   elsif (cmd == "ENA_CPU_HOG")
      Osk::flight.send_cmd("HS","ENA_CPU_HOG")
   elsif (cmd == "DIS_CPU_HOG")
      Osk::flight.send_cmd("HS","DIS_CPU_HOG")
   elsif (cmd == "CLR_CPU_RESET_CNT")
      Osk::flight.send_cmd("HS","CLR_CPU_RESET_CNT")
   elsif (cmd == "SET_MAX_CPU_RESETS")
      max_resets = ask_string("Enter max processor resets before watchdog reset","5")
      Osk::flight.send_cmd("HS","SET_MAX_CPU_RESETS with MAX_RESETS #{max_resets}")
   elsif (cmd.include? "DUMP_")
      case cmd
      when "DUMP_APP_MON_TBL"
         tbl_name = FswConfigParam::HS_APP_MON_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_HS_AMT_TBL
      when "DUMP_EVENT_MON_TBL"
         tbl_name = FswConfigParam::HS_EVENT_MON_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_HS_EMT_TBL
      when "DUMP_MSG_ACT_TBL"
         tbl_name = FswConfigParam::HS_MSG_ACT_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_HS_MAT_TBL
      when "DUMP_EXEC_CTR_TBL"
         tbl_name = FswConfigParam::HS_EXEC_CNT_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_HS_XCT_TBL
      else
         raise "Error in HS screen management definitions for #{cmd}. Screen text file and ruby scipt file don't agree on table dump options."
      end
      Osk::Ops::send_cfe_dump_tbl_cmd("HS.#{tbl_name}", tbl_mgr_def_filename)
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to hs_mgmt_send_cmd()"
   end
  
end # hs_mgmt_send_cmd()
