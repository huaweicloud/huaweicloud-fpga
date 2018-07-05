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



#Copyright(c) 2017, Huawei Technology Inc, All right reserved Department
HOSTEXE=$1
HOSTEXE_PATH_JDG=`echo "${HOSTEXE}" |egrep "/"`
if [ -z ${HOSTEXE_PATH_JDG} ]
then
    HOSTEXE_PATH=./
else
    HOSTEXE_PATH=${HOSTEXE%/*}
fi

HOSTEXE_NAME=./${HOSTEXE##*/}
KERNEL_NAME=$2
SLOT_ID=$3

function var_chk
{
    if [ -z ${HOSTEXE} ]
    then 
        echo -e "host app is not found! Please check it first!\n"
        exit
    fi
    
    VALID_KERNEL_NAME=`echo ${KERNEL_NAME} |grep .xclbin`
    if [ -z ${KERNEL_NAME} ]
    then 
        echo -e "Error£ºkernel is not found, please check!\n"
        exit
    elif [ "${VALID_KERNEL_NAME}" = "" ] 
    then
    	echo -e "Error£ºkernel is not a xclbin file, please check!\n"
        exit
    fi
    
    VALID_SLOT_ID=`echo ${SLOT_ID}| sed -n "/^[0-9]\+$/p"`
    if [ -z ${SLOT_ID} ]
    then 
        echo -e "Error£ºslot id is not found, please check!\n"
        exit
    elif [ "${VALID_SLOT_ID}" = "" ]
    then
        echo -e "Error£ºslot id is not a number, please check!\n"
        exit
    fi
}

if [ -z $XILINX_SDX ]
then 
    echo -e "Xilinx SDx is not found! Please check it first!\n"
    exit
fi

XILINX_SDX_PATH=${XILINX_SDX}

unset XILINX_SDX
unset XILINX_SDACCEL
unset XCL_EMULATION_MODE

export LD_LIBRARY_PATH=${XILINX_SDX_PATH}/runtime/lib/x86_64:${XILINX_SDX_PATH}/lib/lnx64.o/Default:${XILINX_SDX_PATH}/lib/lnx64.o
export XILINX_OPENCL=$(pwd)/../../../userspace/sdaccel/lib
#export PATH=${XILINX_SDX_PATH}/runtime/bin
#export XCL_PLATFORM=hal
#echo "LD lib path is ${LD_LIBRARY_PATH}"

DRIVER=xocl
check_driver()
{
    MODULE_INFO=`lsmod | grep $DRIVER 2>&1`
    if [ $? != 0 ]; then
       echo "ERROR: the $DRIVER driver is not exist, please install first."
	   exit 1
    fi
}

check_driver

function Usage
{
	echo "------------------------------------------------------------------"
	echo "Usage: run.sh [option]                                            "
	echo "Options:                                                          "
	echo "sh run.sh HOSTEXE XCLBIN SLOT_ID                 Running HW Test  "
    echo "-----------------------------examples-----------------------------"
	echo "running mmult on card 0:                                          "
    echo "sh run.sh mmult /home/fp1/hardware/sdaccel_design/examples/mmult_hls/prj/bin/bin_mmult_hw.xclbin 0"
    echo "running mmult on card 1:                                          "
    echo "sh run.sh mmult ./bin_mmult_hw.xclbin 1                           "
	echo "------------------------------------------------------------------"
}

if [ "$1" == "" -o "$1" == "-h" -o "$1" == "--help" ];then
    Usage
else
    var_chk
    cd ${HOSTEXE_PATH}
    ${HOSTEXE_NAME} ${KERNEL_NAME} ${SLOT_ID}
fi
