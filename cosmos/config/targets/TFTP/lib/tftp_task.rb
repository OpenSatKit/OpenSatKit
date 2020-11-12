###############################################################################
# Trivial File Transfer Protocol (TFTP) Task
#
# Notes:
#   1. This must be installed by placing the following statement in 
#      cmd_tlm_server.txt
#           BACKGROUND_TASK tftp_task.rb
#   2. The FSW TFTP app is always the TFTP server. This design choice was
#      made so the OSK installation does not require the user to install
#      a TFTP server. File transfers can be initiated from the ground by
#      using "Osk::system.file_transfer".  It would also be desirable to
#      initiate file transfers by commanding the FSW TFTP app and this task
#      supports this model. The FSW TFTP app responds to a file transfer
#      request by sending a TFTP_TRANSFER_REQ telemetry packet. This task
#      polls for this message and when it receievs one it initiates the
#      file transfer, This is still not the same as implementing the TFTP
#      protocol across the cmd/tlm link like CFDP but it allows ground
#      an flight scripts/stored commands to initiate file transfers by
#      commanding the TFTP FSW app.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################
require "cosmos"
require "cosmos/tools/cmd_tlm_server/background_task"
require "osk_system"
require "file_transfer"
require "tftp"

module Cosmos

   class TftpTask < BackgroundTask

      NAME = "TFTP Task"
      
      GET_REQ = "1"
      PUT_REQ = "2"
      
      TRANSFER_REQ = { GET_REQ => "GET", PUT_REQ => "PUT"}
      
      attr_reader :name
      attr_reader :status

      attr_reader :get_count
      attr_reader :put_count
      
      def initialize

         super()
         @name = NAME
         @status = "Initializing"
         Logger.info "#{@name} #{@status}"

         @get_count = 0
         @put_count = 0
         
      end

      def call

         @sleeper = Sleeper.new
         @status = "Sleeping for 5 seconds after initial call"
         Logger.info "#{@name} #{@status}"
         return if @sleeper.sleep(5) # Allow interfaces time to start
         @status = "Running"
         Logger.info "#{@name} #{@status}"
         
         last_seq_count = tlm("TFTP TRANSFER_REQ_PKT CCSDS_SEQUENCE")
         
         loop do
         
            begin
               curr_seq_count = tlm("TFTP TRANSFER_REQ_PKT CCSDS_SEQUENCE")
               # Ignore rollover for now, highly unlike in OSK environment
               if (last_seq_count < curr_seq_count)
                  req_str = tlm("TFTP TRANSFER_REQ_PKT CMD_CODE").to_s
                  Logger.info "#{@name} initiating a #{TRANSFER_REQ[req_str]} transfer request"
                  src_file  = tlm("TFTP TRANSFER_REQ_PKT SRC_FILE")
                  dest_file = tlm("TFTP TRANSFER_REQ_PKT DEST_FILE")
                  case req_str
                  when GET_REQ
                     if (Osk::system.file_transfer.get_threaded(src_file, dest_file))
                        Logger.info "#{@name} successfully transferred #{src_file} to #{dest_file}"
                     else
                        Logger.error "#{@name} failed to transfer #{src_file} to #{dest_file}"
                     end
                  when PUT_REQ   
                     if (Osk::system.file_transfer.put_threaded(src_file, dest_file))
                        Logger.info "#{@name} successfully transferred #{src_file} to #{dest_file}"
                     else
                        Logger.error "#{@name} failed to transfer #{src_file} to #{dest_file}"
                     end
                  else
                     Logger.error "#{@name} telemetry request contained an invalid command code #{req_str}"
                  end
               end
               last_seq_count = curr_seq_count

               return if @sleeper.sleep(1)
            
            rescue exception => e
               Logger.error "#{@name} exception #{e}. Backtrace #{e.backtrace}"
               @status = "Exception #{e}"
            end
            
         end # loop
         
         @status = "Finished at #{Time.now.sys.formatted}"
         Logger.info "#{@name} #{@status}"
      
      end # Call()         
         
      def stop

         @status = "Stopped at #{Time.now.sys.formatted}"
         Logger.info "#{@name} #{@status}"
         @sleeper.cancel unless @sleeper.nil? # Could be nil if stop() called before call() 
		
      end # stop()
	
   end # TftpTask
end # module Cosmos