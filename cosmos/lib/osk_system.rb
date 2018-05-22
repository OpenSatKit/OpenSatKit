###############################################################################
# OSK System
#
# Notes:
#   1. System is the primary entry point into the OpenSatKit framework. It
#      serves as a central access point for all OSK configurations and
#      definitions. Whenever possible it uses existing COSMOS definitions
#      to prevent multiple definitions
#   2. System is designed as a Singleton 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'cosmos'
require 'file_transfer'

module Osk
   
   class System
   
      @@instance = nil
      
      # 
      # file_transfer configured for flight-ground file transfer protocol which
      # typically would be TFTP or CFDP
      #
      attr_reader :file_transfer
      
      def initialize
         raise "Osk::System created twice" unless @@instance.nil?
         init_variables
         @@instance = self
      end # End initialize()
      
      def init_variables
         @file_transfer = TftpFileTransfer.new()
      end # End init_variables()
    
      def switch_local_to_pisat_cfs(host_ip_addr=HOST_IP_ADDR, pisat_ip_addr=PISAT_IP_ADDR)
   
         if (interface_state(COSMOS_CFS_INT) == 'CONNECTED')
            # Done in case switch can be performed without shutting down an interface  
            cmd("KIT_TO ENABLE_TELEMETRY with IP_ADDR #{NULL_IP_ADDR}")
	         wait(2)
            disconnect_interface(COSMOS_CFS_INT)
            wait(5)
         end
         connect_interface(COSMOS_CFS_INT,pisat_ip_addr,CFS_CMD_PORT,CFS_TLM_PORT,nil,nil,128,nil,nil)
         wait(5)
         @file_transfer = TftpFileTransfer.new(pisat_ip_addr)
         cmd("KIT_TO ENABLE_TELEMETRY with IP_ADDR #{host_ip_addr}")
         wait(2)
         cmd("TFTP INIT_SOCKET with IP_ADDR #{host_ip_addr}")         
         #Cosmos.run_process("ruby tools/CmdSender -p \"KIT_TO ENABLE_TELEMETRY\"")
         #connect_interface("CFS_INT",udp_cs_interface.rb,192.168.0.1,1234,1235,nil,nil,128,nil,nil)
         #map_target_to_interface("CS","CFS_PI_INT")      
      end # End connect_to_pisat()
      
      def switch_pisat_to_local_cfs()

         if (interface_state(COSMOS_CFS_INT) == 'CONNECTED')
            # Done in case switch can be performed without shutting down an interface  
            cmd("KIT_TO ENABLE_TELEMETRY with IP_ADDR #{NULL_IP_ADDR}")
	         wait(2)
            disconnect_interface(COSMOS_CFS_INT)
            wait(5)
         end
         connect_interface(COSMOS_CFS_INT,"localhost",CFS_CMD_PORT,CFS_TLM_PORT,nil,nil,128,nil,nil)
         wait(5)
         @file_transfer = TftpFileTransfer.new(COSMOS_IP_ADDR)
         cmd("KIT_TO ENABLE_TELEMETRY with IP_ADDR #{COSMOS_IP_ADDR}")
         wait(2)
         cmd("TFTP INIT_SOCKET with IP_ADDR #{COSMOS_IP_ADDR}")         

      end # End connect_to_pisat()
      
      def connect_to_local_cfs()

         if (interface_state(COSMOS_CFS_INT) != 'CONNECTED')
            connect_interface(COSMOS_CFS_INT,"localhost",CFS_CMD_PORT,CFS_TLM_PORT,nil,nil,128,nil,nil)
         end
   
      end # End connect_to_pisat()

   end # System class
   
   # An instance of system is created outside of the module
   class << self; attr_accessor :system; end

end # Module Osk


Osk.system = Osk::System.new()

