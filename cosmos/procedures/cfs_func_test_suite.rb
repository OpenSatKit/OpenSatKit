###############################################################################
# cFS Application Functional Test Suite
#
# Notes:
#   1. This file manages the test suite for cFS apps. At the moment it only
#      tests the apps used by SimSat but it can be expanded to test a
#      mission independent library.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

load 'cfe_es_func_test.rb'
load 'cs_func_test.rb'

class CfsFuncTestSuite < Cosmos::TestSuite
  
   def initialize
      super()
      add_test('CfeEsFuncTest')
      add_test('CsFuncTest')
   end
  
end

