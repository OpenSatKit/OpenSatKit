###############################################################################
# Mission Application Functional Test Suite
#
# Notes:
#   1. This file manages the test suites for mission apps specifically
#      written for SimSat.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

load 'f42_func_test.rb'
load 'i42_func_test.rb'
load 'isim_func_test.rb'

class MissionFuncTestSuite < Cosmos::TestSuite
  
   def initialize
      super()
      add_test('F42FuncTest')
      add_test('I42FuncTest')
      add_test('IsimFuncTest')
   end
  
end

