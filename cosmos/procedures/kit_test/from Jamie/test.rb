$app_name = "TST1"
command_name = "_NOOP"

#setup individual app params
$target = $app_name
$packetname = "_HKTLMPKT"
$vcountername = "CMDCOUNTER"
$errcountername = "ERRCOUNTER"
$streamid = "6336"


$vcountername = "COMMAND_COUNT"
$errcountername = "ERROR_COUNT"
$validcommands = tlm($target + " CFE_" + $app_name + $packetname + " " + $vcountername)
$errcommands = tlm($target + " CFE_" + $app_name + $packetname + " " + $errcountername)
cmd($target + " " + $app_name + command_name + " with CCSDS_STREAMID " + $streamid +", CCSDS_SEQUENCE 49152, CCSDS_LENGTH 1, CCSDS_RESERVED 0, CCSDS_CHECKSUM 0, COMMAND_CODE 0")
#delay until a packet with an updated command count is received
$packetnumber = tlm($target + " CFE_" + $app_name + $packetname + " RECEIVED_COUNT")
wait($target + " CFE_" + $app_name + $packetname + " RECEIVED_COUNT > $packetnumber", 10)
if tlm($target + " CFE_" + $app_name + $packetname + " " + $vcountername) > $validcommands
    puts "#$app_name #$command_name Command Valid"
    elsif tlm($target + " CFE_" + $app_name + $packetname + " " + $errcountername) > $errcommands
    puts "#$app_name #$command_name Command Invalid"
    else
    puts "************ #$app_name #$command_name Command did not register in command counters ************"
    puts $target + " CFE_" + $app_name + $packetname + " " + $vcountername
    puts "valid commands = #$validcommands"
    $validcommands = tlm($target + " CFE_" + $app_name + $packetname + " " + $vcountername)
    puts "valid commands = #$validcommands"
    puts "invalid commands = #$errcommands"
end

