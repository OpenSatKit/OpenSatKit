###############################################################################
# Demonstrate a file transfer from ground to flight using FILE_IN
#
# Notes:
#   1. Prototype script to show that the FILE_IN app basic functionality works
#   2. A test or operational script would have more automated checks to verify
#      that the transfer is working correctly and it would not include status
#      being printed
#   3. A separate thread should be used if the transfer needs to run in the
#      background
#
# Global Script Variables:
#   simsat_ops_enable - Boolean indicating whether ops example is active
#   simsat_ops_status - Text string displayed on ops example screen
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'fsw_config_param'
require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'osk_ops'
require 'file_in_const'

DEBUG = false
# demo_filename is the filename without a path
def demo_file_in_transfer(demo_filename, test_cancel_cmd=false)

   flt_demo_filename = File.join(Osk::FLT_SRV_DIR, demo_filename)
   gnd_demo_filename = File.join(FILE_IN_TEST_FILE_DIR, demo_filename)  

   file_crc   = 0
   bytes_read = 0
   data_segment_id = FswConfigParam::FILE_IN_DATA_SEG_ID_START
   
   demo_file_size = File.size(gnd_demo_filename) # Returns nil if file nonexistent or file size is zero
   
   if demo_file_size != nil
   
      transfer_file = Thread.new {
         
         Osk::flight.send_cmd("FILE_IN","START_TRANSFER with DEST_FILENAME #{flt_demo_filename}")
         if DEBUG; wait end
         message_box("Start transfer sent for #{flt_demo_filename}", "OK")
         open(gnd_demo_filename,'rb') do |demo_file|
         
            bytes_read = 0
            while bytes_read < demo_file_size
               remaining_bytes = (demo_file_size - bytes_read)
               data_segment_len = remaining_bytes < FswConfigParam::FILE_IN_DATA_SEG_MAX_LEN ? remaining_bytes : FswConfigParam::FILE_IN_DATA_SEG_MAX_LEN
               data = demo_file.read(data_segment_len)    
               bytes_read += data.size
               if DEBUG
                  puts "remaining_bytes = #{remaining_bytes}\n"
                  puts "data_segment_len = #{data_segment_len}\n"
                  puts "data = #{data}\n"
                  puts "bytes_read = #{bytes_read}\n"
                  puts "data_segment_id = #{data_segment_id}\n"
               end
               data = "#{"0x"+data.unpack('H*')[0]}"  ## "#{"0x"+data.pack('c*').unpack('H*')[0]}"
               Osk::flight.send_cmd("FILE_IN","DATA_SEGMENT with ID #{data_segment_id}, LEN #{data_segment_len}, DATA #{data}")
               message_box("Sent data segment #{data_segment_id}", "OK")
               data_segment_id += 1
               if (test_cancel_cmd == true)
                  Osk::flight.send_cmd("FILE_IN","CANCEL_TRANSFER")      
               end
            end
         end
         if DEBUG; wait end
         Osk::flight.send_cmd("FILE_IN","FINISH_TRANSFER with FILE_CRC #{file_crc}, FILE_LEN #{demo_file_size}, LAST_DATA_SEG_ID #{data_segment_id-1}")
         
      } # End new thread
      
   else
      message_box("Error accessing #{gnd_demo_filename}. Either the file does not exists or has a length of zero.",false)
   end # If !nil
   
end # demo_file_in_transfer()

#demo_file_in_transfer("file_in_test.txt")
#demo_file_in_transfer("file_in_test.txt", test_cancel_cmd=true)
#demo_file_in_transfer("simsat_dir.dat")
#demo_file_in_transfer("simsat_dir.dat", test_cancel_cmd=true)

