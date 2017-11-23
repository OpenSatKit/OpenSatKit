######################################
# Title: Integration Script
# Author: James Bible
# Date: 8/16/2016
######################################

######################################
# START
######################################
# get MIS or STREAMID from msgids ex: ci_msgids.h
# make a list of all the streamid's
# GPM global partipitation measurement (mission)
# send housekeeping request? CFE_SB_SEND_HK_MID must be in scheduler table 
# and filter table, different ID's one without send in it


#setup: enable telemetry & send command to save app info.
cmd("TO TO_ENABLE_TELEMETRY with CMD_ID 6272, SEQUENCE 49152, SIZE 19, COMMAND_CODE 2, DATA '127.0.0.1', PORT 1235")
cmd("CFE_CORE CFE_ES_QUERY_ALL with CCSDS_STREAMID 6150, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 65, CCSDS_CHECKSUM 0, CCSDS_FC 9, QUERYALLFILENAME '/cf/apps_running'")

#delay until a telemetry packet with an updated command count is received
$packetnumber = tlm("CFE_CORE CFE_ES_HKTLMPKT RECEIVED_COUNT")
wait("CFE_CORE CFE_ES_HKTLMPKT RECEIVED_COUNT > $packetnumber", 10)

#test DS APP
$validcommands = tlm("DS CFE_DS_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("DS CFE_DS_HKTLMPKT ERROR_COUNT")
cmd("DS DS_NOOP with CCSDS_STREAMID 6331, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CCSDS_CHECKSUM 0, COMMAND_CODE 0")
$packetnumber = tlm("DS CFE_DS_HKTLMPKT RECEIVED_COUNT")
wait("DS CFE_DS_HKTLMPKT RECEIVED_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("DS CFE_DS_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("DS CFE_DS_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_DS_NOOP #$command_name Command Invalid"
    else
    puts "CFE_DS_NOOP #$command_name Command did not register in command counters"
end

#test LC APP
$validcommands = tlm("LC CFE_LC_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("LC CFE_LC_HKTLMPKT ERROR_COUNT")
cmd("LC LC_NOOP with CCSDS_STREAMID 6308, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CCSDS_CHECKSUM 0, COMMAND_CODE 0")
$packetnumber = tlm("LC CFE_LC_HKTLMPKT RECEIVED_COUNT")
wait("LC CFE_LC_HKTLMPKT RECEIVED_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("LC CFE_LC_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("LC CFE_LC_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_LC_NOOP #$command_name Command Invalid"
    else
    puts "CFE_LC_NOOP #$command_name Command did not register in command counters"
end

#test FM APP
$validcommands = tlm("FM CFE_FM_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("FM CFE_FM_HKTLMPKT ERROR_COUNT")
cmd("FM FM_NOOP with CCSDS_STREAMID 6336, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CCSDS_CHECKSUM 0, COMMAND_CODE 0")
$packetnumber = tlm("FM CFE_FM_HKTLMPKT RECEIVED_COUNT")
wait("FM CFE_FM_HKTLMPKT RECEIVED_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("FM CFE_FM_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("FM CFE_FM_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_FM_NOOP #$command_name Command Invalid"
    else
    puts "CFE_FM_NOOP #$command_name Command did not register in command counters"
end

#test BM APP
$validcommands = tlm("BM CFE_BM_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("BM CFE_BM_HKTLMPKT ERROR_COUNT")
cmd("BM BM_NOOP with CCSDS_STREAMID 6336, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CCSDS_CHECKSUM 0, COMMAND_CODE 0")
$packetnumber = tlm("BM CFE_BM_HKTLMPKT RECEIVED_COUNT")
wait("BM CFE_BM_HKTLMPKT RECEIVED_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("BM CFE_BM_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("BM CFE_BM_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_BM_NOOP #$command_name Command Invalid"
    else
    puts "CFE_BM_NOOP #$command_name Command did not register in command counters"
end


#test EVS APP
$validcommands = tlm("CFE_CORE CFE_EVS_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("CFE_CORE CFE_EVS_HKTLMPKT ERROR_COUNT")
cmd('CFE_CORE CFE_EVS_NOOP with CCSDS_STREAMID 6145, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_CHECKSUM 0, CCSDS_FC 0')
$packetnumber = tlm("CFE_CORE CFE_EVS_HKTLMPKT RECEIVED_COUNT")
wait("CFE_CORE CFE_EVS_HKTLMPKT RECEIVED_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("CFE_CORE CFE_EVS_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("CFE_CORE CFE_EVS_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_EVS_NOOP #$command_name Command Invalid"
    else
    puts "CFE_EVS_NOOP #$command_name Command did not register in command counters"
end

#test SB APP
$validcommands = tlm("CFE_CORE CFE_SB_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("CFE_CORE CFE_SB_HKTLMPKT ERROR_COUNT")
cmd('CFE_CORE CFE_SB_NOOP with CCSDS_STREAMID 6147, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_CHECKSUM 0, CCSDS_FC 0')
$packetnumber = tlm("CFE_CORE CFE_SB_HKTLMPKT RECEIVED_COUNT")
wait("CFE_CORE CFE_SB_HKTLMPKT RECEIVED_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("CFE_CORE CFE_SB_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("CFE_CORE CFE_SB_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_SB_NOOP #$command_name Command Invalid"
    else
    puts "CFE_SB_NOOP #$command_name Command did not register in command counters"
end

#test ES APP
$validcommands = tlm("CFE_CORE CFE_ES_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("CFE_CORE CFE_ES_HKTLMPKT ERROR_COUNT")
cmd('CFE_CORE CFE_ES_NOOP with CCSDS_STREAMID 6150, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_CHECKSUM 0, CCSDS_FC 0')
$packetnumber = tlm("CFE_CORE CFE_ES_HKTLMPKT RECEIVED_COUNT")
wait("CFE_CORE CFE_ES_HKTLMPKT RECEIVED_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("CFE_CORE CFE_ES_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("CFE_CORE CFE_ES_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_ES_NOOP #$command_name Command Invalid"
    else
    puts "CFE_ES_NOOP #$command_name Command did not register in command counters"
end

#test TBL APP
$validcommands = tlm("CFE_CORE CFE_TBL_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("CFE_CORE CFE_TBL_HKTLMPKT ERROR_COUNT")
cmd('CFE_CORE CFE_TBL_NOOP with CCSDS_STREAMID 6148, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_CHECKSUM 0, CCSDS_FC 0')
$packetnumber = tlm("CFE_CORE CFE_TBL_HKTLMPKT RECEIVED_COUNT")
wait("CFE_CORE CFE_TBL_HKTLMPKT RECEIVED_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("CFE_CORE CFE_TBL_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("CFE_CORE CFE_TBL_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_TBL_NOOP #$command_name Command Invalid"
    else
    puts "CFE_TBL_NOOP #$command_name Command did not register in command counters"
end

#test TIME APP
$validcommands = tlm("CFE_CORE CFE_TIME_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("CFE_CORE CFE_TIME_HKTLMPKT ERROR_COUNT")
cmd('CFE_CORE CFE_TIME_NOOP with CCSDS_STREAMID 6149, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_CHECKSUM 0, CCSDS_FC 0')
$packetnumber = tlm("CFE_CORE CFE_TIME_HKTLMPKT RECEIVED_COUNT")
wait("CFE_CORE CFE_TIME_HKTLMPKT RECEIVED_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("CFE_CORE CFE_TIME_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("CFE_CORE CFE_TIME_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_TIME_NOOP #$command_name Command Invalid"
    else
    puts "CFE_TIME_NOOP #$command_name Command did not register in command counters"
end

#test SCH_LAB APP
$validcommands = tlm("SCH SCH_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("SCH SCH_HKTLMPKT ERROR_COUNT")
cmd('SCH SCH_NOOP with CCSDS_STREAMID 6293, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_CHECKSUM 0, CCSDS_FC 0')
$packetnumber = tlm("SCH SCH_HKTLMPKT RECEIVED_COUNT")
wait("SCH SCH_HKTLMPKT COMMAND_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("SCH SCH_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("SCH SCH_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_SCH_NOOP #$command_name Command Invalid"
    else
    puts "CFE_SCH_NOOP #$command_name Command did not register in command counters"
end

#test CI APP
$validcommands = tlm("CI CFE_CI_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("CI CFE_CI_HKTLMPKT ERROR_COUNT")
cmd("CI CI_NOOP with CCSDS_STREAMID 2180, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CCSDS_CHECKSUM 0, COMMAND_CODE 0")
$packetnumber = tlm("CI CFE_CI_HKTLMPKT RECEIVED_COUNT")
wait("CI CFE_CI_HKTLMPKT COMMAND_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("CI CFE_CI_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("CI CFE_CI_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_CI_NOOP #$command_name Command Invalid"
    else
    puts "CFE_CI_NOOP #$command_name Command did not register in command counters"
end

#test TO APP
$validcommands = tlm("TO CFE_TO_HKTLMPKT COMMAND_COUNT")
$errcommands = tlm("TO CFE_TO_HKTLMPKT ERROR_COUNT")
cmd("TO TO_NOOP with CCSDS_STREAMID 6272, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CCSDS_CHECKSUM 0, COMMAND_CODE 0")
$packetnumber = tlm("TO CFE_TO_HKTLMPKT RECEIVED_COUNT")
wait("TO CFE_TO_HKTLMPKT COMMAND_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
if tlm("TO CFE_TO_HKTLMPKT COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm("TO CFE_TO_HKTLMPKT ERROR_COUNT") > $errcommands
    puts "CFE_TO_NOOP #$command_name Command Invalid"
    else
    puts "CFE_TO_NOOP #$command_name Command did not register in command counters"
end

#test SAMPLE APP
#$validcommands = tlm("SAMPLE CFE_SAMPLE_HKTLMPKT COMMAND_COUNT")
#$errcommands = tlm("SAMPLE CFE_SAMPLE_HKTLMPKT ERROR_COUNT")
#cmd("SAMPLE SAMPLE_NOOP with CCSDS_STREAMID 2180, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CCSDS_CHECKSUM 0, COMMAND_CODE 0")
#$packetnumber = tlm("SAMPLE CFE_SAMPLE_HKTLMPKT RECEIVED_COUNT")
#wait("SAMPLE CFE_TO_HKTLMPKT COMMAND_COUNT > $packetnumber", 10)  #delay until a packet with an updated packet count is received
#if tlm("SAMPLE CFE_SAMPLE_HKTLMPKT COMMAND_COUNT") > $validcommands
#    puts "#$app_name #$command_name Command Valid"
#    elsif tlm("SAMPLE CFE_SAMPLE_HKTLMPKT ERROR_COUNT") > $errcommands
#    puts "CFE_SAMPLE_NOOP #$command_name Command Invalid"
#    else
#    puts "CFE_SAMPLE_NOOP #$command_name Command did not register in command counters"
#end





#load app info & store it into an array
filename='../cfs//build/exe/cpu1/cf/apps_running'
file = File.open(filename, "rb")
contents = file.read

#set command to be tested
command_name = "_NOOP"

# start at the first app name index
$i = 0
# init to the first index into array to get the app name
$offset = 72 #72 #1576

while contents.length > $offset  do
  $app_name = contents[$offset..$offset+12].strip

  #setup individual app params
  $target = $app_name
  $streamid = "6336"
    
    
  #should handle everything besides core and scheduler
  if !($app_name.eql?("TO") || $app_name.eql?("CI") || $app_name.eql?("CFE_TBL") || $app_name.eql?("CFE_TIME") || $app_name.eql?("CFE_ES")|| $app_name.eql?("CFE_SB") || $app_name.eql?("CFE_EVS") || $app_name.eql?("SCH_LAB_APP"))
    if $app_name.eql?("SAMPLE_APP")
      $target = "SAMPLE"
      $streamid = "2180"
  end
        
  $validcommands = tlm($target + " CFE_" + $app_name + "_HKTLMPKT" + " COMMAND_COUNT")
  $errcommands = tlm($target + " CFE_" + $app_name + "_HKTLMPKT" + " ERROR_COUNT")
  cmd($target + " " + $app_name + command_name + " with CCSDS_STREAMID " + $streamid +", CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CCSDS_CHECKSUM 0, COMMAND_CODE 0")
  #delay until a packet with an updated command count is received
  $packetnumber = tlm($target + " CFE_" + $app_name + "_HKTLMPKT" + " RECEIVED_COUNT")
  wait($target + " CFE_" + $app_name + "_HKTLMPKT" + " RECEIVED_COUNT > $packetnumber", 10)
  if tlm($target + " CFE_" + $app_name + "_HKTLMPKT" + " COMMAND_COUNT") > $validcommands
    puts "#$app_name #$command_name Command Valid"
  elsif tlm($target + " CFE_" + $app_name + "_HKTLMPKT" + " ERROR_COUNT") > $errcommands
    puts "#$app_name #$command_name Command Invalid"
  else
    puts "************ #$app_name #$command_name Command did not register in command counters ************"
    puts $target + " CFE_" + $app_name + "_HKTLMPKT" + " COMMAND_COUNT"
    puts "valid commands = #$validcommands"
    $validcommands = tlm($target + " CFE_" + $app_name + "_HKTLMPKT" + " COMMAND_COUNT")
    puts "valid commands = #$validcommands"
    puts "invalid commands = #$errcommands"
  end
end

# index in to the next app name
$i +=1
$offset += 188 # offset for the next app name
    
end

#NOTE: CI & SAMPLE_APP, telemetry packets received, does not increment their command counters when sending commands
#NOTE: SCH times out, telemetry not being sent


puts("Apps found running = #$i" )






