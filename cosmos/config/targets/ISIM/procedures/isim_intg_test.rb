###############################################################################
# Verify the Instrument Simulator application is operational within a target
# system. 
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'cosmos'
require 'cosmos/script'

require_utility 'app_test'

class IsimIntgTest < AppTest

  def initialize (_app)
    super(_app)
  end
  
  def app_test
    return verify_noop
  end
  
end # Class IsimIntgTest