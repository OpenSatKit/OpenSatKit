require "utils_visiona/utils" unless defined?(UTILS_VISIONA)

module CFDP

  class PDUACK

    attr_accessor :directiveCode
    attr_accessor :directiveSubtypeCode
    attr_accessor :conditionCode
    attr_accessor :transactionStatus

    @@fdCode = 6
    @@minLength = 2
    @@totalVariblesInstanceds = 4

    def initialize(*content)

      # Content empty means creation of empty PDUACK
      return if content.empty?
      content = content[0]

      if (content.is_a?(Array))

        # First verify length of given pduACK
        Utils_visiona.verifyLength("less", content.length, @@minLength)

        # Verify if we have a Byte array
        Utils_visiona.checkByteArray(content)

        @directiveCode = Utils_visiona.getBits(content[0], 5, 8)
        @directiveSubtypeCode = Utils_visiona.getBits(content[0], 1, 4)
        @conditionCode = Utils_visiona.getBits(content[1], 5, 8)
        @transactionStatus = Utils_visiona.getBits(content[1], 1, 2)
      elsif (content.is_a?(Hash))

        @directiveCode = content[:directiveCode] if Utils_visiona.hasSymbol?(content, :directiveCode, Integer, 4)
        @directiveSubtypeCode = content[:directiveSubtypeCode] if Utils_visiona.hasSymbol?(content, :directiveSubtypeCode, Integer, 4)
        @conditionCode = content[:conditionCode] if Utils_visiona.hasSymbol?(content, :conditionCode, Integer, 4)
        @transactionStatus = content[:transactionStatus] if Utils_visiona.hasSymbol?(content, :transactionStatus, Integer, 2)
      else

        raise Utils_visiona::VerifyError, "Not a valid input for PDUACK with class #{content.class}"
      end
    end

    def valid?

      begin

        # Check for instanceds vars
        Utils_visiona.compareValues(self.instance_variables.length, @@totalVariblesInstanceds, "variables instanceds")

        # Check for types instanceds
        return false unless @directiveCode.is_a?(Integer)
        return false unless @directiveSubtypeCode.is_a?(Integer)
        return false unless @conditionCode.is_a?(Integer)
        return false unless @transactionStatus.is_a?(Integer)
      rescue

        return false
      end

      return true
    end

    def length

      # 1 for fdCode and 2 for stuffs
      return 1 + 2
    end

    def pack

      raise Utils_visiona::VerifyError, "Not a valid PDU Ack" unless valid?

      binArray = Array.new
      binArray << @@fdCode
      binArray << ((@directiveCode << 4) + (@directiveSubtypeCode))
      binArray << ((@conditionCode << 4) + @transactionStatus)

      # this must be a byte array (0-255 values)
      for i in 0..binArray.length-1 do
        binArray[i] = binArray[i] & 0xFF
      end

      return binArray
    end

    def to_s

      raise Utils_visiona::VerifyError, "Not a valid PDU Ack" unless valid?

      output = ""
      output << "This is the beginning of the ACK Pdu\n"
      output << "Directive Code: #{@directiveCode}\n"
      output << "Directive Subtype Code: #{@directiveSubtypeCode}\n"
      output << "Condition Code: #{@conditionCode} (#{CFDP.conditionCodeToStr(@conditionCode)})\n"
      output << "transaction Status: #{@transactionStatus}\n"
      output << "This is the ending of the ACK Pdu\n"

      return output
    end
  end # end class PDUACK
end # end module CFDP