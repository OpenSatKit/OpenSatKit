###############################################################################
# Test TFTP from script runner
#
# Author: David McComas
#
#
################################################################################


puts "hello.dat".split('.').last
puts "hello.dat".split('.').first

#require 'tftp'
#  
#FSW_TMP_DAT_FILE = '/cf/tmp.dat'
#GND_TMP_DAT_FILE = '/tftpboot/tmp.dat'
#
#tftp = TFTP.new('127.0.0.1')
#tftp.putbinaryfile(GND_TMP_DAT_FILE, FSW_TMP_DAT_FILE)