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

if [ -z $XILINX_SDX ]
then 
    echo -e "Xilinx SDx is not found! Please check it first!\n"
    exit
fi

XILINX_SDX_PATH=${XILINX_SDX}
unset XILINX_SDX

export LD_LIBRARY_PATH=${XILINX_SDX_PATH}/runtime/lib/x86_64/:${XILINX_SDX_PATH}/lib/lnx64.o/
export XILINX_OPENCL=$(pwd)/../../../userspace/sdaccel/lib/
export XCL_PLATFORM=hal

DRIVER=xdma
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
	echo "Usage: run.sh [option]                                 "
	echo "Options:                                               "
	echo "sh run.sh HOSTEXE TARGET_XCLBIN           Running CPU/HW Emulation "
    echo "-----------------------------example------------------------------"
    echo "sh run.sh mmult /home/fp1/hardware/sdaccel_design/examples/mmult_hls/prj/bin/bin_mmult_hw.xclbin"
    echo "                               or                                 "
    echo "sh run.sh mmult ./bin_mmult_hw.xclbin"
	echo "------------------------------------------------------------------"
}

if [ "$1" == "" -o "$1" == "-h" -o "$1" == "--help" ];then
    Usage
else
    cd ${HOSTEXE_PATH}
    ${HOSTEXE_NAME} ${KERNEL_NAME}
fi
