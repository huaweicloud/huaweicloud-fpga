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
#SDX_SETUPFILE="/software/Xilinx/SDx_2017.1/SDx/2017.1/settings64.sh"
#if [ ! -f $SDX_SETUPFILE ]
if [ -z $XILINX_SDX ]
then 
    echo -e "Xilinx SDx is not found! Please check it first!\n"
    exit
fi

#source $SDX_SETUPFILE 

function make_sw_emu
{
	cd ${realpath}/../src
    make clean
	make cpu_em
    if [ $? -ne 0 ]
    then 
        CPU_EM_ISOK=0
    else
        CPU_EM_ISOK=1
    fi
	cd $realpath
}

function make_hw_emu
{
	cd ${realpath}/../src
    make clean
	make hw_em
    if [ $? -ne 0 ]
    then 
        HW_EM_ISOK=0
    else
        HW_EM_ISOK=1
    fi
	cd $realpath
}

function make_host
{
	cd ${realpath}/../src
    make clean
	make host
    if [ $? -ne 0 ]
    then 
        HOST_COMP_ISOK=0
    else
        HOST_COMP_ISOK=1
    fi
	cd $realpath
}

function make_hw
{
	cd ${realpath}/../src
    make clean
	make hw
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

function Usage
{
	echo "-------------------------------------------------------"
	echo "Usage: compile.sh [option]                                 "
	echo "Options:                                               "
	echo "    sh  compile.sh  cpu_em     CPU Emulation "
	echo "    sh  compile.sh  hw_em      HW Emulation "
	echo "    sh  compile.sh  host       only compile host "
	echo "    sh  compile.sh  hw         HW  compile"
	echo "    sh  compile.sh  clean      clean compiled files "
	echo "-------------------------------------------------------"
}

if [[ $1 == "cpu_em" ]]
    then
		make_sw_emu
        if [ $CPU_EM_ISOK == 1 ]
        then
            echo "*************************************************"
            echo "****                                     ********"
            echo "****  CPU EMULATION COMPILE PASSED       ********"
            echo "****                                     ********"
            echo "*************************************************"
        else 
            echo "*************************************************"
            echo "****                                     ********"
            echo -e "****  CPU EMULATION COMPILE\e[1;31m FIALED \e[0m     ********"
            echo "****                                     ********"
            echo "*************************************************"
        fi
elif [[ $1 == "hw_em" ]]
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
elif [[ $1 == "host" ]]
    then
		make_host
        if [ $HOST_COMP_ISOK == 1 ]
        then
            echo "*************************************************"
            echo "****                                     ********"
            echo "****       HOST COMPILE PASSED           ********"
            echo "****                                     ********"
            echo "*************************************************"
        else 
            echo "*************************************************"
            echo "****                                     ********"
            echo -e "****       HOST COMPILE\e[1;31m FIALED \e[0m          ********"
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
#elif [[ $1 == "--help" ]] || [[ $1 == "-h" ]]
#    then
#		Usage			
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
	#echo "Input err!"
	#echo " *** No arguments to compile, input -h or --help for details!"
fi
