#!/usr/bin/env bash

OPEN_SAT_KIT_VERSION="0.0.1"

main()
{
	# set default path unless environment var exists
	if [ ! -n "${!OPEN_SAT_KIT_PATH}" ]; then
		OPEN_SAT_KIT_PATH="`pwd`/OpenSatKit/"
	fi
	announce "Installing OpenSatKit v$OPEN_SAT_KIT_VERSION in \"$OPEN_SAT_KIT_PATH\""
	confirm "Continue?" 1

	announce "Downloading archive zip file from \"https://github.com/OpenSatKit/OpenSatKit/archive/master.tar.gz\""
	curl -LO https://github.com/OpenSatKit/OpenSatKit/archive/master.tar.gz
	tar -xzf master.tar.gz
	rm master.tar.gz
	cd OpenSatkit-master

	announce "Installing dependencies"
  cmdtoapt="apt-get update -y; apt-get install -y cmake; apt-get install -y curl; apt-get install -y default-jre; apt-get install -y gcc-multilib; apt-get install -y xfce4-terminal; apt-get install -y gcc; apt-get install -y g++; apt-get install -y libssl-dev; apt-get install -y libyaml-dev; apt-get install -y libffi-dev; apt-get install -y libreadline6-dev; apt-get install -y zlib1g-dev; apt-get install -y libgdbm3; apt-get install -y libgdbm-dev; apt-get install -y libncurses5-dev; apt-get install -y git; apt-get install -y libgstreamer0.10-dev; apt-get install -y libgstreamer-plugins-base0.10-dev; apt-get install -y cmake; apt-get install -y freeglut3; apt-get install -y freeglut3-dev; apt-get install -y qt4-default; apt-get install -y qt4-dev-tools; apt-get install -y libsmokeqt4-dev;"
  sudo bash -c "$cmdtoapt"

  announce "Installing ruby 2.2.6 using rbenv"
  if [ -d $HOME/.rbenv ]; then
    read -p ".rbenv folder already exists. Delete for clean install? (Yn): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]?$ ]]; then
      rm -rf ~/.rbenv
    fi
  fi
  if [ ! -d $HOME/.rbenv ]; then
    git clone https://github.com/sstephenson/rbenv.git ~/.rbenv
    git clone https://github.com/sstephenson/ruby-build.git ~/.rbenv/plugins/ruby-build
  else
    echo ".rbenv folder exists. Skipping git clone..."
  fi
  echo 'export PATH="$HOME/.rbenv/bin:$PATH"' >> ~/.bashrc
  export PATH="$HOME/.rbenv/bin:$PATH"
  echo 'eval "$(rbenv init -)"' >> ~/.bashrc
  eval "$(rbenv init -)"
  CONFIGURE_OPTS="--enable-shared" rbenv install 2.2.6
  rbenv rehash
  rbenv global 2.2.6
  echo 'gem: --no-ri --no-rdoc' >> ~/.gemrc

  announce "Installing COSMOS gem"
  gem install cosmos
  rbenv rehash

  announce "Building CFS"
  cd ./cfs/
	make distclean
	make prep
	make
	make install
	cp apps/kit_sch/fsw/tables/* build/exe/cpu1/cf/
	cp apps/kit_to/fsw/tables/kit_to_pkttbl.xml build/exe/cpu1/cf/
	cd ..

	announce "Finished. Launching COSMOS with 'ruby cosmos/Launcher'"
	ruby cosmos/Launcher

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
