###############################################################################
# OSK Application Functional Test Suite
#
# Notes:
#   1. This file manages the test suite for OSK apps. At the moment it only
#      tests the apps used by SimSat but it can be expanded to test a
#      mission independent library.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

load 'kit_ci_func_test.rb'
load 'kit_sch_func_test.rb'
load 'kit_to_func_test.rb'
load 'tftp_func_test.rb'

class OskFuncTestSuite < Cosmos::TestSuite
  
   def initialize
      super()
      add_test('KitCiFuncTest')
      add_test('KitSchFuncTest')
      add_test('KitToFuncTest')
      add_test('TftpFuncTest')
   end
  
end

