#!/bin/bash
#
#-------------------------------------------------------------------------------
#Copyright (c) 2017 Huawei Technologies Co., Ltd. All Rights Reserved.
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

if [[ "$0" =~ ^\/.* ]]; then
	script=$0
else
	script=$(pwd)/$0
fi
script=$(readlink -f $script)
script_path=${script%/*}

realpath=$(readlink -f $script_path)

echo $realpath
FPGADESIGN_PATH=${realpath}/../../..
XDMADRV_PATH=${FPGADESIGN_PATH}/software/kernel_drivers/xdma_driver/driver/xclng/xdma
USERHAL_PATH=${FPGADESIGN_PATH}/software/userspace/sdaccel/driver/xclhal/source
USERHALLIB_PATH=${USERHAL_PATH}/../../../lib/runtime/platforms/hal/driver
echo "dma drv  path is ${DMADRV_PATH}"
if [ -z $XILINX_SDX ]
then 
    echo -e "Xilinx SDx is not found! Please check it first!\n"
    exit
fi

#check xdma driver is installed or not
DRIVER=xdma
check_driver()
{
    MODULE_INFO=`lsmod | grep $DRIVER 2>&1`
    if [ $? != 0 ]; then
       echo "Warning: $DRIVER driver is not exist, will install first."
    else
       rmmod xdma.ko
       echo "$DRIVER driver is uninstalled!"
    fi
}
check_driver

echo -e "\nCompile and installing...\n"

cd ${XDMADRV_PATH}
make clean

echo -e "\nXdma driver is compling..."
XDMA_COMLOG=`make |egrep -w 'err|ERR|error|ERROR'`
if [[ -n "$XDMA_COMLOG" ]]; then
    echo "ERROR: xdma driver compiled error, please check first!"
    exit 1
fi

insmod xdma.ko
XDMADRV_LS=`lsmod | grep "$DRIVER"`
if [[ -z "$XDMADRV_LS" ]]; then
    echo "ERROR: $DRIVER driver is not exist, will install first!"
    exit 1
fi

make clean

rm -rf ${USERHALLIB_PATH}/*
cd ${USERHAL_PATH} 
make clean

echo -e "\nHAL is compling..."
HAL_COMLOG=`make |egrep -w 'err|ERR|error|ERROR'`
if [[ -n "$HAL_COMLOG" ]]; then
    echo "ERROR: hal compiled error, please check first!"
    exit 1
fi
make clean

cd ${realpath}

echo "*************************************************"
echo "Compile and install driver and HAL lib completed!"
echo "*************************************************"
