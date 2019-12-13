#!/bin/bash

#save working dir
savedir=$PWD

DIR="${BASH_SOURCE%/*}"
if [[ ! -d "$DIR" ]]; then DIR="$PWD"; fi

realp=`realpath $DIR`
root_d=`dirname $realp`



# change into cumbia source top level folder
cd $root_d

if [ ! -r $root_d/scripts/config.sh ]; then
        echo " \e[1;31merror\e[0m file $root_d/scripts/config.sh is missing"
else

    echo -e "\n \e[1;33minfo\e[0m: reading configuration from \"$root_d/scripts/config.sh\""
    . "$root_d/scripts/config.sh" &>/dev/null

    echo -e " \e[1;33minfo\e[0m: using installation prefix \"$root_d\""

    if [ ! -d $root_d/lib ] || [ ! -d $root_d/include ] || [ ! -d $root_d/share ]; then
        echo -e "\n\e[1;31m*\n* error\e[0m: directory \"$root_d\" does not contain dirs lib/ include/ share/ subfolders\n\e[1;31m*\e[0m\n"
    else

        echo -e "\n \e[1;32m---\e[0m\n"

        echo -e "\e[4mPKGCONFIG\e[0m"
        pkg_cfg_p=$install_prefix/lib/pkgconfig
        if [[ $PKG_CONFIG_PATH != *"$pkg_cfg_p"* ]]; then
            echo -e " \e[1;32maction\e[0m prepending \e[0;32m$pkg_cfg_p\e[0m to \e[0;3mPKG_CONFIG_PATH "
            PKG_CONFIG_PATH=$pkg_cfg_p:$PKG_CONFIG_PATH
            export PKG_CONFIG_PATH
        else
            echo -e " \e[1;33minfo\e[0m: \e[0;3m$pkg_cfg_p\e[0m already in PKG_CONFIG_PATH"
        fi

        echo -e " \e[1;33minfo\e[0m: you can now build using this version of the library. Remember to call "
        echo -e " \e[1;33minfo\e[0m: \e[1;32mqmake INSTALL_ROOT=$install_prefix\e[0m before building \e[1;32mQt applications\e[0m"
        echo ""

        echo -e "\e[4mLD_LIBRARY_PATH\e[0m"
        libpath=$install_prefix/lib
        if [[ $LD_LIBRARY_PATH != *"$libpath"* ]]; then
            echo -e " \e[1;32maction\e[0m prepending \e[0;32m$libpath\e[0m to \e[0;3mLD_LIBRARY_PATH \e[0m"
            export LD_LIBRARY_PATH=$libpath:$LD_LIBRARY_PATH
            echo -e -n " \e[1;33minfo\e[0m: found (and added to LD_LIBRARY_PATH) libraries: "
        else
            echo -e " \e[1;33minfo\e[0m: \e[0;3m$libpath\e[0m already in LD_LIBRARY_PATH. Libraries: "
        fi
        echo -e " \e[0;34m | `find $libpath -name *.so.*.*.* -printf "%f | "` \e[0m"
        echo ""

        echo -e "\e[4mQT_PLUGINS_PATH\e[0m"

        plupath=$install_prefix/lib/qumbia-plugins

        if [[ $QT_PLUGIN_PATH != *"$plupath"* ]]; then
            echo -e " \e[1;32maction\e[0m prepending \e[0;32m$plupath\e[0m to \e[0;3mQT_PLUGIN_PATH \e[0m"
            export QT_PLUGIN_PATH=$plupath:$QT_PLUGIN_PATH
            echo -e -n " \e[1;33minfo\e[0m: added plugins: "
        else
            echo -e " \e[1;33minfo\e[0m: \e[0;3m$plupath\e[0m already in QT_PLUGIN_PATH. Plugins: "
        fi
        echo -e " \e[0;34m | `find $install_prefix/lib/qumbia-plugins/ -name *.so -printf "%f | "` \e[0m"
        echo ""

        echo -e "\e[4mPATH\e[0m"
        path=$install_prefix/bin
        if [[ $PATH != *"$path"* ]]; then
            echo -e " \e[1;32maction\e[0m prepending \e[0;32m$path\e[0m to \e[0;3mPATH \e[0m"
            export PATH=$install_prefix/bin:$PATH
        else
            echo -e " \e[1;33minfo\e[0m: \e[0;3m$path\e[0m already in PATH."
        fi

        echo ""

        echo -e "\e[4mBASH COMPLETION\e[0m"
        if [ -f $install_prefix/share/bash_completion.d/cumbia ]; then
            echo -e " \e[1;32maction\e[0m sourcing \e[0;36m$install_prefix/share/bash_completion.d/cumbia/bin\e[0m for bash completion"
        fi

        if [ -d $install_prefix/share/bash-completion.d ]; then
            echo -e " \e[1;32maction\e[0m sourcing files under \e[0;36m$install_prefix/share/bash-completion.d\e[0m for bash completion"
            echo -e -n " \e[1;33minfo\e[0m: scripts: "
            echo -e " \e[0;34m | `find $install_prefix/share/bash-completion.d -type f -printf "%f | "` \e[0m\n"

        for f in $install_prefix/share/bash-completion.d/*
            do
            source $f
        done
         #   source `find $install_prefix/share/bash-completion.d -type f`

        fi
        echo ""

        # restore dir
        cd $savedir

        echo -e " \e[1;32m---\e[0m\n"
   fi # root_d contains include lib share
fi # file config.sh exists


