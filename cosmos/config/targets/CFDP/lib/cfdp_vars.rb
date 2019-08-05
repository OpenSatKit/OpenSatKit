# CFDP TASK VARS
PDU_TARGET_NAME = "PDU"
PDU_SEND_TARGET_PACKET = "CF_GND_TO_SPACE_PDU"
PDU_RECV_TARGET_PACKET = "CF_SPACE_TO_GND_PDU"

# CFDP ENGINE VARS
# How many seconds before receiving a response
EOF_TIMEOUT = 2
INACTIVITY_TIMEOUT = 50
FINISHED_TIMEOUT = 2
NAK_TIMEOUT = 2
FINISH_LOG_TIMEOUT = 120 # seconds before closing the logFile

# How many times before performing an error
EOF_LIMIT = 10
INACTIVITY_LIMIT = 0
FINISHED_LIMIT = 10
NAK_LIMIT = 10
FINISH_LOG_LIMIT = 0

# CONSTRAINTS
MYID = 21
DESTINATIONID = 24
USE_CRC = 0						# (0 - false, 1 - true). CRC is not supported @ CFS CF
MAX_PDU_SIZE = 500				# Bytes
MAX_STRING_SIZE = 64			# Used for destination and source file names
SAVE_FILE_UPON_ERROR = true 	# Choose whether to save file or not if error (Class 1 only)

# FILE VARS
LOG_DIRECTORY = "#{Cosmos::USERPATH}/outputs/logs/"
PDULOG = "CFDP_PDU_Received"
ERRORLOG = "CFDP_Error_Log"

# ENGINE TASK VARS
SLEEP_TIME_BTW_PDUS = 0.01 	# time sleeping between sending pdus

# OTHERS
DEBUG = 1