###############################################################################
# Demonstrate a file transfer from ground to flight using FILE_IN
#
# Notes:
#   1. Prototype script to show that the FILE_IN app basci functionality works
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
require 'file_out_const'

# demo_filename is the filename without a path
def demo_file_in_transfer(demo_filename)

   flt_demo_filename = File.join(Osk::FLT_SRV_DIR,demo_filename)
   gnd_demo_filename = File.join(FILE_IN_TEST_FILE_DIR,demo_filename)  

   bytes_read = 0
   data_segment_id = FswConfigParam::FILE_IN_DATA_SEG_ID_START
   
   demo_file_size = File.size(gnd_demo_filename) # Returns nil if file nonexistent or file size is zero
   
   if demo_file_size != nil
   
      Osk::flight.send_cmd("FILE_IN","START_TRANSFER with DEST_FILENAME #{flt_demo_filename}")
      wait
      open(gnd_demo_filename,'rb') do |demo_file|
      
         bytes_read = 0
         while bytes_read < demo_file_size
            remaining_bytes = (demo_file_size - bytes_read)
            puts "remaining_bytes = #{remaining_bytes}\n"
            data_segment_len = remaining_bytes < FswConfigParam::FILE_IN_DATA_SEG_MAX_LEN ? remaining_bytes : FswConfigParam::FILE_IN_DATA_SEG_MAX_LEN
            puts "data_segment_len = #{data_segment_len}\n"
            data = demo_file.read(data_segment_len)    
            puts "data = #{data}\n"
            bytes_read += data.size
            puts "bytes_read = #{bytes_read}\n"
            puts "data_segment_id = #{data_segment_id}\n"
            data = "#{"0x"+data.unpack('H*')[0]}"  ## "#{"0x"+data.pack('c*').unpack('H*')[0]}"
            Osk::flight.send_cmd("FILE_IN","DATA_SEGMENT with ID #{data_segment_id}, LEN #{data_segment_len}, DATA #{data}")
            data_segment_id += 1
            # wait
         end
      end
      wait
      Osk::flight.send_cmd("FILE_IN","FINISH_TRANSFER with FILE_LEN #{bytes_read}, FILE_CRC 0, LAST_DATA_SEG_ID #{data_segment_id-1}")

   else
      message_box("Error accessing #{gnd_demo_filename}. Either the file does not exists or has a length of zero.",false)
   end # If !nil
   
end # demo_file_in_transfer()

#demo_file_in_transfer("file_in_test.txt")
demo_file_in_transfer("simsat_dir.dat")

