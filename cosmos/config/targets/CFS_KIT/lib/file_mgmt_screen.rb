###############################################################################
# cFS Kit File Management 
#
# Notes:
#   1. Uses custom parameter prompts instead of general purpose FswApp methods
#      since it can be confusing when to use full path vs just a filename. In 
#      addition the cmd & tlm definition comments need to be improved if they 
#      are goign to be used for user input guidance.  
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_system'
require 'osk_flight'
require 'osk_ops'


################################################################################
## Send Commands
################################################################################

def file_mgmt_send_cmd(screen, cmd)
 

   if (cmd == "GET_FILE")
      # Update screen with working directories to help remind user where tables are stored
      Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_DIR, Osk::FLT_SRV_DIR)
      if (Osk::Ops::get_flt_file_prompt(Osk::GND_SRV_DIR))
         Osk::Ops::set_work_dir_widget(screen)
      end
   elsif (cmd == "PUT_FILE")
      # Update screen with working directories to help remind user where tables are stored
      Osk::Ops::set_work_dir_widget(screen, Osk::GND_SRV_DIR, Osk::FLT_SRV_DIR)
      if (Osk::Ops::put_flt_file_prompt(Osk::GND_SRV_DIR))
         Osk::Ops::set_work_dir_widget(screen)
      end
   elsif (cmd == "CREATE_DIR")
      dir_name = ask_string("Enter full path of new directory.")
      Osk::flight.fm.send_cmd("CREATE_DIR with DIRECTORY #{dir_name}")
   elsif (cmd == "DELETE_DIR")
      dir_name = ask_string("Enter full path of directory to be deleted.")
      Osk::flight.fm.send_cmd("DELETE_DIR with DIRECTORY #{dir_name}")  
   elsif (cmd == "LIST_DIR_TO_PKT")
      dir_name = ask_string("Enter full path of directory to be listed.",Osk::FLT_SRV_DIR)
      offset = ask_string("Enter starting offset (0..N) of first file to list.","0")
      Osk::flight.fm.send_cmd("SEND_DIR_PKT with DIRECTORY #{dir_name}, DIRLISTOFFSET #{offset}")  
   elsif (cmd == "WRITE_DIR_TO_FILE")
      flt_dir_name = ask_string("Enter full path of flight directory to be listed.",Osk::FLT_SRV_DIR)
      Osk::Ops::send_flt_bin_file_cmd("FM", "WRITE_DIR_TO_FILE with DIRECTORY #{flt_dir_name}, ", Osk::TBL_MGR_DEF_FM_DIR)
   elsif (cmd == "COPY_FILE")
      src_file_name = ask_string("Enter full source /path/filename.")
      dst_file_name = ask_string("Enter full destination /path/filename.")
      Osk::flight.fm.send_cmd("COPY_FILE with SOURCE #{src_file_name}, TARGET #{dst_file_name}")  
   elsif (cmd == "MOVE_FILE")
      src_file_name = ask_string("Enter full source /path/filename.")
      dst_file_name = ask_string("Enter full destination /path/filename.")
      overwrite_str = combo_box("Overwrite destination if it exists?", 'Yes','No')
      overwrite_str == 'Yes' ? overwrite = 1 : overwrite = 0
      Osk::flight.fm.send_cmd("MOVE_FILE with OVERWRITE #{overwrite}, SOURCE #{src_file_name}, TARGET #{dst_file_name}")  
   elsif (cmd == "RENAME_FILE")
      src_file_name = ask_string("Enter full source /path/filename.")
      dst_file_name = ask_string("Enter full destination /path/filename.")
      Osk::flight.fm.send_cmd("RENAME_FILE with SOURCE #{src_file_name}, TARGET #{dst_file_name}")  
   elsif (cmd == "DELETE_FILE")
      file_name = ask_string("Enter full /path/filename of file to be deleted.", Osk::FLT_SRV_DIR)
      Osk::flight.fm.send_cmd("DELETE_FILE with FILENAME #{file_name}")  
   elsif (cmd == "DELETE_ALL_FILES")
      dir_name = ask_string("Enter full path of directory to delete files.")
      Osk::flight.fm.send_cmd("DELETE_ALL_FILES with DIRECTORY #{dir_name}")  
   elsif (cmd == "DECOMPRESS_FILE")
      src_file_name = ask_string("Enter full source /path/filename.")
      dst_file_name = ask_string("Enter full destination /path/filename.")
      Osk::flight.fm.send_cmd("DECOMPRESS_FILE with SOURCE #{src_file_name}, TARGET #{dst_file_name}")  
   elsif (cmd == "CONCAT_FILE")
      src1_file_name = ask_string("Enter first full source /path/filename.")
      src2_file_name = ask_string("Enter second full source /path/filename.")
      dst_file_name = ask_string("Enter full destination /path/filename.")
      Osk::flight.fm.send_cmd("CONCAT_FILES with SOURCE1 #{src1_file_name}, SOURCE2 #{src2_file_name}, TARGET #{dst_file_name}")  
   elsif (cmd == "GET_FILE_INFO")
      file_name = ask_string("Enter full source /path/filename.", Osk::FLT_SRV_DIR)
      spawn("ruby #{Osk::COSMOS_PKT_VIEWER} -p 'FM FILE_INFO_PKT'")
      Osk::flight.fm.send_cmd("GET_FILE_INFO with FILENAME #{file_name}, CRC 2")  
   elsif (cmd == "LIST_OPEN_FILES")
      Cosmos.run_process("ruby tools/PacketViewer -p \"FM OPEN_FILES_PKT\"")
      wait (2)
      Osk::flight.fm.send_cmd("GET_OPEN_FILES")  
   else
      raise "Error in screen definition file. Undefined command '#{cmd}' sent to file_mgmt_send_cmd()"
   end
  
end # file_mgmt_send_cmd()
