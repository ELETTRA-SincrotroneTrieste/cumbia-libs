#!/bin/bash

DIR="${BASH_SOURCE%/*}"
if [[ ! -d "$DIR" ]]; then DIR="$PWD"; fi

# change into cumbia source top level folder
cd $DIR/..

topdir=$PWD

build=0
docs=0
make_install=0
clean=0
cleandocs=0
push_docs=0
meson=0
env_update=0
epics=0
tango=0
pull=0
srcupdate=0
sudocmd=sudo


srcupdate_conf_f=$HOME/.config/cumbia/srcupdate.conf

operations=()

if [[ $@ == **help** ]]
then
	echo -e "\n\e[1;32mOPTIONS\e[0m\n"
	echo -e " [no arguments] - build cumbia, cumbia-qtcontrols, qumbia-apps, qumbia-plugins."
	echo -e "                  No engine specific projects will be built (cumbia-tango, cumbia-epics, qumbia-tango-controls)" 
	echo -e "                  To build tango modules, add the \"tango\" argument, to build the epics modules, add \"epics\" (example 3.)"
	echo -e " pull - update sources from git"
	echo -e " env-update - execute the \"source\" bash command to update the environment as to the most recently installed scripts"
	echo -e " push-documentation - execute git commit and git push on the \"docs\" directory so that the \"github.io\" pages are updated"
	echo -e " docs - regenerate documentation"
	echo -e " cleandocs - remove locally generated documentation"
	echo -e " clean - execute make clean in every project folder"
	echo -e " install - execute make install from every project folder\n"
	echo -e " no-sudo - do not use \"sudo\" when calling make install\""
	echo -e " tango - add cumbia-tango and qumbia-tango-controls modules"
	echo -e " epics - add cumbia-epics and qumbia-epics-controls modules"
	echo -e " srcupdate - update cumbia sources choosing from the available tags in git (or origin/master). Please note that"
    echo -e "             the copy of the sources managed with srcupdates is not intended to be modified and committed to git."
    echo -e "             This option is normally used by the \e[0;4mcumbia upgrade\e[0m command."
	echo ""
	echo -e "\e[1;32mEXAMPLES\e[0m\n1. $0 pull clean install tango - git pull sources, execute make clean, build and install cumbia, cumbia-qtcontrols, apps, plugins and the tango modules"
	echo -e "2. $0 docs push-documentation tango epics - regenerate the projects' documentation and update it on the \"github.io\" pages (including tango and epics modules)"
	echo -e "3. $0 tango - build cumbia, cumbia-tango, cumbia-qtcontrols, qumbia-tango-controls, qumbia-apps and qumbia-plugins"
	echo ""

	exit 0
fi

if [ ! -r $DIR/config.sh ]; then
	echo " \e[1;31merror\e[0m file $DIR/config.sh is missing"
	exit 1
fi

echo -e "\n \e[1;33minfo\e[0m: reading configuration from \"$DIR/config.sh\""
. "$DIR/projects.sh"
. "$DIR/config.sh"

echo -e " \e[1;33minfo\e[0m: installation prefix is \"$prefix\""

if [[ $@ == **pull** ]]
then
	pull=1
	operations+=(pull)
fi

if [[ $@ == **push-documentation** ]]
then
    push_docs=1
	operations+=(push-documentation)
fi

if [[ $@ == **no-sudo** ]]
then
    $sudocmd=
fi

if [[ $@ == **env-update** ]]
then
    env_update=1
	operations+=(env-update)
fi

if [[ $@ == **install** ]]
then
    make_install=1
	build=1
	meson=1
	operations+=(install)
fi

if [[ $@ == **docs** ]]
then
    meson=1
    docs=1
	operations+=(docs)
fi

if [[ $@ == **cleandocs** ]]
then
	meson=1
    cleandocs=1
	operations+=(cleandocs)
fi

if [[ $@ == **clean** ]]
then
	meson=1
    clean=1
	operations+=(clean)
fi

if [[ $@ == **srcupdate** ]]
then
	srcupdate=1
	operations+=(srcupdate)
fi

## check options compatibility
##
if [ $srcupdate -eq 1 ]  && [ "$#" -ne 1 ] ; then
	echo -e " \e[1;31merror\e[0m: \"srcupdate\" is not compatible with any other option\n"
	exit 1
fi

if [[ $@ == **tango** ]]
then
	tango=1
fi

if [[ $@ == **epics** ]]
then
	epics=1
fi

if  [ "$#" == 0 ]; then
	build=1
	meson=1
fi

if [[ $tango -eq 1 ]]; then
	meson_p+=(cumbia-tango)
	qmake_p+=(qumbia-tango-controls)
fi

if [[ $epics -eq 1 ]]; then
	meson_p+=(cumbia-epics)
	qmake_p+=(qumbia-epics-controls)
fi

if [ ${#operations[@]} -eq 0 ]; then
	operations+=(build)
	build=1
fi

## Print prompt
##
echo -e " \e[1;33minfo\e[0m:"
echo -e "  You may execute $0 --help for the list of available options"
echo -e "  Please refer to https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs README.md for installation instructions"
echo -e "  Documentation: https://elettra-sincrotronetrieste.github.io/cumbia-libs/"

echo -e "\n-----------------------------------------------------------------------"

echo ""
echo -e "\e[1;32;4mSOURCES\e[0m: \n\n * $topdir"
echo ""
echo -e "\e[1;32;4mOPERATIONS\e[0m:\n"
for x in "${operations[@]}"; do
	echo -e " * ${x}"
done

echo ""

if [[ $srcupdate -eq 0 ]]; then 

	## START PRINT OPERATIONS MENU
	echo -e "The \e[1;32;4moperations\e[0m listed above will be applied to the following \e[1;32;4mMODULES\e[0m:\n"

	for x in "${meson_p[@]}" ; do
		echo -e " * ${x} [c++, meson build system]"
	done

	for x in "${qmake_p[@]}" ; do
		echo -e " * ${x} [qt,  qmake build system]"
	done

	for x in "${qmake_subdir_p[@]}"; do
		echo ""
		for sd in `ls -1 -d $DIR/../${x}/*/`; do
			echo -e " * ${sd} [qt,  qmake build system]"
		done
	done	

	if [ $tango -eq 0 ] && [ $epics -eq 0 ]; then
		echo -e " -"
		echo -e " \e[1;33minfo\e[0m"
		echo -e " \e[1;33m*\e[0m neither \e[1;31;4mtango\e[0m nor \e[1;31;4mepics\e[0m modules will be included in the \e[1;32;4mOPERATIONS\e[0m below"
		echo -e " \e[1;33m*\e[0m add \"tango\" and/or \"epics\" to enable cumbia-tango, qumbia-tango-controls"
		echo -e " \e[1;33m*\e[0m and cumbia-epics, qumbia-epics-controls respectively.\n"
	elif  [ $tango -eq 0 ]; then
		echo -e "\n \e[1;33m*\e[0m \e[1;35;4mtango\e[0m module is disabled"
	elif  [ $epics -eq 0 ]; then
		echo -e "\n \e[1;33m*\e[0m \e[1;35;4mepics\e[0m module is disabled"
	fi

	if [ $make_install -eq 1 ]; then 
		echo -e " -"
		echo -e " \e[1;32minstall\e[0m: cumbia will be installed in \"$prefix\""	
	fi
fi ## END PRINT OPERATIONS MENU

echo -e "-----------------------------------------------------------------------"

echo ""
echo -n -e "Do you want to continue? [y|n] [y] "

read  -s -n 1  cont

if [ "$cont" != "y" ]  && [ "$cont" != "yes" ] && [ "$cont" != "" ]; then
	echo -e "\n  You may execute $0 --help"
	echo -e "  Please refer to https://github.com/ELETTRA-SincrotroneTrieste/cumbia-libs README.md for installation instructions"
	echo -e "  Documentation: https://elettra-sincrotronetrieste.github.io/cumbia-libs/"
	
	exit 1
fi

## inform the user that after the installation "cumbia upgrade" can be used to upgrade cumbia
#
if [ ! -r $srcupdate_conf_f ] && [ $make_install -eq 1 ]; then
	echo -e "\n\n \e[1;32;4mREMEMBER\e[0m:\n after the first installation, you can run \e[1;32mcumbia upgrade\e[0m to update cumbia \e[0;4mas long as\e[0m:"
	echo -e "  - this source tree is not removed from this directory (\"$topdir\")"
	echo -e "  - this source tree is not intended for development, i.e. will not be modified and committed to git"
	echo -e -n "\n Press any key to continue "
	read -s -n 1 akey
fi

if [ $pull -eq 1 ]; then
    echo -e "\e[1;32mupdating sources [git pull]...\e[0m"
	git pull
fi

if [[ $srcupdate -eq 1 ]]; then
	wdir=$PWD
	cd $topdir
	# sync tags
	
	echo -e "\e[1;34m\n*\n* UPGRADE fetching available tags...\n*\e[0m"	

	git fetch --tags

	tags=`git tag --list`
	declare -a taglist
	idx=1
	for tag in $tags ; do
		echo -e " $idx. $tag"
		((idx++))
		taglist+=($tag)
	done

	taglist+=(origin/master)
	echo -e " $idx. ${taglist[$((idx-1))]}"
	
	echo -e -n "\ncurrent version: \e[1;32m"

	# With --abbrev set to 0, the command can be used to find the closest tagname without any suffix,
	# Otherwise, it suffixes the tag name with the number of additional commits on top of the tagged 
    # object and the abbreviated object name of the most recent commit.
	#
	git describe --abbrev=0
	
	echo -e "\e[0m"
	echo -e "\nChoose a version [1, ..., $idx] or press any other key to exit"

	read  -s -n 1 choice

	## is choice an integer number??
	re='^[0-9]+$'
	if ! [[ $choice =~ $re ]] ; then
		exit 1
	fi
	
	if [ "$choice" -ge "1" ] && [ "$choice" -le "$idx" ]; then
		array_index=$((choice - 1))
		checkout_tag=${taglist[$array_index]}
		echo  -e "\e[1;34m\n*\n* UPGRADE checking out version $checkout_tag ...\n*\e[0m"
		echo -e " \e[1;33mwarning\e[0m: the srcupdate procedure is intended to be run in a \e[1;37;4mread only\e[0m source tree, where these rules apply:"
		echo -e "          1. you can switch to, build and install different cumbia releases at any time"
		echo -e "          2. you \e[0;35mshould not\e[0m modify and commit changes from here (\e[0;35m$topdir\e[0m)"
		echo ""
		echo -e -n " \e[1;32m*\e[0m Do you want to update the sources to version \e[1;32m$checkout_tag\e[0m [y|n]? [y]: "
		read  -s -n 1 reply
		
		if [ "$reply" != "y" ]  && [ "$reply" != "yes" ] && [ "$reply" != "" ]; then
		 	exit 1
		fi

		git checkout $checkout_tag

		

	else
		echo -e "\n \e[1;31merror\e[0m: choice \"$choice\" is outside the interval [1, $idx]\n"
		exit 1
	fi
	 

	# restore previous directory
	cd $wdir

	##
	## exit successfully after git checkout of the desired version
		
	exit 0


fi # [[ $srcupdate -eq 1 ]]


##                            
## save configuration in $HOME/.config/cumbia/srcupdate.conf
##

if [ ! -d $HOME/.config/cumbia ]; then
	mkdir -p $HOME/.config/cumbia
fi

# empty the file
> "$srcupdate_conf_f"

if [ $tango -eq 1 ]; then
	echo -e "\n# tango enabled" >> $srcupdate_conf_f
	echo "tango=1" >> $srcupdate_conf_f
fi

if [ $epics -eq 1 ]; then
	echo -e "\n# epics enabled" >> $srcupdate_conf_f
	echo "epics=1" >> $srcupdate_conf_f
fi

echo -e "\n# directory with the cumbia sources " >> $srcupdate_conf_f
echo "topdir=$topdir" >> $srcupdate_conf_f

##
## end save configuration in $HOME/.config/cumbia/srcupdate.conf
##

if [ $make_install -eq 1 ] && [ ! -r $prefix ]; then
	echo -e "\n The installation directory \"$prefix\" does not exist. "
	echo -n  -e " Do you want to create it (the operation may require administrative privileges - sudo) [y|n]?  [y] "
	read  -s -n 1 createdir
	if [ "$createdir" != "y" ]  && [ "$reply" != "createdir" ] && [ "$createdir" != "" ]; then
		 	exit 1
	fi
	mkdir -p $prefix
	if [ "$?" -ne 0 ]; then
		if  [[ ! -z  $sudocmd  ]]; then
			echo -e " The \e[1;32msudo\e[0m password is required to create the directory \"$prefix\""
		fi
		$sudocmd mkdir -p $prefix
		if [ "$?" -ne 0 ]; then
			echo -e " \e[1;31merror\e[0m: failed to create installation directory \"$prefix\""
			exit 1
		fi
	fi
fi


if [ $cleandocs -eq 1 ]; then
	docshtmldir=$topdir/$DIR/../docs/html
	if [ -d $docshtmldir ]; then
		echo -e "\e[1;36m\n*\n* CLEAN DOCS under \"$docshtmldir\" ...\n*\e[0m"	
		cd $docshtmldir
		find . -name "html" -type d -exec rm -rf {} \;
		cd $topdir
	else
		echo -e "\e[1;36m\n*\n* CLEAN DOCS directory \"$docshtmldir\" doesn't exist.\n*\e[0m"	
	fi
	exit 0
fi


for x in "${meson_p[@]}" ; do
	cd $DIR/../${x}

	if [ $meson -eq 1 ]; then
		meson builddir
	fi

	cd builddir
	meson configure -Dprefix=$prefix

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

		if [ ! -d $docsdir ]; then
			mkdir -p $docsdir
		fi	

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
		if [ -w $prefix ]; then
			ninja install
		else
			if [ ! -z $sudocmd ]; then
				echo  -e "\e[1;32msudo\e[0m password required:"
			fi
			$sudocmd ninja install
		fi
	fi

	## Back to topdir!
	cd $topdir


done


for x in "${qmake_p[@]}"; do
	cd $DIR/../${x}

	#
	## clean ###
    #	
	if [ $clean -eq 1 ]; then
		echo -e "\e[1;33m\n*\n* CLEAN project ${x}...\n*\e[0m"
		qmake "INSTALL_ROOT=$prefix" && make distclean

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
		qmake "INSTALL_ROOT=$prefix" && make -j9
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
		qmake "INSTALL_ROOT=$prefix" && make doc
		if [ $? -ne 0 ]; then
			echo -e "\e[1;36m\n*\n* BUILD DOCS project ${x} has no \"doc\" target...\n*\e[0m\n"
		fi
		docsdir=$topdir/$DIR/../docs/html/${x}
		docshtmldir=$docsdir/html
		echo -e "\e[1;36m\n*\n* COPY DOCS project ${x} into \"$docsdir\" ...\n*\e[0m"
		
		if [ ! -d $docsdir ]; then
			mkdir -p $docsdir
		fi

		if [ -d $docsdir ]; then
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
		if [ ! -z $sudocmd ]; then
			echo -e  "\e[1;32msudo\e[0m authentication required:"
		fi
		$sudocmd make install
	fi
	cd $topdir


done

savedir=$DIR

for x in "${qmake_subdir_p[@]}"; do

	cd $DIR/../${x}

	for sd in `ls -1 -d */`; do
		cd ${sd} 
		thisdir=${sd}
		if [[ $thisdir == */ ]]; then
			pro_file="${thisdir::-1}.pro"	
		else
			pro_file="${thisdir}.pro"	
		fi

		if [ -f $pro_file ]; then

			#
			## clean ###
			#	
			if [ $clean -eq 1 ]; then
				echo -e "\e[1;33m\n*\n* CLEAN project ${sd}...\n*\e[0m"
				qmake "INSTALL_ROOT=$prefix" && make distclean

				# clean failed?
				if [ $? -ne 0 ]; then
					exit 1
				fi
			fi

			#
			## build ###
			#
			if [ $build -eq 1 ]; then
				echo -e "\e[1;32m\n*\n* BUILD project ${sd}...\n*\e[0m"
				qmake "INSTALL_ROOT=$prefix" && make -j9
				if [ $? -ne 0 ]; then
					exit 1
				fi
			fi

			if [ $docs -eq 1 ]; then

	
				echo -e "\e[1;36m\n*\n* BUILD DOCS project ${sd}...\n*\e[0m"
				
				if [ -d doc ]; then
					rm -rf doc
				fi
			

				qmake "INSTALL_ROOT=$prefix" && make doc
				if [ $? -ne 0 ]; then
					echo -e "\e[1;36m\n*\n* BUILD DOCS project ${sd} has no \"doc\" target...\n*\e[0m\n"
				else
					docsdir=$topdir/$DIR/../docs/html/${sd}
					docshtmldir=$docsdir/html
					echo -e "\e[1;36m\n*\n* COPY DOCS project ${sd} into \"$docsdir\" ...\n*\e[0m"

	
					if [ ! -d $docsdir ]; then
						mkdir -p $docsdir
					fi

					if [ -x $docsdir ]; then
						rm -rf $docshtmldir
						cp -a doc/html $docsdir/
						if [ $? -ne 0 ]; then
							exit 1
						fi
					else # if docs dir does not exist
						echo -e "\e[1;36m\n*\n* COPY DOCS \e[1;31mERROR\e[1;36m: directory \"$docsdir\" does not exist!\n*\e[0m"
						exit 1
					fi # -x $docsdir

				fi # qmake and make successful
			
			fi # docs -eq 1

			# install?
			if [ $make_install -eq 1 ]; then
				echo -e "\e[0;32m\n*\n* INSTALL project ${sd}...\n*\e[0m"
				if [ ! -z $sudocmd ]; then
					echo -e  "\e[1;32msudo\e[0m authentication required:"
				fi
				qmake "INSTALL_ROOT=$prefix" && $sudocmd make install
				if [ $? -ne 0 ]; then
					exit 1
				fi
			fi
		
		fi ## .pro file exists
		
		cd ..
	done # for

	cd ..

done


if [ $push_docs -eq 1 ]; then

	cd $topdir/$DIR/../docs/html
	if [[ ! -z $2 ]]; then
		message=$2
	else
		
		message="Regenerated docs on `date`"
		echo -e "\e[0;36m\n*\n* PUSH DOCS: committed with automatic message \"$message\". Use an additional parameter for a custom message\n  for example \"added new documentation and fixed some links\".\n"
	fi
	
	echo -e "\e[0;36m\n*\n* PUSH DOCS under `pwd` to git with commit message \"$message\"...\n*\e[0m\n"

	git commit . -m "$message"
	
	git push

	cd $topdir
fi

if [ $env_update -eq 1 ]; then
	echo -e "\e[1;32m\n*\n* ENV updating environment for the current shell\n*\e[0m"
	.  /etc/bash_completion.d/cumbia
	.  /etc/bash/bashrc.d/cumbia.sh
	export -f cumbia
fi

if [ $make_install -eq 1 ]; then
	echo -e "\e[0;32m\n*\n* INSTALL \e[1;32myou may need to execute\n*\n  \e[1;36msource  /etc/bash_completion.d/cumbia\e[1;32m && \e[1;36msource  /etc/bash/bashrc.d/cumbia.sh\e[1;32m \n*"
	echo -e "* to enable shortcuts for cumbia apps. Then type \n*\n  \e[1;36mcumbia\e[1;32m\n*\n* to list available options\n*\e[0m"
fi


