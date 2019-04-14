require 'minitest/autorun'

require_relative 'mock/utils.rb'
require_relative '../cfdp'

METADATA_VALID_HASH = {segmentationControl:0, fileSize:0, sourceFileName:"a", destinationFileName:"b"}
METADATA_VALID_ARRAY = [0, 0, 0, 0, 0, 0, 0]
METADATA_TESTING_INPUT_TIMES = 100
METADATA_CORRECT_LENGTH = 10

class Test_CFDP_Metadata_Input_BinStream <  Minitest::Test

  def test_length

    # test ok
    metadata = CFDP::PDUMetadata.new(METADATA_VALID_ARRAY)
    assert_kind_of CFDP::PDUMetadata, metadata

    # test empty
    metadata = CFDP::PDUMetadata.new
    assert_kind_of CFDP::PDUMetadata, metadata

    # test nil
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new(nil)
    end

    # test wrong input
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new("a")
    end

    # test wrong input
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new(12321312312)
    end

    # test empty array
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new([])
    end

    # test below min
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new([0])
    end

    # test below min
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new([0, 0, 0, 0, 0])
    end    

    # test below min
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new([0, 0, 0, 0, 0, 0])
    end

    # test wrong first LV value
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new([0, 0, 0, 0, 0, 12, 0])
    end

    # test wrong first LV value
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new([0, 0, 0, 0, 0, 1, 0])
    end

    # test wrong first LV value
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new([0, 0, 0, 0, 0, 1, 0])
    end    

    # test wrong second LV value
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new([0, 0, 0, 0, 0, 1, 0, 1])
    end 

    # test wrong second LV value
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new([0, 0, 0, 0, 0, 0, 2, 1])
    end

    # test wrong second LV value
    assert_raises "VerifyError" do
      metadata = CFDP::PDUMetadata.new([0, 0, 0, 0, 0, 1, 12, 2, 1])
    end

    # test anything bigger than 255 and lower than 0
    for i in 0..METADATA_VALID_ARRAY.length-1

      METADATA_TESTING_INPUT_TIMES.times do

        customMetadata = METADATA_VALID_ARRAY.dup
        customMetadata[i] = rand(256..256**3)
        assert_raises "VerifyError" do
          CFDP::PDUMetadata.new(customMetadata)
        end
        customMetadata[i] = -1
        assert_raises "VerifyError" do
          CFDP::PDUMetadata.new(customMetadata)
        end
      end
    end
  end

  def test_byte_1

    # check for segmentation control
    metadata = CFDP::PDUMetadata.new([1<<7, 0, 0, 0, 0, 0, 0])
    assert_equal 1, metadata.segmentationControl

    # check for segmentation control
    metadata = CFDP::PDUMetadata.new([0, 0, 0, 0, 0, 0, 0])
    assert_equal 0, metadata.segmentationControl

    # check for wrong bits 2-8
    arr = Array.new
    METADATA_TESTING_INPUT_TIMES.times{arr << rand(1..(2**7)-1)}

    for test in arr

      assert_raises do

        CFDP::PDUMetadata.new([test, 0, 0, 0, 0, 0, 0])
      end
    end
  end

  def test_byte_2_5

    arr = Array.new

    # test offset (32 bits)
    METADATA_TESTING_INPUT_TIMES.times{arr << rand(0..(2**32)-1)}
    for fileSize in arr

      customArray = [0]
      customArray << ((fileSize & 0xFF000000) >> (8*3))
      customArray << ((fileSize & 0x00FF0000) >> (8*2))
      customArray << ((fileSize & 0x0000FF00) >> (8*1))
      customArray << ((fileSize & 0x000000FF) >> (8*0))
      customArray += [0, 0]
      metadata = CFDP::PDUMetadata.new(customArray)
      assert_equal fileSize, metadata.fileSize
    end
  end
end

class Test_CFDP_Metadata_Input_Hash < Minitest::Test

  def test_initialize

    # test ok metadata
    metadata = CFDP::PDUMetadata.new(METADATA_VALID_HASH)
    assert_kind_of CFDP::PDUMetadata, metadata
    assert metadata.valid?

    # test empty metadata
    metadata = CFDP::PDUMetadata.new
    assert_kind_of CFDP::PDUMetadata, metadata

    # test missing any element
    METADATA_VALID_HASH.keys.each do |key|
      newHash = METADATA_VALID_HASH.dup
      newHash.delete(key)
      metadata = CFDP::PDUMetadata.new(newHash)
      assert !metadata.valid?
    end

    # test bug for elements < 0
    METADATA_VALID_HASH.each do |key, value|
      newHash = METADATA_VALID_HASH.dup
      newHash[key] = -1*rand(2**1..256)
      metadata = CFDP::PDUMetadata.new(newHash)
      assert !metadata.valid?
    end

    # test all 1 bit thing
    symbols = [:segmentationControl]
    symbols.each do |symbol|

      METADATA_TESTING_INPUT_TIMES.times do

        newHash = METADATA_VALID_HASH.dup
        newHash[symbol] = rand(2**1..2**10)
        metadata = CFDP::PDUMetadata.new(newHash)
        assert !metadata.valid?
      end
    end

    # test wrong type input
    symbols = METADATA_VALID_HASH.keys
    symbols.each do |symbol|

      newHash = METADATA_VALID_HASH.dup

      # test array
      newHash[symbol] = [1]
      assert !CFDP::PDUMetadata.new(newHash).valid?

      # test array
      newHash[symbol] = [1, 2, 3]
      assert !CFDP::PDUMetadata.new(newHash).valid?

      # test array
      newHash[symbol] = [1, [1]]
      assert !CFDP::PDUMetadata.new(newHash).valid?

      # test Hash
      newHash[symbol] = {symbol: 1}
      assert !CFDP::PDUMetadata.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>1}
      assert !CFDP::PDUMetadata.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>[1]}
      assert !CFDP::PDUMetadata.new(newHash).valid?
    end
  end
end

class Test_CFDP_Metadata_Methods < Minitest::Test

  def test_method_length

    newHash = METADATA_VALID_HASH.dup
    metadata = CFDP::PDUMetadata.new(newHash)
    assert_equal metadata.length, 10

    newHash = METADATA_VALID_HASH.dup
    newHash[:segmentationControl] = 1
    metadata = CFDP::PDUMetadata.new(newHash)
    assert_equal metadata.length, 10

    newHash = METADATA_VALID_HASH.dup
    newHash[:sourceFileName] = "awaka"
    newHash[:destinationFileName] = "awaka1"
    metadata = CFDP::PDUMetadata.new(newHash)
    assert_equal metadata.length, 1+5+6+7
  end

  def test_method_pack

    metadata = CFDP::PDUMetadata.new(METADATA_VALID_HASH)
    assert_equal metadata.pack, [7] + [0, 0, 0, 0, 0, 1, 97, 1, 98]

    valid_array = [128, 0, 1, 144, 0, 16, 47, 114, 97, 109, 47, 116, 101, 115, 116, 49, 48, 48, 46, 116, 120, 116, 24, 67, 58, 47, 100, 111, 119, 110, 108, 105, 110, 107, 115, 47, 116, 101, 115, 116, 49, 48, 48, 46, 116, 120, 116]
    metadata = CFDP::PDUMetadata.new(valid_array)
    assert_equal metadata.pack, [7] + valid_array

    valid_array = [128, 0, 0, 120, 0, 15, 47, 114, 97, 109, 47, 116, 101, 115, 116, 51, 48, 46, 116, 120, 116, 23, 67, 58, 47, 100, 111, 119, 110, 108, 105, 110, 107, 115, 47, 116, 101, 115, 116, 51, 48, 46, 116, 120, 116]
    metadata = CFDP::PDUMetadata.new(valid_array)
    assert_equal metadata.pack, [7] + valid_array
  end

  def test_method_to_s

    # just shouldn't throw error and return a string
    assert_kind_of String, CFDP::PDUMetadata.new(METADATA_VALID_HASH).to_s
    assert_kind_of String, CFDP::PDUMetadata.new(METADATA_VALID_ARRAY).to_s
  end
end