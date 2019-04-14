# FUNCTIONS
CFDP_CLASS_HASH = {"CFDP::PDUMetadata"=>0, "CFDP::PDUFileData"=>1, "CFDP::PDUFinished"=>2,
	"CFDP::PDUNAK"=>3, "CFDP::PDUEOF"=>4, "CFDP::PDUACK"=>5}

def miss_pdus(*args)

	miss_sent_hash = args[0][:miss_sent_pdus]
	unless miss_sent_hash.nil?

		miss_sent_hash.each do |key, value|
			cmd("CFDP_TEST MISS_SENT_PACKET with PDU_CLASS #{CFDP_CLASS_HASH[key.to_s]}, PACKET_NUMBER_ARRAY #{value}")
		end
	end

	miss_received_hash = args[0][:miss_received_pdus]
	unless miss_received_hash.nil?

		miss_received_hash.each do |key, value|
			cmd("CFDP_TEST MISS_RECEIVED_PACKET with PDU_CLASS #{CFDP_CLASS_HASH[key.to_s]}, PACKET_NUMBER_ARRAY #{value}")
		end
	end
	wait(1)
end

# This function specify how long a file transaction should performn
def calculateWaitTime(fileSize, link)

	# fileSize should be in bytes, so...
	# Hypotetical 50% of max performance here.
	perf = 0.5
	return (fileSize/((link*perf).to_i<<7).to_f).ceil*2
end

# This monkey patch is used to check if a string contains
# multiples substrings (in any order)
class String

	def minclude?(arg)

		raise ArgumentError unless arg.is_a?(Array)

		arg.each do |arg1|
			return false unless self.include?(arg1)
		end
		return true
	end
end

# This is a specific function that verifies if TO is available.
# If not, it enables it.
def TO_enabled?(ip, destport, routemask, ifiledesc, default_timeout)

	counter = get_tlm_cnt("TO", "TO_HKPACKET")

	begin

		puts "Verifying if TO is enable"
		wait_check_expression("get_tlm_cnt(\"TO\", \"TO_HKPACKET\") > #{counter}", default_timeout)
	rescue

		puts "TO not enable. Enabling it now!"
		cmd("TO TO_ENABLE_OUTPUT with IP \"#{ip}\", DESTPORT #{destport}, ROUTEMASK #{routemask}, IFILEDESC #{ifiledesc}")
		wait_check_expression("get_tlm_cnt(\"TO\", \"TO_HKPACKET\") > #{counter}", default_timeout)
	end
end

# This is a specific function that verifies if TO is available.
# This is to be called within a test event, so it can contains
# scripting commands for COSMOS.
def TO_available?(default_timeout=5)

	puts "Verifying if TO is enable"

	# This function needs that TO_HKPACKET tlm are sent by default
	counter = get_tlm_cnt("TO", "TO_HKPACKET")
	cmd("TO TO_ENABLE_OUTPUT with IP \"#{MY_IP}\", DESTPORT 1235, ROUTEMASK 0, IFILEDESC 0")
    wait_check_expression("get_tlm_cnt('TO', 'TO_HKPACKET') > #{counter}", default_timeout)
    puts "TO is ok."
end

# This is a specific function that verifies if CF is available.
# This is to be called within a test event, so it can contains
# scripting commands for COSMOS.
def CF_available?(default_timeout)

	puts "Verifying if CF is enable"
	# To must be available in order to check CF
	TO_available?(default_timeout)

	# This functions needs that CF_HKPACKET tlm are sent by default
	counter = get_tlm_cnt("CF", "CF_HKPACKET")
    wait_check_expression("get_tlm_cnt('CF', 'CF_HKPACKET') > #{counter}", default_timeout)
	puts "CF is ok."
end

# This function return the minimal amount of PDUS that will be transfered for a given
# file. fileSize must be in kilobytes.
def PDUS?(pdu_size, fileSize)

	return ((fileSize<<10)/pdu_size).ceil
end

def appendFile(fileName, text)

	return if (text.nil? || fileName.nil?)
	File.open(fileName, 'a+') do |file|
		file.write(text)
	end
end

def downlinkAndCheck(check, classe, channel, priority, preserve, peerID, sourceFileName, destFileName, waitTime)

	downlinkTransfer(classe, channel, priority, preserve, peerID, sourceFileName, destFileName, waitTime)
	wait(1) # this wait is to check if file is gonna be written

	check_expression("#{File.exist?(destFileName)} == #{check}")
	wait(1)
end

# This function starts a file downlink transfer on satelite and
# validate it using CF and TO HK tlm packets
def downlinkTransfer(classe, channel, priority, preserve, peerID, sourceFileName, destFileName, waitTime)

	# Initialize counters
	counter = get_tlm_cnt("CF", "CF_HKPACKET")
	filesSent = tlm("CF CF_HKPACKET ENG_TOTALFILESSENT")

	# Ask for a file
	cmd("CF CF_PLAYBACK_FILE_CC with 	CLASS #{classe},
										CHANNEL #{channel},
										PRIORITY #{priority},
										PRESERVE #{preserve},
										PEERID \"#{peerID}\",
										SRCFILENAME \"#{sourceFileName}\",
										DSTFILENAME \"#{destFileName}\""
	)

	# Wait for successful file transaction
	wait_check_expression("get_tlm_cnt('CF', 'CF_HKPACKET') > #{counter} and tlm('CF CF_HKPACKET ENG_TOTALFILESSENT') > #{filesSent}", waitTime)
end

def uplinkTransfer(classe, destID, sourceFileName, destFileName, waitTime, shouldSucess = true)

	# Initialize counters
    counter = get_tlm_cnt("CF", "CF_HKPACKET")
    filesReceived = tlm("CF CF_HKPACKET APP_TOTALSUCCESSTRANS")
    totalFailedTrans = tlm("CF CF_HKPACKET APP_TOTALFAILEDTRANS")

    # Send file
    cmd("CFDP SEND_FILE with 	CLASS #{classe},
								DEST_ID '#{destID}',
								SRCFILENAME '#{sourceFileName}',
								DSTFILENAME '#{destFileName}'
		")

	# Wait for successful file transaction
	if shouldSucess
    	wait_check_expression("get_tlm_cnt('CF', 'CF_HKPACKET') > #{counter} and tlm('CF CF_HKPACKET APP_TOTALSUCCESSTRANS') > #{filesReceived}", waitTime)
    else
    	wait_check_expression("get_tlm_cnt('CF', 'CF_HKPACKET') > #{counter} and tlm('CF CF_HKPACKET APP_TOTALFAILEDTRANS') > #{totalFailedTrans}", waitTime)
    end
end

def createMainTestDir(current_test_suite, current_test, current_test_case)

	time = Time.now
	mainTestDir = Cosmos::USERPATH+"/outputs/tests/"

	Dir.mkdir(mainTestDir+current_test_suite.to_s) unless Dir.exist?(mainTestDir+current_test_suite.to_s)
	Dir.mkdir(mainTestDir+"#{current_test_suite}/#{current_test}") unless Dir.exist?(mainTestDir+"#{current_test_suite}/#{current_test}")
	Dir.mkdir(mainTestDir+"#{current_test_suite}/#{current_test}/" + current_test_case.to_s) unless Dir.exist?(mainTestDir+"#{current_test_suite}/#{current_test}/" + current_test_case.to_s)
	finalTestDir = mainTestDir+"#{current_test_suite}/#{current_test}/" + current_test_case.to_s
	finalTestDir += "/" + time.strftime("%Y%m%d_%H%M%S")
	Dir.mkdir(finalTestDir)
	Dir.mkdir(finalTestDir+"/input")
	Dir.mkdir(finalTestDir+"/output")

	return finalTestDir
end

def createRandomFile(fileName, size)

	File.open(fileName, 'wb+') do |f|
		size.to_i.times {
			f.write(SecureRandom.random_bytes((1<<10)))
		}
	end
end

def printFullHash(hash)

	return "nil" if (hash.nil? || hash.empty?)
	hashoutput = ""
	hash.each { |key, value| hashoutput << "#{key}=>#{value.to_s}, " }
	return "{#{hashoutput.chop!.chop!}}"
end