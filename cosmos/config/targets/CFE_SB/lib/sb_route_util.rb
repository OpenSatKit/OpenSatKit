###############################################################################
# SB Route Utilities
#
# Notes:
#   1. Written by David McComas, licensed under the copyleft GNU
#      General Public License (GPL).
#
################################################################################

require 'cosmos'
require 'cosmos/script'


# typedef struct{
#   CFE_SB_MsgId_t      MsgId;/**< \brief Message Id portion of the route */
#   CFE_SB_PipeId_t     PipeId;/**< \brief Pipe Id portion of the route */
#   uint8               State;/**< \brief Route Enabled or Disabled */
#   uint16              MsgCnt;/**< \brief Number of msgs with this MsgId sent to this PipeId */
#   char                AppName[OS_MAX_API_NAME];/**< \brief Pipe Depth Statistics */
#   char                PipeName[OS_MAX_API_NAME];/**< \brief Pipe Depth Statistics */
# }CFE_SB_RoutingFileEntry_t;

CFE_FILE_HDR_LEN = 64
SB_ROUTE_DAT_FILE = "sb_route.dat"
SB_ROUTE_TXT_FILE = "sb_route.txt"
OS_MAX_API_NAME = 20
SB_ROUTE_REC_LEN = 46
SB_ROUTE_REC_APP_NAME_INDEX = 6
SB_ROUTE_REC_PIPE_NAME_INDEX = 26

sbRouteArray = IO.binread(SB_ROUTE_DAT_FILE)
begin
  txtFile = File.open(SB_ROUTE_TXT_FILE, "w")
  (CFE_FILE_HDR_LEN..sbRouteArray.length - 1).step(SB_ROUTE_REC_LEN).each do |index|
    routeBinRec = sbRouteArray[index..(index+SB_ROUTE_REC_LEN-1)]
    puts routeBinRec.unpack('SCCS')
    msgId    = routeBinRec.unpack('S')[0]
    puts msgId
    #puts msgId.class
    pipeId   = routeBinRec[2].unpack('C')[0]
    puts pipeId
    #puts pipeId.class
    enabled  = routeBinRec[3].unpack('C')[0]
    puts enabled
    msgCnt   = routeBinRec[4].unpack('S')
    puts msgCnt
    appName  = routeBinRec[SB_ROUTE_REC_APP_NAME_INDEX,OS_MAX_API_NAME] + "\n"
    puts appName
    pipeName = routeBinRec[SB_ROUTE_REC_PIPE_NAME_INDEX,OS_MAX_API_NAME] + "\n"
    puts pipeName
    routeStrRec = "%4x %10s %5d %15s\n" % [msgId,appName.strip!,pipeId,pipeName.strip!]
    #routeStrRec = "%4x %20s %s %20s\n" % [msgId,appName,pipeId,pipeName]
    #routeStrRec = "%20s %20s\n" % [appName.strip!, pipeName.strip!]  # strip returns nil if no change to string. Safe assumption 
    #puts = "%4x\n" % msgId
    #puts = "%2x\n" % pipeId
    #puts = "%20s\n" % appName
    #puts = "%20s\n" % pipeName
    txtFile.write(routeStrRec) 
    #puts "Index %3d: " % index + routeStrRec 
  end
rescue IOError => e
  #some error occur, dir not writable etc.
  puts "Error opening " + SB_ROUTE_TXT_FILE
  puts "Error = " + e.to_s
ensure
  txtFile.close unless txtFile.nil?
end

#http://stackoverflow.com/questions/5236059/unpack-signed-little-endian-in-ruby
#def convertLEBytesToNative( bytes )
#    if ( [1].pack('V').unpack('l').first == 1 )
#        # machine is already little endian
#        bytes.unpack('l')
#    else
#        # machine is big endian
#        convertLEToNative( Array(bytes.unpack('l')))
#    end
#end



#offset = CFE_FILE_HDR_LEN  # Skip cFE header#cnt = 0
#begin
#   puts "Count = " + cnt.to_s 
#   routeRec = IO.binread(SB_ROUTE_FILE, SB_ROUTE_FILE_REC_LEN, offset) 
#   puts "Offset: " + offset.to_s + ", Record Length = " + routeRec.length.to_s 
#   puts routeRec.unpack('SCCS')
#   puts routeRec[6,OS_MAX_API_NAME] + "\n"
#   puts routeRec[26,OS_MAX_API_NAME] + "\n"
#   offset = offset + SB_ROUTE_FILE_REC_LEN
#   cnt += 1
#end until cnt > 4
#end until routeRec.length < SB_ROUTE_FILE_REC_LEN
