#!/bin/bash

echo $PWD

topdir=$PWD

DIR="${BASH_SOURCE%/*}"

echo "la var dir e $DIR"

build=0
docs=0
make_install=0
clean=0

if [[ $@ == **pull** ]]
then
    echo -e "\e[1;32mupdating sources [git pull]...\e[0m"
	git pull
fi

if [[ $@ == **install** ]]
then
    make_install=1
	build=1
fi

if [[ $@ == **doc** ]]
then
    docs=1
fi

if [[ $@ == **clean** ]]
then
    clean=1
fi

if  [ "$#" == 0 ]; then
	build=1
fi



if [[ ! -d "$DIR" ]]; then DIR="$PWD"; fi
. "$DIR/projects.sh"

for x in "${meson_p[@]}" ; do
	cd $DIR/../${x}
	meson builddir
	cd builddir

	#
	## clean ###
    #	
	if [ $clean -eq 1 ]; then
		echo -e "\e[1;33m\n*\n* CLEAN project ${x}...\n*\e[0m"
		ninja clean

		# clean failed?
		if [ $? -ne 0 ]; then
			exit 1
		fi
	fi

	#
	## build ###
	#
	if [ $build -eq 1 ]; then
		echo -e "\e[1;32m\n*\n* BUILD project ${x}...\n*\e[0m"
		ninja 
		if [ $? -ne 0 ]; then
			exit 1
		fi
	fi


	#
	## docs ###
	#
	if [ $docs -eq 1 ]; then
		echo -e "\e[1;36m\n*\n* BUILD DOCS project ${x}...\n*\e[0m"
		if [ -d doc/html ]; then
			rm -rf doc/html
		fi
		ninja doc/html
		if [ $? -ne 0 ]; then
			exit 1
		fi
		docsdir=$topdir/$DIR/../docs/html/${x}
		docshtmldir=$docsdir/html
		echo -e "\e[1;36m\n*\n* COPY DOCS project ${x} into \"$docsdir\" ...\n*\e[0m"
		if [ -x $docsdir ]; then
			rm -rf $docshtmldir
			cp -a doc/html $docsdir/
			if [ $? -ne 0 ]; then
				exit 1
			fi
		else
			echo -e "\e[1;36m\n*\n* COPY DOCS \e[1;31mERROR\e[1;36m: directory \"$docsdir\" does not exist!\n*\e[0m"
			exit 1
		fi
	fi


# install?
	if [ $make_install -eq 1 ]; then
		echo -e "\e[0;32m\n*\n* INSTALL project ${x}...\n*\e[0m"
		ninja install
	fi
	cd $topdir


done


for x in "${qmake_p[@]}"; do
	cd $DIR/../${x}

	#
	## clean ###
    #	
	if [ $clean -eq 1 ]; then
		echo -e "\e[1;33m\n*\n* CLEAN project ${x}...\n*\e[0m"
		make distclean

		# clean failed?
		if [ $? -ne 0 ]; then
			exit 1
		fi
	fi

	#
	## build ###
	#
	if [ $build -eq 1 ]; then
		echo -e "\e[1;32m\n*\n* BUILD project ${x}...\n*\e[0m"
		qmake && make -j9
		if [ $? -ne 0 ]; then
			exit 1
		fi
	fi


	#
	## docs ###
	#
	if [ $docs -eq 1 ]; then
		echo -e "\e[1;36m\n*\n* BUILD DOCS project ${x}...\n*\e[0m"
		if [ -d doc ]; then
			rm -rf doc
		fi
		qmake && make doc
		if [ $? -ne 0 ]; then
			echo -e "\e[1;36m\n*\n* BUILD DOCS project ${x} has no \"doc\" target...\n*\e[0m\n"
		fi
		docsdir=$topdir/$DIR/../docs/html/${x}
		docshtmldir=$docsdir/html
		echo -e "\e[1;36m\n*\n* COPY DOCS project ${x} into \"$docsdir\" ...\n*\e[0m"
		if [ -x $docsdir ]; then
			rm -rf $docshtmldir
			cp -a doc/html $docsdir/
			if [ $? -ne 0 ]; then
				exit 1
			fi
		else
			echo -e "\e[1;36m\n*\n* COPY DOCS \e[1;31mERROR\e[1;36m: directory \"$docsdir\" does not exist!\n*\e[0m"
			exit 1
		fi
	fi


# install?
	if [ $make_install -eq 1 ]; then
		echo -e "\e[0;32m\n*\n* INSTALL project ${x}...\n*\e[0m"
		make install
	fi
	cd $topdir


done

savedir=$DIR

for x in "${qmake_subdir_p[@]}"; do
	echo "changeing into ${x}..."
	cd ${x}
	echo $PWD
	cd $DIR/../${x}

	#
	## clean ###
    #	
	if [ $clean -eq 1 ]; then
		echo -e "\e[1;33m\n*\n* CLEAN project ${x}...\n*\e[0m"
		make distclean

		# clean failed?
		if [ $? -ne 0 ]; then
			exit 1
		fi
	fi

	#
	## build ###
	#
	if [ $build -eq 1 ]; then
		echo -e "\e[1;32m\n*\n* BUILD project ${x}...\n*\e[0m"
		qmake && make -j9
		if [ $? -ne 0 ]; then
			exit 1
		fi
	fi


	#
	## docs ###
	#
	if [ $docs -eq 1 ]; then

		for sd in `ls -1 -d */`; do
			
			## build doc qui
		done # for
	fi

	cd ..

done
