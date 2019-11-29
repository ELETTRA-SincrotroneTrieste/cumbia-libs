#!/bin/bash

DIR="${BASH_SOURCE%/*}"
if [[ ! -d "$DIR" ]]; then DIR="$PWD"; fi

# change into cumbia source top level folder
cd $DIR/..

if [ ! -r $DIR/config.sh ]; then
        echo " \e[1;31merror\e[0m file $DIR/config.sh is missing"
        exit 1
fi

echo -e "\n \e[1;33minfo\e[0m: reading configuration from \"$DIR/config.sh\""
. "$DIR/config.sh"

echo -e " \e[1;33minfo\e[0m: installation prefix is \"$install_prefix\""

echo -e "\n \e[1;32m---\e[0m\n"

echo -e " \e[1;32maction\e[0m prepending \e[0;32m$install_prefix/lib/pkgconfig\e[0m to \e[0;3mPKG_CONFIG_PATH"
PKG_CONFIG_PATH=$install_prefix/lib/pkgconfig:$PKG_CONFIG_PATH
export PKG_CONFIG_PATH

echo -e " \e[1;32maction\e[0m prepending \e[0;32m$install_prefix/lib\e[0m to \e[0;3mLD_LIBRARY_PATH \e[0m"
export LD_LIBRARY_PATH=$install_prefix/lib:$LD_LIBRARY_PATH

echo -e " \e[1;32maction\e[0m prepending \e[0;32m$install_prefix/lib/qumbia-plugins\e[0m to \e[0;3mQT_PLUGIN_PATH \e[0m"
export QT_PLUGIN_PATH=$install_prefix/lib/qumbia-plugins:$QT_PLUGIN_PATH

echo -e " \e[1;32maction\e[0m prepending \e[0;32m$install_prefix/bin\e[0m to \e[0;3mPATH \e[0m"
export PATH=$install_prefix/bin:$PATH

if [ -f $install_prefix/share/bash_completion.d/cumbia ]; then
    echo -e " \e[1;32maction\e[0m sourcing \e[0;36m$install_prefix/share/bash_completion.d/cumbia/bin\e[0m for bash completion"
fi

if [ -d $install_prefix/share/bash_completion.d ]; then
    echo -e " \e[1;32maction\e[0m sourcing files under \e[0;36m$install_prefix/share/bash_completion.d\e[0m for bash completion"
    source $install_prefix/share/bash_completion.d/*
fi


echo -e "\n \e[1;32m---\e[0m\n"
