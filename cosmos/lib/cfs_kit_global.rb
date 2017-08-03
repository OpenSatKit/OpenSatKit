###############################################################################
# cFS Kit Global Definitions 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
# 
################################################################################

######################
## Network Settings ##
######################

DEF_IP_ADDR = "127.0.0.1"

###################
## Text Messages ##
###################

BUTTON_CONT = "OK"
BUTTON_CANCEL = "Cancel"  # Equal to hardcoded message_box() string
UNDEFSTR = "Undefined"

MSG_TBD_FEATURE = "Coming Soon"

#####################
## Directory Paths ##
#####################

# Directory paths relative to Cosmos::USERPATH
REL_DIR_CFS = "../cfs"
REL_DIR_42  = "../42"

GND_SRV_DIR     = "#{Cosmos::USERPATH}/cfs-kit/file_server"
GND_SRV_TBL_DIR = "#{Cosmos::USERPATH}/cfs-kit/file_server/tables"

FLT_SRV_DIR = "/cf"

CFS_KIT_LIB_DIR   = "#{Cosmos::USERPATH}/config/targets/CFS_KIT/lib"
CFS_KIT_DOCS_DIR  = "#{Cosmos::USERPATH}/cfs-kit/docs"
CFS_KIT_TOOLS_DIR = "#{Cosmos::USERPATH}/cfs-kit/tools"
CFS_KIT_PERF_MON_DIR = "#{CFS_KIT_TOOLS_DIR}/perf-monitor"
CFS_KIT_GEN_APP_DIR  = "#{CFS_KIT_TOOLS_DIR}/create-app"

