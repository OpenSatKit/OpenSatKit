###############################################################################
# Verify the cFE Table Service application is operational within the starter 
# kit.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require_utility 'kit_test_app'

class KitTestCfeTbl < KitTestApp

  def initialize (_app)
    super(_app)
  end
  
  def app_test
    return verify_noop
  end
  
end # Class KitTestCfeTbl