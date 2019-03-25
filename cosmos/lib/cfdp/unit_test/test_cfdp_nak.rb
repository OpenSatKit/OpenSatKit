require 'minitest/autorun'

require_relative 'mock/utils.rb'
require_relative '../cfdp'

NAK_VALID_HASH = {scopeStart:0, scopeEnd:0, segmentRequests:[{startOffset:0, endOffset:0}]}
NAK_VALID_ARRAY = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
NAK_TESTING_INPUT_TIMES = 100
NAK_CORRECT_LENGTH = 17

class Test_CFDP_NAK_Input_BinStream < Minitest::Test

  def test_length

    # test ok
    nak = CFDP::PDUNAK.new(NAK_VALID_ARRAY)
    assert_kind_of CFDP::PDUNAK, nak

    # test nil
    assert_raises "VerifyError" do
      CFDP::PDUNAK.new(nil)
    end

    # test wrong input
    assert_raises "VerifyError" do
      nak = CFDP::PDUNAK.new("a")
    end

    # test wrong input
    assert_raises "VerifyError" do
      nak = CFDP::PDUNAK.new(12321312312)
    end

    # test empty array
    assert_raises "VerifyError" do
      header = CFDP::PDUNAK.new([])
    end

    # test below min
    assert_raises "VerifyError" do
      header = CFDP::PDUNAK.new([0])
    end

    # test anything bigger than 255 and lower than 0
    for i in 0..NAK_VALID_ARRAY.length-1

      NAK_TESTING_INPUT_TIMES.times do

        custom = NAK_VALID_ARRAY.dup
        custom[i] = rand(256..256**3)
        assert_raises "VerifyError" do
          CFDP::PDUNAK.new(custom)
        end
        custom[i] = -1*rand(256..256**3)
        assert_raises "VerifyError" do
          CFDP::PDUNAK.new(custom)
        end
      end
    end
  end

  def test_byte_1_4

    arr = Array.new

    # test offset (32 bits)
    NAK_TESTING_INPUT_TIMES.times{arr << rand(0..(2**32)-1)}
    for scopeStart in arr

      customArray = []
      customArray << ((scopeStart & 0xFF000000) >> (8*3))
      customArray << ((scopeStart & 0x00FF0000) >> (8*2))
      customArray << ((scopeStart & 0x0000FF00) >> (8*1))
      customArray << ((scopeStart & 0x000000FF) >> (8*0))
      customArray += [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
      nak = CFDP::PDUNAK.new(customArray)
      assert_equal scopeStart, nak.scopeStart
    end
  end

  def test_byte_4_8

    arr = Array.new

    # test offset (32 bits)
    NAK_TESTING_INPUT_TIMES.times{arr << rand(0..(2**32)-1)}
    for scopeEnd in arr

      customArray = [0, 0, 0, 0]
      customArray << ((scopeEnd & 0xFF000000) >> (8*3))
      customArray << ((scopeEnd & 0x00FF0000) >> (8*2))
      customArray << ((scopeEnd & 0x0000FF00) >> (8*1))
      customArray << ((scopeEnd & 0x000000FF) >> (8*0))
      customArray += [0, 0, 0, 0, 0, 0, 0, 0]
      nak = CFDP::PDUNAK.new(customArray)
      assert_equal scopeEnd, nak.scopeEnd
    end
  end
end

class Test_CFDP_NAK_Input_Hash < Minitest::Test

  def test_initialize

    # test ok
    nak = CFDP::PDUNAK.new(NAK_VALID_HASH)
    assert_kind_of CFDP::PDUNAK, nak
    assert nak.valid?

    # test empty nak
    nak = CFDP::PDUNAK.new
    assert_kind_of CFDP::PDUNAK, nak

    # test missing any element
    NAK_VALID_HASH.keys.each do |key|
      newHash = NAK_VALID_HASH.dup
      newHash.delete(key)
      nak = CFDP::PDUNAK.new(newHash)
      assert !nak.valid?
    end

    # test bug for elements  < 0
    NAK_VALID_HASH.each do |key, value|
      newHash = NAK_VALID_HASH.dup
      newHash[key] = -1
      nak = CFDP::PDUNAK.new(newHash)
      assert !nak.valid?
    end

    # test bug for element 32 bits
    symbols = [:scopeStart, :scopeEnd]
    symbols.each do |symbol|
      newHash = NAK_VALID_HASH.dup
      newHash[symbol] = rand(2**32..2**36)
      nak = CFDP::PDUNAK.new(newHash)
      assert !nak.valid?
    end

    # test wrong type input
    symbols = NAK_VALID_HASH.keys
    symbols.each do |symbol|
      newHash = NAK_VALID_HASH.dup
      # test string
      newHash[symbol] = "a"
      assert !CFDP::PDUNAK.new(newHash).valid?

      # test string
      newHash[symbol] = "abcd"
      assert !CFDP::PDUNAK.new(newHash).valid?

      # test string
      newHash[symbol] = "1"
      assert !CFDP::PDUNAK.new(newHash).valid?

      # test Hash
      newHash[symbol] = {symbol: 1}
      assert !CFDP::PDUNAK.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>1}
      assert !CFDP::PDUNAK.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>[1]}
      assert !CFDP::PDUNAK.new(newHash).valid?
    end
  end
end

class Test_CFDP_NAK_Methods < Minitest::Test


  def test_method_length

    nak = CFDP::PDUNAK.new(NAK_VALID_HASH)
    assert_equal NAK_CORRECT_LENGTH, nak.length

    newHash = NAK_VALID_HASH.dup
    newHash[:startOffset] = 1
    nak = CFDP::PDUNAK.new(newHash)
    assert_equal NAK_CORRECT_LENGTH, nak.length
  end

  def test_method_pack

    nak = CFDP::PDUNAK.new(NAK_VALID_HASH)
    assert_equal [8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], nak.pack

    valid_array = [0, 0, 0, 0, 0, 1, 144, 0, 0, 0, 0, 0, 0, 0, 1, 144, 0, 0, 3, 232, 0, 0, 9, 96, 0, 0, 10, 40, 0, 0, 16, 104, 0, 0, 17, 248, 0, 0, 30, 120, 0, 0, 32, 8, 0, 0, 35, 240, 0, 0, 36, 184, 0, 0, 39, 216, 0, 0, 40, 160, 0, 0, 41, 104, 0, 0, 42, 48, 0, 0, 51, 144, 0, 0, 53, 232, 0, 0, 57, 208, 0, 0, 58, 152, 0, 0, 59, 96, 0, 0, 60, 40, 0, 0, 78, 232, 0, 0, 79, 176, 0, 0, 85, 40, 0, 0, 85, 240, 0, 0, 94, 136, 0, 0, 95, 80, 0, 0, 98, 112, 0, 0, 100, 0, 0, 0, 103, 232, 0, 0, 104, 176, 0, 0, 107, 8, 0, 0, 107, 208, 0, 0, 114, 16, 0, 0, 114, 216, 0, 0, 120, 80, 0, 0, 121, 24, 0, 0, 123, 112, 0, 0, 124, 56, 0, 0, 126, 144, 0, 0, 127, 88, 0, 0, 135, 240, 0, 0, 136, 184, 0, 0, 137, 128, 0, 0, 138, 72, 0, 0, 139, 216, 0, 0, 140, 160, 0, 0, 144, 136, 0, 0, 145, 80, 0, 0, 159, 96, 0, 0, 160, 40, 0, 0, 161, 184, 0, 0, 162, 128, 0, 0, 163, 72, 0, 0, 164, 216, 0, 0, 170, 80, 0, 0, 171, 24, 0, 0, 173, 112, 0, 0, 174, 56, 0, 0, 182, 208, 0, 0, 184, 96, 0, 0, 185, 40, 0, 0, 185, 240, 0, 0, 194, 136, 0, 0, 196, 24, 0, 0, 202, 88, 0, 0, 203, 232, 0, 0, 219, 136, 0, 0, 220, 80, 0, 0, 221, 24, 0, 0, 222, 168, 0, 0, 223, 112, 0, 0, 225, 200, 0, 0, 226, 144, 0, 0, 227, 88, 0, 0, 231, 64, 0, 0, 232, 208, 0, 0, 235, 40, 0, 0, 235, 240, 0, 0, 236, 184, 0, 0, 237, 128, 0, 0, 247, 168, 0, 0, 248, 112, 0, 1, 8, 216, 0, 1, 9, 160, 0, 1, 11, 48, 0, 1, 11, 248, 0, 1, 15, 24, 0, 1, 15, 224, 0, 1, 23, 176, 0, 1, 24, 120, 0, 1, 25, 64, 0, 1, 26, 8, 0, 1, 28, 96, 0, 1, 29, 40, 0, 1, 31, 128, 0, 1, 32, 72, 0, 1, 36, 248, 0, 1, 37, 192, 0, 1, 41, 168, 0, 1, 42, 112, 0, 1, 43, 56, 0, 1, 44, 0, 0, 1, 57, 72, 0, 1, 58, 16, 0, 1, 60, 104, 0, 1, 61, 48, 0, 1, 62, 192, 0, 1, 63, 136, 0, 1, 66, 168, 0, 1, 67, 112, 0, 1, 73, 176, 0, 1, 74, 120, 0, 1, 78, 96, 0, 1, 79, 40, 0, 1, 81, 128, 0, 1, 82, 72, 0, 1, 99, 120, 0, 1, 100, 64, 0, 1, 108, 16, 0, 1, 108, 216, 0, 1, 113, 136]
    nak = CFDP::PDUNAK.new(valid_array)
    assert_equal [8]+valid_array, nak.pack

    valid_array = [0, 0, 0, 0, 0, 1, 144, 0, 0, 0, 29, 176, 0, 0, 30, 120, 0, 0, 32, 8, 0, 0, 35, 240, 0, 0, 36, 184, 0, 0, 39, 216, 0, 0, 40, 160, 0, 0, 41, 104, 0, 0, 42, 48, 0, 0, 51, 144, 0, 0, 53, 232, 0, 0, 57, 208, 0, 0, 58, 152, 0, 0, 59, 96, 0, 0, 60, 40, 0, 0, 78, 232, 0, 0, 79, 176, 0, 0, 85, 40, 0, 0, 85, 240, 0, 0, 94, 136, 0, 0, 95, 80, 0, 0, 98, 112, 0, 0, 100, 0, 0, 0, 103, 232, 0, 0, 104, 176, 0, 0, 107, 8, 0, 0, 107, 208, 0, 0, 114, 16, 0, 0, 114, 216, 0, 0, 120, 80, 0, 0, 121, 24, 0, 0, 123, 112, 0, 0, 124, 56, 0, 0, 126, 144, 0, 0, 127, 88, 0, 0, 135, 240, 0, 0, 136, 184, 0, 0, 137, 128, 0, 0, 138, 72, 0, 0, 139, 216, 0, 0, 140, 160, 0, 0, 144, 136, 0, 0, 145, 80, 0, 0, 159, 96, 0, 0, 160, 40, 0, 0, 161, 184, 0, 0, 162, 128, 0, 0, 163, 72, 0, 0, 164, 216, 0, 0, 170, 80, 0, 0, 171, 24, 0, 0, 173, 112, 0, 0, 174, 56, 0, 0, 182, 208, 0, 0, 184, 96, 0, 0, 185, 40, 0, 0, 185, 240, 0, 0, 194, 136, 0, 0, 196, 24, 0, 0, 202, 88, 0, 0, 203, 232, 0, 0, 219, 136, 0, 0, 220, 80, 0, 0, 221, 24, 0, 0, 222, 168, 0, 0, 223, 112, 0, 0, 225, 200, 0, 0, 226, 144, 0, 0, 227, 88, 0, 0, 231, 64, 0, 0, 232, 208, 0, 0, 235, 40, 0, 0, 235, 240, 0, 0, 236, 184, 0, 0, 237, 128, 0, 0, 247, 168, 0, 0, 248, 112, 0, 1, 8, 216, 0, 1, 9, 160, 0, 1, 11, 48, 0, 1, 11, 248, 0, 1, 15, 24, 0, 1, 15, 224, 0, 1, 23, 176, 0, 1, 24, 120, 0, 1, 25, 64, 0, 1, 26, 8, 0, 1, 28, 96, 0, 1, 29, 40, 0, 1, 31, 128, 0, 1, 32, 72, 0, 1, 36, 248, 0, 1, 37, 192, 0, 1, 41, 168, 0, 1, 42, 112, 0, 1, 43, 56, 0, 1, 44, 0, 0, 1, 57, 72, 0, 1, 58, 16, 0, 1, 60, 104, 0, 1, 61, 48, 0, 1, 62, 192, 0, 1, 63, 136, 0, 1, 66, 168, 0, 1, 67, 112, 0, 1, 73, 176, 0, 1, 74, 120, 0, 1, 78, 96, 0, 1, 79, 40, 0, 1, 81, 128, 0, 1, 82, 72, 0, 1, 99, 120, 0, 1, 100, 64, 0, 1, 108, 16, 0, 1, 108, 216, 0, 1, 113, 136, 0, 1, 115, 24, 0, 1, 125, 64, 0, 1, 126, 8, 0, 1, 126, 208, 0, 1, 127, 152, 0, 1, 132, 72]
    nak = CFDP::PDUNAK.new(valid_array)
    assert_equal [8]+valid_array, nak.pack
  end

  def test_method_to_s

    assert_kind_of String, CFDP::PDUNAK.new(NAK_VALID_HASH).to_s
    assert_kind_of String, CFDP::PDUNAK.new(NAK_VALID_ARRAY).to_s
  end
end