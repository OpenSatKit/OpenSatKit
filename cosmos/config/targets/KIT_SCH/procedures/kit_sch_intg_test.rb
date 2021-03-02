###############################################################################
# Verify the cFE table Service application is fully integrated with OSK.
#
# Notes:
#   1. See app_intg_test.rb's prologue for a description of the purpose and
#      scope of an integration test.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'

require 'app_intg_test'

class KitSchIntgTest < AppIntgTest

  def initialize (app)
    super(app)
  end
  
  def app_test
    return verify_noop
  end
  
end # Class KitSchIntgTest