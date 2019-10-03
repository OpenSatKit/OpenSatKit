###############################################################################
# OpenSatKit Test
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

# Load common utilities first
load 'fsw_app.rb'
load 'cosmos/tools/test_runner/test.rb'

load 'kit_func_test_suite.rb'
#load 'cfs_test_suite.rb'
#load 'usr_test_suite.rb'


# This Test demonstrates the usage of the setup and teardown methods
# as well as defining two tests. Notice that the setup and teardown
# methods must be called exactly that. Other test methods must start
# with 'test_' to be picked up by TestRunner.
class ExampleTest1 < Cosmos::Test
  def initialize
    super()
  end

  # Setup the test case by doing stuff
  def setup
    status_bar("setup")
    puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
    wait(2)
  end

  # test_1 verifies requirement 1
  def test_case_with_long_name_1
    status_bar("test_case_with_long_name_1")
    puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
    Cosmos::Test.puts "This test verifies requirement 1"
    wait(2)
  end

  # test_2 verifies requirement 2
  def test_2
    status_bar("test_2")
    puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
    Cosmos::Test.puts "This test verifies requirement 2"
    if $manual
      answer = ask "Are you sure?"
    else
      answer = 'y'
    end
    wait(2)
  end

  def test_3xx
    status_bar("test_3xx")
    puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
    wait 1
  end

  # Teardown the test case by doing other stuff
  def teardown
    status_bar("teardown")
    puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
    wait(2)
  end

  def helper_method

  end
end


# ExampleTest2 runs test_3 and test_4.
class ExampleTest2 < Cosmos::Test
  def setup
    puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
    wait(2)
  end

  # ExampleTest2::test_2 is different from ExampleTest::test_2
  def test_2
    puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
    raise "BAD"
    puts "continuing past the exception"
    wait 2
  end

  def test_3
    puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
    raise SkipTestCase, "test_3 unimplemented"
    wait(2)
  end

  def test_4
    puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
    if non_existent
      puts "can't get here"
    else
      puts "can't get here either"
    end
    wait(2)
  end

  def teardown
    puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
    wait(2)
  end
end

class ExampleTestSuite < Cosmos::TestSuite
  def initialize
    super()
    add_test('ExampleTest1')
    add_test('ExampleTest2')
  end
end

