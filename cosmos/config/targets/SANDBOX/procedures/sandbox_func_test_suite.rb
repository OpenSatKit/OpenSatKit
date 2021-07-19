###############################################################################
# Sandbox Functional Test Suite
#
# Notes:
#   1. Add and remove app tests as needed. The sandbox is not intended to 
#      be a fixed configuration
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

load 'filemgr_func_test.rb'
load 'osk_c_demo_func_test.rb'

class SandboxFuncTestSuite < Cosmos::TestSuite
  
   def initialize
      super()
      add_test('FileMgrFuncTest')
      add_test('OskCDemoFuncTest')
   end
  
end

