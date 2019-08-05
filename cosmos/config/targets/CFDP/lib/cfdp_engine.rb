##################################################################################################
#######                                 File Description                                 #########
##################################################################################################
#
# filename:      cfdp_engine.rb
# created:       21/05/2017
# version:       1.1.5
# company:       Visiona Tecnologia Espacial S.A.
# developer:     Nickollas Aranha
# reviewer:      Edipo Crispim
# utility:       Provide engine mechanism for ground station to accomplish
#                 correct file transfer protocol, based on CCSDS 727.0-B-4 File Delivery Protocol
#
##################################################################################################
#######                               End File Description                               #########
##################################################################################################
#
#
#
#############################################################################################
#######                                 LOG HISTORY                                 #########
#############################################################################################
#
# Everything developed before the first date is the "version 0" of the software
#
=begin LOG DESCRIPTION

    02/06/2017: Implementing ACK timer for EOF Uplink Class 2.
    02/06/2017: Implementation change for processing activeTransactions upon sending PDU, not on processing. (function managePDUSent)
    02/06/2017: Created functions to better manifest verbal operation from the other methods' designed purpose.
    02/06/2017: Implemented function to handle time on reaching timeout for any timeout on sequence. (function handleTimeout)
    05/06/2017: Finished handleTimeout function
    06/06/2017: Added description to functions for better understanding of code
    07/06/2017: Uplink EOF Timer validated.
    07/06/2017: [Corretion] Verify if transaction already has a timer before adding a new one.
    08/06/2017: Updated downlink pduHandler
    08/06/2017: Started implementation of generateNak function to process missing file data in downlink class 2 situation
    08/06/2017: Updated generatePDU function
    09/06/2017: Many improvements on verifyMissingPDU function.
    09/06/2017: Improvements on handlePDUReceived function. Looking forward upon downlink class 2.
    09/06/2017: Updated Utils_visiona.writeFile function. Enhancements done.
    12/06/2017: [Feature] Added timer for FINISHED_PDU
    12/06/2017: [Corretion] Fixed downlink class 1
    13/06/2017: [Corretion] Changed implementation for sequenceNumber to consider SourceID
    13/06/2017: Added many functions input verification
    22/06/2017: Improved modularity, refactoring done. Created new function <update_transaction>.
    23/06/2017: Function descriptions updated, added input verificators
    23/06/2017: [Feature] Added EOF condition code 4 for class 1 downlink
    23/06/2017: [Corretion] Inserted multiple timers for different conditions on same sequenceNumber (still not fully completed, must test)
    26/06/2017: [Modification] Modified logic to handle events upon sending PDU's, not right after generating them.
    26/06/2017: Done timing events.
    26/06/2017: [Feature] Started writing Log.
    27/06/2017: [Corretion] NakHandler function was missbehaving, changed logic on update_transaction function to keep original FileData PDU (not only data)
    27/06/2017: Many improvements made to Log for errors, pduReceived and events.
    27/06/2017: [Corretion] VerifyMissingPDU function correction.
    27/06/2017: [Corretion] Fixed writeFile function from utils.
    28/06/2017: [Feature] Added timer for NAK_PDU
    28/06/2017: [Corretion] Fixed verifyMissing pdu and nak handler functions for downlink
    29/06/2017: [Feature] User indication
    29/06/2017: Created function to validate inputs (better unified method).
    29/06/2017: Modified insert Timer for FINISHED UP upon generating, not on sending (that avoids sending errors).
    30/06/2017: [Feature] Class 1 downlink now verifies checksum and file size.
    03/07/2017: [Corretion] Change NAK only after receiving an EOF
    03/07/2017: [Corretion] Changed verifyMissingPDU to calculate all missing pdus, not only 1.
    04/07/2017: [Feature] Implemented clean_buffer function when calling finish_transaction
    04/07/2017: [Corretion] File verification must be done upon nak limit (so obdh will have time to send pdus)
    05/07/2017: [Corretion] Permission error upon deleting a file checksum error in downlink class 1.
    20/07/2017: [Modification] CFDP Engine introduced as background task of CTS.
    20/07/2017: [Modification] Removed fileManager tab from CTS
    20/07/2017: [Feature] Added COSMOS Send_file command
    20/07/2017: [Corretion] Small bug fixes upon generating PDU
    04/08/2017: [Feature] Transaction log files are now zipped using GZIP
    24/08/2017: [Corretion] CFDP was generating NAKPDUS bigger than MAX_PDU_SIZE. Fixed.
    29/08/2017: [Corretion] clean_buffer function should not clean ACKS pdus from buffer.
    29/08/2017: [Corretion] NAKPDU verification for NAK_LIMIT fixed.
    05/09/2017: [Corretion] Small fix upon calling ack respond (missing direction)
    05/09/2017: [Corretion] nakHandler function was not generating multiple FD PDUS upon receiving an extended request
    19/09/2017: [Modification] Vars are now in a separated file
    18/10/2017: [Modification] Added mutex
    18/10/2017: [Modification] Remade checksum
    18/10/2017: [Corretion] Now verified errors when received finished PDU
    18/10/2017: [Modification][v1.0.0] Review entire source code
    18/10/2017: [Corretion][v1.0.1] CFDP User Indication on finished uplink class 1 transaction
    19/10/2017: [Corretion][v1.0.2] Transaction now keeps a FILE pointer inside it's transaction for better I/O write @ writeLog
    23/10/2017: [Modification][v1.1.2] Transactions are now a separated class for better understanding and handling
    24/10/2017: [Modification][v1.1.3] Logfiles are now closed and zipped only after FINISH_LOG_TIMEOUT seconds (defined in vars)
    26/10/2017: [Corretion][v1.1.4] Fixed iterating over hash during handleTimeout
    26/10/2017: [Corretion][v1.1.4] Finished PDU must be send after EOF PDU (of same transaction)
    30/10/2017: [Feature][v1.1.5] HKPacket implemented.
    01/11/2017: [Modification][v1.1.6] Transaction has now a replyAck function
    01/11/2017: [Modification][v1.1.6] Engine (only) write logs upon receiving it, in a separated Thread
=end

require_relative 'cfdp_vars'
require_relative 'cfdp_transaction'
require_relative 'cfdp_engine_hkpacket'

require 'thread'
require 'singleton'
require 'cfdp/cfdp'
require 'interfaces/cosmos_cfdp_interface'

module CFDP

  class CFDPEngine

    include Singleton

    attr_reader :pduBuffer
    attr_reader :transactions
    attr_reader :sequenceNumber
    attr_reader :pduReceivedLogFile
    attr_reader :errorLogFile
    attr_reader :hkpacket

    # this function should be executed once only
    def initialize

      # This ensures the function to be executed once only
      return unless @pduBuffer.is_a?(NilClass)
      initialize_vars
    end

    def initialize_vars

      # ENGINE VARS INITIALIZER
      @pduBuffer = Array.new
      @transactions = Array.new
      @sequenceNumber = 0
      @pduBufferMutex = Mutex.new
      @transactionsMutex = Mutex.new
      @sequenceNumberMutex = Mutex.new

      @hkpacket = CFDP::HKPacket.instance
      startedTime = Time.now
      @errorLogFile = LOG_DIRECTORY+ERRORLOG+startedTime.strftime("%Y%m%d_%H%M%S")+".txt"
      @pduReceivedLogFile = LOG_DIRECTORY+PDULOG+startedTime.strftime("%Y%m%d_%H%M%S")+".txt"
    end

    # finalize description
      # UTILITY:
      #   This function will always be executed (unless computer explodes)
      #   It will close all open files (if any)
      # INPUT:
      #   No inputs
      # RETURNS:
      #   No returns.
      # EXCEPTIONS:
      #   No Exceptions.
    # end function description
    def finalize

      @transactions.each {|transaction| transaction.finalize}
    end

    # writeLog description
      # UTILITY:
      #    Function will write log events to specific files.
      # INPUT:
      #    A Integer sequenceNumber, a String text
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if wrong input.
    # end function description
    def writeLog(errorNumber, text)

      Utils_visiona.verifyInput(Integer, errorNumber.class)
      Utils_visiona.verifyInput(String, text.class)

      @hkpacket.eng_totalerrors+=1 unless errorNumber==1

      Thread.new do

        file = errorNumber==1 ? @pduReceivedLogFile : @errorLogFile
        File.open(file, "a+") {|file| file.puts "#{Time.now} #{text}"}
      end
    end

    # Useful simple and verbal function definitions here.
    def packet_is_downlink(pdu); return (pdu.pduHeader.direction==0) end
    def packet_is_uplink(pdu); return (pdu.pduHeader.direction==1) end
    def packet_is_class1(pdu); return (pdu.pduHeader.transmissionMode==1) end
    def packet_is_class2(pdu); return (pdu.pduHeader.transmissionMode==0) end
    def packet_class(pdu); return (pdu.pduHeader.transmissionMode) end


    # generateSequenceNumber description
      # UTILITY:
      #    Function will generate a sequenceNumber (4 bytes max)
      # INPUT:
      #    No inputs
      # RETURNS:
      #    Return a Integer from ground's next sequenceNumber to be used
      # EXCEPTIONS:
      #    No exceptions
    # end function description
    def generateSequenceNumber

      @sequenceNumber = ((@sequenceNumber + 1) & 0xFFFFFFFF)
    end

    # transaction_id description
      # UTILITY:
      #    Function will receive a CFDP::PDUPacket and check it's header sequence number
      #    with the @activeTransactions hash.
      # INPUT:
      #    A CFDP::PDUPacket
      # RETURNS:
      #    Return a Hash with {SourceID=>SequenceNumber} from the PDU Packet.
      # EXCEPTIONS:
      #    Can raise exception if input error
    # end function description
    def transaction_id(pdu)

      Utils_visiona.verifyInput(CFDP::PDUPacket, pdu.class)
      Utils_visiona.verifyInput(CFDP::PDUHeader, pdu.pduHeader.class)

      return {pdu.pduHeader.sourceID=>pdu.pduHeader.sequenceNumber}
    end

    # get_transaction description
      # UTILITY:
      #    Function will receive a CFDP::PDUPacket or ID Hash and return the transaction that has it,
      #    or nil if none.
      # INPUT:
      #    A CFDP::PDUPacket or Hash
      # RETURNS:
      #    Return a Transaction class or nil if none found.
      # EXCEPTIONS:
      #    Can raise exception if input error
    # end function description
    def get_transaction(arg)

      Utils_visiona.verifyInput([CFDP::PDUPacket, Hash], arg.class)

      id = arg.is_a?(Hash) ? arg : transaction_id(arg)
      @transactions.each {|transaction| return transaction if transaction.ID == id}
      return nil
    end

    # transaction_status description
      # UTILITY:
      #    Function will return status from an transaction.
      # INPUT:
      #    A Hash sequenceNumber
      # RETURNS:
      #   Returns nil if transaction doesn't exist,
      #   Returns Transaction object otherwise
      # EXCEPTIONS:
      #    Can raise exception if input error
    # end function description
    def transaction_status(id)

        Utils_visiona.verifyInput(Hash, id.class)
        return get_transaction(id)
    end

    # has_transaction? description
      # UTILITY:
      #    Function will receive a Hash ID and check if it's already
      #     a started transacion
      # INPUT:
      #    A Hash ID
      # RETURNS:
      #    Returns true if @transactions has this transaction's id
      #     returns false otherwise.
      # EXCEPTIONS:
      #    Can raise exception if input error.
    # end function description
    def has_transaction?(id)

      @transactions.each {|trans| return true if (trans.ID == id)}
      return false
    end

    # insert_transaction description
      # UTILITY:
      #    Function will receive a String and a Sequence number,
      #     create a new transaction and insert into @transactions.
      # INPUT:
      #    A String type and a Hash id
      # RETURNS:
      #    No returns
      # EXCEPTIONS:
      #    Can raise exception if type is not "UPLINK" nor "DOWNLINK"
    # end function description
    def insert_transaction(type, id, classe)

      Utils_visiona.verifyInput(String, type.class)
      Utils_visiona.verifyInput(Hash, id.class)
      Utils_visiona.verifyInput(["UPLINK", "DOWNLINK"], type.upcase)
      Utils_visiona.verifyInput(Integer, classe.class)

      # Check if transaction ID already exists.
      if has_transaction?(id)

        CFDP::CFDP_Indication("Cannot insert new transaction #{id} because it is in CFDP history.")
        return
      end

      @transactions << CFDP::Transaction.new(type.upcase, id, classe)
      @hkpacket.eng_inprogresstrans+=1
      CFDP::CFDP_Indication("#{type.upcase} Class ##{((classe+1)%2)+1} transaction #{id} has started.")
    end

    # clean_buffer description
      # UTILITY:
      #    Function will erase all pdus from the @pduBuffer of a given transactions id (except ACKS).
      # INPUT:
      #    A Hash id
      # RETURNS:
      #    No returns
      # EXCEPTIONS:
      #    Can raise exception if input error
    # end function description
    def clean_buffer(id)

      Utils_visiona.verifyInput(Hash, id.class)
      @pduBufferMutex.synchronize {
        @pduBuffer.delete_if do |packet|
          (transaction_id(packet) == id) && (!packet.pduPayload.is_a?(CFDP::PDUACK))
        end
      }
    end

    # pdu2beSent description
      # UTILITY:
      #    Function will pop the first element from @pdubuffer and return it, using thread sync.
      #    This function also call handlePDUSent, so it should be only called when sending pdus
      # INPUT:
      #    No input
      # RETURNS:
      #    Return the first element from @pduBuffer
      # EXCEPTIONS:
      #    No exceptions
    # end function description
    def pdu2beSent

      pdu = Array.new
      @pduBufferMutex.synchronize {pdu = @pduBuffer.shift}

      handlePDUSent(pdu) unless pdu.nil?
      return pdu
    end

    # insertIntoBuffer description
      # UTILITY:
      #    Function will receive a CFDP::PDUPacket and insert into @pduBuffer according to a given type.
      # INPUT:
      #    A String and a CFDP::PDUPacket
      #   String should be "append" or "prepend"
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if input error
    # end function description
    def insertIntoBuffer(pdu)

      Utils_visiona.verifyInput(CFDP::PDUPacket, pdu.class)

      @pduBufferMutex.synchronize {

        @hkpacket.updateVar("SENT", pdu.pduPayload)
        pdu.pduPayload.is_a?(CFDP::PDUACK)? @pduBuffer.unshift(pdu) : @pduBuffer << pdu
      }

      transaction = get_transaction(pdu)
      transaction.writeLog("Inserted #{pdu.pduPayload.class} #{pdu.pack} into @pduBuffer")
    end

    # cycleTimeout description
      # UTILITY:
      #    Function will iterate over all active transactions and verify if any timer has reached limit.
      #   If so, it will call handleTimeout function for that transaction.
      # INPUT:
      #    No inputs.
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    No exceptions can be risen.
    # end function description
    def cycleTimeout

      @transactionsMutex.synchronize {
        @transactions.each {|transaction| transaction.handleTimeout}
      }
    end

    # handlePDUSent description
      # UTILITY:
      #    Function will handle PDU Sent cases.
      # INPUT:
      #    A byte array
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if input error
      #   or if it can't build a PDUPacket
    # end function description
    def handlePDUSent(pduPacket)

      @transactionsMutex.synchronize {

        Utils_visiona.verifyInput(CFDP::PDUPacket, pduPacket.class)

        transaction = get_transaction(pduPacket)

        unless transaction.nil?

          transaction.handlePDUSent(pduPacket)
          return
        end
      }
    end

    # handlePDUReceived description
      # UTILITY:
      #    Function will handle PDU Received cases.
      # INPUT:
      #    a Byte array pdu
      # RETURNS:
      #    No returns.
      # EXCEPTIONS:
      #    Can raise exception if input error
      #   or if it can't build a PDUPacket
    # end function description
    #dcm def handlePDUReceived(pdu)
    def handlePDUReceived(pdu_queue)
    
      @transactionsMutex.synchronize {

        pdu = pdu_queue.pop #dcm
        
        Utils_visiona.verifyInput(Array, pdu.class)

        begin

          pduPacket = CFDP::PDUPacket.new(pdu)
          writeLog(1, "Received PDU: #{pdu}")
          @hkpacket.eng_pdusreceived+=1
          hkpacket.updateVar("RECEIVED", pduPacket.pduPayload)
        rescue

          @hkpacket.eng_pdusrejected+=1
          return
        end

        # check if transaction is active
        transaction = get_transaction(pduPacket)

        if transaction.nil?

          # transaction is not active
          if pduPacket.pduPayload.is_a?(CFDP::PDUMetadata)

            if packet_is_downlink(pduPacket)

              # this means it's a starting downlink, so i must start a new transaction
              insert_transaction("DOWNLINK", transaction_id(pduPacket), packet_class(pduPacket))
              transaction = get_transaction(pduPacket)
              transaction.update(pduPacket)

            end
          elsif packet_is_class2(pduPacket)

            case pduPacket.pduPayload
            when CFDP::PDUEOF, CFDP::PDUFileData

              # I have no metadata of a class 2 transaction, must create new transaction and update it.
              insert_transaction("DOWNLINK", transaction_id(pduPacket), packet_class(pduPacket))
              transaction = get_transaction(pduPacket)
              transaction.update(pduPacket)
            else
              writeLog(0, "Received #{pduPacket.pduPayload.class} of a non active transaction.")
            end
          else

            writeLog(0, "Received #{pduPacket.pduPayload.class} of a non active class #{pduPacket.pduHeader.transmissionMode == 0 ? "2" : "1"} transaction.")
          end
        else

          transaction.handlePDUReceived(pduPacket)
        end
      }
    end

    # uplinkRequest description
      # UTILITY:
      #    This function will start a uplink transaction.
      # INPUT:
      #    a Integer classType (1 or 2),
      #    a Integer destinationID,
      #    a String sourceFileName,
      #    a String destFileName
      # RETURNS:
      #    Returns a Hash indicating the sequenceNumber.
      # EXCEPTIONS:
      #    Can raise exception if input error
      #    Can raise exception if I/O error with %sourceFileName%
    # end function description
    def uplinkRequest(classType, destID, sourceFileName, destFileName)

      Utils_visiona.verifyInput(Integer, classType.class)
      Utils_visiona.verifyInput([1,2], classType)
      Utils_visiona.verifyInput(Integer, destID.class)
      Utils_visiona.verifyInput(String, sourceFileName.class)
      Utils_visiona.verifyInput(String, destFileName.class)

      @sequenceNumberMutex.synchronize {@transaction_id = {MYID=>generateSequenceNumber}}

      beginTimer = Time.now
      insert_transaction("uplink", @transaction_id, classType%2)
      transaction = get_transaction(@transaction_id)

      begin

        fileSize = File.size(sourceFileName)
        checksum = Utils_visiona.calculateFileChecksum(sourceFileName)
        CFDP::CFDP_Indication("Started processing file \"#{sourceFileName}\"")
        transaction.writeLog("Started processing file \"#{sourceFileName}\"")
      rescue Exception => err

        error = "Error in uplink request. Error is #{err}.\n Backtrace is #{err.backtrace}"
        CFDP::CFDP_Indication(error)
        writeLog(0, error)
      end

      # generate a PDUHeader (will use for kinda all pdus, missing 'length' and 'pduType')
      pduHeader = CFDP::PDUHeader.new(version:0, direction:0, transmissionMode:classType%2,
        crcFlag:USE_CRC, sequenceLength:3, idLength:1, sequenceNumber:@transaction_id[MYID], sourceID:MYID,
        destinationID:DESTINATIONID)

      # generate a MetadataPDU
      metadataPDU = CFDP::PDUPacket.new
      metadataPDU.pduPayload = CFDP::PDUMetadata.new(segmentationControl:1, fileSize:fileSize,
        sourceFileName:sourceFileName, destinationFileName:destFileName)

      # now update header and generate full ccsds packet
      pduHeader.pduType = 0 # for file directive
      pduHeader.pduDataLength = metadataPDU.pduPayload.length + (2*USE_CRC)
      metadataPDU.pduHeader = pduHeader.dup

      # insert into transactions
      transaction.update(metadataPDU)
      insertIntoBuffer(metadataPDU)

      # Set vars and start reading file
      currentOffset = 0
      payloadLength = MAX_PDU_SIZE-12

      begin

        File.open(sourceFileName, 'rb') do |file|

          # Read MAX_PDU_SIZE-12 chunks of data
          # Buffer can be nil if file is empty.
          until file.eof?

            buffer = file.read(payloadLength)
            next if buffer.nil?

            # mount pduFileData
            pduFileData = CFDP::PDUPacket.new
            pduFileData.pduPayload = CFDP::PDUFileData.new(offset:currentOffset, data:buffer.bytes)

            # must change pdu Type to data
            pduHeader.pduType = 1
            pduHeader.pduDataLength = pduFileData.pduPayload.length + (2*USE_CRC)
            pduFileData.pduHeader = pduHeader.dup

            transaction.update(pduFileData)
            insertIntoBuffer(pduFileData)
            currentOffset += payloadLength
          end
        end
      rescue Exception => err

        error = "Error while reading from file. Error is #{err}.\n Backtrace #{err.backtrace}"
        CFDP::CFDP_Indication(error)
        writeLog(0, error)
      end

      # now generate EOFPDU
      eofPDU = CFDP::PDUPacket.new
      eofPDU.pduPayload = CFDP::PDUEOF.new(conditionCode:0, fileChecksum:checksum, fileSize:fileSize)
      pduHeader.pduType = 0
      pduHeader.pduDataLength = eofPDU.pduPayload.length + (2*USE_CRC)
      eofPDU.pduHeader = pduHeader.dup
      insertIntoBuffer(eofPDU)
      CFDP::CFDP_Indication ("Done processing file \"#{sourceFileName}\". Time elapsed: #{Time.now-beginTimer} seconds.")
      transaction.writeLog("Done processing file \"#{sourceFileName}\". Time elapsed: #{Time.now-beginTimer} seconds.")
    end
  end
end