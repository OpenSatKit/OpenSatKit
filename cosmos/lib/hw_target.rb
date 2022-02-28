###############################################################################
# Hardware Target
#
# Notes:
#   1. Currently written to support a single target
# 
# License
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require "erb"

module HwTarget

  PROCESSOR_ENDIAN = "LITTLE_ENDIAN"
   
  CPU_ADDR_SIZE = 4
  
  #
  # Only one of the following IDs can be uncommented
  #   LOCAL_*      - Target running on same machine as COSMOS and * = cFE Framework name: AQUILA (6.7.*) or Bootes (6.8.*). Caelum (7.0.*) is not supported yet
  #   PISAT_CAELUM - Using a Rasberry Pi configured according to OSK's Pi-Sat Quick Start Guide using cFE Caelum
  #   PISAT_DEMO   - Special PiSat demo configuration supplied by Alan Cudmore
  #
  # COSMOS must be restarted for a change to ID to take effect. Prior to restarting COSMOS  delete the 'marshall*' cache files in
  # cosmos/oututs/tmp to ensure the new ID value is applied. 
  #

  ID = "LOCAL_AQUILA"  
  #ID = "LOCAL_BOOTES"  
  #ID = "PISAT_CAELUM" 
  #ID = "PISAT_DEMO" 

end # module HwTarget

