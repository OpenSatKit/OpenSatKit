require 'cosmos/interfaces/interface'
require 'cfdp/cfdp'

module Cosmos

  # Base class for interfaces that send and receive messages over UDP
  class VisionaCfdpInterface < Interface

    CFDP_TARGET_NAME = "CFDP"
    HK_PACKET_TIMING = 4

    def initialize

      super
    end

    def connected?

      return true
    end

    def read

      while connected?

        sleep(HK_PACKET_TIMING)
        data = CFDP::CFDPEngine.instance.hkpacket.pack
        data = data.pack('c*')
        read_interface_base(data)
        packet = convert_data_to_packet(data)
        @read_count += 1

        return packet
      end
    end

    def write(packet)

      @write_count +=1
      command_data = packet.buffer
      @bytes_written += command_data.length
      @written_raw_data_time = Time.now
      @written_raw_data = command_data

      identified_command = System.commands.identify(command_data, [CFDP_TARGET_NAME])
      if identified_command
        case identified_command.packet_name
        when "SEND_FILE"
          begin
            classe = packet.read('CLASS', :FORMATTED).to_i
            destID = packet.read('DEST_ID', :FORMATTED).to_i
            srcFileName = packet.read('SRCFILENAME', :FORMATTED)
            dstFileName = packet.read('DSTFILENAME', :FORMATTED)
            CFDP::CFDPEngine.instance.uplinkRequest(classe, destID, srcFileName, dstFileName)
          rescue Exception => err
            Cosmos::Logger.error "Error while receiving SEND_FILE command. Error is #{err}.\n Backtrace #{err.backtrace}"
          end
        else
          raise "Unknown command received at SEND_FILE Interface."
        end
      else
        raise "Unknown command received at CFDP Interface."
      end
    end

    def write_raw(_data)
      raise "write_raw not implemented for CFDP Interface"
    end

    # Raise an error because raw logging is not supported for this interface
    def raw_logger_pair=(_raw_logger_pair)
      raise "Raw logging not supported for CFDP Interface"
    end
  end
end