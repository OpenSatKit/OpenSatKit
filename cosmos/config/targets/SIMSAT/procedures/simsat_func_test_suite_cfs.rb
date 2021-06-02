###############################################################################
# cFS Application Functional Test Suite
#
# Notes:
#   1. This test suite manages the execution of non-OSK open source cFS
#      community apps. At a minimum this includes the cFE service apps and
#      the NASA cFS apps used by SimSat. 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

load 'cfe_es_func_test.rb'
load 'cs_func_test.rb'
load 'fm_func_test.rb'

class SimSatFuncTestSuiteCfs < Cosmos::TestSuite
  
   def initialize
      super()
      
      # cFE Services
      add_test('CfeEsFuncTest')
      ##add_test('CfeEvsFuncTest')
      ##add_test('CfeSbFuncTest')
      ##add_test('CfeTblFuncTest')
      ##add_test('CfeTimeFuncTest')
      
      # cFS Community Apps
      add_test('CsFuncTest')
      add_test('FmFuncTest')
      ##add_test('DsFuncTest')
      ##add_test('HkFuncTest')
      ##add_test('HsFuncTest')
      ##add_test('LcFuncTest')
      ##add_test('MdFuncTest')
      ##add_test('MmFuncTest')
      ##add_test('ScFuncTest')
      
   end
  
end

