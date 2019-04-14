require "utils_visiona/utils" unless defined?(UTILS_VISIONA)

module CFDP

  class PDUFinished

    attr_accessor :conditionCode
    attr_accessor :endSystemStatus
    attr_accessor :deliveryCode
    attr_accessor :fileStatus

    @@fdCode = 5
    @@minLength = 1
    @@totalVariablesInstanceds = 4

    def initialize(*content)

      # Content empty means creation of empty PDUFinished
      return if content.empty?
      content = content[0]

      if (content.is_a?(Array)) # i'm expecting a byte array

        # verify length of given pdu Finished
        Utils_visiona.verifyLength("less", content.length, @@minLength)

        # Verify if we have a Byte array
        Utils_visiona.checkByteArray(content)

        # set offset and data
        @conditionCode = Utils_visiona.getBits(content[0], 5, 8)
        @endSystemStatus = Utils_visiona.getBits(content[0], 4, 4)
        @deliveryCode = Utils_visiona.getBits(content[0], 3, 3)
        @fileStatus = Utils_visiona.getBits(content[0], 1, 2)
      elsif (content.is_a?(Hash))

        @conditionCode = content[:conditionCode] if Utils_visiona.hasSymbol?(content, :conditionCode, Integer, 4)
        @endSystemStatus = content[:endSystemStatus] if Utils_visiona.hasSymbol?(content, :endSystemStatus, Integer, 1)
        @deliveryCode = content[:deliveryCode] if Utils_visiona.hasSymbol?(content, :deliveryCode, Integer, 1)
        @fileStatus = content[:fileStatus] if Utils_visiona.hasSymbol?(content, :fileStatus, Integer, 2)
      else

        raise Utils_visiona::VerifyError, "Not a valid input for PDUFinished with class #{content.class}"
      end
    end

    def valid?

      begin

        # Check for instanceds vars
        Utils_visiona.compareValues(self.instance_variables.length, @@totalVariablesInstanceds, "variables instanceds")

        # Check for types instanceds
        return false unless @conditionCode.is_a?(Integer)
        return false unless @endSystemStatus.is_a?(Integer)
        return false unless @deliveryCode.is_a?(Integer)
        return false unless @fileStatus.is_a?(Integer)
      rescue

        return false
      end

      return true
    end

    def length

      # 1 for FD Code + 1 for payload
      return 1 + 1
    end

    def pack

      raise Utils_visiona::VerifyError, "Not a valid PDU Finished" unless valid?

      binArray = Array.new
      binArray << @@fdCode
      binArray << ((@conditionCode << 4) + (@endSystemStatus << 3) + (@deliveryCode << 2) + @fileStatus)

      # this must be a byte array (0-255 values)
      for i in 0..binArray.length-1 do
        binArray[i] = binArray[i] & 0xFF
      end

      return binArray
    end

    def to_s

      raise Utils_visiona::VerifyError, "Not a valid PDU Finished" unless valid?

      output = ""
      output << "This is the beginning of the FINISHED Pdu\n"
      output << "End System Status: #{@endSystemStatus}\n"
      output << "Condition Code: #{@conditionCode.to_s} (#{CFDP.conditionCodeToStr(@conditionCode)})\n"
      output << "Deilivery Code: #{@deliveryCode}\n"
      output << "File Status: #{@fileStatus}\n"
      output << "This is the ending of FINISHED Pdu\n"

      return output
    end
  end # class PDUFinished
end # module CFDP