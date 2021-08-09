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
  # Only one of the following IDs should be uncommented
  #   LOCAL      - Target running on same machine as COSMOS
  #   PISAT      - Using a Rasberry Pi configured according to OSK's Pi-Sat Quick Start Guide
  #   PISAT_DEMO - Special PiSat demo configuration supplied by Alan Cudmore
  #
  
  ID = "LOCAL"  
  #ID = "PISAT" 
  #ID = "PISAT_DEMO" 

end # module HwTarget

