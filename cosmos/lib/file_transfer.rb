###############################################################################
# File Transfer
#
# Notes:
#   1. Abstract file transfer services so different protocols can be used
#      Currently hard coded for TFTP.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'osk_global'
require 'tftp'

module Osk

   class FileTransfer

      def initialize
      end
  
      def get(flt_filename, gnd_filename)
         raise NotImplementedError
      end # get()

      def put (gnd_filename, flt_filename)
         raise NotImplementedError
      end # put()
 
   end # Class FileTransfer
   
   class TftpFileTransfer < FileTransfer

      attr_reader :tftp
  
      def initialize(ip_addr = Osk::COSMOS_IP_ADDR)
         @tftp = TFTP.new(ip_addr)
      end
  
      def get(flt_filename, gnd_filename)
  
         got_file = true
         # TFTP uses UDP directly without cmd interface so can't use cmd counters to verify execution
         get_file_cnt = tlm("TFTP HK_TLM_PKT GET_FILE_COUNT")
         seq_cnt = tlm("TFTP HK_TLM_PKT CCSDS_SEQUENCE")
         @tftp.getbinaryfile(flt_filename, gnd_filename)
         wait("TFTP HK_TLM_PKT GET_FILE_COUNT == #{get_file_cnt}+1", 10)  # Delay until get file count increments or timeout
         if (tlm("TFTP HK_TLM_PKT CCSDS_SEQUENCE") == seq_cnt)
            prompt ("No telemetry received to verify the error. Verify connection and telemetry output filter table.");
            got_file = false  
         end
      
         return got_file 
    
      end # get()

      def put (gnd_filename, flt_filename)
  
         put_file = true
         # TFTP uses UDP directly without cmd interface so can't use cmd counters to verify execution
         put_file_cnt = tlm("TFTP HK_TLM_PKT PUT_FILE_COUNT")
         seq_cnt = tlm("TFTP HK_TLM_PKT CCSDS_SEQUENCE")
         @tftp.putbinaryfile(gnd_filename, flt_filename)
         wait("TFTP HK_TLM_PKT PUT_FILE_COUNT == #{put_file_cnt}+1", 10)  # Delay until put file count increments or timeout
         if (tlm("TFTP HK_TLM_PKT CCSDS_SEQUENCE") == seq_cnt)
            prompt ("No telemetry received to verify the error. Verify connection and telemetry output filter table.");
            put_file = false  
         end
      
         return put_file 
    
      end # put()

   end # Class FileTransfer

end # Module Osk


  
