###############################################################################
# OCK C Demo Ops Example Script
#
# This script demostrates how the OSK C Demo app can be used in an operational
# script. It's for illustrative purposes to show how to use an app's command and
# telemetry interface in a script. OSK C Demo does not have pratcical operational
# functionality. 
#
# Notes:
#   1. See osk_c_demo_func_test.rb for an example functional test.
#   2. OSK provides a scripting environment on top of COSMOS that perform
#      higher level functionality like verifying a valid command counter
#      increments after a command is processed.
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'osk_global'
require 'osk_system'
require 'osk_ops'

require 'osk_c_demo_const'


######################
## Global Constants ##
######################

FLT_MSGLOG_TBL_LOAD_FILE = "#{Osk::FLT_SRV_DIR}/#{OskCDemo::MSGLOG_TBL_LOAD_FILE}"
FLT_MSGLOG_TBL_DUMP_FILE = "#{Osk::FLT_SRV_DIR}/#{OskCDemo::MSGLOG_TBL_DUMP_FILE}"
GND_MSGLOG_TBL_DUMP_FILE = File.join(Osk::GND_SRV_TBL_DIR, OskCDemo::MSGLOG_TBL_DUMP_FILE)


#######################
## Utility Functions ##
#######################

#
# Get the message log table from FSW, create and return a JSON hash
# No reports on errors since this is a simple demo
#
def get_msglog_tbl 

   msglog_tbl_hash = nil
 
   if (Osk::flight.send_cmd("OSK_C_DEMO", "DUMP_TBL with ID #{OskCDemo::MSGLOG_TBL_ID}, FILENAME #{FLT_MSGLOG_TBL_DUMP_FILE}"))

      if (Osk::system.file_transfer.get(FLT_MSGLOG_TBL_DUMP_FILE,GND_MSGLOG_TBL_DUMP_FILE))
       
         tbl_file = File.read(GND_MSGLOG_TBL_DUMP_FILE)
         msglog_tbl_hash = JSON.parse(tbl_file)
      
      end      
   end

   return msglog_tbl_hash
   
end # get_msglog_tbl()


#
# Use dump file as the load file
# No reports on errors since this is a simple demo
#
def put_msglog_tbl(json_hash) 

   File.open(GND_MSGLOG_TBL_DUMP_FILE,"w") do |f| 
      f.write(JSON.pretty_generate(json_hash))
      f.write("\n") 
   end
   
   if (Osk::system.file_transfer.put(GND_MSGLOG_TBL_DUMP_FILE,FLT_MSGLOG_TBL_DUMP_FILE))

      Osk::flight.send_cmd("OSK_C_DEMO", "LOAD_TBL with ID #{OskCDemo::MSGLOG_TBL_ID}, FILENAME #{FLT_MSGLOG_TBL_DUMP_FILE}")

   end
   
end # put_msglog_tbl()


#
# Create a message log file then play it back and print the contents of
# each playback telemetry packet
#
def msg_log_and_playback(file_entry_cnt, playbk_delay)

   puts "msg_log_and_playback(#{file_entry_cnt}, #{playbk_delay})\n"

   Osk::flight.send_cmd("OSK_C_DEMO","START_LOG with MSG_ID #{Fsw::MsgId::CFE_EVS_HK_TLM_MID}")

   wait("OSK_C_DEMO HK_TLM_PKT MSG_LOG_ENA == 'TRUE'",  10)
   wait("OSK_C_DEMO HK_TLM_PKT MSG_LOG_ENA == 'FALSE'", file_entry_cnt*4+5) # Assume 4 seconds HK rate and add padding  

   msg_log_cnt = tlm("OSK_C_DEMO HK_TLM_PKT MSG_LOG_CNT")
   puts "Message log created with #{msg_log_cnt} entries\n"

   Osk::flight.send_cmd("OSK_C_DEMO","START_PLAYBK")
   wait 2 # Wait for first playback packet
   
   while (msg_log_cnt > 0)

      log_file_entry  = tlm("OSK_C_DEMO PLAYBK_TLM_PKT LOG_FILE_ENTRY")
      msg_hdr_hex_txt = tlm("OSK_C_DEMO PLAYBK_TLM_PKT MSG_HDR_HEX_TXT")
      puts "MsgLog Playback[#{log_file_entry}] = #{msg_hdr_hex_txt}\n" 

      seq_cnt = tlm("OSK_C_DEMO PLAYBK_TLM_PKT #{Ccsds::PRI_HDR_SEQUENCE}")
      wait("OSK_C_DEMO PLAYBK_TLM_PKT #{Ccsds::PRI_HDR_SEQUENCE} != #{seq_cnt}", playbk_delay+2)

      msg_log_cnt -= 1
      
   end
   
   Osk::flight.send_cmd("OSK_C_DEMO","STOP_PLAYBK")

end # msg_log_and_playback()

###########
## SETUP ##
###########

# Ensure command counter verification is enabled & save the current state
# so it can be restored at the end of the script

saved_validate_state = FswApp.validate_cmd?
FswApp.validate_cmd(true) 

# Simple test to verify app is loaded and running

Osk::flight.send_cmd("OSK_C_DEMO", "NOOP")  


##################################
## Default Message Log/Playback ##
##################################

msglog_tbl      = get_msglog_tbl
msglog_tbl_file = msglog_tbl["file"]
msglog_tbl_file_entry_cnt = msglog_tbl_file["entry-cnt"].to_i
msglog_tbl_playbk_delay   = msglog_tbl["playbk-delay"].to_i

msg_log_and_playback(msglog_tbl_file_entry_cnt, msglog_tbl_playbk_delay)

###################################
## Modified Message Log/Playback ##
###################################

msglog_tbl_file["entry-cnt"] = 7
msglog_tbl["playbk-delay"]   = 5
put_msglog_tbl(msglog_tbl)

msg_log_and_playback(msglog_tbl_file["entry-cnt"], msglog_tbl["playbk-delay"])


##############
## Clean Up ##
##############

# Restore table defaults
Osk::flight.send_cmd("OSK_C_DEMO", "LOAD_TBL with ID #{OskCDemo::MSGLOG_TBL_ID}, FILENAME #{FLT_MSGLOG_TBL_LOAD_FILE}")

msglog_filename = tlm("OSK_C_DEMO HK_TLM_PKT MSGLOG_FILENAME")
Osk::flight.send_cmd("FILEMGR", "DELETE_FILE with FILENAME #{msglog_filename}")

FswApp.validate_cmd(saved_validate_state) 
