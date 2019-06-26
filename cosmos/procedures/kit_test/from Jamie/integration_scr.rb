######################################
# Title: Integration Script
# Author: James Bible
# Date: 8/16/2016
######################################

######################################
# START
######################################

#setup: enable telemetry & send command to save app info.
cmd("TO TO_ENABLE_TELEMETRY with CMD_ID 6272, SEQUENCE 49152, SIZE 19, COMMAND_CODE 2, DATA '127.0.0.1', PORT 1235")
cmd("CFE_CORE CFE_ES_QUERY_ALL with CCSDS_STREAMID 6150, CCSDS_SEQUENCE 49152, CCSDS_LENGTH 65, CCSDS_CHECKSUM 0, CCSDS_FC 9, QUERYALLFILENAME '/cf/apps_running'")

#delay until a telemetry packet with an updated command count is received
$packetnumber = tlm("CFE_CORE CFE_ES_HKTLMPKT RECEIVED_COUNT")
wait("CFE_CORE CFE_ES_HKTLMPKT RECEIVED_COUNT > $packetnumber", 10)

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
    
    
    if $app_name.eql?("CFE_TBL") || $app_name.eql?("CFE_TIME") || $app_name.eql?("CFE_ES")|| $app_name.eql?("CFE_SB") || $app_name.eql?("CFE_EVS") || $app_name.eql?("SCH_LAB_APP")
        $target = "CFE_CORE"
        if $app_name.eql?("CFE_ES")
            $streamid = "6150"
            elsif $app_name.eql?("CFE_EVS")
            $streamid = "6145"
            elsif $app_name.eql?("CFE_SB")
            $streamid = "6147"
            elsif $app_name.eql?("CFE_TBL")
            $streamid = "6148"
            elsif $app_name.eql?("CFE_TIME")
            $streamid = "6149"
            elsif $app_name.eql?("SCH_LAB_APP")
            $target = "SCH"
            $app_name = "SCH"
            $streamid = "6293"
        end
        $validcommands = tlm($target + " " + $app_name + "_HKTLMPKT" + " COMMAND_COUNT")
        $errcommands = tlm($target + " " + $app_name + "_HKTLMPKT" + " ERROR_COUNT")
        cmd($target + " " + $app_name + command_name + " with CCSDS_STREAMID " + $streamid + ", CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_CHECKSUM 0, CCSDS_FC 0")
        #delay until a packet with an updated command count is received
        $packetnumber = tlm($target + " " + $app_name + "_HKTLMPKT" + " RECEIVED_COUNT")
        wait($target + " " + $app_name + "_HKTLMPKT" + " RECEIVED_COUNT > $packetnumber", 10)
        if tlm($target + " " + $app_name + "_HKTLMPKT" + " COMMAND_COUNT") > $validcommands
            puts "#$app_name #$command_name Command Valid"
            elsif tlm($target + " " + $app_name + "_HKTLMPKT" + " ERROR_COUNT") > $errcommands
            puts "#$app_name #$command_name Command Invalid"
            else
            puts "#$app_name #$command_name Command did not register in command counters"
        end
        
        #else condition should handle everything besides core and scheduler
        #NOTE: CI & SAMPLE_APP, telemetry packets received, does not increment their command counters when sending commands
        #NOTE: SCH times out, telemetry not being sent
        else
        if $app_name.eql?("CI")
            $streamid = "2180"
            elsif $app_name.eql?("SAMPLE_APP")
            $target = "SAMPLE"
            $streamid = "2180"
            elsif $app_name.eql?("TO")
            $streamid = "6272"
            elsif $app_name.eql?("CI")
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

puts("Apps found running = #$i" )






