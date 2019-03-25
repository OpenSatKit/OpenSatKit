require "utils_visiona/utils" unless defined?(UTILS_VISIONA)

# Warning:
# This version does not support TLVs,
# so there is no Fault Location field (and it will be ignored if given).

module CFDP

  class PDUEOF

    attr_accessor :conditionCode
    attr_accessor :fileChecksum
    attr_accessor :fileSize

    @@fdCode = 4
    @@minLength = 9
    @@totalVariablesInstanceds = 3

    def initialize(*content)

      # Content empty means creation of empty PDUEOF
      return if content.empty?
      content = content[0]

      if (content.is_a?(Array)) # i'm expecting a byte array

        # first verify length of given pduEOF
        Utils_visiona.verifyLength("less", content.length, @@minLength)

        # Verify if we have a Byte array
        Utils_visiona.checkByteArray(content)

        # set stuff
        @conditionCode = Utils_visiona.getBits(content[0], 5, 8)
        @fileChecksum = ((content[1]<<24) + (content[2]<<16) + (content[3]<<8) + (content[4]))
        @fileSize = ((content[5]<<24) + (content[6]<<16) + (content[7]<<8) + (content[8]))
      elsif (content.is_a?(Hash))

        @conditionCode = content[:conditionCode] if Utils_visiona.hasSymbol?(content, :conditionCode, Integer, 4)
        @fileChecksum = content[:fileChecksum] if Utils_visiona.hasSymbol?(content, :fileChecksum, Integer, 32)
        @fileSize = content[:fileSize] if Utils_visiona.hasSymbol?(content, :fileSize, Integer, 32)
      else

        raise Utils_visiona::VerifyError, "Not a valid input for PDUEOF with class #{content.class}"
      end
    end

    def valid?

      begin

        # Check for instanceds vars
        Utils_visiona.compareValues(self.instance_variables.length, @@totalVariablesInstanceds, "variables instanceds")

        # Check for types instanceds
        return false unless @conditionCode.is_a?(Integer)
        return false unless @fileChecksum.is_a?(Integer)
        return false unless @fileSize.is_a?(Integer)
      rescue

        return false
      end

      return true
    end

    def length

      # 1 for FD Code + 9 for content
      return 1 + 9
    end

    def pack

      raise Utils_visiona::VerifyError, "Not a valid PDU EOF" unless valid?

      binArray = Array.new
      binArray << @@fdCode
      binArray << (Utils_visiona.getBits(@conditionCode, 1, 4) << 4)
      for i in (0..3); binArray << Utils_visiona.getBits(@fileChecksum, 1+8*(3-i), 8+8*(3-i)); end
      for i in (0..3); binArray << Utils_visiona.getBits(@fileSize, 1+8*(3-i), 8+8*(3-i)); end

      # this must be a byte array (0-255 values)
      for i in 0..binArray.length-1 do
        binArray[i] = binArray[i] & 0xFF
      end

      return binArray
    end

    def to_s

      raise Utils_visiona::VerifyError, "Not a valid PDU EOF" unless valid?

      output = ""
      output << "This is beginning of EOF info\n"
      output << "Condition Code: #{@conditionCode} (#{CFDP.conditionCodeToStr(@conditionCode)})\n"
      output << "File Checksum: #{@fileChecksum}\n"
      output << "File Size: #{@fileSize}\n"
      output << "This is end of EOF info\n"

      return output
    end
  end # class PDUEOF
end # module CFDP