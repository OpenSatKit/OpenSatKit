###############################################################################
# cFS Education Target
#
# Notes:
#   1. All constants prefixed with target name to avoid name clashes
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_global'

module CfSat

   QUICK_START_FILE           = "cFS-Edu-Quick-Start.pdf"

   #
   # cFS
   #
   CFS_OVERVIEW_FILE = "cFS-Edu-cFS-Overview.pdf"
   CFE_OVERVIEW_FILE    = "cFS-Edu-cFE-Overview.pdf"
   
   CFS_OVERVIEW_YOUTUBE       = "https://www.youtube.com/watch?v=gSsV6fDEcAc"
   CFS_BUILD_OVERVIEW_YOUTUBE = "https://youtu.be/a6ykI6qLtSY"

   #
   # Application Development
   #
   CREATE_APP_HELLO_WORLD_YOUTUBE   = "https://www.youtube.com/watch?v=LspTUdIKAZY&t=1s"
   INSPECT_APP_HELLO_WORLD_YOUTUBE  = "https://www.youtube.com/watch?v=0BSMXeq8B5c&t=4s"

   #
   # Application Tutorial
   #

   APP_TUTOR_GUIDE_FILE    = "osk_c_tutor_guide.pdf"
   APP_TUTOR_INTRO_YOUTUBE = "TBD"

   ########################
   ## Learning Resources ##
   ########################

   # Tutorial

   TUTOR_APP_NAME   = "osk_c_tutor"
   TUTOR_LESSON_CNT = 6
   
   TUTOR_JSON_BASE_FILE = "tutor.json"   # Baseline file that defines tutor parameters
   TUTOR_JSON_USER_FILE = "tutor~.json"  # Dynamic file that manages a user's progress through the lessons

   LESSON_JSON_BASE_FILE = "lesson.json"
   LESSON_JSON_USER_FILE = "lesson~.json"

   TUTOR_APP_DIR     = Osk::fsw_app_dir(TUTOR_APP_NAME)
   TUTOR_DOC_DIR     = File.join(Osk::cfg_target_dir("CFSAT", "docs"),TUTOR_APP_NAME)
   TUTOR_LESSON_DIR  = File.join(TUTOR_APP_DIR,"lesson")
   TUTOR_FSW_SRC_DIR = File.join(TUTOR_APP_DIR,'fsw','src')
   
   TUTOR_JSON_BASE_PATH_FILE = File.join(TUTOR_APP_DIR,TUTOR_JSON_BASE_FILE)
   TUTOR_JSON_USER_PATH_FILE = File.join(TUTOR_APP_DIR,TUTOR_JSON_USER_FILE)

end # CfSat