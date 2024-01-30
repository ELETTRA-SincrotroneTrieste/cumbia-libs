#!/bin/bash

# set your qwt and cumbia root here
# -------------------------------------------
#
QWT_ROOT=/usr/local/qwt-6.2.0-qt6
CUMBIA_ROOT=/usr/local/cumbia-libs/qt6
#
# --------------------------------------------
#
#
echo -e "setting \e[1;31mqt6\e[0m environment for \e[1;36mqwt\e[0m and \e[1;36mcumbia\e[0m"
echo -e "\e[1;36mqwt\e[0m\t -> \e[1;32;3m$QWT_ROOT\e[0m"
echo -e "\e[1;36mcumbia\e[0m\t -> \e[1;32;3m$CUMBIA_ROOT\e[0m"

export PKG_CONFIG_PATH=/usr/local/tango-9.4.0/lib/pkgconfig:\
/usr/local/omniorb-4.2.5/lib/pkgconfig:\
$QWT_ROOT/lib/pkgconfig:\
$CUMBIA_ROOT/lib/pkgconfig:\
$PKG_CONFIG_PATH

alias qmake="qmake6 INSTALL_ROOT=$CUMBIA_ROOT CUMBIA_ROOT=$CUMBIA_ROOT"

export QT_PLUGIN_PATH=/usr/local/cumbia-libs/qt6/lib/qumbia-plugins:\
/usr/local/qtango/lib/plugins

export LD_LIBRARY_PATH=$CUMBIA_ROOT/lib:\
$QWT_ROOT/lib:\
$LD_LIBRARY_PATH

export PATH=$CUMBIA_ROOT/bin:$PATH
