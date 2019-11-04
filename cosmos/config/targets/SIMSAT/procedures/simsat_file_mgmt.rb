###############################################################################
# SimpleSat (SimSat) File Management Utilities 
#
# Defines utilities used to manage SimSat recorder files.
#
# Notes:
#   1. There are 3 sources of files and these are their prioirities from
#      from highest to lowest:
#      A. DS event file
#      B. ISIM science files
#      C. DS auxiliary files
#   2. Assumes filenames contain one 3 character extension
#
# Global Script Variables:
#   simsat_ops_enable - Boolean indicating whether ops example is active
#   simsat_ops_status - Text string displayed on ops example screen
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require 'cfe_file'
require 'fsw_config_param'
require 'osk_global'
require 'osk_system'
require 'osk_flight'
require 'osk_ops'
require 'simsat_global'

#
# 1. Create and downlink a directory listing of the onboard 'recorder'
# 2. Parse directory listing and prioritize file downlink
# 3. Downlink files and remove from recorder. Do not try to downlink files that
#    are created during the pass. 
# TODO - Add logic to either skip open files or close them

def simsat_manage_recorder

   File.delete(SimSat::GND_DIR_LIST_FILE) if File.exist?(SimSat::GND_DIR_LIST_FILE)
   
   Osk::flight.send_cmd("FM","WRITE_DIR_TO_FILE with DIRECTORY #{SimSat::FLT_SRV_DIR}, FILENAME #{SimSat::FLT_DIR_LIST_FILE}")
   wait 2

   if (Osk::system.file_transfer.get(SimSat::FLT_DIR_LIST_FILE,SimSat::GND_DIR_LIST_FILE,20))
      file_hash = simsat_create_file_hash(SimSat::GND_DIR_LIST_FILE)
      file_hash.each do |file_type, file_list|
         file_list.each do |file| 
            flt_dir_filename = "#{SimSat::FLT_SRV_DIR}/#{file}"
            gnd_dir_filename = "#{SimSat::GND_SRV_DIR}/#{file}"
            if (Osk::system.file_transfer.get(flt_dir_filename,gnd_dir_filename,30))
               Osk::flight.send_cmd("FM","DELETE_FILE with FILENAME #{flt_dir_filename}")
            else
               message_box("Failed to transfer #{flt_dir_filename} to #{gnd_dir_filename}. Real ops environment would have a contingency procedure.",false)
            end
         end
      end
   else
      message_box("Failed to transfer #{SimSat::FLT_DIR_LIST_FILE} to #{SimSat::GND_DIR_LIST_FILE}. Real ops environment would have a contingency procedure.",false)
   end

end # simsat_manage_recorder()

#
# Parse the recorder file listing created by File Manager and created
# a dictionary containing containing file type (ISIM, EVENTS, AUX) as
# the keyword and an array of file names as the entry.
#
def simsat_create_file_hash(fm_dir_file)
         
   file_hash = {}
   
   dir_file = File.open(fm_dir_file,'r')
         
   file_hdr = dir_file.read(CfeFile::HDR_LEN)

   if file_hdr[0..3] == CfeFile::CONTENT_ID
        
      fm_hdr = dir_file.read(FswConfigParam::FM_DIR_FILE_HDR_LEN)
      
      if fm_hdr[0..SimSat::FLT_SRV_DIR.length-1] == SimSat::FLT_SRV_DIR
      
         num_files = fm_hdr[68..71].to_s.unpack('l')
         
         files = []
         num_files[0].times do
            file_rec = dir_file.read(FswConfigParam::FM_DIR_FILE_REC_LEN)
            file_ext_index = file_rec[0..63].index('.')
            file_name = file_rec[0..file_ext_index+3].to_s
            files << file_name
         end
         
         file_hash[:event]   = files.grep /^#{SimSat::EVENT_FILENAME_BASE}/
         file_hash[:sci_aux] = files.grep /^#{SimSat::SCI_AUX_FILENAME_BASE}/
         file_hash[:isim]    = files.grep /^#{SimSat::ISIM_FILENAME_BASE}/

      else

         message_box("FM file header does not contain the expected directory #{SimSat::FLT_SRV_DIR}",false)
      
      end
      
   else
         
      message_box("File header does not contain cFE file identifier #{CfeFile::CONTENT_ID}",false)

   end
   
   return file_hash
   
end # simsat_create_file_hash()