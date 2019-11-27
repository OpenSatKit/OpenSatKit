################################################################################
# cFS Kit Table Management Demo (TMD)
#
# Notes:
#   1. TMD_INSTRUCT_x variables are used to put text on the demo screen's
#      text box. The TMD_INFO_x variables are used for extra detailed
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
require 'osk_ops'
require 'fsw_config_param'

################################################################################
## Global Variables
################################################################################

TMD_TBL_FILE = "md_tbl1.dat"
TMD_TBL_PATH_FILE = "#{Osk::FLT_SRV_DIR}/#{TMD_TBL_FILE}"

TMD_TBL_NAME = "MD.DWELL_TABLE1"  # cFS table name
TMD_TBL_DEF_NAME = "md_tbl.txt"   # COSMOS table definition file

TMD_TMP_FLT_PATH_FILENAME = "#{Osk::FLT_SRV_DIR}/#{Osk::TMP_TBL_FILE}"
TMD_TMP_GND_PATH_FILENAME = "#{Osk::GND_SRV_TBL_DIR}/#{Osk::TMP_TBL_FILE}"

TMD_INITIAL_POKE_VAL = 32

################################################################################
## Demo Narrative Text
################################################################################

#
# 1 - Display table registry
# 2 - Configure MD table and poke values in locations being monitored
# 3 - Dump table to file and transfer file to ground
# 4 - Display table using COSMOS table manager tool
# 5 - Illustrate how MD table is monitoring memory locations
# 6 - Modify dwell table and load it to show new behavior
#

# Instruct 0 text is in the file demo screen text file.  It's here to help with the info text.
TMD_INSTRUCT_0 = ["This demo shows basic table management features using Memory Dwell app's (MD)",
                  "'dwell' table. Select <More Info> for a MD overview. The Trivial File Transport",
                  "Protocol (TFTP) app transfers table files between COSMOS and the cFS. Click...",
                  "",
                  "  <More Info> to obtain more information about the current step",
                  "  <Demo> to issue commands to demonstrate a feature in the current step",
                  "  <Next> to move to the next step"]
TMD_INFO_0 = "\n\
The Memory Dwell(MD) application creates a telemetry packet containing the contents of memory \
locations that are specified in its 'dwell' table. MD is typically used for in-orbit FSW \
maintenance or for telemetry changes that occur late in a spacecraft's development lifecycle \
when its undesirable to change a telemetry packet definition.\n\n\
\
This demo configures MD to monitor memory starting at symbol #{FswConfigParam::MM_SYMBOL} plus offset #{FswConfigParam::MM_OFFSET_W0}. \
These are known locations that can be safety altered during the demo. \
"

# 1 - Display table registry
TMD_INSTRUCT_1 = ["The table management screen was opened. This screen is a combination of ",
                  "information from the cfE's Table Service (TBL) Housekeeping telemetry packet",
                  "and TBL Service's table registry packet that is sent in response to a",
                  "SEND_REGISTRY command. ",
                  "",
                  "<Demo> Send TBL SEND_REGISTRY cmd with TABLENAME #{TMD_TBL_NAME}",
                  ""]
# Repeat step zero's info in case the user wasn't inspired to read more earlier
TMD_INFO_1 = "\n\
The Memory Dwell(MD) application creates a telemetry packet containing the contents of memory \
locations that are specified in its 'dwell' tables. MD is typically used for in-orbit FSW \
maintenance or for telemetry changes that occur late in a spacecraft's development lifecycle \
when its undesirable to change a telemetry packet definition.\n\n\
\
This demo configures MD to monitor memory starting at symbol #{FswConfigParam::MM_SYMBOL} plus offset #{FswConfigParam::MM_OFFSET_W0}. \
These are known locations that can be safety altered during the demo. \
"

# 2 - Configure MD table and poke values in locations being monitored
TMD_INSTRUCT_2 = ["Memory Dwell's telemetry screen was opened. The commands sent using <Demo>",
                  "will 'jam' MD's table with locations to monitor. You will see the location",
                  "values when MD's table is dumped and displayed.",
                  "",
                  "<Demo> Jam MD's table to monitor locations at symbol #{FswConfigParam::MM_SYMBOL}+#{FswConfigParam::MM_OFFSET_W0}",
                  "<Demo> Send Memory Manager cmds to write 1,2,3,4,... into monitored locations",
                  ""]
TMD_INFO_2 = "\n\
Jamming is a cFS term that means changing a table's content without using the normal table load \
process. A ground command is the typical method for jamming a table. If an application autonomously \
updates its table then the portion of table that is auonomously updated should probably be a dump-only \
table or using a table may not be the best design choice. \n\n\
\
Memory Manager (MM) is an app that provides a ground interface for directly reading/writing memory locations. \
It is typically used for testing and on-orbit maintenance. This demo uses the 'poke' command that writes \
command-supplied values into a specified location.  
"

# 3 - Dump table to file and transfer file to ground
TMD_INSTRUCT_3 = ["Dump #{TMD_TBL_NAME} to a file and transfer it to the ground.",
                  "",
                  "",
                  "<Demo> Send TBL's DUMP_TBL cmd",
                  "<Demo> Send TFTP get cmd to transfer the file to ground",
                  "",
                  ""]
TMD_INFO_3 = Osk::DEMO_STEP_NO_INFO
                     

# 4 - Display table using COSMOS table manager tool
TMD_INSTRUCT_4 = ["Display the file using COSMOS Table Manager tool.",
                  "",
                  "<Demo> Launch Table Manager",
                  "",                  
                  "   1. On tool menu bar select File->Open->#{Osk::REL_SRV_DIR}",
                  "   2. In open dialog select 'table' folder, #{Osk::TMP_TBL_FILE}, and click 'Open'",
                  "   3. When prompted for table definition file select #{TMD_TBL_DEF_NAME}"]
TMD_INFO_4 = Osk::DEMO_STEP_NO_INFO

# 5 - Illustrate how MD table is monitoring memory locations
TMD_INSTRUCT_5 = ["Illustrate that memory dwell is reporting the value at location",
                  "#{FswConfigParam::MM_SYMBOL}+#{FswConfigParam::MM_OFFSET_W0} by sending a memory 'poke'",
                  "to change the value. The poke value starts at #{TMD_INITIAL_POKE_VAL} and increments",
                  "each time the poke command is sent. Observe MD page and event messages.",
                  "",
                  "<Demo> Send memory 'poke' command", 
                  ""]
TMD_INFO_5 = Osk::DEMO_STEP_NO_INFO

# 6 - Modify dwell table and load it to show new behavior
TMD_INSTRUCT_6 = ["In the Table Manager tool display change ENTRY0_ADDR_OFFSET from #{FswConfigParam::MM_OFFSET_W0} to",
                  "#{FswConfigParam::MM_OFFSET_W3} and hit <enter> to ensure the change sticks. From the menu",
                  "select File->Save to save changes. Use <Demo> to to transfer and load the new table.",
                  "",
                  "<Demo> Send TFTP put cmd to transfer the file to ground. Wait until complete before next <Demo> step",
                  "<Demo> Send table commands to load, validate, and activate the new table",
                  ""]
TMD_INFO_6 = Osk::DEMO_STEP_NO_INFO

TMD_INSTRUCT_ARRAY = [TMD_INSTRUCT_0, TMD_INSTRUCT_1, TMD_INSTRUCT_2, TMD_INSTRUCT_3, TMD_INSTRUCT_4, TMD_INSTRUCT_5, TMD_INSTRUCT_6]
TMD_LAST_STEP = TMD_INSTRUCT_ARRAY.length - 1

def tmd_set_instruct_text(num)

   new_instruct = TMD_INSTRUCT_ARRAY[num]
  
   $instruct1.text = new_instruct[0]
   $instruct2.text = new_instruct[1]
   $instruct3.text = new_instruct[2]
   $instruct4.text = new_instruct[3]
   $instruct5.text = new_instruct[4]
   $instruct6.text = new_instruct[5]
   $instruct7.text = new_instruct[6]

end # tmd_set_instruct_text()


################################################################################
## Demo Flow Control
################################################################################

$file_xfer = nil

$tmd_step = 0   # Demo step incremented by Next button
$tmd_demo = 0   # Issue command(s) to perform the current demo step

$tmd_poke_val = TMD_INITIAL_POKE_VAL

def table_mgmt_demo(screen, button)

   $instruct1  = screen.get_named_widget("Instruct1")
   $instruct2  = screen.get_named_widget("Instruct2")
   $instruct3  = screen.get_named_widget("Instruct3")
   $instruct4  = screen.get_named_widget("Instruct4")
   $instruct5  = screen.get_named_widget("Instruct5")
   $instruct6  = screen.get_named_widget("Instruct6")
   $instruct7  = screen.get_named_widget("Instruct7")

   if (button == "INFO")
  
      display("CFS_KIT TABLE_MGMT_DEMO_INFO_SCREEN",500,50)    

   elsif (button == "NEXT")
  
      $tmd_step += 1
      $tmd_demo = 0
        
      if ($tmd_step <= TMD_LAST_STEP)
         tmd_set_instruct_text($tmd_step)
      end

      case $tmd_step
         when 1
            display("CFS_KIT TABLE_MGMT_SCREEN",1500,50)    
            cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME CFE_TBL, BITMASK 0x01") # Enable debug events
         when 2
            display("MD DWELL_PKT_SCREEN",50,50)  
         when 3..TMD_LAST_STEP
            # Nothing to do 
         else
            cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME CFE_TBL, BITMASK 0x01") # Disable debug events
            $tmd_step = 0
            clear("CFS_KIT TABLE_MGMT_SCREEN")
            clear("CFS_KIT TABLE_MGMT_DEMO_SCREEN")
            clear("CFS_KIT TABLE_MGMT_DEMO_INFO_SCREEN")
            clear("MD DWELL_PKT_SCREEN")  
            message_box("This concludes the demo",false)
         end # Step Case
  
   elsif (button == "DEMO")
  
      case $tmd_step

         # 1 - Send table registry cmd
         when 1
            if ($tmd_demo == 0)
               Osk::flight.cfe_tbl.send_cmd("SEND_REGISTRY with TABLE_NAME #{TMD_TBL_NAME}")
               $tmd_demo += 1
            end

         # 2 - Configure MD table
         when 2
            if ($tmd_demo == 0)
               
               Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 1, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W0}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
               wait 1
               Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 2, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W1}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
               wait 1
               Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 3, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W2}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
               wait 1
               Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 4, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W3}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
               wait 1
               Osk::flight.send_cmd("MD","SET_SIGNATURE with TABLE_ID 1, PAD_16 0, SIGNATURE 'Table  Demo'")
               wait 1   
                  
               $tmd_demo += 1
            
            elsif ($tmd_demo == 1)

               Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x01020304, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W0}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
               wait 1
               Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x05060708, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W1}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
               wait 1
               Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0X090A0B0C, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W2}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
               wait 1
               Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x0D0E0F10, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W3}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
               wait 1
               Osk::flight.send_cmd("MD","START_DWELL with TABLE_MASK 0x0001")

               $tmd_demo += 1
            
            end

         # 3 - Dump table to file & transfer to the ground
         when 3
            if ($tmd_demo == 0)
               
               #TODO - Necessary? FswApp.validate_cmd(true)
               if (Osk::flight.cfe_tbl.send_cmd("DUMP_TBL with ACTIVE_TBL_FLAG 1, TABLE_NAME #{TMD_TBL_NAME}, FILENAME #{TMD_TMP_FLT_PATH_FILENAME}"))
                  $tmd_demo += 1
               end

            elsif ($tmd_demo == 1)
               Osk::system.file_transfer.get(TMD_TMP_FLT_PATH_FILENAME,TMD_TMP_GND_PATH_FILENAME)
               # Don't increment tmd_demo; okay if user repeatedly sends the command
            end

         # 4 - Display table using the Table Manager tool 
         when 4
            if ($tmd_demo == 0)
               Cosmos.run_process("ruby tools/TableManager")
               $tmd_demo += 1
            end

         # 5 - Illustrate how MD table is monitoring memory locations
         when 5
            if ($tmd_demo == 0)
            
               Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 8, MEM_TYPE 1, DATA #{$tmd_poke_val}, ADDR_OFFSET #{FswConfigParam::MM_OFFSET_W0}, ADDR_SYMBOL_NAME #{FswConfigParam::MM_SYMBOL}")
               $tmd_poke_val += 1
               # Don't increment tmd_demo so user can repeatedly sends the command
            end

         # 6 - Modify dwell table and load it to show new behavior
         when 6
            if ($tmd_demo == 0)
               if (Osk::system.file_transfer.put(TMD_TMP_GND_PATH_FILENAME,TMD_TMP_FLT_PATH_FILENAME))
                  $tmd_demo += 1
               end
            elsif ($tmd_demo == 1)
               if (Osk::flight.cfe_tbl.send_cmd("LOAD_TBL with FILENAME #{TMD_TMP_FLT_PATH_FILENAME}"))
                  wait 2
                  if (Osk::flight.cfe_tbl.send_cmd("VALIDATE_TBL with ACTIVE_TBL_FLAG 0, TABLE_NAME #{TMD_TBL_NAME}"))
                     wait 2
                     if (Osk::flight.cfe_tbl.send_cmd("ACTIVATE_TBL with TABLE_NAME #{TMD_TBL_NAME}"))
                        $tmd_demo += 1
                     end
                  end
               end
            end
            
         end # Step Case

   end # Demo button
 
end # table_mgmt_demo_main()


def table_mgmt_demo_more_info(screen)
   
   msg_widget = screen.get_named_widget("msg_box")
   case $tmd_step
      when 0
         msg_widget.text = TMD_INFO_0
      when 1
         msg_widget.text = TMD_INFO_1
      when 2
         msg_widget.text = TMD_INFO_2
      when 3
         msg_widget.text = TMD_INFO_3
      when 4
         msg_widget.text = TMD_INFO_4
      when 5
         msg_widget.text = TMD_INFO_5
      else
         msg_widget.text = Osk::DEMO_STEP_NO_INFO
      end # Case
    
end # table_mgmt_demo_more_info()
