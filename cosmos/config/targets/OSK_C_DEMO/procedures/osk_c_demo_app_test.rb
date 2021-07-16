###############################################################################
# OSK C Demo App Test Module
#
# Notes:
#   1. Test application level functionality. A good percentage of the app 
#      functions are tested by the app object tests. This test is written 
#      last and only covers functionality not covered by the other tests. 
#      This is typically error cases. 
#
# Requirements Verified: Not Applicable
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'osk_global'
require 'osk_ops'
require 'fsw_const'
require 'fsw_config_param'
require 'osk_c_demo_const'
require 'app_func_test'

module AppFuncTest
module OSK_C_DEMO
   
   class AppTest
   
      def initialize(app)
         @app = app
      end

      def valid
         
      end
      
      def invalid

      end
   end # Class AppTest
   
end # Module OSK_C_DEMO
end # Module AppFuncTest
