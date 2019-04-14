require "utils_visiona/utils" unless defined?(UTILS_VISIONA)

module CFDP

  class PDUHeader

    attr_accessor :version
    attr_accessor :pduType
    attr_accessor :direction
    attr_accessor :transmissionMode
    attr_accessor :crcFlag
    attr_accessor :pduDataLength
    attr_accessor :IDLength
    attr_accessor :sequenceLength
    attr_accessor :sourceID
    attr_accessor :sequenceNumber
    attr_accessor :destinationID

    @@minLength = 7
    @@totalVariblesInstanceds = 11

    def initialize(*content)

      # Content empty means creation of empty PDUHeader
      return if content.empty?
      content = content[0]

      # That means i'm creating a PDUHeader from a byte stream
      if (content.is_a?(Array))

        # first verify length of given pduHeader
        Utils_visiona.verifyLength("less", content.length, @@minLength)

        # Verify if we have a Byte array
        Utils_visiona.checkByteArray(content)

        # Verify bit 8, 25, 29 == 0
        Utils_visiona.compareValues(Utils_visiona.getBits(content[0], 1, 1), 0, "on bit \#8 of PDUHeader")
        Utils_visiona.compareValues(Utils_visiona.getBits(content[3], 8, 8), 0, "on bit \#25 of PDUHeader")
        Utils_visiona.compareValues(Utils_visiona.getBits(content[3], 4, 4), 0, "on bit \#29 of PDUHeader")

        # Set first 4 bytes
        @version = Utils_visiona.getBits(content[0], 6, 8)
        @pduType = Utils_visiona.getBits(content[0], 5, 5)
        @direction = Utils_visiona.getBits(content[0], 4, 4)
        @transmissionMode = Utils_visiona.getBits(content[0], 3, 3)
        @crcFlag = Utils_visiona.getBits(content[0], 2, 2)
        @pduDataLength = (content[1]<<8)+content[2]
        @IDLength = Utils_visiona.getBits(content[3], 5, 7)
        @sequenceLength = Utils_visiona.getBits(content[3], 1, 3)

        # new verification now that we have length of entity ID and sequence ID
        # verify if version == 0
        Utils_visiona.compareValues(@version, 0, "version number")
        Utils_visiona.verifyLength("less", content.length, headerSize)

        # passed verification, continue setting things
        @sourceID = 0; for i in 0..@IDLength; @sourceID+=content[4+i]<<(8*(@IDLength-i)); end
        @sequenceNumber = 0; for i in 0..@sequenceLength; @sequenceNumber+=content[4+@IDLength+1+i]<<(8*(@sequenceLength-i)); end
        @destinationID = 0; for i in 0..@IDLength; @destinationID+=content[4+@IDLength+2+@sequenceLength+i]<<(8*(@IDLength-i)); end

        # verify if it's all valid
        valid?
      elsif (content.is_a?(Hash)) # That means i'm creating a custom PDUHeader

        @version = content[:version] if Utils_visiona.hasSymbol?(content, :version, Integer, 3)
        @pduType = content[:pduType] if Utils_visiona.hasSymbol?(content, :pduType, Integer, 1)
        @direction = content[:direction] if Utils_visiona.hasSymbol?(content, :direction, Integer, 1)
        @transmissionMode = content[:transmissionMode] if Utils_visiona.hasSymbol?(content, :transmissionMode, Integer, 1)
        @crcFlag = content[:crcFlag] if Utils_visiona.hasSymbol?(content, :crcFlag, Integer, 1)
        @pduDataLength = content[:pduDataLength] if Utils_visiona.hasSymbol?(content, :pduDataLength, Integer, 16)
        @IDLength = content[:idLength] if Utils_visiona.hasSymbol?(content, :idLength, Integer, 3)
        @sequenceLength = content[:sequenceLength] if Utils_visiona.hasSymbol?(content, :sequenceLength, Integer, 3)
        @sourceID = content[:sourceID] if Utils_visiona.hasSymbol?(content, :sourceID, Integer, 64)
        @sequenceNumber = content[:sequenceNumber] if Utils_visiona.hasSymbol?(content, :sequenceNumber, Integer, 64)
        @destinationID = content[:destinationID] if Utils_visiona.hasSymbol?(content, :destinationID, Integer, 64)
      else

        raise Utils_visiona::VerifyError, "Not a valid input for pduHeader with class #{content.class}"
      end
    end

    def valid?

      begin

        # Check for instanceds vars
        Utils_visiona.compareValues(self.instance_variables.length, @@totalVariblesInstanceds, "variables instanceds")

        # Check for types instanceds
        return false unless @version.is_a?(Integer)
        return false unless @pduType.is_a?(Integer)
        return false unless @direction.is_a?(Integer)
        return false unless @transmissionMode.is_a?(Integer)
        return false unless @crcFlag.is_a?(Integer)
        return false unless @pduDataLength.is_a?(Integer)
        return false unless @IDLength.is_a?(Integer)
        return false unless @sequenceLength.is_a?(Integer)
        return false unless @sourceID.is_a?(Integer)
        return false unless @sequenceNumber.is_a?(Integer)
        return false unless @destinationID.is_a?(Integer)
      rescue

        return false
      end

      return true
    end

    def headerSize

      # this will figure the header size (in octets)
      raise Utils_visiona::VerifyError if (@IDLength.nil? || @sequenceLength.nil?)

      # 4 bytes for static header options. 2*id length for 2x id vars and 1x sequence var. +1 because it's var+1 octets (0 means 1 octet).
      return 4 + 2*(@IDLength+1) + @sequenceLength+1
    end

    def pack

      raise Utils_visiona::VerifyError, "Not a valid PDUHeader" unless valid?

      # create array and insert values.
      binArray = Array.new
      binArray << ((@version << 5) + (@pduType << 4) + (@direction << 3) + (@transmissionMode << 2) + (crcFlag << 1) + 0)
      binArray << Utils_visiona.getBits(@pduDataLength, 9, 16)
      binArray << Utils_visiona.getBits(@pduDataLength, 1, 8)
      binArray << ((@IDLength << 4) + (@sequenceLength))

      # routine for variable length's vars
      for i in (0..@IDLength); binArray << Utils_visiona.getBits(@sourceID, 1+8*(@IDLength-i), 8+8*(@IDLength-i)); end
      for i in (0..@sequenceLength); binArray << Utils_visiona.getBits(@sequenceNumber, 1+8*(@sequenceLength-i), 8+8*(@sequenceLength-i)); end
      for i in (0..@IDLength); binArray << Utils_visiona.getBits(@destinationID, 1+8*(@IDLength-i), 8+8*(@IDLength-i)); end

      # this must be a byte array (0-255 values)
      for i in 0..binArray.length-1 do
        binArray[i] = binArray[i] & 0xFF
      end

      return binArray
    end

    def to_s

      raise Utils_visiona::VerifyError, "Not a valid PDU Header" unless valid?

      output = ""
      output << "This is the beginning of the PDU Header\n"
      output << "Version: #{@version}\n"
      output << "PDU Type: #{(@pduType == 1 ? "File Data" : "File Directive")}\n"
      output << "Direction: #{(@direction == 1 ? "Toward sender" : "Toward receiver")}\n"
      output << "Transmission Mode: #{(@transmissionMode == 1 ? "UNACKNOWLEDGED" : "ACKNOWLEDGED")}\n"
      output << "CRC Flag: #{(@crcFlag == 1 ? "present" : "not present")}\n"
      output << "PDU Data Field Length: #{@pduDataLength}\n"
      output << "Length Entity IDS: #{@IDLength}\n"
      output << "Length of Transaction Sequence: #{@sequenceLength}\n"
      output << "Source ID: #{@sourceID}\n"
      output << "Transaction sequence number: #{@sequenceNumber}\n"
      output << "Destination ID: #{@destinationID}\n"
      output << "This is the end of the PDU Header\n"

      return output
    end
  end
end