#!/bin/bash

old_version="2\.1\.14"
old_version_hex="0x020114"

new_version="2.1.15"
new_version_hex="0x020115"

echo -e " Will replace version $old_version with $new_version and $old_version_hex with $new_version_hex in all .pri and meson.build files"
echo -n  -e " Do you want to continue [y|n]?  [y] "
read  -s -n 1 doit
if [ "$doit" != "y" ]  && [ "$doit" != "" ]; then
        echo -e "\n\n\e[1;35m*\e[0m operation cancelled.\n"
        exit 1
fi

find . -name "*.pri" -exec  sed -i "s/$old_version_hex/$new_version_hex/g" {} \;

find . -name "*.pri" -exec  sed -i "s/$old_version/$new_version/g" {} \;


find . -name "meson.build" -exec  sed -i "s/$old_version_hex/$new_version_hex/g" {} \;

find . -name "meson.build" -exec  sed -i "s/$old_version/$new_version/g" {} \;

find . -name "CMakeLists.txt" -exec  sed -i "s/$old_version_hex/$new_version_hex/g" {} \;
find . -name "CMakeLists.txt" -exec  sed -i "s/$old_version_hex/$new_version_hex/g" {} \;
