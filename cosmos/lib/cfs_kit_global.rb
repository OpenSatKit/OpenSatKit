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

################
## File Names ##
################

TUTORIAL_DEF_FILE = "osk_tutorials.json"
TUTORIAL_SCR_FILE = "tutorial_screen.txt"

OSK_CFE_UG_FILE   = "index.html"

OSK_TMP_BIN_FILE  = "osk_tmp_bin.dat"
OSK_TMP_TBL_FILE  = "osk_tmp_tbl.dat"
OSK_TMP_JSON_FILE = "osk_tmp.json"

#####################
## Directory Paths ##
#####################

# Directory paths relative to Cosmos::USERPATH
REL_DIR_CFS = "../cfs"
REL_DIR_42  = "../42"

GND_BLD_CF_DIR  = File.expand_path(REL_DIR_CFS,Cosmos::USERPATH) + "/build/exe/cpu1/cf"

GND_SRV_DIR     = "#{Cosmos::USERPATH}/cfs_kit/file_server"
GND_SRV_TBL_DIR = "#{Cosmos::USERPATH}/cfs_kit/file_server/tables"

FLT_SRV_DIR = "/cf"

CFS_KIT_LIB_DIR = "#{Cosmos::USERPATH}/config/targets/CFS_KIT/lib"
CFS_KIT_SCR_DIR = "#{Cosmos::USERPATH}/config/targets/CFS_KIT/screens"

CFS_KIT_DOCS_DIR      = "#{Cosmos::USERPATH}/cfs_kit/docs"
CFS_KIT_CFE_UG_DIR    = "#{CFS_KIT_DOCS_DIR}/cfe_users_guide"
CFS_KIT_TUTORIAL_DIR  = "#{Cosmos::USERPATH}/cfs_kit/tutorials"

CFS_KIT_TOOLS_DIR      = "#{Cosmos::USERPATH}/cfs_kit/tools"
CFS_KIT_PERF_MON_DIR   = "#{CFS_KIT_TOOLS_DIR}/perf-monitor"
CFS_KIT_CREATE_APP_DIR = "#{CFS_KIT_TOOLS_DIR}/create-app"
