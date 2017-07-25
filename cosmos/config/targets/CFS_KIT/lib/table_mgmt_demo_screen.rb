###############################################################################
# cFS Kit Table Management Demo
#
# Notes:
#   1. Written by David McComas, licensed under the copyleft GNU
#      General Public License (GPL).
# 
################################################################################

FLT_TMP_TBL_FILE = "#{FLT_WORK_DIR}/~tbl_tmp.dat"    
GND_TMP_TBL_FILE = "#{GND_WORK_DIR}/~tbl_tmp.dat"

GND_TMP_TBL_NO_HDR_FILE = "#{GND_WORK_DIR}/tables/~tbl_tmp_no_hdr.dat"
#TODO GND_TMP_TBL_NO_HDR_FILE = "#{Cosmos::USERPATH}/outputs/tables/~tbl_tmp_no_hdr.dat"

###############################################################################
# cFS Kit Table Management Demo 
#
# Notes:
#   1. Written by David McComas, licensed under the copyleft GNU
#      General Public License (GPL).
# 
################################################################################

#require 'cosmos'
#require 'cosmos/script'

TABLE_DEMO_TBL_NAME = "FM.FreeSpace"

TABLE_DEMO_FLT_FILE = "#{FLT_WORK_DIR}/tbl_tmp.dat"
TABLE_DEMO_GND_FILE = "#{GND_WORK_DIR}/tbl_tmp.dat"

TABLE_DEMO_MSG_INTRO = "\
This demo illustrates some basic table management features using FIle\n\
Manager's #{TABLE_DEMO_TBL_NAME}. \n\

The telemetry to the right is a combination of information from the cfE's\n\
Table Service (TBL) Housekeeping telemetry packet and TBL's table resgirty\n\
packet. The registry packet is sent in response to a TLM_REGISTRY command.\n\

After you press <#{BUTTON_CONT}> the TLM_REGISTRY command will be sent.\n\

You may exit the demo by pressing <#{BUTTON_CANCEL}>\n\
"    

TABLE_DEMO_MSG_DUMP = "\
The next step creates dumps the active table.\ 
"

TABLE_DEMO_MSG_GET_FILE = "\
The next step transfers the table file to the ground.\ 
"

TABLE_DEMO_MSG_DISPLAY = "\
The next step strips the cFE header from the table file and displays the\n\
file in the COSMOS Table Manager tool.
"

def table_demo_main(screen)

  file = FileXfer.new(IP_ADDR)

  # 1. Intro 
  msg_widget = screen.get_named_widget("msg_box")
  msg_widget.text = TABLE_DEMO_MSG_INTRO
  sel = message_box("Press #{BUTTON_CONT} to continue",BUTTON_CONT)
  if (sel == BUTTON_CANCEL) then
    table_demo_exit(true)
    return 
  end
  cmd("TBL TLM_REGISTRY with TABLENAME #{TABLE_DEMO_TBL_NAME}")

  # 2. Dump active table to file
  msg_widget.text = TABLE_DEMO_MSG_DUMP
  sel = message_box("Press #{BUTTON_CONT} to continue",BUTTON_CONT)
  cmd_valid_cnt = tlm("TBL HK_TLM_PKT CMD_VALID_COUNT")
  cmd_error_cnt = tlm("TBL HK_TLM_PKT CMD_ERROR_COUNT")
  seq_cnt = tlm("TBL HK_TLM_PKT CCSDS_SEQUENCE")
  cmd("TBL DUMP_TBL with ACTIVETBLFLAG 1, TABLENAME 'FM.FreeSpace', DUMPFILENAME #{FLT_TMP_TBL_FILE}")
  wait("TBL HK_TLM_PKT CCSDS_SEQUENCE != #{seq_cnt}", 10)  # Delay until updated sequence count or timeout
  if ( (tlm("TBL HK_TLM_PKT CMD_VALID_COUNT") != (cmd_valid_cnt + 1)) || 
       (tlm("TBL HK_TLM_PKT CMD_ERROR_COUNT") !=  cmd_error_cnt))
    table_demo_exit(false,"Dump table command counter check failed")
    return 
  end
  
  # 3. Transfer table file to ground
  msg_widget.text = TABLE_DEMO_MSG_GET_FILE
  sel = message_box("Press #{BUTTON_CONT} to continue",BUTTON_CONT)
  cmd_valid_cnt = tlm("FT HK_TLM_PKT CMD_VALID_COUNT")
  cmd_error_cnt = tlm("FT HK_TLM_PKT CMD_ERROR_COUNT")
  seq_cnt = tlm("TBL HK_TLM_PKT CCSDS_SEQUENCE")
  get_file_cnt = tlm("FT HK_TLM_PKT GET_FILE_COUNT")
  file.get(TABLE_DEMO_FLT_FILE,TABLE_DEMO_GND_FILE)
  wait("FT HK_TLM_PKT CCSDS_SEQUENCE != #{seq_cnt}", 10)  # Delay until updated sequence count or timeout
  if ( (tlm("TBL HK_TLM_PKT CMD_VALID_COUNT") != (cmd_valid_cnt+1)) || 
       (tlm("TBL HK_TLM_PKT CMD_ERROR_COUNT") !=  cmd_error_cnt) ||
       (tlm("TBL HK_TLM_PKT GET_FILE_COUNT")  !=  (get_file_cnt+1)) )
    table_demo_exit(false,"Get file command counter check failed")
    return 
  end


  # 3. Strip big endian file header from file 
  #~tbl_file = IO.binread(GND_TMP_TBL_FILE)
  msg_widget.text = TABLE_DEMO_MSG_DISPLAY
  sel = message_box("Press #{BUTTON_CONT} to continue",BUTTON_CONT)
  IO.copy_stream(GND_TMP_TBL_FILE,GND_TMP_TBL_NO_HDR_FILE,File.size(GND_TMP_TBL_FILE)-64 ,64)

  table_demo_exit(true)
  
end # table_demo_main()

def table_demo_exit(passed,error_str='')

  if (passed)
     sel = message_box("Demo complete. Press #{BUTTON_CONT} to exit",BUTTON_CONT)
  else
     message_box("Demo failed to complete. #{error_str}",BUTTON_CONT)
  end

  # Clear last to allow user to look at screen before exiting.
  clear("CFS_KIT TABLE_DEMO_MSG_SCREEN")
  clear("CFS_KIT TABLE_DEMO_TLM_SCREEN")
 
end # table_demo_exit()


###############################################################################
# cFS Kit File Demo 
#
# Notes:
#   1. Written by David McComas, licensed under the copyleft GNU
#      General Public License (GPL).
#   2. FILE_DEMO_LABEL_x variables are used to put text on the demo screen's
#      text box. The FILE_DEMO_INFO_x variables are used for extra detailed
#      information that is displayed on a separate screen.
#
################################################################################

#require 'cosmos'
#require 'cosmos/script'

FLT_TEMP_DEMO_DIR = "#{FLT_WORK_DIR}/aatmp"  # start name with 'aa' to make it listed first in directory listing

DIR_LIST_FILE = "dir-list.dat"

FILE_DEMO_FLT_PUT_FILE = "#{FLT_TEMP_DEMO_DIR}/put_demo.txt"
FILE_DEMO_GND_PUT_FILE = "#{GND_WORK_DIR}/put_demo.txt"

FILE_DEMO_INFO_DEF = "\n\nNo additional information for this demo step."

# This label is replicated in the file demo screen text file. It's here to help with the info text.
FILE_DEMO_LABEL_0 = ["This demo shows some basic file management features. It uses the Trivial File ",
                     "Transport Protocol (TFTP) App to transfer files between COSMOS and the cFS. It ",
                     "uss the File Manager (FM) App to manage cFS directories and files. Click...",
                     " ",
                     "   <More Info> to obtain more information about the current step",
                     "   <Next> to move to the next step"]
              
FILE_DEMO_INFO_0 = "\n\n\
The TFTP App is configured as a server and the COSMOS ground system is configured as a client. COSMOS \
does not come with TFTP support so an open source GEM has been installed.\n\n\
\
The demo screen contains telemetry from two FM packets. The housekeeping packet provides command\
counters and the directory listing packet provides information about a specific directory that is\
is sent in response to the SEND_DIR_PKT command."

FILE_DEMO_LABEL_1 = ["The SEND_DIR_PKT was sent and the first 10 entries of the #{FLT_WORK_DIR} directory",
                     "are displayed.",
                     "",
                     "Notice that both the FM and child child task command counters incremented because",
                     "the child task creates the directroy listing.",
                     ""]

FILE_DEMO_INFO_1 = "\n\
The SEND_DIR_PKT command takes an offset argument that specifies the starting index into\
the directory listing. An offset of 0 is used through out this demo.\n\n\n\
\
Application command execution counters typically mean a command has been successfully processed.\
However there are often situations when a command may take a while to process and the activity can\
occur in the background. In these situations a child task performs the function and its command\
execution counters (pass/fail) indicate whether the command was completed sucessfully. The parent\
application's execution counter simply means the command was successfuly/unsucessfully parsed and\
passed to the shild task."


FILE_DEMO_LABEL_2 = ["A new directory #{FLT_TEMP_DEMO_DIR} was created using FM's CREATE_DIR",
                     "command. The SEND_DIR_PKT command was also issued to display the latest",
                     "directory contents and the new directory appears as the first file in the",
                     "directory listing.",
                     "",
                     ""]
FILE_DEMO_INFO_2 = FILE_DEMO_INFO_DEF
                     

FILE_DEMO_LABEL_3 = ["",
                     "The TFTP App was used to write (i.e. put) #{FILE_DEMO_FLT_PUT_FILE} ",
                     "from the ground to the cFS. A directory listing of #{FLT_TEMP_DEMO_DIR} ",
                     "shows the new file has been created.",
                     "",
                     ""]
FILE_DEMO_INFO_3 = FILE_DEMO_INFO_DEF

                    
FILE_DEMO_LABEL_4 = ["",
                     "The directory listing of #{FLT_WORK_DIR} was sent to the file #{FILE_DEMO_FLT_PUT_FILE} ",
                     "using FM's WRITE_DIR_TO_FILE command. ",
                     "",
                     "Then the TFTP App was used to get the file from the cFS and write it to the ",
                     "ground file #{GND_WORK_DIR}/#{DIR_LIST_FILE}"]
FILE_DEMO_INFO_4 = FILE_DEMO_INFO_DEF

FILE_DEMO_LABEL_5 = ["All of the files in #{FLT_TEMP_DEMO_DIR} were deleted using FM's",
                     "DELETE_ALL_FILES command. Then DELETE_DIR was used to delete the",
                     "directory itself. All of the files in a directory must be deleted ",
                     "prior to deleting a directory.", 
                     "",
                     "This concludes the demo. Click <Next> to exit."]
FILE_DEMO_INFO_5 = FILE_DEMO_INFO_DEF

$file_demo_step = 0

def file_demo(screen, button)

  line1  = screen.get_named_widget("Label1")
  line2  = screen.get_named_widget("Label2")
  line3  = screen.get_named_widget("Label3")
  line4  = screen.get_named_widget("Label4")
  line5  = screen.get_named_widget("Label5")
  line6  = screen.get_named_widget("Label6")

  if (button == "INFO")
  
    display("CFS_KIT FILE_DEMO_INFO_SCREEN",500,50)    

  elsif (button == "NEXT")
  
    case $file_demo_step
      when 0
        cmd("EVS ENA_APP_EVENT_TYPE with APPNAME CFE_TBL, BITMASK 0x01") # Enable debug events
        $file = FileXfer.new(IP_ADDR) # Can't instantiate at file level until TFTP is a GEM
        wait (1)
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FLT_WORK_DIR}, DIRLISTOFFSET 0")
        line1.text = FILE_DEMO_LABEL_1[0]
        line2.text = FILE_DEMO_LABEL_1[1]
        line3.text = FILE_DEMO_LABEL_1[2]
        line4.text = FILE_DEMO_LABEL_1[3]
        line5.text = FILE_DEMO_LABEL_1[4]
        line6.text = FILE_DEMO_LABEL_1[5]
        $file_demo_step += 1
      when 1
        cmd("FM CREATE_DIR with DIRECTORY #{FLT_TEMP_DEMO_DIR}")
        wait (1)
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FLT_WORK_DIR}, DIRLISTOFFSET 0")
        line1.text = FILE_DEMO_LABEL_2[0]
        line2.text = FILE_DEMO_LABEL_2[1]
        line3.text = FILE_DEMO_LABEL_2[2]
        line4.text = FILE_DEMO_LABEL_2[3]
        line5.text = FILE_DEMO_LABEL_2[4]
        line6.text = FILE_DEMO_LABEL_2[5]
        $file_demo_step += 1
      when 2
        $file.put(FILE_DEMO_GND_PUT_FILE,FILE_DEMO_FLT_PUT_FILE)
        wait (1)
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FLT_TEMP_DEMO_DIR}, DIRLISTOFFSET 0")
        line1.text = FILE_DEMO_LABEL_3[0]
        line2.text = FILE_DEMO_LABEL_3[1]
        line3.text = FILE_DEMO_LABEL_3[2]
        line4.text = FILE_DEMO_LABEL_3[3]
        line5.text = FILE_DEMO_LABEL_3[4]
        line6.text = FILE_DEMO_LABEL_3[5]
        $file_demo_step += 1
      when 3
        cmd("FM WRITE_DIR_TO_FILE with DIRECTORY #{FLT_WORK_DIR}, FILENAME #{FLT_TEMP_DEMO_DIR}/#{DIR_LIST_FILE}")
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FLT_TEMP_DEMO_DIR}, DIRLISTOFFSET 0")
        $file.get("#{FLT_TEMP_DEMO_DIR}/#{DIR_LIST_FILE}","#{GND_WORK_DIR}/#{DIR_LIST_FILE}")
        line1.text = FILE_DEMO_LABEL_4[0]
        line2.text = FILE_DEMO_LABEL_4[1]
        line3.text = FILE_DEMO_LABEL_4[2]
        line4.text = FILE_DEMO_LABEL_4[3]
        line5.text = FILE_DEMO_LABEL_4[4]
        line6.text = FILE_DEMO_LABEL_4[5]
        $file_demo_step += 1
      when 4
        cmd("FM DELETE_ALL_FILES with DIRECTORY #{FLT_TEMP_DEMO_DIR}")
        wait(1)
        cmd("FM DELETE_DIR with DIRECTORY #{FLT_TEMP_DEMO_DIR}")
        cmd("FM SEND_DIR_PKT with DIRECTORY #{FLT_WORK_DIR}, DIRLISTOFFSET 0")
        line1.text = FILE_DEMO_LABEL_5[0]
        line2.text = FILE_DEMO_LABEL_5[1]
        line3.text = FILE_DEMO_LABEL_5[2]
        line4.text = FILE_DEMO_LABEL_5[3]
        line5.text = FILE_DEMO_LABEL_5[4]
        line6.text = FILE_DEMO_LABEL_5[5]
        $file_demo_step += 1
      else
        cmd("EVS DIS_APP_EVENT_TYPE with APPNAME CFE_TBL, BITMASK 0x01") # Disable debug events
        $file_demo_step = 0
        clear("CFS_KIT FILE_DEMO_SCREEN")
        clear("CFS_KIT FILE_DEMO_INFO_SCREEN")
    end # Case
  end # Next
   
end # file_demo_main()

def file_demo_more_info(screen)
   
  msg_widget = screen.get_named_widget("msg_box")
  case $file_demo_step
    when 0
      msg_widget.text = FILE_DEMO_INFO_0
    when 1
      msg_widget.text = FILE_DEMO_INFO_1
    when 2
      msg_widget.text = FILE_DEMO_INFO_2
    when 3
      msg_widget.text = FILE_DEMO_INFO_3
    when 4
      msg_widget.text = FILE_DEMO_INFO_4
    when 5
      msg_widget.text = FILE_DEMO_INFO_5
    else
      msg_widget.text = FILE_DEMO_INFO_DEF
    end # Case
    
end # file_demo_more_info()

require 'cfs_kit_global'
require 'tftp'

FILE_MGMT_FLT_WORK_DIR = FLT_WORK_DIR
FILE_MGMT_GND_WORK_DIR = GND_WORK_DIR

def file_mgmt_send_cmd(screen, cmd)

	if (cmd == "GET_FILE")
    file = FileXfer.new(DEF_IP_ADDR)
    flt_full_file_name = ask_string("Enter full FSW path/filename.")
    gnd_file_name = ask_string("Enter ground filename. File must be in COSMOS server location.")
    gnd_full_file_name = "#{GND_WORK_DIR}/#{gnd_file_name}"
    file.get(flt_full_file_name,gnd_full_file_name)
	elsif (cmd == "PUT_FILE")
    file = FileXfer.new(DEF_IP_ADDR)
    gnd_file_name = ask_string("Enter ground filename. File must be in COSMOS server location.")
    gnd_full_file_name = "#{GND_WORK_DIR}/#{gnd_file_name}"
    flt_full_file_name = ask_string("Enter full FSW path/filename.")
    file.put(gnd_full_file_name,flt_full_file_name)
  elsif (cmd == "CREATE_DIR")
    dir_name = ask_string("Enter full path of new directory.")
    cmd("FM CREATE_DIR with DIRECTORY #{dir_name}")  
	elsif (cmd == "DELETE_DIR")
    dir_name = ask_string("Enter full path of directory to be deleted.")
    cmd("FM DELETE_DIR with DIRECTORY #{dir_name}")  
	elsif (cmd == "LIST_DIR_TO_PKT")
    dir_name = ask_string("Enter full path of directory to be listed.")
    offset = ask_string("Enter starting offset (0..N) of first file to list.")
    cmd("FM SEND_DIR_PKT with DIRECTORY #{dir_name}, DIRLISTOFFSET #{offset}")  
	elsif (cmd == "WRITE_DIR_TO_FILE")
    dir_name = ask_string("Enter full path of directory to be listed.")
    flt_full_file_name = ask_string("Enter full FSW path/filename that will be written.")
    cmd("FM WRITE_DIR with DIRECTORY #{dir_name}, FILENAME #{flt_full_file_name}")  
	elsif (cmd == "COPY_FILE")
    src_file_name = ask_string("Enter full source path/filename.")
    dst_file_name = ask_string("Enter full destination path/filename.")
    cmd("FM COPY_FILE with SOURCE #{src_file_name}, TARGET #{dst_file_name}")  
	elsif (cmd == "MOVE_FILE")
    src_file_name = ask_string("Enter full source path/filename.")
    dst_file_name = ask_string("Enter full destination path/filename.")
    overwrite_str = combo_box("Overwrite destination if it exists?", 'Yes','No')
    if (overwrite_str == 'Yes') 
      overwrite = 1
    else
      overwrite = 0
    end
    cmd("FM MOVE_FILE with OVERWRITE #{overwrite}, SOURCE #{src_file_name}, TARGET #{dst_file_name}")  
	elsif (cmd == "RENAME_FILE")
    src_file_name = ask_string("Enter full source path/filename.")
    dst_file_name = ask_string("Enter full destination path/filename.")
    cmd("FM RENAME_FILE with SOURCE #{src_file_name}, TARGET #{dst_file_name}")  
	elsif (cmd == "DELETE_FILE")
    file_name = ask_string("Enter full path/filename of file to be deleted.")
    cmd("FM DELETE_FILE with FILENAME #{file_name}")  
	elsif (cmd == "DELETE_ALL_FILES")
    dir_name = ask_string("Enter full path of directory to delete files.")
    cmd("FM DELETE_ALL_FILES with DIRECTORY #{dir_name}")  
	elsif (cmd == "DECOMPRESS_FILE")
    src_file_name = ask_string("Enter full source path/filename.")
    dst_file_name = ask_string("Enter full destination path/filename.")
    cmd("FM DECOMPRESS_FILE with SOURCE #{src_file_name}, TARGET #{dst_file_name}")  
	elsif (cmd == "CONCAT_FILE")
    src1_file_name = ask_string("Enter first full source path/filename.")
    src2_file_name = ask_string("Enter second full source path/filename.")
    dst_file_name = ask_string("Enter full destination path/filename.")
    cmd("FM CONCAT_FILES with SOURCE1 #{src1_file_name}, SOURCE2 #{src2_file_name}, TARGET #{dst_file_name}")  
	elsif (cmd == "GET_FILE_INFO")
    #TODO - Add variability. Default to CFE_ES_CRC_16 (only one implemented)
    file_name = ask_string("Enter full source path/filename.")
    display("FM FILE_INFO_PKT",50,50)
    cmd("FM GET_FILE_INFO with #{file_name}, FILEINFOCRC 2")  
	elsif (cmd == "LIST_OPEN_FILES")
    display("FM OPEN_FILES_PKT",50,50)
    cmd("FM GET_OPEN_FILES")  
  else
    prompt("Error in screen definition file. Undefined commmand sent to file_mgmt_send_cmd()")
  end
  
end # file_mgmt_send_cmd()
