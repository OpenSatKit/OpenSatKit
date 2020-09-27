###############################################################################
# SimSat Global
#
# Notes:
#   1. Osk::FLT_SRV_DIR is used flight-ground communication and the SimSat
#      directory is used for SimSat simulation generated files 
# 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'cosmos'
require 'osk_global'

module SimSat

   # Directories
   
   FLT_SRV_DIR = "#{Osk::FLT_SRV_DIR}/simsat"
   GND_SRV_DIR = "#{Osk::GND_SRV_DIR}/simsat"

   # These definitions must match the Data Storage app's table ".Basename"
   # definitions
   
   EVENT_FILENAME_BASE   = "events"
   SCI_AUX_FILENAME_BASE = "sci_aux"
   
   # Must match isim app's filename base
   ISIM_FILENAME_BASE = "isim"
   
   # Temporary file used to indicate the end of the ops example
   STOP_OPS_FILE = "simsat_stop_ops.txt"

   SCH_TBL_FILENAME = "simsat_sch_tbl.json"

   # Directory list file is maintained outside of the recorder
   DIR_LIST_FILE = "simsat_dir.dat"
   FLT_DIR_LIST_FILE = "#{Osk::FLT_SRV_DIR}/#{DIR_LIST_FILE}"
   GND_DIR_LIST_FILE = "#{Osk::GND_SRV_DIR}/#{DIR_LIST_FILE}"

end # Module SimSat
