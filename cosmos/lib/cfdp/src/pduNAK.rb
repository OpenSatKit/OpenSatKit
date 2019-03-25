require "utils_visiona/utils" unless defined?(UTILS_VISIONA)

module CFDP

  class PDUNAK

    attr_accessor :scopeStart
    attr_accessor :scopeEnd
    attr_accessor :segmentRequests

    @@fdCode = 8
    @@minLength = 16
    @@totalVariblesInstanceds = 3

    def initialize(*content)

      # Content empty means creation of empty PDUMetadata
      return if content.empty?
      content = content[0]

      if (content.is_a?(Array))

        # verify length
        Utils_visiona.verifyLength("less", content.length, @@minLength)

        # Verify if we have a Byte array
        Utils_visiona.checkByteArray(content)

        # set stuffs
        @scopeStart = ((content[0]<<24) + (content[1]<<16) + (content[2]<<8) + content[3])
        @scopeEnd = ((content[4]<<24) + (content[5]<<16) + (content[6]<<8) + content[7])

        @segmentRequests = Array.new
        totalSegmentsRequests = (content.length-8)/8
        hashAux = {}
        for i in 1..totalSegmentsRequests
          startOffset = ((content[8*i]<<24) + (content[(8*i)+1]<<16) + (content[(8*i)+2]<<8) + content[(8*i)+3])
          endOffset = ((content[(8*i)+4]<<24) + (content[(8*i)+5]<<16) + (content[(8*i)+6]<<8) + content[(8*i)+7])
          hashAux[:startOffset] = startOffset
          hashAux[:endOffset] = endOffset
          @segmentRequests << hashAux.dup
        end
      elsif (content.is_a?(Hash))

        @scopeStart = content[:scopeStart] if Utils_visiona.hasSymbol?(content, :scopeStart, Integer, 32)
        @scopeEnd = content[:scopeEnd] if Utils_visiona.hasSymbol?(content, :scopeEnd, Integer, 32)
        @segmentRequests = content[:segmentRequests] unless content[:segmentRequests].nil?
      else

        raise Utils_visiona::VerifyError, "Not a valid input for PDUNAK with class #{content.class}"
      end
    end # end initialize

    def length

      # this generates pdu length
      return 1 + 8 + (@segmentRequests.length)*8
    end

    def valid?

      begin

        # Check for instanceds vars
        Utils_visiona.compareValues(self.instance_variables.length, @@totalVariblesInstanceds, "variables instanceds")

        # Check for types instanceds
        return false unless @scopeStart.is_a?(Integer)
        return false unless @scopeEnd.is_a?(Integer)
        return false unless @segmentRequests.is_a?(Array)
      rescue

        return false
      end

      return true
    end

    def pack

      raise Utils_visiona::VerifyError, "Not a valid PDU NAK" unless valid?

      binArray = Array.new
      binArray << @@fdCode
      binArray << Utils_visiona.getBits(@scopeStart, 25, 32)
      binArray << Utils_visiona.getBits(@scopeStart, 17, 24)
      binArray << Utils_visiona.getBits(@scopeStart, 9, 16)
      binArray << Utils_visiona.getBits(@scopeStart, 1, 8)
      binArray << Utils_visiona.getBits(@scopeEnd, 25, 32)
      binArray << Utils_visiona.getBits(@scopeEnd, 17, 24)
      binArray << Utils_visiona.getBits(@scopeEnd, 9, 16)
      binArray << Utils_visiona.getBits(@scopeEnd, 1, 8)

      for i in 0..@segmentRequests.length-1

        binArray << Utils_visiona.getBits(@segmentRequests[i][:startOffset], 25, 32)
        binArray << Utils_visiona.getBits(@segmentRequests[i][:startOffset], 17, 24)
        binArray << Utils_visiona.getBits(@segmentRequests[i][:startOffset], 9, 16)
        binArray << Utils_visiona.getBits(@segmentRequests[i][:startOffset], 1, 8)
        binArray << Utils_visiona.getBits(@segmentRequests[i][:endOffset], 25, 32)
        binArray << Utils_visiona.getBits(@segmentRequests[i][:endOffset], 17, 24)
        binArray << Utils_visiona.getBits(@segmentRequests[i][:endOffset], 9, 16)
        binArray << Utils_visiona.getBits(@segmentRequests[i][:endOffset], 1, 8)
      end

      # this must be a byte array (0-255 values)
      for i in 0..binArray.length-1 do
        binArray[i] = binArray[i] & 0xFF
      end

      return binArray
    end

    def to_s

      raise Utils_visiona::VerifyError, "Not a valid PDU NAK" unless valid?

      output = ""
      output << "This is the beginning of the NAK PDU\n"
      output << "Start of scope is #{@scopeStart}\n"
      output << "End of scope is #{@scopeEnd}\n"
      output << "totalSegmentsRequests = #{@segmentRequests.length}\n"

      for i in 1..@segmentRequests.length

        output << "segmentRequest #{i}\n"
        output << "Start offset = #{@segmentRequests[i-1][:startOffset]}\n"
        output << "End offset = #{@segmentRequests[i-1][:endOffset]}\n"
      end
      output << "This is ending of the NAK PDU\n"

      return output
    end
  end # end class PDUNAK
end # end module CFDP