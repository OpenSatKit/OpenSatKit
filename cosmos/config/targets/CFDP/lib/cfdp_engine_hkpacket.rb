require "singleton"
require "utils_visiona/utils"

module CFDP

  class HKPacket

    include Singleton

    VARS = ["eng_totalerrors", "eng_pdusreceived", "eng_pdusrejected", "eng_inprogresstrans", "eng_totalsuccesstrans",
      "eng_totalfailedtrans", "cond_posacknum", "cond_filestorerejnum", "cond_filechecksumnum", "cond_filesizenum",
      "cond_naklimitnum", "cond_inactivenum", "eng_totalfilessent", "eng_totalfilesrcvd", "eng_rcvd_metadata",
      "eng_rcvd_filedata", "eng_rcvd_eof", "eng_rcvd_finished", "eng_rcvd_nak", "eng_rcvd_ack", "eng_sent_metadata",
      "eng_sent_filedata", "eng_sent_eof", "eng_sent_finished", "eng_sent_nak", "eng_sent_ack", "eng_up_successcounter",
      "eng_up_failedcounter", "eng_down_successdownlinks", "eng_down_faileddownlinks"
    ]

    # vars
    attr_reader :pktID
    attr_reader :eng_sourceid
    attr_accessor :eng_lastfailedtrans
    attr_accessor :eng_up_lastfileuplinked
    attr_accessor :eng_down_lastfiledownlinked
    VARS.each {|var| eval "attr_accessor :#{var}"}

    def initialize

      @pktID = 0x43464450484b
      @eng_sourceid = MYID
      @eng_lastfailedtrans = "None"
      @eng_up_lastfileuplinked = "None"
      @eng_down_lastfiledownlinked = "None"
      VARS.each {|var| eval "@#{var} = 0"}
    end

    def updateVar(type, *pdu)

      if type.is_a?(String)

        case pdu[0]
          when CFDP::PDUMetadata  then type.upcase.eql?("SENT") ? @eng_sent_metadata+=1 : @eng_rcvd_metadata+=1
          when CFDP::PDUFileData  then type.upcase.eql?("SENT") ? @eng_sent_filedata+=1 : @eng_rcvd_filedata+=1
          when CFDP::PDUEOF       then type.upcase.eql?("SENT") ? @eng_sent_eof+=1      : @eng_rcvd_eof+=1
          when CFDP::PDUFinished  then type.upcase.eql?("SENT") ? @eng_sent_finished+=1 : @eng_rcvd_finished+=1
          when CFDP::PDUNAK       then type.upcase.eql?("SENT") ? @eng_sent_nak+=1      : @eng_rcvd_nak+=1
          when CFDP::PDUACK       then type.upcase.eql?("SENT") ? @eng_sent_ack+=1      : @eng_rcvd_ack+=1
        end
      elsif type.is_a?(Integer)

        case type
        when 1 then @cond_posacknum+=1
        when 4 then @cond_filestorerejnum+=1
        when 5 then @cond_filechecksumnum+=1
        when 6 then @cond_filesizenum+=1
        when 7 then @cond_naklimitnum+=1
        when 8 then @cond_inactivenum+=1
        end
      end
    end

    def pack

      binArray = Array.new
      array64 = 64.downto(8).to_a
      (0..array64.length-1).step(8).each {|i| binArray << Utils_visiona.getBits(@pktID, array64[i]-7, array64[i])}
      (0..array64.length-1).step(8).each {|i| binArray << Utils_visiona.getBits(@eng_sourceid, array64[i]-7, array64[i])}
      binArray << Utils_visiona.completeBytes(Utils_visiona.strToDecArray(@eng_lastfailedtrans), 64, "BIG_ENDIAN")
      binArray << Utils_visiona.completeBytes(Utils_visiona.strToDecArray(@eng_up_lastfileuplinked), 64, "BIG_ENDIAN")
      binArray << Utils_visiona.completeBytes(Utils_visiona.strToDecArray(@eng_down_lastfiledownlinked), 64, "BIG_ENDIAN")
      VARS.each {|var| (0..array64.length-1).step(8).each {|i| binArray << Utils_visiona.getBits((eval "@#{var}").to_i, array64[i]-7, array64[i])}}

      return binArray.flatten
    end
  end
end