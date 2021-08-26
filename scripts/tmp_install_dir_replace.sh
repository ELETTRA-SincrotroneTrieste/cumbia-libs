#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 from_dir  to_dir"
    exit 1
else
    tmp_installdir=$1
    install_prefix=$2
fi

# Replace any occurrence of $tmp_installdir in any file (e.g. pkgconfig files)
#
#
echo -e -n "\n\n\e[1;32m* \e[0mreplacing temporary install dir references ($tmp_installdir) across \e[1;2mall files\e[0m with \"\e[1;2m$install_prefix\e[0m\"..."

echo -e -n " .pro, "
find $tmp_installdir -type f -name "*.pro" -exec  sed -i 's#'"$tmp_installdir"'#'"$install_prefix"'#g' {} +

echo -e -n " .pri, "
find $tmp_installdir -type f -name "*.pri" -exec  sed -i 's#'"$tmp_installdir"'#'"$install_prefix"'#g' {} +

echo -e -n "meson.build, "
find $tmp_installdir -type f -name "meson.build" -exec  sed -i 's#'"$tmp_installdir"'#'"$install_prefix"'#g' {} +

echo -e -n "Makefile "
find $tmp_installdir -type f -name "Makefile" -exec  sed -i 's#'"$tmp_installdir"'#'"$install_prefix"'#g' {} +

echo -e -n "*.pc (pkgconfig files) "
find $tmp_installdir -type f -name "*.pc" -exec  sed -i 's#'"$tmp_installdir"'#'"$install_prefix"'#g' {} +

echo -e "\t[\e[1;32mdone\e[0m]\n"

echo -e "\e[1;35mchecking\e[0m for temporary install dir references in files under \e[1;2m$tmp_installdir\e[0m:"
echo -e "[ \e[1;37;3mgrep -rI  "$tmp_installdir"  $tmp_installdir/* \e[0m]"
echo -e "\e[0;33m+----------------------------------------------------------------------------------------+\e[0m"
echo -e "\e[1;31m"

# grep -rI r: recursive I ignore binary files
#
grep -rI "$tmp_installdir" $tmp_installdir/*
#
echo -e "\e[0;33m+----------------------------------------------------------------------------------------+\e[0m"
echo -e "\e[0m [\e[1;35mcheck \e[0;32mdone\e[0m] (^^ make sure there are no red lines within the dashed line block above ^^)"
