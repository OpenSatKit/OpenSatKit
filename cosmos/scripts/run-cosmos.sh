#!/bin/sh
#
# Convenience script for STF-1 simulator development to run STF-1 software and simulator
#
# 2015-08-31 - Mark Suder
#

cd /vagrant/cosmos/
ruby Launcher &
#ruby tools/CmdTlmServer --config cmd_tlm_server.txt &

# This will not stay this way... a relative path should not be used
#xterm -iconic -T 'NOS Engine Standalone Server' -e 'nos_engine_server_standalone -l false -f ~/Desktop/stf1-build/bin/nos_engine_server_stf1_simulator_config.json' &

#cd ~/Desktop/stf1-build/bin
#xterm -iconic -T 'Magnetometer Simulator' -e ~/Desktop/stf1-build/bin/stf-1-magnetometer-simulator &

#cd ~/Desktop/stf1-build/bin/
#xterm -iconic -T 'GPS Simulator' -e ~/Desktop/stf1-build/bin/stf-1-gps-simulator &

#cd ~/Desktop/stf1-build/bin/
#xterm -iconic -T 'EPS Simulator' -e '~/Desktop/stf1-build/bin/eps_sim --iconic true --config ~/Desktop/stf1-build/bin/eps.json' &

#sleep 10

#cd /vagrant/CFS/build/cpu1/exe
#xterm -iconic -T 'IPM Flight Software' -e /vagrant/CFS/build/cpu1/exe/core-linux.bin &

#sleep 5

#cd ~/Desktop/stf1-build/host/
#dos2unix to-enable-tlm.sh
#bash to-enable-tlm.sh

#cd ~/Desktop/stf1-build/webct
#xterm -e python app.py & # May fail if python-tornado is not installed

#firefox localhost:8080/telemetry & # May fail if firefox is not installed
