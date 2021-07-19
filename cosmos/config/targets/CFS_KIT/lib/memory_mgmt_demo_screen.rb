################################################################################
# cFS Kit Memory Management Demo (MMD)
#
# Notes:
#   1. MMD_INSTRUCT_x variables are used to put text on the demo screen's
#      instruction text box. The MMD_INFO_x variables are used for extra detailed
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

################################################################################
## Global Variables
################################################################################

MMD_PAT_ABCD   = 0x0A0B0C0D
MMD_PAT_1234   = 0x01020304
MMD_PAT_0A0B   = 0x0A0B
MMD_PAT_A0     = 0xA0
MMD_PAT_A0_STR = "0xA0"

# MM_AppData+256 is a safe area in MM's global data structure that can be used
# as a working buffer during the demo

MMD_SYMBOL = "MM_AppData"
MMD_OFFSET_W0 = 256
MMD_OFFSET_W1 = 260
MMD_OFFSET_W2 = 264
MMD_OFFSET_W3 = 268

MMD_DMP_FILE = "mm_demo_dmp.dat"

MMD_FLT_PATH_FILENAME = "#{Osk::FLT_SRV_DIR}/#{MMD_DMP_FILE}"
MMD_GND_PATH_FILENAME = "#{Osk::GND_SRV_DIR}/#{MMD_DMP_FILE}"

#MMD_B0_FILE = "mm_demo_b0.dat"
#MMD_FLT_B0_FILE = "#{Osk::FLT_SRV_DIR}/#{MMD_B0_FILE}"
#MMD_GND_B0_FILE = "#{Osk::GND_SRV_DIR}/#{MMD_B0_FILE}"


################################################################################
## Demo Narrative Text
################################################################################

#
# 1 - Lookup symbol
# 2 - Jam & start memory dwell
# 3 - Poke, Peek, and dump in event 
# 4 - Dump memory to a file and transfer file to the ground
# 5 - Display memory dump file using COSMOS table manager tool
# 6 - Fill memory with pattern from command
# 7 = Load memory from dump file to restore values
#

# Instruct 0 text is in the file demo screen text file.  It's here to help with the info text.
MMD_INSTRUCT_0 = ["This demo shows some basic memory management features. It uses the Memory",
                  "Management(MM) app to peek/load and poke/dump memory. It uses the Memory",
                  "Dwell(MD) app to autonomously monitor and telemeter memory locations. Click...",
                  " ",
                  "  <More Info> to obtain more information about the current step",
                  "  <Demo> to issue commands to demonstrate a feature in the current step",
                  "  <Next> to move to the next step"]
              
MMD_INFO_0 = Osk::DEMO_STEP_NO_INFO

MMD_INSTRUCT_1 = ["The memory management and a memory dwell screens have been opened. This",
                  "demo manipulates 16 bytes starting at an offset of #{MMD_OFFSET_W0} bytes",
                  "from the symbol #{MMD_SYMBOL}",
                  "",
                  "<Demo> Send MM LOOKUP_SYMBOL(#{MMD_SYMBOL}) cmd. The address is shown",
                  "                in the MM telemetry screen.",
                  ""]

MMD_INFO_1 = Osk::DEMO_STEP_NO_INFO


# 2 - Jam & start memory dwell
MMD_INSTRUCT_2 = ["Configure memory dwell screen have been opened.",
                  "",
                  "<Demo> Send 4 MD JAM_DWELL commands to configure dwell tabel 1 to ",
                  "                monitor 4 32-bit words starting at #{MMD_SYMBOL}+256",
                  "",
                  "<Demo> Send MD START_DWELL ",
                  ""]
               
MMD_INFO_2 = Osk::DEMO_STEP_NO_INFO

# 3 - Poke, Peek, and dump in event 
MMD_INSTRUCT_3 = ["'Poke' values into memory. The values are automatically displayed in the Memory Dwell",
                  " packet. Use Peek and Dump-in-Event commands to also display values.",
                  "",
                  "<Demo> MM POKE_MEM at #{MMD_SYMBOL}+#{MMD_OFFSET_W0} with incrementing pattern",
                  "<Demo> MM PEEK_MEM at #{MMD_SYMBOL}+#{MMD_OFFSET_W0} with 8, 16, and 32 bit data sizes",
                  "<Demo> MM DUMP_IN_EVENT 8 bytes starting at #{MMD_SYMBOL}+#{MMD_OFFSET_W0}",
                  ""]
MMD_INFO_3 = Osk::DEMO_STEP_NO_INFO

                    
# 4 - Dump memory to a file and transfer file to the ground
MMD_INSTRUCT_4 = ["Dump memory to file and transfer it to the ground",
                  "",
                  "<Demo> MM DUMP_MEM_TO_FILE 16 bytes to #{MMD_FLT_PATH_FILENAME}",
                  "",
                  "<Demo> Send TFTP get cmd to transfer the file to #{MMD_GND_PATH_FILENAME}",
                  "",
                  ""]
MMD_INFO_4 = Osk::DEMO_STEP_NO_INFO

# 5 - Display memory dump file using COSMOS table manager tool
MMD_INSTRUCT_5 = ["Display the memory dump file using COSMOS Table Manager tool.",
                  "",
                  "<Demo> Launch Table Manager", 
                  "",
                  "   1. On tool menu bar select File->Open->#{Osk::REL_SRV_DIR}",
                  "   2. In open dialog select #{MMD_DMP_FILE} and click 'Open'",
                  "   3. When prompted for table defintion file select #{Osk::TBL_MGR_DEF_MM_DMP}"]
MMD_INFO_5 = Osk::DEMO_STEP_NO_INFO

# 6 - Fill memory with pattern from command
MMD_INSTRUCT_6 = ["Fill the middle 8 bytes of the demo memory with a data pattern.",
                  "",
                  "<Demo> MM FILL_MEM 8 bytes with #{MMD_PAT_A0_STR} starting at #{MMD_SYMBOL}+#{MMD_OFFSET_W1}",
                  "",
                  "",
                  "",
                  ""]
MMD_INFO_6 = Osk::DEMO_STEP_NO_INFO

# 7 = Load memory from dump file to restore values
MMD_INSTRUCT_7 = ["Load memory from dump file to restore memory values",
                  "",
                  "<Demo> MM LOAD_MEM_FROM_FILE #{MMD_DMP_FILE}",
                  "",
                  "",
                  "<Next> will close the windows and exit the demo",
                  ""]
MMD_INFO_7 = Osk::DEMO_STEP_NO_INFO

MMD_INSTRUCT_ARRAY = [MMD_INSTRUCT_0, MMD_INSTRUCT_1, MMD_INSTRUCT_2, MMD_INSTRUCT_3, MMD_INSTRUCT_4, MMD_INSTRUCT_5, MMD_INSTRUCT_6, MMD_INSTRUCT_7]
MMD_LAST_STEP = MMD_INSTRUCT_ARRAY.length - 1


def mmd_set_instruct_text(num)

   new_instruct = MMD_INSTRUCT_ARRAY[num]
  
   $instruct1.text = new_instruct[0]
   $instruct2.text = new_instruct[1]
   $instruct3.text = new_instruct[2]
   $instruct4.text = new_instruct[3]
   $instruct5.text = new_instruct[4]
   $instruct6.text = new_instruct[5]
   $instruct7.text = new_instruct[6]

end # mmd_set_instruct_text()

################################################################################
## Demo Flow Control
################################################################################

$file_xfer = nil
$mmd_step = 0   # Demo step incremented by Next button
$mmd_demo = 0   # Issue command(s) to perform the current demo step

def memory_mgmt_demo(screen, button)

   $instruct1  = screen.get_named_widget("Instruct1")
   $instruct2  = screen.get_named_widget("Instruct2")
   $instruct3  = screen.get_named_widget("Instruct3")
   $instruct4  = screen.get_named_widget("Instruct4")
   $instruct5  = screen.get_named_widget("Instruct5")
   $instruct6  = screen.get_named_widget("Instruct6")
   $instruct7  = screen.get_named_widget("Instruct7")

   if (button == "INFO")
  
      display("CFS_KIT MEMORY_MGMT_DEMO_INFO_SCREEN",500,50)    

   elsif (button == "NEXT")
  
      $mmd_step += 1
      $mmd_demo  = 0
    
      if ($mmd_step <= MMD_LAST_STEP)
         mmd_set_instruct_text($mmd_step)
      end

      case $mmd_step
         when 1
            display("CFS_KIT MEMORY_MGMT_SCREEN",1500,50)    
            display("MD DWELL_PKT_SCREEN",50,50)    
            cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME MM, BITMASK 0x01") # Enable debug events
            wait (1)
            cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME MD, BITMASK 0x01") # Enable debug events
            wait (1)
            # Stop HS app aliveness because dots in the cFS console interfere with messages
            Osk::flight.send_cmd("HS","DIS_ALIVENESS")
         when 2..MMD_LAST_STEP
            # Keep case statement for maintenance
         else
            cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME MM, BITMASK 0x01") # Disable debug events
            wait(1)
            cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME MD, BITMASK 0x01") # Disable debug events
            $mmd_step = 0
            clear("CFS_KIT MEMORY_MGMT_SCREEN")    
            clear("MD DWELL_PKT_SCREEN")    
            clear("CFS_KIT MEMORY_MGMT_DEMO_SCREEN")
            clear("CFS_KIT MEMORY_MGMT_DEMO_INFO_SCREEN")
      
      end # Step Case
        
  elsif (button == "DEMO")
  
      case $mmd_step

         # Lookup a symbol
         when 1
            if ($mmd_demo == 0)
               Osk::flight.send_cmd("MM","LOOKUP_SYMBOL with SYMBOL_NAME #{MMD_SYMBOL}")
            # Don't increment mmd_demo; okay if user repeatedly sends lookup cmd
            end
      
         # Jam Dwell table and start a dwell
         when 2
            if ($mmd_demo == 0)
               Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 1, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               wait(1)
               Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 2, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               wait(1)
               Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 3, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W2}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               wait(1)
               Osk::flight.send_cmd("MD","JAM_DWELL with TABLE_ID 1, ENTRY_ID 4, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W3}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               wait(1)
               Osk::flight.send_cmd("MD","SET_SIGNATURE with TABLE_ID 1, PAD_16 0, SIGNATURE 'Memory Management Demo'")
               $mmd_demo += 1
            elsif ($mmd_demo == 1)
               Osk::flight.send_cmd("MD","START_DWELL with TABLE_MASK 0x0001")
               # Don't increment mmd_demo; okay if user repeatedly sends start dwell cmd. 
            end

         # 3 - Poke, Peek, and dump in event 
         when 3
            if ($mmd_demo == 0)
               Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x01020304, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               wait(1)
               Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x05060708, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               wait(1)
               Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0X090A0B0C, ADDR_OFFSET #{MMD_OFFSET_W2}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               wait(1)
               Osk::flight.send_cmd("MM","POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x0D0E0F10, ADDR_OFFSET #{MMD_OFFSET_W3}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               $mmd_demo += 1
            elsif ($mmd_demo == 1)
               Osk::flight.send_cmd("MM","PEEK_MEM with DATA_SIZE  8, MEM_TYPE 1, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               wait(1)
               Osk::flight.send_cmd("MM","PEEK_MEM with DATA_SIZE 16, MEM_TYPE 1, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               wait(1)
               Osk::flight.send_cmd("MM","PEEK_MEM with DATA_SIZE 32, MEM_TYPE 1, ADDR_OFFSET #{MMD_OFFSET_W2}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               $mmd_demo += 1
            elsif ($mmd_demo == 2)
               Osk::flight.send_cmd("MM","DUMP_IN_EVENT with MEM_TYPE 1, NUM_BYTES 8, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               # Don't increment mmd_demo and repeatedly send send dump in event. 
            end

         # 4 - Dump memory to a file and transfer file to the ground
         when 4
            if ($mmd_demo == 0)
               Osk::flight.send_cmd("MM","DUMP_MEM_TO_FILE with MEM_TYPE 1, NUM_BYTES 16, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}, FILENAME #{MMD_FLT_PATH_FILENAME}")
               $mmd_demo += 1
            elsif ($mmd_demo == 1)
               Osk::Ops.get_flt_file(MMD_FLT_PATH_FILENAME,MMD_GND_PATH_FILENAME)
               # Don't increment mmd_demo; okay to repeat last command
            end

         # 5 - Display memory dump file using COSMOS table manager tool
         when 5
            if ($mmd_demo == 0)
               Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, MMD_DMP_FILE, Osk::TBL_MGR_DEF_MM_DMP)
               $mmd_demo += 1
            end

         # 6 - Fill memory with pattern from command
         when 6
            if ($mmd_demo == 0)
               Osk::flight.send_cmd("MM","FILL_MEM with MEM_TYPE 1, NUM_BYTES 8, FILL_PATTERN #{MMD_PAT_A0}, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
               # Don't increment mmd_demo; okay to repeat last command
            end

         # 7 = Load memory from dump file to restore values
         when 7
            if ($mmd_demo == 0)
               Osk::flight.send_cmd("MM","LOAD_MEM_FROM_FILE with FILENAME #{MMD_FLT_PATH_FILENAME}")
               # Don't increment mmd_demo; okay to repeat last command
            end
      end # Step Case
  end # Demo
   
end # memory_mgmt_demo()

def memory_mgmt_demo_more_info(screen)
   
   msg_widget = screen.get_named_widget("msg_box")
   case $mmd_step
      when 0
         msg_widget.text = MMD_INFO_0
      when 1
         msg_widget.text = MMD_INFO_1
      when 2
         msg_widget.text = MMD_INFO_2
      when 3
         msg_widget.text = MMD_INFO_3
      when 4
         msg_widget.text = MMD_INFO_4
      when 5
         msg_widget.text = MMD_INFO_5
      when 6
         msg_widget.text = MMD_INFO_6
      when 7
         msg_widget.text = MMD_INFO_6
      else
         msg_widget.text = Osk::DEMO_STEP_NO_INFO
   end # Case
    
end # memory_mgmt_demo_more_info()

