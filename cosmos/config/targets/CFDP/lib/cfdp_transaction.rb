##################################################################################################
#######                                 File Description                                 #########
##################################################################################################
#
# filename:      cfdp_transaction.rb
# created:       23/10/2017
# company:       Visiona Tecnologia Espacial S.A.
# developer:     Nickollas Aranha
# reviewer:      Edipo Crispim
# utility:       This is a specific class to be used with cfdp_engine.rb
#
##################################################################################################
#######                               End File Description                               #########
##################################################################################################

require_relative "cfdp_vars"
require_relative "cfdp_engine_hkpacket"
require 'zlib'
require 'cosmos'
require 'thread'
require 'fileutils'
require "utils_visiona/utils"
require "interfaces/cosmos_cfdp_interface"

module CFDP

	class Transaction

		# Transaction CFDP vars
		attr_reader :type # "DOWNLINK" or "UPLINK"
		attr_reader :cfdpClass
		attr_reader :ID
		attr_reader :pdus # Hash that can have keys "METADATA", "FILEDATA", "EOF", "FINISHED", "NAK".
		attr_reader :timeStarted
    attr_reader :finishType # "SUCCESS" or "FAILED"

		# Transaction useful vars
		attr_reader :status #Can be either ["Active", "Finished"]
    attr_reader :timer
		attr_reader :logFile

    # Action var
    ACTIONS = {

      "EOF" =>        {LIMIT:EOF_LIMIT,         TIMEOUT:EOF_TIMEOUT,        CONDITION:1},
      "INACTIVITY" => {LIMIT:INACTIVITY_LIMIT,  TIMEOUT:INACTIVITY_TIMEOUT, CONDITION:8},
      "FINISHED" =>   {LIMIT:FINISHED_LIMIT,    TIMEOUT:FINISHED_TIMEOUT,   CONDITION:1},
      "NAK" =>        {LIMIT:NAK_LIMIT,         TIMEOUT:NAK_TIMEOUT,        CONDITION:7},
      "FINISH_LOG" => {LIMIT:FINISH_LOG_LIMIT,  TIMEOUT:FINISH_LOG_TIMEOUT}
    }

		def initialize(type, sequenceNumberHash, classe)

      Utils_visiona.verifyInput(String, type.class)
      Utils_visiona.verifyInput(Hash, sequenceNumberHash.class)

      @type = type
			@ID = sequenceNumberHash
      @cfdpClass = classe
      initialize_vars

      startLogFile
      writeLog("#{@type.upcase} transaction #{@ID} has started.")
		end

    def initialize_vars

      @timeStarted = Time.now
      @pdus = Hash.new
      @pdus["FILEDATA"] = Hash.new
      @timer = Hash.new
      @status = "ACTIVE"
    end

    def finalize

      finishLogFile
    end

    # Useful simple and verbal function definitions here.
    def haspdueof?; return !@pdus["EOF"].nil? end
    def haspdumetadata?; return !@pdus["METADATA"].nil? end
    def haspdufinished?; return !@pdus["FINISHED"].nil? end
    def haspdufiledata?(offset); return !@pdus["FILEDATA"][offset].nil? end
    def isdownlink?; return @type.eql?("DOWNLINK") end
    def isuplink?; return !isdownlink? end
    def isclass1?; return @cfdpClass==1 end
    def isclass2?; return @cfdpClass==0 end
    def isactive?; return @status.eql?("ACTIVE") end
    def isfinished?; return @status.eql?("FINISHED") end
    def hastimer?(*type); return type.empty? ? !@timer.empty? : !@timer[type[0]].nil? end

    # startLogFile description
      # UTILITY:
      #    Function will start a logFile based on started time and set it to @logFile variable.
      # INPUT:
      #    No inputs.
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    No exceptions.
    # end function description
		def startLogFile

      return unless @logFile.nil?
      @logFile = LOG_DIRECTORY + "#{@timeStarted.strftime('%Y%m%d_%H%M%S')}_#{@ID.keys[0]}-#{@ID.values[0]}.txt"
		end

    # finishLogFile description
      # UTILITY:
      #    Function will zip a logfile to .gzip and remove existing logFile
      # INPUT:
      #    No inputs
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if something wrong with file I/O
    # end function description
    def finishLogFile

      Thread.new do

        begin

          # Zips file
          Zlib::GzipWriter.open(@logFile + ".gz") do |gzip|
            File.open(@logFile) do |fp|
              while chunk = fp.read(16 * 1024) do
                gzip.write chunk
              end
            end
          end

          # Remove original
          FileUtils.rm_f(@logFile) until !File.exist?(@logFile)
        rescue Exception => err

          CFDP::CFDP_Indication("Error finishing log file for transaction #{@ID}. Error is #{err}")
          CFDP::CFDPEngine.instance.writeLog(1, "Error finishing log file for transaction #{@ID}. Error is #{err}")
        end
      end
    end

    # writeLog description
      # UTILITY:
      #    Function will write log events to specific log file.
      # INPUT:
      #    A String text
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if wrong input.
    # end function description
		def writeLog(text)

      Utils_visiona.verifyInput(String, text.class)

      return if @logFile.nil?

      Thread.new do

        File.open(@logFile, 'a+') {|file| file.puts "#{Time.now} #{text}"}
      end
		end

    # create_file description
      # UTILITY:
      #    Function will handle create file cases.
      # INPUT:
      #    No inputs.
      # RETURNS:
      #    Returns the condition code.
      # EXCEPTIONS:
      #    Can raise exception if input error
    # end function description
    def create_file

      conditionCode = 0

      # Verify if we have a metadata
      if haspdumetadata? && haspdueof?

        # Write file
        writeLog("Checking to write file \"#{@pdus["METADATA"].pduPayload.destinationFileName}\".")
        CFDP::CFDP_Indication("Checking to write file \"#{@pdus["METADATA"].pduPayload.destinationFileName}\".")

        begin

          Utils_visiona.writeFile(@pdus["METADATA"].pduPayload.destinationFileName, @pdus["FILEDATA"].dup)

          if File.size(@pdus["METADATA"].pduPayload.destinationFileName) != @pdus["EOF"].pduPayload.fileSize

            conditionCode = 6
            File.delete(@pdus["METADATA"].pduPayload.destinationFileName) unless SAVE_FILE_UPON_ERROR
          elsif Utils_visiona.calculateFileChecksum(@pdus["METADATA"].pduPayload.destinationFileName) != @pdus["EOF"].pduPayload.fileChecksum

            conditionCode = 5
            File.delete(@pdus["METADATA"].pduPayload.destinationFileName) unless SAVE_FILE_UPON_ERROR
          end
        rescue Exception => err

          conditionCode = 4
          CFDP::CFDP_Indication("Error while creating file. Error is #{err}.\n Backtrace: #{err.backtrace}")
        end

        if (conditionCode==0)

          CFDP::HKPacket.instance.eng_totalfilesrcvd+=1
          writeLog("Done writing file \"#{@pdus["METADATA"].pduPayload.destinationFileName}\".")
          CFDP::CFDP_Indication("Done writing file \"#{@pdus["METADATA"].pduPayload.destinationFileName}\".")
        elsif SAVE_FILE_UPON_ERROR

          CFDP::HKPacket.instance.eng_totalfilesrcvd+=1
          writeLog("Done writing file with error #{CFDP.conditionCodeToStr(conditionCode)} \"#{@pdus["METADATA"].pduPayload.destinationFileName}\".")
          CFDP::CFDP_Indication("Done writing file with error #{CFDP.conditionCodeToStr(conditionCode)} \"#{@pdus["METADATA"].pduPayload.destinationFileName}\".")
        else

          CFDP::HKPacket.instance.updateVar(conditionCode)
          writeLog("Error #{CFDP.conditionCodeToStr(conditionCode)} in writing file \"#{@pdus["METADATA"].pduPayload.destinationFileName}\".")
          CFDP::CFDP_Indication("Error #{CFDP.conditionCodeToStr(conditionCode)} in writing file \"#{@pdus["METADATA"].pduPayload.destinationFileName}\".")
        end

        return conditionCode
      end
    end

    # insertTimer description
      # UTILITY:
      #    Function will insert a "interval" Timer, according to type
      # INPUT:
      #    A String type. Can be ["EOF", "FINISHED", "INACTIVITY", "NAK", "FINISH_LOG"]
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if input error.
    # end function description
    def insertTimer(type)

      Utils_visiona.verifyInput(String, type.class)

      if hastimer?(type)

        writeLog("Timer of type: #{type} already exists. Overriding")
        disableTimer(type)
      end

      @timer[type] = Hash.new
      @timer[type]["Started"] = Time.now
      @timer[type]["Tries"] = 0
      writeLog("Timer of type:#{type} inserted.")
    end

    # resetTimer description
      # UTILITY:
      #    Function will reset a Timer "interval" type to be reseted
      # INPUT:
      #    A String type
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if input error
    # end function description
    def resetTimer(type)

      Utils_visiona.verifyInput(String, type.class)

      unless hastimer?(type)

        writeLog("Timer of type: #{type} does not exist. Inserting new timer.")
        insertTimer(type)
        return
      end

      @timer[type].store "Started", Time.now
      writeLog("Timer of type:#{type} reseted.")
    end

    # disableTimer description
      # UTILITY:
      #    Function will disable timer on a given transaction.
      # INPUT:
      #    A string type
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #   Can raise exceptions if input error
    # end function description
    def disableTimer(type)

      Utils_visiona.verifyInput(String, type.class)

      return unless hastimer?(type)
      @timer.delete(type)
      writeLog("Timer of type #{type} disabled")
    end

    # handleTimeout description
      # UTILITY:
      #    Function will verify what action to take based on "TimerAction".
      # INPUT:
      #    No inputs
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #   Can raise exceptions if input error.
    # end function description
    def handleTimeout

      return unless hastimer?

      @timer.keys.each do |timerType|

        # This is because @timer can be modified during an iteration
        next if (@timer[timerType].nil? || @timer[timerType].empty?)

        if Time.now > (@timer[timerType]["Started"] + ACTIONS[timerType][:TIMEOUT])

          writeLog("Reached timeout of type #{timerType}")

          # close log File.
          if timerType.eql?("FINISH_LOG")

            disableTimer(timerType)
            writeLog("Zipping log file.")
            finishLogFile
            next
          end

          # we reached a timeout here. Must choose what to do
          # verify if limit has been exceeded
          if @timer[timerType]["Tries"] >= ACTIONS[timerType][:LIMIT]

            # Limit reached, finish transaction and generate an error. Insert this into LOG
            CFDP::CFDP_Indication("Transaction #{@ID} has detected an error. Error is: #{timerType}_LIMIT. Finishing transaction.")
            writeLog("Transaction #{@ID} has detected an error. Error is: #{timerType}_LIMIT. Finishing transaction.")
            CFDP::HKPacket.instance.updateVar(ACTIONS[timerType][:CONDITION])
            finish("FAILED")
          else

            # Limit not reached, reset Timer, re-send things and update transaction
            resetTimer(timerType)

            # Verify action
            case timerType
            when "EOF", "FINISHED"

              # Must re-send PDU from that transaction and update limit counter.
              writeLog("#{timerType}_LIMIT reached, re-sending PDU")
              CFDPEngine.instance.insertIntoBuffer(@pdus[timerType])
              @timer[timerType]["Tries"]+=1
            when "NAK"

              # check if my NAK has been responded
              nakPdu = verifyMissingPDU

              if nakPdu.nil?

                writeLog("NAK has been fully responded.")
                disableTimer("NAK")
                finish_downlink_transaction if haspdueof?
                next
              elsif nakPdu.pack == @pdus["NAK"].pack

                writeLog("NAK_LIMIT reached, re-sending PDU")
                @timer[timerType]["Tries"]+=1
              else

                writeLog("NAK partially responded. Generated new NAK #{nakPdu.pack.to_s}.")
                writeLog("Overriding tries of type #{timerType} to 0.")
                @pdus["NAK"] = nakPdu
                @timer[timerType]["Tries"]=0
              end

              CFDPEngine.instance.insertIntoBuffer(nakPdu)
            end
          end
        end
      end
    end

    # getNextOffset description
      # UTILITY:
      #    Function will receive a offset (could be nearest offset) and calculate
      #     the next offset if transaction is active.
      # INPUT:
      #    A Integer offset
      # RETURNS:
      #    Returns a Integer or nil if offset is the last from the array
      # EXCEPTIONS:
      #    Can raise exception if input error
    # end function description
    def getNextOffset(offset)

      Utils_visiona.verifyInput(Integer, offset.class)

      if isactive?

        actualOffset = @pdus["FILEDATA"].keys.select{|currentOffset| currentOffset<=offset}.max
        currentIndex = @pdus["FILEDATA"].keys.sort.index(actualOffset)

        return nil if currentIndex==@pdus.length-1
        return @pdus["FILEDATA"].keys.sort[currentIndex+1]
      end
    end

    # verifyMissingPDU description
      # UTILITY:
      #    Function will check for missing PDUs
      # INPUT:
      #    No inputs
      # RETURNS:
      #    Returns nil if no missing PDUS,
      #   returns CFDP::PDUNAK otherwise.
      # EXCEPTIONS:
      #    Can raise exception if transaction is not active.
    # end function description
    def verifyMissingPDU

      startOffset = 0; endOffset = 0; scopeEnd = 0
      requestArray = Array.new

      raise unless isactive?

      writeLog("Verifying file completude")

      # first verify if we have metadata
      if @pdus["METADATA"].nil?

        requestArray << {startOffset:0, endOffset:0}
      else

        fileSize = @pdus["METADATA"].pduPayload.fileSize
        scopeEnd = @pdus["METADATA"].pduPayload.fileSize

        dataArray = @pdus["FILEDATA"].keys.sort
        # verify for offset 0
        requestArray << {startOffset:0, endOffset:dataArray[0]} unless dataArray[0] == 0

        for i in 0..(dataArray.length-1)

          startOffset = dataArray[i]

          # non error cases
          if (i == (dataArray.length-1))

            break if (startOffset + @pdus["FILEDATA"][startOffset].pduPayload.data.length) == fileSize

            # we didn't reach total file size, verify if we have EOF
            if haspdueof?
              # we lost pdus between last pdu and eof
              startOffset += @pdus["FILEDATA"][startOffset].pduPayload.data.length
              endOffset = fileSize
              requestArray << {startOffset:startOffset, endOffset:endOffset}
            end
            break
          end

          # verify for miss pdus between pdus
          if (startOffset + @pdus["FILEDATA"][startOffset].pduPayload.data.length) != dataArray[i+1]

            startOffset += @pdus["FILEDATA"][startOffset].pduPayload.data.length
            endOffset = dataArray[i+1]
            requestArray << {startOffset:startOffset, endOffset:endOffset}
            break if (8+(8*(requestArray.length+1))) >= MAX_PDU_SIZE
          end
        end
      end

      return nil if requestArray.empty?
      return CFDP.generatePDU("NAK", {sourceID:DESTINATIONID, destinationID:MYID, sequenceNumber:@ID[DESTINATIONID], direction:1, transmissionMode:0,
          scopeStart:0, scopeEnd:scopeEnd, segmentRequests:requestArray})
    end

    # finish_downlink_transaction description
      # UTILITY:
      #    Function will send a FINISHED pdu and write data to a file (finish procedure from downlink).
      # INPUT:
      #    No inputs
      # RETURNS:
      #   No returns
      # EXCEPTIONS:
      #    No exceptions.
    # end function description
    def finish_downlink_transaction

      conditionCode = create_file

      writeLog("Cleaning buffer from transaction #{@ID}")
      CFDP::CFDPEngine.instance.clean_buffer(@ID)

      # transaction is complete, send FINISHED Pdu
      finishedPdu = CFDP.generatePDU("FINISHED", {sourceID:DESTINATIONID, destinationID:MYID, conditionCode:conditionCode, endSystemStatus:1,
        deliveryCode:(conditionCode == 0 ? 0 : 1), fileStatus:(conditionCode == 4 ? 1 : 2), direction:1, transmissionMode:0, sequenceNumber:@ID[DESTINATIONID]})
      writeLog("Generated FINISHED PDU: #{finishedPdu.pack.to_s}")
      update(finishedPdu)

      CFDP::CFDPEngine.instance.insertIntoBuffer(finishedPdu)

      CFDP::CFDP_Indication("Transaction #{@ID} finished with <#{CFDP.conditionCodeToStr(conditionCode)}> condition.")
      writeLog("Transaction #{@ID} finished with <#{CFDP.conditionCodeToStr(conditionCode)}> condition.")
    end

    # nakHandler description
      # UTILITY:
      #    Function will receive a NAK Pdu and process it by verifying how many requests it has received
      #   and which offsets it needs. The function them update the buffer with desired FileDatas or metadata
      # INPUT:
      #    A CFDP::PDUPacket
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if input error
    # end function description
    def nakHandler(nakPdu)

      Utils_visiona.verifyInput(CFDP::PDUPacket, nakPdu.class)
      Utils_visiona.verifyInput(CFDP::PDUNAK, nakPdu.pduPayload.class)

      writeLog("Entered NAKHandler function.")

      # check if transaction is active
      if isactive?

        # re-send missed pdu
        requests = nakPdu.pduPayload.segmentRequests.length
        for i in 0..(requests-1)

          startOffset = nakPdu.pduPayload.segmentRequests[i][:startOffset]
          endOffset = nakPdu.pduPayload.segmentRequests[i][:endOffset]

          if (startOffset == 0) && (endOffset == 0)

            # this means that I have to send a metadata PDU
            writeLog("NAKHandler: ")
            CFDP::CFDPEngine.instance.insertIntoBuffer(@pdus["METADATA"])
          else

            until startOffset.nil? || (startOffset >= endOffset)
              writeLog("NAKHandler: ")
              CFDP::CFDPEngine.instance.insertIntoBuffer(@pdus["FILEDATA"][startOffset])
              startOffset = getNextOffset(startOffset)
            end
          end
        end
      end
    end

    # update description
      # UTILITY:
      #    Function will receive a PDU and update transaction.
      #    This function will handle timer events, but won't finish transactions.
      # INPUT:
      #    A CFDP::PDUPacket
      # RETURNS:
      #    No returns
      # EXCEPTIONS:
      #    Can raise exception if input error
    # end function description
    def update(pdu)

      Utils_visiona.verifyInput(CFDP::PDUPacket, pdu.class)

      pdu = pdu.dup

      case pdu.pduPayload
      when CFDP::PDUMetadata

        if haspdumetadata?
          writeLog("Received duplicated pdu #{pdu.pduPayload.class}. Ignoring.") unless isuplink?
        else
          @pdus.store "METADATA", pdu
          writeLog("Updated transaction with #{pdu.pduPayload.class} and buffer: #{pdu.pack.to_s}")
          CFDP::CFDP_Indication("Metadata #{isuplink? ? "sent" : "received"} for transaction #{@ID}")
          (hastimer?("INACTIVITY") ? resetTimer("INACTIVITY") : insertTimer("INACTIVITY")) if isdownlink?
        end
      when CFDP::PDUFileData

        if haspdufiledata?(pdu.pduPayload.offset)
          writeLog("Received duplicated pdu #{pdu.pduPayload.class} with offset #{pdu.pduPayload.offset}. Ignoring.") unless isuplink?
        else
          @pdus["FILEDATA"].store pdu.pduPayload.offset, pdu
          writeLog("Updated transaction with #{pdu.pduPayload.class} and buffer: #{pdu.pack.to_s}")
          resetTimer("INACTIVITY") unless isuplink?
        end
      when CFDP::PDUEOF

        if haspdueof?
          writeLog("Received duplicated pdu #{pdu.pduPayload.class}. Ignoring.") unless isuplink?
        else
          @pdus.store "EOF", pdu
          writeLog("Updated transaction with #{pdu.pduPayload.class} and buffer: #{pdu.pack.to_s}")
          CFDP::CFDP_Indication("EOF #{isuplink? ? "sent" : "received"} for transaction #{@ID}")
          insertTimer("EOF") unless ((isclass1? && isuplink?) || isdownlink? || hastimer?("EOF"))
        end
      when CFDP::PDUFinished

        if haspdufinished?
          writeLog("#{isuplink? ? "Received" : "Sent"} duplicated pdu #{pdu.pduPayload.class}. Ignoring.") unless isuplink?
        else
          @pdus.store "FINISHED", pdu
          writeLog("Updated transaction with #{pdu.pduPayload.class} and buffer: #{pdu.pack.to_s}")
          insertTimer("FINISHED") unless (isuplink? || hastimer?("FINISHED"))
        end
      when CFDP::PDUNAK

        if @pdus["NAK"] == pdu
          writeLog("Received duplicated pdu #{pdu.pduPayload.class}. Ignoring.") unless isdownlink?
        else
          if isdownlink?
            @pdus.store "NAK", pdu
            writeLog("Updated transaction with #{pdu.pduPayload.class} and buffer: #{pdu.pack.to_s}")
            insertTimer("NAK") unless hastimer?("NAK")
          else
            nakHandler(pdu)
          end
        end
      else
        writeLog("Invalid PDU Class #{pdu.pduPayload.class} with buffer #{pdu.pack.to_s} in update method.")
      end
    end

    # finish description
      # UTILITY:
      #    Function will make a transaction FINISHED and do finished procedures.
      # INPUT:
      #    No inputs
      # RETURNS:
      #    No returns
      # EXCEPTIONS:
      #    No exceptions.
    # end function description
    def finish(finishtype)

      return unless isactive?

      @finishType = finishtype

      # HK Packet update
      CFDP::HKPacket.instance.eng_inprogresstrans-=1
      if @finishType.eql?("FAILED")
        CFDP::HKPacket.instance.eng_totalfailedtrans+=1
        CFDP::HKPacket.instance.eng_lastfailedtrans = @ID.to_s
      else
        CFDP::HKPacket.instance.eng_totalsuccesstrans+=1
      end

      if @type.eql?("DOWNLINK")

        (@finishType.eql?("SUCCESS") ? CFDP::HKPacket.instance.eng_down_successdownlinks+=1 : CFDP::HKPacket.instance.eng_down_faileddownlinks+=1)
        CFDP::HKPacket.instance.eng_down_lastfiledownlinked = @pdus["METADATA"].pduPayload.destinationFileName unless @finishType.eql?("FAILED")
      else

        (@finishType.eql?("SUCCESS") ? CFDP::HKPacket.instance.eng_up_successcounter+=1 : CFDP::HKPacket.instance.eng_up_failedcounter+=1)
        unless @finishType.eql?("FAILED")

          CFDP::HKPacket.instance.eng_up_lastfileuplinked = @pdus["METADATA"].pduPayload.destinationFileName
          CFDP::HKPacket.instance.eng_totalfilessent+=1
        end
      end

      # Other procedures
      str = "Finishing transaction #{@ID}. Time elapsed: #{Time.now-@timeStarted} seconds"
      CFDP::CFDP_Indication(str)
      writeLog(str)

      ACTIONS.each {|timerType, values| disableTimer(timerType)}

      @status = "FINISHED"
      insertTimer("FINISH_LOG")
    end

    # replyWithAck description
      # UTILITY:
      #    Function will handle Finished and EOF PDU's to reply with ack.
      #    It does automatically insert into CFDP Engine's pduBuffer
      # INPUT:
      #    A CFDP::PDUPacket pdu
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if input error
      #   or if it can't build a PDUPacket
    # end function description
    def replyWithAck?(pduPacket)

      Utils_visiona.verifyInput(CFDP::PDUPacket, pduPacket.class)
      return unless isclass2?

      status = (@finishType.nil? ? 1 : 2)
      sourceID = MYID
      destinationID = DESTINATIONID
      direction = 0

      # This is a received PDU.
      case pduPacket.pduPayload
      when CFDP::PDUEOF
        
        # I only receive EOF packets in DOWNLINK events
        return unless @type.eql?("DOWNLINK")
        direction = 1
        sourceID = destinationID
        destinationID = MYID
      when CFDP::PDUFinished

        # I only receive FINISHED packets in UPLINK events
        return unless @type.eql?("UPLINK")
        # use default values from function start
      else
        return
      end

      ackPdu = CFDP.generatePDU("ACK", directiveCode:pduPacket.pduPayload.class.class_variable_get(:@@fdCode),
        directiveSubtypeCode:0, conditionCode:pduPacket.pduPayload.conditionCode, transactionStatus:status,
        direction:direction, transmissionMode:0, sourceID:sourceID, destinationID:destinationID,
        sequenceNumber:pduPacket.pduHeader.sequenceNumber)

      writeLog("Generated ACK PDU: #{ackPdu.pack.to_s}")
      CFDP::CFDPEngine.instance.insertIntoBuffer(ackPdu)
    end

    # handlePDUSent description
      # UTILITY:
      #    Function will handle PDU Sent cases.
      # INPUT:
      #    A CFDP::PDUPacket pduPacket
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if input error
    # end function description
    def handlePDUSent(pduPacket)

      Utils_visiona.verifyInput(CFDP::PDUPacket, pduPacket.class)

      writeLog("Sent #{pduPacket.pduPayload.class}: #{pduPacket.pack.to_s}")

      if isactive?

        if pduPacket.pduPayload.is_a?(CFDP::PDUACK)

          if pduPacket.pduPayload.directiveCode == 5

            writeLog("Sending FINISHED Ack, must finish transaction")
            writeLog("Transaction #{@ID} finished with <#{CFDP.conditionCodeToStr(pduPacket.pduPayload.conditionCode)}> condition")
            CFDP::CFDP_Indication("Transaction #{@ID} finished with <#{CFDP.conditionCodeToStr(pduPacket.pduPayload.conditionCode)}> condition")
            CFDP::HKPacket.instance.updateVar(pduPacket.pduPayload.conditionCode)
            finish(pduPacket.pduPayload.conditionCode == 0 ? "SUCCESS" : "FAILED")
          end
        else

          update(pduPacket)

          if (pduPacket.pduPayload.is_a?(CFDP::PDUEOF) && isclass1?)

            CFDP::CFDP_Indication("Transaction #{@ID} finished with <#{CFDP.conditionCodeToStr(pduPacket.pduPayload.conditionCode)}> condition.")
            writeLog("Transaction #{@ID} finished with <#{CFDP.conditionCodeToStr(pduPacket.pduPayload.conditionCode)}> condition.")
            CFDP::HKPacket.instance.updateVar(pduPacket.pduPayload.conditionCode)
            finish(pduPacket.pduPayload.conditionCode == 0 ? "SUCCESS" : "FAILED")
          end
        end
      end
    end

    # handlePDUReceived description
      # UTILITY:
      #    Function will handle PDU Received cases.
      # INPUT:
      #    A CFDP::PDUPacket pdu
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if input error
      #   or if it can't build a PDUPacket
    # end function description
    def handlePDUReceived(pduPacket)

      Utils_visiona.verifyInput(CFDP::PDUPacket, pduPacket.class)
      writeLog("Received #{pduPacket.pduPayload.class}: #{pduPacket.pack.to_s}")

      replyWithAck?(pduPacket)

      if @status.eql?("FINISHED")

        CFDP::CFDP_Indication("Transaction #{@ID} already finished. Ignoring.")
        return
      end

      if isclass2?

        # this is a class 2 active transaction, verify pdu class
        case pduPacket.pduPayload
        when CFDP::PDUEOF

          unless haspdueof?

            # downlink situation:
            if isdownlink?

              # Store EOF
              update(pduPacket)

              # downlink situation. (class 2 transaction active, received EOF. must check for condition codes
              # first, and take action based on them.
              case pduPacket.pduPayload.conditionCode
              when 0

                # no error condition, verify file completude
                nakPdu = verifyMissingPDU
                if nakPdu.nil?
                  finish_downlink_transaction
                else
                  CFDP::CFDPEngine.instance.insertIntoBuffer(nakPdu)
                  CFDP::CFDP_Indication("File incomplete. Generated NAKPDU and waiting for response.")
                  writeLog("File incomplete. Generated NAKPDU #{nakPdu.pack.to_s}. Waiting for response.")
                  insertTimer("NAK")
                end
              else
                # it's an error, cancel transaction and indicate the user
                CFDP::CFDP_Indication("Transaction #{@ID} cancelled due to <#{CFDP.conditionCodeToStr(pduPacket.pduPayload.conditionCode)}>.")
                writeLog("Transaction #{@ID} cancelled due to <#{CFDP.conditionCodeToStr(pduPacket.pduPayload.conditionCode)}>.")
                CFDP::HKPacket.instance.updateVar(pduPacket.pduPayload.conditionCode)
                finish("FAILED")
              end
            end
          end
        when CFDP::PDUMetadata, CFDP::PDUFileData, CFDP::PDUNAK

          update(pduPacket)
        when CFDP::PDUFinished

          # uplink situation:
          if isuplink?

            # Clean buffer
            writeLog("Cleaning buffer from transaction #{@ID}")
            CFDP::CFDPEngine.instance.clean_buffer(@ID)
          else
            # downlink situation (should not receive a FINISHED pdu, error event maybe?)
            CFDP::CFDPEngine.instance.writeLog(0, "Received FINISHED PDU in downlink event.")
          end
        when CFDP::PDUACK
          # check for directive code of PDUAck
          case pduPacket.pduPayload.directiveCode
          when 4
            # received an ACK EOF, so disable timer.
            disableTimer("EOF")
          when 5
            # received an ACK FINISHED. Disable timers
            disableTimer("FINISHED")
            finish("SUCCESS")
          else
            CFDP::CFDPEngine.instance.writeLog(0, "ACK's directive code #{pduPacket.pduPayload.directiveCode} error.")
          end
        else

          CFDP::CFDPEngine.instance.writeLog(0, "Class #{pduPacket.pduPayload.class} not implemented")
        end
      elsif isclass1?

        # this is a class 1 transaction
        case pduPacket.pduPayload
        when CFDP::PDUEOF, CFDP::PDUFileData

          update(pduPacket)

          unless pduPacket.pduPayload.is_a?(CFDP::PDUFileData)

            # received a class 1 EOF downlink. Must check condition code
            # and finish transactions
            (conditionCode = create_file) if pduPacket.pduPayload.conditionCode == 0

            # remove transaction anyway because we received EOF class 1.
            CFDP::CFDP_Indication ("Transaction #{@ID} finished with <#{CFDP.conditionCodeToStr(conditionCode)}> condition.")
            writeLog("Transaction #{@ID} finished with <#{CFDP.conditionCodeToStr(conditionCode)}> condition.")
            CFDP::HKPacket.instance.updateVar(conditionCode)
            finish(conditionCode==0 ? "SUCCESS" : "FAILED")
          end
        else

          CFDP::CFDPEngine.instance.writeLog(0, "Received #{pduPacket.pduPayload.class} on a class 1 transaction. Don't know how to handle it.")
        end
      end
    end
	end
end