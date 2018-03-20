###############################################################################
# cFS Kit File Management 
#
# Notes:
#   1. TODO - Update FILE_MGMT_FLT_SRV_DIR & FILE_MGMT_GND_SRV_DIR with user changes
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'cfs_kit_global'
require 'file_transfer'

################################################################################
## Global Variables
################################################################################

FILE_MGMT_FLT_SRV_DIR = FLT_SRV_DIR
FILE_MGMT_GND_SRV_DIR = GND_SRV_DIR

################################################################################
## Configure Screen
################################################################################

def file_mgmt_set_work_dir(screen)

  gnd_work_dir_widget = screen.get_named_widget("gnd_work_dir")
  gnd_work_dir_widget.text = FILE_MGMT_GND_SRV_DIR

  flt_work_dir_widget = screen.get_named_widget("flt_work_dir")
  flt_work_dir_widget.text = FILE_MGMT_FLT_SRV_DIR

end # file_mgmt_set_work_dir()

################################################################################
## Send Commands
################################################################################

def file_mgmt_send_cmd(screen, cmd)
 
  if (cmd == "GET_FILE")
    flt_full_file_name = ask_string("Enter full FSW path/filename.")
    gnd_file_name = ask_string("Enter ground filename without path. File will be in kit server location.")
    gnd_full_file_name = "#{GND_SRV_DIR}/#{gnd_file_name}"
    $file_xfer = Osk::system.file_transfer
    $file_xfer.get(flt_full_file_name,gnd_full_file_name)
  elsif (cmd == "PUT_FILE")
    gnd_file_name = ask_string("Enter ground filename without path. File will be in kit server location.")
    gnd_full_file_name = "#{GND_SRV_DIR}/#{gnd_file_name}"
    flt_full_file_name = ask_string("Enter full FSW path/filename.")
    $file_xfer = Osk::system.file_transfer
    $file_xfer.put(gnd_full_file_name,flt_full_file_name)
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
    Cosmos.run_process("ruby tools/PacketViewer -p \"FM OPEN_FILES_PKT\"")
    cmd("FM GET_OPEN_FILES")  
  else
    prompt("Error in screen definition file. Undefined command '#{cmd}' sent to file_mgmt_send_cmd()")
  end
  
end # file_mgmt_send_cmd()
