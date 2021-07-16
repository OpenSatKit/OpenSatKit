###############################################################################
# SimSat Constants
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


   MISSION_FSW_QS_FILE     = "Mission-Fsw-Quick-Start.pdf"
   MISSION_FSW_SIMSAT_FILE = "Mission-Fsw-Simple-Sat.pdf"   
   
   MISSION_FSW_APPS_ADC_FILE      = "Mission-Fsw-Apps-Adc.pdf"
   MISSION_FSW_APPS_DATAFILE_FILE = "Mission-Fsw-Apps-DataFileMgmt.pdf"
   MISSION_FSW_APPS_HS_FILE       = "Mission-Fsw-Apps-HealthSafety.pdf"
   MISSION_FSW_APPS_MAINT_FILE    = "Mission-Fsw-Apps-Maintenance.pdf"
   MISSION_FSW_APPS_RUNTIME_FILE  = "Mission-Fsw-Apps-RunTime.pdf"
   
   #####################
   ## Directory Paths ##
   #####################

   
   # Directories
   # - DS and ISIM tables are dependent on recorder path definitions
   # - GND2FSW supports "cheating" because scripts can manipulate FSW directories
   #   via ground paths
   
   SIMSAT_DIR = 'simsat'
   
   FLT_SRV_DIR = "#{Osk::FLT_SRV_DIR}/#{SIMSAT_DIR}"
   FLT_REC_DIR = "#{Osk::FLT_SRV_DIR}/#{SIMSAT_DIR}/rec"
   GND_SRV_DIR = "#{Osk::GND_SRV_DIR}/#{SIMSAT_DIR}"
   GND_REL_SRV_DIR = "#{Osk::REL_SRV_DIR}/#{SIMSAT_DIR}"

   # CFDP App has it's own dir ending in slash conventions and defaults to simsat's needs 
   CFDP_FLT_DIR = FLT_REC_DIR + "/"
   CFDP_GND_DIR = GND_REL_SRV_DIR + "/"

   # Table
   TBL_DIR      = Osk::cfg_target_dir(SIMSAT_DIR,'tables')   
   TBL_TOOL_DIR = Osk::cfg_target_dir(SIMSAT_DIR,'tables/tbl_tool')
   
   GND2FSW_REC_DIR = File.join(Osk::CFS_SIMSAT_EXE_DIR,SimSat::FLT_REC_DIR)

   # Files
   
   KIT_TO_4K_TBL  = "kit_to_4kSafe_tbl.json"   # TODO - Assumption between tbl_tool config and this definition
   KIT_TO_32K_TBL = "kit_to_32kNom_tbl.json"
   
   KIT_TO_4K_FLT_FILE  = File.join(FLT_SRV_DIR,KIT_TO_4K_TBL) 
   KIT_TO_32K_FLT_FILE = File.join(FLT_SRV_DIR,KIT_TO_32K_TBL) 
   
   KIT_TO_4K_GND_FILE  = Osk::cfg_target_dir_file(SIMSAT_DIR,'tables',KIT_TO_4K_TBL)
   KIT_TO_32K_GND_FILE = Osk::cfg_target_dir_file(SIMSAT_DIR,'tables',KIT_TO_32K_TBL)

   YOUTUBE_COMMUNITY_APPS_DATAFILE  = "https://youtu.be/Y-oxWg8rNB0"
   YOUTUBE_COMMUNITY_APPS_HS        = "https://youtu.be/Nmp9ACvATHY"
   YOUTUBE_COMMUNITY_APPS_INTRO     = "https://youtu.be/vA5d9morxZg"
   YOUTUBE_COMMUNITY_APPS_MAINT     = "https://youtu.be/pzAkAhBR-Pk"
   YOUTUBE_COMMUNITY_APPS_RUNTIME   = "https://www.youtube.com/watch?v=p3RrovUyLOs"
   
   
   # These definitions must match the Data Storage app's table ".Basename"
   # definitions
   
   EVENT_FILENAME_BASE   = "events"
   SCI_AUX_FILENAME_BASE = "sci_aux"
   
   # Must match isim app's filename base
   ISIM_FILENAME_BASE = "isim"
   
   # Temporary file used to indicate the end of the ops example
   STOP_OPS_FILE = "simsat_stop_ops.txt"
   GND2FSW_STOP_OPS_FILE = File.join(GND2FSW_REC_DIR,STOP_OPS_FILE)

   SCH_TBL_FILENAME = "simsat_sch_tbl.json"

   # Directory list file is maintained outside of the recorder
   DIR_LIST_FILE = "simsat_dir.dat"
   FLT_DIR_LIST_FILE = "#{Osk::FLT_SRV_DIR}/#{DIR_LIST_FILE}"
   GND_DIR_LIST_FILE = "#{Osk::GND_SRV_DIR}/#{DIR_LIST_FILE}"

end # Module SimSat
