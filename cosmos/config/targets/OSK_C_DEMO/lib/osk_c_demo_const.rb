###############################################################################
# OSK C Demo Constants
#
# Notes:
#   1. The demo app is used as the final goal for the OSK app development. A
#      cFS osk_c_tutor app exists for the development steps but there's no 
#      corresponding COSMOS target because osk_c_demo is used as the target.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General 
#   Public License (GPL).
# 
################################################################################

require 'osk_global'

module OskCDemo

   ##############################
   ## Compile-time definitions ##
   ##############################

   MSGLOG_TBL_ID = 0

   ###########################
   ## Init file definitions ##
   ###########################

   INI_TBL_LOAD_FILE = "osk_c_demo_ini.json"
   INI_TBL_DUMP_FILE = "osk_c_demo_ini~.json"

   #######################
   ## Table definitions ##
   #######################

   MSGLOG_TBL_LOAD_FILE = "osk_c_demo_tbl.json"
   MSGLOG_TBL_DUMP_FILE = "osk_c_demo_tbl~.json"

   LOG_FILE_PREFIX = "/cf/msg_"
   LOG_FILE_EXT    = ".txt"

   ########################
   ## Learning Resources ##
   ########################

   # Tutorial
   
   TUTOR_APP_NAME = "osk_c_tutor"
   
   TUTOR_JSON_BASE_FILE = "tutor.json"   # Baseline file that defines tutor parameters
   TUTOR_JSON_USER_FILE = "tutor~.json"  # Dynamic file that manages a user's progress through the lessons
   
   LESSON_JSON_BASE_FILE = "lesson.json"
   LESSON_JSON_USER_FILE = "lesson~.json"
   
end # module OskCDemo
