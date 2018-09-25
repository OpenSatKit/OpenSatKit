###############################################################################
# Kit Application Test Suite
#
# Notes:
#   1. This file manages the test suites for OSK. The following test suites are
#      defined: Kit, cFS, and Mission. A test group is defined for each app and
#      testcases verify specific app features (i.e. requirements). The kit test
#      suite tests kit apps, the cFS test suite tests cFS apps, and additonal
#      apps added by the user are managed by the mission test suite.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################


load 'kit_ci_test.rb'
load 'kit_sch_test.rb'
load 'kit_to_test.rb'

class KitTestSuite < Cosmos::TestSuite
  
   def initialize
      super()
      add_test('KitCiTest')
      add_test('KitSchTest')
      add_test('KitToTest')
   end
  
end

