###############################################################################
# CFE_ES Application Functional Test
#
# Notes:
#   1. Loaded by cFS test suite so it can assume common utilities have been 
#      preloaded.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'app_func_test'

class CfeEsFuncTest < Cosmos::Test

   include AppFuncTest
   
   def initialize
      super()
      @app = app_func_test_init("CFE_ES")
   end

   def setup
      status_bar("#{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case} - setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      puts "@app.version = #{@app.version}\n"
   end

   def teardown
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
   end

   def helper_method
   end

end # Class CfeEsFuncTest

