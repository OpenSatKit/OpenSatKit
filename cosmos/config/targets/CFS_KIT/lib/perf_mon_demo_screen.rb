################################################################################
# Performance Monitor Demo (PMD) Screen Scripts
#
# Notes:
#   1. Written by David McComas, licensed under the copyleft GNU
#      General Public License (GPL).
#   2. PMD_LABEL_x variables are used to put text on the demo screen's
#      text box. The PMD_INFO_x variables are used for extra detailed
#      information that is displayed on a separate screen.
#   3. The performoance monitor feature has been equated to a Logic 
#      Analyzer (LA) so LA is in the comamnd names and in the code.
#
################################################################################

require 'cfs_kit_global'
require 'file_transfer'
require 'perf_mon_screen'

################################################################################
## Global Variables
################################################################################

PMD_DAT_FILE = "perf-mon-demo.dat"
PMD_DAT_FLT_FILE = "#{FLT_WORK_DIR}/#{PMD_DAT_FILE}"
PMD_DAT_GND_FILE = "#{GND_WORK_DIR}/#{PMD_DAT_FILE}"
PMD_INFO_DEF = "\n\nNo additional information for this demo step."

PMD_DIR_LIST_FILE = "dir-list.dat"
PMD_DIR_LIST_FLT_FILE = "#{FLT_WORK_DIR}/#{PMD_DIR_LIST_FILE}"

################################################################################
## Demo Narrative Text
################################################################################

# Label 0 text is in the performance monitor demo screen text file. It's here to help with the info text.
PMD_LABEL_0 = ["This demo shows how to use the cFS Performance Monitor functionality. Code execution",
               "markers are captured in FSW memory and transferred to a file. The file is dumped to",
               "the ground and the CPM tool is used to display the captured execution data. Click...",
               " ",
               "   <More Info> to obtain more information about the current step",
               "   <Next> to move to the next step"]
              
PMD_INFO_0 = "\n\n\
The cFE Executive Serve (ES) app controls the performance monitor data. The operational steps are \n\n\
  1. Send the START_LA_DATA command to start the data collection \n\
  2. Wait some amount of time for the data to be collected \n\
  3. Send the STOP_LA_DATA command. This writes the data to a file \n\
  4. Transfer the data to the ground \n\
  5. Analyse the data using the cFS Performance Monitor tool. \n\n\

ES configuration parameter CFE_ES_PERF_DATA_BUFFER_SIZE defines amount of memory used to capture execution markers. \n\
The tools\\perfutils-java directory contains the CPM user's guide."

# 1-Filter Mask
PMD_LABEL_1 = ["The SET_LA_FILTER_MASK command was sent four times to configure the filter",
               "masks for the demo. The demo traces performance IDs:",
               "     39(0x0027) - FM App",
               "     44(0x002C) - FM_CHILD",
               "    100(0x0064) - DEMO App",
               " The command parameters issued were: [0]=0, [1]=0x1080, [2]=0, [3]=0x0010"]

PMD_INFO_1 = "\n\n\
There is one bit for each performance marker (ID) and a '1' indicates the ID should be \n\
traced. The masks are arranged in an array and the bits are logically numbered 0..127 \n\
spanning the 32-bit array entries. \n\n\

ES configuration parameter CFE_ES_PERF_MAX_IDS defines the number of performance monitor IDs. \n\
It defaults to 128 which is why the demo page is configured as a 4 dimensional array with 32 \n\
bits per entry."

# 2-Trigger Mask
PMD_LABEL_2 = ["The SET_LA_TRIG_MASK command was sent four times to configure the trigger",
               "masks for the demo. The demo uses the demo app as the trigger to start data",
               "collection so the command parameters issued were: [0]=0, [1]=0, [2]=0, [3]=0x0010",
               "",
               "Triggers are evaluated after a START_LA_DATA command is sent which starts when",
               "<Next> is clicked. There will be a 12s delay in the screen update as data is collected."]
                     
PMD_INFO_2 = "\n\n\
The trigger mask are structured identically to the filter mask. A '1' indicates to use \n\
an ID as a trigger to start data capture. \n\n\

ES configuration parameter CFE_ES_PERF_MAX_IDS defines the number of \n\
performance monitor IDs. It defaults to 128 which is why the demo page \n\
is configured as a 4 dimensional array with 32 bits per entry."

# 3-Collect Data
PMD_LABEL_3 = ["The START_LA_DATA command was issued to start data collection. Multiple",
               "FM commands were issued to create interesting trace data.",
               "",
               "The STOP_LA_DATA command was issued and the data writtten to #{PMD_DAT_FLT_FILE}",
               "When <Next> is clicked the file will be transferred to the ground and the ",
               "cFS Performance Monitor(CPM) tool will be launched."]
PMD_INFO_3 = PMD_INFO_DEF


# 4-Transfer file and use CPM to display the results
PMD_LABEL_4 = ["Data is collected for approximately 12 seconds. The sequence of events is:",
               "    - Wait 3s, Send FM Send Directory packet command",
               "    - Wait 3s, Send FM Write Directory to file command",
               "    - Wait 3s, Send FM Send Directory packet command",
               "    - Wait 3s, Send STOP_LA_DATA command",
               "See <More Info> for a how to view the data in the CPM tool"]
PMD_INFO_4 = "\n\n\
Perform the following steps to import and view the data in the CPM tool:\n\
    1. Select the Log Menu item in the top left and then the Read Log drop\n\
        down menu.\n\
    2. In the file dialogue box navigate to /config/targets/CFS_KIT/file_server\n\
        directory\n\ 
    3. Select #{PMD_DAT_FILE} and click Read. \n\
    4. You'll see a couple of status dialogue boxes stating the number of IDs\n\
        (should be 3) and that some inconsistencies exist. This is okay and\n\
        it may or may not be true depending on timing.\n\n\

The total time is roughly 12 seconds and the commanding events will occur \n\
before seconds 3, 6, and 9 because the COSMOS wait commands are not\n\
exactly 3 seconds. \n\n\

There are four rows in the display:\n\
   CPU Activity - This is a summation of all the traced activity. \n\
     oxoooooo27 - FM App - Wakes up TBD. \n\
     oxoooooo2c - FM Child Task - Runs for each command. Notice write directory\n\
                       to file takes longer than the send directory listing to a\n\
                       telemetry packet \n\
     oxoooooo64 - Demo App, Excutes every 500ms.\n\n\
"

################################################################################
## Demo Flow Control
################################################################################

$pmd_step = 0

def perf_mon_demo(screen, button)

  line1  = screen.get_named_widget("Label1")
  line2  = screen.get_named_widget("Label2")
  line3  = screen.get_named_widget("Label3")
  line4  = screen.get_named_widget("Label4")
  line5  = screen.get_named_widget("Label5")
  line6  = screen.get_named_widget("Label6")

  if (button == "INFO")
  
    display("CFS_KIT PERF_MON_DEMO_INFO_SCREEN",500,50)    

  elsif (button == "NEXT")
  
    $pmd_step += 1
    case $pmd_step
      # 1-Filter Mask
      when 1
        cmd("CFE_EVS ENA_APP_EVENT_TYPE with APPNAME CFE_ES, BITMASK 0x01") # Enable debug events
        line1.text = PMD_LABEL_1[0]
        line2.text = PMD_LABEL_1[1]
        line3.text = PMD_LABEL_1[2]
        line4.text = PMD_LABEL_1[3]
        line5.text = PMD_LABEL_1[4]
        line6.text = PMD_LABEL_1[5]
        # Performance IDs: FM 39, FM_CHILD 44, DEMO 100
        # [1] = 0x1080
        # [3] = 0x0010
        cmd ("CFE_ES SET_LA_FILTER_MASK with FILTERMASKNUM 0, FILTERMASK 0x0000")  
        wait(1)
        cmd ("CFE_ES SET_LA_FILTER_MASK with FILTERMASKNUM 1, FILTERMASK 0x1080")  
        wait(1)
        cmd ("CFE_ES SET_LA_FILTER_MASK with FILTERMASKNUM 2, FILTERMASK 0x0000")  
        wait(1)
        cmd ("CFE_ES SET_LA_FILTER_MASK with FILTERMASKNUM 3, FILTERMASK 0x0010")  
      
      # 2-Trigger Mask
      when 2
        line1.text = PMD_LABEL_2[0]
        line2.text = PMD_LABEL_2[1]
        line3.text = PMD_LABEL_2[2]
        line4.text = PMD_LABEL_2[3]
        line5.text = PMD_LABEL_2[4]
        line6.text = PMD_LABEL_2[5]
        cmd ("CFE_ES SET_LA_TRIG_MASK with TRIGGERMASKNUM 0, TRIGGERMASK 0x0000")  
        wait(1)
        cmd ("CFE_ES SET_LA_TRIG_MASK with TRIGGERMASKNUM 1, TRIGGERMASK 0x0000")  
        wait(1)
        cmd ("CFE_ES SET_LA_TRIG_MASK with TRIGGERMASKNUM 2, TRIGGERMASK 0x0000")  
        wait(1)
        cmd ("CFE_ES SET_LA_TRIG_MASK with TRIGGERMASKNUM 3, TRIGGERMASK 0x0010")  
      
      # 3-Collect Data
      when 3
        line1.text = PMD_LABEL_3[0]
        line2.text = PMD_LABEL_3[1]
        line3.text = PMD_LABEL_3[2]
        line4.text = PMD_LABEL_3[3]
        line5.text = PMD_LABEL_3[4]
        line6.text = PMD_LABEL_3[5]
        cmd ("CFE_ES START_LA_DATA with TRIGGERMODE 0")
        wait (3)
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FLT_WORK_DIR}, DIRLISTOFFSET 0")
        wait (3)
        cmd("FM WRITE_DIR_TO_FILE with DIRECTORY #{FLT_WORK_DIR}, FILENAME #{PMD_DIR_LIST_FLT_FILE}")
        wait (3)
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FLT_WORK_DIR}, DIRLISTOFFSET 0")
        wait (3)
        cmd_valid_cnt = tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT")
        cmd_error_cnt = tlm("CFE_ES HK_TLM_PKT CMD_ERROR_COUNT")
        seq_cnt = tlm("CFE_ES HK_TLM_PKT CCSDS_SEQUENCE")
        cmd ("CFE_ES STOP_LA_DATA with DATAFILENAME #{PMD_DAT_FLT_FILE}")  
        wait("CFE_ES HK_TLM_PKT CMD_VALID_COUNT == #{cmd_valid_cnt}+1", 10)  # Delay until updated valid cmd count or timeout
        if ( (tlm("CFE_ES HK_TLM_PKT CMD_VALID_COUNT") != (cmd_valid_cnt + 1)) || 
             (tlm("CFE_ES HK_TLM_PKT CMD_ERROR_COUNT") !=  cmd_error_cnt))
          if (tlm("CFE_ES HK_TLM_PKT CCSDS_SEQUENCE") == seq_cnt)
            prompt ("No telemetry received to verify the error. Verify connection and telemetry output filter table.");
          else
            prompt ("Executive Service had an error processing the command. See event message for details.")
          end
        else
          prompt ("Successfully created #{PMD_DAT_FLT_FILE}. Click <Next> to transfer file to COSMOS and luanch the performance analyzer tool.")
        end 

      # 4-Transfer file and use cFS Performance Monitor (CPM) tool to display the results
      when 4
        line1.text = PMD_LABEL_4[0]
        line2.text = PMD_LABEL_4[1]
        line3.text = PMD_LABEL_4[2]
        line4.text = PMD_LABEL_4[3]
        line5.text = PMD_LABEL_4[4]
        line6.text = PMD_LABEL_4[5]
        # TFTP uses UDP directly without cmd interface so can't use cmd counters to verify execution
        get_file_cnt = tlm("TFTP HK_TLM_PKT GET_FILE_COUNT")
        seq_cnt = tlm("TFTP HK_TLM_PKT CCSDS_SEQUENCE")
        file_xfer = FileTransfer.new()
        file_xfer.get(PMD_DAT_FLT_FILE,PMD_DAT_GND_FILE)
        wait("TFTP HK_TLM_PKT GET_FILE_COUNT == #{get_file_cnt}+1", 10)  # Delay until get file count increments or timeout
        if (tlm("TFTP HK_TLM_PKT CCSDS_SEQUENCE") == seq_cnt)
          prompt ("No telemetry received to verify the error. Verify connection and telemetry output filter table.");
        else
          perf_mon_launch_app(screen, "PERF_MONITOR_TOOL")
        end 

      else
        cmd("CFE_EVS DIS_APP_EVENT_TYPE with APPNAME CFE_ES, BITMASK 0x01") # Disable debug events
        $pmd_step = 0
        clear("CFS_KIT PERF_MON_DEMO_SCREEN")
        clear("CFS_KIT PERF_MON_DEMO_INFO_SCREEN")
    end # case
  end # button
   
end # perf_mon_demo_main()

def perf_mon_demo_more_info(screen)
   
  msg_widget = screen.get_named_widget("msg_box")
  case $pmd_step
    when 0
      msg_widget.text = PMD_INFO_0
    when 1
      msg_widget.text = PMD_INFO_1
    when 2
      msg_widget.text = PMD_INFO_2
    when 3
      msg_widget.text = PMD_INFO_3
    when 4
      msg_widget.text = PMD_INFO_4
    else
      msg_widget.text = PMD_INFO_DEF
    end # Case
    
end # perf_mon_demo_more_info()

