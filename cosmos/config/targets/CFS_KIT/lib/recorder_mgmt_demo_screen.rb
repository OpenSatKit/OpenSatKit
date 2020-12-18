################################################################################
# cFS Kit Recorder Management Demo (RMD)
#
# Notes:
#   1. RMD_INSTRUCT_x variables are used to put text on the demo screen's
#      instruction text box. The RMD_INFO_x variables are used for extra detailed
#      information that is displayed on a separate screen.
#   2. Debug events are enabled for the apps used during the demo.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
# 
################################################################################

require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'simsat_const'

RMD_EVENT_FILTER_TBL_IDX = 6
RMD_EVENT_FILE_TBL_IDX   = 0
RMD_HK_FILE_TBL_IDX      = 1

RMD_TBL_PATH_FILE = "#{Osk::FLT_SRV_DIR}/#{Osk::TMP_TBL_FILE}"

RMD_EVS_FILE = "events1000.dat"   # Assumes DS configured for 4 digit count in file name 
RMD_EVS_PATH_FILE = "#{SimSat::FLT_REC_DIR}/#{RMD_EVS_FILE}"

################################################################################
## Demo Narrative Text
################################################################################

#
# 1 - Dump packet filter and file table 
# 2 - Enable Event and HK files, Send noop events and update DS file info
# 3 - Stop recording, transfer and display event file 
#

# Instruct 0 text is in the file demo screen text file.  It's here to help with the info text.
RMD_INSTRUCT_0 = ["This demo illustrates some basic recorder management features. This demo concentrates",
                  "on the Data Storage(DS) app. Onboard recorder management is much more complicated and",
                  "involves multiple apps. Select <More Info> for a brief overview. Click...",
                  " ",
                  "  <More Info> to obtain more information about the current step",
                  "  <Demo> to issue commands to demonstrate a feature in the current step",
                  "  <Next> to move to the next step"
                  ]
              
RMD_INFO_0 = "\n\
DS accepts packets from the Software Bus(SB) and writes them to files. Files can be generated \
from other sources. Science payloads, cameras, etc. often have an app that manages their interface \
and creates data files. Files are also generated in response to commands (e.g. SB routing tables). \
These files are typically stored in a static directory structure.\n\n\
\
Each mission must design a file management scheme that includes when files are closed, how they are \
downlinked in a prioritized order, and when they are deleted.  The CCSDS File Data Protocol (CFDP) \
app (CF) includes features that help with these tasks.\
"

# 1 - Dump packet filter and file table 
RMD_INSTRUCT_1 = ["Two DS tables define its behavior. The 'filter' table defines which packets will be sent to",
                  "a file. This includes a packet filtering algorithm. The 'file' table defines how a file is",
                  "named and when it should be closed.",
                  " ",
                  "<Demo> Dump/display DS 'filter' table. Note #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID} event msg ID's file table index is #{RMD_EVENT_FILTER_TBL_IDX}",
                  "<Demo> Dump/display DS 'file' table. Note [#{RMD_EVENT_FILE_TBL_IDX}]=Event filter, [#{RMD_HK_FILE_TBL_IDX}]=cFE Houekeeping",
                  " "]

RMD_INFO_1 = "\n\n\
Message #{Fsw::MsgId::CFE_EVS_EVENT_MSG_MID} is the event message ID and note that its file table index is #{RMD_EVENT_FILE_TBL_IDX}. \
Defining an event message file is a common mission design pattern. The packet filter table passes every event \
message to the file.  The event file can be dumped at the start of a ground contact.\n\n\
\
The housekeeping (HK) app combines pieces of packets into a new packet. HK's default table is defined to combine \
the command counters from each cFE service app's housekeeping into a new combined cFS HK packet. DS's default \
table defines a file to store this packet. \ 
"


# 2 - Enable Event and Housekeeping files
RMD_INSTRUCT_2 = ["DS's file table defaults to disabled. After you enable the files, DS's file info packet",
                  "and FM's open file packet are requested and displayed. ",
                  " ",
                  "<Demo> Enable DS's event and cFE housekeeping files (takes a few seconds)",
                  "<Demo> Generates data to be stored by sending noop commands to each cFE service",
                  " ",
                  " "]
               
RMD_INFO_2 = "\n\n\
Defining an event message file is a common mission design pattern. The packet filter table passes every event \
message to the file.  The event file can be dumped at the start of a ground contact.\n\n\
\
The housekeeping (HK) app combines pieces of packets into a new packet. HK's default table is defined to combine \
the command counters from each cFE service app's housekeeping into a new combined cFS HK packet. DS's default \
table defines a file to store this packet. \ 
"

# 3 - Stop recording, transfer and display event file 
RMD_INSTRUCT_3 = ["When you perform the disable and close steps, DS's file info packet and FM's",
                  "open file packet are requested and displayed. ",
                  "",
                  "<Demo> Disable DS's generation of event and housekeeping files",
                  "<Demo> Close event and housekeeping files",
                  "<Demo> Transfer and display the event file",
                  ""]
RMD_INFO_3 = Osk::DEMO_STEP_NO_INFO


RMD_INSTRUCT_ARRAY = [RMD_INSTRUCT_0, RMD_INSTRUCT_1, RMD_INSTRUCT_2, RMD_INSTRUCT_3]
RMD_LAST_STEP = RMD_INSTRUCT_ARRAY.length - 1

def rmd_set_instruct_text(num)

   new_instruct = RMD_INSTRUCT_ARRAY[num]
  
   $instruct1.text = new_instruct[0]
   $instruct2.text = new_instruct[1]
   $instruct3.text = new_instruct[2]
   $instruct4.text = new_instruct[3]
   $instruct5.text = new_instruct[4]
   $instruct6.text = new_instruct[5]
   $instruct7.text = new_instruct[6]

end # rmd_set_instruct_text()

################################################################################
## Demo Flow Control
################################################################################

$rmd_step = 0   # Demo step incremented by Next button
$rmd_demo = 0   # Issue command(s) to perform the current demo step

def recorder_mgmt_demo(screen, button)

   $instruct1  = screen.get_named_widget("Instruct1")
   $instruct2  = screen.get_named_widget("Instruct2")
   $instruct3  = screen.get_named_widget("Instruct3")
   $instruct4  = screen.get_named_widget("Instruct4")
   $instruct5  = screen.get_named_widget("Instruct5")
   $instruct6  = screen.get_named_widget("Instruct6")
   $instruct7  = screen.get_named_widget("Instruct7")

   if (button == "INFO")
  
      display("CFS_KIT RECORDER_MGMT_DEMO_INFO_SCREEN",500,50)    

   elsif (button == "NEXT")
  
      $rmd_step += 1
      $rmd_demo  = 0
    
      if ($rmd_step <= RMD_LAST_STEP)
         rmd_set_instruct_text($rmd_step)
      end

      case $rmd_step
         when 1
            display("CFS_KIT RECORDER_MGMT_SCREEN",1500,50)    
            cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME DS, BITMASK 0x01") # Enable debug events
         when 2
            display("DS FILE_1_4_INFO_SCREEN")    
            display("FM OPEN_FILES_SCREEN")    
         when 3..RMD_LAST_STEP
            # Nothing to do
         else
            cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME DS, BITMASK 0x01") # Disable debug events
            $rmd_step = 0
            clear("CFS_KIT RECORDER_MGMT_SCREEN")    
            clear("CFS_KIT RECORDER_MGMT_DEMO_SCREEN")
            clear("CFS_KIT RECORDER_MGMT_DEMO_INFO_SCREEN")
            clear("DS FILE_1_4_INFO_SCREEN")    
            clear("FM OPEN_FILES_SCREEN")    
      end # Step Case
        
   elsif (button == "DEMO")
  
      case $rmd_step

         # 1 - Dump packet filter and file table 
         when 1
            case $rmd_demo
               when 0
                  cmd_str = "DUMP_TBL with ACTIVE_TBL_FLAG 1, TABLE_NAME DS.FILTER_TBL, "
                  Osk::Ops::send_flt_bin_file_cmd("CFE_TBL", cmd_str, "Definition file for #{Osk::TBL_MGR_DEF_DS_FILTER_TBL}", flt_path_filename: RMD_TBL_PATH_FILE, gnd_rel_path: Osk::REL_SRV_TBL_DIR)
                  $rmd_demo += 1
               when 1
                  cmd_str = "DUMP_TBL with ACTIVE_TBL_FLAG 1, TABLE_NAME DS.FILE_TBL, "
                  Osk::Ops::send_flt_bin_file_cmd("CFE_TBL", cmd_str, "Definition file for #{Osk::TBL_MGR_DEF_DS_FILE_TBL}", flt_path_filename: RMD_TBL_PATH_FILE, gnd_rel_path: Osk::REL_SRV_TBL_DIR)
                  $rmd_demo = 0
               else
                  $rmd_demo = 0
               end
      
         # 2 - Enable Event and HK files, Send noop events and update DS file info
         when 2
            case $rmd_demo
               when 0 
                  Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX #{RMD_EVENT_FILE_TBL_IDX}, FILE_STATE 1")
                  wait 2
                  Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX #{RMD_HK_FILE_TBL_IDX}, FILE_STATE 1")
                  wait 4
                  Osk::flight.send_cmd("DS","SEND_FILE_INFO")
                  wait 1
                  Osk::flight.send_cmd("FM","SEND_OPEN_FILES")
                  $rmd_demo += 1
               when 1      
                  # Wait 4 allows the user to see the noop event messages. A 20s loop may seem like a long time
                  # but when someone is learnig and there's a lot goingg on it's better to slow things down
                  create_data = Thread.new {
                     ["CFE_ES", "CFE_EVS", "CFE_SB", "CFE_TBL", "CFE_TIME"].each { |cfe_srv|
                        Osk::Ops::send_flt_cmd(cfe_srv, "#{Osk::CMD_STR_NOOP}")
                        sleep(4)
                        Osk::flight.send_cmd("DS","SEND_FILE_INFO")
                        sleep(1)
                     }       
                  } # End thread
                  wait 2                  
                  Osk::flight.send_cmd("FM","SEND_OPEN_FILES")
               else
                  $rmd_demo = 1
            end # Case $rmd_demo

         # 3 - Stop recording, transfer and display event file 
         when 3
            case $rmd_demo
               when 0 
                  Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX #{RMD_EVENT_FILE_TBL_IDX}, FILE_STATE 0")
                  wait 2
                  Osk::flight.send_cmd("DS","SET_FILE_STATE with FILE_TBL_IDX #{RMD_HK_FILE_TBL_IDX}, FILE_STATE 0")
                  wait 2
                  Osk::flight.send_cmd("DS","SEND_FILE_INFO")
                  wait 2
                  Osk::flight.send_cmd("FM","SEND_OPEN_FILES")
                  $rmd_demo += 1
               when 1 
                  Osk::flight.send_cmd("DS","CLOSE_ALL")
                  wait 2
                  Osk::flight.send_cmd("DS","SEND_FILE_INFO")
                  wait 1
                  Osk::flight.send_cmd("FM","SEND_OPEN_FILES")
                  $rmd_demo += 1
               when 2 
                  Osk::Ops::get_flt_bin_file(RMD_EVS_PATH_FILE, Osk::REL_SRV_DIR, RMD_EVS_FILE, Osk::TBL_MGR_DEF_DS_EVENT_LOG,25)
               else
                  $rmd_demo = 0
            end # Case $rmd_demo
      end # Step Case
  end # Demo
   
end # recorder_mgmt_demo()

def recorder_mgmt_demo_more_info(screen)
   
   msg_widget = screen.get_named_widget("msg_box")
   case $rmd_step
      when 0
         msg_widget.text = RMD_INFO_0
      when 1
         msg_widget.text = RMD_INFO_1
      when 2
         msg_widget.text = RMD_INFO_2
      when 3
         msg_widget.text = RMD_INFO_3
      when 4
         msg_widget.text = RMD_INFO_4
      else
         msg_widget.text = Osk::DEMO_STEP_NO_INFO
   end # Case
    
end # recorder_mgmt_demo_more_info()

