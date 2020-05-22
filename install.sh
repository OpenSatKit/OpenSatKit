#!/usr/bin/env bash

OPEN_SAT_KIT_VERSION="2.2.0"
BASE_DIR="$(pwd)"

# Intentional order: OSK Archive, Dependencies, COSMOS, Build System 
# - Dependencies cover OSK's needs, COSMOS may need additional ones
# - OSK last ensures OSK COSMOS config files not overrriden

main()
{

announce "INSTALLATION WILL TAKE TIME AND INCLUDES PROMPTS!"

# set default path unless environment var exists
if [ ! -n "${!OPEN_SAT_KIT_PATH}" ]; then
   OPEN_SAT_KIT_PATH="`pwd`/OpenSatKit-master/"
fi

announce "Installing OpenSatKit v$OPEN_SAT_KIT_VERSION in \"$OPEN_SAT_KIT_PATH\""
confirm "Continue?" 1

announce "Installing OSK archive zip file from \"https://github.com/OpenSatKit/OpenSatKit/archive/master.tar.gz\""
if [ -d $BASE_DIR/OpenSatKit-master ]; then
   read -p "OpenSatKit-master directory already exists. Delete for clean install? (Yn): " -n 1 -r
   echo
   if [[ $REPLY =~ ^[Yy]?$ ]]; then
      rm -rf $BASE_DIR/OpenSatKit-master
   fi
fi

if [ ! -d $BASE_DIR/OpenSatKit-master ]; then
   wget -c https://github.com/OpenSatKit/OpenSatKit/archive/master.tar.gz
   tar -xzf master.tar.gz
   rm master.tar.gz
else
   echo "Preserving existing OpenSatKit-master. Continuing installation..."
fi

announce "Installing COSMOS (Includes Ruby). When prompted select Sudo and demo is optional (not needed for OSK)."
confirm "Continue?" 1

bash <(\curl -sSL https://raw.githubusercontent.com/BallAerospace/COSMOS/master/vendor/installers/linux_mac/INSTALL_COSMOS.sh)

# Install depedencies above and beyond COSMOS
announce "Installing dependencies"
cmdtoapt="apt-get update -y; apt-get install -y curl; apt-get install -y default-jre; apt-get install -y gcc-multilib; apt-get install -y g++-multilib; apt-get install -y xfce4-terminal; apt-get install -y libqt4-dev; apt-get install libcanberra-gtk*; apt install dos2unix; apt-get update -y;"
sudo bash -c "$cmdtoapt"

cd OpenSatKit-master

announce "Building CFS"
cd ./cfs/
make distclean
make prep
make
make install
cd ..

announce "Building 42"
cd ./42/
make
cd ..

# Complete COSMOS installation
cd ./cosmos/
if [ -f Gemfile.lock ]; then
   rm Gemfile.lock
fi
bundle install
cd ..

# Fix shebang line \r warning by converting from Dos -> Unix
git config --global core.autocrlf input
find ./ -type f -exec dos2unix {} \;

# Starting with Ubuntu 18.04 this is required to make the ruby environment saved 
announce "Close this terminal. Open new terminal: cd OpenSatKit-master/cosmos/  and run 'ruby Launcher' "

return 1
}

# ask the user to confirm an action
confirm() # prompt, default (0: N, 1: Y)
{
   if [ $2 -eq 0 ]; then
      read -p "$1 (y,N)" -n 1 -r
      echo
      if [[ ! $REPLY =~ ^[Yy]$ ]]
      then
         [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1 # handle exits from shell or function but don't exit interactive shell
      fi
   else
      read -p "$1 (Y,n)" -n 1 -r
      echo
      if [[ $REPLY =~ ^[Nn]$ ]]
      then
         [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1 # handle exits from shell or function but don't exit interactive shell
      fi
   fi
}

# make output stand out
announce() # message
{
   echo
   printf '%*s\n' "${COLUMNS:-$(tput cols)}" '' | tr ' ' =
   echo $1
   printf '%*s\n' "${COLUMNS:-$(tput cols)}" '' | tr ' ' =
   echo
}

# check that system is Ubuntu
if [[ "$OSTYPE" == "darwin"* ]]; then # on MacOS
   echo "Sorry, OpenSatKit will only run on Linux. Please use an Ubuntu virtual machine."
   exit 1
elif [[ -z $(which apt-get) ]]; then # no Ubuntu package manager
   echo "You are missing the apt-get package manager."
   echo "Sorry, OpenSatKit currently only supports Ubuntu. Please use an Ubuntu virtual machine."
   exit 1
fi

main
