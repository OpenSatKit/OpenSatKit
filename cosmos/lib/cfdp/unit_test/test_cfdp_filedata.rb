require 'minitest/autorun'

require_relative 'mock/utils.rb'
require_relative '../cfdp'

FILEDATA_VALID_HASH = {offset:0, data:[0]}
FILEDATA_VALID_ARRAY = [0, 0, 0, 0, 0]
FILEDATA_TESTING_INPUT_TIMES = 100
FILEDATA_CORRECT_LENGTH = 5

class Test_CFDP_FileData_Input_BinStream < Minitest::Test

  def test_length

    # test ok
    filedata = CFDP::PDUFileData.new(FILEDATA_VALID_ARRAY)
    assert_kind_of CFDP::PDUFileData, filedata

    # test nil
    assert_raises "VerifyError" do
      CFDP::PDUFileData.new(nil)
    end

    # test wrong input
    assert_raises "VerifyError" do
      filedata = CFDP::PDUFileData.new("a")
    end

    # test wrong input
    assert_raises "VerifyError" do
      filedata = CFDP::PDUFileData.new(12321312312)
    end

    # test empty array
    assert_raises "VerifyError" do
      header = CFDP::PDUFileData.new([])
    end

    # test below min
    assert_raises "VerifyError" do
      header = CFDP::PDUFileData.new([0])
    end

    # test anything bigger than 255 and lower than 0
    for i in 0..FILEDATA_VALID_ARRAY.length-1

      FILEDATA_TESTING_INPUT_TIMES.times do

        custom = FILEDATA_VALID_ARRAY.dup
        custom[i] = rand(256..256**3)
        assert_raises "VerifyError" do
          CFDP::PDUFileData.new(custom)
        end
        custom[i] = -1*rand(256..256**3)
        assert_raises "VerifyError" do
          CFDP::PDUFileData.new(custom)
        end
      end
    end
  end

  def test_byte_1_4

    arr = Array.new

    # test offset (32 bits)
    FILEDATA_TESTING_INPUT_TIMES.times{arr << rand(0..(2**32)-1)}
    for offset in arr

      customArray = []
      customArray << ((offset & 0xFF000000) >> (8*3))
      customArray << ((offset & 0x00FF0000) >> (8*2))
      customArray << ((offset & 0x0000FF00) >> (8*1))
      customArray << ((offset & 0x000000FF) >> (8*0))
      customArray += [0]
      filedata = CFDP::PDUFileData.new(customArray)
      assert_equal offset, filedata.offset
    end
  end

  def test_byte_4_end

    # test data field
    FILEDATA_TESTING_INPUT_TIMES.times do

      arr = Array.new

      FILEDATA_TESTING_INPUT_TIMES.times{arr << rand(0..(2**8)-1)}

      customArray = [0, 0, 0, 0]
      customArray += arr
      filedata = CFDP::PDUFileData.new(customArray)
      assert_equal arr, filedata.data
    end
  end
end

class Test_CFDP_FileData_Input_Hash < Minitest::Test

  def test_initialize

    # test ok
    filedata = CFDP::PDUFileData.new(FILEDATA_VALID_HASH)
    assert_kind_of CFDP::PDUFileData, filedata
    assert filedata.valid?

    # test empty filedata
    filedata = CFDP::PDUFileData.new
    assert_kind_of CFDP::PDUFileData, filedata

    # test missing any element
    FILEDATA_VALID_HASH.keys.each do |key|
      newHash = FILEDATA_VALID_HASH.dup
      newHash.delete(key)
      filedata = CFDP::PDUFileData.new(newHash)
      assert !filedata.valid?
    end

    # test bug for elements  < 0
    FILEDATA_VALID_HASH.each do |key, value|
      newHash = FILEDATA_VALID_HASH.dup
      newHash[key] = -1
      filedata = CFDP::PDUFileData.new(newHash)
      assert !filedata.valid?
    end

    # test bug for element 4 bits
    symbols = [:offset]
    symbols.each do |symbol|
      newHash = FILEDATA_VALID_HASH.dup
      newHash[symbol] = rand(2**32..2**36)
      filedata = CFDP::PDUFileData.new(newHash)
      assert !filedata.valid?
    end

    # test wrong type input
    symbols = FILEDATA_VALID_HASH.keys
    symbols.each do |symbol|
      newHash = FILEDATA_VALID_HASH.dup
      # test string
      newHash[symbol] = "a"
      assert !CFDP::PDUFileData.new(newHash).valid?

      # test string
      newHash[symbol] = "abcd"
      assert !CFDP::PDUFileData.new(newHash).valid?

      # test string
      newHash[symbol] = "1"
      assert !CFDP::PDUFileData.new(newHash).valid?

      # test no data array
      newHash[:data] = 1
      assert !CFDP::PDUFileData.new(newHash).valid?

      # test no data array
      newHash[:data] = 123
      assert !CFDP::PDUFileData.new(newHash).valid?

      # test array
      newHash[:offset] = [1]
      assert !CFDP::PDUFileData.new(newHash).valid?

      # test array
      newHash[:offset] = [1, 2, 3]
      assert !CFDP::PDUFileData.new(newHash).valid?

      # test array
      newHash[:offset] = [1, [1]]
      assert !CFDP::PDUFileData.new(newHash).valid?

      # test Hash
      newHash[symbol] = {symbol: 1}
      assert !CFDP::PDUFileData.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>1}
      assert !CFDP::PDUFileData.new(newHash).valid?

      # test Hash
      newHash[symbol] = {1=>[1]}
      assert !CFDP::PDUFileData.new(newHash).valid?
    end
  end
end

class Test_CFDP_FileData_Methods < Minitest::Test


  def test_method_length

    filedata = CFDP::PDUFileData.new(FILEDATA_VALID_HASH)
    assert_equal FILEDATA_CORRECT_LENGTH, filedata.length

    newHash = FILEDATA_VALID_HASH.dup
    newHash[:offset] = 1
    filedata = CFDP::PDUFileData.new(newHash)
    assert_equal FILEDATA_CORRECT_LENGTH, filedata.length
  end

  def test_method_pack

    filedata = CFDP::PDUFileData.new(FILEDATA_VALID_HASH)
    assert_equal [0, 0, 0, 0, 0], filedata.pack

    valid_array = [0, 0, 1, 144, 163, 146, 148, 121, 255, 158, 194, 184, 72, 114, 80, 60, 177, 243, 109, 242, 146, 40, 182, 87, 252, 5, 139, 15, 66, 131, 249, 252, 205, 143, 157, 112, 235, 239, 236, 152, 245, 218, 72, 99, 194, 177, 157, 177, 8, 119, 6, 51, 205, 194, 167, 78, 118, 253, 80, 39, 83, 217, 16, 39, 207, 34, 177, 143, 252, 169, 21, 236, 197, 190, 233, 207, 108, 21, 186, 255, 55, 203, 205, 83, 171, 170, 227, 172, 159, 252, 123, 60, 139, 187, 118, 137, 152, 142, 40, 44, 171, 129, 3, 177, 126, 148, 79, 229, 237, 244, 179, 33, 230, 73, 215, 124, 204, 18, 59, 88, 75, 226, 55, 162, 181, 176, 199, 196, 38, 88, 20, 116, 70, 101, 153, 143, 41, 153, 240, 157, 148, 180, 182, 2, 241, 180, 68, 247, 186, 220, 28, 245, 156, 151, 189, 228, 245, 203, 224, 151, 176, 215, 255, 109, 216, 173, 73, 131, 239, 124, 206, 190, 178, 128, 88, 242, 156, 236, 52, 192, 32, 152, 128, 187, 135, 141, 168, 169, 15, 191, 136, 200, 7, 164, 39, 89, 114, 185, 94, 75, 167, 128, 94, 182]
    filedata = CFDP::PDUFileData.new(valid_array)
    assert_equal valid_array, filedata.pack

    valid_array = [0, 0, 4, 176, 206, 118, 198, 243, 62, 12, 231, 127, 0, 69, 42, 225, 182, 50, 184, 74, 134, 253, 111, 49, 146, 7, 182, 84, 34, 181, 209, 148, 225, 155, 63, 250, 187, 36, 244, 115, 184, 5, 177, 183, 138, 15, 111, 193, 32, 98, 156, 71, 84, 29, 57, 119, 96, 20, 228, 234, 118, 40, 107, 157, 95, 252, 187, 135, 168, 77, 197, 63, 109, 80, 2, 224, 43, 5, 241, 206, 226, 101, 202, 121, 17, 114, 80, 235, 98, 231, 165, 209, 31, 116, 97, 92, 228, 23, 78, 170, 233, 232, 3, 65, 10, 22, 12, 235, 194, 125, 209, 171, 61, 224, 216, 154, 46, 224, 136, 219, 10, 28, 124, 174, 39, 127, 80, 56, 213, 158, 242, 214, 176, 71, 182, 152, 192, 141, 73, 124, 119, 86, 28, 48, 62, 93, 198, 192, 202, 95, 22, 11, 225, 10, 178, 121, 34, 176, 164, 60, 68, 60, 11, 252, 7, 42, 200, 161, 12, 72, 107, 20, 58, 117, 195, 208, 110, 32, 47, 27, 71, 226, 183, 50, 179, 110, 3, 130, 94, 159, 138, 139, 230, 27, 207, 119, 152, 69, 41, 6, 164, 163, 8, 83]
    filedata = CFDP::PDUFileData.new(valid_array)
    assert_equal valid_array, filedata.pack
  end

  def test_method_to_s

    assert_kind_of String, CFDP::PDUFileData.new(FILEDATA_VALID_HASH).to_s
    assert_kind_of String, CFDP::PDUFileData.new(FILEDATA_VALID_ARRAY).to_s
  end
end