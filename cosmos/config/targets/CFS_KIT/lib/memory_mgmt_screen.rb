###############################################################################
# cFS Kit Memory Management 
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

def memory_mgmt_send_cmd(screen, cmd)

	if (cmd == "LOOKUP_SYMBOL")
      symbol_name = ask_string("Enter flight software symbol name")
      Osk::flight.send_cmd("MM","LOOKUP_SYMBOL with SYMBOL_NAME #{symbol_name}")
	elsif (cmd == "PEEK_MEM")
      mem_type = ask_string("Enter memory type (0=None, 1=RAM, 2=EEPROM")
      data_size = ask_string("Enter data size in bits (8, 16, 32)")
      addr_symbol_name = ask_string("Enter symbol to be used as base address or empty string if no symbol")
      addr_offset = ask_string("Enter offset from symbol or absolute address if no symbol specified")
      Osk::flight.send_cmd("MM","PEEK_MEM with DATA_SIZE #{data_size}, MEM_TYPE #{mem_type}, ADDR_OFFSET #{addr_offset}, ADDR_SYMBOL_NAME #{addr_symbol_name}")
	elsif (cmd == "POKE_MEM")
      mem_type = ask_string("Enter memory type (0=None, 1=RAM, 2=EEPROM")
      data_size = ask_string("Enter data size in bits (8, 16, 32)")
      addr_symbol_name = ask_string("Enter symbol to be used as base address or empty string if no symbol")
      addr_offset = ask_string("Enter offset from symbol or absolute address if no symbol specified")
      data = ask_string("Enter data to be poked (loaded) into the address")
      Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE #{data_size}, MEM_TYPE #{mem_type}, DATA #{data}, ADDR_OFFSET #{addr_offset}, ADDR_SYMBOL_NAME #{addr_symbol_name}")
	elsif (cmd == "DUMP_TO_EVENT")
      mem_type = ask_string("Enter memory type (0=None, 1=RAM, 2=EEPROM")
      num_bytes = ask_string("Enter number of bytes to dump (Max 15)")
      addr_symbol_name = ask_string("Enter symbol to be used as base address or empty string if no symbol")
      addr_offset = ask_string("Enter offset from symbol or absolute address if no symbol specified")
      Osk::flight.send_cmd("MM","DUMP_IN_EVENT with MEM_TYPE #{mem_type}, NUM_BYTES #{num_bytes}, ADDR_OFFSET #{addr_offset}, ADDR_SYMBOL_NAME #{addr_symbol_name}")
	elsif (cmd == "FILL_BLOCK")
      mem_type = ask_string("Enter memory type (0=None, 1=RAM, 2=EEPROM")
      fill_pattern = ask_string("Enter fill pattern (Max 4 byte pattern)")
      num_bytes = ask_string("Enter number of bytes to fill")
      addr_symbol_name = ask_string("Enter symbol to be used as base address or empty string if no symbol")
      addr_offset = ask_string("Enter offset from symbol or absolute address if no symbol specified")
      Osk::flight.send_cmd("MM","FILL_MEM with MEM_TYPE #{mem_type}, NUM_BYTES #{num_bytes}, FILL_PATTERN #{fill_pattern}, ADDR_OFFSET #{addr_offset}, ADDR_SYMBOL_NAME #{addr_symbol_name}")
	elsif (cmd == "LOAD_FROM_FILE")
      put_file = combo_box("Transfer file from ground to flight?", Osk::MSG_BUTTON_YES,Osk::MSG_BUTTON_NO)
      if (put_file == Osk::MSG_BUTTON_YES)
         Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_DIR, Osk::FLT_SRV_DIR)
         if (Osk::Ops::put_flt_file_prompt(Osk::GND_SRV_DIR))
            Osk::Ops::set_work_dir_widget(screen)
         end
      end 
      mem_path_filename = ask_string("Enter full FSW /path/filename of memory load file","#{Osk::Ops::flt_path_filename}")
      Osk::flight.send_cmd("MM","LOAD_MEM_FROM_FILE with FILENAME #{mem_path_filename}")
	elsif (cmd == "DUMP_TO_FILE")    
      mem_type = ask_string("Enter memory type (0=None, 1=RAM, 2=EEPROM")
      num_bytes = ask_string("Enter number of bytes to dump")
      addr_symbol_name = ask_string("Enter symbol to be used as base address or empty string if no symbol")
      addr_offset = ask_string("Enter offset from symbol or absolute address if no symbol specified")
      mem_path_filename = ask_string("Enter full FSW /path/filename file to received the dump")
      Osk::Ops::send_flt_bin_file_cmd("MM", "DUMP_MEM_TO_FILE with MEM_TYPE #{mem_type}, NUM_BYTES #{num_bytes}, ADDR_OFFSET #{addr_offset}, ADDR_SYMBOL_NAME #{addr_symbol_name}, ", Osk::TBL_MGR_DEF_FM_DMP)
	elsif (cmd == "START_DWELL")
      table_mask  = ask_string("Enter table ID bit mask with bit set to 1 to start a dwell table")
      Osk::flight.send_cmd("MD","START_DWELL with TABLE_MASK #{table_mask}")  
   elsif (cmd == "STOP_DWELL")
      table_mask  = ask_string("Enter table ID bit mask with bit set to 1 to stop a dwellctable")
      Osk::flight.send_cmd("MD","STOP_DWELL with TABLE_MASK #{table_mask}")  
   elsif (cmd == "JAM_DWELL_TBL")
      table_id  = ask_string("Enter Table ID (1..4)")
      entry_id  = ask_string("Enter Table Index (1..25)")
      field_len = ask_string("Enter field length in bytes (1..4)")
      interval  = ask_string("Enter MD execution intervals between dwell data collection")
      addr_symbol_name = ask_string("Enter symbol to be used as base address or empty string if no symbol")
      addr_offset = ask_string("Enter offset from symbol or absolute address if no symbol specified")
      Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID #{table_id}, ENTRY_ID #{entry_id}, FIELD_LEN #{field_len}, ADDR_OFFSET #{addr_offset}, DELAY #{interval}, ADDR_SYMBOL_NAME #{addr_symbol_name}")
   elsif (cmd == "DWELL_TLM_PKT")
      display("MD DWELL_PKT_SCREEN",50,50)
	elsif (cmd == "GET_FILE")
      # Update screen with working directories to help remind user where tables are stored
      Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_DIR, Osk::FLT_SRV_DIR)
      if (Osk::Ops::get_flt_file_prompt(Osk::GND_SRV_DIR))
         Osk::Ops::set_work_dir_widget(screen)
      end
   elsif (cmd == "PUT_FILE")
      # Update screen with working directories to help remind user where tables are stored
      Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_DIR, Osk::FLT_SRV_DIR)
      if (Osk::Ops::put_flt_file_prompt(Osk::GND_SRV_DIR))
         Osk::Ops::set_work_dir_widget(screen)
      end
   elsif (cmd == "TODO")
      prompt(Osk::MSG_TBD_FEATURE)
   else
      raise "Error in screen definition file. Undefined command sent to memory_mgmt_send_cmd()"
   end
  
end # memory_mgmt_send_cmd()

