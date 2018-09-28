#!/bin/bash
#
#-------------------------------------------------------------------------------
#      Copyright (c) 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
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

#######################################################################################################################
## get the script path
#######################################################################################################################
if [[ "$0" =~ ^\/.* ]]; then
	script=$0
else
	script=$(pwd)/$0
fi
script=$(readlink -f $script)
script_path=${script%/*}

realpath=$(readlink -f $script_path)

#set environment value
if [ -z $XILINX_SDX ]
then 
    echo -e "Xilinx SDx is not found! Please check it first!\n"
    exit
fi

function Usage
{
	echo "-------------------------------------------------------"
	echo "Usage: compile.sh [option]                              "
	echo "Options:                                               "
	echo "    sh  compile.sh  hw_em      HW Emulation "
	echo "    sh  compile.sh  hw         HW  compile "
	echo "    sh  compile.sh  clean      clean compiled files "
	echo "-------------------------------------------------------"
}

function make_hw_emu
{
	cd ${realpath}/../src
    make clean
	make
    if [ $? -ne 0 ]
    then 
        HW_EM_ISOK=0
    else
        HW_EM_ISOK=1
    fi
	cd $realpath
}

function make_hw
{
	cd ${realpath}/../src
    make clean
	make TARGET=hw
    if [ $? -ne 0 ]
    then 
        HW_COMP_ISOK=0
    else
        HW_COMP_ISOK=1
    fi
    
    if [ -f $realpath/../prj/bin/*.xclbin ];then
	    sh $realpath/../../../lib/scripts/creat_ocl_manifest.sh $realpath
    else
	    echo "ERROR:hardware xclbin not found!"
		cd $realpath
        exit
    fi
        
	cd $realpath
}

function make_clean
{
	cd ${realpath}/../src
	make clean
    if [ $? -ne 0 ]
    then 
        CMP_CLEAN_ISOK=0
    else
        CMP_CLEAN_ISOK=1
    fi
	cd $realpath
}

if [[ $1 == "hw_em" ]]
    then
		make_hw_emu
        if [ $HW_EM_ISOK == 1 ]
        then
            echo "*************************************************"
            echo "****                                     ********"
            echo "****  HW EMULATION COMPILE PASSED        ********"
            echo "****                                     ********"
            echo "*************************************************"
        else 
            echo "*************************************************"
            echo "****                                     ********"
            echo -e "****  HW EMULATION COMPILE\e[1;31m FIALED \e[0m       ********"
            echo "****                                     ********"
            echo "*************************************************"
        fi
elif [[ $1 == "hw" ]]
    then
		make_hw	
        if [ $HW_COMP_ISOK == 1 ]
        then
            echo "*************************************************"
            echo "****                                     ********"
            echo "****   HARDWARE COMPILE PASSED           ********"
            echo "****                                     ********"
            echo "*************************************************"
        else 
            echo "*************************************************"
            echo "****                                     ********"
            echo -e "****   HARDWARE COMPILE\e[1;31m FIALED \e[0m          ********"
            echo "****                                     ********"
            echo "*************************************************"
        fi
elif [[ $1 == "clean" ]]
    then
		make_clean	
        if [ $CMP_CLEAN_ISOK == 1 ]
        then
            echo "*************************************************"
            echo "****                                     ********"
            echo "****   COMPILE CLEAN COMPLETED           ********"
            echo "****                                     ********"
            echo "*************************************************"
        else 
            echo "*************************************************"
            echo "****                                     ********"
            echo -e "****   COMPILE CLEAN\e[1;31m FIALED \e[0m             ********"
            echo "****                                     ********"
            echo "*************************************************"
        fi
else
    while [ "$1" != "" ]; do
        case $1 in
            -h | --help)   Usage
            exit
            ;;
            * )
            echo "error: invalid input '$1'."
	        echo "error: *** No rules to compile."
            echo "Input -h or --help for details!"
            exit
            ;;
        esac
        shift
    done
	echo "error: *** No rules to compile."
    echo "Input -h or --help for details!"
fi
