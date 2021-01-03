#!/bin/bash

CURUN_PY=./curun-db.py

CONF_FILE=$HOME/.config/cumbia/cumbia-build/options.sh

UPDATE=0

declare -a args=( )
declare -a exports=()
declare -a tests=()

function get_args {
## declare an array variable
    declare -a arr=("RUN" "RUN.txt" "run" "run.txt" "README" "README.txt" "README.md")
    ## now loop through the above array
    for i in "${arr[@]}"
    do
        input="$1/$i"
        if [ -r $input ]; then
            echo "looking for command line args in $input..."
            while IFS= read -r line
            do
            if [[ $line =~ ^args\s+* ]]; then
                argus=${line#"args "}CURUN_PY
                args+=("$argus")
            elif [[ $line =~ ^export\+* ]]; then
                exports+=("$line")
            elif [[ $line =~  ^\[test\]\s+* ]]; then
                test=${line#"[test] "}
                tests+=("$test")
            fi
             
            done < "$input"
        fi
    
    # or do whatever with individual element of the array
    done
}

if test -f "$CONF_FILE"; then
    . $CONF_FILE
else
    echo "First time you run cumbia run? Execute cumbia build setup"
fi

if [ -z "$1" ]; then
    projnam=`basename $PWD`
    if [ ! -d bin ] || [ ! -x bin/$projnam ] || [ ! -x bin/$projnam-gui ]; then
        echo -e "\e[1;31m-\e[0m Could not find a binary for the project $projnam under $PWD."
        echo -e "\e[1;31m-\e[0m Type \e[4mcumbia build $projnam\e[0m"
        exit 1
    fi
elif [[ $1 == *"/"* ]]; then
	eval $@
	if [ $? -eq 0 ]; then
		args="${@:2}"
		p=$1
		echo "command $@ successful path $p args $args"
		# curun-db.py add "path/to/app"  "arg1 arg2 ... argN" "VARIABLE1=val1"
		$CURUN_PY add -c "$p" -a "$args"

	fi
   
elif [ -d "$BUILD_PATH" ]; then
    savedir=$PWD
    REPO=$REPO_ROOT/$1
    DESTDIR=$BUILD_PATH/$1
    BINDIR=$DESTDIR/bin
    if [ ! -d $DESTDIR ]; then
        echo -e "\e[1.33m-\e[0m directory $DESTDIR does not exist. I'll try to build the project $1"
        cumbia-build.sh $1
    elif [ $UPDATE -ne 0 ]; then
        cumbia-build.sh $1
    fi
    if [ ! -d $BINDIR ]; then
            echo -e "\e[1;31m-\e[0m Could not find a binary for the project $projnam under $BINDIR."
            cumbia-build.sh $1
    fi
    
    CMD=""
    
    if [ -x $BINDIR/$1-gui ]; then 
        CMD=$BINDIR/$1-gui
    elif [ -x $BINDIR/$1 ]; then 
        CMD=$BINDIR/$1
    fi
    
    # args ?
    
## declare an array variable
    get_args $DESTDIR
        
    for i in "${exports[@]}"
        do
        ${exports[*]}
    done
    
    c=0
    declare -a options=()
    
    for i in "${args[@]}"
        do
        c=$((c+1))
        options+=("$CMD ${args[*]}")
        echo "$c. $CMD ${args[*]}"
    done
    
      
    for i in "${tests[@]}"
        do
        c=$((c+1))
        t="${tests[*]}"
        t="${t/"[target]"/$CMD}" 
        options+=("$t")
        echo "$c. $t"
    done
    
    len=${#options[@]}
    if [ $len -gt 1 ]; then
        echo -e -n "Which command would you like to execute [1-$len]? "
        read choice
    
        echo "choice is $choice"
        if [[ $choice -ge 1 ]] && [[ $choice -le $len ]]; then
            echo "will execute $choice: --> ${options[$choice-1]}"
            
            # launch the program
            eval ${options[$choice-1]}
        else
            echo -e "\e[1.31m-\e[0m invalid choice\n"
        fi
    elif [ $len -eq 1 ]; then
        eval ${options[0]}
    fi
    
    
    
else
    echo -e "\e[1;31m:-(\e[0m the build path specified in the configuration does not exist"
fi

