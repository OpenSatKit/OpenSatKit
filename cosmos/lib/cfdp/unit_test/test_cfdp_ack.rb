require 'minitest/autorun'

require_relative 'mock/utils.rb'
require_relative '../cfdp'

ACK_VALID_HASH = {directiveCode:0, directiveSubtypeCode:0, conditionCode:0, transactionStatus:0}
ACK_VALID_ARRAY = [0, 0]
ACK_TESTING_INPUT_TIMES = 100
ACK_CORRECT_LENGTH = 10

class Test_CFDP_Ack_Input_BinStream <  Minitest::Test

  def test_length

    # test ok
    ack = CFDP::PDUACK.new(ACK_VALID_ARRAY)
    assert_kind_of CFDP::PDUACK, ack

    # test nil
    assert_raises "VerifyError" do
      CFDP::PDUACK.new(nil)
    end

    # test wrong input
    assert_raises "VerifyError" do
      ack = CFDP::PDUACK.new("a")
    end

    # test wrong input
    assert_raises "VerifyError" do
      ack = CFDP::PDUACK.new(12321312312)
    end

    # test empty array
    assert_raises "VerifyError" do
      header = CFDP::PDUACK.new([])
    end

    # test below min
    assert_raises "VerifyError" do
      header = CFDP::PDUACK.new([0])
    end

    # test anything bigger than 255 and lower than 0
    validHeader = ACK_VALID_ARRAY
    for i in 0..ACK_VALID_ARRAY.length-1

      ACK_TESTING_INPUT_TIMES.times do

        custom = ACK_VALID_ARRAY.dup
        custom[i] = rand(256..256**3)
        assert_raises "VerifyError" do
          CFDP::PDUACK.new(custom)
        end
        custom[i] = -1*rand(256..256**3)
        assert_raises "VerifyError" do
          CFDP::PDUACK.new(custom)
        end
      end
    end
  end

  def test_byte_1

    bits = 4
    arr = Array.new
    # test directiveCode (4 bits)
    ACK_TESTING_INPUT_TIMES.times{arr << rand(0..(2**bits)-1)}

    for directiveCode in arr
      ack = CFDP::PDUACK.new([(directiveCode<<4), 0])
      assert_equal directiveCode, ack.directiveCode
    end

    arr = Array.new
    # test directiveSubtypeCode (4 bits)
    ACK_TESTING_INPUT_TIMES.times{arr << rand(0..(2**bits)-1)}

    for directiveSubtypeCode in arr
      ack = CFDP::PDUACK.new([directiveSubtypeCode, 0])
      assert_equal directiveSubtypeCode, ack.directiveSubtypeCode
    end
  end

  def test_byte_2

    bits = 4
    arr = Array.new
    ACK_TESTING_INPUT_TIMES.times{arr << rand(0..(2**bits)-1)}
    # test conditionCode (4 bits)

    for conditionCode in arr
      ack = CFDP::PDUACK.new([0, conditionCode<<4])
      assert_equal conditionCode, ack.conditionCode
    end

    bits = 2
    arr = Array.new
    ACK_TESTING_INPUT_TIMES.times{arr << rand(0..(2**bits)-1)}
    # test transactionStatus (2 bits)

    for transactionStatus in arr
      ack = CFDP::PDUACK.new([0, transactionStatus])
      assert_equal transactionStatus, ack.transactionStatus
    end
  end
end

class Test_CFDP_Ack_Input_Hash < Minitest::Test

  def test_initialize

    # test ok ack
    ack = CFDP::PDUACK.new(ACK_VALID_HASH)
    assert_kind_of CFDP::PDUACK, ack
    assert ack.valid?

    # test empty ack
    ack = CFDP::PDUACK.new
    assert_kind_of CFDP::PDUACK, ack

    # test missing any element
    ACK_VALID_HASH.keys.each do |key|
      newHash = ACK_VALID_HASH.dup
      newHash.delete(key)
      ack = CFDP::PDUACK.new(newHash)
      assert !ack.valid?
    end

    # test bug for elements  < 0
    ACK_VALID_HASH.each do |key, value|

      newHash = ACK_VALID_HASH.dup
      newHash[key] = -1*rand(2**4..256)
      ack = CFDP::PDUACK.new(newHash)
      assert !ack.valid?
    end

    # test bug for element > 4 bits
    symbols = [:directiveCode, :directiveSubtypeCode, :conditionCode]
    symbols.each do |symbol|

      ACK_TESTING_INPUT_TIMES.times do

        newHash = ACK_VALID_HASH.dup
        newHash[symbol] = rand(2**4..256)
        ack = CFDP::PDUACK.new(newHash)
        assert !ack.valid?
      end
    end

    # test transactionStatus > 2 bits
    newHash = ACK_VALID_HASH.dup
    ACK_TESTING_INPUT_TIMES.times do

      newHash[:transactionStatus] = rand(2**2..256)
      ack = CFDP::PDUACK.new(newHash)
      assert !ack.valid?
    end

    # test wrong type input
    symbols = ACK_VALID_HASH.keys
    symbols.each do |symbol|
      newHash = ACK_VALID_HASH.dup
      # test string
      newHash[symbol] = "a"
      assert !CFDP::PDUACK.new(newHash).valid?

      # test string
      newHash[symbol] = "abcd"
      assert !CFDP::PDUACK.new(newHash).valid?

      # test string
      newHash[symbol] = "1"
      assert !CFDP::PDUACK.new(newHash).valid?

      # test array
      newHash[symbol] = [1]
      assert !CFDP::PDUACK.new(newHash).valid?

      # test array
      newHash[symbol] = [1, 2, 3]
      assert !CFDP::PDUACK.new(newHash).valid?

      # test array
      newHash[symbol] = [1, [1]]
      assert !CFDP::PDUACK.new(newHash).valid?

      # test Hash
      newHash[symbol] = {symbol: 1}
      assert !CFDP::PDUACK.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>1}
      assert !CFDP::PDUACK.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>[1]}
      assert !CFDP::PDUACK.new(newHash).valid?
    end
  end
end

class Test_CFDP_Ack_Methods < Minitest::Test

  def test_method_length

    ack = CFDP::PDUACK.new(ACK_VALID_HASH)
    assert_equal 3, ack.length

    newHash = ACK_VALID_HASH.dup
    newHash[:directiveCode] = 1
    ack = CFDP::PDUACK.new(newHash)
    assert_equal 3, ack.length
  end

  def test_method_pack

    ack = CFDP::PDUACK.new(ACK_VALID_HASH)
    assert_equal [6, 0, 0], ack.pack

    valid_array = [200, 128+3]
    ack = CFDP::PDUACK.new(valid_array)
    assert_equal [6]+valid_array, ack.pack

    valid_array = [13, 80+3]
    ack = CFDP::PDUACK.new(valid_array)
    assert_equal [6]+valid_array, ack.pack
  end

  def test_method_to_s

    assert_kind_of String, CFDP::PDUACK.new(ACK_VALID_HASH).to_s
    assert_kind_of String, CFDP::PDUACK.new([0, 0]).to_s
  end
end