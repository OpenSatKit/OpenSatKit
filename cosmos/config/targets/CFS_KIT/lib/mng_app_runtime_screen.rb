###############################################################################
# Kit Application Runtime Screen Scripts 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General
#   Public License (GPL).
# 
###############################################################################

require 'cosmos'
require 'cosmos/script'
Cosmos.catch_fatal_exception do
  require 'cosmos/tools/cmd_sender/cmd_sender'
  require 'cosmos/tools/tlm_viewer/screen'
  require 'cosmos/tools/tlm_viewer/tlm_viewer'
end

require 'osk_global'
require 'osk_system'
require 'osk_flight'


################################################################################
## Global Variables
################################################################################


# Use different file name than FSW default name so it doesn't get overwritten 

MSG_TBL_ID = 0
SCH_TBL_ID = 1

MSG_TBL_DMP_FILE = "osk_sch_msg_tbl~.json"
SCH_TBL_DMP_FILE = "osk_sch_sch_tbl~.json"

FLT_MSG_TBL_DMP_FILE = "#{Osk::FLT_SRV_DIR}/#{MSG_TBL_DMP_FILE}"
GND_MSG_TBL_DMP_FILE = "#{Osk::GND_SRV_TBL_DIR}/#{MSG_TBL_DMP_FILE}"

FLT_SCH_TBL_DMP_FILE = "#{Osk::FLT_SRV_DIR}/#{SCH_TBL_DMP_FILE}"
GND_SCH_TBL_DMP_FILE = "#{Osk::GND_SRV_TBL_DIR}/#{SCH_TBL_DMP_FILE}"

GND_SCH_SUM_FILE = "#{Osk::GND_SRV_TBL_DIR}/kit_sch_sum.txt"
GND_SCH_MSG_FILE = "#{Osk::GND_SRV_TBL_DIR}/kit_sch_msg.txt"

################################################################################
## Screen Commands
################################################################################

def mng_app_runtime(screen, cmd)

   if (cmd == "DISPLAY_MSG")
      create_kit_sch_msg_file
   elsif (cmd == "LOAD_MSG_TBL_ENTRY")
      Osk::Ops::send_flt_cmd("KIT_SCH", "LOAD_MSG_TBL_ENTRY")
   elsif (cmd == "SEND_MSG_TBL_ENTRY")
      Osk::Ops::send_flt_cmd("KIT_SCH", "SEND_MSG_TBL_ENTRY")
   elsif (cmd == "DISPLAY_SCH")
      create_kit_sch_sum_file
   elsif (cmd == "CFG_SCH_TBL_ENTRY")
      Osk::Ops::send_flt_cmd("KIT_SCH", "CFG_SCH_TBL_ENTRY")
   elsif (cmd == "LOAD_SCH_TBL_ENTRY")
      Osk::Ops::send_flt_cmd("KIT_SCH", "LOAD_SCH_TBL_ENTRY")
   elsif (cmd == "SEND_SCH_TBL_ENTRY")
      Osk::Ops::send_flt_cmd("KIT_SCH", "SEND_SCH_TBL_ENTRY")
   elsif (cmd == "TO_ADD_PKT")
      Osk::Ops::send_flt_cmd("KIT_TO", "ADD_PKT")
   elsif (cmd == "TO_REMOVE_PKT")
      Osk::Ops::send_flt_cmd("KIT_TO", "REMOVE_PKT")
   elsif (cmd == "TO_SEND_PKT_TLM")
      Osk::Ops::send_flt_cmd("KIT_TO", "SEND_PKT_TLM")
   elsif (cmd == "TO_UPDATE_FILTER")
      Osk::Ops::send_flt_cmd("KIT_TO", "UPDATE_FILTER")
   elsif (cmd == "MANAGE_JSON_TBL")
      tbl_scr_name = File.basename(Osk::JSON_TBL_MGMT_SCR_FILE, '.txt').upcase
      display("CFS_KIT #{tbl_scr_name}",50,50)
   elsif (cmd == "KIT_SCH_TBL_ENTRY_TLM")
      Cosmos.run_process("ruby tools/PacketViewer -p \"KIT_SCH TBL_ENTRY_TLM_PKT\"")
   elsif (cmd == "CONFIG_EDITOR")
      spawn("ruby #{Osk::COSMOS_CFG_EDITOR}")
   else
      prompt("Error in screen definition file. Undefined command sent to mng_app_runtime()")
   end
   
end # mng_app_runtime()


################################################################################
## Create kit_sch table summary file
################################################################################

def create_kit_sch_sum_file()

   # File transfers can time out so disable check. Other checks will catch file issues
   saved_validate_state = FswApp.validate_cmd?
   FswApp.validate_cmd(false)
   
   # 1. Create hash from message table

   if ( not Osk::flight.send_cmd("KIT_SCH","DUMP_TBL with ID #{MSG_TBL_ID}, FILENAME #{FLT_MSG_TBL_DMP_FILE}") )
      prompt ("KIT_SCH dump message table command failed.");
      FswApp.validate_cmd(saved_validate_state)
      return
   end
   
   wait 2
   
   if (Osk::system.file_transfer.get(FLT_MSG_TBL_DMP_FILE,GND_MSG_TBL_DMP_FILE))
      tbl_file = File.read(GND_MSG_TBL_DMP_FILE)
      msg_tbl_hash = JSON.parse(tbl_file)
      #puts msg_tbl_hash
      msg_array = msg_tbl_hash["message-array"]
      #puts msg_array
      # Each array entry is a hash
      #msg_array.each do |msg|
	   #   puts "msg["  + msg["message"]["id"].to_s + "] = " + "0x%04X" % msg["message"]["stream-id"] + "\n"
      #   #puts msg["message"]
      #end
   else
      prompt("Failed to transfer flight file #{FLT_MSG_TBL_DMP_FILE} to ground file #{GND_MSG_TBL_DMP_FILE}")
      FswApp.validate_cmd(saved_validate_state)
	   return
	end

   # 2. Create hash from scheduler table

   if ( not Osk::flight.send_cmd("KIT_SCH","DUMP_TBL with ID #{SCH_TBL_ID}, FILENAME #{FLT_SCH_TBL_DMP_FILE}") )
      prompt ("KIT_SCH dump scheduler table command failed");
      FswApp.validate_cmd(saved_validate_state)
      return
   end
   
   if (Osk::system.file_transfer.get(FLT_SCH_TBL_DMP_FILE,GND_SCH_TBL_DMP_FILE))
      tbl_file = File.read(GND_SCH_TBL_DMP_FILE)
      sch_tbl_hash = JSON.parse(tbl_file)
      #puts sch_tbl_hash
      #slot_array = sch_tbl_hash["slot-array"]
      #slot_array.each do |slot|
      #   puts slot
      #   puts "\n"
      #   puts slot["slot"]["index"].to_s + "\n"
      #   activity_array = slot["slot"]["activity-array"]
      #   activity_array.each do |activity|
      #      #puts activity
      #      #puts "\n"
      #      puts activity["activity"]["index"].to_s     + "\n"
      #      puts activity["activity"]["enable"]         + "\n"
      #      puts activity["activity"]["frequency"].to_s + "\n"
      #      puts activity["activity"]["offset"].to_s    + "\n"
      #      puts activity["activity"]["msg-id"].to_s    + "\n"
      #   end # Activity Array
      #end # Slot Array
   else
      prompt("Failed to transfer flight file #{FLT_SCH_TBL_DMP_FILE} to ground file #{GND_SCH_TBL_DMP_FILE}")
      FswApp.validate_cmd(saved_validate_state)
	   return
	end
   
   # 3. Create summary file & display it
   
   File.open("#{GND_SCH_SUM_FILE}","w") do |f| 
      f.write ("Slot Activity Enabled Frequency Offset MsgTblIdx StreamId\n")
      slot_array = sch_tbl_hash["slot-array"]
      slot_array.each do |slot|
         activity_array = slot["slot"]["activity-array"]
         activity_array.each do |activity|
            msg_id = activity["activity"]["msg-id"]
            f.write(" %2d  "      % slot["slot"]["index"])
            f.write("  %2d    "   % activity["activity"]["index"])
            f.write("  %-5s  "    % activity["activity"]["enabled"])
            f.write("   %2d     " % activity["activity"]["period"])
            f.write("  %2d   "    % activity["activity"]["offset"])
            f.write("   %3d     " % msg_id)
            if msg_array[msg_id].nil?
               f.write("  nil\n")
            else
               f.write("0x%04X" % msg_array[msg_id]["message"]["stream-id"] + "\n")
            end
         end # Activity Array
         f.write("---------------------------------------------------------\n")         
      end # Slot Array
   end # File.open
   
   Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{GND_SCH_SUM_FILE}'")
   
   FswApp.validate_cmd(saved_validate_state)

end # create_kit_sch_sum_file()

################################################################################
## Create kit_sch message table file
################################################################################

def create_kit_sch_msg_file()

   # File transfers can time out so disable check. Other checks will catch file issues
   saved_validate_state = FswApp.validate_cmd?
   FswApp.validate_cmd(false)

   # 1. Create hash from message table

   if ( not Osk::flight.send_cmd("KIT_SCH","DUMP_TBL with ID #{MSG_TBL_ID}, FILENAME #{FLT_MSG_TBL_DMP_FILE}") )
      prompt ("KIT_SCH dump message table command failed.");
      FswApp.validate_cmd(saved_validate_state)
      return
   end
   
   wait 2
   
   if (Osk::system.file_transfer.get(FLT_MSG_TBL_DMP_FILE,GND_MSG_TBL_DMP_FILE))
      tbl_file = File.read(GND_MSG_TBL_DMP_FILE)
      msg_tbl_hash = JSON.parse(tbl_file)
      msg_array = msg_tbl_hash["message-array"]
   else
      prompt("Failed to transfer flight file #{FLT_MSG_TBL_DMP_FILE} to ground file #{GND_MSG_TBL_DMP_FILE}")
      FswApp.validate_cmd(saved_validate_state)
	   return
	end

   # 2. Create summary file
   
   File.open("#{GND_SCH_MSG_FILE}","w") do |f| 
      f.write ("Index StreamId\n")
      msg_array.each do |msg|
         f.write(" %3d   " % msg["message"]["id"])
         f.write(" 0x%04X" % msg["message"]["stream-id"] + "\n")
      end
   end # File.open
   
   Cosmos.run_process("ruby lib/OskTxtFileViewer -f '#{GND_SCH_MSG_FILE}'")

   FswApp.validate_cmd(saved_validate_state)

end # create_kit_sch_msg_file()
