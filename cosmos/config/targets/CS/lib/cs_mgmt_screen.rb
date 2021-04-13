###############################################################################
# Checksum App Management 
#
# Notes:
#   1.  Most CS commands don't require parameters and could be sent directly
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

def cs_mgmt_send_cmd(screen, cmd)
 
   ### App Management
   if (cmd == "NOOP")
      Osk::flight.send_cmd("CS",Osk::CMD_STR_NOOP)
   elsif (cmd == "RESET")
      Osk::flight.send_cmd("CS",Osk::CMD_STR_RESET)
   elsif (cmd == "ENA_ALL")
      Osk::flight.send_cmd("CS","ENA_ALL")
   elsif (cmd == "DIS_ALL")
      Osk::flight.send_cmd("CS","DIS_ALL")
   
   ### Oneshot
   elsif (cmd == "START_ONESHOT")
      # Don't prompt for BYTES_PER_CYCLE to minimize user input prompts. A value of zero uses config parameter.
      address = ask_string("Enter starting address","0x00000000")
      num_bytes = ask_string("Enter number of bytes to checksum","256")
      Osk::flight.send_cmd("CS","START_ONESHOT with ADDRESS #{address}, NUM_BYTES #{num_bytes}, BYTES_PER_CYCLE 0")
   elsif (cmd == "CANCEL_ONESHOT")
      Osk::flight.send_cmd("CS","CANCEL_ONESHOT")
   
   ### App Table
   elsif (cmd == "ENA_APP_TBL")
      Osk::flight.send_cmd("CS","ENA_APP_TBL")
   elsif (cmd == "DIS_APP_TBL")
      Osk::flight.send_cmd("CS","DIS_APP_TBL")
   elsif (cmd == "ENA_APP_TBL_ENTRY")
      app_name = ask_string("Enter app name")
      Osk::flight.send_cmd("CS","ENA_APP_TBL_ENTRY with APP_NAME #{app_name}")
   elsif (cmd == "DIS_APP_TBL_ENTRY")
      app_name = ask_string("Enter app name")
      Osk::flight.send_cmd("CS","DIS_APP_TBL_ENTRY with APP_NAME #{app_name}")
   elsif (cmd == "COMPUTE_APP_BASELINE")
      app_name = ask_string("Enter app name")
      Osk::flight.send_cmd("CS","COMPUTE_APP_BASELINE with APP_NAME #{app_name}")
   elsif (cmd == "REPORT_APP_BASELINE")
      app_name = ask_string("Enter app name")
      Osk::flight.send_cmd("CS","REPORT_APP_BASELINE with APP_NAME #{app_name}")

   ### Tables Table
   elsif (cmd == "ENA_TABLES_TBL")
      Osk::flight.send_cmd("CS","ENA_TABLES_TBL")
   elsif (cmd == "DIS_TABLES_TBL")
      Osk::flight.send_cmd("CS","DIS_TABLES_TBL")
   elsif (cmd == "ENA_TABLES_TBL_ENTRY")
      tbl_name = ask_string("Enter table name(app.table)")
      Osk::flight.send_cmd("CS","ENA_TABLES_TBL_ENTRY with TBL_NAME #{tbl_name}")
   elsif (cmd == "DIS_TABLES_TBL_ENTRY")
      tbl_name = ask_string("Enter table name(app.table)")
      Osk::flight.send_cmd("CS","DIS_TABLES_TBL_ENTRY with TBL_NAME #{tbl_name}")
   elsif (cmd == "COMPUTE_TBL_BASELINE")
      tbl_name = ask_string("Enter table name(app.table)")
      Osk::flight.send_cmd("CS","COMPUTE_TBL_BASELINE with TBL_NAME #{tbl_name}")
   elsif (cmd == "REPORT_TBL_BASELINE")
      tbl_name = ask_string("Enter table name(app.table)")
      Osk::flight.send_cmd("CS","REPORT_TBL_BASELINE with TBL_NAME #{tbl_name}")


   ### Memory Table
   elsif (cmd == "ENA_MEM_TBL")
      Osk::flight.send_cmd("CS","ENA_MEM_TBL")
   elsif (cmd == "DIS_MEM_TBL")
      Osk::flight.send_cmd("CS","DIS_MEM_TBL")
   elsif (cmd == "ENA_MEM_TBL_ENTRY")
      entry_id = ask_string("Enter memory table entry id (0..#{FswConfigParam::CS_MAX_NUM_MEMORY_TABLE_ENTRIES})")
      Osk::flight.send_cmd("CS","ENA_MEM_TBL_ENTRY with ENTRY_ID #{entry_id}")
   elsif (cmd == "DIS_MEM_TBL_ENTRY")
      entry_id = ask_string("Enter memory table entry id (0..#{FswConfigParam::CS_MAX_NUM_MEMORY_TABLE_ENTRIES})")
      Osk::flight.send_cmd("CS","DIS_MEM_TBL_ENTRY with ENTRY_ID #{entry_id}")
   elsif (cmd == "COMPUTE_MEM_BASELINE")
      entry_id = ask_string("Enter memory table entry id (0..#{FswConfigParam::CS_MAX_NUM_MEMORY_TABLE_ENTRIES})")
      Osk::flight.send_cmd("CS","COMPUTE_MEM_BASELINE with ENTRY_ID #{entry_id}")
   elsif (cmd == "REPORT_MEM_BASELINE")
      entry_id = ask_string("Enter memory table entry id (0..#{FswConfigParam::CS_MAX_NUM_MEMORY_TABLE_ENTRIES})")
      Osk::flight.send_cmd("CS","REPORT_MEM_BASELINE with ENTRY_ID #{entry_id}")
   elsif (cmd == "GET_MEM_TBL_ENTRY_ID")
      address = ask_string("Enter starting address","0x00000000")
      Osk::flight.send_cmd("CS","GET_MEM_TBL_ENTRY_ID with ADDRESS #{address}")

   ### EEPROM Table
   elsif (cmd == "ENA_EEPROM_TBL")
      Osk::flight.send_cmd("CS","ENA_EEPROM_TBL")
   elsif (cmd == "DIS_EEPROM_TBL")
      Osk::flight.send_cmd("CS","DIS_EEPROM_TBL")
   elsif (cmd == "ENA_EEPROM_TBL_ENTRY")
      entry_id = ask_string("Enter EEPROM table entry id (0..#{FswConfigParam::CS_MAX_NUM_EEPROM_TABLE_ENTRIES})")
      Osk::flight.send_cmd("CS","ENA_EEPROM_TBL_ENTRY with ENTRY_ID #{entry_id}")
   elsif (cmd == "DIS_EEPROM_TBL_ENTRY")
      entry_id = ask_string("Enter EEPROM table entry id (0..#{FswConfigParam::CS_MAX_NUM_EEPROM_TABLE_ENTRIES})")
      Osk::flight.send_cmd("CS","DIS_EEPROM_TBL_ENTRY with ENTRY_ID #{entry_id}")
   elsif (cmd == "COMPUTE_EEPROM_BASELINE")
      entry_id = ask_string("Enter EEPROM table entry id (0..#{FswConfigParam::CS_MAX_NUM_EEPROM_TABLE_ENTRIES})")
      Osk::flight.send_cmd("CS","COMPUTE_EEPROM_BASELINE with ENTRY_ID #{entry_id}")
   elsif (cmd == "REPORT_EEPROM_BASELINE")
      entry_id = ask_string("Enter EEPROM table entry id (0..#{FswConfigParam::CS_MAX_NUM_EEPROM_TABLE_ENTRIES})")
      Osk::flight.send_cmd("CS","REPORT_EEPROM_BASELINE with ENTRY_ID #{entry_id}")
   elsif (cmd == "GET_EEPROM_TBL_ENTRY_ID")
      address = ask_string("Enter starting address","0x00000000")
      Osk::flight.send_cmd("CS","GET_EEPROM_TBL_ENTRY_ID with ADDRESS #{address}")

   ### Operating System
   elsif (cmd == "ENA_OS")
      Osk::flight.send_cmd("CS","ENA_OS")
   elsif (cmd == "DIS_OS")
      Osk::flight.send_cmd("CS","DIS_OS")
   elsif (cmd == "COMPUTE_OS_BASELINE")
      Osk::flight.send_cmd("CS","COMPUTE_OS_BASELINE")
   elsif (cmd == "REPORT_OS_BASELINE")
      Osk::flight.send_cmd("CS","REPORT_OS_BASELINE")

   ### cFE Core
   elsif (cmd == "ENA_CFE_CORE")
      Osk::flight.send_cmd("CS","ENA_CFE_CORE")
   elsif (cmd == "DIS_CFE_CORE")
      Osk::flight.send_cmd("CS","DIS_CFE_CORE")
   elsif (cmd == "COMPUTE_CFE_CORE_BASELINE")
      Osk::flight.send_cmd("CS","COMPUTE_CFE_CORE_BASELINE")
   elsif (cmd == "REPORT_CFE_CORE_BASELINE")
      Osk::flight.send_cmd("CS","REPORT_CFE_CORE_BASELINE")

   ### Dump & Display Tables from multiple tabs
   elsif (cmd.include? "DUMP_")
      case cmd
      when "DUMP_APP_DEF_TBL"
         tbl_name = FswConfigParam::CS_APP_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_APP_TBL
      when "DUMP_EEPROM_DEF_TBL"
         tbl_name = FswConfigParam::CS_EEPROM_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_EEPROM_TBL
      when "DUMP_MEM_DEF_TBL"
         tbl_name = FswConfigParam::CS_MEMORY_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_MEMORY_TBL
      when "DUMP_TABLES_DEF_TBL"
         tbl_name = FswConfigParam::CS_TABLES_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_TABLES_TBL
      when "DUMP_APP_RES_TBL"
         tbl_name = FswConfigParam::CS_APP_RES_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_APP_RES_TBL
      when "DUMP_EEPROM_RES_TBL"
         tbl_name = FswConfigParam::CS_EEPROM_RES_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_EEPROM_RES_TBL
      when "DUMP_MEM_RES_TBL"
         tbl_name = FswConfigParam::CS_MEMORY_RES_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_MEMORY_RES_TBL
      when "DUMP_TABLES_RES_TBL"
         tbl_name = FswConfigParam::CS_TABLES_RES_TBL_NAME
         tbl_mgr_def_filename = Osk::TBL_MGR_DEF_CS_TABLES_RES_TBL
      else
         raise "Error in CS screen management definitions for #{cmd}. Screen text file and ruby scipt file don't agree on table dump options."
      end
      Osk::Ops::send_cfe_dump_tbl_cmd("CS.#{tbl_name}", tbl_mgr_def_filename)

   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to cs_mgmt_send_cmd()"
   end
  
end # cs_mgmt_send_cmd()
