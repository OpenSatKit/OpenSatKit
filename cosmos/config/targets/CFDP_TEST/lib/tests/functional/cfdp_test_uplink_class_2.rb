class Uplink_Class_2 < Cosmos::Test

  def initialize
    super
  end

  def manual_test_initialize

      directory = ask "Insert directory of #{Cosmos::Test.current_test_case} test"
      # load config here
      # should receive the root dir of the test
      # e.g. "C:\COSMOS\VisionaRTS\outputs\tests\cfdp\test_miss_eof\2017_8_18_13_30_25\"
      return unless Dir.exist?(directory)
      return unless load(directory+"/input/config.rb")

      CF_available?(DEFAULT_TIMEOUT)

      # main test procedure
      DOWNLINK_FILE_SIZES.each do |size|

        # Reset interface
        cmd("CFDP_TEST RESET_COUNTERS")

        # copy file to downlink_directory first
        FileUtils.cp(directory+"/input/"+TEST_FILE_NAME+size.to_s+".txt",
          DOWNLINK_DIRECTORY+TEST_FILE_NAME+size.to_s+".txt"
        )

        # drop packets
        rcvd_hash = Hash.new; sent_hash = Hash.new
        begin; RCVD_HASH[size].nil?; rcvd_hash[size] = RCVD_HASH[size]; rescue; rcvd_hash[size] = nil; end
        begin; SENT_HASH[size].nil?; sent_hash[size] = SENT_HASH[size]; rescue; sent_hash[size] = nil; end

        miss_pdus(miss_sent_pdus:sent_hash[size], miss_received_pdus:rcvd_hash[size])
        wait(DEFAULT_WAIT)

        # send file to satellite
        uplinkTransfer( 2,
                        SAT_ID,
                        DOWNLINK_DIRECTORY+TEST_FILE_NAME+size.to_s+'.txt',
                        UPLINK_DIRECTORY+TEST_FILE_NAME+size.to_s+'.txt',
                        calculateWaitTime(size<<10, UPLOAD_LINK)+INACTIVITY_TIMEOUT
        )
        wait(DEFAULT_WAIT)
      end
  end

  def initialize_vars

    @timeStarted = Time.now
    @memoryUsed = tlm("CF CF_HKPACKET APP_MEMINUSE")
    @pduRcvd = tlm("CF CF_HKPACKET APP_PDUSRECEIVED")
    @ch0PDUSent = tlm("CF CF_HKPACKET DOWN_CH0_PDUSSENT")
    cmd("CFDP_TEST RESET_COUNTERS")
  end

  def write_output_result

    # This is to finish passed result
    appendFile(@resultFile, "1\n")

    resultsHash = Hash.new
    resultsHash["Time"] = Time.now-@timeStarted
    resultsHash["Memory"] = tlm("CF CF_HKPACKET APP_MEMINUSE") - @memoryUsed
    resultsHash["PDUs Received by s/c"] = tlm("CF CF_HKPACKET APP_PDUSRECEIVED") - @pduRcvd
    resultsHash["PDUs Sent by s/c"] = tlm("CF CF_HKPACKET DOWN_CH0_PDUSSENT") - @ch0PDUSent

    output = ""
    resultsHash.each {|key, value| output << "#{key}: #{value}\n"}
    appendFile(@resultFile, output)
  end

  def get_random_combination(fileSize)

      missHash = Hash.new
      fileDirectiveArray = [1, 2]
      picks = CFDP::PDUCLASSES.sample(1 + rand(CFDP::PDUCLASSES.count))
      totalPdus = PDUS?(GROUND_MAX_PDU_SIZE, fileSize)

      picks.each do |classe|

        next if classe == CFDP::PDUPrompt
        missHash[classe] = classe == CFDP::PDUFileData ?
          (1..totalPdus).to_a.sample(1 + rand((1..totalPdus).to_a.count)%(MAX_PDU_FD_LOSS*totalPdus)).sort :
          fileDirectiveArray.sample(1 + rand(fileDirectiveArray.count))
      end

      return missHash
  end

  def setup(*args)

    # I don't want to create directory setup upon initiating a test group or suite
    return if Cosmos::Test.current_test_case.eql?("setup")

    rcvd_hash = Hash.new; sent_hash = Hash.new
    @timeStarted = Time.now
    @finalTestDir = createMainTestDir(Cosmos::Test.current_test_suite, Cosmos::Test.current_test, Cosmos::Test.current_test_case)
    @resultFile = @finalTestDir + "/output/result.txt"
    appendFile(@resultFile, "0")
    @configFile = @finalTestDir+"/input/config.rb"

    puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"

    CF_available?(DEFAULT_TIMEOUT)

    appendFile(@configFile.to_s,
                            "DOWNLINK_FILE_SIZES = #{DOWNLINK_FILE_SIZES}\n" <<
                            "TEST_FILE_NAME = \"#{TEST_FILE_NAME}\"\n" <<
                            "DEFAULT_WAIT = #{DEFAULT_WAIT}\n" <<
                            "DEFAULT_TIMEOUT = #{DEFAULT_TIMEOUT}\n" <<
                            "SAT_ID = #{SAT_ID}\n" <<
                            "DOWNLOAD_LINK = #{DOWNLOAD_LINK}\n" <<
                            "UPLOAD_LINK = #{UPLOAD_LINK}\n" <<
                            "INACTIVITY_TIMEOUT = #{INACTIVITY_TIMEOUT}\n" <<
                            "GROUND_MAX_PDU_SIZE = #{GROUND_MAX_PDU_SIZE}\n" <<
                            "MAX_PDU_FD_LOSS = #{MAX_PDU_FD_LOSS}\n" <<
                            "RCVD_HASH = Hash.new\n" <<
                            "SENT_HASH = Hash.new\n\n"
    )

    if args[0].is_a?(Hash)

      rcvd_hash = args[0][:miss_received_pdus].nil? ? Hash.new : args[0][:miss_received_pdus]
      sent_hash = args[0][:miss_sent_pdus].nil? ? Hash.new : args[0][:miss_sent_pdus]
    end

    DOWNLINK_FILE_SIZES.each do |size|

      # reset counters
      cmd("CFDP_TEST RESET_COUNTERS")
      wait(DEFAULT_WAIT)

      # create file
      createRandomFile(DOWNLINK_DIRECTORY+TEST_FILE_NAME+size.to_s+".txt", size)

      # copy files to input config
      FileUtils.cp(DOWNLINK_DIRECTORY+TEST_FILE_NAME+size.to_s+".txt",
        @finalTestDir+"/input/"+TEST_FILE_NAME+size.to_s+".txt"
      )

      # Drop packets
      appendFile(@configFile, "RCVD_HASH[#{size}] = #{printFullHash(rcvd_hash[size])}\nSENT_HASH[#{size}] = #{printFullHash(sent_hash[size])}\n")
      miss_pdus(miss_received_pdus:rcvd_hash[size], miss_sent_pdus:sent_hash[size])
      wait(DEFAULT_WAIT)

      # send file to satellite
      uplinkTransfer( 2,
                      SAT_ID,
                      DOWNLINK_DIRECTORY+TEST_FILE_NAME+size.to_s+'.txt',
                      UPLINK_DIRECTORY+TEST_FILE_NAME+size.to_s+'.txt',
                      calculateWaitTime(size<<10, UPLOAD_LINK)+INACTIVITY_TIMEOUT
      )

      # delete created file
      File.delete(DOWNLINK_DIRECTORY+TEST_FILE_NAME+size.to_s+".txt")
      wait(DEFAULT_WAIT)
    end

    wait(DEFAULT_WAIT)
  end

  def test_no_loss_pdu

    puts "Running #{Cosmos::Test.current_test_suite}: #{Cosmos::Test.current_test}: #{Cosmos::Test.current_test_case} on dir #{@finalTestDir}"
    
    initialize_vars

    (manual_test_initialize; teardown; return) if $manual

    setup

    write_output_result; teardown
  end

  def test_miss_metadata

    puts "Running #{Cosmos::Test.current_test_suite}: #{Cosmos::Test.current_test}: #{Cosmos::Test.current_test_case} on dir #{@finalTestDir}"

    initialize_vars

    (manual_test_initialize; teardown; return) if $manual

    sent_hash = Hash.new
    DOWNLINK_FILE_SIZES.each {|size| sent_hash[size] = {CFDP::PDUMetadata=>[1]}}
    setup(miss_sent_pdus:sent_hash)

    write_output_result; teardown
  end

  def test_miss_eof

    puts "Running #{Cosmos::Test.current_test_suite}: #{Cosmos::Test.current_test}: #{Cosmos::Test.current_test_case} on dir #{@finalTestDir}"

    initialize_vars

    (manual_test_initialize; teardown; return) if $manual

    sent_hash = Hash.new
    DOWNLINK_FILE_SIZES.each {|size| sent_hash[size] = {CFDP::PDUEOF=>[1]}}
    setup(miss_sent_pdus:sent_hash)

    write_output_result; teardown
  end

  def test_miss_1_fd_pdu

    puts "Running #{Cosmos::Test.current_test_suite}: #{Cosmos::Test.current_test}: #{Cosmos::Test.current_test_case} on dir #{@finalTestDir}"

    initialize_vars

    (manual_test_initialize; teardown; return) if $manual

    sent_hash = Hash.new
    DOWNLINK_FILE_SIZES.each {|size| sent_hash[size] = {CFDP::PDUFileData=>[rand(1..PDUS?(GROUND_MAX_PDU_SIZE, size))]}}
    setup(miss_sent_pdus:sent_hash)

    write_output_result; teardown
  end

  def test_miss_n_fd_pdu

    puts "Running #{Cosmos::Test.current_test_suite}: #{Cosmos::Test.current_test}: #{Cosmos::Test.current_test_case} on dir #{@finalTestDir}"

    initialize_vars

    (manual_test_initialize; teardown; return) if $manual

    sent_hash = Hash.new
    DOWNLINK_FILE_SIZES.each {|size| sent_hash[size] = {CFDP::PDUFileData=>(1..PDUS?(GROUND_MAX_PDU_SIZE, size)).to_a.sample(1 + rand((1..PDUS?(GROUND_MAX_PDU_SIZE, size)).to_a.count)%(MAX_PDU_FD_LOSS*PDUS?(GROUND_MAX_PDU_SIZE, size))).sort}}
    setup(miss_sent_pdus:sent_hash)

    write_output_result; teardown
  end

  def test_miss_rcvd_nak_pdu

    puts "Running #{Cosmos::Test.current_test_suite}: #{Cosmos::Test.current_test}: #{Cosmos::Test.current_test_case} on dir #{@finalTestDir}"

    initialize_vars

    (manual_test_initialize; teardown; return) if $manual

    rcvd_hash = Hash.new; sent_hash = Hash.new
    DOWNLINK_FILE_SIZES.each {|size| rcvd_hash[size] = {CFDP::PDUNAK=>[1]}; sent_hash[size] = {CFDP::PDUFileData=>[rand(1..PDUS?(GROUND_MAX_PDU_SIZE, size))]}}
    setup(miss_received_pdus:rcvd_hash, miss_sent_pdus:sent_hash)

    write_output_result; teardown
  end

  def test_miss_rcvd_finished_pdu

    puts "Running #{Cosmos::Test.current_test_suite}: #{Cosmos::Test.current_test}: #{Cosmos::Test.current_test_case} on dir #{@finalTestDir}"

    initialize_vars

    (manual_test_initialize; teardown; return) if $manual

    rcvd_hash = Hash.new
    DOWNLINK_FILE_SIZES.each {|size| rcvd_hash[size] = {CFDP::PDUFinished=>[1]}}
    setup(miss_received_pdus:rcvd_hash)

    write_output_result; teardown
  end

  def test_miss_sent_ack_pdu

    puts "Running #{Cosmos::Test.current_test_suite}: #{Cosmos::Test.current_test}: #{Cosmos::Test.current_test_case} on dir #{@finalTestDir}"

    initialize_vars

    (manual_test_initialize; teardown; return) if $manual

    sent_hash = Hash.new
    DOWNLINK_FILE_SIZES.each {|size| sent_hash[size] = {CFDP::PDUACK=>[1]}}
    setup(miss_sent_pdus:sent_hash)

    write_output_result; teardown
  end

  def test_miss_rcv_ack_pdu

    puts "Running #{Cosmos::Test.current_test_suite}: #{Cosmos::Test.current_test}: #{Cosmos::Test.current_test_case} on dir #{@finalTestDir}"

    initialize_vars

    (manual_test_initialize; teardown; return) if $manual

    rcvd_hash = Hash.new
    DOWNLINK_FILE_SIZES.each {|size| rcvd_hash[size] = {CFDP::PDUACK=>[1]}}
    setup(miss_received_pdus:rcvd_hash)

    write_output_result; teardown
  end

  def test_combination

    puts "Running #{Cosmos::Test.current_test_suite}: #{Cosmos::Test.current_test}: #{Cosmos::Test.current_test_case} on dir #{@finalTestDir}"

    initialize_vars

    (manual_test_initialize; teardown; return) if $manual

    rcvd_hash = Hash.new; sent_hash = Hash.new
    DOWNLINK_FILE_SIZES.each {|size| sent_hash[size] = get_random_combination(size); rcvd_hash[size] = get_random_combination(size)}
    setup(miss_received_pdus:rcvd_hash, miss_sent_pdus:sent_hash)

    write_output_result; teardown
  end

  def teardown

    return if Cosmos::Test.current_test_case.eql?("teardown")
    # delete files from downlink directory
    begin; DOWNLINK_FILE_SIZES.each {|size| File.delete(DOWNLINK_DIRECTORY+TEST_FILE_NAME+size.to_s+".txt")}; rescue; end

    # for teardown, we must remove all entries on history list
    # to free cf's memory. Type 1 is uplink, type 2 is downlink (channel is 0 in type 2 downlink)
    queueNodes = tlm("CF CF_HKPACKET APP_QNODESALLOCATED")
    dQueueNodes = tlm("CF CF_HKPACKET APP_QNODESDEALLOCATED")

    return if queueNodes == dQueueNodes

    (1..2).step(1) do |n|
      cmd("CF CF_PURGE_QUEUE_CC with TYPE #{n}, CHANNEL 0, QUEUE HISTORY, SPARE 0")
      wait(DEFAULT_WAIT)
    end

    wait_check_expression("tlm('CF CF_HKPACKET APP_QNODESDEALLOCATED') > #{dQueueNodes}", DEFAULT_TIMEOUT)
  end
end