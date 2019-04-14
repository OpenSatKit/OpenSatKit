require 'minitest/autorun'

require_relative 'mock/utils.rb'
require_relative '../cfdp'

FINISHED_VALID_HASH = {conditionCode:0, endSystemStatus:0, deliveryCode:0, fileStatus:0}
FINISHED_VALID_ARRAY = [0]
FINISHED_TESTING_INPUT_TIMES = 100
FINISHED_CORRECT_LENGTH = 10

class Test_CFDP_Finished_Input_BinStream <  Minitest::Test

  def test_length

    # test ok
    finished = CFDP::PDUFinished.new(FINISHED_VALID_ARRAY)
    assert_kind_of CFDP::PDUFinished, finished

    # test empty
    finished = CFDP::PDUFinished.new
    assert_kind_of CFDP::PDUFinished, finished

    # test nil
    assert_raises "VerifyError" do
      finished = CFDP::PDUFinished.new(nil)
    end

    # test wrong input
    assert_raises "VerifyError" do
      finished = CFDP::PDUFinished.new("a")
    end

    # test wrong input
    assert_raises "VerifyError" do
      finished = CFDP::PDUFinished.new(12321312312)
    end

    # test empty array
    assert_raises "VerifyError" do
      finished = CFDP::PDUFinished.new([])
    end

    # test anything bigger than 255 and lower than 0
    for i in 0..FINISHED_VALID_ARRAY.length-1

      FINISHED_TESTING_INPUT_TIMES.times do

        customFinished = FINISHED_VALID_ARRAY.dup
        customFinished[i] = rand(256..256**3)
        assert_raises "VerifyError" do
          CFDP::PDUFinished.new(customFinished)
        end
        customFinished[i] = -1
        assert_raises "VerifyError" do
          CFDP::PDUFinished.new(customFinished)
        end
      end
    end
  end

  def test_byte_1

    arr = Array.new
    FINISHED_TESTING_INPUT_TIMES.times{arr << rand(0..(2**4)-1)}
    # test conditionCode (4 bits)
    for conditionCode in arr

      finished = CFDP::PDUFinished.new([conditionCode<<4, 0])
      assert_equal conditionCode, finished.conditionCode
    end

    # test endSystemStatus
    finished = CFDP::PDUFinished.new([8, 0])
    assert_equal finished.endSystemStatus, 1

    # test end deliveryCode
    finished = CFDP::PDUFinished.new([4, 0])
    assert_equal finished.deliveryCode, 1

    arr = Array.new
    FINISHED_TESTING_INPUT_TIMES.times{arr << rand(0..(2**2)-1)}
    # test fileStatus
    for fileStatus in arr

      finished = CFDP::PDUFinished.new([fileStatus, 0])
      assert_equal fileStatus, finished.fileStatus
    end
  end
end

class Test_CFDP_Finished_Input_Hash < Minitest::Test

  def test_initialize

    # test ok finished
    finished = CFDP::PDUFinished.new(FINISHED_VALID_HASH)
    assert_kind_of CFDP::PDUFinished, finished
    assert finished.valid?

    # test empty finished
    finished = CFDP::PDUFinished.new
    assert_kind_of CFDP::PDUFinished, finished

    # test missing any element
    FINISHED_VALID_HASH.keys.each do |key|
      newHash = FINISHED_VALID_HASH.dup
      newHash.delete(key)
      finished = CFDP::PDUFinished.new(newHash)
      assert !finished.valid?
    end

    # test bug for elements  < 0
    FINISHED_VALID_HASH.each do |key, value|
      newHash = FINISHED_VALID_HASH.dup
      newHash[key] = -1*rand(2**1..256)
      finished = CFDP::PDUFinished.new(newHash)
      assert !finished.valid?
    end

    # test higher > 4 bits
    symbols = [:conditionCode]
    symbols.each do |symbol|

      FINISHED_TESTING_INPUT_TIMES.times do

        newHash = FINISHED_VALID_HASH.dup
        newHash[symbol] = rand(2**4..2**10)
        finished = CFDP::PDUFinished.new(newHash)
        assert !finished.valid?
      end
    end

    # test all 1 bit thing
    symbols = [:endSystemStatus, :deliveryCode]
    symbols.each do |symbol|

      FINISHED_TESTING_INPUT_TIMES.times do

        newHash = FINISHED_VALID_HASH.dup
        newHash[symbol] = rand(2**1..2**10)
        finished = CFDP::PDUFinished.new(newHash)
        assert !finished.valid?
      end
    end

    # test wrong type input
    symbols = FINISHED_VALID_HASH.keys
    symbols.each do |symbol|
      newHash = FINISHED_VALID_HASH.dup
      # test string
      newHash[symbol] = "a"
      assert !CFDP::PDUFinished.new(newHash).valid?

      # test string
      newHash[symbol] = "abcd"
      assert !CFDP::PDUFinished.new(newHash).valid?

      # test string
      newHash[symbol] = "1"
      assert !CFDP::PDUFinished.new(newHash).valid?

      # test array
      newHash[symbol] = [1]
      assert !CFDP::PDUFinished.new(newHash).valid?

      # test array
      newHash[symbol] = [1, 2, 3]
      assert !CFDP::PDUFinished.new(newHash).valid?

      # test array
      newHash[symbol] = [1, [1]]
      assert !CFDP::PDUFinished.new(newHash).valid?

      # test Hash
      newHash[symbol] = {symbol: 1}
      assert !CFDP::PDUFinished.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>1}
      assert !CFDP::PDUFinished.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>[1]}
      assert !CFDP::PDUFinished.new(newHash).valid?
    end
  end
end

class Test_CFDP_Finished_Methods < Minitest::Test

  def test_method_length

    newHash = FINISHED_VALID_HASH.dup
    finished = CFDP::PDUFinished.new(newHash)
    assert_equal finished.length, 2

    newHash = FINISHED_VALID_HASH.dup
    newHash[:conditionCode] = 1
    finished = CFDP::PDUFinished.new(newHash)
    assert_equal finished.length, 2

    newHash = FINISHED_VALID_HASH.dup
    newHash[:endSystemStatus] = 1
    newHash[:fileStatus] = 3
    finished = CFDP::PDUFinished.new(newHash)
    assert_equal finished.length, 2
  end

  def test_method_pack

    finished = CFDP::PDUFinished.new(FINISHED_VALID_HASH)
    assert_equal finished.pack, [5, 0]

    valid_array = [0]
    finished = CFDP::PDUFinished.new(valid_array)
    assert_equal finished.pack, [5] + valid_array

    valid_array = [10]
    finished = CFDP::PDUFinished.new(valid_array)
    assert_equal finished.pack, [5] + valid_array
  end

  def test_method_to_s

    # just shouldn't throw error and return a string
    assert_kind_of String, CFDP::PDUFinished.new(FINISHED_VALID_HASH).to_s
    assert_kind_of String, CFDP::PDUFinished.new(FINISHED_VALID_ARRAY).to_s
  end
end