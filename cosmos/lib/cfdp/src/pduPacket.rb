require "utils_visiona/utils" unless defined?(UTILS_VISIONA)

module CFDP

  class PDUPacket

    attr_accessor :pduHeader
    attr_accessor :pduPayload

    def initialize(*content)

      if (content[0].is_a?(Array))

        content = content[0]
        @pduHeader = CFDP::PDUHeader.new(content)
        payloadData = content[@pduHeader.headerSize..content.length]

        if (@pduHeader.pduType == 1)

          @pduPayload = CFDP::PDUFileData.new(payloadData)
        else

          fdCode = content[@pduHeader.headerSize]
          payloadData = payloadData[1..payloadData.length]

          @pduPayload = case fdCode
            when 4 then CFDP::PDUEOF.new(payloadData)
            when 5 then CFDP::PDUFinished.new(payloadData)
            when 6 then CFDP::PDUACK.new(payloadData)
            when 7 then CFDP::PDUMetadata.new(payloadData)
            when 8 then CFDP::PDUNAK.new(payloadData)
            when 9 then raise "Not implemented Prompt PDU"
            when 10 then raise "Not implemented Keep Alive PDU"
            else raise "Unknown directive code"
          end
        end
      end
    end

    def pack

      packet = @pduHeader.pack + @pduPayload.pack
      crc = []

      if @pduHeader.crcFlag == 1

        crc_calc = CFDP.calculateCRC(packet.dup)
        crc = [Utils_visiona.getBits(crc_calc, 9, 16),
              Utils_visiona.getBits(crc_calc, 1, 8)]
      end 

      return packet + crc
    end
  end # end class pduPacket
end # end module CFDP