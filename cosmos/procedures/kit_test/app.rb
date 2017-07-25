###############################################################################
# Defines a base class for representing an application being tested by the 
# starter kit's integration test.  This representation includes both ground 
# and FSW information.
#
# Notes:
#   1. Written by David McComas, licensed under the copyleft GNU
#      General Public License (GPL).
#
################################################################################

class App

  attr_reader :name              # App name common to flight/ground
  attr_reader :target, :hk_pkt   # COSMOS definitions
  attr_reader :cmd_mid
  
  def initialize (_name,_target,_hk_pkt,_cmd_mid)
    @name    = _name
    @target  = _target
    @hk_pkt  = _hk_pkt
    @cmd_mid = _cmd_mid
  end
  
end # Class App