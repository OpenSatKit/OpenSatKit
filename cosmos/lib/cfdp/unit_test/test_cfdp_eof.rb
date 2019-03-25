require 'minitest/autorun'

require_relative 'mock/utils.rb'
require_relative '../cfdp'

EOF_VALID_HASH = {conditionCode:0, fileChecksum:0, fileSize:0}
EOF_VALID_ARRAY = [0, 0, 0, 0, 0, 0, 0, 0, 0]
EOF_TESTING_INPUT_TIMES = 100
EOF_CORRECT_LENGTH = 10

class Test_CFDP_Eof_Input_BinStream < Minitest::Test

  def test_length

    # test ok
    eof = CFDP::PDUEOF.new(EOF_VALID_ARRAY)
    assert_kind_of CFDP::PDUEOF, eof

    # test nil
    assert_raises "VerifyError" do
      CFDP::PDUEOF.new(nil)
    end

    # test wrong input
    assert_raises "VerifyError" do
      eof = CFDP::PDUEOF.new("a")
    end

    # test wrong input
    assert_raises "VerifyError" do
      eof = CFDP::PDUEOF.new(12321312312)
    end

    # test empty array
    assert_raises "VerifyError" do
      header = CFDP::PDUEOF.new([])
    end

    # test below min
    assert_raises "VerifyError" do
      header = CFDP::PDUEOF.new([0])
    end

    # test anything bigger than 255 and lower than 0
    for i in 0..EOF_VALID_ARRAY.length-1

      EOF_TESTING_INPUT_TIMES.times do

        custom = EOF_VALID_ARRAY.dup
        custom[i] = rand(256..256**3)
        assert_raises "VerifyError" do
          CFDP::PDUEOF.new(custom)
        end
        custom[i] = -1*rand(256..256**3)
        assert_raises "VerifyError" do
          CFDP::PDUEOF.new(custom)
        end
      end
    end
  end

  def test_byte_1

    # test directiveCode (4 bits)
    for conditionCode in 0..(2**4)-1
      eof = CFDP::PDUEOF.new([(conditionCode<<4), 0, 0, 0, 0, 0, 0, 0, 0])
      assert_equal conditionCode, eof.conditionCode
    end
  end

  def test_byte_2_5

    arr = Array.new
    # test fileChecksum (32 bits)
    EOF_TESTING_INPUT_TIMES.times{arr << rand(0..(2**32)-1)}
    for fileChecksum in arr
      arr = []
      arr << 0
      arr << ((fileChecksum & 0xFF000000) >> (8*3))
      arr << ((fileChecksum & 0x00FF0000) >> (8*2))
      arr << ((fileChecksum & 0x0000FF00) >> (8*1))
      arr << ((fileChecksum & 0x000000FF) >> (8*0))
      arr += [0, 0, 0, 0]
      eof = CFDP::PDUEOF.new(arr)
      assert_equal fileChecksum, eof.fileChecksum
    end
  end

  def test_byte_6_9

    arr = Array.new
    # test fileChecksum (32 bits)
    EOF_TESTING_INPUT_TIMES.times{arr << rand(0..(2**32)-1)}
    for fileSize in arr
      arr = [0, 0, 0, 0, 0]
      arr << ((fileSize & 0xFF000000) >> (8*3))
      arr << ((fileSize & 0x00FF0000) >> (8*2))
      arr << ((fileSize & 0x0000FF00) >> (8*1))
      arr << ((fileSize & 0x000000FF) >> (8*0))
      eof = CFDP::PDUEOF.new(arr)
      assert_equal fileSize, eof.fileSize
    end
  end
end

class Test_CFDP_Eof_Input_Hash < Minitest::Test

  def test_initialize

    # test ok
    eof = CFDP::PDUEOF.new(EOF_VALID_HASH)
    assert_kind_of CFDP::PDUEOF, eof
    assert eof.valid?

    # test empty eof
    eof = CFDP::PDUEOF.new
    assert_kind_of CFDP::PDUEOF, eof

    # test missing any element
    EOF_VALID_HASH.keys.each do |key|
      newHash = EOF_VALID_HASH.dup
      newHash.delete(key)
      eof = CFDP::PDUEOF.new(newHash)
      assert !eof.valid?
    end

    # test bug for elements  < 0
    EOF_VALID_HASH.each do |key, value|
      newHash = EOF_VALID_HASH.dup
      newHash[key] = -1
      eof = CFDP::PDUEOF.new(newHash)
      assert !eof.valid?
    end

    # test bug for element 4 bits
    symbols = [:conditionCode]
    symbols.each do |symbol|
      newHash = EOF_VALID_HASH.dup
      newHash[symbol] = rand(2**4..256)
      eof = CFDP::PDUEOF.new(newHash)
      assert !eof.valid?
    end

    # test bug for element 32 bits
    symbols = [:fileChecksum, :fileSize]
    symbols.each do |symbol|
      EOF_TESTING_INPUT_TIMES.times do
        newHash = EOF_VALID_HASH.dup
        newHash[symbol] = rand(2**32..2**40)
        eof = CFDP::PDUEOF.new(newHash)
        assert !eof.valid?
      end
    end

    # test wrong type input
    symbols = EOF_VALID_HASH.keys
    symbols.each do |symbol|
      newHash = EOF_VALID_HASH.dup
      # test string
      newHash[symbol] = "a"
      assert !CFDP::PDUEOF.new(newHash).valid?

      # test string
      newHash[symbol] = "abcd"
      assert !CFDP::PDUEOF.new(newHash).valid?

      # test string
      newHash[symbol] = "1"
      assert !CFDP::PDUEOF.new(newHash).valid?

      # test array
      newHash[symbol] = [1]
      assert !CFDP::PDUEOF.new(newHash).valid?

      # test array
      newHash[symbol] = [1, 2, 3]
      assert !CFDP::PDUEOF.new(newHash).valid?

      # test array
      newHash[symbol] = [1, [1]]
      assert !CFDP::PDUEOF.new(newHash).valid?

      # test Hash
      newHash[symbol] = {symbol: 1}
      assert !CFDP::PDUEOF.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>1}
      assert !CFDP::PDUEOF.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>[1]}
      assert !CFDP::PDUEOF.new(newHash).valid?
    end
  end
end

class Test_CFDP_Eof_Methods < Minitest::Test


  def test_method_length

    eof = CFDP::PDUEOF.new(EOF_VALID_HASH)
    assert_equal EOF_CORRECT_LENGTH, eof.length

    newHash = EOF_VALID_HASH.dup
    newHash[:conditionCode] = 1
    eof = CFDP::PDUEOF.new(newHash)
    assert_equal EOF_CORRECT_LENGTH, eof.length
  end

  def test_method_pack

    eof = CFDP::PDUEOF.new(EOF_VALID_HASH)
    assert_equal [4, 0, 0, 0, 0, 0, 0, 0, 0, 0], eof.pack

    valid_array = [0, 147, 140, 133, 158, 0, 1, 144, 0]
    eof = CFDP::PDUEOF.new(valid_array)
    assert_equal [4]+valid_array, eof.pack

    valid_array = [0, 105, 236, 217, 249, 0, 1, 144, 0]
    eof = CFDP::PDUEOF.new(valid_array)
    assert_equal [4]+valid_array, eof.pack
  end

  def test_method_to_s

    assert_kind_of String, CFDP::PDUEOF.new(EOF_VALID_HASH).to_s
    assert_kind_of String, CFDP::PDUEOF.new(EOF_VALID_ARRAY).to_s
  end
end