require 'minitest/autorun'

require_relative 'mock/utils.rb'
require_relative '../cfdp'

HEADER_VALID_HASH = {version:0, pduType:0, direction:0, transmissionMode:0, crcFlag:0,
      pduDataLength:0, idLength:0, sequenceLength:0, sourceID:0, sequenceNumber:0,
      destinationID:0}
HEADER_VALID_ARRAY = [0, 0, 0, 19, 0, 21, 0, 0, 0, 69, 0, 24]
HEADER_TESTING_INPUT_TIMES = 100
HEADER_CORRECT_LENGTH = 10

class Test_CFDP_Header_Input_BinStream <  Minitest::Test

  def test_length

    # test ok
    header = CFDP::PDUHeader.new(HEADER_VALID_ARRAY)
    assert_kind_of CFDP::PDUHeader, header

    # test empty
    header = CFDP::PDUHeader.new
    assert_kind_of CFDP::PDUHeader, header

    # test nil
    assert_raises "VerifyError" do
      header = CFDP::PDUHeader.new(nil)
    end

    # test wrong input
    assert_raises "VerifyError" do
      header = CFDP::PDUHeader.new("a")
    end

    # test wrong input
    assert_raises "VerifyError" do
      header = CFDP::PDUHeader.new(12321312312)
    end

    # test empty array
    assert_raises "VerifyError" do
      header = CFDP::PDUHeader.new([])
    end

    # test below min
    assert_raises "VerifyError" do
      header = CFDP::PDUHeader.new([0, 0, 0])
    end

    # test below min_2
    assert_raises "VerifyError" do
      header = CFDP::PDUHeader.new([0, 0, 0, 0, 0, 0])
    end

    # test below min_3
    assert_raises "VerifyError" do
      header = CFDP::PDUHeader.new([0, 0, 0, 0, 0, 0])
    end

    # test anything bigger than 255 and lower than 0
    for i in 0..HEADER_VALID_ARRAY.length-1

      HEADER_TESTING_INPUT_TIMES.times do

        customHeader = HEADER_VALID_ARRAY.dup
        customHeader[i] = rand(256..256**3)
        assert_raises "VerifyError" do
          CFDP::PDUHeader.new(customHeader)
        end
        customHeader[i] = -1
        assert_raises "VerifyError" do
          CFDP::PDUHeader.new(customHeader)
        end
      end
    end
  end

  def test_byte_1

    # test wrong version (3 bits)
    for conditionCode in 1..(2**3)-1
      assert_raises do
        header = CFDP::PDUHeader.new([conditionCode<<5, 0, 0, 19, 0, 21, 0, 0, 0, 69, 0, 24])
      end
    end

    # test wrong bit 8
    assert_raises "VerifyError" do
      header = CFDP::PDUHeader.new([0+1, 0, 0, 19, 0, 21, 0, 0, 0, 69, 0, 24])
    end

    # test first byte
    header = CFDP::PDUHeader.new([0, 0, 0, 19, 0, 21, 0, 0, 0, 69, 0, 24])
    assert_equal header.pduType, 0
    assert_equal header.direction, 0
    assert_equal header.transmissionMode, 0
    assert_equal header.crcFlag, 0

    # test pduType
    header = CFDP::PDUHeader.new([16, 0, 0, 19, 0, 21, 0, 0, 0, 69, 0, 24])
    assert_equal header.pduType, 1

    # test direction
    header = CFDP::PDUHeader.new([8, 0, 0, 19, 0, 21, 0, 0, 0, 69, 0, 24])
    assert_equal header.direction, 1

    # test transmissionMode
    header = CFDP::PDUHeader.new([4, 0, 0, 19, 0, 21, 0, 0, 0, 69, 0, 24])
    assert_equal header.transmissionMode, 1

    # test crcFlag
    header = CFDP::PDUHeader.new([2, 0, 0, 19, 0, 21, 0, 0, 0, 69, 0, 24])
    assert_equal header.crcFlag, 1
  end

  def test_byte_2_3

    # byte 2-3 is pdu data length
    arr = Array.new
    HEADER_TESTING_INPUT_TIMES.times{arr << rand(0..(2**16)-1)}

    for dataLength in arr

      header = CFDP::PDUHeader.new([0, (dataLength&0xFF00)>>8, dataLength&0x00FF, 19, 0, 21, 0, 0, 0, 69, 0, 24])
      assert_equal header.pduDataLength, dataLength
    end
  end

  def test_byte_4

    # test wrong bit 25
    assert_raises "VerifyError" do
      header = CFDP::PDUHeader.new([0, 0, 0, 128, 0, 21, 0, 0, 0, 69, 0, 24])
    end

    # test wrong bit 29
    assert_raises "VerifyError" do
      header = CFDP::PDUHeader.new([0, 0, 0, 8, 0, 21, 0, 0, 0, 69, 0, 24])
    end

    # check for length entity ID
    entityIDLength = 1
    header = CFDP::PDUHeader.new([0, 0, 0, ((entityIDLength&0x07)<<4), 0, 21, 0, 0, 0, 69, 0, 24])
    assert_equal header.IDLength, entityIDLength

    # check for length entity ID_2
    entityIDLength = 2
    header = CFDP::PDUHeader.new([0, 0, 0, ((entityIDLength&0x07)<<4)+3, 0, 0, 21, 0, 0, 0, 69, 0, 0, 24])
    assert_equal header.IDLength, entityIDLength

    # check for length sequence number transaction
    transactionLength = 3
    header = CFDP::PDUHeader.new([0, 0, 0, (transactionLength&0x07), 0, 0, 21, 0, 0, 0, 69, 0, 24])
    assert_equal header.sequenceLength, transactionLength

    # check for length sequence number transaction_2
    transactionLength = 4
    header = CFDP::PDUHeader.new([0, 0, 0, 4, 0, 0, 21, 0, 0, 0, 0, 69, 0, 24])
    assert_equal header.sequenceLength, transactionLength

    # check wrong size for length ID (only higher size, other errors are checked in PDUPacket. Header will ignore out of expected bytes.)
    entityIDLength = 2
    assert_raises "VerifyError" do
      # + 3 is for transaction sequence length
      header = CFDP::PDUHeader.new([0, 0, 0, ((entityIDLength&0x07)<<4)+3, 0, 21, 0, 0, 0, 69, 0, 24])
    end

    # check wrong size for length ID (only higher size, other errors are checked in PDUPacket. Header will ignore out of expected bytes.)
    entityIDLength = 5
    assert_raises "VerifyError" do
      # + 3 is for transaction sequence length
      header = CFDP::PDUHeader.new([0, 0, 0, ((entityIDLength&0x07)<<4)+3, 0, 21, 0, 0, 0, 69, 0, 24, 0, 0])
    end

    # check wrong size for sequenceLength ID (only higher size, other errors are checked in PDUPacket. Header will ignore out of expected bytes.)
    transactionLength = 4
    assert_raises "VerifyError" do
      # 16 is for id length = 1
      header = CFDP::PDUHeader.new([0, 0, 0, (transactionLength&0x07)+16, 0, 21, 0, 0, 0, 69, 0, 24])
    end
  end
end

class Test_CFDP_Header_Input_Hash < Minitest::Test

  def test_initialize

    # test ok header
    header = CFDP::PDUHeader.new(HEADER_VALID_HASH)
    assert_kind_of CFDP::PDUHeader, header
    assert header.valid?

    # test empty header
    header = CFDP::PDUHeader.new
    assert_kind_of CFDP::PDUHeader, header

    # test missing any element
    HEADER_VALID_HASH.keys.each do |key|
      newHash = HEADER_VALID_HASH.dup
      newHash.delete(key)
      header = CFDP::PDUHeader.new(newHash)
      assert !header.valid?
    end

    # test bug for elements  < 0
    HEADER_VALID_HASH.each do |key, value|
      newHash = HEADER_VALID_HASH.dup
      newHash[key] = -1
      header = CFDP::PDUHeader.new(newHash)
      assert !header.valid?
    end

    # test version, lengthID and sequenceLength > 3 bits
    symbols = [:version, :idLength, :sequenceLength]
    symbols.each do |symbol|
      newHash = HEADER_VALID_HASH.dup
      newHash[symbol] = 8
      header = CFDP::PDUHeader.new(newHash)
      assert !header.valid?
    end

    # test all 1 bit thing
    symbols = [:pduType, :direction, :transmissionMode, :crcFlag]
    symbols.each do |symbol|
      newHash = HEADER_VALID_HASH.dup
      newHash[symbol] = 2
      header = CFDP::PDUHeader.new(newHash)
      assert !header.valid?
    end

    # test wrong type input
    symbols = HEADER_VALID_HASH.keys
    symbols.each do |symbol|
      newHash = HEADER_VALID_HASH.dup
      # test string
      newHash[symbol] = "a"
      assert !CFDP::PDUHeader.new(newHash).valid?

      # test string
      newHash[symbol] = "abcd"
      assert !CFDP::PDUHeader.new(newHash).valid?

      # test string
      newHash[symbol] = "1"
      assert !CFDP::PDUHeader.new(newHash).valid?

      # test array
      newHash[symbol] = [1]
      assert !CFDP::PDUHeader.new(newHash).valid?

      # test array
      newHash[symbol] = [1, 2, 3]
      assert !CFDP::PDUHeader.new(newHash).valid?

      # test array
      newHash[symbol] = [1, [1]]
      assert !CFDP::PDUHeader.new(newHash).valid?

      # test Hash
      newHash[symbol] = {symbol: 1}
      assert !CFDP::PDUHeader.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>1}
      assert !CFDP::PDUHeader.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>[1]}
      assert !CFDP::PDUHeader.new(newHash).valid?
    end
  end
end

class Test_CFDP_Header_Methods < Minitest::Test

  def test_method_headerSize

    newHash = HEADER_VALID_HASH.dup
    header = CFDP::PDUHeader.new(newHash)
    assert_equal header.headerSize, 7

    newHash = HEADER_VALID_HASH.dup
    newHash[:idLength] = 1
    header = CFDP::PDUHeader.new(newHash)
    assert_equal header.headerSize, 9

    newHash = HEADER_VALID_HASH.dup
    newHash[:idLength] = 1
    newHash[:sequenceLength] = 3
    header = CFDP::PDUHeader.new(newHash)
    assert_equal header.headerSize, 12
  end

  def test_method_pack

    header = CFDP::PDUHeader.new(HEADER_VALID_HASH)
    assert_equal header.pack, [0, 0, 0, 0, 0, 0, 0]

    valid_array = [0, 0, 0, 19, 0, 21, 0, 0, 0, 69, 0, 24]
    header = CFDP::PDUHeader.new(valid_array)
    assert_equal header.pack, valid_array

    valid_array = [0, 0, 0, 20, 0, 21, 0, 0, 0, 0, 69, 0, 24]
    header = CFDP::PDUHeader.new(valid_array)
    assert_equal header.pack, valid_array

    valid_array = [0, 0, 0, 32+4, 0, 0, 21, 0, 0, 0, 0, 69, 0, 0, 24]
    header = CFDP::PDUHeader.new(valid_array)
    assert_equal header.pack, valid_array
  end

  def test_method_to_s

    # just shouldn't throw error and return a string
    assert_kind_of String, CFDP::PDUHeader.new(HEADER_VALID_HASH).to_s
    assert_kind_of String, CFDP::PDUHeader.new(HEADER_VALID_ARRAY).to_s
  end
end