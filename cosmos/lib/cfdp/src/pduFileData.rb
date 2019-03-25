require "utils_visiona/utils" unless defined?(UTILS_VISIONA)

module CFDP

  class PDUFileData

    attr_accessor :offset
    attr_accessor :data

    @@minLength = 5
    @@totalVariblesInstanceds = 2

    def initialize(*content)

      # Content empty means creation of empty PDUFileData
      return if content.empty?
      content = content[0]

      if (content.is_a?(Array)) # i'm expecting a byte array

        # verify length of given pdu File Data
        Utils_visiona.verifyLength("less", content.length, @@minLength)

        #  Verify if we have a Byte array
        Utils_visiona.checkByteArray(content)

        # set offset and data
        @offset = ((content[0]<<24) + (content[1]<<16) + (content[2]<<8) + content[3])
        @data = content[4..content.length]
      elsif (content.is_a?(Hash))

        @offset = content[:offset] if Utils_visiona.hasSymbol?(content, :offset, Integer, 32)
        @data = content[:data] if Utils_visiona.hasSymbol?(content, :data, Array, 2**8)
      else

        raise Utils_visiona::VerifyError, "Not a valid input for PDUFileData with class #{content.class}"
      end
    end

    def valid?

      begin

        # Check for instanceds vars
        Utils_visiona.compareValues(self.instance_variables.length, @@totalVariblesInstanceds, "variables instanceds")

        # Check for types instanceds
        return false unless @offset.is_a?(Integer)
        return false unless @data.is_a?(Array)

        #  Verify if we have a Byte array data
        Utils_visiona.checkByteArray(@data)
      rescue

        return false
      end

      return true
    end

    def length

      # 4 bytes for offset + data length
      return 4 + @data.length
    end

    def pack

      raise Utils_visiona::VerifyError, "Not a valid PDU FileData" unless valid?

      binArray = Array.new
      binArray << Utils_visiona.getBits(@offset, 25, 32)
      binArray << Utils_visiona.getBits(@offset, 17, 24)
      binArray << Utils_visiona.getBits(@offset, 9, 16)
      binArray << Utils_visiona.getBits(@offset, 1, 8)
      binArray += @data # Guarantee that data is an array

      # this must be a byte array (0-255 values)
      for i in 0..binArray.length-1 do
        binArray[i] = binArray[i] & 0xFF
      end

      return binArray
    end

    def to_s

      raise Utils_visiona::VerifyError, "Not a valid PDUFileData" unless valid?

      output = ""
      output << "This is the beginning of the PDUFileData" << "\n"
      output << "Offset is #{@offset}" << "\n"
      output << "Data is " << data.to_s << "\n"
      output << "This is the ending of the PDUFileData" << "\n"

      return output
    end
  end # class PDUFileData
end # end module CFDP