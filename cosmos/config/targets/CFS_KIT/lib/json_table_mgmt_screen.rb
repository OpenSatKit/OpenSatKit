###############################################################################
# JSON Table Management Screen
#
# Notes:
#   1. The user interaction with loads and dumps should be as close as possible
#      to the cFS interface defined in table_mgmt_screen.rb.
#   2. create_json_table_mgmt_scr() is called during system initialization and
#      creates a screen that supports all of the apps using the OSK JSON table
#      framework.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'json'

require 'cosmos'

require 'osk_global'
require 'osk_system'
require 'osk_ops'
require 'osk_flight'

################################################################################
## Screen Commands
################################################################################

def json_table_mgmt(screen, cmd)

   #
   # Menu text format: 'App-ID-Table Name'
   #   'App'         = COSMOS target name for the application
   #   'ID'          = Numeric app-specific table identifier starts at 0
   #   'Table Name'  = Table name and can have spaces
   #
   app_menu = screen.get_named_widget("app").text
   app_token = app_menu.split('-')
   
   app_name     = app_token[0]
   tbl_id       = app_token[1].to_i

   if (cmd == "LOAD")
      Osk::Ops::load_json_tbl(app_name,tbl_id)
   elsif (cmd == "DUMP")
      Osk::Ops::dump_json_tbl(app_name,tbl_id)
   elsif (cmd == "DISPLAY")
      Osk::Ops::display_json_tbl(app_name,tbl_id)
   else
      prompt("Error in screen definition file. Undefined command sent to json_table_mgmt()")
   end
   
end # json_table_mgmt()

