#!/usr/bin/env #!/usr/bin/env bash

OPEN_SAT_KIT_VERSION="1.6.0"
BASE_DIR="$(pwd)"

main()
{
announce "INSTALLATION WILL TAKE TIME AND INCLUDES PROMPTS!"

# set default path unless env var exists
if [ ! -n "${OPEN_SAT_KIT_PATH}"]; then
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

nnounce "Installing dependencies"
cmdtoapt="yum update -y; yum install -y cmake; yum install -y curl; yum install -y default-jre; yum install -y gcc-multilib; yum install -y xfce4-terminal; yum install -y gcc; yum install -y g++; yum install -y libssl-dev; yum install -y libyaml-dev; yum install -y libffi-dev; yum install -y libreadline6-dev; yum install -y zlib1g-dev; yum install -y libgdbm3; yum install -y libgdbm-dev; yum install -y libncurses5-dev; yum install -y git; yum install -y libgstreamer0.10-dev; yum install -y libgstreamer-plugins-base0.10-dev; yum install -y freeglut3; yum install -y freeglut3-dev; yum install -y qt4-default; yum install -y qt4-dev-tools;yum install -y libqt4-dev; yum install -y libsmokeqt4-dev; yum install libcanberra-gtk*;apt install dos2unix; yum update -y;"

sudo bash -c "$cmdtoapt"

announce "Installing COSMOS (Includes Ruby). When prompted select Sudo. Demo is optional (not needed for OSK)."
confirm "Continue?" 1

bash <(\curl -sSL https://raw.githubusercontent.com/BallAerospace/COSMOS/master/vendor/installers/linux_mac/INSTALL_COSMOS.sh)

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
make cd ..

# Complete COSMOS Install
cd ./cosmos/
if [ -f Gemfile.lock ]; then
  rm Gemfile.lock
fi
bundle install
cd ..

# Fix shebang line \r warning by converting from Dos->Unix
git config --global core.autocrlf input
find ./ -type f -exec dos2unix {} \;

# Maybe required. Is required in Ubuntu edition
announce "Close this terminal. Open new terminal: cd OpenSatKit-master/cosmos/ and run 'ruby Launcher' "

return 1
}

# Ask user to confirm an action
confirm() # prompt, default (0:N, 1: Y)
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

main
