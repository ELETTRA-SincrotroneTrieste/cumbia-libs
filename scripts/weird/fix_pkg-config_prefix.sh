#!/bin/bash


# This script replaces prefix=any/prefix with the prefix specified on the command line argument
# Example:
#  before: prefix=/tmp/usr/local/cumbia-libs-1.3.0
#  call ./fix_pkg-config_prefix.sh  /usr/local/cumbia-libs
#  after: prefix=/usr/local/cumbia-libs

# It can be used when plugins or third party modules are built within a temporary folder 
# that is subsequently moved elsewhere. 
# You won't need this normally.

############### WARNING ###################
#          use at your own risk           #
###########################################

if [ -z "$1" ]
  then
    echo -e "Usage: $0 \e[1;3mprefix\e[0m\n"
  else
	  find . -name "*.pc" -exec sed -i 's|'prefix=.*'|prefix='$1'|g'  {} \;
fi

