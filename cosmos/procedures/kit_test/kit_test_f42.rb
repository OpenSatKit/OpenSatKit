###############################################################################
# Verify the 42 Simulator FSW Controller (F42) application is operational 
# within the starter kit.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require_utility 'kit_test_app'

class KitTestF42 < KitTestApp

  def initialize (_app)
    super(_app)
  end
  
  def app_test
    return verify_noop
  end
  
end # Class KitTestF42