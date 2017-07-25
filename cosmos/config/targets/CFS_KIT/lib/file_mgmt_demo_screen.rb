################################################################################
# cFS Kit File Management Demo (FMD)
#
# Notes:
#   1. Written by David McComas, licensed under the copyleft GNU
#      General Public License (GPL).
#   2. FMD_LABEL_x variables are used to put text on the demo screen's
#      text box. The FMD_INFO_x variables are used for extra detailed
#      information that is displayed on a separate screen.
# 
################################################################################

require 'cfs_kit_global'
require 'file_transfer'

################################################################################
## Global Variables
################################################################################

# start name with 'aa' to make it listed first in directory listing
FMD_FLT_TEMP_DIR = "#{FLT_WORK_DIR}/aatmp"  

FMD_DIR_LIST_FILE = "dir-list.dat"

FMD_FLT_PUT_FILE = "#{FMD_FLT_TEMP_DIR}/put_demo.txt"
FMD_GND_PUT_FILE = "#{GND_WORK_DIR}/put_demo.txt"

FMD_INFO_DEF = "\n\nNo additional information for this demo step."

################################################################################
## Demo Narrative Text
################################################################################

# Label 0 text is in the file demo screen text file.  It's here to help with the info text.
FMD_LABEL_0 = ["This demo shows some basic file management features. It uses the Trivial File ",
               "Transport Protocol (TFTP) App to transfer files between COSMOS and the cFS. It ",
               "uses the File Manager (FM) App to manage cFS directories and files. Click...",
               " ",
               "   <More Info> to obtain more information about the current step",
               "   <Next> to move to the next step"]
              
FMD_INFO_0 = "\n\n\
The TFTP App is configured as a server and the COSMOS ground system is configured as a client. COSMOS \
does not come with TFTP support so an open source GEM has been installed.\n\n\
\
The demo screen contains telemetry from two FM packets. The housekeeping packet provides command\
counters and the directory listing packet provides information about a specific directory that is\
is sent in response to the SEND_DIR_PKT command."

FMD_LABEL_1 = ["The SEND_DIR_PKT was sent and the first 10 entries of the #{FLT_WORK_DIR} directory",
                     "are displayed.",
                     "",
                     "Notice that both the FM and child child task command counters incremented because",
                     "the child task creates the directroy listing.",
                     ""]

FMD_INFO_1 = "\n\
The SEND_DIR_PKT command takes an offset argument that specifies the starting index into\
the directory listing. An offset of 0 is used through out this demo.\n\n\n\
\
Application command execution counters typically mean a command has been successfully processed.\
However there are often situations when a command may take a while to process and the activity can\
occur in the background. In these situations a child task performs the function and its command\
execution counters (pass/fail) indicate whether the command was completed sucessfully. The parent\
application's execution counter simply means the command was successfuly/unsucessfully parsed and\
passed to the shild task."


FMD_LABEL_2 = ["A new directory #{FMD_FLT_TEMP_DIR} was created using FM's CREATE_DIR",
               "command. The SEND_DIR_PKT command was also issued to display the latest",
               "directory contents and the new directory appears as the first file in the",
               "directory listing.",
               "",
               ""]
FMD_INFO_2 = FMD_INFO_DEF
                     

FMD_LABEL_3 = ["",
               "The TFTP App was used to write (i.e. put) #{FMD_FLT_PUT_FILE} ",
               "from the ground to the cFS. A directory listing of #{FMD_FLT_TEMP_DIR} ",
               "shows the new file has been created.",
               "",
               ""]
FMD_INFO_3 = FMD_INFO_DEF

                    
FMD_LABEL_4 = ["",
               "The directory listing of #{FLT_WORK_DIR} was sent to the file #{FMD_FLT_PUT_FILE} ",
               "using FM's WRITE_DIR_TO_FILE command. ",
               "",
               "Then the TFTP App was used to get the file from the cFS and write it to the ",
               "ground file #{GND_WORK_DIR}/#{FMD_DIR_LIST_FILE}"]
FMD_INFO_4 = FMD_INFO_DEF

FMD_LABEL_5 = ["All of the files in #{FMD_FLT_TEMP_DIR} were deleted using FM's",
               "DELETE_ALL_FILES command. Then DELETE_DIR was used to delete the",
               "directory itself. All of the files in a directory must be deleted ",
               "prior to deleting a directory.", 
               "",
              "This concludes the demo. Click <Next> to exit."]
FMD_INFO_5 = FMD_INFO_DEF


################################################################################
## Demo Flow Control
################################################################################

$file_xfer = FileTransfer.new()
$fmd_step = 0

def file_mgmt_demo(screen, button)

  line1  = screen.get_named_widget("Label1")
  line2  = screen.get_named_widget("Label2")
  line3  = screen.get_named_widget("Label3")
  line4  = screen.get_named_widget("Label4")
  line5  = screen.get_named_widget("Label5")
  line6  = screen.get_named_widget("Label6")

  if (button == "INFO")
  
    display("CFS_KIT FILE_MGMT_DEMO_INFO_SCREEN",500,50)    

  elsif (button == "NEXT")
  
    $fmd_step += 1
    case $fmd_step
      when 1
        cmd("CFE_EVS ENA_APP_EVENT_TYPE with APPNAME FM, BITMASK 0x01") # Enable debug events
        wait (1)
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FLT_WORK_DIR}, DIRLISTOFFSET 0")
        line1.text = FMD_LABEL_1[0]
        line2.text = FMD_LABEL_1[1]
        line3.text = FMD_LABEL_1[2]
        line4.text = FMD_LABEL_1[3]
        line5.text = FMD_LABEL_1[4]
        line6.text = FMD_LABEL_1[5]
        
      when 2
        cmd("FM CREATE_DIR with DIRECTORY #{FMD_FLT_TEMP_DIR}")
        wait (1)
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FLT_WORK_DIR}, DIRLISTOFFSET 0")
        line1.text = FMD_LABEL_2[0]
        line2.text = FMD_LABEL_2[1]
        line3.text = FMD_LABEL_2[2]
        line4.text = FMD_LABEL_2[3]
        line5.text = FMD_LABEL_2[4]
        line6.text = FMD_LABEL_2[5]
        
      when 3
        $file_xfer.put(FMD_GND_PUT_FILE,FMD_FLT_PUT_FILE)
        wait (1)
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FMD_FLT_TEMP_DIR}, DIRLISTOFFSET 0")
        line1.text = FMD_LABEL_3[0]
        line2.text = FMD_LABEL_3[1]
        line3.text = FMD_LABEL_3[2]
        line4.text = FMD_LABEL_3[3]
        line5.text = FMD_LABEL_3[4]
        line6.text = FMD_LABEL_3[5]
        
      when 4
        cmd("FM WRITE_DIR_TO_FILE with DIRECTORY #{FLT_WORK_DIR}, FILENAME #{FMD_FLT_TEMP_DIR}/#{FMD_DIR_LIST_FILE}")
        wait (2)
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FMD_FLT_TEMP_DIR}, DIRLISTOFFSET 0")
        wait (1)
        $file_xfer.get("#{FMD_FLT_TEMP_DIR}/#{FMD_DIR_LIST_FILE}","#{GND_WORK_DIR}/#{FMD_DIR_LIST_FILE}")
        line1.text = FMD_LABEL_4[0]
        line2.text = FMD_LABEL_4[1]
        line3.text = FMD_LABEL_4[2]
        line4.text = FMD_LABEL_4[3]
        line5.text = FMD_LABEL_4[4]
        line6.text = FMD_LABEL_4[5]
        
      when 5
        cmd("FM DELETE_ALL_FILES with DIRECTORY #{FMD_FLT_TEMP_DIR}")
        wait(1)
        cmd("FM DELETE_DIR with DIRECTORY #{FMD_FLT_TEMP_DIR}")
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FLT_WORK_DIR}, DIRLISTOFFSET 0")
        line1.text = FMD_LABEL_5[0]
        line2.text = FMD_LABEL_5[1]
        line3.text = FMD_LABEL_5[2]
        line4.text = FMD_LABEL_5[3]
        line5.text = FMD_LABEL_5[4]
        line6.text = FMD_LABEL_5[5]
        
      else
        cmd("CFE_EVS DIS_APP_EVENT_TYPE with APPNAME FM, BITMASK 0x01") # Disable debug events
        $fmd_step = 0
        clear("CFS_KIT FILE_MGMT_DEMO_SCREEN")
        clear("CFS_KIT FILE_MGMT_DEMO_INFO_SCREEN")
    end # Case
  end # Next
   
end # file_mgmt_demo_main()

def file_mgmt_demo_more_info(screen)
   
  msg_widget = screen.get_named_widget("msg_box")
  case $fmd_step
    when 0
      msg_widget.text = FMD_INFO_0
    when 1
      msg_widget.text = FMD_INFO_1
    when 2
      msg_widget.text = FMD_INFO_2
    when 3
      msg_widget.text = FMD_INFO_3
    when 4
      msg_widget.text = FMD_INFO_4
    when 5
      msg_widget.text = FMD_INFO_5
    else
      msg_widget.text = FMD_INFO_DEF
    end # Case
    
end # file_mgmt_demo_more_info()

