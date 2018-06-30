#!/usr/bin/env bash

OPEN_SAT_KIT_VERSION="0.0.1"

main()
{

announce "INSTALLATION WILL TAKE TIME AND INCLUDES PROMPTS!"

# set default path unless environment var exists
if [ ! -n "${!OPEN_SAT_KIT_PATH}" ]; then
OPEN_SAT_KIT_PATH="`pwd`/OpenSatKit-master/"
fi

announce "Installing OpenSatKit v$OPEN_SAT_KIT_VERSION in \"$OPEN_SAT_KIT_PATH\""
confirm "Continue?" 1

announce "Downloading archive zip file from \"https://github.com/OpenSatKit/OpenSatKit/archive/master.tar.gz\""
wget https://github.com/OpenSatKit/OpenSatKit/archive/master.tar.gz
tar -xzf master.tar.gz
rm master.tar.gz
cd OpenSatKit-master

announce "Installing dependencies"
cmdtoapt="apt-get update -y; apt-get install -y cmake; apt-get install -y curl; apt-get install -y default-jre; apt-get install -y gcc-multilib; apt-get install -y xfce4-terminal; apt-get install -y gcc; apt-get install -y g++; apt-get install -y libssl-dev; apt-get install -y libyaml-dev; apt-get install -y libffi-dev; apt-get install -y libreadline6-dev; apt-get install -y zlib1g-dev; apt-get install -y libgdbm3; apt-get install -y libgdbm-dev; apt-get install -y libncurses5-dev; apt-get install -y git; apt-get install -y libgstreamer0.10-dev; apt-get install -y libgstreamer-plugins-base0.10-dev; apt-get install -y freeglut3; apt-get install -y freeglut3-dev; apt-get install -y qt4-default; apt-get install -y qt4-dev-tools;apt-get install -y libqt4-dev; apt-get install -y libsmokeqt4-dev; apt-get install libcanberra-gtk*;apt install dos2unix; apt-get update -y;"


sudo bash -c "$cmdtoapt"

announce "Installing ruby 2.4.2 using rbenv"

if [ -d $HOME/.rbenv ]; then
read -p ".rbenv folder already exists. Delete for clean install? (Yn): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]?$ ]]; then
rm -rf ~/.rbenv
fi
fi
if [ ! -d $HOME/.rbenv ]; then

#rbenv installation
git clone https://github.com/sstephenson/rbenv.git ~/.rbenv

#ruby-build installation for rbenv install command
git clone https://github.com/sstephenson/ruby-build.git ~/.rbenv/plugins/ruby-build

#automatically rehash rbenv using plugin
git clone https://github.com/sstephenson/rbenv-gem-rehash.git ~/.rbenv/plugins/rbenv-gem-rehash

#automatically install default gems
git clone https://github.com/rbenv/rbenv-default-gems.git $(rbenv root)/plugins/rbenv-default-gems
else
echo ".rbenv folder exists. Skipping git clone..."
fi

#export rbenv path to bashrc
echo 'export PATH="$HOME/.rbenv/bin:$PATH"' >> ~/.bashrc
export PATH="$HOME/.rbenv/bin:$PATH"

#export shims path to bashrc
echo 'export PATH="$HOME/.rbenv/shims:$PATH"' >> ~/.bashrc
export PATH="$HOME/.rbenv/shims:$PATH"

#initialize rbenv with terminal restart
echo 'eval "$(rbenv init -)"' >> ~/.bashrc
eval "$(rbenv init -)"

CONFIGURE_OPTS="--enable-shared" rbenv install 2.4.2
rbenv global 2.4.2
echo 'gem: --no-ri --no-rdoc' >> ~/.gemrc

#check ruby version and path of installations
ruby -v
which rbenv
which ruby
echo 'gem: --no-ri --no-rdoc' >> ~/.gemrc

announce "Installing COSMOS gem"
gem install cosmos
cd ./cosmos/
gem install bundler
bundle update
gem clean up
cd ..

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
cd ../..

#to fix shebang line \r warning by converting from Dos -> Unix
git config --global core.autocrlf input
find ./ -type f -exec dos2unix {} \;

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
