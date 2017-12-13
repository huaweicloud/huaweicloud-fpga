#!/bin/sh
#
#-------------------------------------------------------------------------------
#      Copyright 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
# 
#      This program is free software; you can redistribute it and/or modify
#      it under the terms of the Huawei Software License (the "License").
#      A copy of the License is located in the "LICENSE" file accompanying 
#      this file.
# 
#      This program is distributed in the hope that it will be useful,
#      but WITHOUT ANY WARRANTY; without even the implied warranty of
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#      Huawei Software License for more details. 
#-------------------------------------------------------------------------------

function check_simulator () {
    # Get simulator
    if [ $# -lt 1 ] ; then
        echo -e "No simulator selected, using vivado xsim for default."
    elif [ $1 != "vcs" -a $1 != "questa" -a $1 != "vivado" ] ; then
        echo -e "Selected simulator is illegal, using vivado xsim for default."
    fi
}

function get_simulator () {
    # Get simulator
    if [ $# -lt 1 ] ; then
        # echo -e "No simulator selected, using vivado xsim for default."
        echo "vivado"
    elif [ $1 != "vcs" -a $1 != "questa" -a $1 != "vivado" ] ; then
        # echo -e "Selected simulator is illegal, using vivado xsim for default."
        echo "vivado"
    else
        echo $1
    fi
}

function check_valid_cfg () {
    # Get simulator
    if [ $# -lt 1 -o x$1 == "x" ] ; then
        # echo -e "No simulator selected, using vivado xsim for default."
        return 0
    fi
    strcfg=$1
    cfgcoment=${strcfg:0:1}
    if [ $cfgcoment == "#" ] ; then
        return 0;
    elif [ $cfgcoment == "/" ] ; then
        cfgcoment=${strcfg:1:1}
        if [ $cfgcoment == "/" ] ; then
            return 0
        fi
    fi
    return 1;
}

function get_valid_test () {
    # Get testcase dir 
    if [ $# -lt 1 -o x$1 == "x" ] ; then
        return 0
    fi
	declare -a testlist=(`find $1 -type f -name "test.cfg"`)
	testnum=${#testlist[@]}
	if [ $testnum -lt 1 ] ; then
		echo "No Vailid Testcase has been found!"
	else
		echo "$testnum Testcase Exists:"
		echo "---------------------------------------------------------"
		for item in ${testlist[@]} ; do
			testdir=`dirname $item`
			testname=${testdir##*/}
			echo $testname
		done
		echo "---------------------------------------------------------"
	fi
}

