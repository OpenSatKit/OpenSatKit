require_relative 'src/pduHeader'
require_relative 'src/pduFileData'
require_relative 'src/pduMetadata'
require_relative 'src/pduEOF'
require_relative 'src/pduFinished'
require_relative 'src/pduAck'
require_relative 'src/pduPacket'
require_relative 'src/pduNAK'
require_relative 'src/pduPrompt'

module CFDP

  PDUCLASSES = [CFDP::PDUMetadata, CFDP::PDUFileData, CFDP::PDUFinished, CFDP::PDUNAK,
  CFDP::PDUEOF, CFDP::PDUACK]

  # here we translate codes of table 5-5 of ccsds manual
  def conditionCodeToStr(condition)

    Utils_visiona.verifyInput(Integer, condition.class)

    output = case condition
      when 0 then "No error"
      when 1 then "Positive ACK limit reached"
      when 2 then "Keep alive limit reached"
      when 3 then "Invalid transmission mode"
      when 4 then "Filestore rejection"
      when 5 then "File checksum failure"
      when 6 then "File size error"
      when 7 then "NAK limit reached"
      when 8 then "Inactivity detected"
      when 9 then "Invalid file structure"
      when 10 then "Check limit reached"
      when 14 then "Suspend.request received"
      when 15 then "Cancel.request received"
      else "Invalid condition"
    end

    return output
  end

  # incomplete function
  def generatePDU(type, *args)

    #puts "type is #{type} and class is #{type.class}"
    Utils_visiona.verifyInput(String, type.class)
    Utils_visiona.verifyInput(Hash, args[0].class)

    args = args[0]
    pduType = 0
    pdu = CFDP::PDUPacket.new
    begin
      pdu.pduPayload = case type.upcase
      when "EOF"
        pduPayload = CFDP::PDUEOF.new(conditionCode:args[:conditionCode], fileChecksum:args[:fileChecksum])
      when "ACK"
        pduPayload = CFDP::PDUACK.new(directiveCode:args[:directiveCode], directiveSubtypeCode:args[:directiveSubtypeCode],
        conditionCode:args[:conditionCode], transactionStatus:args[:transactionStatus])
      when "FINISHED"
        pduPayload = CFDP::PDUFinished.new(conditionCode:args[:conditionCode], endSystemStatus:args[:endSystemStatus],
          deliveryCode:args[:deliveryCode], fileStatus:args[:fileStatus])
      when "PROMPT"
        pduPayload = CFDP::PDUPrompt.new()
      when "NAK"
        pduPayload = CFDP::PDUNAK.new(scopeStart:args[:scopeStart], scopeEnd:args[:scopeEnd], segmentRequests:args[:segmentRequests])
      when "FILEDATA"
        pduType = 1
        pduPayload = CFDP::PDUFileData.new()
      else
        raise StandardError, "PDU Type invalid."
      end

      pdu.pduPayload.valid?
      pdu.pduHeader = CFDP::PDUHeader.new(version:0, pduType:pduType, direction:args[:direction],
        transmissionMode:args[:transmissionMode], crcFlag:USE_CRC, sourceID:args[:sourceID], pduDataLength:pdu.pduPayload.length + (2*USE_CRC),
        destinationID:args[:destinationID], sequenceNumber:args[:sequenceNumber],
        sequenceLength:3, idLength:1)
    rescue Exception => err

      puts "Error when trying to create a #{type} PDU. Error is:"
      puts err
      raise
    end

    return pdu
  end

  # generate CRC using CCITT SDLC error polynomial (X16 + X12 + X5 + 1)
  def calculateCRC(data)
    
    # data must be an array of bytes
    crc = 0xFFFF

    for ch in data

      ch = (ch & 0xFF) << 8; # guarantee of byte

      8.times do

        xor_flag = ((crc ^ ch) & 0x8000)!=0 ? true : false
        crc = ((crc << 1) & 0xFFFF)
        crc = crc ^ 0x1021 if xor_flag
        ch = ((ch << 1) & 0xFFFF)
      end
    end

    return crc
  end

  # Utiilities
  module_function :conditionCodeToStr
  module_function :generatePDU
  module_function :calculateCRC
end