###############################################################################
# File Transfer
#
# Notes:
#   1. Abstract file transfer services so different protocols can be used
#      Currently hard coded for TFTP.
#   2. The orginal design concept evolved from having a single file transfer 
#      protocol to using both TFTP and CFDP. CFDP is used/configured for the
#      reference mission and TFTP has been left in place for OSK
#      "infrastructure". 
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
   
   #
   # The simple get/put methods are designed to be used in oeprational type
   # procedures that are synchronous and have user interaction. The threaded
   # get/put versions are for background tasks.
   #
   class TftpFileTransfer < FileTransfer

      attr_reader :tftp
  
      def initialize(ip_addr = Osk::COSMOS_IP_ADDR)
         @tftp = TFTP.new(ip_addr)
      end
  
      def get(flt_filename, gnd_filename, tlm_timeout = Osk::TFTP_GET_TIMEOUT)
  
         got_file = true
         # TFTP uses UDP directly without cmd interface so can't use cmd counters to verify execution
         get_file_cnt = tlm("TFTP HK_TLM_PKT GET_FILE_COUNT")
         seq_cnt = tlm("TFTP HK_TLM_PKT CCSDS_SEQUENCE")
         @tftp.getbinaryfile(flt_filename, gnd_filename)
         wait("TFTP HK_TLM_PKT GET_FILE_COUNT == #{get_file_cnt}+1", tlm_timeout)  # Delay until get file count increments or timeout
         if (tlm("TFTP HK_TLM_PKT CCSDS_SEQUENCE") == seq_cnt)
            prompt ("No telemetry received to verify the error. Verify connection and telemetry output filter table.");
            got_file = false  
         end
      
         return got_file 
    
      end # get()

      def put (gnd_filename, flt_filename, tlm_timeout = Osk::TFTP_PUT_TIMEOUT)
  
         put_file = true
         # TFTP uses UDP directly without cmd interface so can't use cmd counters to verify execution
         put_file_cnt = tlm("TFTP HK_TLM_PKT PUT_FILE_COUNT")
         seq_cnt = tlm("TFTP HK_TLM_PKT CCSDS_SEQUENCE")
         @tftp.putbinaryfile(gnd_filename, flt_filename)
         wait("TFTP HK_TLM_PKT PUT_FILE_COUNT == #{put_file_cnt}+1", tlm_timeout)  # Delay until put file count increments or timeout
         if (tlm("TFTP HK_TLM_PKT CCSDS_SEQUENCE") == seq_cnt)
            prompt ("No telemetry received to verify the error. Verify connection and telemetry output filter table.");
            put_file = false  
         end
      
         return put_file 
    
      end # put()

      def get_threaded(flt_filename, gnd_filename)

         got_file = false
         
         transfer = Thread.new do
            begin
               @tftp.getbinaryfile(flt_filename, gnd_filename)
               got_file = true
            rescue TFTPTimeout => e
               # Do nothing since errors will be reported by caller
            end
         end
         
         transfer.join
         return got_file
          
      end # get_threaded()

      def put_threaded (gnd_filename, flt_filename)
      
         put_file = false
         
         transfer = Thread.new do
            begin
               @tftp.putbinaryfile(gnd_filename, flt_filename)
               put_file = true
            rescue TFTPTimeout => e
               # Do nothing since errors will be reported by caller
            end
         end
         
         transfer.join
         return put_file

      end # put_threaded()


   end # Class FileTransfer

end # Module Osk


  
