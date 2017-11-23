COSMOS Ground system target for PiControl framework

Copy the PICONTROL directory into your cosmos configuration. For example:
cosmosdemo/config/targets/PICONTROL

It provides a simple command path to command subsystems in the framework, and a couple of simple/example telemetry packets. 

See PICONTROL/cmd_tlm_targets.txt for the IP address of the Pi that you are commanding. Also the UDP ports for commands and telemetry are defined in this file. 

It currently uses Port 8080 for UDP commands to the Pi and 8081 for UDP telemetry coming back from the Pi. 



