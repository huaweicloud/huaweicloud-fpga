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

#source $SDX_SETUPFILE 

EXE_PATH=${realpath}/../prj/bin/

EXE_MODE=$1
EXE_NAME=$2
XCLBIN=$3

EXE=./${EXE_NAME##*/}
EXE_XCLBIN=./${XCLBIN##*/}


function Usage
{
	echo "------------------------------------------------------------------"
	echo "Usage: run.sh [option]                                 "
	echo "Options:                                               "
	echo "sh run.sh emu TARGET XCLBIN           Running CPU/HW Emulation "
	#echo "sh run.sh hw    TARGET    Running HW "
    echo "-----------------------------example------------------------------"
    echo "sh run.sh emu ../prj/bin/mmult ../prj/bin/bin_mmult_cpu_emu.xclbin"
	echo "------------------------------------------------------------------"
}

if [[ $EXE_MODE == "emu" ]] && [ -n "$EXE_NAME" ]; 
    then
        echo "                                   "
        echo "Emulation test is beginning..."
        echo "                                   "
	    export XCL_EMULATION_MODE=true
		cd $EXE_PATH
		${EXE} ${EXE_XCLBIN}
		cd $realpath	
elif [[ $EXE_MODE == "hw" ]] && [ -n "$EXE_NAME" ];
    then
        export XCL_EMULATION_MODE=false
        echo "                                   "
        echo "HW mode test is NOT SUPPORTED in this folder!"
        echo "Please goto <EXCUTE-FOLDER> for hw mode test!"
else
    while [ "$1" != "" ]; do
        case $1 in
            -h | --help)   Usage
            exit
            ;;
            * )
            echo "error: invalid input '$1'."
	        echo "error: *** No rules to run."
            echo "Input -h or --help for details!"
            exit
            ;;
        esac
        shift
    done
	echo "error: *** No rules to run."
    echo "Input -h or --help for details!"
fi

