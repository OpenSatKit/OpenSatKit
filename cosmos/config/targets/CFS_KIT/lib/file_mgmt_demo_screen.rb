################################################################################
# cFS Kit File Management Demo (FMD)
#
# Notes:
#   1. FMD_INSTRUCT_x variables are used to put text on the demo screen's
#      text box. The FMD_INFO_x variables are used for extra detailed
#      information that is displayed on a separate screen.
#   2. Debug events are enabled for the apps used during the demo.
#   3. A conscious effort was made to choose between direct COSMOS cmd() and
#      Osk::flight.xxx.send_cmd(). cmd() has no verification and has faster
#      response while the Osk method verifies the command and is slower. If
#      a demo button is repeatedly pushes that causes an error it's good to 
#      notify the user. If enabling debug messages fails then its no big deal.
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

################################################################################
## Global Variables
################################################################################

# start name with 'aa' to make it listed first in directory listing
FMD_FLT_TEMP_DIR = "#{Osk::FLT_SRV_DIR}/aatmp"  

FMD_DEMO_PUT_FILE = "fm_demo_put.txt"

FMD_FLT_PUT_FILE = "#{FMD_FLT_TEMP_DIR}/#{FMD_DEMO_PUT_FILE}"
FMD_GND_PUT_FILE = "#{Osk::GND_SRV_DIR}/#{FMD_DEMO_PUT_FILE}"


################################################################################
## Demo Narrative Text
################################################################################

#
# 1 - Display directory
# 2 - Create new directory and display in directory listing
# 3 - Put a file to FSW
# 4 - Send directory listing to a file and downlink the file
# 5 - Delete demo directory
#

# Instruct 0 text is in the file demo screen text file.  It's here to help with the info text.
FMD_INSTRUCT_0 = ["This demo shows some basic file management features. It uses the Trivial File ",
                  "Transport Protocol (TFTP) App to transfer files between COSMOS and the cFS. It ",
                  "uses the File Manager (FM) App to manage cFS directories and files. Click...",
                  " ",
                  "  <More Info> to obtain more information about the current step",
                  "  <Demo> to issue commands to demonstrate a feature in the current step",
                  "  <Next> to move to the next step"]
              
FMD_INFO_0 = "\n\n\
The TFTP App is configured as a server and the COSMOS ground system is configured as a client. COSMOS \
does not come with TFTP support so an open source GEM has been installed.\n\n\
\
The File Management screen contains telemetry from two FM packets. The housekeeping packet provides command \
counters and the directory listing packet provides information about a specific directory that is \
is sent in response to the SEND_DIR_PKT command."

# 1 - Display directory
FMD_INSTRUCT_1 = ["The file management screen was opened. This step illustrates the directory cmd.",
                  "",
                  "<Demo> Send FM SEND_DIR_PKT cmd to display the first 10 entries of the #{Osk::FLT_SRV_DIR}",
                  "                directory",
                  "",
                  "Notice that both the FM and child child task cmd counters increment when the",
                  "cmd is issued because FM's child task creates the directory listing."]

FMD_INFO_1 = "\n\
The SEND_DIR_PKT command takes an offset argument that specifies the starting index into \
the directory listing. An offset of 0 is used through out this demo.\n\n\
\
FM's WRITE_DIR_TO_FILE cmd can be used to write an entire directory listing to a file.\n\n\
\
Application command execution counters typically mean a command has been successfully processed. \
However there are often situations when a command may take a while to process and the activity can \
occur in the background. In these situations a child task performs the function and its command \
execution counters (pass/fail) indicate whether the command was completed successfully. The parent \
application's execution counter simply means the command was successfully/unsuccessfully parsed and \
passed to the child task."


# 2 - Create new directory and display in directory listing
FMD_INSTRUCT_2 = ["Create a new directory. After the directory is created, FM's SEND_DIR_PKT is sent",
                  "to display the latest #{Osk::FLT_SRV_DIR} directory contents. The new directory appears",
                  "as the first file in the directory listing.",
                  "",
                  "<Demo> Send FM's CREATE_DIR to create a new directory #{FMD_FLT_TEMP_DIR}",
                  "",
                  ""]
FMD_INFO_2 = Osk::DEMO_STEP_NO_INFO
                     

# 3 - Put a file to FSW
FMD_INSTRUCT_3 = ["Use the TFTP App to put (i.e. write) #{FMD_FLT_PUT_FILE} from the ground to the",
                  "cFS. A directory listing of #{FMD_FLT_TEMP_DIR} shows the new file has been ",
                  "created.",
                  "",
                  "<Demo> Send TFTP's put cmd to transfer #{FMD_FLT_PUT_FILE} to the FSW", 
                  "",
                  ""]
FMD_INFO_3 = Osk::DEMO_STEP_NO_INFO

                    
# 4 - Send directory listing to a file and downlink the file
FMD_INSTRUCT_4 = ["Demonstrate writing a directory to a file and transferring the file to the ground.",
                  "",
                  "<Demo> Send FM's WRITE_DIR_TO_FILE cmd to write directory listing of #{Osk::FLT_SRV_DIR}",
                  "               to the file #{Osk::TMP_BIN_FILE}",
                  "<Demo> Send TFTP get cmd to transfer the file from the cFS and write it to the ",
                  "               ground file #{Osk::GND_SRV_DIR}/#{Osk::TMP_BIN_FILE}",
                  ""]
FMD_INFO_4 = Osk::DEMO_STEP_NO_INFO

# 5 - Delete demo directory
FMD_INSTRUCT_5 = ["Delete the demo directory. All of the files in the directory need",
                  "to be deleted prior to deleting the directory.",
                  "",
                  "<Demo> Send FM's DELETE_ALL_FILES cmd to delete all files in #{FMD_FLT_TEMP_DIR}",
                  "<Demo> Send FM's DELETE_DIR cmd to delete the directory itself",
                  "",
                  "This concludes the demo. Click <Next> to exit."]
FMD_INFO_5 = Osk::DEMO_STEP_NO_INFO

FMD_INSTRUCT_ARRAY = [FMD_INSTRUCT_0, FMD_INSTRUCT_1, FMD_INSTRUCT_2, FMD_INSTRUCT_3, FMD_INSTRUCT_4, FMD_INSTRUCT_5]
FMD_LAST_STEP = FMD_INSTRUCT_ARRAY.length - 1


def fmd_set_instruct_text(num)

   new_instruct = FMD_INSTRUCT_ARRAY[num]
  
   $instruct1.text = new_instruct[0]
   $instruct2.text = new_instruct[1]
   $instruct3.text = new_instruct[2]
   $instruct4.text = new_instruct[3]
   $instruct5.text = new_instruct[4]
   $instruct6.text = new_instruct[5]
   $instruct7.text = new_instruct[6]

end # fmd_set_instruct_text()


################################################################################
## Demo Flow Control
################################################################################

$fmd_step = 0   # Demo step incremented by Next button
$fmd_demo = 0   # Issue command(s) to perform the current demo step

def file_mgmt_demo(screen, button)

   $instruct1  = screen.get_named_widget("Instruct1")
   $instruct2  = screen.get_named_widget("Instruct2")
   $instruct3  = screen.get_named_widget("Instruct3")
   $instruct4  = screen.get_named_widget("Instruct4")
   $instruct5  = screen.get_named_widget("Instruct5")
   $instruct6  = screen.get_named_widget("Instruct6")
   $instruct7  = screen.get_named_widget("Instruct7")

   if (button == "INFO")
  
      display("CFS_KIT FILE_MGMT_DEMO_INFO_SCREEN",0,50)    

   elsif (button == "NEXT")
  
      $fmd_step += 1
      $fmd_demo = 0
    
      if ($fmd_step <= FMD_LAST_STEP)
         fmd_set_instruct_text($fmd_step)
      end

      case $fmd_step
         when 1
            display("CFS_KIT FILE_MGMT_SCREEN",1500,50)    
            # Use direct commands without verify to keep response fast
            cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME FM, BITMASK 0x01") # Enable debug events
            wait (1)
            cmd("CFE_EVS ENA_APP_EVENT_TYPE with APP_NAME TFTP, BITMASK 0x01") # Enable debug events
         when 2..FMD_LAST_STEP
            # Keep case statement for maintenance
         else
            # Use direct commands without verify to keep response fast
            cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME FM, BITMASK 0x01") # Disable debug events
            wait (1)
            cmd("CFE_EVS DIS_APP_EVENT_TYPE with APP_NAME TFTP, BITMASK 0x01") # Disable debug events
            $fmd_step = 0
            clear("CFS_KIT FILE_MGMT_SCREEN")
            clear("CFS_KIT FILE_MGMT_DEMO_SCREEN")
            clear("CFS_KIT FILE_MGMT_DEMO_INFO_SCREEN")
      end # Step Case
  
   elsif (button == "DEMO")
  
      case $fmd_step

         # 1 - Send directory cmd
         when 1
            if ($fmd_demo == 0)
               Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_SRV_DIR}, DIR_LIST_OFFSET 0")
               # Don't increment fmd_demo; okay if user repeatedly sends the directory cmd
            end
      
         # 2 - Create new directory
         when 2
            if ($fmd_demo == 0)
               Osk::flight.send_cmd("FM","CREATE_DIR with DIRECTORY #{FMD_FLT_TEMP_DIR}")
               wait (1)
               Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_SRV_DIR}, DIR_LIST_OFFSET 0")
               # Don't increment fmd_demo; okay if user repeatedly sends the create dir to see an error
            end

         # 3 - Put file from ground to flight 
         when 3
            if ($fmd_demo == 0)
               Osk::Ops::put_flt_file(FMD_GND_PUT_FILE,FMD_FLT_PUT_FILE)
               wait (1)
               Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{FMD_FLT_TEMP_DIR}, DIR_LIST_OFFSET 0")
               # Don't increment fmd_demo; okay if user repeatedly sends the command
            end

         # 4 - Write directory listing to file and transfer to the ground
         when 4
            if ($fmd_demo == 0)
               Osk::flight.send_cmd("FM","WRITE_DIR_TO_FILE with DIRECTORY #{Osk::FLT_SRV_DIR}, FILENAME #{FMD_FLT_TEMP_DIR}/#{Osk::TMP_BIN_FILE}, SIZE_TIME_MODE 1")
               wait (2)
               Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{FMD_FLT_TEMP_DIR}, DIR_LIST_OFFSET 0")
               $fmd_demo += 1
            elsif ($fmd_demo == 1)
               if (Osk::Ops.get_flt_file("#{FMD_FLT_TEMP_DIR}/#{Osk::TMP_BIN_FILE}","#{Osk::GND_SRV_DIR}/#{Osk::TMP_BIN_FILE}"))
                  Osk::Ops::launch_tbl_mgr(Osk::REL_SRV_DIR, Osk::TMP_BIN_FILE, Osk::TBL_MGR_DEF_FM_DIR)
               else
                  raise "FM Demo - File transfer from flight to ground failed" 
               end    
               # Don't increment fmd_demo; okay if user repeatedly sends the last command
            end

         # 5 - Delete files in demo directory and delete demo directory
         when 5
            if ($fmd_demo == 0)
               Osk::flight.send_cmd("FM","DELETE_ALL_FILES with DIRECTORY #{FMD_FLT_TEMP_DIR}")
               wait (2)
               Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{FMD_FLT_TEMP_DIR}, DIR_LIST_OFFSET 0")
               $fmd_demo += 1
            elsif ($fmd_demo == 1)
               Osk::flight.send_cmd("FM","DELETE_DIR with DIRECTORY #{FMD_FLT_TEMP_DIR}")
               wait (2)
               Osk::flight.send_cmd("FM","SEND_DIR_PKT with DIRECTORY #{Osk::FLT_SRV_DIR}, DIR_LIST_OFFSET 0")
               # Don't increment fmd_demo; okay if user repeatedly sends the command
            end

      end # Step Case
   end # Demo button
 
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
         msg_widget.text = Osk::DEMO_STEP_NO_INFO
      end # Case
    
end # file_mgmt_demo_more_info()

