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

require 'cfs_kit_global'
require 'file_transfer'

################################################################################
## Global Variables
################################################################################

TMD_TBL_NAME = "FM.FreeSpace"          # cFS table name
TMD_TBL_DEF_NAME = "fm_freespace.txt"  # COSMOS table definition file
TMD_TBL_FILE = "tbl_demo_dmp.dat"      # File name used in demo. Don't clobber real name

TMD_FLT_TMP_FILE = "#{FLT_SRV_DIR}/#{TMD_TBL_FILE}"
TMD_GND_TMP_FILE = "#{GND_SRV_TBL_DIR}/#{TMD_TBL_FILE}"

TMD_INFO_DEF = "\n\nNo additional information for this demo step."

################################################################################
## Demo Narrative Text
################################################################################

#
# 1 - Display table registry
# 2 - Dump table to file and transfer file to ground
# 3 - Display table using COSMOS table manager tool
# 4 - Table load
#

# Instruct 0 text is in the file demo screen text file.  It's here to help with the info text.
TMD_INSTRUCT_0 = ["This demo shows some basic table management features using File Manager's (FM) ",
                  "FreeSpace table. It uses the Trivial File Transport Protocol (TFTP) App to ",
                  "transfer table files between COSMOS and the cFS. Click...",
                  "",
                  "  <More Info> to obtain more information about the current step",
                  "  <Demo> to issue commands to demonstrate a feature in the current step",
                  "  <Next> to move to the next step"]
TMD_INFO_0 = TMD_INFO_DEF


# 1 - Display table registry
TMD_INSTRUCT_1 = ["The table management screen was opened. This screen is a combination of ",
                  "information from the cfE's Table Service (TBL) Housekeeping telemetry packet",
                  "and TBL Service's table registry packet that is sent in response to a",
                  "TLM_REGISTRY command.",
                  "",
                  "<Demo> Send TBL TLM_REGISTRY cmd with TABLENAME #{TMD_TBL_NAME}",
                  "",
                  ""]
TMD_INFO_1 = TMD_INFO_DEF

# 2 - Dump table to file and transfer file to ground
TMD_INSTRUCT_2 = ["Dump #{TMD_TBL_NAME} to a file and transfer it to the ground.",
                  "",
                  "",
                  "<Demo> Send TBL's DUMP_TBL cmd",
                  "<Demo> Send TFTP get cmd to transfer the file to ground",
                  "",
                  ""]
TMD_INFO_2 = TMD_INFO_DEF
                     

# 3 - Display table using COSMOS table manager tool
TMD_INSTRUCT_3 = ["Display the file using COSMOS Table Manager tool.",
                  "",
                  "<Demo> Launch Table Manager", 
                  "",
                  "1. Within tool select File->Open->file_server/table, then select #{TMD_TBL_FILE}",
                  "2. Within table definition dialog navigate to #{GND_SRV_TBL_DIR}",
                  "   and select #{TMD_TBL_DEF_NAME}"]
TMD_INFO_3 = TMD_INFO_DEF

# 4 - Table load
TMD_INSTRUCT_4 = ["Transfer a table file from ground to flight and load it.",
                  "",
                  "<Demo> #{MSG_TBD_FEATURE}", 
                  "",
                  "This concludes the demo. Click <Next> to exit.",
                  "",
                  ""]
TMD_INFO_4 = TMD_INFO_DEF

TMD_INSTRUCT_ARRAY = [TMD_INSTRUCT_0, TMD_INSTRUCT_1, TMD_INSTRUCT_2, TMD_INSTRUCT_3, TMD_INSTRUCT_4]
TMD_LAST_STEP = 4

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

$file_xfer = FileTransfer.new()
$tmd_step = 0   # Demo step incremented by Next button
$tmd_demo = 0   # Issue command(s) to perform the current demo step

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
        display("CFS_KIT TABLE_MGMT_SCREEN",500,50)    
        cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME CFE_TBL, BITMASK 0x01") # Enable debug events
      when 2..TMD_LAST_STEP
        # Keep case statement for maintenance
      else
        cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME CFE_TBL, BITMASK 0x01") # Disable debug events
        $tmd_step = 0
        clear("CFS_KIT TABLE_MGMT_SCREEN")
        clear("CFS_KIT TABLE_MGMT_DEMO_SCREEN")
        clear("CFS_KIT TABLE_MGMT_DEMO_INFO_SCREEN")
    end # Step Case
  
  elsif (button == "DEMO")
  
    case $tmd_step

      # 1 - Send table registry cmd
      when 1
        if ($tmd_demo == 0)
          cmd("CFE_TBL TLM_REGISTRY with TABLE_NAME #{TMD_TBL_NAME}")
          # Don't increment tmd_demo; okay if user repeatedly sends the registry cmd
        end
      
      # 2 - Dump table to file & transfer to the ground
      when 2
        if ($tmd_demo == 0)
          cmd_valid_cnt = tlm("CFE_TBL HK_TLM_PKT CMD_VALID_COUNT")
          cmd_error_cnt = tlm("CFE_TBL HK_TLM_PKT CMD_ERROR_COUNT")
          seq_cnt = tlm("CFE_TBL HK_TLM_PKT CCSDS_SEQUENCE")
          cmd("CFE_TBL DUMP_TBL with ACTIVE_TBL_FLAG 1, TABLE_NAME #{TMD_TBL_NAME}, DUMP_FILENAME #{TMD_FLT_TMP_FILE}")
          wait("CFE_TBL HK_TLM_PKT CCSDS_SEQUENCE != #{seq_cnt}", 10)  # Delay until updated sequence count or timeout
          if ( (tlm("CFE_TBL HK_TLM_PKT CMD_VALID_COUNT") != (cmd_valid_cnt + 1)) || 
               (tlm("CFE_TBL HK_TLM_PKT CMD_ERROR_COUNT") !=  cmd_error_cnt))
          
          end
          $tmd_demo += 1
        elsif ($tmd_demo == 1)
          $file_xfer.get("#{TMD_FLT_TMP_FILE}","#{TMD_GND_TMP_FILE}")
          # Don't increment tmd_demo; okay if user repeatedly sends the command
        end

      # 3 - Display table using the Table Manager tool 
      when 3
        if ($tmd_demo == 0)
          Cosmos.run_process("ruby tools/TableManager")
          $tmd_demo += 1
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
      msg_widget.text = TMD_INFO_DEF
    end # Case
    
end # table_mgmt_demo_more_info()
