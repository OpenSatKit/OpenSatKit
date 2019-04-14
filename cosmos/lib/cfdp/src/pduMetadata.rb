require "utils_visiona/utils" unless defined?(UTILS_VISIONA)

# Does not support:
# - Messages to User
# - File of unbounded size
# - Filestore requests
# - Fault Handler overrides
# - Flow label

module CFDP

  class PDUMetadata

    attr_accessor :segmentationControl
    attr_accessor :fileSize #in octets (bytes)
    attr_accessor :sourceFileName
    attr_accessor :destinationFileName

    @@fdCode = 7
    @@minLength = 7
    @@totalVariblesInstanceds = 4

    def initialize(*content)

      # Content empty means creation of empty PDUMetadata
      return if content.empty?
      content = content[0]

      if (content.is_a?(Array)) # I'm expecting a byte array

        # we first verify length of given pduPayload
        Utils_visiona.verifyLength("less", content.length, @@minLength)

        # Verify if we have a Byte array
        Utils_visiona.checkByteArray(content)

        # Verify bit 2-8
        Utils_visiona.compareValues(Utils_visiona.getBits(content[0], 1, 7), 0, "on bits \#2-8 of PDUMetadata")

        # set first 5 bytes
        @segmentationControl = Utils_visiona.getBits(content[0], 8, 8)
        @fileSize = (content[1]<<24) + (content[2]<<16) + (content[3]<<8) + content[4]

        # first LV value (guaranteed that has this length, because minlength is 7)
        lengthSourceFileName = content[5]
        Utils_visiona.verifyLength("less", content.length, @@minLength+lengthSourceFileName)
        @sourceFileName = ""; for i in 0..(lengthSourceFileName-1); @sourceFileName << content[6+i].chr; end unless lengthSourceFileName <= 0

        # must check if we have at least one more byte for second LV value
        Utils_visiona.verifyLength("less", content.length, 5+(1+lengthSourceFileName))

        # second LV value
        lengthDestinationFileName = content[6+lengthSourceFileName]
        Utils_visiona.verifyLength("less", content.length, 5 + (1+lengthSourceFileName) + (1+lengthDestinationFileName))
        @destinationFileName = ""; for i in 0..(lengthDestinationFileName-1); @destinationFileName << content[5+lengthSourceFileName+2+i].chr; end unless lengthDestinationFileName <= 0
      elsif (content.is_a?(Hash))

        @segmentationControl = content[:segmentationControl] if Utils_visiona.hasSymbol?(content, :segmentationControl, Integer, 1)
        @fileSize = content[:fileSize] if Utils_visiona.hasSymbol?(content, :fileSize, Integer, 32)
        @sourceFileName = content[:sourceFileName] if Utils_visiona.hasSymbol?(content, :sourceFileName, String, 2**8)
        @destinationFileName = content[:destinationFileName] if Utils_visiona.hasSymbol?(content, :destinationFileName, String, 2**8)
      else

        raise Utils_visiona::VerifyError, "Not a valid input for PDUMetadata with class #{content.class}"
      end
    end

    def valid?

      begin

        # Check for instanceds vars
        Utils_visiona.compareValues(self.instance_variables.length, @@totalVariblesInstanceds, "variables instanceds")

        # Check for types instanceds
        return false unless @segmentationControl.is_a?(Integer)
        return false unless @fileSize.is_a?(Integer)
        return false unless @sourceFileName.is_a?(String)
        return false unless @destinationFileName.is_a?(String)
      rescue

        return false
      end

      return true
    end

    def length

      raise Utils_visiona::VerifyError if (@sourceFileName.nil? || @destinationFileName.nil?)

      # 1 for fdCode, 1 for SegmentationCtrl, 4 for file size, 2x LV Values (L = 1 byte, V = L bytes)
      return 1 + 1 + 4 + (1+ @sourceFileName.length) + (1 + @destinationFileName.length)
    end

    def pack

      raise Utils_visiona::VerifyError, "Not a valid PDU Metadata" unless valid?

      binArray = Array.new
      binArray << @@fdCode
      binArray << (@segmentationControl<<7)
      binArray << Utils_visiona.getBits(@fileSize, 25, 32)
      binArray << Utils_visiona.getBits(@fileSize, 17, 24)
      binArray << Utils_visiona.getBits(@fileSize, 9, 16)
      binArray << Utils_visiona.getBits(@fileSize, 1, 8)

      unless @sourceFileName.length == 0

        binArray << @sourceFileName.length
        binArray << Utils_visiona.strToDecArray(@sourceFileName)
      end
      unless @destinationFileName.length == 0

        binArray << @destinationFileName.length
        binArray << Utils_visiona.strToDecArray(@destinationFileName)
      end

      binArray = binArray.flatten

      # this must be a byte array (0-255 values)
      for i in 0..binArray.length-1 do
        binArray[i] = binArray[i] & 0xFF
      end

      return binArray
    end

    def to_s

      raise Utils_visiona::VerifyError, "Not a valid PDU Metadata" unless valid?

      output = ""
      output << "This is beginning of METADATA info\n"
      output << "Segmentation Control: #{(@segmentationControl == 1 ? "Boundaries not respected" : " Boundaries respected")}\n"
      output << "File Size: #{@fileSize} bytes\n"
      output << "Length source file: #{@lengthSourceFileName}\n"
      output << "Source file name: #{@sourceFileName}\n"
      output << "Length destination file: #{@lengthDestinationFileName}\n"
      output << "Destination file name: #{@destinationFileName}\n"
      output << "This is end of METADATA info\n"

      return output
    end
  end # end class PDUMetadata
end # end module CFDP