################################################################################
# cFS Kit Memory Management Demo (MMD)
#
# Notes:
#   1. MMD_LABEL_x variables are used to put text on the demo screen's
#      text box. The MMD_INFO_x variables are used for extra detailed
#      information that is displayed on a separate screen.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
# 
################################################################################

require 'cfs_kit_global'
require 'file_transfer'

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
MMD_FLT_DMP_FILE = "#{FLT_WORK_DIR}/#{MMD_DMP_FILE}"
MMD_GND_DMP_FILE = "#{GND_WORK_DIR}/#{MMD_DMP_FILE}"

MMD_B0_FILE = "mm_demo_b0.dat"
MMD_FLT_B0_FILE = "#{FLT_WORK_DIR}/#{MMD_B0_FILE}"
MMD_GND_B0_FILE = "#{GND_WORK_DIR}/#{MMD_B0_FILE}"

MMD_INFO_DEF = "\n\nNo additional information for this demo step."

################################################################################
## Demo Narrative Text
################################################################################

#
# 1 - Lookup symbol
# 2 - Jam & start memory dwell
# 3 - Poke, Peek, and dump in event 
# 4 - Dump to file and display it
# 5 - Fill memory with pattern from command
# 6 = Load memory from file containing a different pattern
#

# Label 0 text is in the file demo screen text file.  It's here to help with the info text.
MMD_LABEL_0 = ["This demo shows some basic memory management features. It uses the Memory",
               "Management(MM) app to peek/load and poke/dump memory. It uses the Memory",
               "Dwell(MD) app to autonomously monitor and telemeter memory locations. Click...",
               " ",
               "   <More Info> to obtain more information about the current step",
               "   <Next> to move to the next step"]
              
MMD_INFO_0 = MMD_INFO_DEF

MMD_LABEL_1 = ["The memory management and a memory dwell screen have been opened. This",
               "demo manipulates 16 bytes starting at an offset of #{MMD_OFFSET_W0} bytes",
               "from the symbol #{MMD_SYMBOL}",
               "",
               "<Demo> Send MM LOOKUP_SYMBOL(#{MMD_SYMBOL}) cmd. The address is shown",
               "                in the MM telemetry screen."]

MMD_INFO_1 = MMD_INFO_DEF


# 2 - Jam & start memory dwell
MMD_LABEL_2 = ["Configure memory dwell screen have been opened.",
               "",
               "<Demo> Send 4 MD JAM_DWELL commands to configure dwell tabel 1 to ",
               "                monitor 4 32-bit words starting at #{MMD_SYMBOL}+256",
               "",
               "<Demo> Send MD START_DWELL "]
               
MMD_INFO_2 = MMD_INFO_DEF

# 3 - Poke, Peek, and dump in event 
MMD_LABEL_3 = ["'Poke' values into memory. Display using MD pkt, Peek, Dump-in-Event",
               "",
               "<Demo> MM POKE_MEM at #{MMD_SYMBOL}+#{MMD_OFFSET_W0} with incrementing pattern",
               "<Demo> MM PEEK_MEM at #{MMD_SYMBOL}+#{MMD_OFFSET_W0} with 8, 16, and 32 bit data sizes",
               "<Demo> MM DUMP_IN_EVENT 8 bytes starting at #{MMD_SYMBOL}+#{MMD_OFFSET_W0}", 
               ""]
MMD_INFO_3 = MMD_INFO_DEF

                    
# 4 - Dump memory to file and display it
MMD_LABEL_4 = ["Dump memory to file and display the contents",
               "",
               "<Demo> MM DUMP_MEM_TO_FILE 16 bytes to #{MMD_FLT_DMP_FILE} and transfer",
               "                to #{MMD_GND_DMP_FILE}",
               "<Demo> Display #{MMD_GND_DMP_FILE}",
               ""]
MMD_INFO_4 = MMD_INFO_DEF

# 5 - Fill memory with pattern from command
MMD_LABEL_5 = ["Fill the middle 8 bytes of the demo memory with a data pattern.",
               "",
               "<Demo> MM FILL_MEM 8 bytes with #{MMD_PAT_A0_STR} starting at #{MMD_SYMBOL}+#{MMD_OFFSET_W1}",
               "",
               "",
               ""]
MMD_INFO_5 = MMD_INFO_DEF

# 6 = Load memory from file containing a different pattern
MMD_LABEL_6 = ["Load memory from file to overwrite fill pattern from previous step",
               "",
               "<Demo> MM LOAD_MEM_FROM_FILE #{MMD_FLT_B0_FILE}",
               "",
               "",
               "<Next> will close the windows and exit the demo"]
MMD_INFO_6 = MMD_INFO_DEF

################################################################################
## Demo Flow Control
################################################################################

$file_xfer = FileTransfer.new()
$mmd_step = 0   # Demo step incremented by Next button
$mmd_demo = 0   # Issue command(s) to perform the current demo step

def memory_mgmt_demo(screen, button)

  line1  = screen.get_named_widget("Label1")
  line2  = screen.get_named_widget("Label2")
  line3  = screen.get_named_widget("Label3")
  line4  = screen.get_named_widget("Label4")
  line5  = screen.get_named_widget("Label5")
  line6  = screen.get_named_widget("Label6")

  if (button == "INFO")
  
    display("CFS_KIT MEMORY_MGMT_DEMO_INFO_SCREEN",500,50)    

  elsif (button == "NEXT")
  
    $mmd_step += 1
    $mmd_demo  = 0
    
    case $mmd_step
      when 1
        display("CFS_KIT MEMORY_MGMT_SCREEN",500,50)    
        display("MD DWELL_PKT_SCREEN",500,50)    
        cmd("CFE_EVS ENA_APP_EVENT_TYPE with APPNAME MM, BITMASK 0x01") # Enable debug events
        wait (1)
        cmd("CFE_EVS ENA_APP_EVENT_TYPE with APPNAME MD, BITMASK 0x01") # Enable debug events
        line1.text = MMD_LABEL_1[0]
        line2.text = MMD_LABEL_1[1]
        line3.text = MMD_LABEL_1[2]
        line4.text = MMD_LABEL_1[3]
        line5.text = MMD_LABEL_1[4]
        line6.text = MMD_LABEL_1[5]
        
      when 2
        line1.text = MMD_LABEL_2[0]
        line2.text = MMD_LABEL_2[1]
        line3.text = MMD_LABEL_2[2]
        line4.text = MMD_LABEL_2[3]
        line5.text = MMD_LABEL_2[4]
        line6.text = MMD_LABEL_2[5]
        
      when 3
        line1.text = MMD_LABEL_3[0]
        line2.text = MMD_LABEL_3[1]
        line3.text = MMD_LABEL_3[2]
        line4.text = MMD_LABEL_3[3]
        line5.text = MMD_LABEL_3[4]
        line6.text = MMD_LABEL_3[5]
        
      when 4
        line1.text = MMD_LABEL_4[0]
        line2.text = MMD_LABEL_4[1]
        line3.text = MMD_LABEL_4[2]
        line4.text = MMD_LABEL_4[3]
        line5.text = MMD_LABEL_4[4]
        line6.text = MMD_LABEL_4[5]
        
      when 5
        line1.text = MMD_LABEL_5[0]
        line2.text = MMD_LABEL_5[1]
        line3.text = MMD_LABEL_5[2]
        line4.text = MMD_LABEL_5[3]
        line5.text = MMD_LABEL_5[4]
        line6.text = MMD_LABEL_5[5]

      when 6
        line1.text = MMD_LABEL_6[0]
        line2.text = MMD_LABEL_6[1]
        line3.text = MMD_LABEL_6[2]
        line4.text = MMD_LABEL_6[3]
        line5.text = MMD_LABEL_6[4]
        line6.text = MMD_LABEL_6[5]
    
      else
        cmd("CFE_EVS DIS_APP_EVENT_TYPE with APPNAME MM, BITMASK 0x01") # Disable debug events
        cmd("CFE_EVS DIS_APP_EVENT_TYPE with APPNAME MD, BITMASK 0x01") # Disable debug events
        $mmd_step = 0
        clear("CFS_KIT MEMORY_MGMT_SCREEN")    
        clear("MD DWELL_PKT_SCREEN")    
        clear("CFS_KIT MEMORY_MGMT_DEMO_SCREEN")
        clear("CFS_KIT MEMORY_MGMT_DEMO_INFO_SCREEN")
    end # Case
        
  elsif (button == "DEMO")
  
    case $mmd_step

      # Lookup a symbol
      when 1
        if ($mmd_demo == 0)
          cmd("MM LOOKUP_SYMBOL with SYMBOL_NAME #{MMD_SYMBOL}")
          # Don't increment mmd_demo; okay if user repeatedly sends lookup cmd
        end
      
      # Jam Dwell table and start a dwell
      when 2
        if ($mmd_demo == 0)
          cmd("MD JAM_DWELL with TABLE_ID 1, ENTRY_ID 1, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          cmd("MD JAM_DWELL with TABLE_ID 1, ENTRY_ID 2, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          cmd("MD JAM_DWELL with TABLE_ID 1, ENTRY_ID 3, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W2}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          cmd("MD JAM_DWELL with TABLE_ID 1, ENTRY_ID 4, FIELD_LEN 4, DELAY 1, ADDR_OFFSET #{MMD_OFFSET_W3}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          $mmd_demo += 1
        elsif ($mmd_demo == 1)
          cmd("MD START_DWELL with TABLE_MASK 0x0001")
          # Don't increment mmd_demo; okay if user repeatedly sends start dwell cmd. 
        end

      # 3 - Poke, Peek, and dump in event 
      when 3
        if ($mmd_demo == 0)
          cmd("MM POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x01020304, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          cmd("MM POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x05060708, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          cmd("MM POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0X090A0B0C, ADDR_OFFSET #{MMD_OFFSET_W2}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          cmd("MM POKE_MEM with DATA_SIZE 32, MEM_TYPE 1, DATA 0x0D0E0F10, ADDR_OFFSET #{MMD_OFFSET_W3}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          $mmd_demo += 1
        elsif ($mmd_demo == 1)
          cmd("MM PEEK_MEM with DATA_SIZE  8, MEM_TYPE 1, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          cmd("MM PEEK_MEM with DATA_SIZE 16, MEM_TYPE 1, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          cmd("MM PEEK_MEM with DATA_SIZE 32, MEM_TYPE 1, ADDR_OFFSET #{MMD_OFFSET_W2}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          $mmd_demo += 1
        elsif ($mmd_demo == 2)
          cmd("MM DUMP_IN_EVENT with MEM_TYPE 1, NUM_BYTES 8, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
          # Don't increment mmd_demo and repeatedly send send dump in event. 
        end

      # 4 - Dump to file and display it
      when 4
        if ($mmd_demo == 0)
          cmd("MM DUMP_MEM_TO_FILE with MEM_TYPE 1, NUM_BYTES 16, ADDR_OFFSET #{MMD_OFFSET_W0}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}, FILE_NAME #{MMD_FLT_DMP_FILE}")
          wait (2)
          $file_xfer.get("#{MMD_FLT_DMP_FILE}","#{MMD_GND_DMP_FILE}")
          $mmd_demo += 1
        elsif ($mmd_demo == 1)
          #TODO - Implement display memory dump file
          prompt("Display memory dump file not implemented")
          $mmd_demo += 1
        end

      # 5 - Fill memory with pattern from command
      when 5
        if ($mmd_demo == 0)
          cmd("MM FILL_MEM with MEM_TYPE 1, NUM_BYTES 8, FILL_PATTERN #{MMD_PAT_A0}, ADDR_OFFSET #{MMD_OFFSET_W1}, ADDR_SYMBOL_NAME #{MMD_SYMBOL}")
        end

      # 6 = Load memory from file containing a different pattern
      when 6
        if ($mmd_demo == 0)
          $file_xfer.put("#{MMD_GND_B0_FILE}","#{MMD_FLT_B0_FILE}")
          cmd("MM LOAD_MEM_FROM_FILE with FILE_NAME #{MMD_FLT_B0_FILE}")
        end
    end # Case
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
    else
      msg_widget.text = MMD_INFO_DEF
    end # Case
    
end # memory_mgmt_demo_more_info()

