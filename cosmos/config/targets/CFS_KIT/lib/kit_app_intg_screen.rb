###############################################################################
# Kit Application Integration Screen Scripts 
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

# TODO - Create OSK global or app base class for ask_app_fw definitions
OSK_TBL_REPLACE    = 0
OSK_TBL_UPDATE_REC = 1

# Use different file name than FSW default name so it doesn't get overwritten 

MSG_TBL_ID = 0
SCH_TBL_ID = 1

TMP_TBL_FILE = "osk_tmp_tbl.json"

MSG_TBL_DMP_FILE = "kit_sch_msg~.json"
SCH_TBL_DMP_FILE = "kit_sch_sch~.json"

FLT_TMP_TBL_FILE = "#{Osk::FLT_SRV_DIR}/#{TMP_TBL_FILE}"
GND_TMP_TBL_FILE = "#{Osk::GND_SRV_TBL_DIR}/#{TMP_TBL_FILE}"

FLT_MSG_TBL_DMP_FILE = "#{Osk::FLT_SRV_DIR}/#{MSG_TBL_DMP_FILE}"
GND_MSG_TBL_DMP_FILE = "#{Osk::GND_SRV_TBL_DIR}/#{MSG_TBL_DMP_FILE}"

FLT_SCH_TBL_DMP_FILE = "#{Osk::FLT_SRV_DIR}/#{SCH_TBL_DMP_FILE}"
GND_SCH_TBL_DMP_FILE = "#{Osk::GND_SRV_TBL_DIR}/#{SCH_TBL_DMP_FILE}"

GND_DBG_TBL_FILE = "#{Osk::GND_SRV_TBL_DIR}/dbg_tbl.txt"

################################################################################
## Screen Commands
################################################################################

def kit_app_intg(screen, cmd)

   if (cmd == "CONFIG_EDITOR")
      spawn("ruby #{Osk::COSMOS_CFG_EDITOR}")
   elsif (cmd == "TO_ADD_PKT")
      Osk::Ops::send_flt_cmd("KIT_TO", "ADD_PKT")
   elsif (cmd == "TO_REMOVE_PKT")
      Osk::Ops::send_flt_cmd("KIT_TO", "REMOVE_PKT")
   elsif (cmd == "SCH_CFG_ENTRY")
      Osk::Ops::send_flt_cmd("KIT_SCH", "CFG_SCH_ENTRY")
   elsif (cmd == "SCH_SUM_FILE")
      create_kit_sch_sum_file
   else
      prompt("Error in screen definition file. Undefined command sent to kit_app_intg()")
   end
   
end # kit_app_intg()


################################################################################
## Create kit_sch table summary file
################################################################################

def create_kit_sch_sum_file()

         
   # 1. Create hash from message table

   if ( not Osk::flight.kit_sch.send_cmd("DUMP_TBL with ID #{MSG_TBL_ID}, TBL_FILENAME #{FLT_MSG_TBL_DMP_FILE}") )
      prompt ("KIT_SCH dump message table command failed.");
      return
   end
   
   if (Osk::system.file_transfer.get(FLT_MSG_TBL_DMP_FILE,GND_MSG_TBL_DMP_FILE))
      tbl_file = File.read(GND_MSG_TBL_DMP_FILE)
      msg_tbl_hash = JSON.parse(tbl_file)
      #puts msg_tbl_hash
   else
      prompt("Failed to transfer flight file #{FLT_MSG_TBL_DMP_FILE} to ground file #{GND_MSG_TBL_DMP_FILE}")
	   return
	end

   wait (5)
   
   # 2. Create hash from scheduler table

   if ( not Osk::flight.kit_sch.send_cmd("DUMP_TBL with ID #{SCH_TBL_ID}, TBL_FILENAME #{FLT_SCH_TBL_DMP_FILE}") )
      prompt ("KIT_SCH dump scheduler table command failed");
      return
   end
   
   if (Osk::system.file_transfer.get(FLT_SCH_TBL_DMP_FILE,GND_SCH_TBL_DMP_FILE))
      tbl_file = File.read(GND_SCH_TBL_DMP_FILE)
      sch_tbl_hash = JSON.parse(tbl_file)
      #puts sch_tbl_hash
   else
      prompt("Failed to transfer flight file #{FLT_SCH_TBL_DMP_FILE} to ground file #{GND_SCH_TBL_DMP_FILE}")
	   return
	end
   
   # 3. Create summary file
   
   File.open("#{GND_DBG_TBL_FILE}","w") do |f| 
      f.write ("Slot Activity Enabled Frequency Offset MsgTblIndx StreamId\n")
      sch_tbl_hash.each do |key, value|
	      puts key + "\n"
         #puts "   " + value + "\n"
	      f.write(key+"\n")
         #f.write("   "+value+"\n")
      end
   end
   #File.open("#{GND_TBL_FILE}","w") do |f| 
	#   f.write(JSON.pretty_generate(tbl_hash))
	#   f.write("\n")  # FSW JSMN tokenizer requires a newline after closing bracket
   #end

end # create_kit_sch_sum_file()

#   tbl_file = File.read(GND_SCH_TBL_DMP_FILE)
#   sch_tbl_hash = JSON.parse(tbl_file)
#   File.open("#{GND_DBG_TBL_FILE}","w") do |f| 
#      f.write ("Slot Activity Enabled Frequency Offset MsgTblIndx StreamId\n")
#      sch_tbl_hash.each do |key, value|
#	      puts key + "\n"
#         #puts "   " + value + "\n"
#	      #f.write(key+"\n")
#        #f.write("   " + value.class+"\n")
#     end
#     slots = sch_tbl_hash["slot"]
#     slots.each do |key, value|
#	      puts "Slot key: " + key + "\n"
#	      puts "Slot value: " + value.to_s + "\n"
#      end
#      activities = sch_tbl_hash["activity"]
#      activities.each do |key, value|
#	      puts "Act key: " + key + "\n"
#	      puts "Act value: " + value.to_s + "\n"
#      end
#   end
#return
