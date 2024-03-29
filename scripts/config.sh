#!/bin/bash

# This file sets two configuration variables used by cubuild.sh
#
#  I. PKG_CONFIG_PATH
#
# II. Installation prefix
#
#
# PKG_CONFIG_PATH must specify where dependencies can be found.
# Required dependencies are
#
# 1. Tango (for tango modules: cumbia-tango and qumbia-tango-controls)
# 2. Epics (for epics modules: cumbia-epics and qumbia-epics-controls)
# 3. Qwt libraries (for cumbia-qtcontrols)
#
#                                                         I
#
#
## =======================================          PKG_CONFIG_PATH          ===========================

## if PKG_CONFIG_PATH is not already exported, you can define it here
## 
## example:
#
# pkg_config_path=/usr/local/lib/pkgconfig:/usr/local/tango-9.2.5a/lib/pkgconfig:/usr/local/omniorb-4.2.1/lib/pkgconfig:/usr/local/zeromq-4.0.7/lib/pkgconfig:/usr/local/epics/base-3.16.1/lib/pkgconfig:/usr/local/qwt-6.1.3/lib/pkgconfig 

if [ -z $install_prefix ]; then
    prefix_from_environment=0
 
## =======================================================================================================


#
#                                                        II
#
#
## ============================================= global installation prefix ==============================
## Set the global prefix for the installation
## This will set INSTALL_ROOT variable in Qt qmake projects and -Dprefix=$prefix in meson projects
##

##  vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
 
#install_prefix=/usr/local/cumbia-libs
    install_prefix=/usr/local/cumbia-libs

##  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    echo -e "\e[1;32m* \e[0;4mconfig.sh\e[0m: install_prefix: $install_prefix."
    echo -e "\e[1;32m* \e[0mYou can either set the \"install_prefix\" variable before calling this script"
    echo -e "\e[1;32m* \e[0mor edit scripts/config.sh"
else
    prefix_from_environment=1
    install_prefix=$install_prefix
fi


## =======================================================================================================

#
#                                                        III
#
#
## ============================================= temporary installation prefix ==============================
#
## Use this to build and place all the library destination files into a specific directory
## Default: $PWD/build
#
##  vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
if [ -z $build_dir ]; then
    build_dir=$PWD/build
    build_dir_from_environment=0
    echo -e "\e[0;32m* \e[0;4mconfig.sh\e[0m: build dir: $build_dir."
    echo -e "\e[0;32m* \e[0mYou can either set the \"build_dir\" variable before calling this script"
    echo -e "\e[0;32m* \e[0mor edit scripts/config.sh\n"

else
    build_dir_from_environment=1
    echo ""
    echo -e "\e[0;32m* \e[0;4mconfig.sh\e[0m: a complete build of the library  will be placed into \e[1;3m$build_dir\e[0m"
    echo -e "\e[0;32m* \e[0mand will be \e[0;3mready to be manually copied\e[0m into \e[0;3m$install_prefix \e[0m\n"
    build_dir=$build_dir
fi

## ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#
#
## This is passed to meson configure -Dlibdir=
## lib or lib64 (meson's default)
lib_dir=lib

#
#  ============ meson buildtype =========================================================================
#
## This is passed to meson configure -Dbuildtype=
## Possible choices are: "plain", "debug", "debugoptimized", "release", "minsize", "custom".
#
## ** Note: for Qt qmake projects, it is necessary to edit each .pro file and set CONFIG += debug|release
#
#
build_type=release
#
#
#  =======================================================================================================
#
#

if [ -n "$PKG_CONFIG_PATH" ];  then
	echo -e "\n \e[1;33minfo\e[0m: \e[1;37;4mpkgconfig paths\e[0m from shell environment (PKG_CONFIG_PATH is defined):\e[0m"
elif [ ! -z $pkg_config_path ]; then
	export PKG_CONFIG_PATH=$pkg_config_path
	echo -e "\n \e[1;33minfo\e[0m: \e[1;37;4mpkgconfig paths\e[0m from \e[1;37;4mscripts/config.sh\e[0m file:\e[0m"
fi

paths=$(echo $PKG_CONFIG_PATH | tr ":" "\n")

echo ""

for path in $paths
do
	echo -e " - $path"
done
echo ""

