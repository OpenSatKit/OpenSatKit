require 'cosmos/interfaces/protocols/protocol'
require 'cfdp/cfdp'

module Cosmos

  @received_pdu_counter = Hash.new
  @sent_pdu_counter = Hash.new
  @miss_received_pdus = Hash.new
  @miss_sent_pdus = Hash.new

  def self.pdu_test_initialize

    @received_pdu_counter = Hash.new
    @sent_pdu_counter = Hash.new
    @miss_received_pdus = Hash.new
    @miss_sent_pdus = Hash.new

    CFDP::PDUCLASSES.each do |classe|
      @received_pdu_counter[classe] = 0
      @sent_pdu_counter[classe] = 0
      @miss_received_pdus[classe] = Array.new
      @miss_sent_pdus[classe] = Array.new
    end
  end

  def self.received_pdu_counter
    return @received_pdu_counter
  end

  def self.received_pdu_counter=(val)
    @received_pdu_counter=val
  end

  def self.sent_pdu_counter
    return @sent_pdu_counter
  end

  def self.sent_pdu_counter=(val)
    @sent_pdu_counter=val
  end

  def self.miss_received_pdus
    return @miss_received_pdus
  end

  def self.miss_received_pdus=(val)
    @miss_received_pdus=val
  end

  def self.miss_sent_pdus
    return @miss_sent_pdus
  end

  def self.miss_sent_pdus=(val)
    @miss_sent_pdus=val
  end
end

module Cosmos
  # Protocol which permanently overrides an item value such that reading the
  # item returns the overriden value. Methods are prefixed with underscores
  # so the API can include the original name which calls out to these
  # methods. Clearing the override requires calling normalize_tlm.
  class CfdpTestProtocol < Protocol

    CFDP_TEST_TARGET_NAME = "CFDP_TEST"
    PDU_CLASSES = {0=>CFDP::PDUMetadata, 1=>CFDP::PDUFileData, 2=>CFDP::PDUFinished, 
                    3=>CFDP::PDUNAK, 4=>CFDP::PDUEOF, 5=>CFDP::PDUACK}  

    # @param allow_empty_data [true/false] Whether STOP should be returned on empty data
    def initialize(allow_empty_data = false)

      Cosmos::pdu_test_initialize
      super(allow_empty_data)
    end

    def miss_packet?(direction, pduClass)

      case direction.upcase
      when "RECEIVED"

        pdu_received_counter = Cosmos::received_pdu_counter
        unless Cosmos::miss_received_pdus[pduClass].nil?
          @packet_number = pdu_received_counter[pduClass]
          return true if Cosmos::miss_received_pdus[pduClass].include?(@packet_number)
        end
      when "SENT"

        pdu_sent_counter = Cosmos::sent_pdu_counter
        unless Cosmos::miss_sent_pdus[pduClass].nil?
          @packet_number = pdu_sent_counter[pduClass]
          return true if Cosmos::miss_sent_pdus[pduClass].include?(@packet_number)
        end
      end

      return false
    end

    def read_packet(packet)

      identified_packet = System.telemetry.identify!(packet.buffer, [PDU_TARGET_NAME])

      unless identified_packet.nil?

        packetPayload = identified_packet.read('PAYLOAD', :RAW)
        pduClass = CFDP::PDUPacket.new(packetPayload.bytes).pduPayload.class
        Cosmos::received_pdu_counter[pduClass] = Cosmos::received_pdu_counter[pduClass]+1
        if miss_packet?("received", pduClass)
          Cosmos::Logger.info "Missing received #{pduClass} ##{@packet_number} on purpose."
          packet = :STOP
        end
      end

      return packet
    end

    def write_packet(packet)

      # update packet counter here.
      case packet.target_name
      when CFDP_TEST_TARGET_NAME
        #puts "oi"
        # This is a command that should not be sent
        case packet.packet_name
        when "MISS_SENT_PACKET", "MISS_RECEIVED_PACKET"

          packet_array = Array.new
          hashe = Hash.new

          classe = PDU_CLASSES[packet.read('PDU_CLASS', :RAW).to_i]
          packet_number_array = packet.read('PACKET_NUMBER_ARRAY', :RAW)
          packet_number_array.tr(" []", "").split(",").each {|ele| packet_array << ele.to_i}

          (arr = Cosmos::miss_sent_pdus[classe].nil? ? Array.new : Cosmos::miss_sent_pdus[classe]) if packet.packet_name.eql?("MISS_SENT_PACKET")
          (arr = Cosmos::miss_received_pdus[classe].nil? ? Array.new : Cosmos::miss_received_pdus[classe]) if packet.packet_name.eql?("MISS_RECEIVED_PACKET")

          arr = arr+packet_array
          hashe[classe] = arr.uniq

          Cosmos::miss_sent_pdus = Cosmos::miss_sent_pdus.merge(hashe) if packet.packet_name.eql?("MISS_SENT_PACKET")
          Cosmos::miss_received_pdus = Cosmos::miss_received_pdus.merge(hashe) if packet.packet_name.eql?("MISS_RECEIVED_PACKET")
        when "RESET_COUNTERS"

          Cosmos::pdu_test_initialize
        end

        packet = :STOP
      when PDU_TARGET_NAME
        unless packet.nil?

          pduClass = CFDP::PDUPacket.new(packet.read('PAYLOAD', :RAW).bytes).pduPayload.class

          # update counter
          Cosmos::sent_pdu_counter[pduClass] = Cosmos::sent_pdu_counter[pduClass]+1

          if miss_packet?("sent", pduClass)
            Cosmos::Logger.info "Missing sent #{pduClass} ##{@packet_number} on purpose."
            packet = :STOP
          end
        end
      end

      return packet
    end
  end
end